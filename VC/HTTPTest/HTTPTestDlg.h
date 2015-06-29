// HTTPTestDlg.h : header file
//

#if !defined(AFX_HTTPTESTDLG_H__31C96ECE_6F01_4BBF_8544_3FDCB37F155B__INCLUDED_)
#define AFX_HTTPTESTDLG_H__31C96ECE_6F01_4BBF_8544_3FDCB37F155B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CHTTPTestDlg dialog

class CHTTPTestDlg : public CDialog
{
// Construction
public:
	void ShowYourAddr();
	CString m_IP;
	CHTTPTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CHTTPTestDlg)
	enum { IDD = IDD_HTTPTEST_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHTTPTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CHTTPTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton1();
	afx_msg void OnButtonNew();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HTTPTESTDLG_H__31C96ECE_6F01_4BBF_8544_3FDCB37F155B__INCLUDED_)
