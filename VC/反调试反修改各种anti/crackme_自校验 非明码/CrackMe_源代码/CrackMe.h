// CrackMe.h : main header file for the CRACKME application
//

#if !defined(AFX_CRACKME_H__DDCE9A22_C3B6_4052_8259_9F8C2EAEA2DE__INCLUDED_)
#define AFX_CRACKME_H__DDCE9A22_C3B6_4052_8259_9F8C2EAEA2DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCrackMeApp:
// See CrackMe.cpp for the implementation of this class
//

class CCrackMeApp : public CWinApp
{
public:
	CCrackMeApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCrackMeApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCrackMeApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CRACKME_H__DDCE9A22_C3B6_4052_8259_9F8C2EAEA2DE__INCLUDED_)
