// HipsDlg.h : header file
//

#if !defined(AFX_HIPSDLG_H__FC3A3FF9_82B7_448E_BC91_CB49789A0DD4__INCLUDED_)
#define AFX_HIPSDLG_H__FC3A3FF9_82B7_448E_BC91_CB49789A0DD4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CHipsDlg dialog

#define UM_TRAYNOTIFICATION (WM_USER+100)        //自定义消息
#define ID_TASKBARICON 100

class CHipsDlg : public CDialog
{
// Construction
public:
	VOID InitIconNotify();
	NOTIFYICONDATA m_Nid;
	VOID InitHipsReports();
	HINSTANCE m_hInst;
	CHipsDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CHipsDlg)
	enum { IDD = IDD_HIPS_DIALOG };
	CListCtrl	m_HipsReports;
	CButton	m_BtnOff;
	CButton	m_BtnOn;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHipsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
    //{{AFX_MSG(CHipsDlg)
    afx_msg LRESULT OnTrayNotification(WPARAM wId, LPARAM lEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnBtnOn();
	afx_msg void OnBtnOff();
	afx_msg void OnBtnClearlog();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HIPSDLG_H__FC3A3FF9_82B7_448E_BC91_CB49789A0DD4__INCLUDED_)
