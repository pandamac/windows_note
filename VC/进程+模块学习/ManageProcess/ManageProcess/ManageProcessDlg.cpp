// ManageProcessDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ManageProcess.h"
#include "ManageProcessDlg.h"
#include <Tlhelp32.h>
#include "Create.h"

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
// CManageProcessDlg dialog

CManageProcessDlg::CManageProcessDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CManageProcessDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CManageProcessDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CManageProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CManageProcessDlg)
	DDX_Control(pDX, IDC_LIST_MODULE, m_ListModule);
	DDX_Control(pDX, IDC_LIST_PROCESS, m_ListProcess);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CManageProcessDlg, CDialog)
	//{{AFX_MSG_MAP(CManageProcessDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_EXIT, OnBtnExit)
	ON_BN_CLICKED(IDC_BTN_SHOWMODULE, OnBtnShowmodule)
	ON_BN_CLICKED(IDC_BTN_TERMINATE, OnBtnTerminate)
	ON_BN_CLICKED(IDC_BTN_STOP, OnBtnStop)
	ON_BN_CLICKED(IDC_BTN_RESUME, OnBtnResume)
	ON_BN_CLICKED(IDC_BTN_CREATE, OnBtnCreate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CManageProcessDlg message handlers

BOOL CManageProcessDlg::OnInitDialog()
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
    // DebugPrivilege();
    InitProcessList();
    ShowProcess();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CManageProcessDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CManageProcessDlg::OnPaint() 
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
HCURSOR CManageProcessDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

VOID CManageProcessDlg::InitProcessList()
{
    m_ListProcess.SetExtendedStyle(
                m_ListProcess.GetExtendedStyle()
                | LVS_EX_FULLROWSELECT 
                | LVS_EX_GRIDLINES );

    m_ListProcess.InsertColumn(0, "序号");
    m_ListProcess.InsertColumn(1, "进程名");
    m_ListProcess.InsertColumn(2, "进程ID");

    m_ListProcess.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
    m_ListProcess.SetColumnWidth(1, 200);
    m_ListProcess.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);

    m_ListModule.SetExtendedStyle(
                m_ListModule.GetExtendedStyle()
                | LVS_EX_FULLROWSELECT
                | LVS_EX_GRIDLINES );

    m_ListModule.InsertColumn(0, "序号");
    m_ListModule.InsertColumn(1, "DLL名");
    m_ListModule.InsertColumn(2, "DLL路径");
    
    m_ListModule.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
    m_ListModule.SetColumnWidth(1, 100);
    m_ListModule.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);
}

void CManageProcessDlg::OnBtnExit() 
{
	// TODO: Add your control notification handler code here
	EndDialog(0);
}

VOID CManageProcessDlg::ShowProcess()
{
    m_ListProcess.DeleteAllItems();

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if ( hSnap == INVALID_HANDLE_VALUE )
    {
        AfxMessageBox("CreateToolhelp32Snapshot Error");
        return ;	
    }

    PROCESSENTRY32 Pe32 = { 0 };
    Pe32.dwSize = sizeof(PROCESSENTRY32);

    BOOL bRet = Process32First(hSnap, &Pe32);
    int i = 0;
    CString str;

    while ( bRet )
    {
        str.Format("%d", i);
        m_ListProcess.InsertItem(i, str);
        m_ListProcess.SetItemText(i, 1, Pe32.szExeFile);
        str.Format("%d", Pe32.th32ProcessID);
        m_ListProcess.SetItemText(i, 2, str);

        i ++;
        bRet = Process32Next(hSnap, &Pe32);
    }
}

VOID CManageProcessDlg::ShowModule()
{
    m_ListModule.DeleteAllItems();

    int nPid = GetSelectPid();

    MODULEENTRY32 Me32 = { 0 };
    Me32.dwSize = sizeof(MODULEENTRY32);
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, nPid);
    if ( hSnap == INVALID_HANDLE_VALUE )
    {
        AfxMessageBox("CreateToolhelp32Snapshot Error");
        return ;
    }
    BOOL bRet = Module32First(hSnap, &Me32);
    int i = 0;
    CString str;
    while ( bRet )
    {
        str.Format("%d", i);
        m_ListModule.InsertItem(i, str);
        m_ListModule.SetItemText(i, 1, Me32.szModule);
        m_ListModule.SetItemText(i, 2, Me32.szExePath);
        i ++;
        bRet = Module32Next(hSnap, &Me32);
    }
}



void CManageProcessDlg::OnBtnShowmodule() 
{
	// TODO: Add your control notification handler code here
	ShowModule();
}


VOID CManageProcessDlg::DebugPrivilege()
{
    HANDLE hToken = NULL;

    BOOL bRet = OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken);
    
    if ( bRet == TRUE )
    {
        TOKEN_PRIVILEGES tp;
        tp.PrivilegeCount = 1;
        LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
        
        CloseHandle(hToken);
    }
}

void CManageProcessDlg::OnBtnTerminate() 
{
	// TODO: Add your control notification handler code here
    int nPid = GetSelectPid();

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, nPid);
    
    TerminateProcess(hProcess, 0);

    CloseHandle(hProcess);

    ShowProcess();

    m_ListModule.DeleteAllItems();
}


void CManageProcessDlg::OnBtnStop() 
{
	// TODO: Add your control notification handler code here
	int nPid = -1;
    nPid = GetSelectPid();

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, nPid);
    if ( hSnap == INVALID_HANDLE_VALUE )
    {
        AfxMessageBox("CreateToolhelp32Snapshot Error");
        return ;
    }

    THREADENTRY32 Te32 = { 0 };
    Te32.dwSize = sizeof(THREADENTRY32);
    BOOL bRet = Thread32First(hSnap, &Te32);
    
    while ( bRet )
    {
        if ( Te32.th32OwnerProcessID == nPid )
        {
            HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, Te32.th32ThreadID);
            
            SuspendThread(hThread);
            
            CloseHandle(hThread);
        }

        bRet = Thread32Next(hSnap, &Te32);
    }
}

void CManageProcessDlg::OnBtnResume() 
{
	// TODO: Add your control notification handler code here
    int nPid = -1;
    nPid = GetSelectPid();
    
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, nPid);
    if ( hSnap == INVALID_HANDLE_VALUE )
    {
        AfxMessageBox("CreateToolhelp32Snapshot Error");
        return ;
    }
    
    THREADENTRY32 Te32 = { 0 };
    Te32.dwSize = sizeof(THREADENTRY32);
    BOOL bRet = Thread32First(hSnap, &Te32);
    
    while ( bRet )
    {
        if ( Te32.th32OwnerProcessID == nPid )
        {
            HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, Te32.th32ThreadID);
            
            ResumeThread(hThread);
            
            CloseHandle(hThread);
        }
        
        bRet = Thread32Next(hSnap, &Te32);
    }
}

int CManageProcessDlg::GetSelectPid()
{
    int nPid = -1;
    
    POSITION Pos = m_ListProcess.GetFirstSelectedItemPosition();
    int nSelect = -1;
    while ( Pos )
    {
        nSelect = m_ListProcess.GetNextSelectedItem(Pos);
    }
    
    if ( -1 == nSelect )
    {
        AfxMessageBox("请选中要显示DLL的进程");
        return -1;
    }
    
    char  szPid[10] = { 0 };

    m_ListProcess.GetItemText(nSelect, 2, szPid, 10);
    nPid = atoi(szPid);

    return nPid;
}

void CManageProcessDlg::OnBtnCreate() 
{
	// TODO: Add your control notification handler code here
    CCreate CreateDlg;
    CreateDlg.DoModal();

    if ( strlen(CreateDlg.szFilePath) > 0 )
    {
        STARTUPINFO si = { 0 };
        PROCESS_INFORMATION pi;
        si.cb = sizeof(STARTUPINFO);
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_SHOWNORMAL;
        BOOL bRet = CreateProcess(CreateDlg.szFilePath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
        if ( bRet == FALSE )
        {
            AfxMessageBox("请检查输入的文件是否正确");
            return ;
        }
        else
        {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
    }
    else
    {
        AfxMessageBox("请输入完整的内容");
        return ;
    }

	ShowProcess();
    m_ListModule.DeleteAllItems();
}