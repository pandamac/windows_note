// vehtext.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "vehtext.h"
#include <Windows.h>

PVOID (CALLBACK* AddVectoredExceptionHandler)(ULONG FirstHandler,PVOID VectoredHandler);

LONG WINAPI VectoredHandler(
							PEXCEPTION_POINTERS ExceptionInfo
)
{
	

	if (ExceptionInfo->ExceptionRecord->ExceptionAddress = (PVOID)0x004503b7)
	{
		MessageBox(NULL,"触发CC断点","提示",MB_ICONINFORMATION);
		ExceptionInfo->ContextRecord->Eip = 0x0045039c;
	
		DWORD dwOldProtect;
		VirtualProtect((PVOID)0x004503b7,0x1000,PAGE_EXECUTE_READWRITE,&dwOldProtect);
	
		*(PBYTE)0x004503b7 = 0x6a;
	return EXCEPTION_CONTINUE_EXECUTION;
	}
	
	return EXCEPTION_CONTINUE_SEARCH;
}
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			{
				MessageBox(NULL,"DLL载入成功","提示",MB_ICONINFORMATION);
					
				// AddVectoredExceptionHandler必须为指针
				*(FARPROC*)&AddVectoredExceptionHandler = GetProcAddress(\
					GetModuleHandle("Kernel32.dll"),"AddVectoredExceptionHandler");
					AddVectoredExceptionHandler(1,VectoredHandler);

				DWORD dwOldProtect;
				VirtualProtect((PVOID)0x004503b7,0x1000,PAGE_EXECUTE_READWRITE,&dwOldProtect);

				*(PBYTE)0x004503b7 = 0xcc;
					
					break;
			}
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			{
				MessageBox(NULL,"DLL卸载","提示",MB_ICONINFORMATION);
			}
			break;
    }
    return TRUE;
}


// This is an example of an exported variable
VEHTEXT_API int nVehtext=0;

// This is an example of an exported function.
VEHTEXT_API int fnVehtext(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see vehtext.h for the class definition
CVehtext::CVehtext()
{ 
	return; 
}

