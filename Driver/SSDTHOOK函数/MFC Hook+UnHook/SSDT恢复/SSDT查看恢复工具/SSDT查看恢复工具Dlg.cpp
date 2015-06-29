// SSDT查看恢复工具Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "SSDT查看恢复工具.h"
#include "SSDT查看恢复工具Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <windows.h>
#include <shlwapi.h>
#include <tchar.h>
#include "..\\EXE\\SSDT.h"


extern ULONG NTBase;		//NT 基址
extern ULONG TotalSSDTCount;		//SSDT服务个数
extern ULONG RealCount;		//枚举所找到的服务个数
extern pSSDTSaveTable pSSDTST;	//保存的SSDT缓冲表

HANDLE hDriver = NULL;		//驱动句柄
BOOL bLoadSysSuccess = TRUE;

/////////////////////////////////////////////////////////////////////////////
// CSSDTDlg dialog

CSSDTDlg::CSSDTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSSDTDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSSDTDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSSDTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSSDTDlg)
	DDX_Control(pDX, IDC_LIST1, m_List);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSSDTDlg, CDialog)
	//{{AFX_MSG_MAP(CSSDTDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnReShowSSDT)
	ON_BN_CLICKED(IDC_BUTTON2, OnReSSDT)
	ON_BN_CLICKED(IDC_BUTTON3, OnReSSDTAndThrowSpilth)
	ON_NOTIFY( NM_CUSTOMDRAW, IDC_LIST1, OnDrawColorForMyList )		//为改变颜色添加的消息
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSSDTDlg message handlers

//加载驱动
BOOL LoadMyDriver()
{
	//得到驱动程序绝对路径
	TCHAR szDriver[MAX_PATH+1];
	::GetModuleFileName( NULL, szDriver, MAX_PATH );
	lstrcpy( _tcsrchr( szDriver, _T('\\') ) + 1, _T("SSDT.sys") );
	if( !PathFileExists( szDriver ) )
	{
		return FALSE;
	}
	hDriver = LoadDriver( szDriver );
	if( INVALID_HANDLE_VALUE == hDriver )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CSSDTDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	if( !LoadMyDriver() )
	{
		bLoadSysSuccess = FALSE;
		MessageBox( "驱动程序加载失败,请确保其在同一目录下!", "错误", MB_OK );
	}
	else
	{
		ShowSSDT();		//显示SSDT
	}
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSSDTDlg::OnPaint() 
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
	if( !bLoadSysSuccess )		//驱动加载失败
	{
		OnOK();		//正常退出程序
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSSDTDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL bIsFirst = TRUE;

void CSSDTDlg::ShowSSDT()
{
	ULONG i;

	//枚举SSDT
	if( !EnumSSDT( hDriver ) )
	{
		//卸载驱动
		if( hDriver )
		{
			UnloadDriver( hDriver );
			hDriver = NULL;
		}
		MessageBox( "枚举SSDT失败!", "错误", MB_OK );
		OnOK();		//正常退出程序
	}
	if( TotalSSDTCount == -1 )
	{
		return ;
	}

	if( bIsFirst )
	{
		//添加列
		m_List.InsertColumn( 0, "服务号", LVCFMT_CENTER, 50, -1 );
		m_List.InsertColumn( 1, "当前地址", LVCFMT_CENTER, 100, -1 );
		m_List.InsertColumn( 2, "原始地址", LVCFMT_CENTER, 100, -1 );
		m_List.InsertColumn( 3, "服务函数名", LVCFMT_LEFT, 250, -1 );
		m_List.InsertColumn( 4, "模块名", LVCFMT_LEFT, 300, -1 );
		
		bIsFirst = FALSE;
	}
	//使其可选择一行
	m_List.SetExtendedStyle(m_List.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	
	//添加内容
	char szIndex[10];
	char szCurAddr[18];
	char szOrgAddr[18];

	for( i = 0; i < TotalSSDTCount; i ++ )
	{
		sprintf( szIndex, "0x%04X", \
			((pSSDTSaveTable)((ULONG)pSSDTST + i * sizeof(SSDTSaveTable)))->ulServiceNumber & 0xFFFF \
			);
		sprintf( szCurAddr, "0x%08X", \
			((pSSDTSaveTable)((ULONG)pSSDTST + i * sizeof(SSDTSaveTable)))->ulCurrentFunctionAddress \
			);
		sprintf( szOrgAddr, "0x%08X", \
			((pSSDTSaveTable)((ULONG)pSSDTST + i * sizeof(SSDTSaveTable)))->ulOriginalFunctionAddress \
			);
		m_List.InsertItem( m_List.GetItemCount(), szIndex );
		m_List.SetItemText( m_List.GetItemCount()-1, 1, szCurAddr );
		m_List.SetItemText( m_List.GetItemCount()-1, 2, szOrgAddr );
		m_List.SetItemText( m_List.GetItemCount()-1, 3, \
			((pSSDTSaveTable)((ULONG)pSSDTST + i * sizeof(SSDTSaveTable)))->ServiceFunctionName \
			);
		m_List.SetItemText( m_List.GetItemCount()-1, 4, \
			((pSSDTSaveTable)((ULONG)pSSDTST + i * sizeof(SSDTSaveTable)))->ModuleName \
			);
	}
}

void CSSDTDlg::OnReShowSSDT() 
{
	// TODO: Add your control notification handler code here
	m_List.DeleteAllItems();
	ShowSSDT();
}

BOOL CSSDTDlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	//卸载驱动
	if( hDriver )
	{
		UnloadDriver( hDriver );
		hDriver = NULL;
	}
	
	return CDialog::DestroyWindow();
}

void CSSDTDlg::OnReSSDT() 
{
	// TODO: Add your control notification handler code here
	//确认
	if( IDYES == MessageBox( "该操作有一定的危险性,是否继续?", "恢复SSDT提示", MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2 ) )
	{
		//恢复SSDT
		if( ReSSDT( hDriver ) )
		{
			MessageBox( "恢复SSDT成功!", "成功", MB_OK );
		}
		else
		{
			MessageBox( "恢复SSDT失败!", "失败", MB_OK );
		}
		//重绘
		OnReShowSSDT();
	}
}

void CSSDTDlg::OnReSSDTAndThrowSpilth() 
{
	// TODO: Add your control notification handler code here
	//确认
	if( IDYES == MessageBox( "该操作有一定的危险性,是否继续?", "恢复SSDT并清理提示", MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2 ) )
	{
		//恢复SSDT,并删掉非系统SSDT函数
		if( ReSSDTAndThrowSpilth( hDriver ) )
		{
			MessageBox( "恢复SSDT并删掉非系统SSDT函数成功!", "成功", MB_OK );
		}
		else
		{
			MessageBox( "恢复SSDT并删掉非系统SSDT函数失败!", "失败", MB_OK );
		}
		//重绘
		OnReShowSSDT();
	}
}
//改变 m_List 控件单行的颜色
void CSSDTDlg::OnDrawColorForMyList( NMHDR *pNmHdr, LRESULT *pResult )
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNmHdr );

    *pResult = CDRF_DODEFAULT;

	if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
	{
        *pResult = CDRF_NOTIFYITEMDRAW;
	}
    else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
	{
        *pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
    else if ( (CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage )
	{
		COLORREF clrNewTextColor, clrNewBkColor;
        
		int    nItem = static_cast<int>( pLVCD->nmcd.dwItemSpec );

		//如果当前SSDT地址与原始地址不一样,则显示字体为红色,否则为黑色
		if( \
			((pSSDTSaveTable)((ULONG)pSSDTST + nItem * sizeof(SSDTSaveTable)))->ulCurrentFunctionAddress \
			!= ((pSSDTSaveTable)((ULONG)pSSDTST + nItem * sizeof(SSDTSaveTable)))->ulOriginalFunctionAddress \
			)		//设置为红色
		{
			clrNewTextColor = RGB( 255, 0, 0 );
		}
		else		//设置为黑色
		{
			clrNewTextColor = RGB( 0, 0, 0 );
		}

		//设置背景色
		if( nItem%2 ==0 )
		{
			clrNewBkColor = RGB( 240, 240, 240 );	//偶数行背景色为灰色
		}
		else
		{
			clrNewBkColor = RGB( 255, 255, 255 );	//奇数行背景色为白色
		}

		pLVCD->clrText = clrNewTextColor;
		pLVCD->clrTextBk = clrNewBkColor;

        *pResult = CDRF_DODEFAULT;
	}
}
