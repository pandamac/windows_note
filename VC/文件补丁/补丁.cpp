// 补丁.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>

int main(int argc, char* argv[])
{
#define PATCH_ADDRESS 0x00408EC2

	char szFileName[] = "5Star.exe";
	BOOL flag = TRUE;
	BYTE ReadBuffer[128] = {0};
	BYTE TarGetData[] = {0x0F,0x85,0x0A,0x00,0x00,0x00};
	BYTE WriteData[] =	{0x74,0x0E,0x90,0x90,0x90,0x90};
	DWORD Oldpp;

	STARTUPINFO si = {sizeof(STARTUPINFO)};
	PROCESS_INFORMATION pi;
	if (!CreateProcessA(szFileName,0,0,0,0,CREATE_SUSPENDED,0,0,&si,&pi))
	{
		MessageBox(NULL,"CreateProcess Failed","error",MB_ICONERROR);
		return FALSE;
	}
	
	while (flag)
	{
		ResumeThread(pi.hThread);
		Sleep(10);//程序运行10MS
		SuspendThread(pi.hThread);//看程序是否已解码
		ReadProcessMemory(pi.hProcess,(LPVOID)PATCH_ADDRESS,&ReadBuffer,6,NULL);
		if (0 == memcmp(TarGetData,ReadBuffer,6))
		{
			VirtualProtectEx(pi.hProcess,(LPVOID)PATCH_ADDRESS,6,PAGE_EXECUTE_READWRITE,&Oldpp);
			WriteProcessMemory(pi.hProcess,(LPVOID)PATCH_ADDRESS,&WriteData,6,0);
			ResumeThread(pi.hThread);
			flag = FALSE;
		}
	}

	
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return 0;
}

