// Tips.cpp : implementation file
//

#include "stdafx.h"
#include "Hips.h"
#include "Tips.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTips dialog


CTips::CTips(CWnd* pParent /*=NULL*/)
	: CDialog(CTips::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTips)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTips::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTips)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTips, CDialog)
	//{{AFX_MSG_MAP(CTips)
	ON_BN_CLICKED(IDC_BTN_OK, OnBtnOk)
	ON_BN_CLICKED(IDC_BTN_CANCEL, OnBtnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTips message handlers

void CTips::OnBtnOk() 
{
	// TODO: Add your control notification handler code here
	bRet = TRUE;
    EndDialog(0);
}

void CTips::OnBtnCancel() 
{
	// TODO: Add your control notification handler code here
    bRet = FALSE;
    EndDialog(0);
}

BOOL CTips::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
    RECT rt;
    
    int nScreenWidth = GetSystemMetrics(SM_CXFULLSCREEN);
    int nScreenHeight = GetSystemMetrics(SM_CYFULLSCREEN);
    
    GetWindowRect(&rt);

    int nWidth = rt.right - rt.left;
    int nHeight = rt.bottom - rt.top;

    //int nX = (nScreenWidth + (rt.right - rt.left)) / 2;
    //int nY = (nScreenHeight + (rt.bottom - rt.top)) / 2;
    
    rt.left = (nScreenWidth - nWidth)/* / 2*/;
    rt.top = (nScreenHeight - nHeight)/* / 2*/;
    rt.right = rt.left + nWidth;
    rt.bottom = rt.top + nHeight;
    
    MoveWindow(&rt, FALSE);

    SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    SetDlgItemTextW(m_hWnd, IDC_EDIT_TEXT, sz);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
