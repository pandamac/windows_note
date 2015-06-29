// MulAppExeDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CMulAppExeDlg 对话框
class CMulAppExeDlg : public CDialog
{
// 构造
public:
	CMulAppExeDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MULAPPEXE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

private:
	void RunAppWithHook(LPTSTR tzFilePathName, LPTSTR tzDllPath);

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnNMClickSyslink1(NMHDR *pNMHDR, LRESULT *pResult);
	CRichEditCtrl m_RichEditMessage;
	afx_msg void OnBnClickedButtonFile();
	CEdit m_EditFile;
	afx_msg void OnBnClickedButtonExecute();
};
