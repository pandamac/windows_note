#include "SampleDLL.h"

extern "C" {

	SAMPLEDLL_API BOOL DeleteDirectory(LPCTSTR lpszDir)
	{
		if(lpszDir == NULL)
		{
			return FALSE;
		}
		WIN32_FIND_DATA fData = {0};
		TCHAR tcDirectory[MAX_PATH] = {0}, tcScanFileType[MAX_PATH] = {0};
		int iLength = strlen(lpszDir);
		if (iLength > MAX_PATH) //如果目录路径过长则失败
		{
			return FALSE;
		}
		strcpy(tcScanFileType, lpszDir);  //保存扫描文件夹
		if ('\\' == tcScanFileType[iLength - 1])
			strcat(tcScanFileType, "*.*");
		else
			strcat(tcScanFileType, "\\*.*"); //构造扫描文件类型名
		HANDLE hFindHandle = FindFirstFile(tcScanFileType, &fData);
		if (hFindHandle != INVALID_HANDLE_VALUE)
		{
			do 
			{
				if ('.' == fData.cFileName[0])
				{   //跳过上层目录
					continue;
				}
				strcpy(tcDirectory, lpszDir);  //保存扫描文件夹
				if ( '\\' != tcDirectory[iLength - 1])
					strcat(tcDirectory, "\\");

				if (iLength + strlen(fData.cFileName) > MAX_PATH) //文件名过长则失败
				{
					break;
				}
				strcat(tcDirectory, fData.cFileName); //新文件名或子文件夹名
				fData.dwFileAttributes &= FILE_ATTRIBUTE_DIRECTORY;
				if (FILE_ATTRIBUTE_DIRECTORY == fData.dwFileAttributes)//判断是否是目录
				{
					DeleteDirectory(tcDirectory);  //递归删除子目录
				}
				else
				{
					DeleteFile(tcDirectory);   //删除目录中的文件。
				}
			} while (FindNextFile(hFindHandle, &fData));
			FindClose(hFindHandle);
		}
		return RemoveDirectory(lpszDir); //删除目录
	}
}
