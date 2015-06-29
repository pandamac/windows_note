// ManageProcess.h : main header file for the MANAGEPROCESS application
//

#if !defined(AFX_MANAGEPROCESS_H__0719BCCC_D940_424E_966D_358B2C616059__INCLUDED_)
#define AFX_MANAGEPROCESS_H__0719BCCC_D940_424E_966D_358B2C616059__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CManageProcessApp:
// See ManageProcess.cpp for the implementation of this class
//

class CManageProcessApp : public CWinApp
{
public:
	CManageProcessApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CManageProcessApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CManageProcessApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MANAGEPROCESS_H__0719BCCC_D940_424E_966D_358B2C616059__INCLUDED_)
