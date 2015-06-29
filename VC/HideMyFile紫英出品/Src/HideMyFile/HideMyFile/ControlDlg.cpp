// ControlDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HideMyFile.h"
#include "ControlDlg.h"
#include "afxdialogex.h"

extern CConfigManager g_ConfigManager;
extern CDriverManager g_DriverManager;

// CControlDlg 对话框

IMPLEMENT_DYNAMIC(CControlDlg, CDialogEx)

CControlDlg::CControlDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CControlDlg::IDD, pParent)
{
    m_bOptionLoaded = FALSE;
}

CControlDlg::~CControlDlg()
{
}

void CControlDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CCM_CLOSE, m_ConfigClose);
    DDX_Control(pDX, IDC_BTN_SAVE, m_BtnSavePwd);
}


BEGIN_MESSAGE_MAP(CControlDlg, CDialogEx)
    ON_WM_NCHITTEST()
    ON_WM_CTLCOLOR()
    ON_STN_CLICKED(IDC_CCM_CLOSE, &CControlDlg::OnStnClickedConfigClose)
    ON_STN_CLICKED(IDC_CHK_AUTOSTART, &CControlDlg::OnStnClickedChkAutostart)
    ON_STN_CLICKED(IDC_CHK_SHOWTRAY, &CControlDlg::OnStnClickedChkShowtray)
    ON_STN_CLICKED(IDC_OPT_TOTRAY, &CControlDlg::OnStnClickedOptTotray)
    ON_STN_CLICKED(IDC_OPT_CLOSE, &CControlDlg::OnStnClickedOptClose)
    ON_WM_PAINT()
    ON_STN_CLICKED(IDC_BTN_SAVE, &CControlDlg::OnStnClickedBtnSave)
    ON_STN_CLICKED(IDC_CHK_HIDEPROCESS, &CControlDlg::OnStnClickedChkHideprocess)
END_MESSAGE_MAP()


// CControlDlg 消息处理程序


void CControlDlg::PostNcDestroy()
{
    // TODO: 在此添加专用代码和/或调用基类

    CDialogEx::PostNcDestroy();

    delete this;
}


void CControlDlg::OnCancel()
{
    // TODO: 在此添加专用代码和/或调用基类
    DestroyWindow();
    //CDialogEx::OnCancel();
}


LRESULT CControlDlg::OnNcHitTest(CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    UINT nHitTest = CDialog::OnNcHitTest(point);

    if (nHitTest == HTCLIENT)
    {
        return HTCAPTION;
    }
    else
    {
        return nHitTest;
    }
    //return CDialogEx::OnNcHitTest(point);
}


HBRUSH CControlDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

    // TODO:  在此更改 DC 的任何特性
    if (pWnd == this)
    {
        return m_BkBrush;
    }

    if (CTLCOLOR_STATIC == nCtlColor)
    {
        pDC->SetBkMode(TRANSPARENT);
        return (HBRUSH)(GetStockObject(NULL_BRUSH));
    }
    // TODO:  如果默认的不是所需画笔，则返回另一个画笔
    return hbr;
}


BOOL CControlDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  在此添加额外的初始化

    m_ConfigClose.SetButtonImages(IDB_CLOSE_NORMAL, IDB_CLOSE_ACTIVE);
    m_BtnSavePwd.SetButtonImages(IDB_SAVE_NORMAL, IDB_SAVE_ACTIVE);


    CBitmap bitmapBackgroud;
    bitmapBackgroud.LoadBitmapW(IDB_CONTROL_BG);
    m_BkBrush.CreatePatternBrush(&bitmapBackgroud);
    bitmapBackgroud.DeleteObject();
    
    CenterWindow();

    m_PwdFont.CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, L"Microsoft YaHei");

    CEdit *pPwdEdit = (CEdit*)(GetDlgItem(IDC_EDIT_NEW_PWD));
    pPwdEdit->SetFont(&m_PwdFont);

    //SetDlgOptions();
    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void CControlDlg::SetCheckState( UINT uID, BOOL bState )
{
    CStatic *pCheckStatic = (CStatic *)(GetDlgItem(uID));
    if (bState)
    {

        CBitmap BitmapActive;
        BitmapActive.LoadBitmap(IDB_CHK_SEL);
        pCheckStatic->SetBitmap((HBITMAP)BitmapActive);
        BitmapActive.DeleteObject();
        UpdateWindow();
        //Invalidate(TRUE);
    }
    else
    {
        CBitmap BitmapNormal;
        BitmapNormal.LoadBitmap(IDB_CHK_UNSEL);
        pCheckStatic->SetBitmap((HBITMAP)BitmapNormal);
        BitmapNormal.DeleteObject();
        UpdateWindow();
        //Invalidate(TRUE);
    }
}

void CControlDlg::SetOptionState( UINT uID1, UINT uID2)
{
    CStatic *pOptionStatic1 = (CStatic *)(GetDlgItem(uID1));
    CStatic *pOptionStatic2 = (CStatic *)(GetDlgItem(uID2));

    CBitmap BitmapSel;
    CBitmap BitmapUnSel;

    BitmapSel.LoadBitmap(IDB_OPT_SEL);
    BitmapUnSel.LoadBitmap(IDB_OPT_UNSEL);

    pOptionStatic1->SetBitmap((HBITMAP)BitmapSel);
    pOptionStatic2->SetBitmap((HBITMAP)BitmapUnSel);

    BitmapSel.DeleteObject();
    BitmapUnSel.DeleteObject();

}


void CControlDlg::OnStnClickedConfigClose()
{
    OnOK();
}

void CControlDlg::OnStnClickedChkAutostart()
{
    //取反
    int nValue = !(g_ConfigManager.GetConfigValueInt(CCM_TYPE_AUTOSTART));

    //写入配置文件
    g_ConfigManager.SetConfigValueInt(CCM_TYPE_AUTOSTART, nValue);

    //改变选项状态
    SetCheckState(IDC_CHK_AUTOSTART, (BOOL)nValue);

    SetAutoStart((BOOL)nValue);
}

void CControlDlg::OnStnClickedChkHideprocess()
{
    //取反
    int nValue = !(g_ConfigManager.GetConfigValueInt(CCM_TYPE_HIDEPROCESS));

    //写入配置文件
    g_ConfigManager.SetConfigValueInt(CCM_TYPE_HIDEPROCESS, nValue);

    //改变选项状态
    SetCheckState(IDC_CHK_HIDEPROCESS, (BOOL)nValue);

    //取当前进程名
    CString strCurProcessName = g_DriverManager.GetCurrentProcessName();

    g_DriverManager.SetProcessHiddenState(strCurProcessName, (BOOL)nValue);
}

void CControlDlg::OnStnClickedChkShowtray()
{
    //取反
    int nValue = !(g_ConfigManager.GetConfigValueInt(CCM_TYPE_SHOWTRAY));

    //写入配置文件
    g_ConfigManager.SetConfigValueInt(CCM_TYPE_SHOWTRAY, nValue);

    //改变选项状态
    SetCheckState(IDC_CHK_SHOWTRAY, (BOOL)nValue);
}


void CControlDlg::OnStnClickedOptTotray()
{
    //取反
    int nValue = !(g_ConfigManager.GetConfigValueInt(CCM_TYPE_CLOSESTATE));

    //写入配置文件
    g_ConfigManager.SetConfigValueInt(CCM_TYPE_CLOSESTATE, nValue);


    SetOptionState(IDC_OPT_TOTRAY, IDC_OPT_CLOSE);
}

void CControlDlg::OnStnClickedOptClose()
{
    //取反
    //取反
    int nValue = !(g_ConfigManager.GetConfigValueInt(CCM_TYPE_CLOSESTATE));

    //写入配置文件
    g_ConfigManager.SetConfigValueInt(CCM_TYPE_CLOSESTATE, nValue);

    SetOptionState(IDC_OPT_CLOSE, IDC_OPT_TOTRAY);
}

void CControlDlg::SetDlgOptions()
{
    int bAutoStart = g_ConfigManager.GetConfigValueInt(CCM_TYPE_AUTOSTART);
    int bShowTray = g_ConfigManager.GetConfigValueInt(CCM_TYPE_SHOWTRAY);
    int bCloseState = g_ConfigManager.GetConfigValueInt(CCM_TYPE_CLOSESTATE);
    int bHideProcess = g_ConfigManager.GetConfigValueInt(CCM_TYPE_HIDEPROCESS);
//     wsprintf(szBuf, L"Auto = %d, ShowTray = %d, bHideProcess = %d, bCloseState = %d \r\n", bAutoStart, bHideProcess, bShowTray, bCloseState);
// 
//     OutputDebugString(szBuf);

    SetCheckState(IDC_CHK_AUTOSTART, (BOOL)bAutoStart);
    SetCheckState(IDC_CHK_SHOWTRAY, (BOOL)bShowTray);
    SetCheckState(IDC_CHK_HIDEPROCESS, (BOOL)bHideProcess);

    //如果有隐藏进程选项，设置进程为隐藏
    if (bHideProcess)
    {
        CString strCurProcessName = g_DriverManager.GetCurrentProcessName();
        g_DriverManager.SetProcessHiddenState(strCurProcessName, TRUE);
    }

    if (bCloseState == 0)
    {
       SetOptionState(IDC_OPT_TOTRAY, IDC_OPT_CLOSE);
    }
    else if (bCloseState == 1)
    {
        SetOptionState(IDC_OPT_CLOSE, IDC_OPT_TOTRAY);
    }

}


void CControlDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    
    //重绘窗口
    SetDlgOptions();
}


void CControlDlg::OnStnClickedBtnSave()
{
    CString strNewPassword;
    CString strEncodePassword;
    CString strPrompot;
    GetDlgItemText(IDC_EDIT_NEW_PWD, strNewPassword.GetBuffer(20), 20);
    strNewPassword.ReleaseBuffer();

    if (strNewPassword == "" || strNewPassword.GetLength() == 0)
    {
        MessageBox(L"密码不能为空哟~！^_^", L"错误", MB_OK|MB_ICONEXCLAMATION);
        return;
    }
    else
    {
        strPrompot.Format(L"新的启动密码： %s \r\n确定要更改为新的管理密码吗？", strNewPassword);
        if (MessageBox(strPrompot, L"提示", MB_YESNO|MB_ICONQUESTION) == IDYES)
        {
            strEncodePassword = g_ConfigManager.XORPassword(strNewPassword);

            g_ConfigManager.SetConfigValueString(CCM_TYPE_CONFIGPWD, strEncodePassword);

            SetDlgItemText(IDC_EDIT_NEW_PWD, L"");

            MessageBox(L"新的管理密码修改成功！", L"修改成功", MB_OK|MB_ICONINFORMATION);
        }
    }
}

void CControlDlg::SetAutoStart( BOOL bFlag )
{
    OSVERSIONINFO OsVerInfo;
    OsVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    GetVersionEx(&OsVerInfo);

    if (OsVerInfo.dwMajorVersion < 6) //XP System
    {
        SetXPAutoStart(bFlag); //XP下是注册表启动
    }
    else
    {
        SetWin7AutoStart(bFlag);//Win7下是计划任务启动
    }
}

void CControlDlg::SetWin7AutoStart( BOOL bFlag )
{
    TCHAR szCurrentPath[MAX_PATH];
    GetModuleFileName(NULL,szCurrentPath,MAX_PATH);

    TCHAR szCreateCmd[400] ={0};
    TCHAR szDeleteCmd[40] = L"/c SCHTASKS /DELETE /TN HideMyFile /F"; //删除计划任务

    if (bFlag)
    {
        //添加计划任务
        wsprintf(szCreateCmd,L"/c SCHTASKS /CREATE /TN HideMyFile /TR \"%s /autorun\" /SC ONLOGON /RU \"BUILTIN\\Administrators\" /RL HIGHEST",szCurrentPath);
        ShellExecute(NULL,L"OPEN",L"cmd.exe",szCreateCmd,NULL,SW_HIDE);
    }
    else
    {
        ShellExecute(NULL,L"OPEN",L"cmd.exe",szDeleteCmd,NULL,SW_HIDE);
    }
}

void CControlDlg::SetXPAutoStart( BOOL bFlag )
{
    TCHAR szCurrrentPath[MAX_PATH+1];
    GetModuleFileName(NULL,szCurrrentPath,MAX_PATH);
    wcscat_s(szCurrrentPath,L" /autorun");

    HKEY  hKey;
    DWORD cbData = MAX_PATH;

    if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER,L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",0,KEY_WRITE,&hKey))
    {
        MessageBox(L"打开注册表失败！",L"提示",MB_OK|MB_ICONERROR);
        RegCloseKey(hKey);
        return;
    }

    if (bFlag)
    {
        if (ERROR_SUCCESS !=RegSetValueEx(hKey,L"HideMyFile",0,REG_SZ,(CONST BYTE*)szCurrrentPath, cbData))
        {
            MessageBox(L"设置开机启动失败！",L"提示",MB_OK|MB_ICONERROR);
            RegCloseKey(hKey);
            return;
        }
    }
    else//取消开机启动
    {
        if (ERROR_SUCCESS !=RegDeleteValue(hKey,L"HideMyFile"))
        {
            MessageBox(L"取消开机启动失败！",L"提示",MB_OK|MB_ICONERROR);
            RegCloseKey(hKey);
            return;
        }
    }
    RegCloseKey(hKey);
}






BOOL CControlDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->wParam == VK_RETURN)
        {
            if (GetFocus() == GetDlgItem(IDC_EDIT_NEW_PWD))
            {
                OnStnClickedBtnSave();
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

