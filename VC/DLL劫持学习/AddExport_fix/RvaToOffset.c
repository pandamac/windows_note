#include <windows.h>
#include "AddExport.h"

/*
参数：lpFile指向PE文件内存映射头部，Rva指向虚拟地址
功能：通过虚拟地址循环判断该虚拟地址落在哪个section中，并返回Rva对应的由lpFile指向的PE内存映射地址
返回值：返回NULL为出错
*/

VOID * WINAPI RvaToOffset (IMAGE_DOS_HEADER *lpFile,VOID *Rva)
{
	PIMAGE_DOS_HEADER lpDosHead;	//指向Dos头指针
	PIMAGE_NT_HEADERS32 lpNTHead;	//指向NT头指针
	DWORD dwNumeOfSection;			//节数目
	PIMAGE_OPTIONAL_HEADER32 lpOptionHead;	//指向可选头
	PIMAGE_SECTION_HEADER lpSectionHead;	//指向节表头
	int i;
	
	lpDosHead = lpFile;
	lpNTHead = (PIMAGE_NT_HEADERS32)((byte *)lpDosHead+lpDosHead->e_lfanew);
	dwNumeOfSection = lpNTHead->FileHeader.NumberOfSections;
	lpOptionHead = (PIMAGE_OPTIONAL_HEADER32)((byte *)lpNTHead+
					sizeof (DWORD)+
					sizeof (IMAGE_FILE_HEADER));
	lpSectionHead = (byte *)&(lpOptionHead->NumberOfRvaAndSizes)+
					sizeof (DWORD)+
					((lpOptionHead->NumberOfRvaAndSizes)*(sizeof (IMAGE_DATA_DIRECTORY)));
	for	(i=0;dwNumeOfSection != 0;dwNumeOfSection--,i++)	//循环计算Rva落在哪个节中
	{
		if ((DWORD)Rva >= lpSectionHead[i].VirtualAddress && 
			(DWORD)Rva <= (lpSectionHead[i].VirtualAddress+lpSectionHead[i].Misc.VirtualSize))
			//如果Rva落在某个节的Rva区间
			return (VOID *)((DWORD)Rva-lpSectionHead[i].VirtualAddress+
							(DWORD)lpFile+
							lpSectionHead[i].PointerToRawData);
			//返回Rva对应的指向PE内存映射的地址
	}
	return NULL;
}

