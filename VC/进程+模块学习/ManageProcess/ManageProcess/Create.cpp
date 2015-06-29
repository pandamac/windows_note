// Create.cpp : implementation file
//

#include "stdafx.h"
#include "ManageProcess.h"
#include "Create.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCreate dialog


CCreate::CCreate(CWnd* pParent /*=NULL*/)
	: CDialog(CCreate::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCreate)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CCreate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreate)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCreate, CDialog)
	//{{AFX_MSG_MAP(CCreate)
	ON_BN_CLICKED(IDC_BTN_CANCEL, OnBtnCancel)
	ON_BN_CLICKED(IDC_BTN_OK, OnBtnOk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreate message handlers

void CCreate::OnBtnCancel() 
{
	// TODO: Add your control notification handler code here
	*szFilePath = '\0';

    EndDialog(0);
}

void CCreate::OnBtnOk() 
{
	// TODO: Add your control notification handler code here
	ZeroMemory(szFilePath, MAX_PATH);

    GetDlgItemText(IDC_EDIT_FILEPATH, szFilePath, MAX_PATH);

    EndDialog(0);
}
