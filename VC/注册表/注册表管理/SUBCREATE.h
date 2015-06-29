#if !defined(AFX_SUBCREATE_H__0003DA78_E8FE_4748_9C1A_BA08D09AE219__INCLUDED_)
#define AFX_SUBCREATE_H__0003DA78_E8FE_4748_9C1A_BA08D09AE219__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SUBCREATE.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SUBCREATE dialog

class SUBCREATE : public CDialog
{
// Construction
public:
	void SetDate(HKEY mkey,CString subkey);
	CString SubKey;
	HKEY MKEY;
	SUBCREATE(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SUBCREATE)
	enum { IDD = IDD_CREATE_SUBKEY };
	CString	m_sub_value;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SUBCREATE)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SUBCREATE)
	afx_msg void OnCreate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUBCREATE_H__0003DA78_E8FE_4748_9C1A_BA08D09AE219__INCLUDED_)
