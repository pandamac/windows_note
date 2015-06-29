// LpkDll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <Windows.h>
#include <tchar.h>

HANDLE g_hModule = NULL;   //保存自己的模块句柄
extern "C" __declspec(dllexport) void Test()
{
	TCHAR tcModulePath[MAX_PATH] = {0};
	//获取自身的完整路径
	GetModuleFileName((HMODULE)g_hModule, tcModulePath, MAX_PATH);
	MessageBox(0, tcModulePath, _T("LpkDll的加载路径"), 0);
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved)
{
	g_hModule = hModule; //保存模块加载句柄
	return TRUE;
}

