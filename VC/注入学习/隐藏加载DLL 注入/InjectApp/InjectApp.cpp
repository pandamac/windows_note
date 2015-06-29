// InjectApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include "InjectDllWithOutModule.h"
#include <tchar.h>

//本示例演示隐式向记事本进程注入一个DLL，但是并不产生新模块，
//该DLL将实时向优盘中写入一个a.txt的文件。
extern "C"  BOOL ThreadMain(_InjectData *pParam);
int main(int argc, char* argv[])
{
	_InjectData *pInjectData = new _InjectData;
	pInjectData->dwImageBase = (DWORD)0;
	
	strcpy(pInjectData->cLoad, "LoadLibraryA");
	strcpy(pInjectData->cGetAddr, "GetProcAddress");
	strcpy(pInjectData->cKernel32, "kernel32.dll");
	wcscpy(pInjectData->wcKernel32, L"kernel32.dll");

	DWORD dwProcessId = 0;
	HWND hWnd = ::FindWindow(_T("Notepad"), NULL);
	if(hWnd == NULL)
	{
		return -1;
	}
	GetWindowThreadProcessId(hWnd, &dwProcessId);
	CInjectDllWithOutModule test;
	if (!test.LoadDll("../Debug/InjectedDll.dll"))
	{
		return 0;
	}
	test.InjectDllAndRun(dwProcessId, "ThreadMain");
	return 0;
}

