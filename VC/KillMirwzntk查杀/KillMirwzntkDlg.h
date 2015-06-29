// KillMirwzntkDlg.h : header file
//

#if !defined(AFX_KILLMIRWZNTKDLG_H__3BAB0F4F_7551_4C06_8E08_22481DD50BDA__INCLUDED_)
#define AFX_KILLMIRWZNTKDLG_H__3BAB0F4F_7551_4C06_8E08_22481DD50BDA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CKillMirwzntkDlg dialog

class CKillMirwzntkDlg : public CDialog
{
// Construction
public:
	CKillMirwzntkDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CKillMirwzntkDlg)
	enum { IDD = IDD_KILLMIRWZNTK_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKillMirwzntkDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CKillMirwzntkDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnKill();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KILLMIRWZNTKDLG_H__3BAB0F4F_7551_4C06_8E08_22481DD50BDA__INCLUDED_)
