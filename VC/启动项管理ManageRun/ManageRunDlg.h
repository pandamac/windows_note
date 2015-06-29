// ManageRunDlg.h : header file
//

#if !defined(AFX_MANAGERUNDLG_H__2900C8CB_6E7C_45B3_ADF3_D36ED97A003B__INCLUDED_)
#define AFX_MANAGERUNDLG_H__2900C8CB_6E7C_45B3_ADF3_D36ED97A003B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CManageRunDlg dialog

class CManageRunDlg : public CDialog
{
// Construction
public:
	VOID ShowRunList();
	VOID InitRunList();
	CManageRunDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CManageRunDlg)
	enum { IDD = IDD_MANAGERUN_DIALOG };
	CListCtrl	m_RunList;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CManageRunDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CManageRunDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnAdd();
	afx_msg void OnBtnDel();
	afx_msg void OnBtnQuit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MANAGERUNDLG_H__2900C8CB_6E7C_45B3_ADF3_D36ED97A003B__INCLUDED_)
