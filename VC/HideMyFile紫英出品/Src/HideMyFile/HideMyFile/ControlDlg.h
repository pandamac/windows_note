#pragma once
#include "afxwin.h"
#include "ImageStatic.h"
#include "ConfigManager.h"
#include "DriverManager.h"

// CControlDlg 对话框

class CControlDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CControlDlg)

public:
	CControlDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CControlDlg();

// 对话框数据
	enum { IDD = IDD_DLG_CONTROL };

private:
       CBrush m_BkBrush;
       CFont  m_PwdFont;
       BOOL   m_bOptionLoaded;

       void SetCheckState(UINT uID, BOOL bState);
       void SetOptionState(UINT uID1, UINT uID2);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
    virtual void PostNcDestroy();
    virtual void OnCancel();
public:
    void SetDlgOptions();
    void SetAutoStart(BOOL bFlag);
    void SetWin7AutoStart(BOOL bFlag);
    void SetXPAutoStart(BOOL bFlag);

    virtual BOOL OnInitDialog();
    afx_msg LRESULT OnNcHitTest(CPoint point);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnStnClickedConfigClose();
    afx_msg void OnStnClickedChkAutostart();
    afx_msg void OnStnClickedChkShowtray();
    afx_msg void OnStnClickedOptTotray();
    afx_msg void OnStnClickedOptClose();
    afx_msg void OnPaint();
    afx_msg void OnStnClickedBtnSave();
    afx_msg void OnStnClickedChkHideprocess();
public:
    CImageStatic m_ConfigClose;
    CImageStatic m_BtnSavePwd;
    virtual BOOL PreTranslateMessage(MSG* pMsg);
};
