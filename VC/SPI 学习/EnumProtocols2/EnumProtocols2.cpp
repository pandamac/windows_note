///////////////////////////////////////////////////////////
// WSCEnumProtocols.cpp文件

#include "StdAfx.h"
#include <locale.h>
#include <Ws2spi.h>		//  SPI函数定义在Ws2spi.h文件中
#include <windows.h>
#include <stdio.h>
#pragma comment(lib, "WS2_32")	// 链接到WS2_32.lib

void main()
{
	int nProtocols;
	int nError;
	DWORD dwSize = 0;
	LPWSAPROTOCOL_INFOW pProtoInfo = NULL;
	//  首次调用，pProtoInfo传入NULL，取得需要的缓冲区长度
	if(WSCEnumProtocols(NULL, pProtoInfo, &dwSize, &nError) == SOCKET_ERROR)
	{
		if(nError != WSAENOBUFS)
		{
			return ;
		}
	}
	// 申请足够缓冲区内存。
	pProtoInfo = (LPWSAPROTOCOL_INFOW)::GlobalAlloc(GPTR, dwSize);
	if (pProtoInfo == NULL)
	{
		return ;
	}
	//再次调用WSCEnumProtocols函数
	nProtocols = ::WSCEnumProtocols(NULL, pProtoInfo, &dwSize, &nError);
	wchar_t szWPath[MAX_PATH] = {0};
	int iLen = MAX_PATH;
	//使控制台支持中文编码////////////////////////////////////////////////////////////////////
	setlocale(LC_ALL, "chs");
	wprintf(L"%-10s%-25s%s\n\n", L"提供者ID", L"协议类型", L"DLL路径");
	wchar_t *szWType[3] = {L"分层协议", L"基础协议", L"协议链"};
	int iIndex = 0;
	for(int i=0; i<nProtocols; i++)
	{	//打印传输服务提供者ID
		wprintf(L"%-13d", pProtoInfo[i].dwCatalogEntryId);
		//打印传输服务提供者协议类型
		iIndex = (pProtoInfo[i].ProtocolChain.ChainLen>1?2:pProtoInfo[i].ProtocolChain.ChainLen);
		wprintf(L"%-15s", szWType[iIndex]);
		//打印传输服务提供者所对应DLL的路径
		WSCGetProviderPath(&pProtoInfo[i].ProviderId, szWPath, &iLen, NULL);
		wprintf(L"%-15s\n", szWPath);
	}
	wprintf(L"共计 %d 个传输服务提供者\n", nProtocols);
	//释放内存
	GlobalFree(pProtoInfo);
	//恢复控制台编码//////////////////////////////////////////////////////////////////////////
	setlocale(LC_ALL, NULL);
}
