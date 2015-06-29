// SSDT查看恢复工具.h : main header file for the SSDT查看恢复工具 application
//

#if !defined(AFX_SSDT_H__11510C0E_6006_4B46_95D6_A9DC747C0D1B__INCLUDED_)
#define AFX_SSDT_H__11510C0E_6006_4B46_95D6_A9DC747C0D1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSSDTApp:
// See SSDT查看恢复工具.cpp for the implementation of this class
//

class CSSDTApp : public CWinApp
{
public:
	CSSDTApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSSDTApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSSDTApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SSDT_H__11510C0E_6006_4B46_95D6_A9DC747C0D1B__INCLUDED_)
