#include <windows.h>
#include "AddExport.h"

/*
参数：lpFileMap指向LPSTFILEMAP类型的结构体
功能：通过传入的LPSTFILEMAP类型结构体中的szDll名及各类内核对象访问权限等，内存映射该文件，并将打开的句柄和Map句柄填入传入的结构体内
返回值：返回NULL为出错，否则返回指向内存映射文件的指针
*/

LPVOID WINAPI MapFile (LPSTFILEMAP lpstFileMap)
{
	HANDLE	hFile;
	HANDLE	hFileMap;
	HANDLE	lpFileMap;
	
//	hFile = CreateFile	(lpstFileMap->szDll,GENERIC_READ | GENERIC_WRITE,
//		FILE_SHARE_READ | FILE_SHARE_WRITE,
	hFile = CreateFile	(lpstFileMap->szDll,lpstFileMap->dwDesiredAccess,
		lpstFileMap->dwShareMode,
		NULL,
		lpstFileMap->dwCreationDisposition,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if ((DWORD)hFile == -1)
		return NULL;
	
//	hFileMap = CreateFileMapping (hFile,NULL,PAGE_READWRITE,0,0,NULL);
	hFileMap = CreateFileMapping (hFile,NULL,lpstFileMap->flProtect,0,0,NULL);
	if (!hFileMap)
	{
		CloseHandle(hFile);
		return NULL;
	}
//	lpFileMap = MapViewOfFile(hFileMap,FILE_MAP_READ | FILE_MAP_WRITE,0,0,0);
	lpFileMap = MapViewOfFile(hFileMap,lpstFileMap->dwDesiredAccessMap,0,0,0);
	if (!lpFileMap)
	{
		CloseHandle(hFileMap);
		CloseHandle(hFile);
		return NULL;
	}
	lpstFileMap->hFile = hFile;
	lpstFileMap->hFileMap = hFileMap;
	return (LPVOID)lpFileMap;
	/**/
}