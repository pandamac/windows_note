// ·´Ïò×Ö·û´®reverseDlg.h : header file
//

#if !defined(AFX_REVERSEDLG_H__A4444D65_9A30_4900_A1F1_F24A4D6BBD90__INCLUDED_)
#define AFX_REVERSEDLG_H__A4444D65_9A30_4900_A1F1_F24A4D6BBD90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CReverseDlg dialog

class CReverseDlg : public CDialog
{
// Construction
public:
	CReverseDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CReverseDlg)
	enum { IDD = IDD_REVERSE_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReverseDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CReverseDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnOk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REVERSEDLG_H__A4444D65_9A30_4900_A1F1_F24A4D6BBD90__INCLUDED_)
