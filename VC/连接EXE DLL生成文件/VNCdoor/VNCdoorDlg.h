// VNCdoorDlg.h : header file
//

#if !defined(AFX_VNCDOORDLG_H__A3F6057B_38BC_40E5_AED3_E42A4B857933__INCLUDED_)
#define AFX_VNCDOORDLG_H__A3F6057B_38BC_40E5_AED3_E42A4B857933__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CVNCdoorDlg dialog

class CVNCdoorDlg : public CDialog
{
// Construction
public:
	CVNCdoorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CVNCdoorDlg)
	enum { IDD = IDD_VNCDOOR_DIALOG };
	int		m_port1;
	CString	m_pass;
	CString	m_backip;
	int		m_port2;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVNCdoorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CVNCdoorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	afx_msg void OnButton4();
	afx_msg void OnButton3();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VNCDOORDLG_H__A3F6057B_38BC_40E5_AED3_E42A4B857933__INCLUDED_)
