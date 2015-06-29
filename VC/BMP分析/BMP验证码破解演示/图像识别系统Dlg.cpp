// 图像识别系统Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "图像识别系统.h"
#include "图像识别系统Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define			BMP_CODE_NAME		"Code.bmp"			//验证码保存为此文件名


#pragma pack(1)		//结构体对齐方式,以一字节对齐

//BMP文件头结构体
typedef struct _BMPHead
{
	short Head_Flag;	//1		//0x00-0x01 bmp文件标志，固定为'BM'
	int File_Size;		//2		//0x02-0x05 文件总长度，包括文件头
	int Reserved;		//3		//0x06-0x09 保留未用
	int Data_Offset;	//4		//0x0A-0x0D 数据区的开始位置，由文件头算起
	int Head_Info_Size;	//5		//0x0E-0x11 位图信息头的大小,Windows下为0x28
	int Pic_Width;		//6		//0x12-0x15 图形的宽度，单位是像素
	int Pic_High;		//7		//0x16-0x19 图形的高度，单位像素
	short Pic_Plans;	//8		//0x1A-0x1B 图形的页数
	short Color_Bit;	//9		//0x1C-0x1D 颜色的位数（深度）
	int Compress;		//10		//0x1E-0x21 压缩方式，0表示未压缩
	int Data_Size;		//11		//0x22-0x25 数据区的长度
	int Pic_Xppm;		//12		//0x26-0x29 水平分辨率
	int Pic_Yppm;		//13		//0x2A-0x2D 垂直分辨率
	int Color_Used;		//14		//0x2E-0x31 位图使用的颜色数
	int Color_Imp;		//15		//0x32-0x35 重要颜色数目
} BMPHead, *pBMPHead;


typedef struct _RESULT_NUM
{
	int LastResult[5];		//最后的五个验证码结果
	int Correct[5];			//五个验证码结果的匹配率
} RESULT_NUM, *pRESULT_NUM;

//数字字符特征信息
char Num[10][56] = \
{
	/*0*/{"\xFF\xFF\xFF\xFF\xFF\xFF\xFC\xCC\xCF\xFF\xFF\xCC\xFF\xCC\xFF\xFF\xCC\xCF\xCC\xFF\xFF\xCC\xCF\xCC\xFF\xFF\xCC\xFF\xCC\xFF\xFF\xCC\xFC\xCC\xFF\xFF\xCC\xFC\xCC\xFF\xFF\xCC\xFF\xCC\xFF\xFF\xFC\xCC\xCF\xFF\xFF\xFF\xFF\xFF\xFF"},
	/*1*/{"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFC\xCF\xFF\xFF\xFF\xFC\xCF\xFF\xFF\xFF\xFC\xCF\xFF\xFF\xFF\xFC\xCF\xFF\xFF\xFF\xFC\xCF\xFF\xFF\xFF\xFC\xCF\xFF\xFF\xCC\xCC\xCF\xFF\xFF\xFF\xCC\xCF\xFF\xFF\xFF\xFC\xCF\xFF\xFF\xFF\xFF\xFF\xFF"},
	/*2*/{"\xFF\xFF\xFF\xFF\xFF\xFF\xCC\xCC\xCC\xFF\xFF\xCC\xFF\xFF\xFF\xFF\xFC\xCF\xFF\xFF\xFF\xFF\xCC\xFF\xFF\xFF\xFF\xFC\xCF\xFF\xFF\xFF\xFF\xCC\xFF\xFF\xCC\xFF\xCC\xFF\xFF\xCC\xFF\xCC\xFF\xFF\xFC\xCC\xCF\xFF\xFF\xFF\xFF\xFF\xFF"},
	/*3*/{"\xFF\xFF\xFF\xFF\xFF\xFF\xFC\xCC\xCF\xFF\xFF\xCC\xFF\xCC\xFF\xFF\xCC\xFF\xCC\xFF\xFF\xFF\xFF\xCC\xFF\xFF\xFF\xCC\xCF\xFF\xFF\xFF\xFF\xCC\xFF\xFF\xCC\xFF\xCC\xFF\xFF\xCC\xFF\xCC\xFF\xFF\xFC\xCC\xCF\xFF\xFF\xFF\xFF\xFF\xFF"},
	/*4*/{"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xCC\xFF\xFF\xFF\xFF\xCC\xFF\xFF\xCC\xCC\xCC\xCF\xFF\xCC\xFF\xCC\xFF\xFF\xFC\xCF\xCC\xFF\xFF\xFC\xCF\xCC\xFF\xFF\xFC\xCF\xCC\xFF\xFF\xFC\xCF\xFF\xFF\xFF\xFC\xCF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"},
	/*5*/{"\xFF\xFF\xFF\xFF\xFF\xFF\xCC\xCC\xFF\xFF\xFF\xFF\xFC\xCF\xFF\xFF\xFF\xFF\xCC\xFF\xFF\xFF\xFF\xCC\xFF\xFF\xCC\xCC\xCF\xFF\xFF\xCC\xFF\xFF\xFF\xFF\xCC\xFF\xFF\xFF\xFF\xCC\xFF\xFF\xFF\xFF\xCC\xCC\xCC\xFF\xFF\xFF\xFF\xFF\xFF"},
	/*6*/{"\xFF\xFF\xFF\xFF\xFF\xFF\xFC\xCC\xCF\xFF\xFF\xCC\xFF\xCC\xFF\xFF\xCC\xFF\xCC\xFF\xFF\xCC\xFF\xCC\xFF\xFF\xCC\xFF\xCC\xFF\xFF\xCC\xCC\xCF\xFF\xFF\xFC\xCF\xFF\xFF\xFF\xFF\xCC\xFF\xFF\xFF\xFF\xCC\xCF\xFF\xFF\xFF\xFF\xFF\xFF"},
	/*7*/{"\xFF\xFF\xFF\xFF\xFF\xFF\xFC\xCF\xFF\xFF\xFF\xFC\xCF\xFF\xFF\xFF\xFC\xCF\xFF\xFF\xFF\xFF\xCC\xFF\xFF\xFF\xFF\xCC\xFF\xFF\xFF\xFF\xFC\xCF\xFF\xFF\xFF\xFC\xCF\xFF\xFF\xFF\xFF\xCC\xFF\xFF\xCC\xCC\xCC\xFF\xFF\xFF\xFF\xFF\xFF"},
	/*8*/{"\xFF\xFF\xFF\xFF\xFF\xFF\xFC\xCC\xCF\xFF\xFF\xCC\xFF\xCC\xFF\xFF\xCC\xFF\xCC\xFF\xFF\xCC\xFC\xCC\xFF\xFF\xFC\xCC\xCF\xFF\xFF\xCC\xCF\xCC\xFF\xFF\xCC\xFF\xCC\xFF\xFF\xCC\xFF\xCC\xFF\xFF\xFC\xCC\xCF\xFF\xFF\xFF\xFF\xFF\xFF"},
	/*9*/{"\xFF\xFF\xFF\xFF\xFF\xFF\xFC\xCC\xFF\xFF\xFF\xFF\xCC\xFF\xFF\xFF\xFF\xFC\xCF\xFF\xFF\xFC\xCC\xCC\xFF\xFF\xCC\xFF\xCC\xFF\xFF\xCC\xFF\xCC\xFF\xFF\xCC\xFF\xCC\xFF\xFF\xCC\xFF\xCC\xFF\xFF\xFC\xCC\xCF\xFF\xFF\xFF\xFF\xFF\xFF"}
};

/////////////////////////////////////////////////////////////////////////////
// CMyDlg dialog

CMyDlg::CMyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMyDlg)
	m_Url = _T("http://www.huihu32.cn/function/c_validcode.asp?name=commentvalid");
	m_Num = _T("");
	m_Correct = _T("");
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyDlg)
	DDX_Text(pDX, IDC_URL, m_Url);
	DDX_Text(pDX, IDC_NUM, m_Num);
	DDX_Text(pDX, IDC_CORRECT, m_Correct);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMyDlg, CDialog)
	//{{AFX_MSG_MAP(CMyDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyDlg message handlers

BOOL CMyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	SetProcessInfo( "欢迎使用 Adly 开发的验证码识别工具!" );
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMyDlg::OnPaint() 
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
	
	//重新加载验证码图标
	HBITMAP Bitmap, OldBitmap;
	Bitmap = (HBITMAP)LoadImage( AfxGetInstanceHandle(), BMP_CODE_NAME, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION );
	CDC MemDC;
	CDC *pDC;
	pDC = GetDC();
	MemDC.CreateCompatibleDC( pDC );
	OldBitmap = (HBITMAP)MemDC.SelectObject( Bitmap );
	pDC->BitBlt( 270, 175, 50, 11, &MemDC, 0, 0, SRCCOPY );
	MemDC.SelectObject( OldBitmap );
}

HCURSOR CMyDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMyDlg::SetProcessInfo( char* Str )	//设置过程信息
{
	GetDlgItem( IDC_Result )->SetWindowText( Str );
}

BOOL CMyDlg::DownLoadBmp()
{
	UpdateData( TRUE );	//得到更新后的验证码网址

	LONG ret = ::URLDownloadToFile( 0, m_Url, BMP_CODE_NAME, 0, 0 );	//下载验证码，保存为 Code.bmp

	if( !ret )		//成功
	{
		return TRUE;
	}

	return FALSE;
}

void CMyDlg::OnStart()	//开始识别验证码
{
	/////////////////////////////
	//下载验证码
	SetProcessInfo( "正在下载验证码 ..." );
	if( !DownLoadBmp() )
	{
		SetProcessInfo( "下载验证码失败 ..." );
		return ;
	}
	/////////////////////////
	//显示验证码
	SetProcessInfo( "正在加载验证码 ..." );
	HBITMAP Bitmap, OldBitmap;
	Bitmap = (HBITMAP)LoadImage( AfxGetInstanceHandle(), BMP_CODE_NAME, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION );
	CDC MemDC;
	CDC *pDC;
	pDC = GetDC();
	MemDC.CreateCompatibleDC( pDC );
	OldBitmap = (HBITMAP)MemDC.SelectObject( Bitmap );
	pDC->BitBlt( 270, 175, 50, 11, &MemDC, 0, 0, SRCCOPY );
	MemDC.SelectObject( OldBitmap );
	/////////////////////////
	//分析验证码
	SetProcessInfo( "正在分析验证码数据 ..." );
	//打开验证码文件
	HANDLE hBmpCode = ::CreateFile( BMP_CODE_NAME, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( hBmpCode == INVALID_HANDLE_VALUE )		//打开失败
	{
		SetProcessInfo( "打开Bmp文件失败 ..." );
		return ;
	}
	/////////////////////////////////////
	//这里将bmp数据写入到 hBmpData 里面

	//读取BMP信息头
	DWORD dwRead;
	BMPHead BmpHead;
	BOOL Success = ::ReadFile( hBmpCode, &BmpHead, sizeof( BmpHead ), &dwRead, NULL );
	if( !Success || ( dwRead != sizeof( BMPHead ) ) )		//读取文件头失败
	{
		CloseHandle( hBmpCode );
		SetProcessInfo( "读取Bmp信息头失败 ..." );
		return ;
	}

	char BmpDataBuffer[5][55];

	//设置文件指针
	::SetFilePointer( hBmpCode, BmpHead.Data_Offset, NULL, FILE_BEGIN );

	int i, j;
	for( i = 0; i < 11; i ++ )		//BMP图片像素高
	{
		for( j = 0; j < 5; j ++ )		//BMP图片中有五个数字
		{
			//读取数据到五个数组中
			Success = ::ReadFile( hBmpCode, (void*)&BmpDataBuffer[j][i*5], 5, &dwRead, NULL );	//每次读取 5 byte
			if( !Success || ( dwRead != 5 ) )		//读取失败
			{
				CloseHandle( hBmpCode );
				SetProcessInfo( "读取Bmp数据失败 ..." );
				return ;
			}
		}
		::SetFilePointer( hBmpCode, 3, NULL, FILE_CURRENT );		//文件指针向后移3字节
	}

	//去掉图片中的噪声
	for( i = 0; i < 5; i ++ )		//5个数字
	{
		for( j = 0; j < 55; j ++ )		//每个数字 55 byte表示
		{
			if( ( BmpDataBuffer[i][j] & 0xF0 ) == 0x80 )
			{
				BmpDataBuffer[i][j] |= 0xF0 ;
			}
			if( ( BmpDataBuffer[i][j] & 0x0F ) == 0x08 )
			{
				BmpDataBuffer[i][j] |= 0x0F ;
			}
		}
	}

	//进行像素匹配
	RESULT_NUM Result;		//保存最后的结果

	int k;
	for( k = 0; k < 5; k ++ )			//5个数字
	{
		int CorrectCount[10] = {0,0,0,0,0,0,0,0,0,0};
		int Total = 0;
		for( i = 0; i < 55; i ++ )		//比较 55 个字节
		{
			for( j = 0; j < 10; j ++ )		//比较 10 个字模进行匹配
			{
				Total ++;
				Total ++;
				if( ( BmpDataBuffer[k][i] & 0xF0 ) == ( Num[j][i] & 0xF0 ) )	//高4位匹配成功
				{
					CorrectCount[j] ++;
				}
				if( ( BmpDataBuffer[k][i] & 0x0F ) == ( Num[j][i] & 0x0F ) )	//低4位匹配成功
				{
					CorrectCount[j] ++;
				}
			}
		}
		Total /= 10;
		//进行统计比较出最佳匹配
		Result.LastResult[k] = 0;
		int MaxCorrect = 0;
		for( i = 0; i < 10; i ++ )		//从10个统计中选出最佳值
		{
			if( MaxCorrect < CorrectCount[i] )
			{
				MaxCorrect = CorrectCount[i];
				Result.LastResult[k] = i;
			}
		}
		Result.Correct[k] = (int)( (double)MaxCorrect/(double)Total * 100.0 );
	}

	//将结果设置到显示界面
	char ResultNumString[6];
	char ResultCorrectString[20];
	sprintf( ResultNumString, "%d%d%d%d%d", Result.LastResult[0], Result.LastResult[1], \
		Result.LastResult[2], Result.LastResult[3], Result.LastResult[4] );
	sprintf( ResultCorrectString, "%d|%d|%d|%d|%d", Result.Correct[0], Result.Correct[1], \
		Result.Correct[2], Result.Correct[3], Result.Correct[4] );

	m_Num.Format( "%s", ResultNumString );
	m_Correct.Format( "%s", ResultCorrectString );

	UpdateData( FALSE );

	CloseHandle( hBmpCode );
	SetProcessInfo( "图像识别完成!!!" );
}

void CMyDlg::OnClose() 
{
	//删除 BMP_CODE_NAME 文件
	::DeleteFile( BMP_CODE_NAME );
	CDialog::OnClose();
}
