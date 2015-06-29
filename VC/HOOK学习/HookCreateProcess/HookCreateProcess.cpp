#include "ILHook.h"

CILHook CreateProcessHook;

// 我们实现的Hook函数
BOOL
WINAPI
MyCreateProcessW(
    LPCWSTR lpApplicationName,
    LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFOW lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation
    )
{
    BOOL bRet = FALSE;

    if ( MessageBoxW(NULL, lpApplicationName, lpCommandLine, MB_YESNO) == IDYES )
    {
        CreateProcessHook.UnHook();
        bRet = CreateProcessW(lpApplicationName,
                        lpCommandLine,
                        lpProcessAttributes,
                        lpThreadAttributes,
                        bInheritHandles,
                        dwCreationFlags,
                        lpEnvironment,
                        lpCurrentDirectory,
                        lpStartupInfo,
                        lpProcessInformation);
         CreateProcessHook.ReHook();
    } 
    else
    {
        MessageBox(NULL, "您启动的程序被拦截", "提示", MB_OK);
    }
    
//     CreateProcessHook.UnHook();
//     // 弹出被创建进程的进程名
//     MessageBoxW(NULL, lpApplicationName, lpCommandLine, MB_OK);
// 
//     // 创建进程
//     bRet = CreateProcessW(lpApplicationName,
//                     lpCommandLine,
//                     lpProcessAttributes,
//                     lpThreadAttributes,
//                     bInheritHandles,
//                     dwCreationFlags,
//                     lpEnvironment,
//                     lpCurrentDirectory,
//                     lpStartupInfo,
//                     lpProcessInformation);
// 
//     CreateProcessHook.ReHook();

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
            // Hook CreateProcessW()函数
            CreateProcessHook.Hook("kernel32.dll",
                "CreateProcessW",
                (PROC)MyCreateProcessW);
            break;
        }
    case DLL_PROCESS_DETACH:
        {
            CreateProcessHook.UnHook();
            break;
        }
    }
    
    return TRUE;
}

