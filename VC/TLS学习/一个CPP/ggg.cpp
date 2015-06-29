// ggg.cpp : Defines the entry point for the application.
// coded by Benina (REA Team 2010)
//

#include "stdafx.h"

typedef struct _IMAGE_TLS_DIRECTORY32_ {
	DWORD   StartAddressOfRawData;
	DWORD   EndAddressOfRawData;
	DWORD   AddressOfIndex;
	DWORD   AddressOfCallBacks;
	DWORD   SizeOfZeroFill;
	DWORD   Characteristics;
} IMAGE_TLS_DIRECTORY32_, * PIMAGE_TLS_DIRECTORY32_;

VOID NTAPI on_tls_callback(PVOID handle, DWORD reason, PVOID resv)
{
	// Run code here
	switch (reason )
	{
		case  DLL_PROCESS_ATTACH:
		
			{
				MessageBox(NULL, TEXT("Thread attach!"), TEXT("TLS Callback"), 0);
				break;
			}

		case DLL_PROCESS_DETACH:
			{			
				MessageBox(NULL, TEXT("Thread detach!"), TEXT("TLS Callback"), 0);
				break;
			}
		default:
		break;
	};
}
#pragma comment(linker, "/INCLUDE:__tls_used")

extern "C" {
	
#pragma data_seg(".tls")
	__declspec(allocate(".tls"))
		DWORD __tls_start = 0x0;
	
#pragma data_seg(".tls")
	__declspec(allocate(".tls"))
		DWORD __tls_end = 0x0;
	
#pragma data_seg(".tls")
	__declspec(allocate(".tls"))
		DWORD __tls_index = 0x0;
#pragma data_seg(".tls")
	__declspec(allocate(".tls"))
		DWORD __tls_func = (DWORD) on_tls_callback;
#pragma data_seg(".tls")
	__declspec(allocate(".tls"))
		DWORD __tls_size = 0x0;
#pragma data_seg(".tls")
	__declspec(allocate(".tls"))
		DWORD __tls_flag = 0x0;	
}

#pragma data_seg(".rdata$T")
__declspec(allocate(".rdata$T"))
extern "C" const _IMAGE_TLS_DIRECTORY32_ _tls_used =
{
	(DWORD) &__tls_start,
		(DWORD) &__tls_end,
		(DWORD) &__tls_index,
		(DWORD ) &__tls_func,
		(DWORD ) 0,
		(DWORD ) 0
};


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MessageBox(NULL, TEXT("Hello, Main Program!"), TEXT("Main Proc"), 0);

	return 0;
}



