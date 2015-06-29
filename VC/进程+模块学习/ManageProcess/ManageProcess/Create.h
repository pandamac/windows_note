#if !defined(AFX_CREATE_H__BA9FD32A_65F4_4121_A0C3_2462F2884838__INCLUDED_)
#define AFX_CREATE_H__BA9FD32A_65F4_4121_A0C3_2462F2884838__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Create.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCreate dialog

class CCreate : public CDialog
{
// Construction
public:
	char szFilePath[MAX_PATH];
	CCreate(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCreate)
	enum { IDD = IDD_DLG_CREATE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreate)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCreate)
	afx_msg void OnBtnCancel();
	afx_msg void OnBtnOk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CREATE_H__BA9FD32A_65F4_4121_A0C3_2462F2884838__INCLUDED_)
