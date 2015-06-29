#include <Windows.h>
#include <stdio.h>

struct Date
{
	short month;
	short day;
	short year;
};

DWORD ShowFileTime(PFILETIME lptime)
{
	FILETIME ftLocal;
	SYSTEMTIME st;
	FileTimeToLocalFileTime(lptime,&ftLocal);
	FileTimeToSystemTime(&ftLocal,&st);
	printf("%d年%#02d月%#02d日, %#02d:%#02d:%#02d\n",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	return 0;
}

int main()
{
	WIN32_FILE_ATTRIBUTE_DATA wfad;

	if(!GetFileAttributesEx("main.c",GetFileExInfoStandard,&wfad))
	{
		printf("获取文件属性失败: %d\n", GetLastError());
		return 1;
	}

	printf("创建时间:\t");
	ShowFileTime(&wfad.ftCreationTime);
	printf("访问时间:\t");
	ShowFileTime(&wfad.ftLastAccessTime);
	printf("修改时间:\t");
	ShowFileTime(&wfad.ftLastWriteTime);

	system("pause");
	return 0;
}