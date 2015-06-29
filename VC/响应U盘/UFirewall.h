// UFirewall.h : main header file for the UFIREWALL application
//

#if !defined(AFX_UFIREWALL_H__6DDEDA6D_221C_43DD_8E97_FFBE17E98048__INCLUDED_)
#define AFX_UFIREWALL_H__6DDEDA6D_221C_43DD_8E97_FFBE17E98048__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CUFirewallApp:
// See UFirewall.cpp for the implementation of this class
//

class CUFirewallApp : public CWinApp
{
public:
	CUFirewallApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUFirewallApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CUFirewallApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UFIREWALL_H__6DDEDA6D_221C_43DD_8E97_FFBE17E98048__INCLUDED_)
