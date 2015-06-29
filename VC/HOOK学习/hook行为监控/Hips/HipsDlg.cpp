// HipsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Hips.h"
#include "HipsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHipsDlg dialog

CHipsDlg::CHipsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHipsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHipsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHipsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHipsDlg)
	DDX_Control(pDX, IDC_LIST_HOOKLOG, m_HipsReports);
	DDX_Control(pDX, IDC_BTN_OFF, m_BtnOff);
	DDX_Control(pDX, IDC_BTN_ON, m_BtnOn);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CHipsDlg, CDialog)
	//{{AFX_MSG_MAP(CHipsDlg)
    ON_MESSAGE(UM_TRAYNOTIFICATION, OnTrayNotification)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_COPYDATA()
	ON_BN_CLICKED(IDC_BTN_ON, OnBtnOn)
	ON_BN_CLICKED(IDC_BTN_OFF, OnBtnOff)
    ON_BN_CLICKED(IDC_BTN_CLEARLOG, OnBtnClearlog)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHipsDlg message handlers

BOOL CHipsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
    InitHipsReports();
    InitIconNotify();

    m_BtnOn.EnableWindow(TRUE);
    m_BtnOff.EnableWindow(FALSE);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHipsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CHipsDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CHipsDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
#include "tips.h"

#define HIPS_CREATEPROCESS  0x00000001L
#define HIPS_REGSETVALUE    0x00000002L
#define HIPS_REGDELETEVALUE 0x00000003L

typedef struct _HIPS_INFO
{
    WCHAR wProcessName[0x200];
    DWORD dwHipsClass;
}HIPS_INFO, *PHIPS_INFO;

BOOL CHipsDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct) 
{
	// TODO: Add your message handler code here and/or call default

    CTips Tips;
    PHIPS_INFO pHipsInfo = (PHIPS_INFO)pCopyDataStruct->lpData;
    wcscpy(Tips.sz, pHipsInfo->wProcessName);

    Tips.DoModal();
    
    int nNum = m_HipsReports.GetItemCount();
    CString Str;
    Str.Format("%d", nNum);
    m_HipsReports.InsertItem(nNum, Str);

    SYSTEMTIME StTime;
    GetLocalTime(&StTime);
    Str.Format("%04d/%02d/%02d %02d:%02d:%02d",
            StTime.wYear,
            StTime.wMonth,
            StTime.wDay,
            StTime.wHour,
            StTime.wMonth,
            StTime.wSecond);
    m_HipsReports.SetItemText(nNum, 1, Str);
    Str.Format("%S", Tips.sz);
    m_HipsReports.SetItemText(nNum, 2, Str);

    switch ( pHipsInfo->dwHipsClass )
    {
    case HIPS_CREATEPROCESS:
        {
            Str = "进程创建";
            break;
        }
    case HIPS_REGSETVALUE:
        {
            break;
        }
    case HIPS_REGDELETEVALUE:
        {
            break;
        }
    }
    m_HipsReports.SetItemText(nNum, 3, Str);

    Str.Format("%s", Tips.bRet ? "放行" : "拦截");
    m_HipsReports.SetItemText(nNum, 4, Str);

    if ( Tips.bRet )
    {
        return 0;
    } 
    else
    {
        return -1;
    }

	return CDialog::OnCopyData(pWnd, pCopyDataStruct);
}

// extern "C" VOID SetHookOn(HWND hWnd);
// extern "C" VOID SetHookOff();
// 
// #pragma comment (lib, "EasyHips")

typedef VOID (*SETHOOKON)(HWND);
typedef VOID (*SETHOOKOFF)();

void CHipsDlg::OnBtnOn() 
{
	// TODO: Add your control notification handler code here
/*	SetHookOn(m_hWnd);*/
    m_hInst = LoadLibrary("EasyHips.dll");
    SETHOOKON SetHookOn = (SETHOOKON)GetProcAddress(m_hInst, "SetHookOn");
    
    SetHookOn(GetSafeHwnd());
    FreeLibrary(m_hInst);
    m_BtnOn.EnableWindow(FALSE);
    m_BtnOff.EnableWindow(TRUE);
}

void CHipsDlg::OnBtnOff() 
{
	// TODO: Add your control notification handler code here
    /*	SetHookOff();*/
    m_hInst = GetModuleHandle("EasyHips.dll");
    SETHOOKOFF SetHookOff = (SETHOOKOFF)GetProcAddress(m_hInst, "SetHookOff");
    SetHookOff();
    CloseHandle(m_hInst);
    FreeLibrary(m_hInst);
    m_BtnOn.EnableWindow(TRUE);
    m_BtnOff.EnableWindow(FALSE);
}

VOID CHipsDlg::InitHipsReports()
{
    m_HipsReports.SetExtendedStyle(m_HipsReports.GetExtendedStyle()
                            | LVS_EX_GRIDLINES);

    m_HipsReports.InsertColumn(0, "序号");
    m_HipsReports.InsertColumn(1, "时间");
    m_HipsReports.InsertColumn(2, "进程");
    m_HipsReports.InsertColumn(3, "类型");
    m_HipsReports.InsertColumn(4, "事件");

    m_HipsReports.SetColumnWidth(0, 20);
    m_HipsReports.SetColumnWidth(1, 80);
    m_HipsReports.SetColumnWidth(2, 200);
    m_HipsReports.SetColumnWidth(3, 100);
    m_HipsReports.SetColumnWidth(4, 50);
}

void CHipsDlg::OnBtnClearlog() 
{
	// TODO: Add your control notification handler code here
	m_HipsReports.DeleteAllItems();
}

BOOL CHipsDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
    switch(LOWORD(wParam))
    {
    case IDM_EXIT:
        {
            OnDestroy();
            break;
        }
    case IDM_SHOW:
        {
            this->ShowWindow(SW_SHOW);
            Shell_NotifyIcon(NIM_DELETE, &m_Nid);
            break;
        }
    }
	
	return CDialog::OnCommand(wParam, lParam);
}

VOID CHipsDlg::InitIconNotify()
{
    m_Nid.cbSize = sizeof(NOTIFYICONDATA);
    m_Nid.hWnd = this->GetSafeHwnd();
    m_Nid.uID = IDR_MAINFRAME/*ID_TASKBARICON*/;
    m_Nid.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP;
    m_Nid.uCallbackMessage = UM_TRAYNOTIFICATION;
    m_Nid.hIcon = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 16, 16, 0);
    strcpy(m_Nid.szTip, "Easy Hips For R3");
}

LRESULT CHipsDlg::OnTrayNotification(WPARAM wId, LPARAM lEvent)
{
    if( wId != m_Nid.uID || (lEvent != WM_LBUTTONUP && lEvent != WM_RBUTTONUP) )
    {
        return 0;
    }
    
    //加载菜单
    CMenu menu;
    if(!menu.LoadMenu(IDR_TRAYMENU))
    {
        return 0;
    }   
    
    //获取弹出菜单
    CMenu *pSubMenu = menu.GetSubMenu(0);
    if(!pSubMenu)
    {
        return 0;
    }
    
    if(lEvent == WM_RBUTTONUP)
    {
        //设置默认菜单项
        ::SetMenuDefaultItem(pSubMenu->m_hMenu, 0, TRUE);
        
        //获取鼠标位置
        CPoint mouse;
        GetCursorPos(&mouse);
        
        //设置快捷菜单
        ::SetForegroundWindow(m_Nid.hWnd);
        ::TrackPopupMenu(pSubMenu->m_hMenu, 0, mouse.x, mouse.y, 0, m_Nid.hWnd, NULL);
    }
    else
    {
        ::SendMessage(m_Nid.hWnd, WM_COMMAND, pSubMenu->GetMenuItemID(0), 0);
    }
    
    return 0;
}

void CHipsDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
    Shell_NotifyIcon(NIM_ADD, &m_Nid);
    ShowWindow(SW_HIDE);
//This function sends a message to the system to add, modify, 
//or delete an icon from the taskbar status area.	
	// CDialog::OnClose();
}

void CHipsDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
//     m_hInst = GetModuleHandle("EasyHips.dll");
//     SETHOOKOFF SetHookOff = (SETHOOKOFF)GetProcAddress(m_hInst, "SetHookOff");
//     SetHookOff();
//     CloseHandle(m_hInst);
	ExitProcess(0);
}
