// 查看输入表函数地址、Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "查看输入表函数地址、.h"
#include "查看输入表函数地址、Dlg.h"

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
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMyDlg, CDialog)
//{{AFX_MSG_MAP(CMyDlg)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(IDC_BUTTON1, OnOpenFile)
ON_BN_CLICKED(IDC_BUTTON2, OnSHOW)
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
CString FilePathName;
void CMyDlg::OnOpenFile() 
{
	// TODO: Add your control notification handler code here
	// TODO: 在此添加控件通知处理程序代码
    CFileDialog dlg(TRUE, //TRUE为OPEN对话框，FALSE为SAVE AS对话框
        NULL, 
        NULL,
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        (LPCTSTR)_TEXT("EXE Files (*.exe)|*.exe|All Files (*.*)|*.*||"),
        NULL);
    if(dlg.DoModal()==IDOK)
    {
        FilePathName=dlg.GetPathName(); //文件名保存在了FilePathName里
		GetDlgItem(IDC_EDIT1)->SetWindowText(FilePathName);
		UpdateData(FALSE);
    }
    else
    {
		return;
    }
}
CString szShow = "";
CString szTemp;
void CMyDlg::OnSHOW() 
{
	// TODO: Add your control notification handler code here

		HANDLE hFile=CreateFile(FilePathName,GENERIC_READ,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_ARCHIVE,NULL);
		
		if(INVALID_HANDLE_VALUE!=hFile)
		{
			
			HANDLE hMap=CreateFileMapping(hFile,NULL,PAGE_READONLY,0,0,NULL);
			if(hMap)
			{
				DWORD dwsize = GetFileSize(hFile,NULL);
				LPVOID lpBase=MapViewOfFile(hMap,FILE_MAP_READ,0,0,dwsize);
				if(lpBase)
				{
					IMAGE_DOS_HEADER *pDosHeader=(IMAGE_DOS_HEADER *)lpBase;
					IMAGE_OPTIONAL_HEADER *pOptHeader=(IMAGE_OPTIONAL_HEADER *)((BYTE *)lpBase+pDosHeader->e_lfanew+24);
					IMAGE_IMPORT_DESCRIPTOR *pImportDesc=(IMAGE_IMPORT_DESCRIPTOR *)((BYTE *)lpBase+pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
					szTemp = "";
					while(pImportDesc->FirstThunk)
					{
						char *pszDllName=(char *)((BYTE *)lpBase+pImportDesc->Name);
						szTemp.Format("\n模块名称:%s\n",pszDllName);
						szShow += szTemp;
						szShow += "\r\n";
						IMAGE_THUNK_DATA *pThunk=(IMAGE_THUNK_DATA *)((BYTE *)lpBase+pImportDesc->OriginalFirstThunk);
						int n=0;
						while(pThunk->u1.Function)
						{
							char *pszFunName=(char *)((BYTE *)lpBase+(DWORD)pThunk->u1.AddressOfData+2);
							PDWORD lpAddr=(DWORD *)((BYTE *)lpBase+pImportDesc->FirstThunk)+n;
							szTemp.Format("从此模块导入的函数:%-25s,函数地址：%x\n",pszFunName,lpAddr);
							szShow += szTemp;
							szShow += "\r\n";
							++n;
							++pThunk;
						}
						++pImportDesc;
					}
					UnmapViewOfFile(lpBase);
				}
				CloseHandle(hMap);			
			}
			CloseHandle(hFile);
		}	
	
	GetDlgItem(IDC_EDIT2)->SetWindowText(szShow);
	UpdateData(FALSE);
	return;
}
