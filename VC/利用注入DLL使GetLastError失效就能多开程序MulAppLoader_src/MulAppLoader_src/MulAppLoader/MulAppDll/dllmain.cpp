#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <windows.h>
#include ".\\detour\\detours.h"

static DWORD (WINAPI * Old_GetLastError)() = GetLastError;
DWORD New_GetLastError()
{
	DWORD dwLastError = Old_GetLastError();
	return dwLastError == ERROR_ALREADY_EXISTS ? 0 : dwLastError;
}

VOID Hook ()
{
	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	// 这里可以连续多次调用DetourAttach，表明Hook多个函数
	DetourAttach(&(PVOID&)Old_GetLastError, New_GetLastError ) ;

	DetourTransactionCommit();
}

VOID UnHook ()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	// 这里可以连续多次调用DetourDetach，表明撤消多个函数Hook
	DetourDetach(&(PVOID&)Old_GetLastError, New_GetLastError ) ;

	DetourTransactionCommit();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					   )
{
	if(ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		Hook();
	}
	else if(ul_reason_for_call == DLL_PROCESS_DETACH)
	{
		UnHook();
	}

	return TRUE;
}