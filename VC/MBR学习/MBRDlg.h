// MBRDlg.h : header file
//

#if !defined(AFX_MBRDLG_H__B85FFE66_AA5B_4E11_BFD6_9E5BD470A157__INCLUDED_)
#define AFX_MBRDLG_H__B85FFE66_AA5B_4E11_BFD6_9E5BD470A157__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMBRDlg dialog

class CMBRDlg : public CDialog
{
// Construction
public:
	CMBRDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMBRDlg)
	enum { IDD = IDD_MBR_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMBRDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMBRDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MBRDLG_H__B85FFE66_AA5B_4E11_BFD6_9E5BD470A157__INCLUDED_)
