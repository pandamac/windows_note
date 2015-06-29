// 测试用DLL.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <windows.h>

//1）当dll使用静态C运行时库进行链接(CRT，C run-time library ).时，不要使用该函数，因为此时dll需要DLL_THREAD_ATTACH 和DLL_THREAD_DETATCH通知才能发挥正常作用。
int __stdcall StartAddress(int a1)
{
	return MessageBoxA(0, "HOOK!", "Warning!", 0);
}

BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
	DWORD hObject;

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//MessageBox(NULL,"DLL注入","DLL注入",MB_OK);
		//MessageBox(NULL,"+++++++++","++++++++++",MB_OK);

		DisableThreadLibraryCalls((HMODULE)hModule);
// 禁用指定的DLL的DLL_THREAD_ATTACH和DLL_THREAD_DETACH通知，这样可以减小某些程序的工作集大小。
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)StartAddress, 0, 0, &hObject);
	    CloseHandle((HANDLE)hObject);

		break;
// 	case DLL_PROCESS_DETACH:
// 		MessageBox(NULL,"+++++++++","++++++++++",MB_OK);
// 		//MessageBox(NULL,"DLL注入结束","DLL注入结束",MB_OK);
// 		break;
// 	case DLL_THREAD_ATTACH:
// 		
// 		break;
// 	case DLL_THREAD_DETACH:
// 		
// 		break;
	}
    return TRUE;
}

// 
// extern "C" void __declspec(dllexport)  FuncInDll (void)
// {
// 	MessageBox(NULL,"函数","函数",MB_OK);
// }
// extern "C" void __declspec(dllexport)  FuncInDll2 (void)
// {
// 	MessageBox(NULL,"DLL2","DLL2",MB_OK);
// }