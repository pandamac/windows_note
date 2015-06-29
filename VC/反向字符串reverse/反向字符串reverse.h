// 反向字符串reverse.h : main header file for the 反向字符串REVERSE application
//

#if !defined(AFX_REVERSE_H__905A6EEA_F29A_4984_8FA2_7991DE2B289D__INCLUDED_)
#define AFX_REVERSE_H__905A6EEA_F29A_4984_8FA2_7991DE2B289D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CReverseApp:
// See 反向字符串reverse.cpp for the implementation of this class
//

class CReverseApp : public CWinApp
{
public:
	CReverseApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReverseApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CReverseApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REVERSE_H__905A6EEA_F29A_4984_8FA2_7991DE2B289D__INCLUDED_)
