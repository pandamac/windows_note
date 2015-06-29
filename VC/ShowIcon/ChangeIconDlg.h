#if !defined(AFX_CHANGEICONDLG_H__B9FC5B87_7BEF_11D3_905B_444553540001__INCLUDED_)
#define AFX_CHANGEICONDLG_H__B9FC5B87_7BEF_11D3_905B_444553540001__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChangeIconDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChangeIconDlg dialog
#include "resource.h"
#include "IconListBox.h"

class CChangeIconDlg : public CDialog
{
// Construction
public:
	CChangeIconDlg(CWnd* pParent = NULL);   // standard constructor
	int m_nIndex;
// Dialog Data
	//{{AFX_DATA(CChangeIconDlg)
	enum { IDD = IDD_CHANGEICONDIALOG };
	CString	m_strFileName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChangeIconDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CIconListBox m_listboxIcon;
	CString lpszPath;
	BOOL bAccord;
	void ShowIcon();
	// Generated message map functions
	//{{AFX_MSG(CChangeIconDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonBrowse();
	virtual void OnOK();
	afx_msg void OnUpdateEditboxFilename();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGEICONDLG_H__B9FC5B87_7BEF_11D3_905B_444553540001__INCLUDED_)
