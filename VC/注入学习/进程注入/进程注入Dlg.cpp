// 进程注入Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "进程注入.h"
#include "进程注入Dlg.h"
#include <Tlhelp32.h>

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
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON1, Onzhuru)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyDlg message handlers

BOOL CMyDlg::OnInitDialog()
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

	UpdateMain();
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

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



void CMyDlg::UpdateMain()
{
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);

	m_list.InsertColumn(0,"序号",LVCFMT_LEFT,50);
	m_list.InsertColumn(1,"进程名",LVCFMT_LEFT,200);
	m_list.InsertColumn(2,"PID",LVCFMT_LEFT,100);
	GetClientRect(m_rect);
	


	HANDLE hProcessSnap = NULL;
	
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	 if (INVALID_HANDLE_VALUE == hProcessSnap)
    {
        MessageBox("创建进程快照失败");
        return;
    }

		PROCESSENTRY32 processentry32 = {sizeof(PROCESSENTRY32W)};

		BOOL flag = Process32First(hProcessSnap,&processentry32);
		int i=0;
		 m_list.DeleteAllItems();//先删除所有的行,方便再次运行
		 if (!PromoteProcessPrivileges())
		 {
			 MessageBox(_T("提升进程权限失败"));
        }

		 while (flag)
		{
			CString buff;
			buff.Format("%d",i);
			
			m_list.InsertItem(i,buff.GetBuffer(buff.GetLength()));
					
			m_list.SetItemText(i,1,processentry32.szExeFile);

			buff.Format("%u",processentry32.th32ProcessID);
			m_list.SetItemText(i,2,buff.GetBuffer(buff.GetLength()));

			flag = Process32Next(hProcessSnap,&processentry32);
			zhu[i].PID = processentry32.th32ProcessID;
		
			strcpy(zhu[i].szName ,buff.GetBuffer(buff.GetLength()));
		
			i++;
		}
		 CloseHandle(hProcessSnap);
		UpdateData(FALSE);
}

 BOOL CMyDlg::PromoteProcessPrivileges()//提升当前进程权限
 {
	 HANDLE Token;
	
		if (OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&Token))
		{
			TOKEN_PRIVILEGES  TokenPrivileges;
			TokenPrivileges.PrivilegeCount = 1;

			if (!LookupPrivilegeValue(NULL,"SeSecurityPrivilege",&TokenPrivileges.Privileges[0].Luid))
			{
				MessageBox("LookupPrivilegeValue失败");
				return FALSE;
			}
			TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
			if (!AdjustTokenPrivileges(Token,FALSE,&TokenPrivileges,sizeof(TokenPrivileges),NULL,NULL))
			{
				MessageBox("AdjustTokenPrivileges失败");
				return FALSE;
			}
		}
		CloseHandle(Token);
		return TRUE;
		
		
 }


void CMyDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnClose();
}

BOOL CMyDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN)
	{
		int virtKey = (int)pMsg->wParam;
		if (virtKey == VK_RETURN || virtKey == VK_ESCAPE)
		{
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CMyDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (nType == SIZE_MINIMIZED)
	{
		return;
	}
	CWnd *pWnd;
	pWnd = GetDlgItem(IDC_LIST1);
	if (pWnd)
	{
		CRect rect,rect_l;
		GetClientRect(&rect_l);
		pWnd->GetWindowRect(&rect);
		
		ScreenToClient(&rect);
		
		rect.right = cx;
		rect.bottom = rect.bottom + (rect_l.bottom - m_rect.bottom);
		pWnd->MoveWindow(rect);

		GetClientRect(m_rect);

	}
}

void WINAPI CMyDlg::MyThread(void *MyArg) //远程进程
{
	RemoteStruct *MyRemoteStruct2=(RemoteStruct*)MyArg;
	MyMessageBox My;
	My=(MyMessageBox)MyRemoteStruct2->Address; //指向MessageBox地址
	My(NULL,MyRemoteStruct2->MyText,MyRemoteStruct2->MyCaption,MB_OK); //调用函数
}

void CMyDlg::Onzhuru() 
{
	// TODO: Add your control notification handler code here

	POSITION pos_s;
	pos_s=m_list.GetFirstSelectedItemPosition();
	
	int selected = m_list.GetNextSelectedItem(pos_s);

	DWORD Pid=0;
	char FunctionText[255]={NULL}; //内容
	char FunctionCaption[255]={NULL}; //标题
	
	
	LPVOID lpArgBuf=NULL; //申请的空间(参数)
	LPVOID lpFunBuf=NULL; //申请的空间(函数)
	
	DWORD WriteByteArg=0; //参数实际写入字节数
	DWORD WriteByteFun=0; //函数实际写入字节数

	//MyRemoteStruct.Address=(DWORD)(MessageBox); //取得函数地址
	RemoteStruct MyRemoteStruct;
	
	MyRemoteStruct.Address = (PROC)(GetProcAddress(LoadLibrary("user32.dll"),"MessageBoxA"));
	UpdateData();
	CString szCapton,szText;
	MyRemoteStruct.PID = zhu[selected].PID;

	GetDlgItem(IDC_EDIT1)->GetWindowText(szCapton);
	strcpy(MyRemoteStruct.MyCaption,szCapton.GetBuffer(szCapton.GetLength()));
	GetDlgItem(IDC_EDIT2)->GetWindowText(szText);
	strcpy(MyRemoteStruct.MyText,szText.GetBuffer(szText.GetLength()));

	HANDLE ProcessHandle=::OpenProcess(PROCESS_ALL_ACCESS,FALSE,zhu[selected].PID); //打开进程
	if (NULL == ProcessHandle)
	{
		MessageBox("OpenProcess错误");
		CloseHandle(ProcessHandle);
		return ;
	}
	lpArgBuf = ::VirtualAllocEx(ProcessHandle,0,1024,MEM_COMMIT,PAGE_EXECUTE_READWRITE);
	if (NULL == lpArgBuf)
	{
		MessageBox("VirtualAllocEx1错误");
		CloseHandle(ProcessHandle);
		return ;
	}
	if (!WriteProcessMemory(ProcessHandle,lpArgBuf,&MyRemoteStruct,sizeof(RemoteStruct),&WriteByteArg))
	{
		MessageBox("WriteProcessMemory错误");
		CloseHandle(ProcessHandle);
		return ;
	}
	
	lpFunBuf = ::VirtualAllocEx(ProcessHandle,0,4096,MEM_COMMIT,PAGE_EXECUTE_READWRITE);
	if (NULL == lpFunBuf)
	{
		MessageBox("VirtualAllocEx2错误");
		CloseHandle(ProcessHandle);
		return ;
	}
	if (!WriteProcessMemory(ProcessHandle,lpFunBuf,&MyThread,4096,&WriteByteFun))
	{
		MessageBox("WriteProcessMemory错误");
		CloseHandle(ProcessHandle);
		return ;
	}
	DWORD Threadid = NULL;
	HANDLE ThreadHandle = ::CreateRemoteThread(ProcessHandle,0,0,\
		(LPTHREAD_START_ROUTINE)lpFunBuf,lpArgBuf,0,&Threadid);

	::WaitForSingleObject(ThreadHandle,INFINITE);
	VirtualFreeEx(ProcessHandle,lpArgBuf,1024,MEM_DECOMMIT);
	VirtualFreeEx(ProcessHandle,lpFunBuf,4096,MEM_DECOMMIT);

	CloseHandle(ProcessHandle);
	CloseHandle(ThreadHandle);
	MessageBox("注入成功");
	return;
}
