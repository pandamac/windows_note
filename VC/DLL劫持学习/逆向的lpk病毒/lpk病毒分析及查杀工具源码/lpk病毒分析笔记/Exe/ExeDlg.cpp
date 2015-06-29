// ExeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Exe.h"
#include "ExeDlg.h"
#include <tlhelp32.h>
#include "md5.h"
#include <Shlwapi.h>
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
// CExeDlg dialog

CExeDlg::CExeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExeDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CExeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExeDlg)
	DDX_Control(pDX, BTN_FREECONTROLSOURCE, m_btnUnControl);
	DDX_Control(pDX, BTN_CONTROLSOURCE, m_btnControl);
	DDX_Control(pDX, IDC_FAILLIST, m_FailList);
	DDX_Control(pDX, IDC_OKLIST, m_OKList);
	DDX_Control(pDX, BTN_DELETELPK, m_BtnDeleteLpk);
	DDX_Control(pDX, ID_SCANFILENAME, m_CurScanFileMsg);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CExeDlg, CDialog)
	//{{AFX_MSG_MAP(CExeDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(BTN_CONTROLSOURCE, OnControlsource)
	ON_BN_CLICKED(BTN_FREECONTROLSOURCE, OnFreecontrolsource)
	ON_BN_CLICKED(BTN_DELETELPK, OnDeletelpk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExeDlg message handlers

BOOL CExeDlg::OnInitDialog()
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

void CExeDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CExeDlg::OnPaint() 
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
HCURSOR CExeDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL EnableDebugPrivilege(BOOL bEnableDebugPrivilege)
{ 
    HANDLE hToken;
    TOKEN_PRIVILEGES tp;
    LUID luid;
	
    if(!::OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
    {
        return FALSE;
    }
	
    if(!::LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
    {
        ::CloseHandle(hToken);
        return FALSE;
    }
	
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    if(bEnableDebugPrivilege)
    {
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    }
    else
    {
        tp.Privileges[0].Attributes = 0;
    }
	
    if(!::AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL))
    {
        ::CloseHandle(hToken);
        return FALSE;
    }
	
    ::CloseHandle(hToken);
	
    if(::GetLastError() == ERROR_NOT_ALL_ASSIGNED)
    {
        return FALSE;
    }
    return TRUE;
}

BOOL GetAppPath(CString& strPathName) {
	
	CString strAppPath;  // 保存结果
	TCHAR szModuleFileName[MAX_PATH]; // 全路径名
	TCHAR drive[_MAX_DRIVE];  // 盘符名称，比如说C盘啊，D盘啊
	TCHAR dir[_MAX_DIR]; // 目录
	TCHAR fname[_MAX_FNAME];  // 进程名字
	TCHAR ext[_MAX_EXT]; //后缀，一般为exe或者是dll
	

	//获得当前进程的文件路径
	if (NULL == GetModuleFileName(NULL, szModuleFileName, MAX_PATH)) 
		return FALSE;

	//分割该路径，得到盘符，目录，文件名，后缀名
	_splitpath( szModuleFileName, drive, dir, fname, ext );  

	strAppPath = drive;
	strAppPath += dir;
	strPathName = strAppPath;

	return TRUE;
}


void CExeDlg::OnControlsource() 
{
	// TODO: Add your control notification handler code here
	m_btnControl.EnableWindow(FALSE);
	HANDLE hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	PROCESSENTRY32 PE = {sizeof(PROCESSENTRY32)};
	CString strDestDllAppPath;
	CString Msg;
	int dwHookProCount = 0;
	
	m_DestInfo.RemoveAll();
	EnableDebugPrivilege(TRUE);

	if(FALSE == GetAppPath(strDestDllAppPath) ||
	   FALSE == Process32First(hSnapshot, &PE))
	{
		goto EXIT_FREE;
	}
	
	strDestDllAppPath += "HookApi.dll";
	do{
		BOOL IsOk;
		
		if (PE.th32ProcessID == GetCurrentProcessId())
			continue;

		HANDLE hDestPro = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PE.th32ProcessID);
		if (NULL == hDestPro)	
			continue;

		//分配要注入的dll名称
		LPVOID lpDestAddr = VirtualAllocEx(hDestPro, 
											NULL, 
											0x1000, 
											MEM_COMMIT, 
											PAGE_EXECUTE_READWRITE);
		if (NULL == lpDestAddr){
			CloseHandle(hDestPro);
			continue;
		}
		
		//将要注要的dll路径名称写入到目标进程
		DWORD dwWriteCount = 0;
		IsOk = WriteProcessMemory(hDestPro, 
							lpDestAddr, 
							strDestDllAppPath.GetBuffer(0), 
							strDestDllAppPath.GetLength(),
							&dwWriteCount);
		
		if (FALSE == IsOk){
			VirtualFreeEx(hDestPro, lpDestAddr, 0x1000, MEM_DECOMMIT);
			CloseHandle(hDestPro);
			continue;
		}
		
		//获取LoadLibrary地址
		LPTHREAD_START_ROUTINE lpLoadLibrary;
		HMODULE hKernel = GetModuleHandle("Kernel32.dll");
		lpLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel, "LoadLibraryA");

		//注入dll
		HANDLE hDestThread = CreateRemoteThread(hDestPro,
												NULL,
												NULL,
												lpLoadLibrary,
												lpDestAddr,
												0,
												0);
		if (NULL == hDestThread)
		{
			VirtualFreeEx(hDestPro, lpDestAddr, 0x1000, MEM_DECOMMIT);
			CloseHandle(hDestPro);
			continue;
		}

		WaitForSingleObject(hDestThread,  INFINITE);

		DWORD dwhModule = 0;

		GetExitCodeThread(hDestThread, &dwhModule);
		if (NULL != dwhModule) 
		{
			dwHookProCount++;
			//保存信息,以备释放
			DESTMODULEINFO DestInfo = {0};
			DestInfo.dwProcessID = PE.th32ProcessID;
			DestInfo.hModule = (HMODULE)dwhModule;

			m_DestInfo.AddTail(DestInfo);
		}

		VirtualFreeEx(hDestPro, lpDestAddr, 0x1000, MEM_DECOMMIT);
		CloseHandle(hDestThread);
		CloseHandle(hDestPro);

	}while(FALSE != Process32Next(hSnapshot, &PE));
	
	Msg.Format("成功监控进程数: %d list元素个数:%d",dwHookProCount, m_DestInfo.GetCount());
	SetWindowText(Msg);

EXIT_FREE:

	EnableDebugPrivilege(FALSE);
	if (INVALID_HANDLE_VALUE != hSnapshot)
	{
		CloseHandle(hSnapshot);
		hSnapshot = INVALID_HANDLE_VALUE;
	}

}

DWORD _stdcall SanfLpkFile(LPVOID lpThis);

void CExeDlg::OnFreecontrolsource() 
{
	CString Msg;
	DWORD dwUnHookProCount = 0;
	
	EnableDebugPrivilege(TRUE);
	//获取 FreeLibrary 地址 
	LPTHREAD_START_ROUTINE lpFreeLibrary;
	HMODULE hKernel = GetModuleHandle("Kernel32.dll");
	lpFreeLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel, "FreeLibrary");
	if (NULL == lpFreeLibrary) 
		return ;

	POSITION pos = m_DestInfo.GetHeadPosition();
	DESTMODULEINFO DestInfo = {0};

	while ( NULL != pos)
	{
		DestInfo = m_DestInfo.GetNext(pos);
		HANDLE hDestPro = OpenProcess(PROCESS_ALL_ACCESS, FALSE, DestInfo.dwProcessID);
		if (NULL == hDestPro)	
			continue;

		//释放dll
		HANDLE hDestThread = CreateRemoteThread(hDestPro,
			NULL,
			NULL,
			lpFreeLibrary,
			(LPVOID)DestInfo.hModule,
			0,
			0);

		if (NULL == hDestThread)
		{
			CloseHandle(hDestPro);
			continue;
		}
		
		WaitForSingleObject(hDestThread,  INFINITE);
		
		DWORD dwThreadRet = 0;
		GetExitCodeThread(hDestThread, &dwThreadRet);
		if (NULL != dwThreadRet) dwUnHookProCount++;
		
		CloseHandle(hDestThread);
		CloseHandle(hDestPro);
	}
	
	Msg.Format("共成功Unhook进程数: %d",dwUnHookProCount);
	SetWindowText(Msg);

	EnableDebugPrivilege(FALSE);
	m_btnControl.EnableWindow(TRUE);
}

void CExeDlg::OnDeletelpk() 
{
	// TODO: Add your control notification handler code here
	 m_BtnDeleteLpk.EnableWindow(FALSE);

	HANDLE hThread = CreateThread(0,
								 0,
								 (LPTHREAD_START_ROUTINE)SanfLpkFile,
								 (LPVOID)this,
								 0,
								 0);
	
}

void CExeDlg::FindLpk(CString strCurPath)
{
	WIN32_FIND_DATA wfd = {0};
	CString lpPath;

	lpPath = strCurPath + "*.*";


	HANDLE hFind = FindFirstFile(lpPath, &wfd);
	if (INVALID_HANDLE_VALUE == hFind) 
		return;

	do 
	{
		if ( 0 == strcmp(".", wfd.cFileName)|| 0 == strcmp("..", wfd.cFileName))
			goto NEXT;

		if (strcmp("aa", wfd.cFileName) == 0)
		{
			int i = 0;
		}

		//注意这里是标志,别用== 比较
		if (FILE_ATTRIBUTE_DIRECTORY & wfd.dwFileAttributes)
		{
			FindLpk(strCurPath+wfd.cFileName +"\\");
		}
		else
		{
			CString strFileName = strCurPath+wfd.cFileName;
			CString strFilterFile = wfd.cFileName;
			strFilterFile.MakeLower();

			if (strFilterFile.Right(3) == "dll" ||
				strFilterFile.Right(3) == "exe")
			{
				m_CurScanFileMsg.SetWindowText("正在验证特征" + strFileName);

				HANDLE hFile; 
				char FileBuf[1000];
				char* lpMD5Value = NULL;
				
				//文件处理
				RtlZeroMemory(FileBuf, 1000);
				//m_CurScanFileMsg.SetWindowText(strFileName);
				hFile = CreateFile(strFileName,      
									GENERIC_READ,              // open for reading 
									FILE_SHARE_READ,           // share for reading 
									NULL,                      // no security 
									OPEN_EXISTING,             // existing file only 
									FILE_ATTRIBUTE_NORMAL,     // normal file 
									NULL);                     // no attr. template 
				
				DWORD dwRet = SetFilePointer(hFile, 0x400, 0, FILE_BEGIN);
				BOOL IsOk = ReadFile(hFile, FileBuf, 1000, &dwRet, NULL);
				CloseHandle(hFile);
				
				if (IsOk)	//读取成功
				{
					//母体文件0400处开始1000字节md5值为: 8b4f20968d1947e8d7c7c62d52c6a373
					//判断是否为母体
					if (IsOk && 
						(MD5Check("8b4f20968d1947e8d7c7c62d52c6a373", FileBuf, 1000) ||
						 MD5Check("f540fd4de29072b9fd021c8b377e5989", FileBuf, 1000))
					   )
					{
						//删除病毒文件
						if (SetFileAttributes(strFileName, FILE_ATTRIBUTE_NORMAL) && DeleteFile(strFileName))
						{
							//添加到成功列表
							
							int nIndex = m_OKList.GetCount();
							m_OKList.InsertString(nIndex, strFileName);
						}
						else
						{
							//添加到失败列表
							int nIndex = m_FailList.GetCount();
							m_FailList.InsertString(nIndex, strFileName + "确定为病毒文件,正在使用! ");
						}
						
					}
				}
				else
				{
					//添加到失败列表
					int nIndex = m_FailList.GetCount();
					m_FailList.InsertString(nIndex, strFileName+"无法访问");
				}
			}
		}
NEXT:	
		BOOL IsFindOK = FindNextFile(hFind, &wfd);
		if (FALSE == IsFindOK /*|| ERROR_NO_MORE_FILES == GetLastError()*/)
		{	//目录递归查找
			FindClose(hFind);
			return ;
		}

	} while (TRUE);


	

	return;
}

DWORD _stdcall SanfLpkFile(LPVOID lpThis)
{
	CExeDlg* lpDlg = (CExeDlg*)lpThis;
	CString strNoDelLpkFilePath;
	int nIndex = 0;
	char DiskNameBuf[MAX_PATH] = {0};
	int nDiskCount = GetLogicalDriveStrings(MAX_PATH, DiskNameBuf)/4;
	
	for (int i = 0; i < nDiskCount; i++)
	{
		CString stCurDiskName;
		stCurDiskName.Format("%s", DiskNameBuf+i*4);
		lpDlg->FindLpk(stCurDiskName);
	}
	lpDlg->m_BtnDeleteLpk.EnableWindow(TRUE);

	return 0;
}

