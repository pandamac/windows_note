// VNCdoor.h : main header file for the VNCDOOR application
//

#if !defined(AFX_VNCDOOR_H__F7F122B8_5ADC_4E0A_B900_156FC2C63446__INCLUDED_)
#define AFX_VNCDOOR_H__F7F122B8_5ADC_4E0A_B900_156FC2C63446__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CVNCdoorApp:
// See VNCdoor.cpp for the implementation of this class
//

class CVNCdoorApp : public CWinApp
{
public:
	CVNCdoorApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVNCdoorApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CVNCdoorApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VNCDOOR_H__F7F122B8_5ADC_4E0A_B900_156FC2C63446__INCLUDED_)
