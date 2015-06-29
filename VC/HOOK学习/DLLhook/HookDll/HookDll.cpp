// HookDll.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <windows.h>
#include "HookDll.h"
#define KEYHOOKLIB_EXPORTS

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 共享数据段
#pragma data_seg("YCIShared")
HWND g_hWndCaller = NULL;	// 保存主窗口句柄
HHOOK g_hHook = NULL;		// 保存钩子句柄
#pragma data_seg()



// 一个通过内存地址取得模块句柄的帮助函数
HMODULE WINAPI ModuleFromAddress(PVOID pv) 
{
	MEMORY_BASIC_INFORMATION mbi;
	if(::VirtualQuery(pv, &mbi, sizeof(mbi)) != 0)
	{
		return (HMODULE)mbi.AllocationBase;
	}
	else
	{
		return NULL;
	}
}

// 键盘钩子函数
LRESULT CALLBACK KeyHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if(nCode < 0 || nCode == HC_NOREMOVE)
		return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);
	
	if(lParam & 0x40000000)	// 消息重复就交给下一个hook链
	{
		return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);
	}
	
	// 通知主窗口。wParam参数为虚拟键码, lParam参数包含了此键的信息
	::PostMessage(g_hWndCaller, HM_KEY, wParam, lParam);
	
	return ::CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

// 安装、卸载钩子的函数
BOOL WINAPI SetKeyHook(BOOL bInstall, DWORD dwThreadId, HWND hWndCaller)
{
	BOOL bOk;
	g_hWndCaller = hWndCaller;
	
	if(bInstall)
	{
		g_hHook = ::SetWindowsHookEx(WH_KEYBOARD, KeyHookProc, 
			ModuleFromAddress(KeyHookProc), dwThreadId);
		bOk = (g_hHook != NULL);
	} 
	else 
	{
		bOk = ::UnhookWindowsHookEx(g_hHook);
		g_hHook = NULL;
	}
	
	return bOk;
}