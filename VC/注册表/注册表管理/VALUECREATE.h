#if !defined(AFX_VALUECREATE_H__BE53E30C_98F2_444D_AE51_58ED9D9B8F6D__INCLUDED_)
#define AFX_VALUECREATE_H__BE53E30C_98F2_444D_AE51_58ED9D9B8F6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VALUECREATE.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// VALUECREATE dialog

class VALUECREATE : public CDialog
{
// Construction
public:
	void SetDate(HKEY mkey,CString subkey);
	CString SubKey;
	HKEY MKEY;

	VALUECREATE(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(VALUECREATE)
	enum { IDD = IDD_CREATE_VALUE };
	CString	m_date;
	CString	m_name;
	CString	m_type;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VALUECREATE)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VALUECREATE)
	afx_msg void OnCreate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VALUECREATE_H__BE53E30C_98F2_444D_AE51_58ED9D9B8F6D__INCLUDED_)
