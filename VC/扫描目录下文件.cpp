#include <Windows.h>
#include <stdio.h>

DWORD dwTotalFileNum = 0;

DWORD ListAllFileInDirectory(LPTSTR szPath)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hListFile;
	TCHAR szFilePath[MAX_PATH];
	TCHAR szFullPath[MAX_PATH];
	
	lstrcpy(szFilePath,szPath);
	lstrcat(szFilePath,TEXT("\\*"));
	
	hListFile = FindFirstFile(szFilePath, &FindFileData);
	if(hListFile == INVALID_HANDLE_VALUE)
	{
		printf("´íÎó: %d\n", GetLastError());
		return 1;
	}
	else
	{
		do
		{
			if(lstrcmp(FindFileData.cFileName, TEXT(".")) == 0 ||
				lstrcmp(FindFileData.cFileName, TEXT("..")) == 0)
			{
				continue;
			}
			wsprintf(szFullPath, "%s\\%s", szPath, FindFileData.cFileName);
			dwTotalFileNum++;
			printf("\n%d\t%s\t",dwTotalFileNum, szFullPath);
			if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				printf("<DIR>");
				ListAllFileInDirectory(szFullPath);
			}

		}while(FindNextFile(hListFile,&FindFileData));
	}
	return 0;
}

int main()
{
	ListAllFileInDirectory(TEXT("d:\\demo\\demo"));
	printf("\n");
	system("pause");
	return 0;
}