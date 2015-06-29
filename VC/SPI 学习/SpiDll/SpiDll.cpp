// SpiDll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <Ws2spi.h>
#pragma comment(lib, "WS2_32")	// 链接到WS2_32.lib

// 声明要使用UNICODE字符串
#define UNICODE
#define _UNICODE

WSPPROC_TABLE g_NextProcTable;//保存下层提供者填充的SPI地址表
wchar_t	g_szCurrentApp[MAX_PATH];	// 当前调用本DLL的程序的名称
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

int WSPAPI WSPSendTo(
					 SOCKET			s,
					 LPWSABUF		lpBuffers,
					 DWORD			dwBufferCount,
					 LPDWORD			lpNumberOfBytesSent,
					 DWORD			dwFlags,
					 const struct sockaddr FAR * lpTo,
					 int				iTolen,
					 LPWSAOVERLAPPED	lpOverlapped,
					 LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
					 LPWSATHREADID	lpThreadId,
					 LPINT			lpErrno
					 )
{	
	// 拒绝所有目的端口为6688的UDP封包
	SOCKADDR_IN sa = *(SOCKADDR_IN*)lpTo;
	if(sa.sin_port == htons(6688))
	{
		return SOCKET_ERROR;
	}
	return g_NextProcTable.lpWSPSendTo(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpTo
		, iTolen, lpOverlapped, lpCompletionRoutine, lpThreadId, lpErrno);
}

int WSPAPI WSPStartup(WORD wVersionRequested,
					  LPWSPDATA lpWSPData,
					  LPWSAPROTOCOL_INFOW lpProtocolInfo,
					  WSPUPCALLTABLE UpcallTable,
					  LPWSPPROC_TABLE lpProcTable)
{
	//我们编写的DLL用于协议链中，所以如果是基础协议或分层协议使用则直接返回错误
	if(lpProtocolInfo->ProtocolChain.ChainLen <= 1)
	{	
		return WSAEPROVIDERFAILEDINIT;
	}
	//如果有应用程序联网，进行拦截，并询问是否允许通过
	wchar_t szName[512] = {0};
	wsprintfW(szName, L"应用程序：%s 正在联网，被拦截，是否允许？", g_szCurrentApp);
	if (MessageBoxW(0, szName, L"系统提示", MB_YESNO|MB_ICONWARNING) == IDNO)
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
	//保存修改前的SPI函数地址表以便我们的函数使用
	g_NextProcTable = *lpProcTable;
	// 修改SPI函数地址表，Hook感兴趣的函数，这里做为示例，仅Hook了WSPSendTo函数
	// 您还可以Hook其它函数，如WSPSocket、WSPCloseSocket、WSPConnect等
 //	lpProcTable->lpWSPSendTo = WSPSendTo;
	GlobalFree(pProtoInfo);
	return nRet;
}


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			// 取得主模块的名称
			GetModuleFileNameW(NULL, g_szCurrentApp, MAX_PATH);
		}
		break;
	}
	return TRUE;
}

