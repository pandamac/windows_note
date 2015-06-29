// SendToAspDlg.h : header file
//

#if !defined(AFX_SENDTOASPDLG_H__D2A73762_1A33_4DF7_BA33_D922B1C1F500__INCLUDED_)
#define AFX_SENDTOASPDLG_H__D2A73762_1A33_4DF7_BA33_D922B1C1F500__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CSendToAspDlg dialog

class CSendToAspDlg : public CDialog
{
// Construction
public:
	CSendToAspDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSendToAspDlg)
	enum { IDD = IDD_SENDTOASP_DIALOG };
	CEdit	m_Edit3;
	CEdit	m_Edit2;
	CEdit	m_Edit1;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendToAspDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSendToAspDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDTOASPDLG_H__D2A73762_1A33_4DF7_BA33_D922B1C1F500__INCLUDED_)
