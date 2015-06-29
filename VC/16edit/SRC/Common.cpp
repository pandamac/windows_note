
#include "Common.h"

//
// constants
//
#define          CUT_CHAR             0x18
#define          COPY_CHAR            0x03
#define          PASTE_CHAR           0x16
#define          STRZ_CHAR            0x1a

//
// global variables
//

//
// window proc stub for hex editbox's
//
BOOL HookHexEditbox(HWND hEdit)
{
	if (SetWindowLong(
		hEdit,
		GWL_WNDPROC,
		(DWORD)HexOnlyEditProc))
		return TRUE;
	else
		return FALSE;
}

//
// call this with an edit box as argument to monitor the occurrence of the ENTER key
//
BOOL HookEditboxEnter(HWND hEdit)
{
	if (SetWindowLong(
		hEdit,
		GWL_WNDPROC,
		(DWORD)EditHookEnterProc))
		return TRUE;
	else
		return FALSE;
}

LRESULT __stdcall HexOnlyEditProc(HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam)
{
	CHAR      c = 0;
	char      cBuff[9], *pCH;
	DWORD     dw;
	WNDCLASS  wc;

	switch(Msg)
	{
	case WM_PASTE:
		// check whether the contents is a valid hex number string
		if (OpenClipboard(hWnd))
		{
			pCH = (char*)GetClipboardData(EnumClipboardFormats(CF_LOCALE)); // for win2k
			if (!pCH)
				pCH = (char*)GetClipboardData(EnumClipboardFormats(0));
			CloseClipboard();
			if (pCH)
				if (lstrcpyn(cBuff, pCH, sizeof(cBuff)))
					if (HexStrToInt(cBuff, &dw))
						break;			
		}
		return 0;

	case WM_KEYDOWN:
		// inform parent window via WM_COMMAND msg about ENTER key occurrence
		switch(wParam)
		{
		case VK_RETURN:
			SendMessage(
				GetParent(hWnd),
				WM_COMMAND,
				MAKEWPARAM(GetDlgCtrlID(hWnd), NM_HEXEDITENTER),
				(LPARAM)hWnd);
			return 0;

		case VK_ESCAPE:
			// if the control handles this key it'll close the parent dlg - dunno why :(
			return 0;
		}
		break;

	case WM_CHAR:
		// force hex characters
		if (wParam != VK_BACK    &&
		   wParam  != CUT_CHAR   && 
		   wParam  != PASTE_CHAR &&
		   wParam  != COPY_CHAR  &&
		   wParam  != STRZ_CHAR)         
		{
			c = toupper(wParam);
			if ( (c < '0' || c > '9') &&
				 (c < 'A' || c > 'F'))
				 return 0;
		}
		break;
	}

	// get default edit wnd proc
	GetClassInfo(NULL, "EDIT", &wc);

	return CallWindowProc(wc.lpfnWndProc, hWnd, Msg, wParam, lParam);
}

LRESULT __stdcall EditHookEnterProc(HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam)
{
	WNDCLASS  wc;

	switch(Msg)
	{
	case WM_KEYDOWN:
		// inform parent window via WM_COMMAND msg about ENTER key occurrence
		switch(wParam)
		{
		case VK_RETURN:
			SendMessage(
				GetParent(hWnd),
				WM_COMMAND,
				MAKEWPARAM(GetDlgCtrlID(hWnd), NM_HEXEDITENTER),
				(LPARAM)hWnd);
			return 0;

		case VK_ESCAPE:
			// if the control handles this key it'll close the parent dlg - dunno why :(
			return 0;
		}
		break;
	}

	// get default edit wnd proc
	GetClassInfo(NULL, "EDIT", &wc);

	return CallWindowProc(wc.lpfnWndProc, hWnd, Msg, wParam, lParam);
}

//
// convert a hex number string to a DW
//
BOOL HexStrToInt(char *szHexStr, DWORD *pdwHexVal)
{
	char   *pCH, c;
	DWORD  dwVal = 0, dw;

	pCH = szHexStr;
	while (*pCH)
	{
		c = toupper(*pCH++);
		if (c >= 'A' && c <= 'F')
			dw = (DWORD)c - ((DWORD)'A' - 10);
		else if (c >= '0' && c <= '9')
			dw = (DWORD)c - (DWORD)'0';
		else
			return FALSE; // invalid hex char
		dwVal = (dwVal << 4) + dw;
	}

	*pdwHexVal = dwVal;
	return TRUE;
}

/*
BOOL CopyTextData2Clipboard(void* pData, DWORD dwc)
{
	HANDLE   hMem;
	void*    pMem;

	if (!OpenClipboard(NULL))
		return FALSE; // FAILURE
	hMem = GlobalAlloc(GHND | GMEM_SHARE, dwc + 1);
	if (!hMem)
	{
		CloseClipboard();
		return FALSE; // FAILURE
	}
	pMem = GlobalLock(hMem);
	memcpy(pMem, pData, dwc);
	GlobalUnlock(hMem);
	EmptyClipboard();
	if (!SetClipboardData(CF_TEXT, hMem))
	{
		CloseClipboard();
		return FALSE; // FAILURE
	}

	CloseClipboard(); // Cleanup

	return TRUE; // SUCCESS
}
*/