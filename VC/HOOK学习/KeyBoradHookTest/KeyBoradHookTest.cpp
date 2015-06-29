// KeyBoradHookTest.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

extern "C" __declspec(dllexport) VOID SetHookOn();
extern "C" __declspec(dllexport) VOID SetHookOff();

// 钩子句柄
HHOOK g_Hook = NULL;
// DLL模块句柄
HINSTANCE g_Inst = NULL;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    // 保存DLL的模块句柄
    g_Inst = (HINSTANCE)hModule;

    return TRUE;
}

// 钩子函数
LRESULT CALLBACK KeyboardProc(
                              int code,       // hook code
                              WPARAM wParam,  // virtual-key code
                              LPARAM lParam   // keystroke-message information
)
{
    if ( code < 0 )
    {
        return CallNextHookEx(g_Hook, code, wParam, lParam);
    }

    if ( code == HC_ACTION && lParam > 0 )
    {
        char szBuf[MAXBYTE] = { 0 };
        GetKeyNameText(lParam, szBuf, MAXBYTE);
        MessageBox(NULL, szBuf, NULL, MB_OK);
    }

    return CallNextHookEx(g_Hook, code, wParam, lParam);
}

VOID SetHookOn()
{
    // 安装钩子
    g_Hook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, g_Inst, 0);
}

VOID SetHookOff()
{
    // 卸载钩子
    UnhookWindowsHookEx(g_Hook);
}