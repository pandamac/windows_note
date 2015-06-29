// WSAAsyncSelect.h : main header file for the WSAASYNCSELECT application
//

#if !defined(AFX_WSAASYNCSELECT_H__B72DE63E_35ED_4995_A7B8_63FE82A8DBE5__INCLUDED_)
#define AFX_WSAASYNCSELECT_H__B72DE63E_35ED_4995_A7B8_63FE82A8DBE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CWSAAsyncSelectApp:
// See WSAAsyncSelect.cpp for the implementation of this class
//

class CWSAAsyncSelectApp : public CWinApp
{
public:
	CWSAAsyncSelectApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWSAAsyncSelectApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CWSAAsyncSelectApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WSAASYNCSELECT_H__B72DE63E_35ED_4995_A7B8_63FE82A8DBE5__INCLUDED_)
