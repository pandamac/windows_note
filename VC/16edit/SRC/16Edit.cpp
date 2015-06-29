
#define  WIN32_LEAN_AND_MEAN
#include "16Edit.h"
#include "HexEditWnd.h"
#include "16EditDll.h"

#pragma comment(linker,"/BASE:0x1600000 /FILEALIGN:0x200 /MERGE:.rdata=.text /MERGE:.data=.text /SECTION:.text,EWR /IGNORE:4078")
#pragma pack(1)

//
// global variables
//
HexEditWnd             HEdit;
HINSTANCE              hInst;


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  fdwReason, 
                       LPVOID lpReserved
					 )
{
	switch(fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		hInst = (HINSTANCE)hModule;
		DisableThreadLibraryCalls((HMODULE)hModule);
		break;
	}

    return TRUE;
}

//
// HEdit routine stubs for exportation
//
BOOL __stdcall HEEnterWindowLoop()
{
	if (HEdit.bHEVisible)
		return FALSE;

	HEditWindowThread(NULL);

	return TRUE;
}

BOOL __stdcall HEEnterWindowLoopInNewThread()
{
	if (HEdit.bHEVisible)
		return FALSE; 

	HEdit.CreateMainWndThread();

	return TRUE;
}

BOOL __stdcall HESpecifySettings(PHE_SETTINGS ps)
{
	return HEdit.DoSpecifySettings(ps);
}