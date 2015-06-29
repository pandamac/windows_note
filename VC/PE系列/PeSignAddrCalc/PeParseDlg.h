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
	VOID CalcAddr(int nInNum, DWORD dwAddr);
	int GetAddrInSecNum(DWORD dwAddr);
    void HexStrToInt(char *szHexStr, DWORD *pdwHexVal);
	DWORD GetAddr();
	VOID InitAddrEdit();
	int m_nSelect;
	VOID GetPeInfo();
	VOID EnumSections();
	VOID ParseBasePe();
	VOID FileClose();
	PIMAGE_DOS_HEADER m_pDosHdr;
    PIMAGE_NT_HEADERS m_pNtHdr;
    PIMAGE_SECTION_HEADER m_pSecHdr;

	BOOL FileCreate(char *szFileName);
	LPVOID m_lpBase;
    HANDLE m_hMap;
	HANDLE m_hFile;
	BOOL IsPeFileAndGetPEPointer();
	VOID InitSectionList();

	CPeParseDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPeParseDlg)
	enum { IDD = IDD_PEPARSE_DIALOG };
	CEdit	m_EditFileOffset;
	CEdit	m_EditRva;
	CEdit	m_EditVa;
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
	afx_msg void OnRadioVa();
	afx_msg void OnRadioRva();
	afx_msg void OnRadioFileoffset();
	afx_msg void OnBtnCalc();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PEPARSEDLG_H__13847462_85E3_4E68_BAB8_D6548059CB2B__INCLUDED_)
