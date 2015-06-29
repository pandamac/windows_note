#pragma once
#include "ImageStatic.h"
#include "afxwin.h"

// CLoginDlg 对话框

class CLoginDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLoginDlg)

public:
	CLoginDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLoginDlg();

// 对话框数据
	enum { IDD = IDD_DLG_LOGIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
    CBrush m_BkBrush;
    CFont  m_EditFont;

public:
    virtual BOOL OnInitDialog();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnPaint();
    CImageStatic m_BtnClose;
    CImageStatic m_BtnOK;
    afx_msg LRESULT OnNcHitTest(CPoint point);
    afx_msg void OnStnClickedLoginClose();
    afx_msg void OnStnClickedLoginOk();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
};
