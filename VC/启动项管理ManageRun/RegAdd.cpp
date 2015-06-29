// RegAdd.cpp : implementation file
//

#include "stdafx.h"
#include "ManageRun.h"
#include "RegAdd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegAdd dialog


CRegAdd::CRegAdd(CWnd* pParent /*=NULL*/)
	: CDialog(CRegAdd::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRegAdd)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CRegAdd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegAdd)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRegAdd, CDialog)
	//{{AFX_MSG_MAP(CRegAdd)
	ON_BN_CLICKED(IDC_BTN_OK, OnBtnOk)
	ON_BN_CLICKED(IDC_BTN_CANCEL, OnBtnCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegAdd message handlers

void CRegAdd::OnBtnOk() 
{
	// TODO: Add your control notification handler code here
    ZeroMemory(szKeyName, MAXBYTE);
    ZeroMemory(szKeyValue, MAX_PATH);

	GetDlgItemText(IDC_EDIT_KEYNAME, szKeyName, MAXBYTE);
    GetDlgItemText(IDC_EDIT_KEYVALUE, szKeyValue, MAX_PATH);

    EndDialog(0);
}

void CRegAdd::OnBtnCancel() 
{
	// TODO: Add your control notification handler code here
	*szKeyName = '\0';
    *szKeyValue = '\0';

    EndDialog(0);
}