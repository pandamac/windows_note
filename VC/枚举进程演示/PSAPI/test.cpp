#include <windows.h>
#include <stdio.h>

#include "psapi.h"
#pragma comment(lib,"psapi.lib")

int main()
{
	DWORD ProcessCount;
	DWORD cbNeeded;
	DWORD ProcessId[1024];

	EnumProcesses(ProcessId,sizeof(ProcessId),&cbNeeded);
	ProcessCount = cbNeeded/sizeof(DWORD);

	HMODULE hModule;
	char    szPath[MAX_PATH];

	for(DWORD i = 0; i < ProcessCount; i ++)
	{
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,FALSE,ProcessId[i]);
		if(hProcess)
		{
			EnumProcessModules(hProcess,&hModule,sizeof(hModule),&cbNeeded);
			GetModuleFileNameEx(hProcess,hModule,szPath,sizeof(szPath));

			printf("PID:%d \t%s\n",ProcessId[i],szPath);
		}
		else
			continue;
	}

	return 0;
}