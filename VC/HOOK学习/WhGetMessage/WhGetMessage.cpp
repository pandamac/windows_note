#include <windows.h>

extern "C" __declspec(dllexport) VOID SetHookOn();
extern "C" __declspec(dllexport) VOID SetHookOff();

HHOOK g_HHook = NULL;
HINSTANCE g_hInst = NULL;

VOID DoSomeThing()
{
    /*
    ……
    自己要实现功能的代码
    ……
    */
}

BOOL WINAPI DllMain(
                    HINSTANCE hinstDLL,  // handle to the DLL module
                    DWORD fdwReason,     // reason for calling function
                    LPVOID lpvReserved   // reserved
)
{
    switch ( fdwReason )
    {
    case DLL_PROCESS_ATTACH:
        {
            g_hInst = hinstDLL;
            DoSomeThing();
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
    return CallNextHookEx(g_HHook, code, wParam, lParam);
}

VOID SetHookOn()
{
    g_HHook = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, g_hInst, 0);
}

VOID SetHookOff()
{
    UnhookWindowsHookEx(g_HHook);
}