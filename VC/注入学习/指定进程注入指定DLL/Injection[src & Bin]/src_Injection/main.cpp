#include <windows.h>
#include <CommCtrl.h>

#include "InjectDlg.h"
#include "resource.h"

#pragma comment(lib, "comctl32.lib")


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int CmdShow)
{
	//³õÊ¼»¯¿Ø¼þ
	INITCOMMONCONTROLSEX icce = {0} ;
	
	icce.dwSize = sizeof (INITCOMMONCONTROLSEX) ;
	icce.dwICC = ICC_WIN95_CLASSES | ICC_LISTVIEW_CLASSES | ICC_BAR_CLASSES;

	InitCommonControlsEx(&icce);

	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DLGMAIN), NULL, DlgProc, NULL);

	return 0;
}