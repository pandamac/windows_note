// dll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "dll.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			{
				MessageBox(0,"准备开始DLL注入","",MB_ICONINFORMATION);
				DWORD dwProctect;
				VirtualProtect((PVOID)0x00415211,0x1,PAGE_EXECUTE_READWRITE,&dwProctect);
				*(PBYTE)0x00415211 = 0x85;
				VirtualProtect((PVOID)0x00415211,0x1,dwProctect,&dwProctect);

				break;
			}
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			{
				MessageBox(0,"DLL注入完成","",MB_ICONINFORMATION);
			}
			break;
    }
    return TRUE;
}


// This is an example of an exported variable
DLL_API int nDll=0;

// This is an example of an exported function.
DLL_API int fnDll(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see dll.h for the class definition
CDll::CDll()
{ 
	return; 
}

