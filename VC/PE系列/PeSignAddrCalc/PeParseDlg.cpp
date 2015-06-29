// PeParseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PeParse.h"
#include "PeParseDlg.h"

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
// CPeParseDlg dialog

CPeParseDlg::CPeParseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPeParseDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPeParseDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_hFile = NULL;
    m_hMap = NULL;
    m_lpBase = NULL;

    m_pDosHdr = NULL;
    m_pNtHdr = NULL;
    m_pSecHdr = NULL;

    m_nSelect = -1;
}

void CPeParseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPeParseDlg)
	DDX_Control(pDX, IDC_EDIT_FILEOFFSET, m_EditFileOffset);
	DDX_Control(pDX, IDC_EDIT_RVA, m_EditRva);
	DDX_Control(pDX, IDC_EDIT_VA, m_EditVa);
	DDX_Control(pDX, IDC_LIST_SECTION, m_SectionLIst);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPeParseDlg, CDialog)
	//{{AFX_MSG_MAP(CPeParseDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_EXIT, OnBtnExit)
	ON_BN_CLICKED(IDC_BTN_SHOW, OnBtnShow)
	ON_BN_CLICKED(IDC_RADIO_VA, OnRadioVa)
	ON_BN_CLICKED(IDC_RADIO_RVA, OnRadioRva)
	ON_BN_CLICKED(IDC_RADIO_FILEOFFSET, OnRadioFileoffset)
	ON_BN_CLICKED(IDC_BTN_CALC, OnBtnCalc)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPeParseDlg message handlers

BOOL CPeParseDlg::OnInitDialog()
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
    InitSectionList();
    InitAddrEdit();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPeParseDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CPeParseDlg::OnPaint() 
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
HCURSOR CPeParseDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CPeParseDlg::OnBtnExit() 
{
	// TODO: Add your control notification handler code here
	EndDialog(0);
}

void CPeParseDlg::OnBtnShow() 
{
    FileClose();

	// TODO: Add your control notification handler code here
    char szFileName[MAX_PATH] = { 0 };
    BOOL bRet = FALSE;

	GetDlgItemText(IDC_EDIT_FILE_PATH, szFileName, MAX_PATH);
    bRet = FileCreate(szFileName);

    if ( bRet == FALSE )
    {
        AfxMessageBox("文件打开错误!");
        return ;
    }

    bRet = IsPeFileAndGetPEPointer();
    if ( bRet == FALSE )
    {
        AfxMessageBox("不是有效的PE文件格式!");
        return ;
    }

    ParseBasePe();

    EnumSections();

    GetPeInfo();
}

VOID CPeParseDlg::InitSectionList()
{
    m_SectionLIst.SetExtendedStyle(
            m_SectionLIst.GetExtendedStyle()
            | LVS_EX_GRIDLINES);

    m_SectionLIst.InsertColumn(0, " 节名 ");
    m_SectionLIst.InsertColumn(1, " V.偏移 ");
    m_SectionLIst.InsertColumn(2, " V.大小 ");
    m_SectionLIst.InsertColumn(3, " R.偏移 ");
    m_SectionLIst.InsertColumn(4, " R.大小 ");
    m_SectionLIst.InsertColumn(5, "标志");

    m_SectionLIst.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
    m_SectionLIst.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
    m_SectionLIst.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);
    m_SectionLIst.SetColumnWidth(3, LVSCW_AUTOSIZE_USEHEADER);
    m_SectionLIst.SetColumnWidth(4, LVSCW_AUTOSIZE_USEHEADER);
    m_SectionLIst.SetColumnWidth(5, LVSCW_AUTOSIZE_USEHEADER);
}

BOOL CPeParseDlg::IsPeFileAndGetPEPointer()
{
    BOOL bRet = FALSE;

    m_pDosHdr = (PIMAGE_DOS_HEADER)m_lpBase;

    if ( m_pDosHdr->e_magic != IMAGE_DOS_SIGNATURE )
    {
        return bRet;
    }

    m_pNtHdr = (PIMAGE_NT_HEADERS)((DWORD)m_lpBase + m_pDosHdr->e_lfanew);

    if ( m_pNtHdr->Signature != IMAGE_NT_SIGNATURE )
    {
        return bRet;
    }

    m_pSecHdr = (PIMAGE_SECTION_HEADER)((DWORD)&(m_pNtHdr->OptionalHeader) + m_pNtHdr->FileHeader.SizeOfOptionalHeader);

    bRet = TRUE;
    return bRet;
}

BOOL CPeParseDlg::FileCreate(char *szFileName)
{
    BOOL bRet = FALSE;

    m_hFile = CreateFile(szFileName, 
                GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL);
    if ( m_hFile == INVALID_HANDLE_VALUE )
    {
        return bRet;
    }

    m_hMap = CreateFileMapping(m_hFile, NULL, PAGE_READWRITE | SEC_IMAGE, 0, 0, 0);
    if ( m_hMap == NULL )
    {
        CloseHandle(m_hFile);
        return bRet;
    }

    m_lpBase = MapViewOfFile(m_hMap, FILE_MAP_READ | FILE_SHARE_WRITE, 0, 0, 0);
    if ( m_lpBase == NULL )
    {
        CloseHandle(m_hMap);
        CloseHandle(m_hFile);
        return bRet;
    }

    bRet = TRUE;
    return bRet;
}

VOID CPeParseDlg::FileClose()
{
    if ( m_lpBase != NULL )
    {
        UnmapViewOfFile(m_lpBase);
        m_lpBase = NULL;
    }

    if ( m_hMap != NULL )
    {
        CloseHandle(m_hMap);
        m_hMap = NULL;
    }

    if ( m_hFile != NULL )
    {
        CloseHandle(m_hFile);
        m_hFile = NULL;
    }
}

VOID CPeParseDlg::ParseBasePe()
{
    CString StrTmp;

    StrTmp.Format("%08X", m_pNtHdr->OptionalHeader.AddressOfEntryPoint);
    SetDlgItemText(IDC_EDIT_EP, StrTmp);

    StrTmp.Format("%08X", m_pNtHdr->OptionalHeader.ImageBase);
    SetDlgItemText(IDC_EDIT_IMAGEBASE, StrTmp);

    StrTmp.Format("%d.%d", 
        m_pNtHdr->OptionalHeader.MajorLinkerVersion,
        m_pNtHdr->OptionalHeader.MinorLinkerVersion);
    SetDlgItemText(IDC_EDIT_LINKVERSION, StrTmp);

    StrTmp.Format("%02X", m_pNtHdr->FileHeader.NumberOfSections);
    SetDlgItemText(IDC_EDIT_SECTIONNUM, StrTmp);

    StrTmp.Format("%08X", m_pNtHdr->OptionalHeader.FileAlignment);
    SetDlgItemText(IDC_EDIT_FILEALIGN, StrTmp);

    StrTmp.Format("%08X", m_pNtHdr->OptionalHeader.SectionAlignment);
    SetDlgItemText(IDC_EDIT_SECALIGN, StrTmp);
}

VOID CPeParseDlg::EnumSections()
{
    int nSecNum = m_pNtHdr->FileHeader.NumberOfSections;

    int i = 0;
    CString StrTmp;

    for ( i = 0; i < nSecNum; i ++ )
    {
        m_SectionLIst.InsertItem(i, (const char *)m_pSecHdr[i].Name);

        StrTmp.Format("%08X", m_pSecHdr[i].VirtualAddress);
        m_SectionLIst.SetItemText(i, 1, StrTmp);

        StrTmp.Format("%08X", m_pSecHdr[i].Misc.VirtualSize);
        m_SectionLIst.SetItemText(i, 2, StrTmp);

        StrTmp.Format("%08X", m_pSecHdr[i].PointerToRawData);
        m_SectionLIst.SetItemText(i, 3, StrTmp);

        StrTmp.Format("%08X", m_pSecHdr[i].SizeOfRawData);
        m_SectionLIst.SetItemText(i, 4, StrTmp);

        StrTmp.Format("%08X", m_pSecHdr[i].Characteristics);
        m_SectionLIst.SetItemText(i, 5, StrTmp);
    }
}

#define NAMELEN 20
#define SIGNLEN 32

typedef struct _SIGN
{
    char szName[NAMELEN];
    BYTE bSign[SIGNLEN + 1];
}SIGN, *PSIGN;

SIGN Sign[2] = 
{
    {
        "VC6",
        "\x55\x8B\xEC\x6A\xFF\x68\xC0\x54\x41\x00" \
        "\x68\xF8\x26\x40\x00\x64\xA1\x00\x00\x00" \
        "\x00\x50\x64\x89\x25\x00\x00\x00\x00\x83" \
        "\xC4\x94"
    },
    {
        "ASPACK",
        "\x60\xE8\x03\x00\x00\x00\xE9\xEB\x04\x5D" \
        "\x45\x55\xC3\xE8\x01\x00\x00\x00\xEB\x5D" \
        "\xBB\xED\xFF\xFF\xFF\x03\xDD\x81\xEB" \
        "\x00\xB0\x01"
    }
};

VOID CPeParseDlg::GetPeInfo()
{
    PBYTE pSign = NULL;

    pSign = (PBYTE)((DWORD)m_lpBase + m_pNtHdr->OptionalHeader.AddressOfEntryPoint);

    if ( memcmp(Sign[0].bSign, pSign, SIGNLEN) == 0 )
    {
        SetDlgItemText(IDC_EDIT_PEINFO, Sign[0].szName);
    }
    else if ( memcmp(Sign[1].bSign, pSign, SIGNLEN) == 0  )
    {
        SetDlgItemText(IDC_EDIT_PEINFO, Sign[1].szName);
    }
    else
    {
        SetDlgItemText(IDC_EDIT_PEINFO, "未知");
    }
}


void CPeParseDlg::OnRadioVa() 
{
	// TODO: Add your control notification handler code here
	m_EditVa.EnableWindow(TRUE);
    m_EditRva.EnableWindow(FALSE);
    m_EditFileOffset.EnableWindow(FALSE);

    m_nSelect = 1;
}

void CPeParseDlg::OnRadioRva() 
{
	// TODO: Add your control notification handler code here
    m_EditVa.EnableWindow(FALSE);
    m_EditRva.EnableWindow(TRUE);
    m_EditFileOffset.EnableWindow(FALSE);
    
    m_nSelect = 2;	
}

void CPeParseDlg::OnRadioFileoffset() 
{
	// TODO: Add your control notification handler code here
    m_EditVa.EnableWindow(FALSE);
    m_EditRva.EnableWindow(FALSE);
    m_EditFileOffset.EnableWindow(TRUE);
    
    m_nSelect = 3;	
}

void CPeParseDlg::OnBtnCalc() 
{
	// TODO: Add your control notification handler code here
	DWORD dwAddr = 0;
    // 获取的地址
    dwAddr = GetAddr();

    // 地址所在的节
    int nInNum = GetAddrInSecNum(dwAddr);

    // 计算其他地址
    CalcAddr(nInNum, dwAddr);
}

VOID CPeParseDlg::InitAddrEdit()
{
    m_EditVa.EnableWindow(FALSE);
    m_EditRva.EnableWindow(FALSE);
    m_EditFileOffset.EnableWindow(FALSE);
}

DWORD CPeParseDlg::GetAddr()
{
    char szAddr[10] = { 0 };
    DWORD dwAddr = 0;
    switch ( m_nSelect )
    {
    case 1:
        {
            GetDlgItemText(IDC_EDIT_VA, szAddr, 10);
            HexStrToInt(szAddr, &dwAddr);
            break;
        }
    case 2:
        {
            GetDlgItemText(IDC_EDIT_RVA, szAddr, 10);
            HexStrToInt(szAddr, &dwAddr);
            break;
        }
    case 3:
        {
            GetDlgItemText(IDC_EDIT_FILEOFFSET, szAddr, 10);
            HexStrToInt(szAddr, &dwAddr);
            break;
        }
    }

    return dwAddr;
}

void CPeParseDlg::HexStrToInt(char *szHexStr, DWORD *pdwHexVal)
{
    char   *pCH, c;
    DWORD  dwVal = 0, dw;
    
    pCH = szHexStr;
    while (*pCH)
    {
        c = toupper(*pCH++);
        if (c >= 'A' && c <= 'F')
            dw = (DWORD)c - ((DWORD)'A' - 10);
        else if (c >= '0' && c <= '9')
            dw = (DWORD)c - (DWORD)'0';
        else
            return ; // invalid hex char
        dwVal = (dwVal << 4) + dw;
    }
    
    *pdwHexVal = dwVal;
}

int CPeParseDlg::GetAddrInSecNum(DWORD dwAddr)
{
    int nInNum = 0;
    int nSecNum = m_pNtHdr->FileHeader.NumberOfSections;
    switch ( m_nSelect )
    {
    case 1:
        {
            DWORD dwImageBase = m_pNtHdr->OptionalHeader.ImageBase;
            for ( nInNum = 0; nInNum < nSecNum; nInNum ++ )
            {
                if ( dwAddr >= dwImageBase + m_pSecHdr[nInNum].VirtualAddress
                    && dwAddr <= dwImageBase + m_pSecHdr[nInNum].VirtualAddress + m_pSecHdr[nInNum].Misc.VirtualSize)
                {
                    return nInNum;
                }
            }
            break;
        }
    case 2:
        {
            for ( nInNum = 0; nInNum < nSecNum; nInNum ++ )
            {
                if ( dwAddr >= m_pSecHdr[nInNum].VirtualAddress
                    && dwAddr <= m_pSecHdr[nInNum].VirtualAddress + m_pSecHdr[nInNum].Misc.VirtualSize)
                {
                    return nInNum;
                }
            }
            break;
        }
    case 3:
        {
            for ( nInNum = 0; nInNum < nSecNum; nInNum ++ )
            {
                if ( dwAddr >= m_pSecHdr[nInNum].PointerToRawData
                    && dwAddr <= m_pSecHdr[nInNum].PointerToRawData + m_pSecHdr[nInNum].SizeOfRawData)
                {
                    return nInNum;
                }
            }
            break;
        }
    }

    return -1;
}

VOID CPeParseDlg::CalcAddr(int nInNum, DWORD dwAddr)
{
    DWORD dwVa = 0;
    DWORD dwRva = 0;
    DWORD dwFileOffset = 0;

    switch ( m_nSelect )
    {
    case 1:
        {
            dwVa = dwAddr;
            dwRva = dwVa - m_pNtHdr->OptionalHeader.ImageBase;
            dwFileOffset = m_pSecHdr[nInNum].PointerToRawData + (dwRva - m_pSecHdr[nInNum].VirtualAddress);
            break;
        }
    case 2:
        {
            dwVa = dwAddr + m_pNtHdr->OptionalHeader.ImageBase;
            dwRva = dwAddr;
            dwFileOffset = m_pSecHdr[nInNum].PointerToRawData + (dwRva - m_pSecHdr[nInNum].VirtualAddress);
            break;
        }
    case 3:
        {
            dwFileOffset = dwAddr;
            dwRva = m_pSecHdr[nInNum].VirtualAddress + (dwFileOffset - m_pSecHdr[nInNum].PointerToRawData);
            dwVa = dwRva + m_pNtHdr->OptionalHeader.ImageBase;
            break;
        }
    }

    SetDlgItemText(IDC_EDIT_SECTION, (const char *)m_pSecHdr[nInNum].Name);

    CString str;
    str.Format("%08X", dwVa);
    SetDlgItemText(IDC_EDIT_VA, str);
    
    str.Format("%08X", dwRva);
    SetDlgItemText(IDC_EDIT_RVA, str);
    
    str.Format("%08X", dwFileOffset);
    SetDlgItemText(IDC_EDIT_FILEOFFSET, str);
}

void CPeParseDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
    FileClose();
}
