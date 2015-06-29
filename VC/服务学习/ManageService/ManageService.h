// ManageService.h : main header file for the MANAGESERVICE application
//

#if !defined(AFX_MANAGESERVICE_H__7E9854D5_1F38_4BED_8E30_CE6B6E5B9A3D__INCLUDED_)
#define AFX_MANAGESERVICE_H__7E9854D5_1F38_4BED_8E30_CE6B6E5B9A3D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CManageServiceApp:
// See ManageService.cpp for the implementation of this class
//

class CManageServiceApp : public CWinApp
{
public:
	CManageServiceApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CManageServiceApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CManageServiceApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MANAGESERVICE_H__7E9854D5_1F38_4BED_8E30_CE6B6E5B9A3D__INCLUDED_)
