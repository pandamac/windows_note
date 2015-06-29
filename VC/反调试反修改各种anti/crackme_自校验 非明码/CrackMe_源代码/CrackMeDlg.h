// CrackMeDlg.h : header file
//

#if !defined(AFX_CRACKMEDLG_H__536A2B44_62F7_4CDF_A46D_89B28857C796__INCLUDED_)
#define AFX_CRACKMEDLG_H__536A2B44_62F7_4CDF_A46D_89B28857C796__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CCrackMeDlg dialog

class CCrackMeDlg : public CDialog
{
// Construction
public:
	CCrackMeDlg(CWnd* pParent = NULL);	// standard constructor
	void OnInit();
// Dialog Data
	//{{AFX_DATA(CCrackMeDlg)
	enum { IDD = IDD_CRACKME_DIALOG };
	CString	m_CSUserName;
	CString	m_CSRegKey;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCrackMeDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL MemCRC32();
	UINT TimerID;
	BOOL CheckReg(CString UserName,int RegKey);
	DWORD CheckRam(DWORD add1,DWORD size);
	DWORD CRC32(BYTE *ptr,DWORD Size);
	BOOL IsFileModified();
	
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CCrackMeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonok();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CRACKMEDLG_H__536A2B44_62F7_4CDF_A46D_89B28857C796__INCLUDED_)
