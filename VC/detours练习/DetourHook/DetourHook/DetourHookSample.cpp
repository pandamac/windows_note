// RedirectApi.cpp : 定义应用程序的入口点。
//

#include <windows.h>
#include "..\\detour\\detours.h"


static int (WINAPI* OLD_MessageBoxA)( HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType ) = MessageBoxA;

int WINAPI NEW_MessageBoxA( HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType )
{
	// 此处可以观察/修改调用参数，甚至可以取消调用直接返回。
	// ……

	// 修改输入参数，调用原函数
	int ret = OLD_MessageBoxA ( hWnd, "输入参数已被修改", "[测试]", uType ) ;

	// 此处可以查看/修改调用原函数的返回值
	// ……

	return ret ;
}

VOID Hook ()
{
	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	
	// 这里可以连续多次调用DetourAttach，表明Hook多个函数
	DetourAttach(&(PVOID&)OLD_MessageBoxA, NEW_MessageBoxA ) ;

	DetourTransactionCommit();
}

VOID UnHook ()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	// 这里可以连续多次调用DetourDetach，表明撤消多个函数Hook
	DetourDetach(&(PVOID&)OLD_MessageBoxA, NEW_MessageBoxA ) ;

	DetourTransactionCommit();
}

int WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	MessageBoxA ( 0, "正常消息框", "测试", 0 ) ;
	Hook () ;
	MessageBoxA ( 0, "正常消息框", "测试", 0 ) ;
	UnHook () ;
	return 0 ;
}