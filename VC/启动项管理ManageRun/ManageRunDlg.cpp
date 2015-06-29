// ManageRunDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ManageRun.h"
#include "ManageRunDlg.h"
#include "RegAdd.h"

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
// CManageRunDlg dialog

CManageRunDlg::CManageRunDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CManageRunDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CManageRunDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CManageRunDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CManageRunDlg)
	DDX_Control(pDX, IDC_LIST_RUN, m_RunList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CManageRunDlg, CDialog)
	//{{AFX_MSG_MAP(CManageRunDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_ADD, OnBtnAdd)
	ON_BN_CLICKED(IDC_BTN_DEL, OnBtnDel)
	ON_BN_CLICKED(IDC_BTN_QUIT, OnBtnQuit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CManageRunDlg message handlers

BOOL CManageRunDlg::OnInitDialog()
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
    InitRunList();
    ShowRunList();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CManageRunDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CManageRunDlg::OnPaint() 
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
HCURSOR CManageRunDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

VOID CManageRunDlg::InitRunList()
{
    m_RunList.SetExtendedStyle(
            m_RunList.GetExtendedStyle()
            | LVS_EX_GRIDLINES // 有网络格
            | LVS_EX_FULLROWSELECT);    // 选择单行

    m_RunList.InsertColumn(0, _T("NO."));
    m_RunList.InsertColumn(1, _T("键    名"));
    m_RunList.InsertColumn(2, _T("键    值"));

    m_RunList.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
    m_RunList.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
    m_RunList.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);
}

#define REG_RUN "Software\\Microsoft\\Windows\\CurrentVersion\\Run\\"

void CManageRunDlg::OnBtnAdd() 
{
	// TODO: Add your control notification handler code here
	CRegAdd RegAdd;
    RegAdd.DoModal();
    if ( strlen(RegAdd.szKeyName) > 0 && strlen(RegAdd.szKeyValue) > 0 )
    {
        HKEY hKey = NULL;
        LONG lRet = RegOpenKey(HKEY_LOCAL_MACHINE, REG_RUN, &hKey);

        if ( lRet != ERROR_SUCCESS )
        {
            return ;
        }

        RegSetValueEx(hKey, RegAdd.szKeyName, NULL, REG_SZ, (const unsigned char *)RegAdd.szKeyValue, strlen(RegAdd.szKeyValue) + sizeof(char));

        RegCloseKey(hKey);

	    ShowRunList();
    }
    else
    {
        AfxMessageBox("请输入完整的内容");
    }
}

void CManageRunDlg::OnBtnDel() 
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_RunList.GetFirstSelectedItemPosition();
    int nSelected = -1;

    while ( pos )
    {
        nSelected = m_RunList.GetNextSelectedItem(pos);
    }

    if ( -1 == nSelected )
    {
        AfxMessageBox("请选择要删除的启动项");
        return ;
    }

    char szKeyName[MAXBYTE] = { 0 };
    m_RunList.GetItemText(nSelected, 1, szKeyName, MAXBYTE);

    AfxMessageBox(szKeyName);

    HKEY hKey = NULL;

    LONG lRet = RegOpenKey(HKEY_LOCAL_MACHINE, REG_RUN, &hKey);

    RegDeleteValue(hKey, szKeyName);

    RegCloseKey(hKey);
    
    ShowRunList();
}

void CManageRunDlg::OnBtnQuit() 
{
	// TODO: Add your control notification handler code here
	EndDialog(0);
}

VOID CManageRunDlg::ShowRunList()
{
    m_RunList.DeleteAllItems();
	
    DWORD dwType = 0;
    DWORD dwBufferSize = MAXBYTE;
    DWORD dwKeySize = MAXBYTE;
    char szValueName[MAXBYTE] = { 0 };
    char szValueKey[MAXBYTE] = { 0 };
    
    HKEY hKey = NULL;
    LONG lRet = RegOpenKey(HKEY_LOCAL_MACHINE, REG_RUN, &hKey);

    if ( lRet != ERROR_SUCCESS )
    {
        return ;
    }

    int i = 0;
    CString strTmp;
    while ( TRUE )
    {
        lRet = RegEnumValue(hKey, i, szValueName, &dwBufferSize, NULL, &dwType, (unsigned char *)szValueKey, &dwKeySize);
        if ( lRet == ERROR_NO_MORE_ITEMS )
        {
            break;
        }
        strTmp.Format("%d", i);
        m_RunList.InsertItem(i, strTmp);
        m_RunList.SetItemText(i, 1, szValueName);
        m_RunList.SetItemText(i, 2, szValueKey);
        i ++;
        ZeroMemory(szValueName,MAXBYTE);
        ZeroMemory(szValueKey, MAXBYTE);
    }

    RegCloseKey(hKey);
}
