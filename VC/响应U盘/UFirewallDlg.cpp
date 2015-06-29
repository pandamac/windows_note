// UFirewallDlg.cpp : implementation file
//

#include "stdafx.h"
#include "UFirewall.h"
#include "UFirewallDlg.h"

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
// CUFirewallDlg dialog

CUFirewallDlg::CUFirewallDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUFirewallDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUFirewallDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUFirewallDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUFirewallDlg)
	DDX_Control(pDX, IDC_BTN_SAFEOPEN, m_SafeOpen);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CUFirewallDlg, CDialog)
	//{{AFX_MSG_MAP(CUFirewallDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_SAFEOPEN, OnBtnSafeopen)
    ON_MESSAGE(WM_DEVICECHANGE, OnDeviceChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUFirewallDlg message handlers

BOOL CUFirewallDlg::OnInitDialog()
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
    //m_SafeOpen.EnableWindow(FALSE);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CUFirewallDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CUFirewallDlg::OnPaint() 
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
HCURSOR CUFirewallDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
#include <Dbt.h>

BOOL CUFirewallDlg::OnDeviceChange(UINT nEventType, DWORD dwData)
{
    if ( nEventType == DBT_DEVICEARRIVAL )
    {
        GetDriverName(dwData);
        MessageBox(DriverName);

        if ( DriverName != "" )
        {
            m_SafeOpen.EnableWindow(TRUE);

            CString File = DriverName;
            File += "\\autorun.inf";

            char szBuff[MAX_PATH] = { 0 };

            if ( GetFileAttributes(File.GetBuffer(0)) == -1 )
            {
                m_SafeOpen.EnableWindow(FALSE);
                return FALSE;
            }

            // 获取open后面的内容
            GetPrivateProfileString(
                "AutoRun",        // section name
                "open",        // key name
                NULL,        // default string
                szBuff,  // destination buffer
                MAX_PATH,              // size of destination buffer
                File.GetBuffer(0)        // initialization file name
            );

            CString str;
            str = "是否删除：";
            str += szBuff;
            if ( MessageBox(str, NULL, MB_YESNO) == IDYES )
            {
                // 删除要执行的文件
                DeleteFile(str.GetBuffer(0));
            }
        }
    }
    else if ( nEventType == DBT_DEVICEREMOVECOMPLETE )
    {
        m_SafeOpen.EnableWindow(FALSE);
    }
    
    return TRUE;
}

VOID CUFirewallDlg::GetDriverName(DWORD dwData)
{
    PDEV_BROADCAST_HDR pDevHdr = (PDEV_BROADCAST_HDR)dwData;
    if ( pDevHdr->dbch_devicetype == DBT_DEVTYP_VOLUME )
    {
        PDEV_BROADCAST_VOLUME pDevVolume = (PDEV_BROADCAST_VOLUME)pDevHdr;
        
        // pDevVolume->dbcv_flags为0表示为U盘
        if ( pDevVolume->dbcv_flags == 0 )
        {            
            char i;
            
            // 通过将pDevVolume->dbcv_unitmask移位来判断盘符
            DWORD dwUnitmask = pDevVolume->dbcv_unitmask;
            for (i = 0; i < 26; ++i)
            {
                if ( dwUnitmask & 0x1)
                {
                    break;
                }
                
                dwUnitmask = dwUnitmask >> 1;
            }
            
            if ( i >= 26 )
            {
                return ;
            }
            
            DriverName.Format("%c:", i + 'A');
        }
    }
}

void CUFirewallDlg::OnBtnSafeopen() 
{
	// TODO: Add your control notification handler code here
    ShellExecute(NULL, "open", DriverName.GetBuffer(0), NULL, NULL, SW_SHOW);
}
