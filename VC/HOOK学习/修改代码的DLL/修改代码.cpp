// 修改代码.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <windows.h>

DWORD g_addr;

__declspec (naked)  void IsCPUID(void)
{
	__asm
	{
		mov eax,0x11111111;
		mov ebx,0x22222222
		mov ecx,0x33333333;
		mov edx,0x44444444;

		mov dword ptr ss:[ebp-0x114],eax
		push g_addr
		ret
	}
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
			::MessageBox(NULL, "DLL注入成功...", ":)", MB_ICONINFORMATION);

			g_addr = 0x00401037;
			BYTE  szJmp[6] = {0x90};
			szJmp[0] = 0xe9;
			
			DWORD dwJmp = (DWORD)IsCPUID - 0x00401037 - 5;
			memcpy(&szJmp[1],&dwJmp,4);

			DWORD dwProtet = 0;
			VirtualProtect((void*)g_addr,0x10000,PAGE_EXECUTE_READWRITE,&dwProtet);
			
			memcpy((void*)g_addr,(void*)szJmp,6);
			g_addr = g_addr + 6;

		}
		break;
	}
	
    return TRUE;
}

