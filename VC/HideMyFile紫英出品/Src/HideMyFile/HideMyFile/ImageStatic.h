#pragma once
#include "afxwin.h"
class CImageStatic :
    public CStatic
{
public:
    CImageStatic(void);
    ~CImageStatic(void);
    DECLARE_MESSAGE_MAP()
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);

public:
    void SetButtonImages(UINT nIDNormal, UINT nIDActive);
    void SetCurrentImage(UINT nIDCurrent);
private:
    UINT m_BitmapNormal;
    UINT m_BitmapActive;
public:

    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

