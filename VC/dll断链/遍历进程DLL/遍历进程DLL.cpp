// 遍历进程DLL.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <Tlhelp32.h>

int main(int argc, char* argv[])//遍历模块
{
	PROCESSENTRY32 processentry32;
	MODULEENTRY32 moduleentry32;
	DWORD PID;
	HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);

	Process32First(handle,&processentry32);
	do 
	{
		if (!strcmp(processentry32.szExeFile,"dll断链.exe"))
		{
			PID = processentry32.th32ProcessID;
		}
	} while (Process32Next(handle,&processentry32));
	
	
	HANDLE handle2 = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,PID);

	Module32First(handle2,&moduleentry32);

	do 
	{
		printf("模块名称：%s\r\n",moduleentry32.szModule);
		printf("模块路径：%s\r\n\n",moduleentry32.szExePath);
		

	} while (Module32Next(handle2,&moduleentry32));
	return 0;
}

