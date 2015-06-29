

#include "stdafx.h"
#include "服务管理.h"
#include "服务管理Dlg.h"
#include <windows.h> 
#include <Winsvc.h> 



BOOL EnablePriv();//提升进程权限
DWORD    CreateServer_StartType = SERVICE_AUTO_START;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	
	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString	m_server_name;
	CString	m_server_path;
	CString	m_server_info;
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	afx_msg void OnCreate();
	afx_msg void OnOpenFile();
	afx_msg void OnRadio_CreateServer_Manager();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_server_name = _T("");
	m_server_path = _T("");
	m_server_info = _T("");
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_ADD_SERVER_NAME, m_server_name);
	DDX_Text(pDX, IDC_ADD_SERVER_PATH, m_server_path);
	DDX_Text(pDX, IDC_ADD_SERVER_INFO, m_server_info);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//{{AFX_MSG_MAP(CAboutDlg)
ON_BN_CLICKED(IDOK, OnCreate)
ON_BN_CLICKED(IDC_BUTTON_OPEN_FILE, OnOpenFile)
ON_BN_CLICKED(IDC_RADIO2, OnRadio_CreateServer_Manager)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyDlg dialog

CMyDlg::CMyDlg(CWnd* pParent /*=NULL*/)
: CDialog(CMyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMyDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyDlg)
	DDX_Control(pDX, IDC_LIST1, m_list);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMyDlg, CDialog)
//{{AFX_MSG_MAP(CMyDlg)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_WM_CONTEXTMENU()
ON_COMMAND(ID_MENU_REF, OnMenuRef)
ON_COMMAND(ID_MENU_DEL, OnMenuDel)
ON_COMMAND(ID_MENU_START, OnMenuStart)
ON_COMMAND(ID_MENU_STOP, OnMenuStop)
ON_COMMAND(ID_MENU_ABOUT, OnMenuAbout)
ON_COMMAND(ID_MENU_CREATE, OnMenuCreate)
ON_COMMAND(ID_MENU_START_AUTO, OnMenuStartAuto)
ON_COMMAND(ID_MENU_START_DISABLE, OnMenuStartDisable)
ON_COMMAND(ID_MENU_START_MANAGER, OnMenuStartManager)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyDlg message handlers

BOOL CMyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
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
	
	
	m_list.InsertColumn(0,"服务名称",LVCFMT_LEFT,100,-1);
	m_list.InsertColumn(1,"显示名称",LVCFMT_LEFT,100,-1);
	m_list.InsertColumn(2,"启动状态",LVCFMT_LEFT,100,-1);
	m_list.InsertColumn(3,"启动类别",LVCFMT_LEFT,100,-1);
	m_list.InsertColumn(4,"程序路径",LVCFMT_LEFT,240,-1);
	
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}




void CMyDlg::OnSysCommand(UINT nID, LPARAM lParam)
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



void CMyDlg::OnPaint() 
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
HCURSOR CMyDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}




void CMyDlg::OnOK() 
{
	
	m_list.DeleteAllItems();
	UpdateData(false);
	
	
	LPQUERY_SERVICE_CONFIG ServicesInfo = NULL;
	LPENUM_SERVICE_STATUS lpServices    = NULL; 
	
	DWORD    nSize = 0; 
	DWORD    n; 
	DWORD    nResumeHandle = 0; 
	DWORD	 dwServiceType = SERVICE_WIN32; 
	
	SC_HANDLE schSCManager = NULL; 
	BOOL     Flag = FALSE; 
	
	
	
	if((schSCManager=OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS))==NULL)
	{
		AfxMessageBox("OpenSCManager Error\n");
	}
	
	lpServices = (LPENUM_SERVICE_STATUS) LocalAlloc(LPTR, 64 * 1024);        // Allocate Ram 
	
	EnumServicesStatus(schSCManager,
					   dwServiceType, 
					   SERVICE_STATE_ALL,
					   (LPENUM_SERVICE_STATUS)lpServices, 
					   64 * 1024, 
					   &nSize, 
					   &n, 
					   &nResumeHandle);
	
	
	for (unsigned long i = 0; i < n; i++) 
	{ 
		SC_HANDLE service = NULL;
		DWORD     nResumeHandle = 0; 
		
		service=OpenService(schSCManager,lpServices[i].lpServiceName,SERVICE_ALL_ACCESS);
		
		ServicesInfo = (LPQUERY_SERVICE_CONFIG) LocalAlloc(LPTR, 64 * 1024);        // Allocate Ram 
		
		QueryServiceConfig(service,ServicesInfo,64 * 1024,&nResumeHandle);
		
		
		int nitem=m_list.InsertItem(0,lpServices[i].lpServiceName);
		
		m_list.SetItemText(nitem,1,lpServices[i].lpDisplayName);
		
		if ( lpServices[i].ServiceStatus.dwCurrentState!=SERVICE_STOPPED)
		{
			m_list.SetItemText(nitem,2,"已启动");
		}
		
		
		if(2==ServicesInfo->dwStartType)
		{
			m_list.SetItemText(nitem,3,"自 动>>");
		}
		if(3==ServicesInfo->dwStartType)
		{
			m_list.SetItemText(nitem,3,"手 动");
		}
		if(4==ServicesInfo->dwStartType)
		{
			m_list.SetItemText(nitem,3,"禁 止");
		}
		
		m_list.SetItemText(nitem,4,ServicesInfo->lpBinaryPathName);
		
		UpdateData(false);
		
	}
	
}


void CMyDlg::OnCancel() 
{
	
	CDialog::OnCancel();
}



void CMyDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu popMenu;
	VERIFY(popMenu.LoadMenu(IDR_MENU_POPUP));
	CMenu *p=popMenu.GetSubMenu(0);
	ASSERT(p!=NULL);
	p->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,point.x,point.y,AfxGetMainWnd());
}




void CMyDlg::OnMenuRef() 
{
	OnOK();
}



void CMyDlg::OnMenuDel() 
{
	EnablePriv();
	
	char ServerName[100]=" ";
	
	int x=m_list.GetSelectionMark();
	m_list.GetItemText(x,0,ServerName,100);
	
	
	
	SC_HANDLE scm;
    SC_HANDLE service;
    SERVICE_STATUS status;
	
	
	if((scm=OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE))==NULL)
	{
		MessageBox("OpenSCManager Error\n");
	}
	
    service=OpenService(scm,ServerName,SERVICE_ALL_ACCESS|DELETE);
	
	if(!service)
	{
		MessageBox("OpenService error!\n");
	}
	
    BOOL isSuccess=QueryServiceStatus(service,&status);
    if (!isSuccess)
	{
        MessageBox("QueryServiceStatus error!\n");
	}
	
    if ( status.dwCurrentState!=SERVICE_STOPPED )
	{
		
        isSuccess=ControlService(service,SERVICE_CONTROL_STOP,&status);
        if (!isSuccess )
		{
            MessageBox("Stop Service error!\n");
		}
		
	}
	
	isSuccess=::DeleteService(service);
	if (!isSuccess)
	{
		MessageBox("删除服务失败！");
	}
	else
	{ 
		MessageBox("删除服务成功！");
	}
	CloseServiceHandle(service );
	CloseServiceHandle(scm);
	
	
}

void CMyDlg::OnMenuStart() 
{
	EnablePriv();
	
	char ServerName[100]=" ";
	
	int x=m_list.GetSelectionMark();
	m_list.GetItemText(x,0,ServerName,100);
	
	
	
	SC_HANDLE scm;
    SC_HANDLE service;
    SERVICE_STATUS status;
	
	
	if((scm=OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE))==NULL)
	{
		MessageBox("OpenSCManager Error\n");
	}
	
    service=OpenService(scm,ServerName,SERVICE_ALL_ACCESS);
	
	if(!service)
	{
		MessageBox("OpenService error!\n");
	}
	
    BOOL isSuccess=QueryServiceStatus(service,&status);
    if (!isSuccess)
	{
        MessageBox("QueryServiceStatus error!\n");
	}
	
    if ( status.dwCurrentState==SERVICE_STOPPED )
	{
		isSuccess=::StartService(service,NULL,NULL);
		
		if (!isSuccess)
		{
			MessageBox("启动服务失败！");
		}
		else
		{ 
			MessageBox("启动服务成功！");
		}
	}
	else
	{
		MessageBox("服务以经启动！");
	}
	
	CloseServiceHandle(service );
	CloseServiceHandle(scm);
	
}

void CMyDlg::OnMenuStop() 
{
	EnablePriv();
	
	char ServerName[100]=" ";
	
	int x=m_list.GetSelectionMark();
	m_list.GetItemText(x,0,ServerName,100);
	
	SC_HANDLE scm;
    SC_HANDLE service;
    SERVICE_STATUS status;
	
	if((scm=OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE))==NULL)
	{
		MessageBox("OpenSCManager Error\n");
	}
	
    service=OpenService(scm,ServerName,SERVICE_ALL_ACCESS);
	
	if(!service)
	{
		MessageBox("OpenService error!\n");
	}
	
    BOOL isSuccess=QueryServiceStatus(service,&status);
    if (!isSuccess)
	{
        MessageBox("QueryServiceStatus error!\n");
	}
	
    if ( status.dwCurrentState!=SERVICE_STOPPED )
	{
		
        isSuccess=ControlService(service,SERVICE_CONTROL_STOP,&status);
        if (!isSuccess )
		{
            MessageBox("停止服务失败！");
		}
		else
		{
			MessageBox("停止服务成功！");
		}
		
	}
	
}

void CMyDlg::OnMenuAbout() 
{
	MessageBox("        服务管理器1.0        \n\n          作者：冷风");
}

void CMyDlg::OnMenuCreate() 
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();	
}



void CAboutDlg::OnCreate() 
{
	EnablePriv();
	
	UpdateData(true);
	SC_HANDLE scm=NULL;
	SC_HANDLE service=NULL;
	
	if((scm=OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE))==NULL)
	{
		AfxMessageBox("OpenSCManager Error");
	}
	service=CreateService(
		scm,m_server_name,m_server_info, 
		SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, 
		CreateServer_StartType, SERVICE_ERROR_NORMAL,
		m_server_path, 0, 0, 0, 0, 0 );
	if(service)
		AfxMessageBox("服务创建成功");
	else
		AfxMessageBox("服务创建失败");
	
	CloseServiceHandle(service);
	CloseServiceHandle(scm);
}




void CAboutDlg::OnOpenFile() 
{
	CFileDialog dlg(true);
	dlg.DoModal();
	CString path=dlg.GetPathName();
	//更新控件
	m_server_path=path;
	UpdateData(false);	
}





void CMyDlg::OnMenuStartAuto() 
{
	EnablePriv();
	char ServerName[100]=" ";
	
	int x=m_list.GetSelectionMark();
	m_list.GetItemText(x,0,ServerName,100);
	
	
	SC_HANDLE scm;
    SC_HANDLE service;
	
	
	if((scm=OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE))==NULL)
	{
		MessageBox("OpenSCManager Error\n");
	}
	
    service=OpenService(scm,ServerName,SERVICE_ALL_ACCESS);
	
	if(!service)
	{
		MessageBox("OpenService error!\n");
	}
	
	
	
    SC_LOCK sclLock; 
    DWORD  dwStartType; 
	// Need to acquire database lock before reconfiguring. 
	
    sclLock = LockServiceDatabase(scm); 
	
    // If the database cannot be locked, report the details. 
	
    if (sclLock == NULL) 
    { 
		
        // Exit if the database is not locked by another process. 
		
        if (GetLastError() != ERROR_SERVICE_DATABASE_LOCKED) 
            MessageBox("LockServiceDatabase 失败\n      继续运行吗?"); 
		
        
    } 
	
    // The database is locked, so it is safe to make changes. 
	
    dwStartType = SERVICE_AUTO_START;
	
    if (! ChangeServiceConfig( 
        service,        // handle of service 
        SERVICE_NO_CHANGE, // service type: no change 
        dwStartType,       // change service start type 
        SERVICE_NO_CHANGE, // error control: no change 
        NULL,              // binary path: no change 
        NULL,              // load order group: no change 
        NULL,              // tag ID: no change 
        NULL,              // dependencies: no change 
        NULL,              // account name: no change
		NULL,				// password: no change
		NULL))				//displayname 
    {
        MessageBox("ChangeServiceConfig error!\n"); 
    }
    else 
        MessageBox("设置成功！方式为 自动 \n"); 
	
    // Release the database lock. 
	
    UnlockServiceDatabase(sclLock); 
	
    // Close the handle to the service. 
	
    CloseServiceHandle(service); 
	
}




void CMyDlg::OnMenuStartDisable() 
{
	
	EnablePriv();
	char ServerName[100]=" ";
	
	int x=m_list.GetSelectionMark();
	m_list.GetItemText(x,0,ServerName,100);
	
	
	SC_HANDLE scm;
    SC_HANDLE service;
	
	
	if((scm=OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE))==NULL)
	{
		MessageBox("OpenSCManager Error\n");
	}
	
    service=OpenService(scm,ServerName,SERVICE_ALL_ACCESS);
	
	if(!service)
	{
		MessageBox("OpenService error!\n");
	}
	
	
	
    SC_LOCK sclLock; 
    DWORD  dwStartType; 
	// Need to acquire database lock before reconfiguring. 
	
    sclLock = LockServiceDatabase(scm); 
	
    // If the database cannot be locked, report the details. 
	
    if (sclLock == NULL) 
    { 
		
        // Exit if the database is not locked by another process. 
		
        if (GetLastError() != ERROR_SERVICE_DATABASE_LOCKED) 
            MessageBox("LockServiceDatabase 失败\n      继续运行吗?"); 
		
        
    } 
	
    // The database is locked, so it is safe to make changes. 
	
    dwStartType = SERVICE_DISABLED;
	
    if (! ChangeServiceConfig( 
        service,        // handle of service 
        SERVICE_NO_CHANGE, // service type: no change 
        dwStartType,       // change service start type 
        SERVICE_NO_CHANGE, // error control: no change 
        NULL,              // binary path: no change 
        NULL,              // load order group: no change 
        NULL,              // tag ID: no change 
        NULL,              // dependencies: no change 
        NULL,              // account name: no change
		NULL,				// password: no change
		NULL))				//displayname 
    {
        MessageBox("ChangeServiceConfig error!\n"); 
    }
    else 
        MessageBox("设置成功！方式为 禁止"); 
	
    // Release the database lock. 
	
    UnlockServiceDatabase(sclLock); 
	
    // Close the handle to the service. 
	
    CloseServiceHandle(service); 
	
}

void CMyDlg::OnMenuStartManager() 
{
	
	EnablePriv();
	char ServerName[100]=" ";
	
	int x=m_list.GetSelectionMark();
	m_list.GetItemText(x,0,ServerName,100);
	
	
	SC_HANDLE scm;
    SC_HANDLE service;
	
	
	if((scm=OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE))==NULL)
	{
		MessageBox("OpenSCManager Error\n");
	}
	
    service=OpenService(scm,ServerName,SERVICE_ALL_ACCESS);
	
	if(!service)
	{
		MessageBox("OpenService error!\n");
	}
	
	
	
    SC_LOCK sclLock; 
    DWORD  dwStartType; 
	// Need to acquire database lock before reconfiguring. 
	
    sclLock = LockServiceDatabase(scm); 
	
    // If the database cannot be locked, report the details. 
	
    if (sclLock == NULL) 
    { 
		
        // Exit if the database is not locked by another process. 
		
        if (GetLastError() != ERROR_SERVICE_DATABASE_LOCKED) 
            MessageBox("LockServiceDatabase 失败\n      继续运行吗?"); 
		
        
    } 
	
    // The database is locked, so it is safe to make changes. 
	
    dwStartType = SERVICE_DEMAND_START;
	
    if (! ChangeServiceConfig( 
        service,        // handle of service 
        SERVICE_NO_CHANGE, // service type: no change 
        dwStartType,       // change service start type 
        SERVICE_NO_CHANGE, // error control: no change 
        NULL,              // binary path: no change 
        NULL,              // load order group: no change 
        NULL,              // tag ID: no change 
        NULL,              // dependencies: no change 
        NULL,              // account name: no change
		NULL,				// password: no change
		NULL))				//displayname 
    {
        MessageBox("ChangeServiceConfig error!\n"); 
    }
    else 
        MessageBox("设置成功！方式为 手动\n"); 
	
    // Release the database lock. 
	
    UnlockServiceDatabase(sclLock); 
	
    // Close the handle to the service. 
	
    CloseServiceHandle(service); 
	
	
}



void CAboutDlg::OnRadio_CreateServer_Manager() 
{
	CreateServer_StartType = SERVICE_DEMAND_START;	
}


BOOL EnablePriv()
{
	
	HANDLE hToken;
	if ( OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&hToken) )
	{
		TOKEN_PRIVILEGES tkp;
		
		LookupPrivilegeValue( NULL,SE_DEBUG_NAME,&tkp.Privileges[0].Luid );//修改进程权限
		tkp.PrivilegeCount=1;
		tkp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges( hToken,FALSE,&tkp,sizeof tkp,NULL,NULL );//通知系统修改进程权限
		
		return( (GetLastError()==ERROR_SUCCESS) );
		
	}
	
}

