#include <WINDOWS.H>
#include "AddExport.h"

/*
参数：lpFileMap指向内存映射文件
功能：通过传入的lpFileMap以及LPSTFILEMAP类型结构体中的hFile、hFileMap，释放该内存映射文件
返回值：返回NULL为出错，否则返回-1
*/

BOOL WINAPI FreeMap (LPVOID lpFileMap,LPSTFILEMAP lpstFileMap)
{
	if (!UnmapViewOfFile(lpFileMap))
		return 0;
	if (!CloseHandle(lpstFileMap->hFileMap))
		return 0;
	if (!CloseHandle(lpstFileMap->hFile))
		return 0;
	return -1;
	
}
