// DebugHookDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DebugHook.h"
#include "DebugHookDlg.h"
#include "stdio.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
///////////////////////////////////////////////////////////////////////
HHOOK	hDebug = NULL ;	// 钩子句柄

// 键盘钩子消息处理过程
LRESULT CALLBACK DebugProc ( int nCode, WPARAM wParam, LPARAM lParam )
{
	if ( nCode == HC_ACTION )
	{
		PDEBUGHOOKINFO pDebugHookInfo = (PDEBUGHOOKINFO)lParam ;
		switch ( wParam )
		{
		case WH_KEYBOARD:
		case WH_MOUSE:
			{
				// 如果钩子不是由当前DEBUG钩子所在线程安装
				// 就直接返回非0值，取消钩子函数过程调用
				if ( pDebugHookInfo->idThread != pDebugHookInfo->idThreadInstaller )
					return 1 ;
			}
			break ;
		}
	}
	// 继续传递消息
	return CallNextHookEx ( hDebug, nCode, wParam, lParam ) ;
}

BOOL WINAPI SetHook ( BOOL isInstall ) 
{
	// 需要安装，且钩子不存在
	if ( isInstall && !hDebug )
	{
		// 设置全局钩子
		hDebug = SetWindowsHookEx ( WH_DEBUG, (HOOKPROC)DebugProc, 0, GetCurrentThreadId() ) ;
		if ( hDebug == NULL )
			return FALSE ;
	}

	// 需要卸载，且钩子存在
	if ( !isInstall && hDebug )
	{
		// 卸载钩子
		BOOL ret = UnhookWindowsHookEx ( hDebug ) ;
		hDebug	= NULL ;
		return ret ;
	}

	return TRUE ;
}
//////////////////////////////////////////////////////////////////////////////////

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


// CDebugHookDlg 对话框




CDebugHookDlg::CDebugHookDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDebugHookDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDebugHookDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDebugHookDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_START, &CDebugHookDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_STOP, &CDebugHookDlg::OnBnClickedStop)
END_MESSAGE_MAP()


// CDebugHookDlg 消息处理程序

BOOL CDebugHookDlg::OnInitDialog()
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

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDebugHookDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDebugHookDlg::OnPaint()
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
HCURSOR CDebugHookDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDebugHookDlg::OnBnClickedStart()
{
	SetHook ( TRUE ) ;
}

void CDebugHookDlg::OnBnClickedStop()
{
	SetHook ( FALSE ) ;
}
