
#include "16Edit.h"
#include <commctrl.h>
#include <stddef.h>
#include "HexEditWnd.h"
#include "File.h"
#include "Common.h"
#include "16EditDll.h"
#include "CPathString.h"
#include "WideChar.h"

//
// function prototypes
//
LRESULT  __stdcall HEditWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT  __stdcall TBHookProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL     __stdcall GotoDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL     __stdcall SelBlockDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL     __stdcall SearchDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
DWORD    __stdcall HEditWindowThread(void* pArg);

//
// constants
//
//const DWORD             dwMinusOne          = -1;

#define                 ID_TB               0x2000
#define                 ID_TRAYICON         0x2001

#define                 IDT_FIRST           0x1000
#define                 IDT_RESTORE         IDT_FIRST + 1
#define                 IDT_EXIT            IDT_FIRST + 2
#define                 WM_TRAYMENU         WM_USER   + IDT_FIRST

/*                      
#define                 TB_FIRST            0x500
#define                 TB_CLOSE            TB_FIRST +  1
#define                 TB_SAVE             TB_FIRST +  2
#define                 TB_GOTO             TB_FIRST +  3
#define                 TB_ABOUT            TB_FIRST +  4
#define                 TB_WIN2TOP          TB_FIRST +  5
#define                 TB_SEARCH           TB_FIRST +  6
#define                 TB_SELBLOCK         TB_FIRST +  7
#define                 TB_CUT              TB_FIRST +  8
#define                 TB_COPY             TB_FIRST +  9
#define                 TB_PASTE            TB_FIRST + 10
*/

// tb buttons
const TBBUTTON   TBbs[]       = 
{
	{  6, TB_WIN2TOP,        TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{  1, TB_GOTO,           TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{  8, TB_SELBLOCK,       TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{ 15, TB_SELALL,         TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{  7, TB_SEARCH,         TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{ 13, TB_SEARCHDOWN,     TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{ 12, TB_SEARCHUP,       TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{  0, NULL,              TBSTATE_ENABLED, (BYTE)TBSTYLE_SEP,    0, 0},
	{ 14, TB_UNDO,           TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{  2, TB_SAVE,           TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{  0, NULL,              TBSTATE_ENABLED, (BYTE)TBSTYLE_SEP,    0, 0},
	{  9, TB_CUT,            TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{ 10, TB_COPY,           TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{ 11, TB_PASTE,          TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{  0, NULL,              TBSTATE_ENABLED, (BYTE)TBSTYLE_SEP,    0, 0},
	{  3, TB_ABOUT,          TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0},
	{  0, TB_CLOSE,          TBSTATE_ENABLED, (BYTE)TBSTYLE_BUTTON, 0, 0}
};

#define					HEDIT_WND_CLASS     "16Edit Main Window"
#define					HEDIT_WND_TITLE     "[ 16Edit FX ]"
#define                 HEDIT_TRAY_TIP      "[16Edit]"
#define                 szMemoryBuff        "memory buffer"
#define                 HEDIT_WND_WIDTH     638
#define					HEDIT_WND_HEIGHT    400
#define                 SB_HEIGHT           20

#define                 INI_NAME            "16Edit.ini"
#define                 INI_SECTION         "Options"
#define                 INI_WINPOSKEY       "WinPos"

#define                 CF_16Edit           "CF_16EDIT"

#define                 STR_NO_MEM          "Not enough memory available !"

#define                 DEF_FONT_HEIGHT     17
#define                 DEF_FONT_WIDTH       8

#define                 RGB_BLACK           RGB(0, 0, 0)
#define                 RGB_RED             RGB(0xFF, 0, 0)
#define                 RGB_SBGRAY          RGB(0x40, 0x40, 0x40)

#define                 H2                  "%02lX"
#define                 H8                  "%08lX"
#define                 _H8                 "0x%08lX"

#define                 DEF_MAX_LINES       19 // A LOOK UP VALUE
#define                 PAIRS_X             (11 * uFontWidth)
#define                 DIGIT_PAIR_WIDTH    (3  * uFontWidth)
#define                 CHARS_X             (PAIRS_X + 16 * DIGIT_PAIR_WIDTH + uFontWidth)

#define                 WM_MOUSEWHEEL       0x020A
#define                 WHEEL_MOVE_STEPS    4

//
// global variables
//
BOOL               bStartCaretPos;
HE_POS             posStartCaretPos;

BOOL               bStartSel;
DWORD              dwStartSelStart;
DWORD              dwStartSelEnd;

BOOL               bStartOnTop;

BOOL               bParentWin;
HWND               hwndParent;

WNDPROC            pOrgTBWndProc;

HexEditWnd::HexEditWnd()
{
	LOGFONT lf;

	// save dll base
	hDllBase = GetModuleHandle(HEDIT_DLL_NAME);

	//
	// init some variables
	//
	iyHETop           = 0;
	iyHEBottom        = 0;
	bHEVisible        = FALSE;

	InitEdition();
	ZERO(search);

	// path/dir stuff
	GetModuleFileName( hInst, cInitialDir, sizeof(cInitialDir) );
	CPathString::PathToDir( cInitialDir );
	CPathString::ForceEndBackslash( cInitialDir);
	wsprintf(cIniPath, "%s"INI_NAME, cInitialDir);

	// create text font
	ZERO(lf);
	lf.lfHeight     = DEF_FONT_HEIGHT;
	lf.lfWidth      = DEF_FONT_WIDTH;
	lf.lfWeight     = FW_LIGHT;
	lstrcpy(lf.lfFaceName, "Courier New");
	hFont   = CreateFontIndirect(&lf);

	lf.lfUnderline  = TRUE;
	hFontU  = CreateFontIndirect(&lf);

	// create an own clipboard format
	cf16Edit = RegisterClipboardFormat(CF_16Edit);

	// build tray popup menu
	hmTray = CreatePopupMenu();
	AppendMenu(hmTray, MF_STRING, IDT_RESTORE, "restore");
	AppendMenu(hmTray, MF_STRING, IDT_EXIT, "exit");
}

HexEditWnd::~HexEditWnd()
{
	//
	// Cleanup
	//
	DeleteObject(hFont);
	DeleteObject(hFontU);

	DestroyMenu(hmTray);

	if (search.bInited)
	{
		if (search.pData)
			free(search.pData);
		if (search.pDlgStr)
			free(search.pDlgStr);
	}
}

void HexEditWnd::InitEdition()
{
	//
	// init some variables
	//
	ZERO(stat);
	ZERO(search);
	hMainWnd          = 0;
	diData.dwSize     = 0;
	bHEOnTop          = 0;
	uMaxLines         = DEF_MAX_LINES;
	bResizingAllowed  = FALSE;
	pActionHandler    = NULL;
	bStartCaretPos    = FALSE;
	bStartSel         = FALSE;
	bStartOnTop       = FALSE;
	bParentWin        = FALSE;
	bMinToTray        = FALSE;
	bSaveWinPos       = FALSE;
	bRestoreWinPos    = FALSE;
	bUserWinPos       = FALSE;

	return;
}

void HexEditWnd::QuitEdition()
{
	free(diData.pDataBuff);
	// diOrgData.pDataBuff is freeed by "FILE::~FILE"

	return;
}

LRESULT __stdcall HEditWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_SHOWWINDOW:
		HEdit.HEHandleWM_SHOWWINDOW(hWnd, uMsg, wParam, lParam);
		break;

	case WM_SIZE:
		HEdit.HEHandleWM_SIZE(hWnd, wParam, lParam);
		break;

	case WM_NOTIFY:
		if (HEdit.HEHandleWM_NOTIFY(hWnd, uMsg, wParam, lParam))
			return 0;
		break;

	case WM_MOVE:
		HEdit.HEHandleWM_MOVE(hWnd, wParam, lParam);
		break;

	case WM_PAINT:
		HEdit.PaintText(hWnd);
		break;

	case WM_KEYDOWN:
		HEdit.HEHandleWM_KEYDOWN(hWnd, uMsg, wParam, lParam);
		break;

	case WM_CHAR:
		return HEdit.HEHandleWM_CHAR(hWnd, uMsg, wParam, lParam);

	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
		HEdit.HEHandleLButton(hWnd, uMsg, wParam, lParam);
		break;

	case WM_MOUSEMOVE:
		HEdit.HEHandleWM_MOUSEMOVE(hWnd, uMsg, wParam, lParam);
		break;

	case WM_MOUSEWHEEL: // mouse scrolling support
		HEdit.HEHandleWM_MOUSEWHEEL(hWnd, uMsg, wParam, lParam);
		break;

	case WM_VSCROLL:
		HEdit.HEHandleWM_VSCROLL(hWnd, uMsg, wParam, lParam);
		break;

	case WM_SETFOCUS:
		HEdit.HEHandleWM_SETFOCUS(hWnd);
		break;
          
    case WM_KILLFOCUS:
		HEdit.HEHandleWM_KILLFOCUS(hWnd);
		break;

	case WM_COMMAND:
		HEdit.HEHandleWM_COMMAND(hWnd, uMsg, wParam, lParam);
		break;

	case WM_TRAYMENU:
		HEdit.HEHandleWM_TRAYMENU(hWnd, uMsg, wParam, lParam);
		return 0;

	case WM_CLOSE:
		HEdit.HEHandleWM_CLOSE(hWnd, uMsg, wParam, lParam);
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//
// report the main window procedure about WM_MOUSEMOVE to support scrolling up
// while selecting a block
//
LRESULT __stdcall TBHookProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_MOUSEMOVE:
		HEdit.HEHandleWM_MOUSEMOVE(hWnd, uMsg, wParam, lParam);
		break;

	case WM_LBUTTONUP:
		SendMessage(HEdit.GetHEditWndHandle(), uMsg, 0, 0);
		break;
	}

	return CallWindowProc(pOrgTBWndProc, hWnd, uMsg, wParam, lParam);
}

BOOL __stdcall GotoDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		HEdit.InitGotoDlg(hDlg);
		return TRUE;

	case WM_SHOWWINDOW:
		SetFocus( GetDlgItem(hDlg, GO_OFFSET) );
		return TRUE;

	case WM_COMMAND:
		return HEdit.GDHandleWM_COMMAND(hDlg, uMsg, wParam, lParam);

	case WM_CLOSE:
		EndDialog(hDlg, wParam);
		return TRUE;
	}

	return FALSE; // ERR
}

BOOL __stdcall SelBlockDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		HEdit.InitSelBlockDlg(hDlg);
		return TRUE;

	case WM_SHOWWINDOW:
		CheckDlgButton(hDlg, SB_RADIOEND, TRUE);
		SetFocus( GetDlgItem(hDlg, SB_START) );
		return TRUE;

	case WM_COMMAND:
		return HEdit.SBHandleWM_COMMAND(hDlg, uMsg, wParam, lParam);

	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	}

	return FALSE; // ERR
}

BOOL __stdcall SearchDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		HEdit.SSInitDlg(hDlg, uMsg, wParam, lParam);
		return TRUE;

	case WM_SHOWWINDOW:
		SetFocus( GetDlgItem(hDlg, SS_STR) );
		return TRUE;

	case WM_COMMAND:
		return HEdit.SSHandleWM_COMMAND(hDlg, uMsg, wParam, lParam);

	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	}

	return FALSE; // ERR
}

BOOL HexEditWnd::CreateMainWndThread()
{
	return GuiThread.Create(HEditWindowThread, NULL, 0);
}

DWORD __stdcall HEditWindowThread(void* pArg)
{
	WNDCLASS               wc;
	MSG                    msg;
	UINT                   icx, icy, ix, iy;
	HWND                   hWnd, hTB;
	RECT                   rct;
	HACCEL                 hAccel;
	HE_ACTION              act;
	HE_WIN_POS             wp;

	InitCommonControls();

	//
	// do window stuff
	//
	ZERO(wc);
	wc.style          = CS_HREDRAW | CS_VREDRAW;
	wc.hInstance      = HEdit.GetInstance();
	wc.hIcon          = LoadIcon(HEdit.GetInstance(), (PSTR)IDI_16Edit);
	wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszClassName  = HEDIT_WND_CLASS;
	wc.lpfnWndProc    = HEditWndProc;
	RegisterClass(&wc);

	icx = HEDIT_WND_WIDTH;
	icy = HEDIT_WND_HEIGHT;
	ix = (GetSystemMetrics(SM_CXFULLSCREEN) - icx) / 2;
	iy = (GetSystemMetrics(SM_CYFULLSCREEN) - icy) / 2;

	// create main wnd
	hWnd = CreateWindow(
		HEDIT_WND_CLASS,
		HEDIT_WND_TITLE,
		WS_OVERLAPPEDWINDOW | WS_VSCROLL,
		ix,
		iy,
		icx,
		icy,
		bParentWin ? hwndParent : 0,                  // HE_SET_PARENTWINDOW
		NULL,
		HEdit.GetInstance(),
		NULL);
	HEdit.SetHEditWndHandle(hWnd);

	// handle parent window state
	if (bParentWin)
		if (IsWindowEnabled(hwndParent))
			EnableWindow(hwndParent, FALSE);

	// create toolbar
	hTB = CreateToolbarEx(
		hWnd,
		WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT,
		ID_TB,
		16, // number of buttons in the bitmap
		NULL,
		(DWORD)LoadBitmap(HEdit.GetInstance(), (PSTR)IDB_TOOLBAR),
		(LPTBBUTTON)&TBbs,
		17, // number of buttons to create
		16,
		16,
		16,
		16,
		sizeof(TBBUTTON));
	// save TB information
	HEdit.SetTBHandle(hTB);
	GetClientRect(hTB, &rct);
	HEdit.iyHETop = rct.bottom - rct.top + 2;

	// hook TB
	pOrgTBWndProc = (WNDPROC)SetWindowLong(
		hTB,
		GWL_WNDPROC,
		(DWORD)TBHookProc);

	// load accelerator
	hAccel = LoadAccelerators(HEdit.GetInstance(), (PSTR)IDR_ACCEL);

	// show wnd
	ShowWindow(hWnd, SW_SHOWNORMAL);
	UpdateWindow(hWnd);
	
	//
	// handle HE_SET_USERWINDOWPOSITION/HE_SET_RESTOREWINDOWPOSITION
	//
	if ( HEdit.bUserWinPos )
	{
		wp = HEdit.wpUser;
		SetWindowPos( hWnd, HWND_NOTOPMOST, wp.ix, wp.iy, wp.icx, wp.icy, SWP_SHOWWINDOW );
		ShowWindow( hWnd, wp.iState );
	}
	else if ( HEdit.bRestoreWinPos )
	{
		if ( GetPrivateProfileStruct(INI_SECTION, INI_WINPOSKEY, &wp, sizeof(wp), HEdit.cIniPath) )
		{
			SetWindowPos(
				hWnd,
				HWND_NOTOPMOST,
				wp.ix, wp.iy, wp.icx, wp.icy, 
				SWP_SHOWWINDOW);
			ShowWindow(hWnd, wp.iState);
		}
	}

	//
	// handle specified caret/sel settings
	//
	HEdit.HandleStartCaretPosSel(hWnd);

	//
	// HE_SET_ONTOP ?
	//
	if (bStartOnTop)
		SendMessage(hWnd, WM_COMMAND, TB_WIN2TOP, 0);

	HEdit.bHEVisible = TRUE;

	//
	// inform about HE_ACTION_WINDOWCREATED
	//
	if (HEdit.pActionHandler)
	{
		ZERO(act);
		act.dwActionCode   = HE_ACTION_WINDOWCREATED;
		act.hwnd16Edit     = hWnd;
		HEdit.pActionHandler(&act);		
	}

	//
	// enter win loop
	//
	while(GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(hWnd, hAccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}

HWND HexEditWnd::GetHEditWndHandle()
{
	return hMainWnd;
}

void HexEditWnd::SetHEditWndHandle(HWND hWnd)
{
	hMainWnd = hWnd;

	return;
}

void HexEditWnd::SetTBHandle(HWND hTB)
{
	this->hTB = hTB;

	return;
}

HINSTANCE HexEditWnd::GetInstance()
{
	return hDllBase;
}

void HexEditWnd::SetVisible(BOOL bVisible)
{
	if (bVisible)
	{
		ShowWindow(hMainWnd, SW_SHOWNORMAL);
		UpdateWindow(hMainWnd);
	}
	else
		ShowWindow(hMainWnd, SW_HIDE);

	return;
}

BOOL HexEditWnd::SetHEDataInfo(PHE_DATA_INFO pDI)
{	
	void* p;

	// cleanup file map
	fInput.Destroy();

	// copy data mem
	p = malloc(pDI->dwSize);
	if (!p)
		return FALSE; // ERR
	memcpy( 
		p,
		pDI->pDataBuff,
		pDI->dwSize);

	// save infos
	diData = diOrgData = *pDI;
	diData.pDataBuff = p;

	// set title
	SetHEWndCaption();

	return TRUE; // OK
}

BOOL HexEditWnd::DoEditFile(char* szFilePath, BOOL bForceReadOnly)
{
	BOOL          bRet;
	void          *p;
	HE_ACTION     act;

	// cleanup file map
	fInput.Destroy();

	// get file handle
	if (bForceReadOnly)
		bRet = fInput.GetFileHandle(szFilePath, F_OPENEXISTING_R);
	else
		bRet = fInput.GetFileHandleWithMaxAccess(szFilePath);
	if (!bRet)
		return FALSE; // FAILURE

	// map file
	if (!fInput.MapFile())
		return FALSE; // FAILURE

	// fill diData
	diOrgData.bReadOnly  = fInput.IsFileReadOnly();
	diOrgData.dwSize     = fInput.GetFSize();
	diOrgData.pDataBuff  = fInput.GetMapPtr();

	// make a copy of this memory block
	p = malloc(diOrgData.dwSize);
	if (!p)
		return FALSE; // ERR
	memcpy(
		p,
		diOrgData.pDataBuff,
		diOrgData.dwSize);
	diData = diOrgData;
	diData.pDataBuff = p;

	// set title
	SetHEWndCaption();

	//
	// inform about HE_ACTION_FILEACCESSINFO
	//
	if (pActionHandler)
	{
		ZERO(act);
		act.dwActionCode  = HE_ACTION_FILEACCESSINFO;
		act.bReadWrite    = !fInput.IsFileReadOnly();
		pActionHandler(&act);
	}

	return TRUE; // OK
}

//
// main painting routine
// (back-buffering methode)
//
BOOL HexEditWnd::PaintText(HWND hWnd)
{
	HDC           hDC, hdcBuff;
	DWORD         dwOffset;
	UINT          u, u2, icySel;
	char          cBuff[40];
	BYTE          byCur;
	RECT          rct;
	HBITMAP       hBmp;
	HBRUSH        hbrColor;
	HGDIOBJ       hobjOld;
	PAINTSTRUCT   ps;

	// sth to paint ?
	if (!diData.dwSize)
		return FALSE; // ERR

	//
	// paint
	//
	hDC      = BeginPaint(hWnd, &ps);
	
	// create virtual bmp
	GetClientRect(hWnd, &rct);
	hdcBuff  = CreateCompatibleDC(hDC);
	hBmp     = CreateCompatibleBitmap(hDC,
		rct.right - rct.left,
		rct.bottom - rct.top - iyHETop);
	hobjOld = SelectObject(hdcBuff, hBmp);

	// fill bmp
	hbrColor = CreateSolidBrush( (COLORREF)0x00FFFFFF );
	FillRect(hdcBuff, &rct, hbrColor);
	DeleteObject(hbrColor);

	// paint to bmp
	dwOffset = stat.dwCurOffset;

	for (u = 0; u < uMaxLines; u++)
	{
		// end of buffer ?
		if (dwOffset >= diData.dwSize)
			break;

		//
		// paint offset
		//
		SetTextColor(hdcBuff, RGB_BLACK);
		wsprintf(cBuff, H8":", dwOffset);
		SelectObject(hdcBuff, hFont);
		TextOut(hdcBuff,
			0, u * uFontHeight,
			cBuff, 9);

		//
		// paint digit pairs
		//
		for (u2 = 0; u2 < 16; u2++)
		{
			// end of buffer ?
			if (dwOffset >= diData.dwSize)
				break; // upper, same structured decision handles painting end

			//
			// change at this position?
			//
			byCur = *(BYTE*)((DWORD)diData.pDataBuff + dwOffset);

			if (dwOffset >= diOrgData.dwSize) // out of range of original buffer ?
				SetTextColor(hdcBuff, RGB_RED);
			else
				SetTextColor(
					hdcBuff,
					*(BYTE*)((DWORD)diOrgData.pDataBuff + dwOffset) != byCur ? RGB_RED : RGB_BLACK);

			//
			// paint digit pair
			//
			if (stat.bCaretVisible &&
				stat.posCaret.dwOffset == dwOffset &&
				stat.posCaret.bTextSection)
				SelectObject(hdcBuff, hFontU);
			else
				SelectObject(hdcBuff, hFont);

			wsprintf(cBuff, H2, byCur);
			TextOut(hdcBuff,
				PAIRS_X + DIGIT_PAIR_WIDTH * u2, u * uFontHeight,
				cBuff, 2);

			//
			// paint character
			//
			if (stat.bCaretVisible &&
				stat.posCaret.dwOffset == dwOffset &&
				!stat.posCaret.bTextSection)
				SelectObject(hdcBuff, hFontU);
			else
				SelectObject(hdcBuff, hFont);

			if (byCur < 0x20)
				lstrcpy(cBuff, ".");
			else
				wsprintf(cBuff, "%c", byCur);
			TextOut(hdcBuff,
				CHARS_X + u2 * uFontWidth, u * uFontHeight,
				cBuff, 1);

			//
			// draw sel
			//
			if (stat.bSel &&
				dwOffset >= stat.dwOffSelStart &&
				dwOffset <= stat.dwOffSelEnd)
			{
				if (dwOffset == stat.dwOffSelEnd ||
					dwOffset % 16 == 15)
					icySel = uFontWidth * 2;
				else
					icySel = DIGIT_PAIR_WIDTH;

				// sel pair
				BitBlt(hdcBuff,
					PAIRS_X + u2 * DIGIT_PAIR_WIDTH,
					u * uFontHeight,
					icySel,
					uFontHeight,
					hdcBuff,
					PAIRS_X + u2 * DIGIT_PAIR_WIDTH,
					u * uFontHeight,
					NOTSRCCOPY);

				// sel char
				BitBlt(hdcBuff,
					CHARS_X + u2 * uFontWidth,
					u * uFontHeight,
					uFontWidth,
					uFontHeight,
					hdcBuff,
					CHARS_X + u2 * uFontWidth,
					u * uFontHeight,
					NOTSRCCOPY);
			}

			// adjust vars
			++dwOffset;
		}
	}

	//
	// SB stuff
	//

	// paint SB text
	SetTextColor(hdcBuff, RGB_SBGRAY);
	SelectObject(hdcBuff, hFont);
	TextOut(hdcBuff, 2, rct.bottom - SB_HEIGHT - 28, cSBText, lstrlen(cSBText) );

	// copy buffer content to client area
	BitBlt(hDC,
		rct.left,
		rct.top + iyHETop,
		rct.right - rct.left,
		rct.bottom - rct.top - iyHETop,
		hdcBuff,
		0, 0, SRCCOPY);

	// cleanup
	SelectObject(hdcBuff, hobjOld);
	DeleteDC(hdcBuff);
	DeleteObject(hBmp);

	EndPaint(hWnd, &ps);

	return TRUE; // OK
}

//
// paints only one offset
// (used to avoid flickering)
//
/*
BOOL HexEditWnd::PaintOffset(HWND hWnd, PHE_POS ppos)
{
	HDC           hDC;
	PAINTSTRUCT   ps;
	DWORD         dwDelta;
	UINT          uLine, uPair;
	char          cBuff[10];
	BYTE          *pby;
	RECT          rct;

	// sth to paint ?
	if (!diData.dwSize)
		return FALSE; // ERR

	// in visible range ?
	if (!IsOffsetVisible(ppos->dwOffset))
		return FALSE; // ERR

	//
	// paint
	//
	pby = (BYTE*)((DWORD)diData.pDataBuff + ppos->dwOffset);
	dwDelta = ppos->dwOffset - stat.dwCurOffset;
	uLine   = dwDelta / uFontHeight;
	uPair   = dwDelta % 16;	

	//hDC = BeginPaint(hWnd, &ps);
	//SetBkMode(hDC, TRANSPARENT);

	// pair
	if (stat.bCaretVisible &&
		stat.posCaret.dwOffset == ppos->dwOffset &&
		stat.posCaret.bTextSection)
		SelectObject(hDC, hFontU);
	else
		SelectObject(hDC, hFont);
	wsprintf(cBuff, H2, *pby);
	TextOut(hDC,
		PAIRS_X + uPair * DIGIT_PAIR_WIDTH,
		uLine * uFontHeight,
		cBuff, 2);

	// character
	if (stat.bCaretVisible &&
		stat.posCaret.dwOffset == ppos->dwOffset &&
		!stat.posCaret.bTextSection)
		SelectObject(hDC, hFontU);
	else
		SelectObject(hDC, hFont);
	wsprintf(cBuff, "%c", *pby);
	TextOut(hDC,
		CHARS_X + uPair * uFontWidth,
		uLine * uFontHeight,
		cBuff, 1);

	//
	// invalidate region at pair and char
	//
	ValidateRect(hWnd, NULL);
	rct.left   = PAIRS_X + uPair * DIGIT_PAIR_WIDTH;
	rct.right  = PAIRS_X + uPair * DIGIT_PAIR_WIDTH + 2 * uFontWidth;
	rct.top    = uLine * uFontHeight;
	rct.bottom = (uLine + 1) * uFontHeight;
	InvalidateRect(hWnd, &rct, TRUE);

	rct.left   = CHARS_X + uPair * uFontWidth;
	rct.right  = CHARS_X + (uPair + 1) * uFontWidth;
	rct.top    = uLine * uFontHeight;
	rct.bottom = (uLine + 1) * uFontHeight;
	InvalidateRect(hWnd, &rct, TRUE);

	//PaintText(hWnd);

	//EndPaint(hWnd, &ps);

	return TRUE;
}
*/

void HexEditWnd::HEHandleWM_SETFOCUS(HWND hWnd)
{
	//
	// handle caret stuff
	//
	CreateCaret(hWnd, NULL, uFontWidth, uFontHeight);
	SetCaretSet(TRUE);
	if (stat.bCaretPosValid)
		SetCaret(&stat.posCaret);

	return;
}
void HexEditWnd::HEHandleWM_KILLFOCUS(HWND hWnd)
{
	//
	// handle caret stuff
	//
	if (!stat.bCaretVisible)
		HideCaret(hWnd);
    DestroyCaret();
	SetCaretSet(FALSE);
	stat.bCaretVisible = FALSE;

	return;
}

//
// save new caret pos and repaints
//
// returns:
// FALSE - mainly if the caret was hidden because it's not in the current visible range
//
BOOL HexEditWnd::SetCaret(PHE_POS ppos)
{
	BOOL     bRet = FALSE;
	DWORD    dwOffDelta;
	UINT     uxPair, uyLine, ux;

	if (IsOutOfRange(ppos))
		return FALSE; // ERR

	SetCaretPosData(ppos);

	if (stat.bSel)
		return FALSE; // ERR

	// new pos in current range?
	if (!IsOffsetVisible(ppos->dwOffset))
	{
		if (stat.bCaretVisible)
			HideCaret(hMainWnd);
		stat.bCaretVisible = FALSE;
		return FALSE; // ERR
	}

	dwOffDelta = ppos->dwOffset - stat.dwCurOffset;
	uyLine = dwOffDelta / 16;
	uxPair = dwOffDelta % 16;

	// caret in the text section ?
	if (ppos->bTextSection)
	{
		bRet = SetCaretPos(
			CHARS_X + uFontWidth * uxPair,
			uyLine * uFontHeight + iyHETop);
	}
	else
	{
		ux = PAIRS_X + uxPair * DIGIT_PAIR_WIDTH;
		if (!ppos->bHiword)
			ux += uFontWidth;
		bRet = SetCaretPos(
			ux,
			uyLine * uFontHeight + iyHETop);
	}
	if (bRet)
	{
		if (!stat.bCaretVisible)
			ShowCaret(hMainWnd);
		stat.bCaretVisible   = TRUE;
		stat.bCaretPosValid  = TRUE;

		SetCaretSelInfoToStatus();
		RepaintClientArea();

		return TRUE; // OK
	}
	else
		return FALSE; // ERR
}

//
// overloaded
//
BOOL HexEditWnd::SetCaret(DWORD dwOffset)
{
	HE_POS  posNew;

	if (IsOutOfRange(dwOffset))
		return FALSE; // ERR

	posNew.bHiword      = TRUE;
	posNew.bTextSection = FALSE;
	posNew.dwOffset     = dwOffset;

	return SetCaret(&posNew);
}

//
// overloaded
//
BOOL HexEditWnd::SetCaret()
{
	return SetCaret(&stat.posCaret);
}

//
// checks whether an Offset is currently visible in the GUI
//
BOOL HexEditWnd::IsOffsetVisible(DWORD dwOffset)
{
	DWORD dwBytes2C;

	// out of mem range ?
	if (dwOffset >= diData.dwSize)
		return FALSE; // ERR

	// out of visible range ?
	dwBytes2C = 16 * uMaxLines;
	if (dwBytes2C + stat.dwCurOffset > diData.dwSize)
		dwBytes2C = diData.dwSize - stat.dwCurOffset;

	if (dwOffset >= stat.dwCurOffset &&
		dwOffset <  stat.dwCurOffset + dwBytes2C)
		return TRUE; // OK
	else
		return FALSE; // ERR
}

BOOL HexEditWnd::HESetFont(HFONT hf)
{
	TEXTMETRIC  tm;
	HDC         hDC;
	BOOL        bRet = FALSE;

	hDC = GetDC(hMainWnd);
	if (!hDC)
		return FALSE; // FAILURE
	SelectObject(hDC, hf);
	if (!GetTextMetrics(hDC, &tm))
		goto Exit; // FAILURE

	uFontHeight = tm.tmHeight;
	uFontWidth  = tm.tmAveCharWidth;

	SendMessage(hMainWnd, WM_SETFONT, (WPARAM)HEdit.hFont, 0);

Exit:
	ReleaseDC(hMainWnd, hDC);

	return bRet; // EXIT
}

void HexEditWnd::HEHandleWM_SIZE(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT              rct;
	UINT              uWidth, uHeight;

	// save pos
	if ( wParam != SIZE_MINIMIZED &&
		 wParam != SIZE_MAXIMIZED)
		 GetWindowRect( hWnd, &rctLastPos );

	//
	// handle HE_SET_MINIMIZETOTRAY
	//
	if (bMinToTray)
	{
		if (wParam == SIZE_MINIMIZED)
		{
			HEditToTray();
			return; // RET
		}
	}

	// wnd size info -> vars
	uWidth = LOWORD(lParam);
	uHeight = HIWORD(lParam); 

	if (uFontHeight) // avoid division through 0
	{
		// calc max lines
		uMaxLines = (uHeight - iyHETop - SB_HEIGHT) / uFontHeight;

		// bottom of HE
		GetClientRect(hWnd, &rct);
		HEdit.iyHEBottom = rct.bottom - SB_HEIGHT;

		// get HE rect
		rctHE.top     = iyHETop;
		rctHE.bottom  = iyHEBottom;
		rctHE.left    = 0;
		rctHE.right   = CHARS_X + 16 * uFontWidth;
	}

	// resize TB
	SendMessage(hTB, TB_AUTOSIZE, 0, 0);

	return; // RET
}

//
// install tray icon + hide main win
//
BOOL HexEditWnd::HEditToTray()
{
	char *pch;

	// add tray icon
	ZERO(nidTray);

	// build tip
	nidTray.cbSize            = sizeof(nidTray);

	lstrcpy(nidTray.szTip, HEDIT_TRAY_TIP);
	if ( IsFileInput() )
	{
		pch = CPathString::ExtractFileName( fInput.GetFilePath() );
		if (pch)
			if (sizeof(nidTray.szTip) - sizeof(HEDIT_TRAY_TIP) - 3 >= lstrlen(pch) )
			{
				lstrcat(nidTray.szTip, " - ");
				lstrcat(nidTray.szTip, pch);
				lstrlen(nidTray.szTip);
			}
	}
	else
	{
		lstrcat(nidTray.szTip, " - ");
		lstrcat(nidTray.szTip, szMemoryBuff);
	}

	nidTray.hWnd              = hMainWnd;
	nidTray.uID               = ID_TRAYICON;
	nidTray.uFlags            = NIF_TIP | NIF_ICON | NIF_MESSAGE;
	nidTray.hIcon             = (HICON)GetClassLong(hMainWnd, GCL_HICON);
	nidTray.uCallbackMessage  = WM_TRAYMENU;
	if (!Shell_NotifyIcon(NIM_ADD, &nidTray))
		return FALSE; // ERR

	// hide win
	ShowWindow(hMainWnd, SW_HIDE);

	return TRUE; // OK
}

BOOL HexEditWnd::HEditKillTrayIcon()
{
	return Shell_NotifyIcon(NIM_DELETE, &nidTray);
}

//
// kill tray icon + show main win
//
BOOL HexEditWnd::HEditReturnFromTray()
{
	// kill tray icon
	if (!HEditKillTrayIcon())
		return FALSE; // ERR

	// show win
	ShowWindow(hMainWnd, SW_RESTORE);
	HEditToTop();

	return TRUE; // OK
}

BOOL HexEditWnd::HEHandleLButton(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	POINT   pClick;
	HE_POS  pos;
	DWORD   dwOldOff;
	BOOL    bSel;

	pClick.x = LOWORD(lParam);
	pClick.y = HIWORD(lParam);

	switch(uMsg)
	{
	case WM_LBUTTONDOWN:
		if (!PointToPos(&pClick, &pos))
		{
			stat.bLastLBDownPos = FALSE;
			return FALSE; // ERR
		}

		stat.bMouseSelecting = TRUE;
		dwOldOff = stat.posCaret.dwOffset;
		stat.posLastLButtonDown = pos;
		stat.bLastLBDownPos = TRUE;
		bSel = KillSelection();
		SetCaret(&pos);
		if (bSel)
			break;

		// handle shift key actions ?
		if ( TESTBIT(wParam, MK_SHIFT) && stat.bCaretPosValid )
			SetSelection( dwOldOff, pos.dwOffset );
		break;

	case WM_LBUTTONUP:
		stat.bMouseSelecting = FALSE;
		break;
	}

	return TRUE; // OK
}

//
// converts a POINT struct to a HE_POS struct
//
// returns:
// FALSE - if POINT is neither in the pair field nor in the text field
//
BOOL HexEditWnd::PointToPos(IN POINT *pp, OUT PHE_POS ppos)
{
	UINT  uLine, uPair, uxCurPair;

	memset(ppos, 0, sizeof(HE_POS));

	// in digit pair field ?
	if ((DWORD)pp->x >= PAIRS_X &&
		(DWORD)pp->x <  PAIRS_X + 16 * DIGIT_PAIR_WIDTH &&
		(DWORD)pp->y >= iyHETop &&
		(DWORD)pp->y <  iyHETop + uMaxLines * uFontHeight)
	{
		uLine =  ((DWORD)pp->y - iyHETop) / uFontHeight;
		uPair =  pp->x - PAIRS_X;
		uPair /= DIGIT_PAIR_WIDTH;

		// x in space between digit pairs ?
		uxCurPair = PAIRS_X + uPair * DIGIT_PAIR_WIDTH; // -> x pos of cur pair
		if ((UINT)pp->x > uxCurPair + 2*uFontWidth)
		{
			// last pair of the line ?
			if (uPair == 0xF)
				return FALSE; // ERR

			// autosel next pair
			if (pp->x - uxCurPair - 2*uFontWidth > uFontWidth / 2)
			{
				++uPair;
				uxCurPair += DIGIT_PAIR_WIDTH;
			}
		}

		// out of range?
		if (IsOutOfRange(stat.dwCurOffset + uLine * 16 + uPair))
			return FALSE; // ERR		

		// x -> LOWORD ?
		ppos->bHiword = ((UINT)pp->x > uxCurPair + uFontWidth) ? FALSE: TRUE;

		// save offset
		ppos->dwOffset = stat.dwCurOffset + uLine * 16 + uPair;
		
		return TRUE; // OK
	}
	// in text field ?
	else if ((DWORD)pp->x >= CHARS_X &&
		     (DWORD)pp->x <  CHARS_X + uFontWidth * 16 &&
			 //(DWORD)pp->y < uMaxLines * uFontHeight + iyHETop)
			 (DWORD)pp->y >= iyHETop &&
			 (DWORD)pp->y <  iyHETop + uMaxLines * uFontHeight)
	{
		uLine =  ((DWORD)pp->y - iyHETop) / uFontHeight;
		uPair =  (UINT)pp->x - CHARS_X;
		uPair /= uFontWidth;

		// out of range?
		if (IsOutOfRange(stat.dwCurOffset + uLine * 16 + uPair))
			return FALSE; // ERR

		// build output
		ppos->bTextSection = TRUE;
		ppos->bHiword      = TRUE;
		ppos->dwOffset     = stat.dwCurOffset + uLine * 16 + uPair;

		return TRUE; // OK
	}
	else
		return FALSE; // ERR
}

//
// cursor key,... handling
//
BOOL HexEditWnd::HEHandleWM_KEYDOWN(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HE_POS  posNew;
	int     iNewLine;
	DWORD   dwOff;

	// caret already set
	if (!stat.bCaretPosValid)
		return FALSE; // ERR

	//
	// handle special keys
	//

	// tab key
	switch(wParam)
	{
	case VK_TAB:
		// switch between text/data fields
		if (stat.bCaret)
		{
			// kill sel
			if (stat.bSel)
				KillSelection();

			// reset caret
			stat.posCaret.bTextSection ^= TRUE;
			stat.posCaret.bHiword       = TRUE;
			SetCaret();

			// make visible if is not
			if (!IsOffsetVisible(stat.posCaret.dwOffset))
				SetTopLine();

			return TRUE; // OK
		}
		else
			return FALSE; // ERR
	}

	// shift key
	if ( IsKeyDown(VK_SHIFT) )
	{
		posNew = stat.posCaret;
		switch(wParam)
		{
		case VK_LEFT:    // left
			posNew.dwOffset--;
			break;

		case VK_RIGHT:   // right
			posNew.dwOffset++;
			break;

		case VK_NEXT:    // page down
			posNew.dwOffset += uMaxLines * 16;
			break;

		case VK_PRIOR:   // page up
			posNew.dwOffset -= uMaxLines * 16;
			break;

		case VK_DOWN:    // down
			posNew.dwOffset += 16;
			break;

		case VK_UP:      // up
			posNew.dwOffset -= 16;
			break;

		default:
			return FALSE; // ERR
		}

		// validate
		if (ValidatePos(&posNew))
			Beep();

		if (stat.bSel)
		{
			if (stat.dwOffSelStart == stat.posCaret.dwOffset)
				dwOff = stat.dwOffSelEnd;
			else
				dwOff = stat.dwOffSelStart;

			SetSelection( dwOff, posNew.dwOffset);
		}
		else
			SetSelection( stat.posCaret.dwOffset, posNew.dwOffset );

		SetCaret(&posNew);
		MakeCaretVisible();

		return TRUE; // OK
	}

	//
	// move caret in HE area / move current line
	//
	posNew    = stat.posCaret;
	iNewLine  = GetCurrentLine();
	switch(wParam)
	{
	case VK_NEXT:    // page down
		posNew.dwOffset += uMaxLines * 16;
		iNewLine += uMaxLines;
		break;

	case VK_PRIOR:   // page up
		posNew.dwOffset -= uMaxLines * 16;
		iNewLine -= uMaxLines;
		break;

	case VK_DOWN:    // down
		posNew.dwOffset += 16;
		++iNewLine;
		break;

	case VK_UP:      // up
		posNew.dwOffset -= 16;
		--iNewLine;
		break;

	case VK_RIGHT:   // rigth
		if (stat.posCaret.bTextSection)
		{
			++posNew.dwOffset;
			posNew.bHiword = TRUE;
		}
		else
		{
			posNew.bHiword ^= 1;	
			if (!stat.posCaret.bHiword)
				++posNew.dwOffset;
		}
		break;

	case VK_LEFT:    // left
		if (stat.posCaret.bTextSection)
		{
			--posNew.dwOffset;
			posNew.bHiword = TRUE;
		}
		else
		{
			posNew.bHiword ^= 1;
			if (stat.posCaret.bHiword)
				--posNew.dwOffset;
		}
		break;

	case VK_BACK:
		if (stat.posCaret.bTextSection)
			--posNew.dwOffset;
		break;

	default:
		return FALSE; // ERR
	}

	// changes -> GUI
	if (stat.bSel &&
		wParam != VK_RIGHT && 
		wParam != VK_LEFT)
	{
		// validate
		if (ValidateLine(&iNewLine))
			Beep();

		// set
		SetCurrentLine(iNewLine);
	}
	else
	{
		// validate
		if (ValidatePos(&posNew))
			Beep();

		// set
		KillSelection();
		SetCaret(&posNew);
		MakeCaretVisible();
	}

	return TRUE; // OK
}

//
// if caret isn't in the visible area, the top line is reset
//
// returns:
// whether it was needed to reset the top line
//
BOOL HexEditWnd::MakeCaretVisible()
{
	DWORD dwLastVisibleOff;

	if (IsOffsetVisible(stat.posCaret.dwOffset))
		return FALSE; // ERR

	dwLastVisibleOff = __min(uMaxLines * 16 + stat.dwCurOffset,
		                     diData.dwSize);
	if (stat.posCaret.dwOffset < stat.dwCurOffset) // caret above ?
		SetTopLine((int)(stat.posCaret.dwOffset / 16));
	else // caret below
		SetTopLine((int)(stat.posCaret.dwOffset / 16 - uMaxLines + 1));

	return TRUE; // OK
}

//
// corrects the information in a given HE_POS structure if it's out of range
//
// returns:
// whether sth was fixed
//
BOOL HexEditWnd::ValidatePos(PHE_POS ppos)
{
	if ((int)ppos->dwOffset < 0)
	{
		ppos->dwOffset = 0;
		ppos->bHiword  = TRUE;
		return TRUE; // OK
	}
	else if (ppos->dwOffset >= diData.dwSize)
	{
		ppos->dwOffset = diData.dwSize - 1;
		ppos->bHiword  = FALSE;
		return TRUE; // OK
	}
	else
		return FALSE; // ERR
}

//
// returns:
// last caret status
//
BOOL HexEditWnd::SetCaretSet(BOOL bSet)
{
	BOOL bRet;

	bRet = stat.bCaret;
	stat.bCaret = bSet;

	return bRet;
}

//
// set the V scrollbar range relative to "diData.dwSize"
//
void HexEditWnd::SetupVScrollbar()
{
	DWORD dwTotalLines = GetTotalLineNum();

	SetScrollRange(hMainWnd, SB_VERT, 0, (int)(dwTotalLines - 1), TRUE);

	return;
}

UINT HexEditWnd::GetTotalLineNum()
{
	DWORD dwTotalLines;

	dwTotalLines = diData.dwSize / 16;
	if (diData.dwSize % 16)
		++dwTotalLines;

	return dwTotalLines;
}

//
// handles V scrollbar movement
//
BOOL HexEditWnd::HEHandleWM_VSCROLL(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int          nPos;
	SCROLLINFO   scrollinfo = { sizeof(SCROLLINFO) };

	nPos = GetScrollPos(hWnd, SB_VERT);

	switch(LOWORD(wParam))
	{
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		// get the tracking position
		scrollinfo.fMask = SIF_TRACKPOS;
		GetScrollInfo(hWnd, SB_VERT, &scrollinfo);
		nPos = scrollinfo.nTrackPos;
		break;

	case SB_TOP:
		nPos = 0;
		break;

	case SB_BOTTOM:
		nPos = GetTotalLineNum() - 1;
		break;

	case SB_LINEDOWN:
		++nPos;
		break;

	case SB_LINEUP:
		--nPos;
		break;

	case SB_PAGEDOWN:
		nPos += uMaxLines;		
		break;

	case SB_PAGEUP:
		nPos -= uMaxLines;
		break;		
	}

	return SetTopLine((int)nPos);
}

//
// sets the line to the top of the visible data range
//
BOOL HexEditWnd::SetTopLine(int iNewLine)
{
	int nNewPos;

	nNewPos = iNewLine;

	// validation
	if (nNewPos < 0)
		nNewPos = 0;
	nNewPos = __min(nNewPos, (int)GetTotalLineNum() - 1);

	// avoid repainting ?
	if (nNewPos == stat.iLastLine)
		return TRUE; // OK

	// set new line
	SetScrollPos(hMainWnd, SB_VERT, nNewPos, TRUE);
	stat.iLastLine = nNewPos;

	// set new offset
	stat.dwCurOffset = nNewPos * 16;

	// reset caret
	if (stat.bCaretPosValid)
		SetCaret();

	// repaint
	RepaintClientArea();

	return TRUE; // OK
}

//
// overloaded
//
BOOL HexEditWnd::SetTopLine(DWORD dwOffset)
{
	return SetTopLine( (int)dwOffset / 16);
}

//
// overloaded
//
BOOL HexEditWnd::SetTopLine()
{
	return SetTopLine((DWORD)stat.posCaret.dwOffset);
}

void HexEditWnd::RepaintClientArea()
{
	InvalidateRect(hMainWnd, NULL, FALSE);

	return;
}

void HexEditWnd::RepaintClientAreaNow()
{
	RepaintClientArea();
	UpdateWindow(hMainWnd);

	return;
}

void HexEditWnd::HEHandleWM_MOUSEWHEEL(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int         nPos;
	short int   iDelta;

	iDelta = (int)HIWORD(wParam);
	nPos = GetScrollPos(hWnd, SB_VERT);
	if (iDelta > 0)
	{
		// move up	
		nPos -= WHEEL_MOVE_STEPS;
	}
	else
	{
		// move down
		nPos += WHEEL_MOVE_STEPS;
	}

	SetTopLine((int)nPos);

	return;
}

void HexEditWnd::HEHandleWM_SHOWWINDOW(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HEdit.HESetFont(HEdit.hFont);
	SetupVScrollbar();
	ConfigureTB();
	SetHEWndCaption();

	return;
}

void HexEditWnd::Beep()
{
	MessageBeep(MB_ICONEXCLAMATION);

	return;
}

BOOL HexEditWnd::HEHandleWM_COMMAND(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case TB_GOTO:
		if (DialogBoxParam(	GetInstance(), (PSTR)IDD_GOTO, hWnd, GotoDlgProc, 0) &&
			stat.bCaretPosValid)
		{
			SetTopLine();
			SetCaret(&stat.posCaret);
		}
		return TRUE;

	case TB_SELBLOCK:
		DialogBoxParam( GetInstance(), (PSTR)IDD_SELBLOCK, hWnd, SelBlockDlgProc, 0);
		return TRUE;

	case TB_SELALL:
		SelectAll();
		return TRUE;

	case TB_SEARCH:
		DialogBoxParam( GetInstance(), (PSTR)IDD_SEARCH, hWnd, SearchDlgProc, 0);
		return TRUE;

	case TB_SEARCHDOWN:
	case TB_SEARCHUP:
		PerformSearchAgain(&search, LOWORD(wParam) == TB_SEARCHDOWN ? TRUE : FALSE);
		return TRUE;

	case TB_WIN2TOP:
		SetHEWnd2Top( bHEOnTop ^ 1 );
		ConfigureTB();
		return TRUE;

	case TB_SAVE:
		SaveChanges();
		return TRUE;

	case TB_UNDO:
		UndoChanges();
		return TRUE;

	case TB_CUT:
		CutSelectedBlock();
		return TRUE;
		
	case TB_COPY:
		CopySelectedBlock();
		return TRUE;

	case TB_PASTE:
		PasteBlockFromCB();
		return TRUE;

	case TB_ABOUT:
		ShowAbout();
		return TRUE;

	case TB_CLOSE:
		HEditQuit();
		return TRUE;

	//
	// tray menu commands (IDT_XXX)
	//
	case IDT_RESTORE:
		HEditReturnFromTray();
		return TRUE;

	case IDT_EXIT:
		HEditKillTrayIcon();
		HEditQuit();
		return TRUE;
	}

	return FALSE; // ERR
}

BOOL HexEditWnd::HEHandleWM_NOTIFY(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//
	// offer TB tooltips
	//
	if (((LPNMHDR)lParam)->code == TTN_NEEDTEXT)
	{
		switch(((LPNMHDR) lParam)->idFrom)
		{
		case TB_SAVE:
			((LPTOOLTIPTEXT)lParam)->lpszText = "save changes  ( STR+S )";
			return TRUE;

		case TB_UNDO:
			((LPTOOLTIPTEXT)lParam)->lpszText = "undo all unsaved changes  ( STR+Z )";
			return TRUE;

		case TB_GOTO:
			((LPTOOLTIPTEXT)lParam)->lpszText = "goto offset  ( STR+G )";
			return TRUE;

		case TB_SELBLOCK:
			((LPTOOLTIPTEXT)lParam)->lpszText = "select block  ( STR+B )";
			return TRUE;
			
		case TB_SELALL:
			if ( IsAllSelected() )
				((LPTOOLTIPTEXT)lParam)->lpszText = "deselect all  ( STR+A )";
			else
				((LPTOOLTIPTEXT)lParam)->lpszText = "select all  ( STR+A )";
			return TRUE;

		case TB_SEARCH:
			((LPTOOLTIPTEXT)lParam)->lpszText = "search  ( STR+F )";
			return TRUE;

		case TB_SEARCHDOWN:
			((LPTOOLTIPTEXT)lParam)->lpszText = "search down again  ( ALT+D )";
			return TRUE;

		case TB_SEARCHUP:
			((LPTOOLTIPTEXT)lParam)->lpszText = "search up again  ( ALT+U )";
			return TRUE;

		case TB_CUT:
			((LPTOOLTIPTEXT)lParam)->lpszText = "cut into clipboard  ( STR+X )";
			return TRUE;

		case TB_COPY:
			((LPTOOLTIPTEXT)lParam)->lpszText = "copy to clipboard  ( STR+C )";
			return TRUE;

		case TB_PASTE:
			((LPTOOLTIPTEXT)lParam)->lpszText = "paste from clipboard  ( STR+V )";
			return TRUE;

		case TB_WIN2TOP:
			((LPTOOLTIPTEXT)lParam)->lpszText =
				bHEOnTop ? "set window state to non-top" : "set window state to top  ( STR+T )";
			return TRUE;

		case TB_ABOUT:
			((LPTOOLTIPTEXT)lParam)->lpszText = "about  ( F12 )";
			return TRUE;

		case TB_CLOSE:
			((LPTOOLTIPTEXT)lParam)->lpszText = "close this window  ( ESC )";
			return TRUE;
		}
	}

	return FALSE; // ERR
}

void HexEditWnd::InitGotoDlg(HWND hDlg)
{
	// hook editbox proc
	HookHexEditbox( GetDlgItem(hDlg, GO_OFFSET) );

	// limit text len
	SendDlgItemMessage(hDlg, GO_OFFSET, EM_SETLIMITTEXT, 8, 0);

	return;
}

BOOL HexEditWnd::GDHandleWM_COMMAND(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DWORD dwNewOff;
	char  cBuff[9];

	switch(LOWORD(wParam))
	{
	case GO_OFFSET:
		if (HIWORD(wParam) != NM_HEXEDITENTER)
			return FALSE;
	case GO_OK:
		// get off
		GetDlgItemText(hDlg, GO_OFFSET, cBuff, sizeof(cBuff));
		HexStrToInt(cBuff, &dwNewOff);
		if (!IsOutOfRange(dwNewOff))
		{
			KillSelection();
			SetCaretPosData(dwNewOff);
			SendMessage(hDlg, WM_CLOSE, 1, 0);
		}
		else
			ErrMsg(hDlg, "Out of range !");
		return TRUE;

	case GO_CANCEL:
		SendMessage(hDlg, WM_CLOSE, 0, 0);
		return TRUE;
	}

	return FALSE;
}

void HexEditWnd::ErrMsg(HWND hWnd, char* szText)
{
	MessageBox(hWnd, szText, "ERROR", MB_ICONERROR);

	return;
}

//
// overloaded
//
void HexEditWnd::ErrMsg(char* szText)
{
	MessageBox(hMainWnd, szText, "ERROR", MB_ICONERROR);

	return;
}

//
// overloaded
//
void HexEditWnd::ErrMsg(HWND hWnd, char* szText, char* szCaption)
{
	MessageBox(hWnd, szText, szCaption, MB_ICONERROR);

	return;
}

BOOL HexEditWnd::IsOutOfRange(DWORD dwOffset)
{
	return (dwOffset >= diData.dwSize) ? TRUE : FALSE;
}

//
// overloaded
//
BOOL HexEditWnd::IsOutOfRange(PHE_POS ppos)
{
	return (ppos->dwOffset >= diData.dwSize) ? TRUE : FALSE;
}

//
// simply fills the HE_POS struct of the caret
//
void HexEditWnd::SetCaretPosData(PHE_POS ppos)
{
	memcpy(
		&stat.posCaret,
		ppos,
		sizeof(HE_POS));
	stat.bCaretPosValid = TRUE;

	return;
}

//
// overloaded
//
void HexEditWnd::SetCaretPosData(DWORD dwOffset)
{
	HE_POS  posNew;

	posNew.bHiword       = TRUE;
	posNew.bTextSection  = FALSE;
	posNew.dwOffset      = dwOffset;
	SetCaretPosData(&posNew);

	return;
}

//
// returns:
// 0 - if the message was processed
//
LRESULT HexEditWnd::HEHandleWM_CHAR(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BYTE       byNew, byMask;

	// caret set ?
	if (!stat.bCaretVisible ||
		!stat.bCaretPosValid)
		return 1; // ERR

	// skip key combinations (STR + Z, ...)
	if (wParam < 0x20)
		return 1; // ERR

	if (IsReadOnly())
	{
		Beep();
		return 1; // ERR
	}

	//
	// process HE change
	//
	if (stat.posCaret.bTextSection)
	{
		*(BYTE*)((DWORD)diData.pDataBuff + stat.posCaret.dwOffset) = (BYTE)wParam;
	}
	else
	{
		// build new byte
		if (wParam >= '0' && wParam <= '9')
			byMask = wParam - 0x30;
		else if (wParam >= 'a' && wParam <= 'f')
			byMask = wParam - 87;
		else if (wParam >= 'A' && wParam <= 'F')
			byMask = wParam - 55;
		else
			return 1; // ERR

		byNew = *(BYTE*)((DWORD)diData.pDataBuff + stat.posCaret.dwOffset);

		if (stat.posCaret.bHiword)
			byNew = (byNew & 0x0F) + byMask * 16;
		else
			byNew = (byNew & 0xF0) + byMask;

		*(BYTE*)((DWORD)diData.pDataBuff + stat.posCaret.dwOffset) = byNew;		
	}

	//
	// reset caret
	//
	if (stat.posCaret.bTextSection)
	{
		++stat.posCaret.dwOffset;
	}
	else
	{
		if (!stat.posCaret.bHiword)
			++stat.posCaret.dwOffset;
		stat.posCaret.bHiword ^= 1;
	}

	stat.bChanges = TRUE;
	ConfigureTB();
	ValidatePos(&stat.posCaret);
	SetCaret();

	return 0; // OK
}

//
// enables/disables toolbar buttons 
//
void HexEditWnd::ConfigureTB()
{
	BOOL bEnabled;

	// save button
	bEnabled = !IsReadOnly() && stat.bChanges;
	SendMessage(hTB,
		TB_CHANGEBITMAP,
		TB_SAVE,
		bEnabled ? 2 : 4);
	SendMessage(hTB,
		TB_SETSTATE,
		TB_SAVE,
		bEnabled ? TBSTATE_ENABLED : TBSTATE_INDETERMINATE);

	// undo button
	bEnabled = !IsReadOnly() && stat.bChanges;
	SendMessage(hTB,
		TB_CHANGEBITMAP,
		TB_UNDO,
		bEnabled ? 14 : 4);
	SendMessage(hTB,
		TB_SETSTATE,
		TB_UNDO,
		bEnabled ? TBSTATE_ENABLED : TBSTATE_INDETERMINATE);

	// top button
	SendMessage(hTB,
		TB_CHANGEBITMAP,
		TB_WIN2TOP,
		bHEOnTop ? 5 : 6);

	// search buttons
	bEnabled = search.bInited;
	SendMessage(hTB,
		TB_CHANGEBITMAP,
		TB_SEARCHDOWN,
		bEnabled ? 13 : 4);
	SendMessage(hTB,
		TB_SETSTATE,
		TB_SEARCHDOWN,
		bEnabled ? TBSTATE_ENABLED : TBSTATE_INDETERMINATE);

	SendMessage(hTB,
		TB_CHANGEBITMAP,
		TB_SEARCHUP,
		bEnabled ? 12 : 4);
	SendMessage(hTB,
		TB_SETSTATE,
		TB_SEARCHUP,
		bEnabled ? TBSTATE_ENABLED : TBSTATE_INDETERMINATE);

	// cut button
	bEnabled = stat.bSel && !IsReadOnly() && IsResizingAllowed();
	SendMessage(hTB,
		TB_CHANGEBITMAP,
		TB_CUT,
		bEnabled ? 9 : 4);
	SendMessage(hTB,
		TB_SETSTATE,
		TB_CUT,
		bEnabled ? TBSTATE_ENABLED : TBSTATE_INDETERMINATE);

	// copy button
	bEnabled = stat.bSel;
	SendMessage(hTB,
		TB_CHANGEBITMAP,
		TB_COPY,
		bEnabled ? 10 : 4);
	SendMessage(hTB,
		TB_SETSTATE,
		TB_COPY,
		bEnabled ? TBSTATE_ENABLED : TBSTATE_INDETERMINATE);

	// paste button
	bEnabled = !IsReadOnly() && IsResizingAllowed();
	SendMessage(hTB,
		TB_CHANGEBITMAP,
		TB_PASTE,
		bEnabled ? 11 : 4);
	SendMessage(hTB,
		TB_SETSTATE,
		TB_PASTE,
		bEnabled ? TBSTATE_ENABLED : TBSTATE_INDETERMINATE);

	return;
}

BOOL HexEditWnd::SaveChanges()
{
	BOOL       bRet = FALSE;
	void*      pNewOrgMem;
	HE_ACTION  act;

	// valid?
	if (IsReadOnly()    ||
		!stat.bChanges)
		goto Exit; // ERR

	SetStatusText("Saving...");
	RepaintClientAreaNow();

	//
	// save changes to buffer (and to disk)
	//
	if (fInput.GetMapPtr())
	{
		// alloc more mem if needed
		if (diOrgData.dwSize < diData.dwSize)
		{
			pNewOrgMem = realloc(diOrgData.pDataBuff, diData.dwSize);

			if (!pNewOrgMem)
			{
				ErrMsg("Not enough memory available to update the buffer with the original data !");
				goto SkipOrgBuffUpdate;
			}
			else
				diOrgData.pDataBuff = pNewOrgMem;
		}
			
		memcpy(
			diOrgData.pDataBuff,
			diData.pDataBuff,
			diData.dwSize);

		// set vars
		diOrgData.dwSize = diData.dwSize;
		fInput.SetMapPtrSize(diOrgData.pDataBuff, diOrgData.dwSize);

SkipOrgBuffUpdate:
		fInput.FlushFileMap();
	}
	else
	{
		// current working memory buffer greater than org mem buffer ?
		if (diData.dwSize > diOrgData.dwSize)
		{
			diData.pDataBuff = realloc(diData.pDataBuff, diOrgData.dwSize);
			diData.dwSize    = diOrgData.dwSize;
		}
		memcpy(
			diOrgData.pDataBuff,
			diData.pDataBuff,
			diData.dwSize);
	}

	//
	// repaint
	//
	stat.bChanges = FALSE;
	SetCaretSelInfoToStatus();
	RepaintClientArea();
	ConfigureTB();

	//
	// inform about HE_ACTION_SAVED
	//
	if (pActionHandler)
	{
		ZERO(act);
		act.dwNewSize     = diData.dwSize;
		act.dwActionCode  = HE_ACTION_SAVED;
		pActionHandler(&act);
	}

	bRet = TRUE;

Exit:

	return bRet;
}

void HexEditWnd::SetHEWndCaption()
{
	char cCaption[400];

	if (!diOrgData.dwSize)
		SetWindowText(hMainWnd, HEDIT_WND_TITLE);

	// build str
	lstrcpy(cCaption, HEDIT_WND_TITLE);
	lstrcat(cCaption, " - \"");
	if (fInput.GetMapPtr())
		lstrcat(cCaption, fInput.GetFilePath());
	else
		lstrcat(cCaption, szMemoryBuff);
	lstrcat(cCaption, "\" ");
	if (IsReadOnly())
		lstrcat(cCaption, "[READONLY]");
	else
		lstrcat(cCaption, "[READWRITE]");

	// set
	SetWindowText(hMainWnd, cCaption);

	return;
}

void HexEditWnd::SetStatusText(char *szFormat, ...)
{
	va_list args;

	va_start(args, szFormat);
	wvsprintf(cSBText, szFormat, args);
	va_end(args);

	return;
}

BOOL HexEditWnd::IsReadOnly()
{
	return diOrgData.bReadOnly;
}

//
// selects a block
//
BOOL HexEditWnd::SetSelection(DWORD dwOffStart, DWORD dwOffEnd)
{
	// input valid?
	if (IsOutOfRange(dwOffStart) ||
		IsOutOfRange(dwOffEnd))
		return FALSE; // ERR

	// set sel vars
	stat.dwOffSelStart   = __min(dwOffStart, dwOffEnd);
	stat.dwOffSelEnd     = __max(dwOffStart, dwOffEnd);
	stat.bSel            = TRUE;

	// set caret vars
	if (stat.bCaretVisible)
	{
		HideCaret(hMainWnd);
		stat.bCaretVisible = FALSE;
	}
	if (dwOffEnd > dwOffStart)
	{
		stat.posCaret.bHiword       = FALSE;
		stat.posCaret.dwOffset      = dwOffEnd;
	}
	else
	{
		stat.posCaret.bHiword   = TRUE;
		stat.posCaret.dwOffset  = dwOffEnd;
	}

	// make caret pos valid
	if (!stat.bCaretPosValid)
	{
		stat.posCaret.bTextSection = FALSE;
		stat.bCaretPosValid        = TRUE;
	}

	// setup TB
	ConfigureTB();

	// repaint
	SetCaretSelInfoToStatus();
	RepaintClientArea();

	return TRUE; // OK
}

//
// returns:
// whether there was a selection before
//
BOOL HexEditWnd::KillSelection()
{
	if (stat.bSel)
	{
		stat.bSel = FALSE;

		// setup TB
		ConfigureTB();

		return TRUE;
	}
	else
		return FALSE;
}

//
// args:
// ppClient - x and y position in client coordinates
//
// returns:
// whether the the input changed the selection
//
BOOL HexEditWnd::Point2Selection(LPPOINT ppClient)
{
	HE_POS  pos, *pposLast;

	if (!stat.bLastLBDownPos)
		return FALSE; //ERR

	if (!PointToPos(ppClient, &pos))
		return FALSE; // ERR

	// mouse moved since last button down ?
	pposLast = &stat.posLastLButtonDown;
	if (pos.bTextSection == pposLast->bTextSection)
	{
		if ((pos.dwOffset != pposLast->dwOffset) ||
			((pos.dwOffset == pposLast->dwOffset) && pos.bHiword != pposLast->bHiword))
		{			 
			 SetSelection(pposLast->dwOffset, pos.dwOffset);
			 RepaintClientArea();
			 return TRUE; // OK
		}
	}

	return FALSE; // ERR
}

//
// Args:
// hWnd - can be the window handle of the main window or of the TB
//
BOOL HexEditWnd::HEHandleWM_MOUSEMOVE(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	POINT  poi;
	BOOL   bRet;

	//
	// handle selection
	//
	if ( !TESTBIT(wParam, MK_LBUTTON) )
		return FALSE; // ERR

	poi.x = LOWORD(lParam);
	poi.y = HIWORD(lParam);

	// cursor not in text/hex region ?
	if ((DWORD)poi.x > PAIRS_X
		&& (DWORD)poi.x < CHARS_X + 16*uFontWidth
		&& stat.bMouseSelecting)
		if ((DWORD)poi.y > iyHETop + uMaxLines*uFontHeight) // under ?
		{
			// scroll down
			stat.posCaret.dwOffset += 2*16;
			ValidatePos( &stat.posCaret );
			MakeCaretVisible();

			if (stat.posCaret.bTextSection)
				poi.x = CHARS_X + 15*uFontWidth;
			else
				poi.x = PAIRS_X + 15*DIGIT_PAIR_WIDTH;
			poi.y = iyHETop + uFontHeight * (uMaxLines-1);
		}
		else if ((DWORD)poi.y < iyHETop) // over ?
		{
			// scroll up
			stat.posCaret.dwOffset -= 2*16;
			ValidatePos( &stat.posCaret );
			MakeCaretVisible();

			if (stat.posCaret.bTextSection)
				poi.x = CHARS_X;
			else
				poi.x = PAIRS_X;
			poi.y = iyHETop;
		}

	bRet = Point2Selection(&poi);

	return bRet; // RET
}

UINT HexEditWnd::GetCurrentLine()
{
	return (UINT)(stat.dwCurOffset / 16);
}

//
// same as "SetTopLine" but fails if "iLine" is invalid
//
BOOL HexEditWnd::SetCurrentLine(UINT iLine)
{
	if ((int)iLine < 0)
		return FALSE; // ERR
	if (iLine > GetTotalLineNum())
		return FALSE; // ERR

	SetTopLine((int)iLine);

	return TRUE;
}

//
// returns:
// whether sth was fixed
//
BOOL HexEditWnd::ValidateLine(int *piLine)
{
	int iLastLine = GetTotalLineNum();

	if (*piLine < 0)
	{
		*piLine = 0;
		return TRUE; // OK
	}
	else if (*piLine > iLastLine)
	{
		*piLine = iLastLine;
		return TRUE; // OK
	}
	else
		return FALSE; // ERR
}

//
// returns:
// whether the last state
//
BOOL HexEditWnd::SetHEWnd2Top(BOOL bTop)
{
	RECT  rect;
	BOOL  bBefore;

	// set wnd state
	GetWindowRect(hMainWnd, &rect);
	SetWindowPos(
		hMainWnd,
		bTop ? HWND_TOPMOST : HWND_NOTOPMOST,
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top,
		SWP_SHOWWINDOW);

	// handle vars
	bBefore  = bHEOnTop;
	bHEOnTop = bTop;

	return bBefore;	
}

void HexEditWnd::InitSelBlockDlg(HWND hDlg)
{
	// hook editbox'S
	HookHexEditbox( GetDlgItem(hDlg, SB_START) );
	HookHexEditbox( GetDlgItem(hDlg, SB_END) );
	HookHexEditbox( GetDlgItem(hDlg, SB_SIZE) );

	// limit text len
	SendDlgItemMessage(hDlg, SB_START, EM_SETLIMITTEXT, 8, 0);
	SendDlgItemMessage(hDlg, SB_END, EM_SETLIMITTEXT, 8, 0);
	SendDlgItemMessage(hDlg, SB_SIZE, EM_SETLIMITTEXT, 8, 0);

	return;
}

BOOL HexEditWnd::SBHandleWM_COMMAND(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DWORD  dwStart, dwEnd;
	char   cBuff[9];
	BOOL   b;
	HWND   hESize, hEEnd;

	switch(LOWORD(wParam))
	{
	case SB_START:
	case SB_END:
	case SB_SIZE:
			if (HIWORD(wParam) != NM_HEXEDITENTER)
				return FALSE;
	case SB_OK:
		// get numbers
		GetDlgItemText(hDlg, SB_START, cBuff, sizeof(cBuff));
		HexStrToInt(cBuff, &dwStart);

		if (IsDlgButtonChecked(hDlg, SB_RADIOEND))
		{
			GetDlgItemText(hDlg, SB_END, cBuff, sizeof(cBuff));
			HexStrToInt(cBuff, &dwEnd);
		}
		else
		{
			GetDlgItemText(hDlg, SB_SIZE, cBuff, sizeof(cBuff));
			HexStrToInt(cBuff, &dwEnd);
			if (dwEnd != 0)
				--dwEnd;
			dwEnd += dwStart;
		}

		// valid?
		if (IsOutOfRange(dwEnd) ||
			IsOutOfRange(dwStart))
			ErrMsg(hDlg, "Out of range !");
		else
		{
			SetTopLine( (DWORD)__min(dwStart, dwEnd) );
			SetSelection(dwStart, dwEnd);
			SendMessage(hDlg, WM_CLOSE, 0, 0);
		}
		return TRUE;

	case SB_RADIOSIZE:
	case SB_RADIOEND:
		// GUI stuff
		hEEnd  = GetDlgItem(hDlg, SB_END);
		hESize = GetDlgItem(hDlg, SB_SIZE);
		b = (LOWORD(wParam) == SB_RADIOEND) ? TRUE : FALSE;
		EnableWindow( hEEnd, b);
		EnableWindow( hESize, b ^ 1);
		SetFocus( hEEnd );	
		SetWindowText( b ? hESize : hEEnd, "");
		SetFocus( b ? hEEnd : hESize);
		return TRUE;

	case SB_CANCEL:
		SendMessage(hDlg, WM_CLOSE, 0, 0);
		return TRUE;
	}

	return FALSE;
}

void HexEditWnd::SSInitDlg(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND   hCur;
	char   cBuff[9];

	// setup str edit box
	HookEditboxEnter( GetDlgItem(hDlg, SS_STR) );

	// setup offset edit box
	hCur = GetDlgItem(hDlg, SS_OFFSET);
	HookHexEditbox(hCur);
	SendMessage(hCur, EM_SETLIMITTEXT, 8, 0);

	if (search.bInited)
	{
		//
		// restore last search options
		//

		// str stuff
		if (search.bASCIIStr)
			CheckDlgButton(hDlg, SS_SEARCHASCII, TRUE);
		if (search.bWideCharStr)
			CheckDlgButton(hDlg, SS_SEARCHUNICODE, TRUE);
		if (search.bCaseSensitive)
			CheckDlgButton(hDlg, SS_CASESENSITIVE, TRUE);
		else
			CheckDlgButton(hDlg, SS_NOCASESENSITIVE, TRUE);

		SetDlgItemText(hDlg, SS_STR, (PSTR)search.pDlgStr);

		/*
		if (search.bASCIIStr)
			SetDlgItemText(hDlg, SS_STR, (PSTR)search.pData);
		else if (search.bWideCharStr)
		{
			if (WideChar::WideToSingleCharStr((PWSTR)search.pData, (PSTR)search.pData, search.dwcBuff))
				SetDlgItemText(hDlg, SS_STR, (PSTR)search.pData);
		}
		else
		{
		}
		*/

		// location stuff
		if (search.bDown)
			CheckDlgButton(hDlg, SS_DOWN, TRUE);
		else
			CheckDlgButton(hDlg, SS_UP, TRUE);
		switch(search.iDlgSearchFrom)
		{
		case 0:
			CheckDlgButton(hDlg, SS_SEARCHFROMTOP, TRUE);
			break;

		case 1:
			if (stat.bCaretPosValid)
			{
				SendMessage(hDlg, WM_COMMAND, SS_SEARCHFROMCURPOS, 0);
				break;
			}
			// ...

		default: // 2 and eventually 1
			wsprintf(cBuff, H8, search.dwStartOff);
			SetDlgItemText(hDlg, SS_OFFSET, cBuff);
			CheckDlgButton(hDlg, SS_SEARCHFROMOFF, TRUE);			
			break;
		}

	}
	else
	{
		//
		// setup default dlg
		//

		// case sensitivity
		CheckDlgButton(hDlg, SS_NOCASESENSITIVE, TRUE);

		// set default settings
		CheckDlgButton(hDlg, SS_SEARCHASCII, TRUE);
		CheckDlgButton(hDlg, SS_SEARCHFROMTOP, TRUE);
		CheckDlgButton(hDlg, SS_DOWN, TRUE);
		if (!stat.bCaretPosValid)
		{
			// enable this radio
			EnableWindow( GetDlgItem(hDlg, SS_SEARCHFROMCURPOS), FALSE);
		}
	}

	// setup GUI
	SSEnableItems(hDlg);

	return;
}

BOOL HexEditWnd::SSHandleWM_COMMAND(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char  cBuff[20];

	switch(LOWORD(wParam))
	{
	case SS_STR:
		if (HIWORD(wParam) != NM_HEXEDITENTER)
			return FALSE;
	case SS_OK:
		if (SSHandleSS_OK(hDlg))
		{
			SendMessage(hDlg, WM_CLOSE, 0, 0);
			PerformStrSearch(&search);
		}
		break;

	case SS_CANCEL:
		SendMessage(hDlg, WM_CLOSE, 0, 0);
		return TRUE;

	case SS_SEARCHASCII:
		CheckDlgButton(hDlg, SS_SEARCHUNICODE, BST_UNCHECKED);
		SSEnableItems(hDlg);
		return TRUE;

	case SS_SEARCHUNICODE:
		CheckDlgButton(hDlg, SS_SEARCHASCII, BST_UNCHECKED);
		SSEnableItems(hDlg);
		return TRUE;

	case SS_CASESENSITIVE:
	case SS_NOCASESENSITIVE:
		CheckRadioButton(hDlg, SS_CASESENSITIVE, SS_NOCASESENSITIVE, LOWORD(wParam) );
		return TRUE;

	case SS_SEARCHFROMCURPOS:
	case SS_SEARCHFROMTOP:
	case SS_SEARCHFROMOFF:
		CheckRadioButton(hDlg, SS_SEARCHFROMTOP, SS_SEARCHFROMOFF, LOWORD(wParam) );
		SSEnableItems(hDlg);

		// focus off editbox
		if (LOWORD(wParam) == SS_SEARCHFROMOFF)
			SetFocus( GetDlgItem(hDlg, SS_OFFSET) );

		// show current pos
		if (LOWORD(wParam) == SS_SEARCHFROMCURPOS)
		{
			// show current off
			wsprintf(cBuff, "("_H8")", stat.posCaret.dwOffset);
			SetDlgItemText(hDlg, SS_CUROFF, cBuff);
		}
		else
			SetDlgItemText(hDlg, SS_CUROFF, NULL);
		return TRUE;

	case SS_UP:
	case SS_DOWN:
		CheckRadioButton(hDlg, SS_DOWN, SS_UP, LOWORD(wParam) );
		return TRUE;
	}

	return FALSE; // ERR
}

void HexEditWnd::SSEnableItems(HWND hDlg)
{
	BOOL b;

	// case sensitive - radio
	b = IsDlgButtonChecked(hDlg, SS_SEARCHASCII) ||
		IsDlgButtonChecked(hDlg, SS_SEARCHUNICODE);
	EnableWindow( GetDlgItem(hDlg, SS_CASESENSITIVE), b);
	EnableWindow( GetDlgItem(hDlg, SS_NOCASESENSITIVE), b);

	// location
	b = IsDlgButtonChecked(hDlg, SS_SEARCHFROMOFF);
	EnableWindow( GetDlgItem(hDlg, SS_OFFSET), b);

	return;
}

//
// get search options from gui
//
BOOL HexEditWnd::SSHandleSS_OK(HWND hDlg)
{
	HWND               hStrEdit;
	HE_SEARCHOPTIONS   data;
	DWORD              dwByte, dwOff;
	char               cBuff[9];
	UINT               u;
	BOOL               bRet = FALSE;

	ZERO(data);

	//
	// handle search data
	//

	// get info
	hStrEdit = GetDlgItem(hDlg, SS_STR);
	data.dwcStr           = GetWindowTextLength(hStrEdit);
	data.bASCIIStr        = IsDlgButtonChecked(hDlg, SS_SEARCHASCII);
	data.bWideCharStr     = IsDlgButtonChecked(hDlg, SS_SEARCHUNICODE);
	data.bCaseSensitive   = IsDlgButtonChecked(hDlg, SS_CASESENSITIVE);

	// valid?
	if (!data.dwcStr)
	{
		ErrMsg(hDlg, "Please enter a search string !");
		goto Exit; // ERR
	}

	if ( !(data.bASCIIStr || data.bWideCharStr) &&
		 data.dwcStr % 2)
	{
		ErrMsg(hDlg, "Hex byte pairs have to result in an even number of digits !");
		goto Exit; // ERR
	}

	// get search str data
	data.dwcBuff = data.dwcStr + 1;
	if (data.bWideCharStr)
	{ 
		data.dwcBuff *= 2;
		data.dwcStr  *= 2;
	}
	data.pData   = malloc(data.dwcBuff);
	if (!data.pData)
	{
		ErrMsg(hDlg, STR_NO_MEM);
		goto Exit; // ERR
	}

	// save entered str
	data.pDlgStr = malloc(data.dwcStr + 1);
	if (!data.pDlgStr)
	{
		ErrMsg(hDlg, STR_NO_MEM);
		goto Exit; // ERR
	}
	GetDlgItemText(hDlg, SS_STR, (PSTR)data.pDlgStr, data.dwcStr + 1);

	// set search str data
	GetDlgItemText(hDlg, SS_STR, (PSTR)data.pData, data.dwcBuff);
	if ( !(data.bASCIIStr || data.bWideCharStr) )
	{
		// ASCII -> bytes
		data.dwcStr /= 2;
		for (u = 0; u < data.dwcStr; u++)
		{
			lstrcpyn(cBuff, (PSTR)((DWORD)data.pData + u * 2), 3);
			if (!HexStrToInt(cBuff, &dwByte))
			{
				ErrMsg(hDlg, "No hex string !");
				goto Exit; // ERR
			}
			*(BYTE*)((DWORD)data.pData + u) = (BYTE)dwByte;
		}
		
	}

	// finally convert ASCII -> UNICODE if wanted
	if (data.bWideCharStr)
		WideChar::SingleToWideCharStr(
			(PSTR)data.pData,
			(PWSTR)data.pData,
			data.dwcBuff);

	//
	// handle location
	//
	if (IsDlgButtonChecked(hDlg, SS_SEARCHFROMCURPOS))
	{
		data.dwStartOff       = stat.posCaret.dwOffset;
		data.iDlgSearchFrom   = 1;
	}
	else if (IsDlgButtonChecked(hDlg, SS_SEARCHFROMTOP))
	{
		data.dwStartOff      = 0;
		data.iDlgSearchFrom  = 0;
	}
	else
	{
		data.iDlgSearchFrom = 2;

		GetDlgItemText(hDlg, SS_OFFSET, cBuff, sizeof(cBuff));
		HexStrToInt(cBuff, &dwOff);
		if (IsOutOfRange(dwOff))
		{
			ErrMsg(hDlg, "Searching start offset is out of range !");
			goto Exit; // ERR
		}
		data.dwStartOff = dwOff;
	}
	data.bDown = IsDlgButtonChecked(hDlg, SS_DOWN);

	//
	// apply structure
	//

	// free old buffers
	if (search.bInited &&
		search.pData)
		free(search.pData);
	if (search.bInited &&
		search.pDlgStr)
		free(search.pDlgStr);

	search          = data;
	search.bInited  = TRUE;

	ConfigureTB();

	bRet = TRUE;

Exit:

	// free buffers
	if (!bRet && data.pData)
		free(data.pData);
	if (!bRet && data.pDlgStr)
		free(data.pDlgStr);

	return bRet; // OK
}

//
// searchs the stuff in the HE_SEARCHOPTIONS structure and setups the GUI
//
// returns:
// whether sth was found
//
//#pragma optimize("", off)
BOOL HexEditWnd::PerformStrSearch(PHE_SEARCHOPTIONS pso)
{
	DWORD   dwCurOff;
	BYTE*   pby;
	BOOL    bFound;

#define ERR_MSG(str) ErrMsg(hMainWnd, str, "Search Result:");

	if (!pso->bInited)
		return FALSE; // ERR

	// set status
	SetStatusText("Searching...");
	RepaintClientAreaNow();

	//
	// search
	//
	dwCurOff = pso->dwStartOff;

	// correct a too big off (when up search)
	if (!pso->bDown &&
		diData.dwSize - dwCurOff < pso->dwcStr)
		dwCurOff = diData.dwSize - pso->dwcStr;

	pby      = (BYTE*)((DWORD)diData.pDataBuff + dwCurOff);
	bFound   = FALSE;

	if (pso->bASCIIStr || pso->bWideCharStr)
	{
		//
		// non-case sensitive ASCII/UNICODE string search
		//
		if (!pso->bCaseSensitive)
		{
			if ( pso->bASCIIStr )
			{
				// ascii
				if (pso->bDown) // ...down
					while (dwCurOff + pso->dwcStr <= diData.dwSize)
					{
						if (!strnicmp((PCSTR)pso->pData, (PCSTR)pby, pso->dwcStr))
						{
							bFound = TRUE;
							break;
						}
						++dwCurOff;
						++pby;
					}
				else // ...up
					while (dwCurOff != (DWORD)-1)
					{
						if (!strnicmp((PCSTR)pso->pData, (PCSTR)pby, pso->dwcStr))
						{
							bFound = TRUE;
							break;
						}
						--dwCurOff;
						--pby;
					}
			}
			else
			{
				// unicode
				if (pso->bDown) // ...down
					while (dwCurOff + pso->dwcStr <= diData.dwSize)
					{
						if ( !wcsnicmp((LPWSTR)pso->pData, (LPWSTR)pby, pso->dwcStr/2) )
						{
							bFound = TRUE;
							break;
						}
						++dwCurOff;
						++pby;
					}
				else // ...up
					while (dwCurOff != (DWORD)-1)
					{
						if ( !wcsnicmp((LPWSTR)pso->pData, (LPWSTR)pby, pso->dwcStr/2) )
						{
							bFound = TRUE;
							break;
						}
						--dwCurOff;
						--pby;
					}
			}
		}
		//
		// case sensitive ASCII/UNICODE string search
		//
		else
		{
			if (pso->bDown) // ..down
				while (dwCurOff + pso->dwcStr <= diData.dwSize)
				{
					if (!memcmp(pso->pData, pby, pso->dwcStr))
					{
						bFound = TRUE;
						break;
					}
					++dwCurOff;
					++pby;
				}
			else // ...up
				while (dwCurOff != -1)
				{
					if (!memcmp(pso->pData, pby, pso->dwcStr))
					{
						bFound = TRUE;
						break;
					}
					--dwCurOff;
					--pby;
				}
		}
	}
	else
	{
		//
		// byte search
		//
		if (pso->bDown) // ...down
			while (dwCurOff + pso->dwcStr <= diData.dwSize)
			{
				if (!memcmp(pso->pData, pby, pso->dwcStr))
				{
					bFound = TRUE;
					break;
				}
				++dwCurOff;
				++pby;
			}
		else
			while (dwCurOff != -1)
			{
				if (dwCurOff == 0x10)
					__asm NOP
				if (!memcmp(pso->pData, pby, pso->dwcStr))
				{
					bFound = TRUE;
					break;
				}
				--dwCurOff;
				--pby;
			}

	}

	//
	// mark found area
	//
	if (bFound)
	{
		// save match off
		//search.dwLastFoundOff = dwCurOff;

		// setup GUI
		SetSelection(dwCurOff, dwCurOff + pso->dwcStr - 1);
		if (!IsOffsetVisible(dwCurOff))
			SetTopLine(dwCurOff);
	}
	else
	{
		ERR_MSG(pso->bDown ? "Buffer end reached." : "Buffer start reached.");
	}

	// refresh status
	SetCaretSelInfoToStatus();
	RepaintClientArea();

	return bFound;

#undef ERR_MSG
}
//#pragma optimize("", on)

BOOL HexEditWnd::PerformSearchAgain(PHE_SEARCHOPTIONS pso, BOOL bDown)
{
	// valid?
	if (!pso->bInited)
		return FALSE; // ERR

	if (!stat.bCaretPosValid)
	{
		ErrMsg("Caret to set !");
		return FALSE; // ERR
	}
	
	// mod HE_SEARCHOPTIONS struct
	// (bad offsets are catched by "PerformStrSearch")
	if (bDown)
	{
		if (stat.bSel)
			pso->dwStartOff = stat.dwOffSelEnd + 1;
		else
			pso->dwStartOff = stat.posCaret.dwOffset;
	}
	else
	{
		if (stat.bSel)
			pso->dwStartOff = stat.dwOffSelStart - 1;
		else
			pso->dwStartOff = stat.posCaret.dwOffset;
	}
	pso->bDown = bDown;

	return PerformStrSearch(pso);
}

BOOL HexEditWnd::CopySelectedBlock()
{
	HANDLE               hMem;
	void                 *pMem;
	PHE_CLIPBOARD_DATA   pcbd;
	DWORD                dwc;
	
	// valid ?
	if (!stat.bSel)
		return FALSE; // ERR

	dwc = stat.dwOffSelEnd - stat.dwOffSelStart + 1;

	//
	// data -> CB
	//
	if (!OpenClipboard(NULL))
	{
		ErrMsg("Couldn't open clipboard !");
		return FALSE; // ERR
	}

	hMem  = GlobalAlloc(GHND | GMEM_SHARE, offsetof(HE_CLIPBOARD_DATA, byDataStart) + dwc);
	if (!hMem)
	{
		CloseClipboard();
		ErrMsg(STR_NO_MEM);
		return FALSE; // ERR
	}
	pMem = GlobalLock(hMem);
	pcbd = (PHE_CLIPBOARD_DATA)pMem;
	memcpy(
		&pcbd->byDataStart,
		(void*)((DWORD)diData.pDataBuff + stat.dwOffSelStart),
		dwc);
	pcbd->dwDataSize = dwc;
	GlobalUnlock(hMem);

	EmptyClipboard();
	if (!SetClipboardData(cf16Edit, hMem))
	{
		GlobalFree(hMem);
		CloseClipboard();
		ErrMsg("Couldn't paste data to clipboard !");
		return FALSE; // ERR
	}

	CloseClipboard(); // Cleanup

	return TRUE; // OK
}

BOOL HexEditWnd::CutSelectedBlock()
{
	HE_POS posNew;

	// valid ?
	if (!stat.bSel    ||
		(IsReadOnly()  ||
		!IsResizingAllowed()))
		return FALSE; // ERR

	//
	// copy block
	//
	if (!CopySelectedBlock())
		return FALSE; // ERR

	//
	// move memory buffer
	//
	if (diData.dwSize - 1 > stat.dwOffSelEnd)
		memcpy(
			(void*)((DWORD)diData.pDataBuff + stat.dwOffSelStart),
			(void*)((DWORD)diData.pDataBuff + stat.dwOffSelEnd + 1),
			diData.dwSize - 2 - stat.dwOffSelEnd);

	//
	// reset diData struct
	//
	diData.dwSize    -= stat.dwOffSelEnd - stat.dwOffSelStart + 1;
	// (This realloc call should never fail because the resulting mem block size
	//  is always smaller.)
	diData.pDataBuff  = realloc(diData.pDataBuff, diData.dwSize);

	// set caret
	posNew.bHiword      = TRUE;
	posNew.dwOffset     = stat.dwOffSelStart;
	posNew.bTextSection = stat.posCaret.bTextSection;
	KillSelection();
	SetCaret(&posNew);

	//
	// repaint
	//
	stat.bChanges = TRUE;
	ConfigureTB();
	SetupVScrollbar();
	ValidatePos( &stat.posCaret );
	SetTopLine();
	RepaintClientArea();

	return TRUE; // OK
}

BOOL HexEditWnd::PasteBlockFromCB()
{
	PHE_CLIPBOARD_DATA   pcbd;
	void                 *pNewMem, *pBlock;
	DWORD                dwcBlock, dwOffAdd;
	HE_POS               posNew;

	// valid ?
	if ( IsReadOnly() || !IsResizingAllowed() )
		return FALSE; // ERR

	if (!stat.bCaretPosValid)
	{
		ErrMsg("Please select one byte first !");
		return FALSE; // ERR
	}

	//
	// get HE_CLIPBOARD_DATA pointer
	//
	if (!IsClipboardFormatAvailable(cf16Edit))
	{
		ErrMsg("No clipboard data in my "CF_16Edit" format available :(");
		return FALSE; // ERR
	}
	if (!OpenClipboard(hMainWnd))
	{
		ErrMsg("Couldn't open clipboard !");
		return FALSE; // ERR
	}
	pcbd = (PHE_CLIPBOARD_DATA) GetClipboardData(cf16Edit); // for win2k
	if (!pcbd)
	{
		CloseClipboard();
		ErrMsg("Couldn't receive clipboard data !");
		return FALSE; // ERR
	}
	CloseClipboard();

	//
	// paste data into buffers
	//

	// wipe selected memory
	if (stat.bSel)
	{
		if (diData.dwSize - 1 > stat.dwOffSelEnd)
			memcpy(
				(void*)((DWORD)diData.pDataBuff + stat.dwOffSelStart),
				(void*)((DWORD)diData.pDataBuff + stat.dwOffSelEnd + 1),
				diData.dwSize - 2 - stat.dwOffSelEnd);

		//
		// reset diData struct
		//
		diData.dwSize    -= stat.dwOffSelEnd - stat.dwOffSelStart + 1;
		// (This realloc call should never fail because the resulting mem block size
		//  is always smaller.)
		diData.pDataBuff  = realloc(diData.pDataBuff, diData.dwSize);

		// kill selection
		KillSelection();
		posNew.bTextSection  = stat.posCaret.bTextSection;
		posNew.bHiword       = TRUE;
		posNew.dwOffset      = stat.dwOffSelStart;
		SetCaret(&posNew);
	}
	
	// enough mem ?
	pNewMem = realloc(diData.pDataBuff, diData.dwSize + pcbd->dwDataSize);
	if (!pNewMem)
	{
		// (old data buffer still existing)
		ErrMsg(STR_NO_MEM);
		return FALSE; // ERR
	}

	// replace old mem block
	dwOffAdd  = stat.posCaret.bHiword ? 0 : 1;
	dwcBlock  = diData.dwSize - (stat.posCaret.dwOffset + dwOffAdd);
	pBlock    = malloc(dwcBlock);
	if (!pBlock)
	{
		ErrMsg(STR_NO_MEM);
		return FALSE; // ERR
	}
	memcpy(
		pBlock,
		(void*)((DWORD)pNewMem + stat.posCaret.dwOffset + dwOffAdd),
		dwcBlock);
	memcpy(
		(void*)((DWORD)pNewMem + stat.posCaret.dwOffset + pcbd->dwDataSize + dwOffAdd),
		pBlock,
		dwcBlock);
	free(pBlock);

	// paste new data block
	memcpy(
		(void*)((DWORD)pNewMem + stat.posCaret.dwOffset + dwOffAdd),
		&pcbd->byDataStart,
		pcbd->dwDataSize);

	// apply changes to vars
	diData.dwSize     = diData.dwSize + pcbd->dwDataSize;
	diData.pDataBuff  = pNewMem;

	//
	// repaint
	//
	stat.bChanges = TRUE;
	ConfigureTB();
	SetupVScrollbar();
	if ( !IsOffsetVisible( stat.posCaret.dwOffset ) )
		SetTopLine();
	RepaintClientArea();

	return TRUE; // OK
}

BOOL HexEditWnd::UndoChanges()
{
	void     *pNewMem;

	// valid?
	if (IsReadOnly() ||
		!stat.bChanges)
		return FALSE; // INT ERR

	//
	// handle buffers with different size
	//
	if (diOrgData.dwSize != diData.dwSize)
	{
		pNewMem = realloc(diData.pDataBuff, diOrgData.dwSize);
		if (!pNewMem)
		{
			ErrMsg("Couldn't change buffer's memory size :(");
			return FALSE; // ERR
		}
		diData.pDataBuff = pNewMem;
		diData.dwSize    = diOrgData.dwSize;
	}

	//
	// apply original buffer data to work buffer
	//
	memcpy(
		diData.pDataBuff,
		diOrgData.pDataBuff,
		diData.dwSize);

	//
	// repaint
	//
	ValidatePos( &stat.posCaret );
	SetCaret();
	stat.bChanges = FALSE;
	ConfigureTB();
	RepaintClientArea();

	return TRUE; // OK
}

void HexEditWnd::SelectAll()
{
	if (IsAllSelected())
	{
		// kill sel
		KillSelection();
		SetCaret();
	}
	else
	{
		// sel
		SetSelection(0, diData.dwSize - 1);
	}

	// repaint
	RepaintClientArea();

	return;
}

BOOL HexEditWnd::IsAllSelected()
{
	return (stat.bSel &&
		stat.dwOffSelStart == 0 &&
		stat.dwOffSelEnd == diData.dwSize - 1) ? TRUE : FALSE;
}

void HexEditWnd::ShowAbout()
{
	MSGBOXPARAMS args;

	ZERO(args);
	args.cbSize         = sizeof( args );
	args.hwndOwner      = hMainWnd;
	args.dwStyle        = MB_USERICON;
	args.hInstance      = GetInstance();
	args.lpszIcon       = (PSTR)IDI_16Edit;
	args.lpszCaption    = "About";
	args.lpszText       = "16Edit - HexEditor Module\n"\
						  "by yoda\n\n"\
						  "LordPE@gmx.net\n"\
						  "y0da.cjb.net";
	MessageBoxIndirect( &args );

	return;
}

//
// exported via stub
//
// returns:
// FALSE - if neither an input file nor an input memory block was specified or the input
//         file could not be loaded or not enough memory or main win already opened
//
BOOL HexEditWnd::DoSpecifySettings(PHE_SETTINGS ps)
{
	BOOL bRet;

	if (bHEVisible)
		return FALSE; // ERR

	// init variables
	InitEdition();

	// install ActionHandler
	if (TESTBIT(ps->dwMask, HE_SET_ACTIONHANDLER) &&
		ps->pHandler)
		pActionHandler = ps->pHandler;

	//
	// handle input buffer
	//
	if (TESTBIT(ps->dwMask, HE_SET_MEMORYBLOCKINPUT))
		bRet = SetHEDataInfo(&ps->diMem);
	else if (TESTBIT(ps->dwMask, HE_SET_INPUTFILE))
		bRet = DoEditFile(
			ps->szFilePath,
			TESTBIT(ps->dwMask, HE_SET_FORCEREADONLY) ? TRUE : FALSE);
	else
		return FALSE; // ERR

	if (!bRet)
		return FALSE; // ERR

	//
	// handle the rest
	//
	bResizingAllowed = TESTBIT(ps->dwMask, HE_SET_NORESIZE) ? FALSE : TRUE;
	bMinToTray       = TESTBIT(ps->dwMask, HE_SET_MINIMIZETOTRAY) ? TRUE : FALSE;
	bSaveWinPos      = TESTBIT(ps->dwMask, HE_SET_SAVEWINDOWPOSITION) ? TRUE : FALSE;
	bRestoreWinPos   = TESTBIT(ps->dwMask, HE_SET_RESTOREWINDOWPOSITION) ? TRUE : FALSE;

	if (TESTBIT(ps->dwMask, HE_SET_FORCEREADONLY))
		diOrgData.bReadOnly = TRUE;

	if (TESTBIT(ps->dwMask, HE_SET_SETCURRENTOFFSET))
	{
		bStartCaretPos    = TRUE;
		posStartCaretPos  = ps->posCaret;
	}

	if (TESTBIT(ps->dwMask, HE_SET_SETSELECTION))
	{
		bStartSel        = TRUE;
		dwStartSelStart  = ps->dwSelStartOff;
		dwStartSelEnd    = ps->dwSelEndOff;
	}

	if (TESTBIT(ps->dwMask, HE_SET_ONTOP))
		bStartOnTop = TRUE;

	if (TESTBIT(ps->dwMask, HE_SET_PARENTWINDOW))
	{
		bParentWin  = TRUE;
		hwndParent  = ps->hwndParent;
	}

	if ( TESTBIT(ps->dwMask, HE_SET_USERWINDOWPOSITION) )
	{
		wpUser = ps->wpUser;
		bUserWinPos = TRUE;
	}
	
	return TRUE; // OK
}

BOOL HexEditWnd::IsResizingAllowed()
{
	return bResizingAllowed;
}

void HexEditWnd::HEHandleWM_CLOSE(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HE_ACTION    act;
	HE_WIN_POS   wp;

	// handle HE_SET_SAVEWINDOWPOSITION
	if (bSaveWinPos)
	{
		wp.ix    = rctLastPos.left;
		wp.iy    = rctLastPos.top;
		wp.icx   = rctLastPos.right - rctLastPos.left;
		wp.icy   = rctLastPos.bottom - rctLastPos.top;
		if ( IsZoomed(hWnd) )
			wp.iState = SW_MAXIMIZE;
		else if ( IsIconic(hWnd) )
			wp.iState = SW_MINIMIZE;
		else
			wp.iState = SW_SHOWNORMAL;
		WritePrivateProfileStruct(INI_SECTION, INI_WINPOSKEY, &wp, sizeof(wp), cIniPath);
	}

	//
	// inform ActionHandler about HE_ACTION_EXITING
	//
	if (pActionHandler)
	{
		ZERO(act);
		act.dwActionCode = HE_ACTION_EXITING;
		pActionHandler(&act);
	}

	// handle variables
	bHEVisible = FALSE;
	QuitEdition();

	//
	// leave win loop
	//
	PostQuitMessage(0);

	// handle parent window state
	if (bParentWin)
	{
		if (!IsWindowEnabled(hwndParent))
			EnableWindow(hwndParent, TRUE);
		SetForegroundWindow(hwndParent);
	}

	return;
}

//
// Sets custom settings in edit (input: HE_SETTINGS)
//
// returns:
// TRUE - if either a block was selected or the caret was set successfully
//
BOOL HexEditWnd::HandleStartCaretPosSel(HWND hWnd)
{
	if (bStartCaretPos)
	{
		if (!IsOutOfRange(&posStartCaretPos))
		{
			SetTopLine(posStartCaretPos.dwOffset);
			SetCaret(&posStartCaretPos);
			return TRUE; // OK
		}
		else
			return FALSE; // ERR
	}
	else if (bStartSel)
	{
		if (HEdit.SetSelection(dwStartSelStart, dwStartSelEnd))
		{
			HEdit.SetTopLine(dwStartSelStart);
			return TRUE; // OK
		}
		else
			return FALSE; // ERR
	}

	return FALSE; // ER
}

void HexEditWnd::HEHandleWM_TRAYMENU(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	POINT p;

	switch(lParam)
	{
	case WM_RBUTTONDOWN:
		GetCursorPos(&p);
		TrackPopupMenu(hmTray, TPM_RIGHTALIGN, p.x, p.y, 0, hWnd, NULL);
		break;

	case WM_LBUTTONDBLCLK:
		HEditReturnFromTray();
		break;
	}

	return;
}

void HexEditWnd::HEditQuit()
{
	SendMessage(hMainWnd, WM_CLOSE, 0, 0);

	return;
}

BOOL HexEditWnd::IsFileInput()
{
	return (fInput.GetMapPtr() != NULL) ? TRUE : FALSE;
}

void HexEditWnd::HEHandleWM_MOVE(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	// save win pos
	if ( !IsIconic(hWnd) &&
		 !IsZoomed(hWnd) )
		 GetWindowRect( hWnd, &rctLastPos );

	return;
}

//
// brings the main window to the foreground
//
void HexEditWnd::HEditToTop()
{
	SetForegroundWindow(hMainWnd);

	return;
}

BOOL HexEditWnd::IsKeyDown(int iVKey)
{
	return ( HIWORD( GetAsyncKeyState(iVKey) ) != 0);
}

void HexEditWnd::SetCaretSelInfoToStatus()
{
	// set cur offset in SB
	if (stat.bSel)
	{
		SetStatusText(
			"Offset: "_H8" - "_H8"    Size:"_H8,
			stat.dwOffSelStart,
			stat.dwOffSelEnd,
			stat.dwOffSelEnd - stat.dwOffSelStart + 1);
	}
	else if (stat.bCaretPosValid)
		SetStatusText("Offset: "_H8, stat.posCaret.dwOffset);
	else
		SetStatusText(NULL);

	return;
}