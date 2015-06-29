// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

#include "..//dlltest//detour//detours.h"


PVOID g_pOldMessageBoxA=NULL;  
typedef int (WINAPI *PfuncMessageBoxA)(HWND hWnd,LPCSTR lpText,LPCSTR lpCaption,UINT uType);  
int WINAPI ZwNewMessageBoxA(HWND hWnd,LPCSTR lpText,LPCSTR lpCaption,UINT uType)  
{  
	return ((PfuncMessageBoxA)g_pOldMessageBoxA)(hWnd, "Hook This!","My hook",uType);  
}  
//安装Hook   
BOOL APIENTRY SetHook()  
{  
	//大概是 开始事务吧  
	DetourTransactionBegin();  
	//更新线程信息  
	DetourUpdateThread(GetCurrentThread());  
	//查询函数 地址 将MessageBoxA 地址保存在这个 指针里面  
	g_pOldMessageBoxA=DetourFindFunction("User32.dll","MessageBoxA");  
	//将我们的拦截函数 附加到 MessageBoxA 地址上  
	DetourAttach(&g_pOldMessageBoxA,ZwNewMessageBoxA);  
	//完成hook  
	LONG ret=DetourTransactionCommit();  
	return ret==NO_ERROR;  
}  

//卸载Hook  
BOOL APIENTRY DropHook()  
{  
	DetourTransactionBegin();  
	DetourUpdateThread(GetCurrentThread());  
	DetourDetach(&g_pOldMessageBoxA, ZwNewMessageBoxA);  
	LONG ret=DetourTransactionCommit();  
	return ret==NO_ERROR;  
}  

static HMODULE s_hDll;  

BOOL APIENTRY DllMain( HMODULE hModule,DWORD  ul_reason_for_call,LPVOID lpReserved )  
{  
	switch (ul_reason_for_call)  
	{  
	case DLL_PROCESS_ATTACH:  
		s_hDll = hModule;  
		DisableThreadLibraryCalls(hModule);  
		SetHook();  
		break;  
	case DLL_THREAD_ATTACH:  
		break;  
	case DLL_THREAD_DETACH:  
		break;  
	case DLL_PROCESS_DETACH:  
		DropHook();//UNHOOK  
		break;  
	}  
	return TRUE;  
}  

