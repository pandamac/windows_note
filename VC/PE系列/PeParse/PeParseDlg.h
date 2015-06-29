// PeParseDlg.h : header file
//

#if !defined(AFX_PEPARSEDLG_H__13847462_85E3_4E68_BAB8_D6548059CB2B__INCLUDED_)
#define AFX_PEPARSEDLG_H__13847462_85E3_4E68_BAB8_D6548059CB2B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPeParseDlg dialog

class CPeParseDlg : public CDialog
{
// Construction
public:
	VOID EnumSections();                // 枚举节表
	VOID ParseBasePe();                 // 解析基础PE字段
	VOID FileClose();                   // 关闭文件
	PIMAGE_DOS_HEADER m_pDosHdr;        // DOS头指针
    PIMAGE_NT_HEADERS m_pNtHdr;         // PE头指针
    PIMAGE_SECTION_HEADER m_pSecHdr;    // 节表指针

	BOOL FileCreate(char *szFileName);  // 打开文件并创建映像视图
	LPVOID m_lpBase;                    // 映像视图指针
    HANDLE m_hMap;                      // 映像句柄
	HANDLE m_hFile;                     // 文件句柄
	BOOL IsPeFileAndGetPEPointer();     // 判断是否为PE文件，并获取相关结构指针
	VOID InitSectionList();             // 初始化列表框

	CPeParseDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPeParseDlg)
	enum { IDD = IDD_PEPARSE_DIALOG };
	CListCtrl	m_SectionLIst;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPeParseDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPeParseDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnExit();
	afx_msg void OnBtnShow();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PEPARSEDLG_H__13847462_85E3_4E68_BAB8_D6548059CB2B__INCLUDED_)
