// LoginDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HideMyFile.h"
#include "LoginDlg.h"
#include "afxdialogex.h"
#include "ConfigManager.h"

extern CConfigManager g_ConfigManager;

// CLoginDlg 对话框

IMPLEMENT_DYNAMIC(CLoginDlg, CDialogEx)

CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLoginDlg::IDD, pParent)
{

}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LOGIN_CLOSE, m_BtnClose);
    DDX_Control(pDX, IDC_LOGIN_OK, m_BtnOK);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialogEx)
    ON_WM_CTLCOLOR()
    ON_WM_PAINT()
    ON_WM_NCHITTEST()
    ON_STN_CLICKED(IDC_LOGIN_CLOSE, &CLoginDlg::OnStnClickedLoginClose)
    ON_STN_CLICKED(IDC_LOGIN_OK, &CLoginDlg::OnStnClickedLoginOk)
END_MESSAGE_MAP()


// CLoginDlg 消息处理程序


BOOL CLoginDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    //对话框背景
    CBitmap bitmapBackgroud;
    bitmapBackgroud.LoadBitmapW(IDB_LOGIN_BG);
    m_BkBrush.CreatePatternBrush(&bitmapBackgroud);
    bitmapBackgroud.DeleteObject();

    //路径框字体
    m_EditFont.CreateFont(24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, L"Microsoft YaHei");

    CEdit *pEdit = (CEdit*)(GetDlgItem(IDC_LOGIN_PWD));
    pEdit->SetFont(&m_EditFont);
    pEdit->SetFocus();

    m_BtnClose.SetButtonImages(IDB_CLOSE_NORMAL, IDB_CLOSE_ACTIVE);
    m_BtnOK.SetButtonImages(IDB_LOGIN_OK_NORMAL, IDB_LOGIN_OK_ACTIVE);

    ::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


HBRUSH CLoginDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

    if (pWnd == this)
    {
        return m_BkBrush;
    }

    if (CTLCOLOR_STATIC == nCtlColor)
    {
        pDC->SetBkMode(TRANSPARENT);
        return (HBRUSH)(GetStockObject(NULL_BRUSH));
    }

    return hbr;
}


void CLoginDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO: 在此处添加消息处理程序代码
    // 不为绘图消息调用 CDialogEx::OnPaint()
}


LRESULT CLoginDlg::OnNcHitTest(CPoint point)
{
    UINT nHitTest = CDialog::OnNcHitTest(point);

    if (nHitTest == HTCLIENT)
    {
        return HTCAPTION;
    }

    return nHitTest;
   
    //return CDialogEx::OnNcHitTest(point);
}


void CLoginDlg::OnStnClickedLoginClose()
{
    OnCancel();
}


void CLoginDlg::OnStnClickedLoginOk()
{
    CString strPwd;
    strPwd = g_ConfigManager.GetConfigValueString(CCM_TYPE_CONFIGPWD);
    strPwd = g_ConfigManager.XORPassword(strPwd);


    CString strInputPwd;
    GetDlgItemText(IDC_LOGIN_PWD, strInputPwd.GetBuffer(100), 100);
    strInputPwd.ReleaseBuffer();

    if (strInputPwd == strPwd)
    {
        OnOK();
    }
    else
    {
        MessageBox(L"密码不对哦~！仔细想想哟~！^_^", L"错误", MB_OK|MB_ICONERROR);
    }
}


BOOL CLoginDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类

    if (pMsg->message == WM_KEYDOWN)
    {
        if(pMsg->wParam == VK_RETURN)
        {
            if (GetFocus() == GetDlgItem(IDC_LOGIN_PWD))
            {
                OnStnClickedLoginOk();
                return TRUE;
            }
        }
        else if (pMsg->wParam == VK_ESCAPE)
        {
            return TRUE;
        }
    }
    return CDialogEx::PreTranslateMessage(pMsg);
}
