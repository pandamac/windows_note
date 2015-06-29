
#ifndef __HexEditWnd_h__
#define __HexEditWnd_h__

#include <windows.h>
#include <shellapi.h>
#include "CThread.h"
#include "File.h"
#include "16EditDll.h"

//
// structures
//

#ifndef EXTERN_HE_STRUCTS
#define EXTERN_HE_STRUCTS

typedef struct _HE_DATA_INFO
{
	void*       pDataBuff;
	DWORD       dwSize;     // data indicator
	BOOL        bReadOnly;
} HE_DATA_INFO, *PHE_DATA_INFO;

typedef struct _HE_POS
{
	DWORD       dwOffset;
	BOOL        bHiword;       // (opt.) first digit of the pair ? ...or the 2nd one ?
	BOOL        bTextSection;  // (opt.) Caret in the text part ?
} HE_POS, *PHE_POS;

#endif // EXTERN_HE_STRUCTS

/*
typedef struct HE_CHANGE
{
	BYTE        byNew;
	DWORD       dwOffset;
} *PHE_CHANGE;
*/

typedef struct HE_CLIPBOARD_DATA
{
	DWORD               dwDataSize;
	BYTE                byDataStart;
} *PHE_CLIPBOARD_DATA;

// changes to this structure could affect "HexEditWnd::HexEditWnd()"
typedef struct HE_STATUS
{
	HE_POS              posLastLButtonDown;
	BOOL                bLastLBDownPos;     // TRUE if posLastLButtonDown is valid
	BOOL                bMouseSelecting;

	HE_POS              posCaret;
	BOOL                bCaret;
	BOOL                bCaretVisible;
	BOOL                bCaretPosValid;     // TRUE if posCaret was at least set one time

	DWORD               dwCurOffset;

	BOOL                bSel;
	DWORD               dwOffSelStart;
	DWORD               dwOffSelEnd;

	int                 iLastLine;

	BOOL                bChanges;           // TRUE if there was at least one change performed
} *PHE_STATUS;

typedef struct HE_SEARCHOPTIONS
{
	BOOL                bInited;            // TRUE if the struct was set at least one time

	void*               pData;              // buffer (malloced)
	DWORD               dwcBuff;
	DWORD               dwcStr;

	BOOL                bASCIIStr;
	BOOL                bWideCharStr;
	BOOL                bCaseSensitive;

	DWORD               dwStartOff;

	BOOL                bDown;

//	DWORD               dwLastFoundOff;     // 0xFFFFFFFF if not set

	// vars being needed to restore the search dlg
	int                 iDlgSearchFrom;     // 0 - top, 1 - cur pos, 2 - off
	void*               pDlgStr;            // buffer (malloced)
} *PHE_SEARCHOPTIONS;

//
// class HexEditWnd
//
class HexEditWnd
{

public:
	HFONT              hFont, hFontU;
	UINT               iyHETop;       // top of HexEdit area
	UINT               iyHEBottom;  
	BOOL               bHEOnTop, bRestoreWinPos, bUserWinPos;
	HE_WIN_POS         wpUser;
	procActionHandler  pActionHandler;
	BOOL               bHEVisible;    // indicates whether the main window is visible
	char               cIniPath[MAX_PATH];
	
	HexEditWnd();
	~HexEditWnd();
	BOOL          CreateMainWndThread();
	HWND          GetHEditWndHandle();
	void          SetHEditWndHandle(HWND hWnd);
	void          SetTBHandle(HWND hTB);
	HINSTANCE     GetInstance();
	void          SetVisible(BOOL bVisible);
	BOOL          SetHEDataInfo(PHE_DATA_INFO pDI);
	BOOL          DoEditFile(char* szFilePath, BOOL bForceReadOnly);
	BOOL          PaintText(HWND hWnd);
	void          HEHandleWM_SETFOCUS(HWND hWnd);
	void          HEHandleWM_KILLFOCUS(HWND hWnd);
	BOOL          HESetFont(HFONT hf);
	void          HEHandleWM_SIZE(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL          HEHandleLButton(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL          HEHandleWM_KEYDOWN(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL          MakeCaretVisible();
	BOOL          ValidatePos(PHE_POS ppos);
	BOOL          SetCaretSet(BOOL bSet);
	BOOL          HEHandleWM_VSCROLL(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void          HEHandleWM_MOUSEWHEEL(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void          HEHandleWM_SHOWWINDOW(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL          HEHandleWM_COMMAND(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL          HEHandleWM_NOTIFY(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void          InitGotoDlg(HWND hDlg);
	BOOL          GDHandleWM_COMMAND(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT       HEHandleWM_CHAR(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL          HEHandleWM_MOUSEMOVE(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void          InitSelBlockDlg(HWND hDlg);
	BOOL          SBHandleWM_COMMAND(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void          SSInitDlg(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL          SSHandleWM_COMMAND(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void          SSEnableItems(HWND hDlg);
	BOOL          SSHandleSS_OK(HWND hDlg);
	BOOL          DoSpecifySettings(PHE_SETTINGS ps);
	BOOL          IsResizingAllowed();
	BOOL          SetCaret(PHE_POS pos);
	BOOL          SetCaret(DWORD dwOffset);
	BOOL          SetCaret();
	BOOL          SetTopLine(int iNewLine);
	BOOL          SetTopLine(DWORD dwOffset);
	BOOL          SetTopLine();
	BOOL          SetSelection(DWORD dwOffStart, DWORD dwOffEnd);
	void          HEHandleWM_CLOSE(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL          SetHEWnd2Top(BOOL bTop);
	BOOL          HandleStartCaretPosSel(HWND hWnd);
	BOOL          HEditToTray();
	BOOL          HEditKillTrayIcon();
	BOOL          HEditReturnFromTray();
	void          HEHandleWM_TRAYMENU(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void          HEHandleWM_MOVE(HWND hWnd, WPARAM wParam, LPARAM lParam);

private:
	HINSTANCE          hDllBase;
	HWND               hMainWnd, hTB;
	CThread            GuiThread;
	HE_DATA_INFO       diData;        // working buffer
	HE_DATA_INFO       diOrgData;     // buffer with current file content
	FILE               fInput;
	UINT               uFontHeight, uFontWidth, uMaxLines;
	HE_STATUS          stat;
	HE_SEARCHOPTIONS   search;
	RECT               rctHE;         // rect of HexEdit area relative to HE client area
	UINT               cf16Edit;
	BOOL               bResizingAllowed, bMinToTray, bSaveWinPos;
	char               cSBText[200];
	NOTIFYICONDATA     nidTray;
	HMENU              hmTray;
	char               cInitialDir[MAX_PATH];
	RECT               rctLastPos;

	BOOL          IsOffsetVisible(DWORD dwOffset);
	BOOL          PointToPos(IN POINT *pp, OUT PHE_POS ppos);
	void          SetupVScrollbar();
	UINT          GetTotalLineNum();
	void          RepaintClientArea();
	void          RepaintClientAreaNow();
	//BOOL          PaintOffset(HWND hWnd, PHE_POS pos);
	void          Beep();
	void          ErrMsg(HWND hWnd, char* szText);
	void          ErrMsg(char* szText);
	void          ErrMsg(HWND hWnd, char* szText, char* szCaption);
	BOOL          IsOutOfRange(DWORD dwOffset);
	BOOL          IsOutOfRange(PHE_POS ppos);
    void          SetCaretPosData(PHE_POS ppos);
    void          SetCaretPosData(DWORD dwOffset);
	void          ConfigureTB();
	BOOL          SaveChanges();
	void          SetHEWndCaption();
	void          SetStatusText(char *szFormat, ...);
	BOOL          IsReadOnly();
	BOOL          KillSelection();
	BOOL          Point2Selection(LPPOINT ppClient);
	UINT          GetCurrentLine();
	BOOL          SetCurrentLine(UINT iLine);
	BOOL          ValidateLine(int *piLine);
	BOOL          PerformStrSearch(PHE_SEARCHOPTIONS pso);
	BOOL          PerformSearchAgain(PHE_SEARCHOPTIONS pso, BOOL bDown);
	BOOL          CopySelectedBlock();
	BOOL          CutSelectedBlock();
	BOOL          PasteBlockFromCB();
	BOOL          UndoChanges();
	void          SelectAll();
	BOOL          IsAllSelected();
	void          ShowAbout();
	void          InitEdition();
	void          QuitEdition();
	void          HEditQuit();
	BOOL          IsFileInput();
	void          HEditToTop();
	BOOL          IsKeyDown(int iVKey);
	void          SetCaretSelInfoToStatus();
};

//
// exports
//
DWORD __stdcall HEditWindowThread(void* pArg);

extern BOOL               bStartCaretPos;
extern HE_POS             posStartCaretPos;
extern BOOL               bStartSel;
extern DWORD              dwStartSelStart;
extern DWORD              dwStartSelEnd;
extern BOOL               bStartOnTop;

#endif