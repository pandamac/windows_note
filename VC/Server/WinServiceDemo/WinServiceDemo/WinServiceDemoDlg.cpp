// WinServiceDemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WinServiceDemo.h"
#include "WinServiceDemoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CWinServiceDemoDlg 对话框




CWinServiceDemoDlg::CWinServiceDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWinServiceDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWinServiceDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWinServiceDemoDlg, CDialog)
//	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_CHECK, &CWinServiceDemoDlg::OnBnClickedButtonCheck)
	ON_BN_CLICKED(IDOK, &CWinServiceDemoDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CWinServiceDemoDlg::OnBnClickedCancel)
	ON_WM_TIMER()
	ON_MESSAGE(WM_SHOWTASK, &CWinServiceDemoDlg::OnShowTask)
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()


// CWinServiceDemoDlg 消息处理程序

BOOL CWinServiceDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_bMsbox = FALSE;
	this->SetWindowText("服务保护程序");
	memset(m_szTipShowInfo,0,sizeof(m_szTipShowInfo));
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	wsprintf(m_szTipShowInfo, "服务保护程序\n%04d-%02d-%02d %02d:%02d Start", 
	sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

//void CWinServiceDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
//{
//	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
//	{
//		CAboutDlg dlgAbout;
//		dlgAbout.DoModal();
//	}
//	else
//	{
//		CDialog::OnSysCommand(nID, lParam);
//	}
//}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CWinServiceDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CWinServiceDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CWinServiceDemoDlg::OnBnClickedButtonCheck()
{
	this->UpdateData();
	CString strService;
	this->GetDlgItem(IDC_EDIT_SERVICE_NAME)->GetWindowText(strService);
	if(strService=="")
	{
		MessageBox("请先输入服务名称！");
		return;
	}
	BYTE bRet = m_Service.CheckServiceStatus(strService);
	switch(bRet)
	{
	case m_Service.SEV_ERROR:
		MessageBox("服务错误");
		break;
	case m_Service.SEV_NO:
		MessageBox("没有该服务");
		break;
	case m_Service.SEV_RUNING:
		MessageBox("服务正在运行");
		break;
	case m_Service.SEV_STOPED:
		MessageBox("服务停止");
		break;
	default:
			break;
	}
}

void CWinServiceDemoDlg::OnBnClickedOk()
{
	m_bMsbox = FALSE;
	this->UpdateData();
	CString strTime;
	this->GetDlgItem(IDC_EDIT_CHECK_TIME)->GetWindowText(strTime);
	int nTime = 5000;
	
	if(atof(strTime)>0.001)
	{
		nTime = (int)(atof(strTime) *1000);
	}
	
	KillTimer(1);
	SetTimer(1,nTime,NULL);
}

void CWinServiceDemoDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	
	int iRet =MessageBox("是否要退出服务保护程序",0,MB_YESNO);

	if(iRet == 7)
	{
		return;
	}
	KillTimer(1);
	OnCancel();
}

void CWinServiceDemoDlg::OnTimer(UINT_PTR nIDEvent)
{
	this->UpdateData();
	CString strService;
	this->GetDlgItem(IDC_EDIT_SERVICE_NAME)->GetWindowText(strService);
	if(strService=="")
	{
		MessageBox("请先输入服务名称！");
		return;
	}
	if(m_Service.SEV_NO ==  m_Service.CheckServiceStatus(strService))
	{
		if(!m_bMsbox)
		{
			MessageBox("没有该服务名称不能保护！");
			m_bMsbox = TRUE;
		}
		return;
	}
	m_bMsbox = FALSE;
	if(m_Service.SEV_STOPED == m_Service.CheckServiceStatus(strService))
	{
		m_Service.StartSevice(strService);
	}
	
	CDialog::OnTimer(nIDEvent);
}

void CWinServiceDemoDlg::ShowIconToTip(BOOL bShow)
{
	if(bShow)
	{
		memset(&m_TipIcon, 0, sizeof(NOTIFYICONDATA));
		m_TipIcon.cbSize=(DWORD)sizeof(NOTIFYICONDATA);
		m_TipIcon.hWnd=this->m_hWnd;
		m_TipIcon.uID=IDR_MAINFRAME;
		m_TipIcon.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP;
		m_TipIcon.uCallbackMessage=WM_SHOWTASK;//自定义的消息名称
		m_TipIcon.hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
		strcpy(m_TipIcon.szTip,m_szTipShowInfo);
		Shell_NotifyIcon(NIM_ADD,&m_TipIcon);//在托盘区添加图标
		ShowWindow(SW_HIDE);
	}
	else
	{
		ShowWindow(SW_SHOWNORMAL);
		Shell_NotifyIcon(NIM_DELETE,&m_TipIcon);
	}

}
void CWinServiceDemoDlg::ShowBalloonTip(const char *pMsg, const char *pTitle, UINT uTimeout, DWORD dwInfoFlags)
{
	m_TipIcon.cbSize=sizeof(NOTIFYICONDATA);
	m_TipIcon.uFlags = NIF_INFO;
	m_TipIcon.uTimeout = uTimeout;
	m_TipIcon.dwInfoFlags = 1;
	strcpy(m_TipIcon.szInfo,pMsg);
	strcpy(m_TipIcon.szInfoTitle,pTitle);
	Shell_NotifyIcon(NIM_MODIFY,&m_TipIcon);//在托盘区添加图标
}


LRESULT CWinServiceDemoDlg::OnShowTask(WPARAM wParam,LPARAM lParam)
//wParam接收的是图标的ID，而lParam接收的是鼠标的行为
{
	if(wParam!=IDR_MAINFRAME)
		return 1;
	switch(lParam)
	{
	case WM_RBUTTONUP://右键起来时弹出快捷菜单
		{
			ShowIconToTip(FALSE);
		}
		break;
	case WM_LBUTTONDBLCLK://双击左键的处理
		{
			ShowIconToTip(FALSE);
		}
		break;
	}
	return 0;
}
void CWinServiceDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	
	if (nID == SC_MINIMIZE)
	{
		ShowIconToTip(TRUE);
		SendMessage(WM_SHOWTASK,0,0);
		ShowBalloonTip("最小化到托盘");
		
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}

	
}
