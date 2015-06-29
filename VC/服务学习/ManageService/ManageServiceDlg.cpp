// ManageServiceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ManageService.h"
#include "ManageServiceDlg.h"
#include <Winsvc.h>

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
// CManageServiceDlg dialog

CManageServiceDlg::CManageServiceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CManageServiceDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CManageServiceDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CManageServiceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CManageServiceDlg)
	DDX_Control(pDX, IDC_LIST_SERVICE, m_ServiceList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CManageServiceDlg, CDialog)
	//{{AFX_MSG_MAP(CManageServiceDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RADIO_WIN32, OnRadioWin32)
	ON_BN_CLICKED(IDC_RADIO_DRIVER, OnRadioDriver)
	ON_BN_CLICKED(IDC_BTN_START, OnBtnStart)
	ON_BN_CLICKED(IDC_BTN_STOP, OnBtnStop)
	ON_BN_CLICKED(IDC_BTN_EXIT, OnBtnExit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CManageServiceDlg message handlers

BOOL CManageServiceDlg::OnInitDialog()
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
    InitServiceList();
    ShowServiceList();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CManageServiceDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CManageServiceDlg::OnPaint() 
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
HCURSOR CManageServiceDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

VOID CManageServiceDlg::InitServiceList()
{
    m_ServiceList.SetExtendedStyle(
                m_ServiceList.GetExtendedStyle()
                | LVS_EX_GRIDLINES
                | LVS_EX_FULLROWSELECT );

    m_ServiceList.InsertColumn(0, "序号");
    m_ServiceList.InsertColumn(1, "服务名");
    m_ServiceList.InsertColumn(2, "显示名");
    m_ServiceList.InsertColumn(3, "状态");

    m_ServiceList.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
    m_ServiceList.SetColumnWidth(1, 100);
    m_ServiceList.SetColumnWidth(2, 250);
    m_ServiceList.SetColumnWidth(3, LVSCW_AUTOSIZE_USEHEADER);
}

VOID CManageServiceDlg::ShowServiceList(DWORD dwServiceType)
{
    m_ServiceList.DeleteAllItems();

    SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if ( NULL == hSCM )
    {
        AfxMessageBox("OpenSCManager Error");
        return ;
    }

    DWORD dwBufSize = 512 * sizeof(ENUM_SERVICE_STATUS);
    DWORD dwByteNeeded, dwServicesReturned, lpResumeHandle = 0;
    ENUM_SERVICE_STATUS SerStatus[512] = { 0 };
    BOOL bRet = EnumServicesStatus(hSCM,
                    dwServiceType,
                    SERVICE_STATE_ALL,
                    SerStatus,
                    dwBufSize,
                    &dwByteNeeded,
                    &dwServicesReturned,
                    &lpResumeHandle);


    if ( FALSE == bRet )
    {
        AfxMessageBox("EnumServiceStatus Error");
        CloseServiceHandle(hSCM);
        return ;
    }

    for ( DWORD i = 0; i < dwServicesReturned; i ++ )
    {
        CString str;
        str.Format("%d", i);
        m_ServiceList.InsertItem(i, str);
        m_ServiceList.SetItemText(i, 1, SerStatus[i].lpServiceName);
        m_ServiceList.SetItemText(i, 2, SerStatus[i].lpDisplayName);
        switch ( SerStatus[i].ServiceStatus.dwCurrentState )
        {
        case SERVICE_PAUSED:
            {
                m_ServiceList.SetItemText(i, 3, "暂停");
                break;
            }
        case SERVICE_STOPPED:
            {
                m_ServiceList.SetItemText(i, 3, "停止");
                break;
            }
        case SERVICE_RUNNING:
            {
                m_ServiceList.SetItemText(i, 3, "运行");
                break;
            }
        default:
            {
                m_ServiceList.SetItemText(i, 3, "其他");
            }
        }
    }

    CloseServiceHandle(hSCM);
}

void CManageServiceDlg::OnRadioWin32() 
{
	// TODO: Add your control notification handler code here
	ShowServiceList(SERVICE_WIN32);
}

void CManageServiceDlg::OnRadioDriver() 
{
	// TODO: Add your control notification handler code here
	ShowServiceList(SERVICE_DRIVER);
}

void CManageServiceDlg::OnBtnStart() 
{
	// TODO: Add your control notification handler code here
    // 选中服务的的索引
	POSITION Pos = m_ServiceList.GetFirstSelectedItemPosition();
    int nSelect = -1;

    while ( Pos )
    {
        nSelect = m_ServiceList.GetNextSelectedItem(Pos);
    }

    if ( -1 == nSelect )
    {
        AfxMessageBox("请选择要启动的服务");
        return ;
    }

    // 获取选中服务的服务名
    char szServiceName[MAXBYTE] = { 0 };
    m_ServiceList.GetItemText(nSelect, 1, szServiceName, MAXBYTE);

    SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if ( NULL == hSCM )
    {
        AfxMessageBox("OpenSCManager Error");
        return ;
    }

    SC_HANDLE hSCService = OpenService(hSCM, szServiceName, SERVICE_ALL_ACCESS);
    
    // 启动服务
    BOOL bRet = StartService(hSCService, 0, NULL);
    if ( bRet == TRUE )
    {
        m_ServiceList.SetItemText(nSelect, 3, "运行");
    }
    else
    {
        int n = GetLastError();
    }

    CloseServiceHandle(hSCService);
    CloseServiceHandle(hSCM);
}

void CManageServiceDlg::OnBtnStop() 
{
	// TODO: Add your control notification handler code here
    // 选中服务的的索引
    POSITION Pos = m_ServiceList.GetFirstSelectedItemPosition();
    int nSelect = -1;
    
    while ( Pos )
    {
        nSelect = m_ServiceList.GetNextSelectedItem(Pos);
    }
    
    if ( -1 == nSelect )
    {
        AfxMessageBox("请选择要停止的服务");
        return ;
    }
    
    // 获取选中服务的服务名
    char szServiceName[MAXBYTE] = { 0 };
    m_ServiceList.GetItemText(nSelect, 1, szServiceName, MAXBYTE);
    
    SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if ( NULL == hSCM )
    {
        AfxMessageBox("OpenSCManager Error");
        return ;
    }
    
    SC_HANDLE hSCService = OpenService(hSCM, szServiceName, SERVICE_ALL_ACCESS);
    SERVICE_STATUS ServiceStatus;
    // 停止服务
    BOOL bRet = ControlService(hSCService, SERVICE_CONTROL_STOP, &ServiceStatus);
    if ( bRet == TRUE )
    {
        m_ServiceList.SetItemText(nSelect, 3, "停止");
    }
    else
    {
        int n = GetLastError();
    }
    
    CloseServiceHandle(hSCService);
    CloseServiceHandle(hSCM);
}

void CManageServiceDlg::OnBtnExit() 
{
	// TODO: Add your control notification handler code here
	EndDialog(0);
}
