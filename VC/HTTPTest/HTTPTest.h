// HTTPTest.h : main header file for the HTTPTEST application
//

#if !defined(AFX_HTTPTEST_H__3B620856_6D35_438E_8CF8_6A558DEB000A__INCLUDED_)
#define AFX_HTTPTEST_H__3B620856_6D35_438E_8CF8_6A558DEB000A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CHTTPTestApp:
// See HTTPTest.cpp for the implementation of this class
//

class CHTTPTestApp : public CWinApp
{
public:
	CHTTPTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHTTPTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CHTTPTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HTTPTEST_H__3B620856_6D35_438E_8CF8_6A558DEB000A__INCLUDED_)
