// SSDT查看恢复工具Dlg.h : header file
//

#if !defined(AFX_SSDTDLG_H__89779584_C412_4CF6_88A1_1F593C86E234__INCLUDED_)
#define AFX_SSDTDLG_H__89779584_C412_4CF6_88A1_1F593C86E234__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CSSDTDlg dialog

class CSSDTDlg : public CDialog
{
// Construction
public:
	CSSDTDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSSDTDlg)
	enum { IDD = IDD_SSDT_DIALOG };
	CListCtrl	m_List;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSSDTDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void OnDrawColorForMyList( NMHDR* pNmHdr, LRESULT *pResult );
	void ShowSSDT();
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSSDTDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnReShowSSDT();
	afx_msg void OnReSSDT();
	afx_msg void OnReSSDTAndThrowSpilth();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SSDTDLG_H__89779584_C412_4CF6_88A1_1F593C86E234__INCLUDED_)
