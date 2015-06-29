// MyShellDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MyShell.h"
#include "MyShellDlg.h"

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
// CMyShellDlg dialog

CMyShellDlg::CMyShellDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMyShellDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMyShellDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyShellDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyShellDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMyShellDlg, CDialog)
	//{{AFX_MSG_MAP(CMyShellDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_SHELL, OnBtnShell)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyShellDlg message handlers

BOOL CMyShellDlg::OnInitDialog()
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

void CMyShellDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMyShellDlg::OnPaint() 
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
HCURSOR CMyShellDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMyShellDlg::OnBtnShell() 
{
	// TODO: Add your control notification handler code here
    GetDlgItemText(IDC_EDIT1, m_StrFile);
    if ( m_StrFile == "")
    {
        AfxMessageBox("请选择一个要加壳的文件");
        return ;
    }

	m_hFile = CreateFile(m_StrFile.GetBuffer(0),
                    GENERIC_READ | GENERIC_WRITE,
                    FILE_SHARE_READ,
                    NULL,
                    OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL,
                    NULL);
    if ( m_hFile == INVALID_HANDLE_VALUE )
    {
        AfxMessageBox("打开文件失败!");
        return ;
    }

    BOOL bRet = IsPeFile();
    if ( bRet == FALSE )
    {
        CloseHandle(m_hFile);
        return ;
    }

    GetPeInfo();

    AddSection();

    Encode();

    WriteDecode();

    CloseHandle(m_hFile);
}

// 判断PE有效性
BOOL CMyShellDlg::IsPeFile()
{
    DWORD dwRead = 0;

    ReadFile(m_hFile, (LPVOID)&m_ImgDos, sizeof(IMAGE_DOS_HEADER), &dwRead, NULL);
    if ( m_ImgDos.e_magic != IMAGE_DOS_SIGNATURE )
    {
        return FALSE;
    }

    SetFilePointer(m_hFile, m_ImgDos.e_lfanew, 0, FILE_BEGIN);

    ReadFile(m_hFile, (LPVOID)&m_NtHdr, sizeof(IMAGE_NT_HEADERS), &dwRead, NULL);
    if ( m_NtHdr.Signature != IMAGE_NT_SIGNATURE )
    {
        return FALSE;
    }

    return TRUE;
}

// 获取PE相关信息
VOID CMyShellDlg::GetPeInfo()
{
    DWORD dwRead = 0;

    m_dwImageBase = m_NtHdr.OptionalHeader.ImageBase;
    m_dwEntryPoint = m_NtHdr.OptionalHeader.AddressOfEntryPoint;

    DWORD dwOffset = m_ImgDos.e_lfanew + 4 + sizeof(IMAGE_FILE_HEADER) + m_NtHdr.FileHeader.SizeOfOptionalHeader;

    SetFilePointer(m_hFile, dwOffset, 0, FILE_BEGIN);

    ReadFile(m_hFile, (LPVOID)&m_SecTextInfo, sizeof(IMAGE_SECTION_HEADER), &dwRead, NULL);
}

DWORD CMyShellDlg::AlignSize(int nSecSize, DWORD Alignment)
{
    int nSize = nSecSize;
    if ( nSize % Alignment != 0 )
    {
        nSecSize = (nSize / Alignment + 1) * Alignment;
    }
    
    return nSecSize;
}

// 添加节
VOID CMyShellDlg::AddSection()
{
    DWORD dwOffset = m_ImgDos.e_lfanew + 4 + sizeof(IMAGE_FILE_HEADER) + m_NtHdr.FileHeader.SizeOfOptionalHeader;
    IMAGE_SECTION_HEADER SecTmp = { 0 };
    DWORD dwRead = 0;

    SetFilePointer(m_hFile, dwOffset, 0, FILE_BEGIN);
    dwOffset = sizeof(IMAGE_SECTION_HEADER) * (m_NtHdr.FileHeader.NumberOfSections - 1);//现在有的区大小
    SetFilePointer(m_hFile, dwOffset, 0, FILE_CURRENT);
    ReadFile(m_hFile, (LPVOID)&SecTmp, sizeof(IMAGE_SECTION_HEADER), &dwRead, NULL);

    strcpy((char *)m_SecNewHdr.Name, "test");
    m_SecNewHdr.Misc.VirtualSize = 0x1000;
    m_SecNewHdr.VirtualAddress = SecTmp.VirtualAddress + AlignSize(SecTmp.Misc.VirtualSize, m_NtHdr.OptionalHeader.SectionAlignment);
    m_SecNewHdr.PointerToRawData = SecTmp.PointerToRawData + SecTmp.SizeOfRawData;
    m_SecNewHdr.SizeOfRawData = 0x1000;
    m_SecNewHdr.Characteristics = m_SecTextInfo.Characteristics;

    WriteFile(m_hFile, (LPVOID)&m_SecNewHdr, sizeof(IMAGE_SECTION_HEADER), &dwRead, NULL);

    m_NtHdr.FileHeader.NumberOfSections ++;
    m_NtHdr.OptionalHeader.SizeOfImage += m_SecNewHdr.SizeOfRawData;
    m_NtHdr.OptionalHeader.AddressOfEntryPoint = m_SecNewHdr.VirtualAddress;
    SetFilePointer(m_hFile, m_ImgDos.e_lfanew, 0, FILE_BEGIN);
    WriteFile(m_hFile, (LPVOID)&m_NtHdr, sizeof(IMAGE_NT_HEADERS), &dwRead, NULL);

    BYTE Byte[0x1000] = { 0 };
    SetFilePointer(m_hFile, 0, 0, FILE_END);
    WriteFile(m_hFile, (LPVOID)Byte, 0x1000, &dwRead, NULL);//节后面再写0x1000

    m_SecTextInfo.Characteristics |= IMAGE_SCN_MEM_WRITE;//可写

    dwOffset = m_ImgDos.e_lfanew + 4 + sizeof(IMAGE_FILE_HEADER) + m_NtHdr.FileHeader.SizeOfOptionalHeader;
    SetFilePointer(m_hFile, dwOffset, 0, FILE_BEGIN);
    WriteFile(m_hFile, (LPVOID)&m_SecTextInfo, sizeof(IMAGE_SECTION_HEADER), &dwRead, NULL);
}

char Decode[] = 
"\x60"
"\xb8\x00\x00\x00\x00"
"\x80\x30\x88"
"\x40"
"\x3d\xff\x4f\x40\x00"
"\x75\xf5"
"\x61"
"\xb8\x00\x00\x00\x00"
"\xff\xe0";

VOID CMyShellDlg::WriteDecode()
{
    DWORD dwWrite = 0;

    // 写入代码节的开始位置
    *(DWORD *)&Decode[2] = m_dwImageBase + m_SecTextInfo.VirtualAddress;
    // 代码节终止位置
    *(DWORD *)&Decode[11] = m_dwImageBase + m_SecTextInfo.VirtualAddress + m_SecTextInfo.Misc.VirtualSize;
    // 写入OEP
    *(DWORD *)&Decode[19] = m_dwImageBase + m_dwEntryPoint;

    SetFilePointer(m_hFile, m_SecNewHdr.PointerToRawData, 0, FILE_BEGIN);
    WriteFile(m_hFile, (LPVOID)Decode, sizeof(Decode), &dwWrite, NULL);
}

// 加密代码节
VOID CMyShellDlg::Encode()
{
    DWORD dwRead = 0;
    PBYTE pByte = NULL;
    pByte = (PBYTE)malloc(m_SecTextInfo.Misc.VirtualSize);

    SetFilePointer(m_hFile, m_SecTextInfo.PointerToRawData, 0, FILE_BEGIN);
    ReadFile(m_hFile, pByte, m_SecTextInfo.Misc.VirtualSize, &dwRead, NULL);

    for ( DWORD i = 0; i < m_SecTextInfo.Misc.VirtualSize; i++ )
    {
        pByte[i] ^= 0x88;
    }

    SetFilePointer(m_hFile, m_SecTextInfo.PointerToRawData, 0, FILE_BEGIN);
    WriteFile(m_hFile, pByte, m_SecTextInfo.Misc.VirtualSize, &dwRead, NULL);

    free(pByte);
}