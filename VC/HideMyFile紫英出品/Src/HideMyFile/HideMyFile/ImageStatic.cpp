#include "StdAfx.h"
#include "ImageStatic.h"
#include "resource.h"

CImageStatic::CImageStatic(void)
{
}


CImageStatic::~CImageStatic(void)
{
}
BEGIN_MESSAGE_MAP(CImageStatic, CStatic)
    ON_WM_MOUSEMOVE()
ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


void CImageStatic::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    ::SetCursor(::LoadCursor(NULL, IDC_HAND));
    CRect rect;
    GetClientRect(&rect);

    HINSTANCE hInst= GetModuleHandle(NULL);
    HBITMAP hBitmap;

    if (GetCapture() != this)
    {
        SetCapture();
        Invalidate(FALSE);
    }
    else
    {
        if (rect.PtInRect(point))
        {
            hBitmap =  LoadBitmap(hInst, MAKEINTRESOURCE(m_BitmapActive));
            SetBitmap(hBitmap);
        }
        else
        {
            hBitmap =  LoadBitmap(hInst, MAKEINTRESOURCE(m_BitmapNormal));
            SetBitmap(hBitmap);
        }
        ReleaseCapture();
        Invalidate(FALSE);
    }
    CStatic::OnMouseMove(nFlags, point);
}

void CImageStatic::SetButtonImages( UINT nIDNormal, UINT nIDActive )
{
    m_BitmapNormal = nIDNormal;
    m_BitmapActive = nIDActive;
}

void CImageStatic::SetCurrentImage( UINT nIDCurrent )
{
    HINSTANCE hInst= GetModuleHandle(NULL);
    HBITMAP hBitmap;
    hBitmap =  LoadBitmap(hInst, MAKEINTRESOURCE(nIDCurrent));
    SetBitmap(hBitmap);
}

void CImageStatic::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    SetCurrentImage(m_BitmapNormal);
    CStatic::OnLButtonDown(nFlags, point);
}
