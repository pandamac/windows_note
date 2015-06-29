// PeParse.h : main header file for the PEPARSE application
//

#if !defined(AFX_PEPARSE_H__D9D09E3E_7AF2_4D52_ADD2_079FA2743494__INCLUDED_)
#define AFX_PEPARSE_H__D9D09E3E_7AF2_4D52_ADD2_079FA2743494__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPeParseApp:
// See PeParse.cpp for the implementation of this class
//

class CPeParseApp : public CWinApp
{
public:
	CPeParseApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPeParseApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPeParseApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PEPARSE_H__D9D09E3E_7AF2_4D52_ADD2_079FA2743494__INCLUDED_)
