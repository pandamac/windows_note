// ManageRun.h : main header file for the MANAGERUN application
//

#if !defined(AFX_MANAGERUN_H__BC05F32D_B5D2_41AC_8813_7215C5BBD220__INCLUDED_)
#define AFX_MANAGERUN_H__BC05F32D_B5D2_41AC_8813_7215C5BBD220__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CManageRunApp:
// See ManageRun.cpp for the implementation of this class
//

class CManageRunApp : public CWinApp
{
public:
	CManageRunApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CManageRunApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CManageRunApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MANAGERUN_H__BC05F32D_B5D2_41AC_8813_7215C5BBD220__INCLUDED_)
