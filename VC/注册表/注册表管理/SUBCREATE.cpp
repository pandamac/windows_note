// SUBCREATE.cpp : implementation file
//

#include "stdafx.h"
#include "注册表管理.h"
#include "SUBCREATE.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SUBCREATE dialog


SUBCREATE::SUBCREATE(CWnd* pParent /*=NULL*/)
	: CDialog(SUBCREATE::IDD, pParent)
{
	//{{AFX_DATA_INIT(SUBCREATE)
	m_sub_value = _T("");
	//}}AFX_DATA_INIT
}


void SUBCREATE::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SUBCREATE)
	DDX_Text(pDX, IDC_EDIT1, m_sub_value);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SUBCREATE, CDialog)
	//{{AFX_MSG_MAP(SUBCREATE)
	ON_BN_CLICKED(IDOK, OnCreate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SUBCREATE message handlers

void SUBCREATE::SetDate(HKEY mkey, CString subkey)
{
	MKEY	=mkey;
	SubKey	=subkey;
}

void SUBCREATE::OnCreate() 
{
	HKEY key;

	UpdateData(true);
	
	SubKey+=m_sub_value;
	
	if (RegCreateKey(MKEY,SubKey.LockBuffer(),&key)!=ERROR_SUCCESS)
	{
		MessageBox("创建注册表失败!");
	}else
	{
		MessageBox("创建注册表成功!");
	}
	
}
