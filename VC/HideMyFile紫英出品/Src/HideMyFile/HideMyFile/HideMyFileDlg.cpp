
// HideMyFileDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HideMyFile.h"
#include "HideMyFileDlg.h"
#include "afxdialogex.h"

#include "ConfigManager.h"
#include "FileManager.h"
#include "DriverManager.h"
#include "LoginDlg.h"


CConfigManager g_ConfigManager;
CFileManager g_FileManager;
CDriverManager g_DriverManager;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CHideMyFileDlg 对话框




CHideMyFileDlg::CHideMyFileDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CHideMyFileDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_MAIN_ICON);
    m_bIsVisable = TRUE;
    m_IsShowLoginDlg = FALSE;
}

void CHideMyFileDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BTN_CONTROL, m_BtnControl);
    DDX_Control(pDX, IDC_BTN_ADD, m_BtnAdd);
    DDX_Control(pDX, IDC_BTN_MIN, m_BtnMinimize);
    DDX_Control(pDX, IDC_BTN_CLOSE, m_BtnClose);
    DDX_Control(pDX, IDC_FOLDER_PATH, m_FilePath);
    DDX_Control(pDX, IDC_BTN_SEARCH, m_BtnSearch);
}

BEGIN_MESSAGE_MAP(CHideMyFileDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_WM_NCHITTEST()
    ON_WM_CTLCOLOR()
    ON_MESSAGE(WM_NOTIFYICON, OnNotifyIcon)
    ON_STN_CLICKED(IDC_BTN_CLOSE, &CHideMyFileDlg::OnStnClickedBtnClose)
    ON_STN_CLICKED(IDC_BTN_MIN, &CHideMyFileDlg::OnStnClickedBtnMin)
    ON_STN_CLICKED(IDC_BTN_CONTROL, &CHideMyFileDlg::OnStnClickedBtnControl)
    ON_STN_CLICKED(IDC_BTN_ADD, &CHideMyFileDlg::OnStnClickedBtnAdd)
    ON_WM_WINDOWPOSCHANGING()
    ON_COMMAND(IDM_EXIT, &CHideMyFileDlg::OnExit)
    ON_NOTIFY(NM_RCLICK, IDC_FOLDER_LIST, &CHideMyFileDlg::OnNMRClickFolderList)
    ON_COMMAND(IDM_HIDE_FILE, &CHideMyFileDlg::OnHideFile)
    ON_COMMAND(IDM_SHOW_FILE, &CHideMyFileDlg::OnShowFile)
    ON_COMMAND(IDM_CANCEL_HIDE, &CHideMyFileDlg::OnCancelHide)
    ON_COMMAND(IDM_REFRESH_LIST, &CHideMyFileDlg::OnRefreshList)
    ON_COMMAND(IDM_EXPLOER_FILE, &CHideMyFileDlg::OnExploerFile)
    ON_STN_CLICKED(IDC_BTN_SEARCH, &CHideMyFileDlg::OnStnClickedBtnSearch)
END_MESSAGE_MAP()


// CHideMyFileDlg 消息处理程序

BOOL CHideMyFileDlg::OnInitDialog()
{
    //单实例运行
    CheckSingleton();

	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

    EnableDebugPrv();

    //初始化控件
    InitializeControls();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CHideMyFileDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CHideMyFileDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CHideMyFileDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



LRESULT CHideMyFileDlg::OnNcHitTest(CPoint point)
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


HBRUSH CHideMyFileDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void CHideMyFileDlg::InitializeControls()
{
    //初始化组件
    g_ConfigManager.Initialize();
    g_DriverManager.Initialize();
    g_FileManager.Initialize();

    //程序参数

    CString strCmdLine = L"";
    strCmdLine.Format(L"%s",AfxGetApp()->m_lpCmdLine);//获取启动参数

    if (strCmdLine == L"/autorun")//如果是自启动参数,隐藏窗口
    {
        m_bIsVisable = FALSE;
        ShowWindow(SW_HIDE);
    }
    else
    {
        if (!CheckLogin()) //管理认证
        {
            m_bIsVisable = FALSE;
            ShowWindow(SW_HIDE);
        }
    }

    //加载驱动
    EnableDebugPrv();
    g_DriverManager.InstallDriver();
    g_DriverManager.StartDriverService();


    //添加到托盘
    if (g_ConfigManager.GetConfigValueInt(CCM_TYPE_AUTOSTART) == 1)
    {
        if (g_ConfigManager.GetConfigValueInt(CCM_TYPE_SHOWTRAY) == 1)
        {
            AddToTrayIcon();
        }
    }


    //进程隐藏
    if (g_ConfigManager.GetConfigValueInt(CCM_TYPE_HIDEPROCESS) == 1)
    {
        CString strCurProcessName = g_DriverManager.GetCurrentProcessName();
        g_DriverManager.SetProcessHiddenState(strCurProcessName, TRUE);
    }

    //设置按钮显示图像
    m_BtnClose.SetButtonImages(IDB_CLOSE_NORMAL, IDB_CLOSE_ACTIVE);
    m_BtnMinimize.SetButtonImages(IDB_MIN_NORMAL, IDB_MIN_ACTIVE);
    m_BtnControl.SetButtonImages(IDB_CONTROL_NORMAL, IDB_CONTROL_ACTIVE);
    m_BtnAdd.SetButtonImages(IDB_BTN_ADD_NORMAL, IDB_BTN_ADD_HOVER);
    m_BtnSearch.SetButtonImages(IDB_SEARCH_NORMAL, IDB_SEARCH_ACTIVE);

    //对话框背景
    CBitmap bitmapBackgroud;
    bitmapBackgroud.LoadBitmapW(IDB_BACKGROUND);
    m_BkBrush.CreatePatternBrush(&bitmapBackgroud);
    bitmapBackgroud.DeleteObject();

    //路径框字体
    m_EditFont.CreateFont(24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, L"Microsoft YaHei");

    CEdit *pEdit = (CEdit*)GetDlgItem(IDC_FOLDER_PATH);
    pEdit->SetFont(&m_EditFont);
    SetDlgItemText(IDC_FOLDER_PATH,L"请选择要隐藏的文件或文件夹...");
    pEdit->SetSel(-1,0);

    //文件夹列表框

    //字体
    m_ListFont.CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                                    CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, L"Microsoft YaHei");

    CListCtrl *pFileList = (CListCtrl *)GetDlgItem(IDC_FOLDER_LIST);
    pFileList->SetFont(&m_ListFont);

    //列头
    TCHAR tItemList[4][10] = {L"序号",L"文件名称",L"文件路径",L"当前状态"};
    int iListSize[4]={60,170,300,60};
    LV_COLUMN lvcolumn;
    pFileList->SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
    for(int i=0;i<4;i++)
    {
        lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH | LVCF_ORDER;
        lvcolumn.fmt = LVCFMT_LEFT;
        lvcolumn.pszText = tItemList[i];
        lvcolumn.iSubItem = i;
        lvcolumn.iOrder = i;
        lvcolumn.cx = iListSize[i];
        pFileList->InsertColumn(i, &lvcolumn);
    }	
    
    //注册系统热键  Ctrl + Shift + 'T'
    RegisterHotKey(GetSafeHwnd(), WM_HIDEHOTKEY, MOD_CONTROL|MOD_SHIFT, 'T');


    //读取文件列表
    ReadFileList();

}


void CHideMyFileDlg::OnStnClickedBtnClose()
{
    int nValue = g_ConfigManager.GetConfigValueInt(CCM_TYPE_CLOSESTATE);
    if (nValue == 0)
    {
        if (g_ConfigManager.GetConfigValueInt(CCM_TYPE_SHOWTRAY) == 1)
        {
            AddToTrayIcon();
        }
        ShowWindow(SW_HIDE);
    }
    else if (nValue == 1)
    {
        if (CheckLogin())//管理认证
        {
            OnOK();
        }
    }
}


void CHideMyFileDlg::OnStnClickedBtnMin()
{
    ShowWindow(SW_MINIMIZE);
}


void CHideMyFileDlg::OnStnClickedBtnControl()
{
    m_ControlDlg = new CControlDlg();
    m_ControlDlg->Create(IDD_DLG_CONTROL, NULL);
    m_ControlDlg->ShowWindow(SW_SHOW);
}

void CHideMyFileDlg::OnStnClickedBtnSearch()
{
    TCHAR szBuffer[MAX_PATH] = {0};
    BROWSEINFO bInfo;
    bInfo.hwndOwner = this->GetSafeHwnd();
    bInfo.pidlRoot = NULL;
    bInfo.pszDisplayName = szBuffer;
    bInfo.lpszTitle = L"选择要隐藏的文件或文件夹";
    bInfo.ulFlags = BIF_BROWSEINCLUDEFILES;
    bInfo.lpfn = NULL;
    bInfo.lParam = NULL;
    bInfo.iImage = NULL;

    ITEMIDLIST *idl;
    CoInitialize(NULL);
    idl = SHBrowseForFolder(&bInfo);
    if (idl)
    {
        TCHAR szPath[MAX_PATH] = {0};
        SHGetPathFromIDList(idl, szPath);
        SetDlgItemText(IDC_FOLDER_PATH, szPath);

        IMalloc *im;
        SHGetMalloc(&im);
        im->Free(idl);
    }
    CoUninitialize();
}

void CHideMyFileDlg::OnStnClickedBtnAdd()
{
    CString strFilePath;

    do 
    {
        //读取文件路径
        GetDlgItemText(IDC_FOLDER_PATH, strFilePath.GetBuffer(MAX_PATH), MAX_PATH);
        strFilePath.ReleaseBuffer();

        if (PathFileExists(strFilePath) && strFilePath.GetLength() > 3)
        {
            if (CheckExistFileInList(strFilePath))
            {
                MessageBox(L"文件已经已经在列表中了哟~~！换一个吧！^_^", L"提示", MB_OK|MB_ICONINFORMATION);
                break;
            }
            //添加文件项目
            g_FileManager.AddFileItem(strFilePath);

            //刷新列表
            OnRefreshList();
        }
        else
        {
            MessageBox(L"文件路径貌似不对吧，再试试看？！^_^", L"出错啦！", MB_OK|MB_ICONINFORMATION);
        }
    } while (0);
    
    SetDlgItemText(IDC_FOLDER_PATH,L"请选择要隐藏的文件或文件夹...");
}

LRESULT CHideMyFileDlg::OnNotifyIcon( WPARAM wParam, LPARAM lParam )
{
    if (lParam == WM_LBUTTONDBLCLK)
    {
        if (!CheckLogin())
        {
            m_bIsVisable = FALSE;
            ShowWindow(SW_HIDE);
        }
        else
        {
            m_bIsVisable = TRUE;
            ShowWindow(SW_SHOW);
            CenterWindow();
        }
    }
    else if(lParam == WM_RBUTTONDOWN)
    {
        CMenu menu;
        menu.LoadMenuW(IDR_TRAY_MENU);
        CMenu *pMenu = menu.GetSubMenu(0);

        CPoint pt;
        GetCursorPos(&pt);
        SetForegroundWindow();//点击空白，菜单消失

        pMenu->TrackPopupMenu(TPM_RIGHTALIGN|TPM_RIGHTBUTTON, pt.x, pt.y, this, 0);//显示菜单
    }
    return TRUE;
}

void CHideMyFileDlg::AddToTrayIcon()
{
    m_ntIcon.cbSize = sizeof(NOTIFYICONDATA);
    m_ntIcon.hIcon = AfxGetApp()->LoadIconW(IDI_MAIN_ICON);
    m_ntIcon.hWnd = m_hWnd;
    m_ntIcon.uCallbackMessage = WM_NOTIFYICON;
    m_ntIcon.uFlags = NIF_MESSAGE|NIF_ICON|NIF_TIP;
    TCHAR tTips[128]=L"小白专属工具~By 小河豚(^-^)";
    wcscpy_s(m_ntIcon.szTip,128*sizeof(TCHAR),tTips);

    ::Shell_NotifyIcon(NIM_ADD,&m_ntIcon);
}


void CHideMyFileDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
    if (!m_bIsVisable)
    {
        lpwndpos->flags &= ~SWP_SHOWWINDOW; 
    }

    CDialogEx::OnWindowPosChanging(lpwndpos);
}

void CHideMyFileDlg::OnExit()
{
    if (CheckLogin())
    {
        OnOK();
    }
}


void CHideMyFileDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类

    //解除热键
    UnregisterHotKey(GetSafeHwnd(), WM_HIDEHOTKEY);

    //删除托盘图标
    Shell_NotifyIcon(NIM_DELETE,&m_ntIcon);

    //卸载驱动
    g_DriverManager.StopDriverService();
    g_DriverManager.UninstallDriver();

    CDialogEx::OnOK();
}

void CHideMyFileDlg::ReadFileList()
{
    CListCtrl *pListCtrl = (CListCtrl*)GetDlgItem(IDC_FOLDER_LIST);
    int nItemCount = g_FileManager.GetFileItemCount();
    CString strID;
    CString strFileName;
    CString strFilePath;
    int  nHidden;

    g_DriverManager.ClearFileCount();

    int nIdx = 0;
    for (nIdx = 0; nIdx < nItemCount; ++nIdx)
    {
       strID.Format(L"%d", g_FileManager.GetItemFileId(nIdx));
       strFileName = g_FileManager.GetItemFileName(nIdx);
       strFilePath = g_FileManager.GetItemFilePath(nIdx);
       nHidden = g_FileManager.GetItemFileState(nIdx);

       pListCtrl->InsertItem(nIdx, strID);
       pListCtrl->SetItemText(nIdx, 1, strFileName);
       pListCtrl->SetItemText(nIdx, 2, strFilePath);
       if (nHidden == 1)
       {
            pListCtrl->SetItemText(nIdx, 3, L"隐藏");
       }
       else if (nHidden == 0)
       {
            pListCtrl->SetItemText(nIdx, 3, L"显示");
       }

       g_DriverManager.AddNewHiddenFile(strFileName);
       g_DriverManager.SetFileDriverHiddenState(strFileName, (BOOL)nHidden);
    }
}


void CHideMyFileDlg::OnNMRClickFolderList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    
    CMenu m_PopMenu;
    m_PopMenu.LoadMenuW(IDR_MAIN_MENU);
    CMenu *pMenu = m_PopMenu.GetSubMenu(0);
    
    CString strHidden;
    int nPos = pNMItemActivate->iItem;
    if (nPos != -1)
    {
        CListCtrl *pListCtrl = (CListCtrl*)GetDlgItem(IDC_FOLDER_LIST);
        pListCtrl->GetItemText(nPos, 3, strHidden.GetBuffer(20), 20);
        strHidden.ReleaseBuffer();
        if (strHidden == L"隐藏")
        {
            pMenu->EnableMenuItem(IDM_HIDE_FILE, MF_BYCOMMAND|MF_GRAYED);
        }
        else if (strHidden == L"显示")
        {
             pMenu->EnableMenuItem(IDM_SHOW_FILE, MF_BYCOMMAND|MF_GRAYED);
        }

    }
    else if (nPos == -1)
    {
        pMenu->EnableMenuItem(IDM_HIDE_FILE, MF_BYCOMMAND|MF_GRAYED);
        pMenu->EnableMenuItem(IDM_SHOW_FILE, MF_BYCOMMAND|MF_GRAYED);
        pMenu->EnableMenuItem(IDM_CANCEL_HIDE, MF_BYCOMMAND|MF_GRAYED);
        pMenu->EnableMenuItem(IDM_EXPLOER_FILE, MF_BYCOMMAND|MF_GRAYED);
    }

    CPoint pt;
    GetCursorPos(&pt);

    pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, pt.x, pt.y, this, 0);

    *pResult = 0;
}


void CHideMyFileDlg::OnHideFile()
{
    //获取列表索引和文件项目索引
    int nIndex = GetSelectItemIndex();
    int nID  = GetFileItemID(nIndex);

     //更新列表中的显示
    CListCtrl *pListCtrl = (CListCtrl*)GetDlgItem(IDC_FOLDER_LIST);

    g_FileManager.SetFileListHiddenState(nID, TRUE);
    pListCtrl->SetItemText(nIndex, 3, L"隐藏");

     //取文件名， 通知驱动显示/隐藏文件
    CString strFileName;
    pListCtrl->GetItemText(nIndex, 1, strFileName.GetBuffer(MAX_PATH), MAX_PATH);
    strFileName.ReleaseBuffer();


    g_DriverManager.SetFileDriverHiddenState(strFileName, TRUE);

}


void CHideMyFileDlg::OnShowFile()
{
    //获取列表索引和文件项目索引
   int nIndex = GetSelectItemIndex();
   int nID  = GetFileItemID(nIndex);

   //更新列表中的显示
   CListCtrl *pListCtrl = (CListCtrl*)GetDlgItem(IDC_FOLDER_LIST);

    g_FileManager.SetFileListHiddenState(nID, FALSE);
    pListCtrl->SetItemText(nIndex, 3, L"显示");

    //取文件名， 通知驱动显示/隐藏文件
    CString strFileName;
    pListCtrl->GetItemText(nIndex, 1, strFileName.GetBuffer(MAX_PATH), MAX_PATH);
    strFileName.ReleaseBuffer();
    
    g_DriverManager.SetFileDriverHiddenState(strFileName, FALSE);
}


void CHideMyFileDlg::OnCancelHide()
{
    //获取列表索引和文件项目索引
    int nIndex = GetSelectItemIndex();
    int nID  = GetFileItemID(nIndex);
   
    //删除文件项目
    g_FileManager.DeleteFileItem(nID);

    //刷新列表
    OnRefreshList();
}


void CHideMyFileDlg::OnRefreshList()
{
    //清空列表
    CListCtrl *pListCtrl = (CListCtrl*)GetDlgItem(IDC_FOLDER_LIST);
    pListCtrl->DeleteAllItems();

    //重新读取列表
    ReadFileList();
}

int CHideMyFileDlg::GetSelectItemIndex()
{
    int nIndex;
    TCHAR szBuf[20] = {0};
    CListCtrl *pListCtrl = (CListCtrl*)GetDlgItem(IDC_FOLDER_LIST);
    for (nIndex = 0; nIndex < pListCtrl->GetItemCount(); ++nIndex)
    {
        if (pListCtrl->GetItemState(nIndex, LVIS_SELECTED) == LVIS_SELECTED)
        {
            return nIndex;
        }
    }

    return -1;
}

int CHideMyFileDlg::GetFileItemID( int nIndex )
{
    int nID;
    TCHAR szBuf[20] = {0};
    CListCtrl *pListCtrl = (CListCtrl*)GetDlgItem(IDC_FOLDER_LIST);

    if (nIndex != -1)
    {
        pListCtrl->GetItemText(nIndex, 0, szBuf, 20);
        nID = _wtoi(szBuf);

        return nID;
    }

    return 0;
}


void CHideMyFileDlg::OnExploerFile()
{
    //获取列表索引和文件项目索引
    int nIndex = GetSelectItemIndex();
    CListCtrl *pListCtrl = (CListCtrl*)GetDlgItem(IDC_FOLDER_LIST);

    //取文件路径
    CString strFilePath;
    pListCtrl->GetItemText(nIndex, 2, strFilePath.GetBuffer(MAX_PATH), MAX_PATH);
    strFilePath.ReleaseBuffer();

    CString strCmd;
    if (PathIsDirectory(strFilePath))
    {
        strCmd.Format(L"/c start %s", strFilePath);
        ShellExecute(NULL,L"OPEN",L"cmd.exe",strCmd,NULL,SW_HIDE);
    }
    else
    {
        strCmd.Format(L"/e,/select,%s", strFilePath);
        ShellExecute(NULL,L"OPEN",L"explorer.exe",strCmd,NULL,SW_SHOW);
    }

}

BOOL CHideMyFileDlg::EnableDebugPrv()
{
    HANDLE hToken;
    LUID SeDebugNameValue;
    TOKEN_PRIVILEGES tkp;

    if(!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken))//打开进程令牌
    {
        //cout<<"Open Token Failed!"<<endl;
        return FALSE;
    }
    if(!LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&SeDebugNameValue))//查找权限信息
    {
        CloseHandle(hToken);
        //cout<<"Look up Privilege Failed!"<<endl;
        return FALSE;
    }
    tkp.PrivilegeCount = 1;//权限数量
    tkp.Privileges[0].Luid = SeDebugNameValue;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if (!AdjustTokenPrivileges(hToken,FALSE,&tkp,sizeof(tkp),NULL,NULL))//提升权限
    {
        CloseHandle(hToken);
        //cout<<"Adjust Token Privileges Failed!"<<endl;
        return FALSE;
    }

    OutputDebugString(L"Done!\n");
    return TRUE;
}

LRESULT CHideMyFileDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (message == WM_HOTKEY)
    {
        if (wParam == WM_HIDEHOTKEY)
        {
            if (!CheckLogin())
            {
                m_bIsVisable = FALSE;
                ShowWindow(SW_HIDE);
            }
            else
            {
                m_bIsVisable = TRUE;
                SetForegroundWindow();
                ShowWindow(SW_SHOW);
                CenterWindow();
            }
        }
    }

    return CDialogEx::DefWindowProc(message, wParam, lParam);
}

BOOL CHideMyFileDlg::CheckLogin()
{
    if (m_IsShowLoginDlg == FALSE)
    {
        m_IsShowLoginDlg = TRUE;
        CLoginDlg dlg;

        if (dlg.DoModal() ==  IDOK)
        {
            m_IsShowLoginDlg = FALSE;
            return TRUE;
        }
        else
        {
            m_IsShowLoginDlg = FALSE;
        }
    }
    
    return FALSE;
}

void CHideMyFileDlg::CheckSingleton()
{
    HANDLE hObject  = CreateMutex(NULL,FALSE,L"HideMyFileSingleton");
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        CloseHandle(hObject);
        MessageBox(L"程序已经运行！\r\n请使用热键 Ctrl + Shift + T 来呼出程序！", L"提示", MB_OK|MB_ICONINFORMATION);
        ExitProcess(0);
    }
}

BOOL CHideMyFileDlg::CheckExistFileInList(CString strFilePath)
{
    CString strPath;
    CListCtrl *pListCtrl = (CListCtrl*)GetDlgItem(IDC_FOLDER_LIST);
    int nIdx = 0;
    int nCount = pListCtrl->GetItemCount();

    for (nIdx = 0; nIdx < nCount; ++nIdx)
    {
        pListCtrl->GetItemText(nIdx, 2, strPath.GetBuffer(100), 100);
        strPath.ReleaseBuffer();

        if (strFilePath == strPath) //文件列表中已经存在
        {
            return TRUE;
        }
    }

    return FALSE;
}


BOOL CHideMyFileDlg::PreTranslateMessage(MSG* pMsg)
{
   if (pMsg->message == WM_KEYDOWN) //屏蔽ESC和回车
   {
       if (pMsg->wParam == VK_ESCAPE ||
           pMsg->wParam == VK_RETURN)
       {
           return TRUE;
       }
   }
    return CDialogEx::PreTranslateMessage(pMsg);
}
