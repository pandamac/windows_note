// GetFuncAddr.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>

int main(int argc, char* argv[])
{
	PBYTE pKernelBaseMem = (PBYTE)0x7c800000; //Kernel32.dll的基址通常为0x7c800000。
	//pKernelBaseMem是Kernel32.dll模块的基址
	IMAGE_NT_HEADERS *pPeHeader = (IMAGE_NT_HEADERS *)(pKernelBaseMem + ((IMAGE_DOS_HEADER *)pKernelBaseMem)->e_lfanew);
	//得到Kernel32.dll模块的导出表
	IMAGE_EXPORT_DIRECTORY *pExportDescriptor =(IMAGE_EXPORT_DIRECTORY *)(pKernelBaseMem + pPeHeader->OptionalHeader.DataDirectory[0].VirtualAddress);
	//按照名称导出的函数个数
	int nNamesNum = pExportDescriptor->NumberOfNames;
	//分别获得导出函数名称序号表、导出函数地址表、导出函数名称地址表。
	WORD * pNameOrdinalsTable = (WORD *)(pKernelBaseMem+pExportDescriptor-> AddressOfNameOrdinals);
	DWORD * pAddressOfName = (DWORD *)(pKernelBaseMem+pExportDescriptor-> AddressOfNames);
	DWORD * pAddressOfFunctions = (DWORD *)(pKernelBaseMem+pExportDescriptor-> AddressOfFunctions);
	//声明用于保存两个函数地址的变量
	DWORD dwLoadLibraryAddress = 0, dwGetProcAddress = 0;
	BOOL bLoadLibrary = TRUE, bGetProcAddress = TRUE; 
	DWORD dwOrdinale = 0; //用于保存导出序号表中对应的序号
	char *pLoadLibrary = "LoadLibraryA", *pGetProcAddress = "GetProcAddress";
	for (int i=0; i<nNamesNum&&(bLoadLibrary || bGetProcAddress); i++)
	{
		char *pFuncName = (char *)pKernelBaseMem+pAddressOfName[i];
		int j = 0;
		while (bLoadLibrary)
		{
			if (pFuncName[j] != pLoadLibrary [j] || pLoadLibrary [j] == 0)
			{	//如果因为pLoadLibrary [j] == 0而退出循环则找到LoadLibraryA函数
				break;
			}
			j++;
		}
		if (bLoadLibrary && pLoadLibrary [j] == 0)
		{
			dwOrdinale = pNameOrdinalsTable[i];
			dwLoadLibraryAddress = pAddressOfFunctions[dwOrdinale];
			bLoadLibrary = FALSE;
		}
		j=0;
		while (bGetProcAddress)
		{
			if (pFuncName[j] != pGetProcAddress [j] || pGetProcAddress [j] == 0)
			{//如果因为pGetProcAddress [j] == 0而退出循环则找到GetProcAddress函数
				break;
			}
			j++;
		}
		if (bGetProcAddress && pGetProcAddress [j] == 0)
		{
			dwOrdinale = pNameOrdinalsTable[i];
			dwGetProcAddress = pAddressOfFunctions[dwOrdinale];
			bGetProcAddress = FALSE;
		}
	}
	printf("LoadLibraryA的地址：  0x%X\n", pKernelBaseMem+dwLoadLibraryAddress);
	printf("GetProcAddress的地址：0x%X", pKernelBaseMem+dwGetProcAddress);
	return 0;
}

