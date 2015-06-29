#if !defined(AFX_TIPS_H__2E575540_8B98_4393_BCEF_E84501BDF493__INCLUDED_)
#define AFX_TIPS_H__2E575540_8B98_4393_BCEF_E84501BDF493__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Tips.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTips dialog

class CTips : public CDialog
{
// Construction
public:
	WCHAR sz[2048];
	BOOL bRet;
	CTips(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTips)
	enum { IDD = IDD_DLG_TIP };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTips)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTips)
	afx_msg void OnBtnOk();
	afx_msg void OnBtnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIPS_H__2E575540_8B98_4393_BCEF_E84501BDF493__INCLUDED_)
