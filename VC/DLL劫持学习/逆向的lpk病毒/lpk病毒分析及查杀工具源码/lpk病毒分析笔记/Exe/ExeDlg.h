// ExeDlg.h : header file
//

#if !defined(AFX_EXEDLG_H__8778CB31_B3ED_4AEA_8F80_EBD3864FFF99__INCLUDED_)
#define AFX_EXEDLG_H__8778CB31_B3ED_4AEA_8F80_EBD3864FFF99__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <afxtempl.h>


/////////////////////////////////////////////////////////////////////////////
// CExeDlg dialog
struct DESTMODULEINFO
{
	DWORD dwProcessID;
	HMODULE hModule;
};

class CExeDlg : public CDialog
{
// Construction
public:
	CExeDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CExeDlg)
	enum { IDD = IDD_EXE_DIALOG };
	CButton	m_btnUnControl;
	CButton	m_btnControl;
	CListBox	m_FailList;
	CListBox	m_OKList;
	CButton	m_BtnDeleteLpk;
	CStatic	m_CurScanFileMsg;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CList <DESTMODULEINFO, DESTMODULEINFO&> m_DestInfo ;
	// Generated message map functions
	//{{AFX_MSG(CExeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnControlsource();
	afx_msg void OnFreecontrolsource();
	afx_msg void OnDeletelpk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void FindLpk(CString CurPath);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXEDLG_H__8778CB31_B3ED_4AEA_8F80_EBD3864FFF99__INCLUDED_)
