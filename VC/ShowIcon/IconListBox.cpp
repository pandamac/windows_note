// IconListBox.cpp : implementation file
//

#include "stdafx.h"
#include "IconListBox.h"
#include "ChangeIconDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIconListBox

CIconListBox::CIconListBox()
{
	nSelect=0;
}

CIconListBox::~CIconListBox()
{
}


BEGIN_MESSAGE_MAP(CIconListBox, CListBox)
	//{{AFX_MSG_MAP(CIconListBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIconListBox message handlers
void CIconListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	int n=lpDrawItemStruct->itemID;
	x=lpDrawItemStruct->rcItem.left;
	y=lpDrawItemStruct->rcItem.top;
	if(lpDrawItemStruct->itemAction & ODA_DRAWENTIRE)
	{
		pDC->DrawIcon(x+1,y+2,m_hIcon[n]);
	}
	if ((lpDrawItemStruct->itemState & ODS_SELECTED) &&
		(lpDrawItemStruct->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
	{
		pDC->FillSolidRect(x,y,34,40,GetSysColor(COLOR_HIGHLIGHT));
		pDC->DrawIcon(x+1,y+2,m_hIcon[n]);
		nSelect=n;
	}
	if ((lpDrawItemStruct->itemAction & ODA_SELECT)&&
		!(lpDrawItemStruct->itemState & ODS_SELECTED))
	{
		pDC->FillSolidRect(x,y,34,40,GetSysColor(COLOR_WINDOW));
		pDC->DrawIcon(x+1,y+2,m_hIcon[n]);
	}
}

void CIconListBox::AddIconItem(HICON hIcon,int index)
{
	int nIndex=AddString((LPCTSTR)hIcon);
	m_hIcon[nIndex]=hIcon;
	nSelect=index;
}