// EnumProtocols.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <winsock2.h>
#pragma comment(lib, "WS2_32")	// 链接到WS2_32.lib
#include <windows.h>
#include <stdio.h>

LPWSAPROTOCOL_INFO GetProvider(LPINT lpnTotalProtocols)
{
	DWORD dwSize = 0;
	LPWSAPROTOCOL_INFO pProtoInfo = NULL;
	
	// 取得需要的缓冲区长度
	if(WSAEnumProtocols(NULL, pProtoInfo, &dwSize) == SOCKET_ERROR)
	{
		if(WSAGetLastError() != WSAENOBUFS) 
			return NULL;
	}
	// 申请缓冲区，再次调用WSAEnumProtocols函数
	pProtoInfo = (LPWSAPROTOCOL_INFO)GlobalAlloc(GPTR, dwSize);
	*lpnTotalProtocols = WSAEnumProtocols(NULL, pProtoInfo, &dwSize);
	return pProtoInfo;
}

void FreeProvider(LPWSAPROTOCOL_INFO pProtoInfo)
{
	::GlobalFree(pProtoInfo);
}

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 2);
	if(WSAStartup(sockVersion, &wsaData) != 0)
	{
		return -1;
	}
	int nTotalProtocols;
	LPWSAPROTOCOL_INFO pProtoInfo = GetProvider(&nTotalProtocols);
	if(pProtoInfo != NULL)
	{
		// 打印出各个提供者的协议信息
		for(int i=0; i<nTotalProtocols; i++)
		{
			printf(" Protocol: %s \n", pProtoInfo[i].szProtocol);
			printf(" CatalogEntryId: %d		ChainLen: %d \n\n", 
				pProtoInfo[i].dwCatalogEntryId, pProtoInfo[i].ProtocolChain.ChainLen);
		}
		FreeProvider(pProtoInfo);
	}
	WSACleanup();	
	return 0;
}

