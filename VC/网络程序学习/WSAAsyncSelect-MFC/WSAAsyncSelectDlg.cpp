// WSAAsyncSelectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WSAAsyncSelect.h"
#include "WSAAsyncSelectDlg.h"
#include <WinSock.h>
#pragma comment(lib,"WS2_32")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
#define WM_SOCKET WM_USER + 101

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
// CWSAAsyncSelectDlg dialog

CWSAAsyncSelectDlg::CWSAAsyncSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWSAAsyncSelectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWSAAsyncSelectDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWSAAsyncSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWSAAsyncSelectDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWSAAsyncSelectDlg, CDialog)
	//{{AFX_MSG_MAP(CWSAAsyncSelectDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_MESSAGE(WM_SOCKET,WindowProc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWSAAsyncSelectDlg message handlers

BOOL CWSAAsyncSelectDlg::OnInitDialog()
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

	SOCKET s = socket(AF_INET,SOCK_STREAM,IPPROTO_IP);
	
	sockaddr_in sin;
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(4567);
	
	bind(s,( sockaddr*)&sin,sizeof(sin));
	
	WSAAsyncSelect(s,this->m_hWnd,WM_SOCKET,FD_ACCEPT | FD_CLOSE);
	
	listen(s,5);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CWSAAsyncSelectDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CWSAAsyncSelectDlg::OnPaint() 
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
HCURSOR CWSAAsyncSelectDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CWSAAsyncSelectDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	
}

LRESULT CWSAAsyncSelectDlg::WindowProc( WPARAM wParam, LPARAM lParam)
{
	SOCKET s=wParam;  
    CString strContent;  
    if(WSAGETSELECTERROR(lParam))  
    {  
        closesocket(s);  
        return false;  
    }  
    switch(WSAGETSELECTEVENT(lParam))  
    {  
    case FD_ACCEPT://检测到有套接字连上来  
        {  
            SOCKET client=accept(s,NULL,NULL);  
            WSAAsyncSelect(client,m_hWnd,WM_SOCKET,FD_READ|FD_WRITE|FD_CLOSE);  
            CString strContent;  
            GetDlgItemText(IDC_EDIT1,strContent);  
            if(!strContent.IsEmpty())  
            {  
                strContent+="\r\n";  
            }             
            strContent+="有客户端连上来了";  
           SetDlgItemText(IDC_EDIT1,strContent);//将内容显示到Edit控件  
        }  
        break;  
    case FD_WRITE:  
        {  
        }  
        break;  
    case FD_READ:  
        {  
            char szText[1024]={0};  
            if(recv(s,szText,1024,0)==-1)  
                closesocket(s);  
            else  
            {                 
                GetDlgItemText(IDC_EDIT1,strContent);  
                if(!strContent.IsEmpty())  
                {  
                    strContent+="\r\n";  
                }  
                CString strTemp;  
                strTemp.Format("%s",szText);  
                strContent+=strTemp;  
               SetDlgItemText(IDC_EDIT1,strContent);//将内容显示到Edit控件中  
            }  
        }  
        break;  
    case FD_CLOSE:  
        {  
            closesocket(s);  
           GetDlgItemText(IDC_EDIT1,strContent);  
            if(!strContent.IsEmpty())  
            {  
                strContent+="\r\n";  
            }             
            strContent+="有客户端断开了";  
            SetDlgItemText(IDC_EDIT1,strContent);//将内容显示到Edit控件中  
        }  
        break;  
    }  
    return true;  
}