// 图像识别系统Dlg.h : header file
//

#if !defined(AFX_DLG_H__93C83A5B_FE6B_4AB2_8A04_9AE120AC0257__INCLUDED_)
#define AFX_DLG_H__93C83A5B_FE6B_4AB2_8A04_9AE120AC0257__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMyDlg dialog

class CMyDlg : public CDialog
{
// Construction
public:
	CMyDlg(CWnd* pParent = NULL);	// standard constructor

	void SetProcessInfo( char* Str );	//设置处理信息
	BOOL DownLoadBmp();			//下载验证码

// Dialog Data
	//{{AFX_DATA(CMyDlg)
	enum { IDD = IDD_MY_DIALOG };
	CStatic	m_Bitmap;
	CString	m_Url;
	CString	m_Num;
	CString	m_Correct;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMyDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnStart();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_H__93C83A5B_FE6B_4AB2_8A04_9AE120AC0257__INCLUDED_)
