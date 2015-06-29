// ChangeIconDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChangeIconDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChangeIconDlg dialog


CChangeIconDlg::CChangeIconDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChangeIconDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChangeIconDlg)
	m_strFileName = _T("");
	//}}AFX_DATA_INIT
}


void CChangeIconDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChangeIconDlg)
	DDX_Text(pDX, IDC_EDITBOX_FILENAME, m_strFileName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChangeIconDlg, CDialog)
	//{{AFX_MSG_MAP(CChangeIconDlg)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	ON_EN_UPDATE(IDC_EDITBOX_FILENAME, OnUpdateEditboxFilename)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangeIconDlg message handlers

BOOL CChangeIconDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	char* s=new char[40];
	GetWindowsDirectory(s,40);
	lpszPath=s;
	lpszPath+="\\System\\Shell32.dll";
	bAccord=FALSE;
	if(m_strFileName=="")
	{
		m_strFileName=lpszPath;
		m_nIndex=0;
	}
	UpdateData(FALSE);
	VERIFY(m_listboxIcon.SubclassDlgItem(IDC_LISTBOX_ICON, this));
	CRect rect;
	m_listboxIcon.GetWindowRect(rect);
	ScreenToClient(rect);
	rect.bottom+=GetSystemMetrics(SM_CXHSCROLL)-13;
	m_listboxIcon.MoveWindow(rect,TRUE);
	m_listboxIcon.SetColumnWidth(37);
	m_listboxIcon.SetItemHeight(0,45);
	HICON hIcon;
	int i=0;
	do
	{	
		hIcon=ExtractIcon(AfxGetApp()->m_hInstance,m_strFileName,i++);
		if(hIcon!=NULL)
		{
			if(m_nIndex<0)
				m_nIndex=0;
			m_listboxIcon.AddIconItem(hIcon,m_nIndex);
		}
	}while(hIcon!=NULL);
	m_listboxIcon.SetCurSel(m_nIndex);
	bAccord=TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChangeIconDlg::OnButtonBrowse() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE); 
	CString strDir=m_strFileName;
	strDir.MakeReverse();
	strDir.Delete(0,strDir.Find('\\')+1);
	strDir.MakeReverse();
	LPCTSTR szFilter="图标文件|*.dll;*.exe;*.ico|程序|*exe|库|*.dll|图标|*.ico|所有文件|*.*||";
	CFileDialog FileDlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,szFilter);
	FileDlg.m_ofn.lpstrTitle="更改图标";
	FileDlg.m_ofn.lpstrInitialDir=strDir;
	if(FileDlg.DoModal()==IDOK)
		m_strFileName=FileDlg.GetPathName();
	UpdateData(FALSE);
	ShowIcon();

}

void CChangeIconDlg::ShowIcon()
{
	int i=0;
	HICON hIcon;
	UpdateData(FALSE);
	m_listboxIcon.ResetContent();
	do
	{
		hIcon=ExtractIcon(AfxGetApp()->m_hInstance,m_strFileName,i++);
		if(hIcon==NULL&&i==1)
		{
			MessageBox("该文件中不包含图标!",NULL,MB_OK|MB_ICONEXCLAMATION);
			m_strFileName=lpszPath;
			i=0;
			hIcon=ExtractIcon(AfxGetApp()->m_hInstance,m_strFileName,i++);
			UpdateData(FALSE);
		}
		if(hIcon!=NULL)
		{
			m_listboxIcon.AddIconItem(hIcon,0);
		}
	}while(hIcon!=NULL);
	m_listboxIcon.SetCurSel(m_nIndex);
	bAccord=TRUE;
}

void CChangeIconDlg::OnOK() 
{
	// TODO: Add extra validation here
	m_nIndex=m_listboxIcon.nSelect;
	if(bAccord==FALSE)
	{
		UpdateData(TRUE);
		WIN32_FIND_DATA FindFileData;
		if(FindFirstFile(m_strFileName,&FindFileData)==INVALID_HANDLE_VALUE)
		{
			MessageBox("未找到该文件!");
			m_strFileName=lpszPath;
			m_nIndex=0;
			UpdateData(FALSE);
		}
		UpdateData(TRUE);
		ShowIcon();
		return;
	}
	CDialog::OnOK();
}

void CChangeIconDlg::OnUpdateEditboxFilename() 
{
	bAccord=FALSE;
}