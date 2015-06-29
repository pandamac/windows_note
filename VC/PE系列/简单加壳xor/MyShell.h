// MyShell.h : main header file for the MYSHELL application
//

#if !defined(AFX_MYSHELL_H__9BC0A89B_1C9E_4D01_9B1A_3DF88AFA6C4E__INCLUDED_)
#define AFX_MYSHELL_H__9BC0A89B_1C9E_4D01_9B1A_3DF88AFA6C4E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMyShellApp:
// See MyShell.cpp for the implementation of this class
//

class CMyShellApp : public CWinApp
{
public:
	CMyShellApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyShellApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMyShellApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYSHELL_H__9BC0A89B_1C9E_4D01_9B1A_3DF88AFA6C4E__INCLUDED_)
