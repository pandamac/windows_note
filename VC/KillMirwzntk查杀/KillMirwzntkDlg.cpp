// KillMirwzntkDlg.cpp : implementation file
//

#include "stdafx.h"
#include "KillMirwzntk.h"
#include "KillMirwzntkDlg.h"
#include <TlHelp32.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKillMirwzntkDlg dialog

CKillMirwzntkDlg::CKillMirwzntkDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKillMirwzntkDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKillMirwzntkDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKillMirwzntkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKillMirwzntkDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CKillMirwzntkDlg, CDialog)
	//{{AFX_MSG_MAP(CKillMirwzntkDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_KILL, OnKill)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKillMirwzntkDlg message handlers

BOOL CKillMirwzntkDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CKillMirwzntkDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CKillMirwzntkDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CKillMirwzntkDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

// 查找指定进程
BOOL FindTargetProcess(char *pszProcessName,DWORD *dwPid)
{
    BOOL bFind = FALSE;
    
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        return bFind;
    }
    
    PROCESSENTRY32 pe = { 0 };
    pe.dwSize = sizeof(pe);
    
    BOOL bRet = Process32First(hProcessSnap,&pe);
    while (bRet)
    {
        if (lstrcmp(pe.szExeFile,pszProcessName) == 0)
        {
            *dwPid = pe.th32ProcessID;
            bFind = TRUE;
            break;
        }
        bRet = Process32Next(hProcessSnap,&pe);
    }

    CloseHandle(hProcessSnap);
    
    return bFind;
}

// 提升权限
BOOL EnableDebugPrivilege(char *pszPrivilege)
{
    HANDLE hToken = INVALID_HANDLE_VALUE;
    LUID luid;
    TOKEN_PRIVILEGES tp;

    BOOL bRet = OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,&hToken);
    if (bRet == FALSE)
    {
        return bRet;
    }

    bRet = LookupPrivilegeValue(NULL,pszPrivilege,&luid);
    if (bRet == FALSE)
    {
        return bRet;
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    bRet = AdjustTokenPrivileges(hToken,FALSE,&tp,sizeof(tp),NULL,NULL);

    return bRet;
}

#define EXE_VIRUS_NAME "mirwzntk.exe"
#define DLL_VIRUS_NAME "mirwznt.dll"

DWORD CRC32(BYTE* ptr,DWORD Size)
{
    
    DWORD crcTable[256],crcTmp1;
    
    //动态生成CRC-32表
    for (int i=0; i<256; i++)
    {
        crcTmp1 = i;
        for (int j=8; j>0; j--)
        {
            if (crcTmp1&1) crcTmp1 = (crcTmp1 >> 1) ^ 0xEDB88320L;
            else crcTmp1 >>= 1;
        }
        
        crcTable[i] = crcTmp1;
    }
    //计算CRC32值
    DWORD crcTmp2= 0xFFFFFFFF;
    while(Size--)
    {
        crcTmp2 = ((crcTmp2>>8) & 0x00FFFFFF) ^ crcTable[ (crcTmp2^(*ptr)) & 0xFF ];
        ptr++;
    }
    
    return (crcTmp2^0xFFFFFFFF);
}

void CKillMirwzntkDlg::OnKill() 
{
	// TODO: Add your control notification handler code here
    BOOL bRet = FALSE;
    DWORD dwPid = 0;
    CString csTxt;
    bRet = FindTargetProcess("mirwzntk.exe",&dwPid);
    if (bRet == TRUE)
    {
        csTxt = _T("检查系统内存...\r\n");
        csTxt += _T("系统中存在病毒进程:mirwzntk.exe\r\n");
        csTxt += _T("准备进行查杀...\r\n");
        SetDlgItemText(IDC_LIST,csTxt);
        bRet = EnableDebugPrivilege(SE_DEBUG_NAME);
        if (bRet == FALSE)
        {
            csTxt += _T("提升权限失败\r\n");
        }
        else
        {
            csTxt += _T("提升权限成功\r\n");
        }
        SetDlgItemText(IDC_LIST,csTxt);
        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwPid);
        if (hProcess == INVALID_HANDLE_VALUE)
        {
            csTxt += _T("无法结束进程\r\n");
            return ;
        }
        bRet = TerminateProcess(hProcess,0);
        if (bRet == FALSE)
        {
            csTxt += _T("无法结束进程\r\n");
            return ;
        }
        csTxt += _T("结束病毒进程\r\n");
        SetDlgItemText(IDC_LIST,csTxt);
        CloseHandle(hProcess);
    }
    else
    {
        csTxt += _T("系统中不存在mirwzntk.exe病毒进程\r\n");
    }

    Sleep(10);

    char szSysPath[MAX_PATH] = { 0 };
    GetSystemDirectory(szSysPath,MAX_PATH);
    
    lstrcat(szSysPath,"\\");
    lstrcat(szSysPath,EXE_VIRUS_NAME);

    csTxt += _T("检查硬盘文件...\r\n");

    if (GetFileAttributes(szSysPath) == 0xFFFFFFFF)
    {
        csTxt += _T("mirwzntk.exe病毒文件不存在\r\n");
    }
    else
    {
        csTxt += _T("mirwzntk.exe病毒文件存在正在进行计算校验和\r\n");

        HANDLE hFile = CreateFile(szSysPath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
        if (hFile == INVALID_HANDLE_VALUE)
        {
            MessageBox("Create Error");
            return ;
        }
        DWORD dwSize = GetFileSize(hFile,NULL);
        if (dwSize == 0xFFFFFFFF)
        {
            MessageBox("GetFileSize Error");
            return ;
        }
        BYTE *pFile = (BYTE*)malloc(dwSize);
        if (pFile == NULL)
        {
            MessageBox("malloc Error");
            return ;
        }
        
        DWORD dwNum = 0;
        ReadFile(hFile,pFile,dwSize,&dwNum,NULL);
        
        DWORD dwCrc32 = CRC32(pFile,dwSize);

        if (pFile != NULL)
        {
            free(pFile);
            pFile = NULL;
        }

        CloseHandle(hFile);

        if (dwCrc32 != 0x2A2F2D77)
        {
            csTxt += _T("校验和验证失败\r\n");
        }
        else
        {
            csTxt += _T("校验和验证成功，正在删除...\r\n");
            bRet = DeleteFile(szSysPath);
            if (bRet)
            {
                csTxt += _T("mirwzntk.exe病毒被删除\r\n");
            } 
            else
            {
                csTxt += _T("mirwzntk.exe病毒无法被删除\r\n");
            }
        }
    }
    
    SetDlgItemText(IDC_LIST,csTxt);

    Sleep(10);

    GetSystemDirectory(szSysPath,MAX_PATH);
    
    lstrcat(szSysPath,"\\");
    lstrcat(szSysPath,DLL_VIRUS_NAME);

    if (GetFileAttributes(szSysPath) == 0xFFFFFFFF)
    {
        csTxt += _T("mirwznt.dll病毒文件不存在\r\n");
    }
    else
    {
        csTxt += _T("mirwznt.dll病毒文件存在正在进行计算校验和\r\n");
        HANDLE hFile = CreateFile(szSysPath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
        if (hFile == INVALID_HANDLE_VALUE)
        {
            MessageBox("Create Error");
            return ;
        }
        DWORD dwSize = GetFileSize(hFile,NULL);
        if (dwSize == 0xFFFFFFFF)
        {
            MessageBox("GetFileSize Error");
            return ;
        }
        BYTE *pFile = (BYTE*)malloc(dwSize);
        if (pFile == NULL)
        {
            MessageBox("malloc Error");
            return ;
        }
        
        DWORD dwNum = 0;
        ReadFile(hFile,pFile,dwSize,&dwNum,NULL);
        
        DWORD dwCrc32 = CRC32(pFile,dwSize);
        
        if (pFile != NULL)
        {
            free(pFile);
            pFile = NULL;
        }

        CloseHandle(hFile);

        if (dwCrc32 != 0x2D0F20FF)
        {
            csTxt += _T("校验和验证失败\r\n");
        }
        else
        {
            csTxt += _T("校验和验证成功，正在删除...\r\n");    
            bRet = DeleteFile(szSysPath);
            if (bRet)
            {
                csTxt += _T("mirwznt.dll病毒被删除\r\n");
            } 
            else
            {
                csTxt += _T("mirwznt.dll病毒无法被删除\r\n");
            }
        }
    }

    Sleep(10);

    csTxt += _T("正在检查注册表...\r\n");
    SetDlgItemText(IDC_LIST,csTxt);

    HKEY hKey;
    char cData[MAXBYTE] = { 0 };
    LONG lSize = MAXBYTE;
    long lRet = RegOpenKey(HKEY_LOCAL_MACHINE,
        "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows",
        &hKey);

    if(lRet == ERROR_SUCCESS)
    {
        lRet = RegQueryValueEx(hKey,"AppInit_DLLs",NULL,NULL,(unsigned char *)cData,(unsigned long *)&lSize);
        if ( lRet == ERROR_SUCCESS)
        {
            if (lstrcmp(cData,"mirwznt.dll") == 0)
            {
                csTxt += _T("注册表项中存在病毒信息\r\n");
            }

            lRet = RegDeleteValue(hKey,"AppInit_DLLs");
            if (lRet == ERROR_SUCCESS)
            {
                csTxt += _T("注册表项中的病毒信息已删除\r\n");
            }
            else
            {
                csTxt += _T("注册表项中的病毒信息无法删除\r\n");
            }
        }
        else
        {
            csTxt += _T("注册表存项中不存在病毒信息\r\n");
        }
        RegCloseKey(hKey);
    } 
    else
    {
        csTxt += _T("注册表信息读取失败\r\n");
    }
    csTxt += _T("病毒检测完成，请使用专业杀毒软件进行全面扫描\r\n");
    SetDlgItemText(IDC_LIST,csTxt);
}

