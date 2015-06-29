#include <WINDOWS.H>
#include "AddExport.h"

/*
参数：szDll指向需要扩大最后一个节的DLL文件名，dwLength为需要添加数据的长度
功能：为szDll的最后一个节扩大dwLength大小长度
返回值：返回NULL为出错，否则返回扩大的最后一个节的新增部分的Rva地址
*/

DWORD WINAPI ExpandLastSection(PTCHAR lpszDll,DWORD dwLength)
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
	DWORD dwOldVirtualSize;
	DWORD dwOldSizeOfRawData;
	DWORD dwRet;
	DWORD dwRealAddLen;	//DLL文件真实增加的大小

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
		MessageBox(NULL,TEXT ("打开需要扩大节的DLL文件失败!"),TEXT ("Error!"),MB_OK);
		return 0;
	}
	//得到新增数据的DLL文件的FileAlignment和页对齐值
	lpDosHead = lpMapFile;
	lpNTHead = (byte *)lpDosHead+lpDosHead->e_lfanew;
	lpOptionHead =	((byte *)lpNTHead+
					sizeof (DWORD)+
					sizeof (IMAGE_FILE_HEADER));
	dwFileAlignment = lpOptionHead->FileAlignment;
	dwSectionAlignment = lpOptionHead->SectionAlignment;
	//得到目标文件节数目
	dwNumeOfSection = lpNTHead->FileHeader.NumberOfSections;

	//通过FileAlignment对齐需要新增的数据大小
	dwAddLen = (dwLength+dwFileAlignment-1) & ~(dwFileAlignment-1);
	//得到第一个节表指针
	lpSectionHead = (byte *)&(lpOptionHead->NumberOfRvaAndSizes)+
					sizeof (DWORD)+
					((lpOptionHead->NumberOfRvaAndSizes)*(sizeof (IMAGE_DATA_DIRECTORY)));

	//保存节数目
	i = dwNumeOfSection;
	//保存原始VirtualSize和SizeOfRawData
	dwOldVirtualSize = lpSectionHead[i-1].Misc.VirtualSize;
	dwOldSizeOfRawData = lpSectionHead[i-1].SizeOfRawData;
	//扩展最后一个节的大小不能 直接在后追加数据，因为增加的数据映射到内存空间后可能会被其它数据复写
	//应该先增加最后一个节的大小和VirtualSize相同
	//先将VirtualSize对齐后设置回去
	lpSectionHead[i-1].Misc.VirtualSize = ((dwOldVirtualSize+dwSectionAlignment-1) & ~(dwSectionAlignment-1));
	lpSectionHead[i-1].SizeOfRawData = lpSectionHead[i-1].Misc.VirtualSize;

	//计算返回值
	dwRet = lpSectionHead[i-1].VirtualAddress + lpSectionHead[i-1].Misc.VirtualSize;
	//扩大最后一个节容量为需要增加的数据长度(节对齐值)
	lpSectionHead[i-1].Misc.VirtualSize += dwAddLen;
	lpSectionHead[i-1].SizeOfRawData += dwAddLen;

	//修改SizeOfImage大小
	lpNTHead->OptionalHeader.SizeOfImage += (dwAddLen+dwSectionAlignment-1) & ~(dwSectionAlignment-1);

	//修改导出表数据目录值，使其指向新增的数据头部Rva
	lpOptionHead->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress = dwRet;
	lpOptionHead->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size = dwLength;
	//得到真实增加的数据大小
	dwRealAddLen = lpSectionHead[i-1].SizeOfRawData - dwOldSizeOfRawData;
	//释放内存
	FreeMap(lpMapFile,&stFileMap);

	//为目标文件增加大小
	hFile = CreateFile(stFileMap.szDll,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
	if ((DWORD)hFile == -1)
		return 0;

	SetFilePointer(hFile,dwRealAddLen,NULL,FILE_END);
	SetEndOfFile(hFile);
	CloseHandle(hFile);
	//返回新的导出表Rva
	return dwRet;

}
