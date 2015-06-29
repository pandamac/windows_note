// KillMirwzntk.h : main header file for the KILLMIRWZNTK application
//

#if !defined(AFX_KILLMIRWZNTK_H__0DF0D79A_94C5_4B98_A72C_3E042404D50F__INCLUDED_)
#define AFX_KILLMIRWZNTK_H__0DF0D79A_94C5_4B98_A72C_3E042404D50F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CKillMirwzntkApp:
// See KillMirwzntk.cpp for the implementation of this class
//

class CKillMirwzntkApp : public CWinApp
{
public:
	CKillMirwzntkApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKillMirwzntkApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CKillMirwzntkApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KILLMIRWZNTK_H__0DF0D79A_94C5_4B98_A72C_3E042404D50F__INCLUDED_)
