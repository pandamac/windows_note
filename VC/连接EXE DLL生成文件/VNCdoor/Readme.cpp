// Readme.cpp : implementation file
//

#include "stdafx.h"
#include "VNCdoor.h"
#include "Readme.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReadme dialog


CReadme::CReadme(CWnd* pParent /*=NULL*/)
	: CDialog(CReadme::IDD, pParent)
{
	//{{AFX_DATA_INIT(CReadme)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CReadme::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReadme)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CReadme, CDialog)
	//{{AFX_MSG_MAP(CReadme)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReadme message handlers
