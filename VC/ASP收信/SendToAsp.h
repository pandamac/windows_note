// SendToAsp.h : main header file for the SENDTOASP application
//

#if !defined(AFX_SENDTOASP_H__A4458935_B509_4F00_A9BE_4B16F0AC16EE__INCLUDED_)
#define AFX_SENDTOASP_H__A4458935_B509_4F00_A9BE_4B16F0AC16EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSendToAspApp:
// See SendToAsp.cpp for the implementation of this class
//

class CSendToAspApp : public CWinApp
{
public:
	CSendToAspApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendToAspApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSendToAspApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDTOASP_H__A4458935_B509_4F00_A9BE_4B16F0AC16EE__INCLUDED_)
