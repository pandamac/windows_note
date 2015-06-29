// VALUECREATE.cpp : implementation file
//

#include "stdafx.h"
#include "注册表管理.h"
#include "VALUECREATE.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// VALUECREATE dialog


VALUECREATE::VALUECREATE(CWnd* pParent /*=NULL*/)
	: CDialog(VALUECREATE::IDD, pParent)
{
	//{{AFX_DATA_INIT(VALUECREATE)
	m_date = _T("");
	m_name = _T("");
	m_type = _T("");
	//}}AFX_DATA_INIT
}


void VALUECREATE::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VALUECREATE)
	DDX_Text(pDX, IDC_EDIT_DATE, m_date);
	DDX_Text(pDX, IDC_EDIT_NAME, m_name);
	DDX_Text(pDX, IDC_EDIT_TYPE, m_type);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VALUECREATE, CDialog)
	//{{AFX_MSG_MAP(VALUECREATE)
	ON_BN_CLICKED(IDOK, OnCreate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VALUECREATE message handlers
void VALUECREATE::SetDate(HKEY mkey, CString subkey)
{
	MKEY	=mkey;
	SubKey	=subkey;
}

void VALUECREATE::OnCreate() 
{
	HKEY	key;

	UpdateData(true);

	if(m_type=="REG_SZ")
	{
		if(RegOpenKeyEx(MKEY,SubKey.LockBuffer(),0,KEY_ALL_ACCESS,&key)==ERROR_SUCCESS)	
		{
			if(::RegSetValueEx(key,m_name.LockBuffer(),0,REG_SZ,(const unsigned char *)m_date.LockBuffer(),MAX_PATH)==ERROR_SUCCESS)
			{
				MessageBox("创建成功!");
			}else
			{
				MessageBox("创建失败!");
			}
		}else
		{
			AfxMessageBox("打开注册表失败！");
		}
	}


	if(m_type=="REG_DWORD")
	{
		if(RegOpenKeyEx(MKEY,SubKey.LockBuffer(),0,KEY_ALL_ACCESS,&key)==ERROR_SUCCESS)	
		{
			if(::RegSetValueEx(key,m_name.LockBuffer(),0,REG_DWORD,(const unsigned char *)m_date.LockBuffer(),4)==ERROR_SUCCESS)//注意数据长度应该设为4
			{
				MessageBox("创建成功!");
			}else
			{
				MessageBox("创建失败!");
			}
		}else
		{
			AfxMessageBox("打开注册表失败！");
		}

	}
	if(m_type=="REG_BINARY")
	{
		if(RegOpenKeyEx(MKEY,SubKey.LockBuffer(),0,KEY_ALL_ACCESS,&key)==ERROR_SUCCESS)	
		{
			if(::RegSetValueEx(key,m_name.LockBuffer(),0,REG_BINARY,(const unsigned char *)m_date.LockBuffer(),4)==ERROR_SUCCESS)//注意数据长度应该设为4
			{
				MessageBox("创建成功!");
			}else
			{
				MessageBox("创建失败!");
			}
		}else
		{
			AfxMessageBox("打开注册表失败！");
		}
	}
	if(m_type=="REG_EXPAND_SZ")
	{
		if(RegOpenKeyEx(MKEY,SubKey.LockBuffer(),0,KEY_ALL_ACCESS,&key)==ERROR_SUCCESS)	
		{
			if(::RegSetValueEx(key,m_name.LockBuffer(),0,REG_SZ,(const unsigned char *)m_date.LockBuffer(),MAX_PATH)==ERROR_SUCCESS)
			{
				MessageBox("创建成功!");
			}else
			{
				MessageBox("创建失败!");
			}
		}else
		{
			AfxMessageBox("打开注册表失败！");
		}
	}	


}


