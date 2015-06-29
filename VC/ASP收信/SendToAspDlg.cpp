// SendToAspDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SendToAsp.h"
#include "SendToAspDlg.h"
//在这里加一个头文件
#include <afxinet.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSendToAspDlg dialog

CSendToAspDlg::CSendToAspDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSendToAspDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSendToAspDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSendToAspDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendToAspDlg)
	DDX_Control(pDX, IDC_EDIT3, m_Edit3);
	DDX_Control(pDX, IDC_EDIT2, m_Edit2);
	DDX_Control(pDX, IDC_EDIT1, m_Edit1);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSendToAspDlg, CDialog)
	//{{AFX_MSG_MAP(CSendToAspDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendToAspDlg message handlers

BOOL CSendToAspDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CSendToAspDlg::OnPaint() 
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

HCURSOR CSendToAspDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CSendToAspDlg::OnOK() 
{
	// 这里是自己写的代码部分，其他的全为MFC代码
	CString Url="";
	CString content="";
	CInternetSession session; 
	INTERNET_PORT nPort = 80;
	CString serverName ="";

	m_Edit1.GetWindowText(Url);
	m_Edit2.GetWindowText(content);

	if(Url==""||content=="")
	{
		MessageBox("URL和内容不能为空，谢谢！","注意",MB_OK);
		return;
	}

	//包装URL
	Url=Url+"?content="+content;

	serverName=_T(Url);
	//当按下发送键，设置URL和内容为不可编辑，显示状态信息:正在发送
	m_Edit1.SetReadOnly(TRUE);
	m_Edit2.SetReadOnly(TRUE);
	m_Edit3.SetWindowText("正在发送信息，请稍后！");
	try{
		//与URL进行连接
		CHttpConnection* pConnection = session.GetHttpConnection(serverName,nPort); 
		//请求页面
		session.OpenURL(serverName,1,INTERNET_FLAG_TRANSFER_ASCII,NULL,0);
		//关闭
		session.Close();
	}
	catch (CInternetException * e)
	{
		MessageBox("请检查你的网络连接是否正常!连接不上服务器.","注意",MB_OK);
		return;
	}
	//当发送完成后，设置URL和内容为可编辑，清空状态信息
	m_Edit1.SetReadOnly(FALSE);
	m_Edit2.SetReadOnly(FALSE);
	m_Edit3.SetWindowText("");
	MessageBox("信息已经发送成功","恭喜",MB_OK);
	m_Edit2.SetWindowText("");
}
