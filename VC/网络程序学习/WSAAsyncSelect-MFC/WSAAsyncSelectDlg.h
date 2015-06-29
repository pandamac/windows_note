// WSAAsyncSelectDlg.h : header file
//

#if !defined(AFX_WSAASYNCSELECTDLG_H__3F807EAF_274A_4250_8B94_0FE03D91A1DB__INCLUDED_)
#define AFX_WSAASYNCSELECTDLG_H__3F807EAF_274A_4250_8B94_0FE03D91A1DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CWSAAsyncSelectDlg dialog

class CWSAAsyncSelectDlg : public CDialog
{
// Construction
public:
	CWSAAsyncSelectDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CWSAAsyncSelectDlg)
	enum { IDD = IDD_WSAASYNCSELECT_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWSAAsyncSelectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CWSAAsyncSelectDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton1();
	afx_msg LRESULT WindowProc( WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WSAASYNCSELECTDLG_H__3F807EAF_274A_4250_8B94_0FE03D91A1DB__INCLUDED_)
