// HOOK.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "HOOK.h"
#include <windows.h>

void SetHook(HMODULE hModule);

typedef int (WINAPI * PFNMESSAGEBOX)(HWND hWnd,LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);

PROC g_proc  = (PROC)MessageBoxA;

int WINAPI MyMessageBox(HWND hWnd,LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
	return ((PFNMESSAGEBOX)g_proc)(hWnd,"被我HOOK的函数","哈哈",uType);
}

// int main(int argc, char* argv[])
// {
// 	MessageBox(NULL,"没有被修改的原函数","原函数",MB_ICONINFORMATION);
// 
// 	SetHook(GetModuleHandleA(NULL));
// 	MessageBox(NULL,"没有被修改的原函数","原函数",MB_ICONINFORMATION);
// 	
// 	return 0;
// }
void SetHook(HMODULE hModule)
{
	IMAGE_DOS_HEADER* pDos = (IMAGE_DOS_HEADER*)hModule;
	
	IMAGE_OPTIONAL_HEADER* pOpt = (IMAGE_OPTIONAL_HEADER*)((BYTE*)pDos + pDos->e_lfanew +24);
	
	IMAGE_IMPORT_DESCRIPTOR* pImageImportDescriptor = \
		(IMAGE_IMPORT_DESCRIPTOR*) ((BYTE*)hModule + \
		pOpt->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
	
	while (pImageImportDescriptor->FirstThunk)
	{
		char* pszDLLName = (char*)((BYTE*)hModule + pImageImportDescriptor->Name);
		
		if (lstrcmpi(pszDLLName, "user32.dll") == 0)
		{
			break;
		}
		pImageImportDescriptor++;
	}
	if (pImageImportDescriptor->FirstThunk)
	{
		IMAGE_THUNK_DATA* pThunk = (IMAGE_THUNK_DATA*)\
			((BYTE*)hModule + pImageImportDescriptor->FirstThunk);
		while (pThunk->u1.Function)
		{
			DWORD* lpAddr = (DWORD*)&(pThunk->u1.Function);
			if (*lpAddr == (DWORD)g_proc)
			{
				DWORD dwOldProtect;
				MEMORY_BASIC_INFORMATION MB;

				VirtualQuery(lpAddr,&MB,sizeof(MB));
				VirtualProtect(lpAddr, sizeof(DWORD),PAGE_READWRITE,&dwOldProtect);

				DWORD* lpNewProc = (DWORD*)MyMessageBox;
				::WriteProcessMemory(::GetCurrentProcess(),lpAddr,&lpNewProc,\
					sizeof(DWORD),NULL);
				VirtualProtect(lpAddr,sizeof(DWORD),dwOldProtect,0);
				return;
			}	
			pThunk++;
		}
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
				/*SetHook(GetModuleHandleA(NULL));*/
				MessageBox(NULL,"DLL载入成功","注意",MB_OK);
				
				SetHook(GetModuleHandleA(NULL));
				
				break;
			}
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			{
				SetHook(GetModuleHandleA(NULL));
				MessageBox(NULL,"DLL卸载成功","注意",MB_OK);
			}
				break;
	}   
    return TRUE;
}



// This is an example of an exported variable
HOOK_API int nHOOK=0;

// This is an example of an exported function.
HOOK_API int fnHOOK(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see HOOK.h for the class definition
CHOOK::CHOOK()
{ 
	return; 
}

