
// HideMyFileDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "ImageStatic.h"
#include "ControlDlg.h"

//托盘消息
#define  WM_NOTIFYICON WM_USER+100 

//快捷键消息
#define  WM_HIDEHOTKEY WM_USER+101


// CHideMyFileDlg 对话框
class CHideMyFileDlg : public CDialogEx
{
// 构造
public:
	CHideMyFileDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_HIDEMYFOLDER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

private:
    CBrush m_BkBrush; //背景画刷
    CFont  m_EditFont; //路径框字体
    CFont  m_ListFont;//文件列表框字体
    CControlDlg *m_ControlDlg; //软件设置
    BOOL   m_IsShowLoginDlg;
    void  CheckSingleton();
private:
    void InitializeControls();

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
    CImageStatic m_BtnControl;
    CImageStatic m_BtnAdd;
    CImageStatic m_BtnMinimize;
    CImageStatic m_BtnClose;
    CImageStatic m_BtnSearch;
    CEdit m_FilePath;
    BOOL m_bIsVisable;
    NOTIFYICONDATA m_ntIcon;

public:
    void AddToTrayIcon();
    void ReadFileList();
    int GetSelectItemIndex();
    int GetFileItemID(int nIndex);
    BOOL CheckExistFileInList(CString strFilePath);
    BOOL EnableDebugPrv();
    BOOL CheckLogin();
    virtual void OnOK();
    virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnNcHitTest(CPoint point);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg LRESULT OnNotifyIcon(WPARAM wParam, LPARAM lParam);
    afx_msg void OnStnClickedBtnClose();
    afx_msg void OnStnClickedBtnMin();
    afx_msg void OnStnClickedBtnControl();
    afx_msg void OnStnClickedBtnAdd();
    afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
    afx_msg void OnExit();
    afx_msg void OnNMRClickFolderList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnHideFile();
    afx_msg void OnShowFile();
    afx_msg void OnCancelHide();
    afx_msg void OnRefreshList();
    afx_msg void OnExploerFile();
    afx_msg void OnStnClickedBtnSearch();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
};
