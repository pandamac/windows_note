// ManageServiceDlg.h : header file
//

#if !defined(AFX_MANAGESERVICEDLG_H__C525BCBE_B513_4A9B_9CA8_4EDBAB934349__INCLUDED_)
#define AFX_MANAGESERVICEDLG_H__C525BCBE_B513_4A9B_9CA8_4EDBAB934349__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CManageServiceDlg dialog

class CManageServiceDlg : public CDialog
{
// Construction
public:
	VOID ShowServiceList(DWORD dwServiceType = SERVICE_WIN32);
	VOID InitServiceList();
	CManageServiceDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CManageServiceDlg)
	enum { IDD = IDD_MANAGESERVICE_DIALOG };
	CListCtrl	m_ServiceList;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CManageServiceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CManageServiceDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnRadioWin32();
	afx_msg void OnRadioDriver();
	afx_msg void OnBtnStart();
	afx_msg void OnBtnStop();
	afx_msg void OnBtnExit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MANAGESERVICEDLG_H__C525BCBE_B513_4A9B_9CA8_4EDBAB934349__INCLUDED_)
