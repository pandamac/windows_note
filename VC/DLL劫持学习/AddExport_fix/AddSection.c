#include <WINDOWS.H>
#include "AddExport.h"

/*
参数：szDll指向需要添加节的DLL文件名，dwLength为需要添加数据的长度
功能：为szDll增加一个新节，只读
返回值：返回NULL为出错，否则返回新增节的Rva地址
*/

DWORD WINAPI AddSection(PTCHAR lpszDll,DWORD dwLength)
{
	STFILEMAP stFileMap;
	LPVOID lpMapFile;
	DWORD dwFileAlignment;	//需要新增节的目标文件对齐值
	DWORD dwSectionAlignment;	//需要新增的目标文件页对齐值
	DWORD dwAddLen;	//按照FileAlignment对齐后的新增节大小
	PIMAGE_DOS_HEADER lpDosHead;	//指向Dos头指针
	PIMAGE_NT_HEADERS32 lpNTHead;	//指向NT头指针
	DWORD dwNumeOfSection;			//节数目
	PIMAGE_OPTIONAL_HEADER32 lpOptionHead;	//指向可选头
	PIMAGE_SECTION_HEADER lpSectionHead;	//指向节表头
	int i;
	HANDLE hFile;
	DWORD dwRet;

	//初始化结构
	stFileMap.szDll = lpszDll;
	stFileMap.hFile = 0;
	stFileMap.hFileMap =0;
	stFileMap.dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
	stFileMap.dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
	stFileMap.dwCreationDisposition = OPEN_EXISTING;
	stFileMap.flProtect = PAGE_READWRITE;
	stFileMap.dwDesiredAccessMap = FILE_MAP_READ | FILE_MAP_WRITE;
	lpMapFile = MapFile(&stFileMap);
	if (!lpMapFile)
	{
		MessageBox(NULL,TEXT ("打开需要新增节的DLL文件失败!"),TEXT ("Error!"),MB_OK);
		return 0;
	}
	//得到新增节DLL文件的FileAlignment和页对齐值
	lpDosHead = lpMapFile;
	lpNTHead = (byte *)lpDosHead+lpDosHead->e_lfanew;
	lpOptionHead =	((byte *)lpNTHead+
					sizeof (DWORD)+
					sizeof (IMAGE_FILE_HEADER));
	dwFileAlignment = lpOptionHead->FileAlignment;
	dwSectionAlignment = lpOptionHead->SectionAlignment;
	//得到目标文件节数目
	dwNumeOfSection = lpNTHead->FileHeader.NumberOfSections;

	//通过FileAlignment对齐需要新增的节大小
	dwAddLen = (dwLength+dwFileAlignment-1) & ~(dwFileAlignment-1);	//对齐新增节大小
	//得到第一个节表指针
	lpSectionHead = (byte *)&(lpOptionHead->NumberOfRvaAndSizes)+
					sizeof (DWORD)+
					((lpOptionHead->NumberOfRvaAndSizes)*(sizeof (IMAGE_DATA_DIRECTORY)));
	//判断最后一个节表往后是否可写入一个新的节表数据
	i = dwNumeOfSection;
	if ((lpSectionHead[i].Characteristics != 0) ||
		(lpSectionHead[i].Misc.VirtualSize != 0) ||
		(lpSectionHead[i].Name[0] != 0) ||					//???
		(lpSectionHead[i].NumberOfLinenumbers != 0) ||
		(lpSectionHead[i].NumberOfRelocations != 0) ||
		(lpSectionHead[i].PointerToLinenumbers != 0) ||
		(lpSectionHead[i].PointerToRawData != 0) ||
		(lpSectionHead[i].PointerToRelocations != 0) ||
		(lpSectionHead[i].SizeOfRawData != 0) ||
		(lpSectionHead[i].VirtualAddress != 0))
	{
		return 0;
	}
	//为新增节表赋值，并对齐这些值
	lpSectionHead[i].Characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ;
	lpSectionHead[i].Misc.VirtualSize = dwLength;
	lpSectionHead[i].VirtualAddress =	(lpSectionHead[i-1].VirtualAddress)+
										((lpSectionHead[i-1].Misc.VirtualSize+dwSectionAlignment-1) & ~(dwSectionAlignment-1));
	lpSectionHead[i].PointerToRawData =	(lpSectionHead[i-1].PointerToRawData)+
										((lpSectionHead[i-1].SizeOfRawData+dwFileAlignment-1) & ~(dwFileAlignment-1));
	lpSectionHead[i].SizeOfRawData = dwAddLen;

	//修改SizeOfImage大小
	lpNTHead->OptionalHeader.SizeOfImage += (dwLength+dwSectionAlignment-1) & ~(dwSectionAlignment-1);
	//增加节数目
	lpNTHead->FileHeader.NumberOfSections++;
	//修改导出表数据目录值，使其指向新节的头部Rva
	lpOptionHead->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress = lpSectionHead[i].VirtualAddress;
	lpOptionHead->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size = dwLength;

	//设置返回值
	dwRet = lpSectionHead[i].VirtualAddress;
	//同步修改值
	//FlushViewOfFile();
	//释放内存
	FreeMap(lpMapFile,&stFileMap);

	//为目标文件增加新节大小
	hFile = CreateFile(stFileMap.szDll,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
	if ((DWORD)hFile == -1)
		return 0;
	SetFilePointer(hFile,dwAddLen,NULL,FILE_END);
	SetEndOfFile(hFile);
	CloseHandle(hFile);
	//返回新的导出表Rva
	return dwRet;

}
