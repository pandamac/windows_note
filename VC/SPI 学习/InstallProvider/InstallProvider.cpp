// InstallProvider.cpp : Defines the entry point for the console application.
//

#define UNICODE
#define _UNICODE

#include "stdafx.h"
#include <RPC.H>
#include <Rpcdce.h>
#include <Ws2spi.h>
#include <Sporder.h>		// 定义了WSCWriteProviderOrder函数
#include <windows.h>
#include <stdio.h>

#pragma comment(lib, "Sporder.lib")	// 实现了UuidCreate函数
#pragma comment(lib, "Rpcrt4.lib")	// 实现了UuidCreate函数
#pragma comment(lib, "WS2_32")	// 链接到WS2_32.lib


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
#define Safe_Delete(pPointer) if(pPointer != NULL) {delete []pPointer; pPointer=NULL;}
#define Safe_New(pPointer, Type, Size)  pPointer = new Type[Size]; if (pPointer == NULL) goto Exit;

BOOL InstallProvider(WCHAR * pProviderName, WCHAR * pwszPathName, int *pProtocols, int iNum)
{
	if (pwszPathName == NULL || pProviderName == NULL)
	{
		return FALSE;
	}
	LPWSAPROTOCOL_INFOW pProtoInfo = NULL;
	//保存要安装的协议类型
	int nIndex = 0, nProtocols, nError, i=0, nArrayCount = 0, *pTemp = NULL;
	DWORD dwSize = 0, *pdwIds = NULL,*pdwOrigCatalogId = NULL;
	WSAPROTOCOL_INFOW *pOriginalProtocolInfo = NULL;
	BOOL bRet = FALSE;
	Safe_New(pTemp, int, iNum)
	//备份一份要安装的协议于pTemp指向的内存中
	memcpy(pTemp, pProtocols, iNum*sizeof(int));
	//为每个协议链准备WSAPROTOCOL_INFOW结构体
	Safe_New(pOriginalProtocolInfo, WSAPROTOCOL_INFOW, iNum)
	//pdwOrigCatalogId指向的数组成员用于保存分层协议下层的基础协议目录ID
	Safe_New(pdwOrigCatalogId, DWORD, iNum)
	DWORD dwLayeredCatalogId;// 我们分层协议的目录ID号
	// 枚举所有服务程序提供者
	nProtocols = GetProvider(pProtoInfo);
	//将所有要安装新提供者的协议在Winsock目录中已存在的位于前端的提供者的WSAPROTOCOL_INFOW保存起来
	for(; i<nProtocols; i++) 	//遍历Winsock目录中所有提供者
	{   
		for (int j=0; j<iNum; j++)
		{	//有iNum个协议要安装新提供者。
			//判断当前提供者对应的协议是否是要安装新提供者的协议
			if(pProtoInfo[i].iAddressFamily == AF_INET && pProtoInfo[i].iProtocol == pTemp[j])
			{
				pTemp[j] = -1; //清除此类型，确保不要查找到重复协议的提供者，只要最前端的。
				//保存该协议正在使用的提供者的信息
				memcpy(&pOriginalProtocolInfo[nArrayCount], &pProtoInfo[i], sizeof(WSAPROTOCOL_INFOW));
				//分层协议中应该去除XP1_IFS_HANDLES标志。
				pOriginalProtocolInfo[nArrayCount].dwServiceFlags1 = 
					pOriginalProtocolInfo[nArrayCount].dwServiceFlags1 & (~XP1_IFS_HANDLES); 
				//将该协议正在使用的提供者的目录ID保存起来。
				pdwOrigCatalogId[nArrayCount] = pProtoInfo[i].dwCatalogEntryId;
				nArrayCount++;
				break; //找到后不必再循环查找其他的
			}
		}
	}  
	// 安装我们的分层协议，获取一个目录ID。
	// 随便找一个下层协议的结构复制过来即可
	WSAPROTOCOL_INFOW LayeredProtocolInfo;
	memcpy(&LayeredProtocolInfo, &pOriginalProtocolInfo[0], sizeof(WSAPROTOCOL_INFOW));
	// 修改协议名称，类型，设置PFL_HIDDEN标志
	wcscpy(LayeredProtocolInfo.szProtocol, pProviderName);
	LayeredProtocolInfo.ProtocolChain.ChainLen = LAYERED_PROTOCOL; // 设置分层协议标志;
	LayeredProtocolInfo.dwProviderFlags |= PFL_HIDDEN;
	
	// 获取一个Guid，安装分层协议
	GUID ProviderLayeredGuid;
	if(UuidCreate(&ProviderLayeredGuid) == RPC_S_OK)
	{
		if(WSCInstallProvider(&ProviderLayeredGuid, pwszPathName, &LayeredProtocolInfo, 1, &nError) == SOCKET_ERROR)
		{
			goto Exit;
		}
	}
	// 重新枚举协议，获取分层协议的目录ID号
	GlobalFree(pProtoInfo);
	nProtocols = GetProvider(pProtoInfo);
	for(i=0; i<nProtocols; i++)
	{	//查找新安装的分层协议提供者的ID并保存。
		if(memcmp(&pProtoInfo[i].ProviderId, &ProviderLayeredGuid, sizeof(ProviderLayeredGuid)) == 0)
		{
			dwLayeredCatalogId = pProtoInfo[i].dwCatalogEntryId;
			break;
		}
	}
	// 安装协议链，多个协议链共用一个分层协议。有多少个协议要安装新提供者就要安装多少个协议链
	// 修改协议名称，类型
	WCHAR wszChainName[WSAPROTOCOL_LEN + 1];
	for(i=0; i<iNum; i++)
	{
		swprintf(wszChainName, L"%ws over %ws", pProviderName, pOriginalProtocolInfo[i].szProtocol);
		wcscpy(pOriginalProtocolInfo[i].szProtocol, wszChainName);
		if(pOriginalProtocolInfo[i].ProtocolChain.ChainLen == 1)
		{	//当前协议正在使用的提供者是基础提供者,令该提供者的ID位于新安装的协议链中第二个位置
			pOriginalProtocolInfo[i].ProtocolChain.ChainEntries[1] = pdwOrigCatalogId[i];
		}
		else
		{
			//当前协议正在使用的提供者是协议链提供者,则令该协议链整体后移,协议链首位置放分层协议
			for(int j = pOriginalProtocolInfo[i].ProtocolChain.ChainLen; j>0; j--)
			{
				pOriginalProtocolInfo[i].ProtocolChain.ChainEntries[j] 
				= pOriginalProtocolInfo[i].ProtocolChain.ChainEntries[j-1];
			}
		}
		//将刚安装的分层协议放置到协议链的链首.
		pOriginalProtocolInfo[i].ProtocolChain.ChainEntries[0] = dwLayeredCatalogId;
		//协议链中新安装了一个分层协议,所以使协议链中所链的协议数量加1。
		pOriginalProtocolInfo[i].ProtocolChain.ChainLen++;	
	}
	// 获取一个Guid，安装协议链
	GUID ProviderChainGuid;
	if(UuidCreate(&ProviderChainGuid) == RPC_S_OK)
	{
		if(WSCInstallProvider(&ProviderChainGuid,
			pwszPathName, pOriginalProtocolInfo, iNum, &nError) == SOCKET_ERROR)
		{
			goto Exit;	
		}
	}
	else
	{
		goto Exit;
	}
	// 重新排序Winsock目录，将我们的协议链提前
	// 重新枚举安装的协议
	GlobalFree(pProtoInfo);
	nProtocols = GetProvider(pProtoInfo);
	Safe_New(pdwIds, DWORD,nProtocols)
	// 将我们新安装的协议链提供者ID放置到ID数组前端
	for(i=0; i<nProtocols; i++)
	{
		if((pProtoInfo[i].ProtocolChain.ChainLen > 1) &&
			(pProtoInfo[i].ProtocolChain.ChainEntries[0] == dwLayeredCatalogId))
		{	//我们新安装的所有协议链的链首都是分层协议,据此查找新安装的协议链
			pdwIds[nIndex++] = pProtoInfo[i].dwCatalogEntryId;
		}
	}
	// 添加其它协议提供者ID到ID数组中
	for(i=0; i<nProtocols; i++)
	{
		if((pProtoInfo[i].ProtocolChain.ChainLen <= 1) ||
			(pProtoInfo[i].ProtocolChain.ChainEntries[0] != dwLayeredCatalogId))
			pdwIds[nIndex++] = pProtoInfo[i].dwCatalogEntryId;
	}
	// 根据ID数组中顺序重新排序Winsock目录
	if((nError = ::WSCWriteProviderOrder(pdwIds, nIndex)) != ERROR_SUCCESS)
	{
		goto Exit;
	}
	bRet = TRUE;   //到这里安装成功
Exit:
	Safe_Delete(pdwIds)
	Safe_Delete(pOriginalProtocolInfo)
	Safe_Delete(pTemp)
	Safe_Delete(pdwOrigCatalogId)
	if (pProtoInfo != NULL)
	{
		GlobalFree(pProtoInfo);
		pProtoInfo = NULL;
	}
	return bRet;
}

BOOL UnInstall(GUID guidFilter)
{
	BOOL bRet = FALSE;
	LPWSAPROTOCOL_INFOW pProtoInfo = NULL;
	DWORD dwLayeredCatalogId;
	// 根据Guid取得分层协议的目录ID号
	int nProtocols = GetProvider(pProtoInfo);
	int nError, i=0;
	for(; i<nProtocols; i++)
	{
		if(memcmp(&guidFilter, &pProtoInfo[i].ProviderId, sizeof(guidFilter)) == 0)
		{
			dwLayeredCatalogId = pProtoInfo[i].dwCatalogEntryId;
			break;
		}
	}
	if(i < nProtocols)
	{
		// 移除协议链
		for(i=0; i<nProtocols; i++)
		{
			if((pProtoInfo[i].ProtocolChain.ChainLen > 1) &&
				(pProtoInfo[i].ProtocolChain.ChainEntries[0] == dwLayeredCatalogId))
			{
				bRet = WSCDeinstallProvider(&pProtoInfo[i].ProviderId, &nError);
			}
		}
		// 移除分层协议
		bRet = WSCDeinstallProvider(&guidFilter, &nError);
	}
	return bRet;
}

int main(int argc, char* argv[])
{
	int iArray[3] = {IPPROTO_UDP, IPPROTO_TCP, IPPROTO_IP};
	InstallProvider(L"SpiDll2", L"E:\\SpiDll2.dll", iArray, 3);
	return 0;
}

