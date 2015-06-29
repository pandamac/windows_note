#if !defined(AFX_ICONLISTBOX_H__B9FC5B85_7BEF_11D3_905B_444553540001__INCLUDED_)
#define AFX_ICONLISTBOX_H__B9FC5B85_7BEF_11D3_905B_444553540001__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IconListBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIconListBox window

class CIconListBox : public CListBox
{
// Construction
public:
	CIconListBox();

// Attributes
public:
	HICON m_hIcon[255];
	int x;
	int y;
	int nSelect;
// Operations
public:
	void AddIconItem(HICON hIcon,int index);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIconListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CIconListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CIconListBox)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ICONLISTBOX_H__B9FC5B85_7BEF_11D3_905B_444553540001__INCLUDED_)
