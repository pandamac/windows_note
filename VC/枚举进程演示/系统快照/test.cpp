#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h>

void main()
{
	PROCESSENTRY32 ProcessEntry = { 0 };
    MODULEENTRY32  ModuleEntry  = { 0 };
	HANDLE hProcessSnap;
    HANDLE hModuleSnap;
	ProcessEntry.dwSize = sizeof(PROCESSENTRY32);
    ModuleEntry.dwSize  = sizeof(MODULEENTRY32);
	
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	BOOL bRet = Process32First(hProcessSnap,&ProcessEntry);
	while(bRet)
	{
		printf("\n%s\n",ProcessEntry.szExeFile);

        hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,ProcessEntry.th32ProcessID);
        bRet = Module32First(hModuleSnap,&ModuleEntry);
        while(bRet)
        {
			printf("\t%s\n",ModuleEntry.szExePath);

            bRet = Module32Next(hModuleSnap,&ModuleEntry);
        }
		
		bRet = Process32Next(hProcessSnap,&ProcessEntry);
	}
}