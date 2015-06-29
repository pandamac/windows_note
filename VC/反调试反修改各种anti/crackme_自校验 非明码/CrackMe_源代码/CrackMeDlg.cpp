// CrackMeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CrackMe.h"
#include "CrackMeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCrackMeDlg dialog

CCrackMeDlg::CCrackMeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCrackMeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCrackMeDlg)
	m_CSUserName = _T("");
	m_CSRegKey = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCrackMeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCrackMeDlg)
	DDX_Text(pDX, IDC_EDIT1, m_CSUserName);
	DDX_Text(pDX, IDC_EDIT2, m_CSRegKey);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCrackMeDlg, CDialog)
	//{{AFX_MSG_MAP(CCrackMeDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTONOK, OnButtonok)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCrackMeDlg message handlers

BOOL CCrackMeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	TimerID=SetTimer(2,500,NULL);

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCrackMeDlg::OnClose() 
{
	KillTimer(TimerID);
	CDialog::OnClose();
}
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCrackMeDlg::OnPaint() 
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
HCURSOR CCrackMeDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CCrackMeDlg::OnButtonok() 
{
	int RegKey[7],retKey=0;
	CString ss;
	char MsgOK[11]	  = {0x7D,0x08,0x18,0x4B,0x68,0x41,0x7F,0x57,0x62,0x1D};
	char MsgError[11] = {0x7D,0x08,0x18,0x4B,0x68,0x41,0x1E,0x47,0x64,0x59};
	char MsgEmpty[21] = {0x6D,0x41,0x60,0x4E,0x62,0x41,0x79,0x69,0x11,0x0D,
						 0x69,0x51,0x11,0x58,0xF8,0xCF,0xCD,0xE1,0xCF,0xD3};
	UpdateData(TRUE);
	if(!(m_CSRegKey=="" || m_CSUserName==""))
	{	
		for(int i=0;i<9;i++)
		{
			retKey += int(m_CSRegKey.GetAt(i) ^ 0xAA);//This method retrieves a single character specified by an index number
		}
		if(CheckReg(m_CSUserName,retKey))
		{
			for(int i=0;i<10;i++) 
			{
				ss+=MsgOK[i] ^ 0xAA;
			}
		}
		else 
		{
			for(int i=0;i<10;i++) 
			{
				ss+=MsgError[i]^0xAA;
			}
		}
	}
	else
	{
		for(int i=0;i<20;i++) 
		{
			ss+=MsgEmpty[i]^0xAA;
		}
	}
	MessageBox(ss);
	ss="";
}

void CCrackMeDlg::OnInit()
{
	char kernel[13] = {0x4B,0x65,0x72,0x6E,0x65,0x6C,0x33,0x32,0x2E,0x64,0x6C,0x6C}; //Kernel32.dll
	char IsDbg[18]  = {0x49,0x73,0x44,0x65,0x62,0x75,0x67,0x67,0x65,0x72,0x50,0x72,
					 0x65,0x73,0x65,0x6E,0x74}; //IsDebuggerPresent
	CString cst;
	for(int i=0;i<12;i++)//将 char 转化为  CString
	{
		cst+=kernel[i];
	}
	HINSTANCE hInst = LoadLibrary(cst);
	cst="";
	if(hInst!=NULL)
	{
		for(int i=0;i<17;i++) 
		{
			cst+=IsDbg[i];
		}
		FARPROC pIsDebuggerPresent=GetProcAddress(hInst,cst);
	
		if(pIsDebuggerPresent()!=NULL)
		{
		//	PostQuitMessage(0);  //发现Debugger,光速逃
		}
	}
	if(!IsFileModified()) 
		PostQuitMessage(0); //文件CRC错误,肯定被修改过
}

BOOL CCrackMeDlg::IsFileModified() //文件CRC Check
{

	DWORD fileSize,OriginalCRC32,NumberOfBytesRW;
	BYTE *pMZheader,pPEheaderRVA;
	TCHAR *pBuffer,szFileName[MAX_PATH];
	GetModuleFileName(NULL,szFileName,MAX_PATH);

	HANDLE hFile=CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,
						    NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,
							NULL);
	if(hFile==INVALID_HANDLE_VALUE) 
		return FALSE;
	fileSize=GetFileSize(hFile,NULL);
	if(fileSize==0xFFFFFFFF) 
		return FALSE;
	pBuffer=new TCHAR[fileSize];
	ReadFile(hFile,pBuffer,fileSize,&NumberOfBytesRW,NULL);
	CloseHandle(hFile);

	pMZheader=(BYTE*)pBuffer;
	pPEheaderRVA=*(pMZheader+0x3C);

	OriginalCRC32=*((DWORD*)(pMZheader+pPEheaderRVA-4));
	fileSize=fileSize-DWORD(pPEheaderRVA);
	pMZheader=pMZheader+pPEheaderRVA;
	//if(CRC32(pMZheader,fileSize)==OriginalCRC32) //PE 指针 + PE后面大小 得到CRC数据
		return TRUE;
	//else 
	//	return FALSE;
}

DWORD CCrackMeDlg::CRC32(BYTE *ptr, DWORD Size)
{
	DWORD CRCTable[256],CRCTmp;
	
	for (int i=0;i<256;i++)
	 {
		CRCTmp=i;
		for (int j=8;j>0;j--)
		 {
			if (CRCTmp&1) CRCTmp=(CRCTmp>>1)^0xEDB88320L;
			else CRCTmp>>=1;
		}
		 CRCTable[i]=CRCTmp;
	 }

	DWORD CRCTmp1=0xFFFFFFFF;
	while(Size--)
	{
		CRCTmp1=((CRCTmp1>>8) & 0x00FFFFFF) ^ CRCTable[(CRCTmp1^(*ptr)) & 0xFF];
		ptr++;
	}
		
	return (CRCTmp1^0xFFFFFFFF);
}

DWORD CCrackMeDlg::CheckRam(DWORD add1, DWORD size)
{
	return CRC32((BYTE*)add1,size);
}

BOOL CCrackMeDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (WM_KEYDOWN == pMsg->message)
    {
       if (VK_RETURN == pMsg->wParam)
            return TRUE;
	   if(VK_ESCAPE == pMsg->wParam)
		   return TRUE;
    }	
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CCrackMeDlg::CheckReg(CString UserName,int RegKey)
{
	int usrtoReg=0,Usr=0;
	BOOL tigger=1;
	char ori[]={0x4B,0x45,0x59,0x2D,0x4B,0x41,0x4E,0x4F,0x4E}; //KEY-KANON

	for(int i=0;i<UserName.GetLength();i++) 
	{
		Usr+=int(UserName.GetAt(i));
	}
	
	for(i=0;i<9;i++)
	{
		usrtoReg+=int(((ori[i] * Usr)%0x1A+0x61)^0xAA);
	}
	if(usrtoReg == RegKey) 
		return TRUE;
	else 
		return FALSE;

}

void CCrackMeDlg::OnTimer(UINT nIDEvent) 
{
	if(!MemCRC32()) 
	{
		PostQuitMessage(0);
	}
	MemCRC32();
	CDialog::OnTimer(nIDEvent);
}



BOOL CCrackMeDlg::MemCRC32()
{
	DWORD fileSize,OriginalCRC32,NumberOfBytesRW;
	DWORD CodeSectionRVA,CodeSectionSize,NumberOfRvaAndSizes,DataDirectorySize,ImageBase;
	BYTE* pMZheader,pPEheaderRVA;
	TCHAR  *pBuffer ;
	TCHAR szFileName[MAX_PATH]; 

	GetModuleFileName(NULL,szFileName,MAX_PATH);

	HANDLE hFile = CreateFile(
		szFileName,
		GENERIC_READ,
		FILE_SHARE_READ, 
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	 if(hFile!=INVALID_HANDLE_VALUE )
	 {
		fileSize=GetFileSize(hFile,NULL);
		if(fileSize==0xFFFFFFFF) //INVALID_FILE_SIZE
			return FALSE;

		pBuffer = new TCHAR [fileSize];
		ReadFile(hFile,pBuffer,fileSize,&NumberOfBytesRW, NULL);
		CloseHandle(hFile);
		
	 }
	 else
		 return FALSE;
	
	pMZheader=(BYTE*)pBuffer;
	pPEheaderRVA= *(pMZheader+0x3c);
	OriginalCRC32=*((DWORD*)(pMZheader+24576));


	NumberOfRvaAndSizes=*((DWORD*)(pMZheader+pPEheaderRVA+0x74));
	DataDirectorySize=NumberOfRvaAndSizes*0x8;
	ImageBase=*((DWORD*)(pMZheader+pPEheaderRVA+0x34));

	delete pBuffer;
	if(OriginalCRC32==CRC32((BYTE*)0x401000,0x1042))//这里不包括 PE文件之前的
		return TRUE;
	else
		return FALSE;
}
