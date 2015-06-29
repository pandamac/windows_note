// EasyHips.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "ILHook.h"
#include <stdio.h>
#include <windows.h>

#pragma data_seg(".shared")
HHOOK g_hHook = NULL;
HWND  g_ExeHwnd = NULL;
#pragma data_seg()

#pragma comment (linker, ".shared, RWS")

extern "C" __declspec(dllexport) VOID SetHookOn(HWND hWnd);
extern "C" __declspec(dllexport) VOID SetHookOff();

CILHook RegSetValueExWHook;
CILHook CreateProcessWHook;
CILHook RegDeleteValueWHook;

HINSTANCE g_hInst = NULL;

#define HIPS_CREATEPROCESS  0x00000001L
#define HIPS_REGSETVALUE    0x00000002L
#define HIPS_REGDELETEVALUE 0x00000003L

typedef struct _HIPS_INFO
{
    WCHAR wProcessName[0x200];
    DWORD dwHipsClass;
}HIPS_INFO, *PHIPS_INFO;

#define __in_opt


LONG
APIENTRY
MyRegSetValueExA (
                __in HKEY hKey,
                __in_opt LPCSTR lpValueName,
                __reserved DWORD Reserved,
                __in DWORD dwType,
                CONST BYTE* lpData,
                __in DWORD cbData
    )
{
    return 0;
}

LONG
APIENTRY
MyRegDeleteValueW (
                 __in HKEY hKey,
                 __in_opt LPCWSTR lpValueName
    )
{
    return 0;
}

#define __inout_opt
BOOL
WINAPI
MyCreateProcessW(
               __in_opt    LPCWSTR lpApplicationName,
               __inout_opt LPWSTR lpCommandLine,
               __in_opt    LPSECURITY_ATTRIBUTES lpProcessAttributes,
               __in_opt    LPSECURITY_ATTRIBUTES lpThreadAttributes,
               __in        BOOL bInheritHandles,
               __in        DWORD dwCreationFlags,
               __in_opt    LPVOID lpEnvironment,
               __in_opt    LPCWSTR lpCurrentDirectory,
               __in        LPSTARTUPINFOW lpStartupInfo,
               __out       LPPROCESS_INFORMATION lpProcessInformation
    )
{
//     WCHAR sz[0x200] = { 0 };
//     if ( wcslen(lpCommandLine) != 0 )
//     {
//         wcscpy(sz, lpCommandLine);
//     }
//     else
//     {
//         wcscpy(sz, lpApplicationName);
//     }

    HIPS_INFO sz = { 0 };
    if ( wcslen(lpCommandLine) != 0 )
    {
        wcscpy(sz.wProcessName, lpCommandLine);
    }
    else
    {
        wcscpy(sz.wProcessName, lpApplicationName);
    }

    sz.dwHipsClass = HIPS_CREATEPROCESS;

    COPYDATASTRUCT cds = { NULL, sizeof(HIPS_INFO), (void *)&sz };
    BOOL bRet = FALSE;
    if ( SendMessage(FindWindow(NULL, "Easy Hips For R3"), WM_COPYDATA, GetCurrentProcessId(), (LPARAM)&cds) != -1 )
    {
        CreateProcessWHook.UnHook();
        bRet = CreateProcessW(lpApplicationName, lpCommandLine,
                    lpProcessAttributes, lpThreadAttributes,
                    bInheritHandles, dwCreationFlags,
                    lpEnvironment, lpCurrentDirectory, 
                    lpStartupInfo, lpProcessInformation);
        CreateProcessWHook.ReHook();
    }
    
    return bRet;
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch ( ul_reason_for_call )
    {
    case DLL_PROCESS_ATTACH:
        {
            g_hInst = (HINSTANCE)hModule;
            RegSetValueExWHook.Hook("advapi32.dll", "RegSetValueExW", (PROC)MyRegSetValueExA);
            RegDeleteValueWHook.Hook("advapi32.dll", "RegDeleteValueW", (PROC)MyRegDeleteValueW);
            CreateProcessWHook.Hook("kernel32.dll", "CreateProcessW", (PROC)MyCreateProcessW);
            break;
        }
    case DLL_PROCESS_DETACH:
        {
            RegSetValueExWHook.UnHook();
            RegDeleteValueWHook.UnHook();
            CreateProcessWHook.UnHook();
            if ( g_hHook != NULL )
            {
                SetHookOff();
            }
            break;
        }
    }

    return TRUE;
}

LRESULT CALLBACK GetMsgProc(
                            int code,       // hook code
                            WPARAM wParam,  // removal option
                            LPARAM lParam   // message
                            )
{
    return CallNextHookEx(g_hHook, code, wParam, lParam);
}

VOID SetHookOn(HWND hWnd)
{
    g_ExeHwnd = hWnd;
    SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, g_hInst, 0);
}

VOID SetHookOff()
{
    UnhookWindowsHookEx(g_hHook);
    g_hHook = NULL;
}