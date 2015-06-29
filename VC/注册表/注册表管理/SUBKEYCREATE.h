#if !defined(AFX_SUBKEYCREATE_H__A80D78BD_AD35_4BBB_994E_0BAF6F264A28__INCLUDED_)
#define AFX_SUBKEYCREATE_H__A80D78BD_AD35_4BBB_994E_0BAF6F264A28__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SUBKEYCREATE.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SUBKEYCREATE dialog

class SUBKEYCREATE : public CDialog
{
// Construction
public:
	SUBKEYCREATE(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SUBKEYCREATE)
	enum { IDD = IDD_CREATE_SUBKEY };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SUBKEYCREATE)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SUBKEYCREATE)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUBKEYCREATE_H__A80D78BD_AD35_4BBB_994E_0BAF6F264A28__INCLUDED_)
