// SingleApp.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>

int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, _T("event_singleapp"));

	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		_tcprintf(_T("event_singleapp already exists.\n"));
		getchar();
		return 0;
	}

	HANDLE hMutex = CreateMutex(NULL, FALSE, _T("mutex_singleapp"));

	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		_tcprintf(_T("mutex_singleapp already exists.\n"));
		getchar();
		return 0;
	}

	while(1)
	{
		Sleep(120000);
	}

	return 0;
}

