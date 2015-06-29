// ChangeIcon.h : main header file for the CHANGEICON application
//

#if !defined(AFX_CHANGEICON_H__4D58AA4A_FE03_468F_A196_3D65E0A8E3A5__INCLUDED_)
#define AFX_CHANGEICON_H__4D58AA4A_FE03_468F_A196_3D65E0A8E3A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CChangeIconApp:
// See ChangeIcon.cpp for the implementation of this class
//

class CChangeIconApp : public CWinApp
{
public:
	CChangeIconApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChangeIconApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CChangeIconApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGEICON_H__4D58AA4A_FE03_468F_A196_3D65E0A8E3A5__INCLUDED_)
