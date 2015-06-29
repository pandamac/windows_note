#include <WINDOWS.H>
#include "AddExport.h"

#define NFIND_ORD 0x0;
#define FIND_ORD 0x1;

/*
参数：
lpFile指向内存映射文件
dwTransitNameLen为中转DLL名字的长度
功能：通过传入的lpFile计算DLL文件导出表所需要的所有字符长度，包括需要的中转函数字符数目
返回值：返回NULL为出错，否则返回以文件对齐的需要新增的节大小（0x200对齐）
*/

DWORD CalExportLength(LPVOID lpFile,DWORD dwTransitNameLen)
{

	PIMAGE_DOS_HEADER lpDosHead;
	PIMAGE_NT_HEADERS32 lpNTHead;
	PIMAGE_OPTIONAL_HEADER32 lpOptionHead;
	PIMAGE_DATA_DIRECTORY lpDataDir;
	PIMAGE_EXPORT_DIRECTORY lpExport;
	DWORD lpName;
	DWORD dwNameLen;
	DWORD dwAddrLen;	//函数入口地址RVA+函数名地址RVA+函数索引总长度
	DWORD *lpNameRva;
	DWORD dwNameStringLen;
	DWORD dwTemp;
	WORD *lpOrdinals;
	int i;
	int n;
	int m;
	int f = NFIND_ORD;	//是否找到函数序号的标志

	lpDosHead = (PIMAGE_DOS_HEADER)lpFile;
	lpNTHead = (PIMAGE_NT_HEADERS32)((byte *)lpDosHead+lpDosHead->e_lfanew);
	lpOptionHead = (PIMAGE_OPTIONAL_HEADER32)((byte *)lpNTHead+
					sizeof (DWORD)+
					sizeof (IMAGE_FILE_HEADER));
	//得到数据目录的第一个成员地址，也就是导出表目录
	lpDataDir = (PIMAGE_DATA_DIRECTORY)(&lpOptionHead->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]);
	//得到导出表在内存中的地址
	lpExport = (PIMAGE_EXPORT_DIRECTORY)RvaToOffset(lpDosHead,lpDataDir->VirtualAddress);
	//开始计算整个导出表所需要用到的内存长度，包括添加的中转输出表的内存长度
	lpName = (DWORD)RvaToOffset(lpDosHead,lpExport->Name);
	dwNameLen = lstrlenA(lpName);
	dwNameLen++;
	dwAddrLen = ((lpExport->NumberOfFunctions)*sizeof(DWORD)+	//存放函数入口地址所需要的总长度
				(lpExport->NumberOfNames)*sizeof(DWORD)+		//存放函数名地址所需要的总长度
				(lpExport->NumberOfNames)*sizeof(WORD));		//存放函数索引所需要的总长度
	//接下来计算函数名字符串所需要的总长度+中转描述字符串长度

	dwNameStringLen = 0;
	dwTemp = 0;
	
	lpNameRva = (DWORD *)RvaToOffset(lpDosHead,lpExport->AddressOfNames);

	for (n = lpExport->NumberOfNames,i = 0;n != 0;n--,i++)
	{
		dwTemp = lstrlenA((DWORD)RvaToOffset(lpDosHead,lpNameRva[i]));
		dwTemp++;	//每个字符串以0结尾，故再加1
		dwTemp *= 2;	//中转描述需要相同大小的名字字符串
		//dwTemp += sizeof TEXT("Bommon.");	//中转描述需要在每个字符串前面加上DLL名字
		dwTemp += dwTransitNameLen;
		dwTemp++;	//加上.的长度
		dwNameStringLen += dwTemp;
	}
	//计算只能以序号进行导出的函数的中转描述字符串长度，DLLName.#19
	n = lpExport->NumberOfFunctions;
	n -= lpExport->NumberOfNames;
	lpOrdinals = (WORD *)RvaToOffset(lpDosHead,lpExport->AddressOfNameOrdinals);

	if (!n)
	{
		dwNameStringLen += dwAddrLen;
		dwNameStringLen += dwNameLen;
		dwNameStringLen += 0x1FF;
		dwNameStringLen &= 0xFFFFFE00;
		return dwNameStringLen;
	}

	for (i=0;n!=0;i++)
	{
		for (m=0;m < lpExport->NumberOfNames;m++)
		{
			if (i==lpOrdinals[m])
			{
				f = FIND_ORD;
				break;
			}
		}
		if (!f)
		{
			//dwNameStringLen += sizeof TEXT("Bommon.#");
			dwNameStringLen += (dwTransitNameLen+2);
			dwNameStringLen += sizeof (DWORD64);
			dwNameStringLen++;
			n--;
		}
		f = NFIND_ORD;
	}
	dwNameStringLen += dwAddrLen;
	dwNameStringLen += dwNameLen;
	dwNameStringLen += 0x1FF;
	dwNameStringLen &= 0xFFFFFE00;
	return dwNameStringLen;

}
