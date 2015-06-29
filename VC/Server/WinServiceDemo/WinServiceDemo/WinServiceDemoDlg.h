// WinServiceDemoDlg.h : 头文件
//

#pragma once
#include "WindowsService.h"
#define WM_SHOWTASK  WM_USER +100
// CWinServiceDemoDlg 对话框
class CWinServiceDemoDlg : public CDialog
{
// 构造
public:
	CWinServiceDemoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_WINSERVICEDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
//	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	CWindowsService m_Service;
	NOTIFYICONDATA m_TipIcon;
	char m_szTipShowInfo[200];
	BYTE m_bMsbox;
public:
	afx_msg void OnBnClickedButtonCheck();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void ShowIconToTip(BOOL bShow = TRUE);
	void ShowBalloonTip(const char* pMsg, const char*pTitle ="服务保护程序", UINT uTimeout=20, DWORD dwInfoFlags=1);
	afx_msg LRESULT OnShowTask(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
};
