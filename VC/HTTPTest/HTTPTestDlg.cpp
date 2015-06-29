// HTTPTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HTTPTest.h"
#include "HTTPTestDlg.h"
#include <afxinet.h>
#include "winsock.h"

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
// CHTTPTestDlg dialog

CHTTPTestDlg::CHTTPTestDlg(CWnd* pParent /*=NULL*/)
: CDialog(CHTTPTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHTTPTestDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHTTPTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHTTPTestDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CHTTPTestDlg, CDialog)
//{{AFX_MSG_MAP(CHTTPTestDlg)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnButtonNew)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHTTPTestDlg message handlers

BOOL CHTTPTestDlg::OnInitDialog()
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
	ShowYourAddr();


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHTTPTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CHTTPTestDlg::OnPaint() 
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
HCURSOR CHTTPTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CHTTPTestDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here

	ShowYourAddr();

	CInternetSession sess; //建立会话
	m_IP = "118.114.242.227";

	GetDlgItemText(IDC_EDIT_IP,m_IP);
	CString URLPath = \
"http://www.baidu.com/s?ie=utf-8&bs=%E6%88%91%E7%9A%84IP&f=8&rsv_bp=1&wd="+m_IP+"&rsv_n=2&rsv_sug3=1&rsv_sug1=1&rsv_sug4=94&inputT=1025";

	
	CHttpFile* pF= (CHttpFile*)sess.OpenURL(URLPath); //打开网址

	CString strData,strAllData;

	while (pF->ReadString(strData))//由于网页使用的是UTF-8编码，保存到本地时会形成乱码，非unicode，必须进行转码
    {
        char * pStr = strData.GetBuffer(strData.GetLength());//获取原始字符串
        int szBuf = MultiByteToWideChar(CP_UTF8,0,pStr,-1,NULL,0);//获取需要多少缓存
        wchar_t * pBuf = new wchar_t[szBuf * sizeof(wchar_t)];//申请缓存空间
        MultiByteToWideChar(CP_UTF8,0,pStr,-1,pBuf,szBuf * sizeof(wchar_t));//转码
        strAllData += pBuf;
        delete [] pBuf;
    }

	pF->Close();
	sess.Close();


	CString strArticleStart = "属于";
	CString strArticleEnd = "。查ip归属地";

	int iStart = strAllData.Find(strArticleStart);
	strAllData = strAllData.Mid(iStart + strArticleStart.GetLength(),\
		strAllData.GetLength());

	int iEnd = strAllData.Find(strArticleEnd);
	strAllData= strAllData.Mid(0,iEnd);

	strAllData.Replace(_T("<p>"),_T("  "));
	strAllData.Replace(_T("</p>"), _T("\r\n"));


	//MessageBox(strAllData);
	if (strAllData.GetLength() == 0)
	{
		SetDlgItemText(IDC_EDIT_ADDR,"查询失败!请检查输入IP是否合法!");
		return;
	}
	
	SetDlgItemText(IDC_EDIT_ADDR,strAllData);	
}



void CHTTPTestDlg::OnButtonNew() 
{
	// TODO: Add your control notification handler code here
	::SetDlgItemText(this->m_hWnd,IDC_EDIT_ADDR,"");
	::SetDlgItemText(this->m_hWnd,IDC_EDIT_IP,"");
	::SetDlgItemText(this->m_hWnd,IDC_EDIT_MYIP,"");
	
}

void CHTTPTestDlg::ShowYourAddr()
{
	WORD wVersionRequested;
    WSADATA wsaData;
    char name[255];
    CString ip;
    PHOSTENT hostinfo;
    wVersionRequested = MAKEWORD( 2, 0 );
	
    if ( WSAStartup( wVersionRequested, &wsaData ) == 0 )
    {
		
		if( gethostname ( name, sizeof(name)) == 0)
        {
			if((hostinfo = gethostbyname(name)) != NULL)
            {
				ip = inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list);
            }
        }
		
        WSACleanup( );
	}
   ::SetDlgItemText(this->m_hWnd,IDC_EDIT_MYIP,ip);
}
