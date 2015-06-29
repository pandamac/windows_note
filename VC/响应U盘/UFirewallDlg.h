// UFirewallDlg.h : header file
//

#if !defined(AFX_UFIREWALLDLG_H__519B40E6_3418_430E_94CD_7CA833ECDA2A__INCLUDED_)
#define AFX_UFIREWALLDLG_H__519B40E6_3418_430E_94CD_7CA833ECDA2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CUFirewallDlg dialog

class CUFirewallDlg : public CDialog
{
// Construction
public:
	CString DriverName;
	VOID GetDriverName(DWORD dwData);
	CUFirewallDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CUFirewallDlg)
	enum { IDD = IDD_UFIREWALL_DIALOG };
	CButton	m_SafeOpen;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUFirewallDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
    
	// Generated message map functions
    //{{AFX_MSG(CUFirewallDlg)
    afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD dwData);
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnSafeopen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UFIREWALLDLG_H__519B40E6_3418_430E_94CD_7CA833ECDA2A__INCLUDED_)
