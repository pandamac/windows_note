// MyShellDlg.h : header file
//

#if !defined(AFX_MYSHELLDLG_H__34E2E56F_4D29_4CBD_BD52_9545BAF4D91D__INCLUDED_)
#define AFX_MYSHELLDLG_H__34E2E56F_4D29_4CBD_BD52_9545BAF4D91D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMyShellDlg dialog

class CMyShellDlg : public CDialog
{
// Construction
public:
	VOID Encode();
	IMAGE_SECTION_HEADER m_SecNewHdr;
	VOID WriteDecode();
    DWORD AlignSize(int nSecSize, DWORD Alignment);
	VOID AddSection();
	IMAGE_NT_HEADERS m_NtHdr;
	IMAGE_DOS_HEADER m_ImgDos;
	VOID GetPeInfo();
	DWORD m_dwEntryPoint;
	DWORD m_dwImageBase;
	IMAGE_SECTION_HEADER m_SecTextInfo;
	CString m_StrFile;
	HANDLE m_hFile;
	BOOL IsPeFile();
	CMyShellDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMyShellDlg)
	enum { IDD = IDD_MYSHELL_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyShellDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMyShellDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnShell();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYSHELLDLG_H__34E2E56F_4D29_4CBD_BD52_9545BAF4D91D__INCLUDED_)
