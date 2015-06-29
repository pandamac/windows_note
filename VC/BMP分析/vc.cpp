/*
 *	作者：adly
 */
#include <stdio.h>
#include <windows.h>

#pragma pack(1)						// 结构体对齐方式,以一字节对齐

#define  INFILENAME  "inBMP.bmp"	// 输入位图文件名
#define  OUTFILENAME  "outBMP.txt"	// 输出位图文件名

// BMP文件头结构体
typedef struct _BMPHead
{
	short Head_Flag;	//1   //0x00-0x01 bmp文件标志，固定为'BM'
	int File_Size;		//2   //0x02-0x05 文件总长度，包括文件头
	int Reserved;		//3   //0x06-0x09 保留未用
	int Data_Offset;	//4   //0x0A-0x0D 数据区的开始位置，由文件头算起
	int Head_Info_Size;//5   //0x0E-0x11 位图信息头的大小,Windows下为0x28
	int Pic_Width;		//6   //0x12-0x15 图形的宽度，单位是象素
	int Pic_High;		//7   //0x16-0x19 图形的高度，单位象素
	short Pic_Plans;	//8   //0x1A-0x1B 图形的页数
	short Color_Bit;	//9   //0x1C-0x1D 颜色的位数（深度）
	int Compress;		//10  //0x1E-0x21 压缩方式，0表示未压缩
	int Data_Size;		//11  //0x22-0x25 数据区的长度
	int Pic_Xppm;		//12  //0x26-0x29 水平分辨率
	int Pic_Yppm;		//13  //0x2A-0x2D 垂直分辨率
	int Color_Used;	//14  //0x2E-0x31 位图使用的颜色数
	int Color_Imp;		//15  //0x32-0x35 重要颜色数目
}BMPHead, *pBMPHead;

// 读取BMP文件头函数
bool ReadBMPHeadFromFile( FILE *in, BMPHead & bmpHead )
{
	if ( fgets( (char*)&bmpHead, sizeof( bmpHead ), in ) == NULL )
	{
		printf( "读取文件头时出现错误!\n" );
		return FALSE;
	}
	return TRUE;
}
// 读取BMP数据到文件
bool ReadBMPDataFromFile( FILE *in, FILE *out )
{
	int ch;
	ch = fgetc( in );
	while( ! feof( in ) )
	{
		ch = fgetc( in );
		if ( ch == 0xFFFFFFFF ) break;
		fprintf( out, "%02X ", ch );
	}
	return TRUE;
}
// 显示BMP文件头函数
bool DisplayBMPHead( BMPHead & bmpHead )
{
	char *bHeadInfo = (char*) malloc( 500 + sizeof( BMPHead ) );
	if ( bHeadInfo == NULL )
	{
		printf( "分配内存失败!\n" );
		return FALSE;
	}
	
	sprintf( bHeadInfo, "文件标志:\t%c%c\n", *((char*)&bmpHead), *((char*)&bmpHead + 1) );
	sprintf( bHeadInfo, "%s文件长度:\t%d\t(0x%X)\n", bHeadInfo, bmpHead.File_Size, bmpHead.File_Size );
	sprintf( bHeadInfo, "%s保留:\t\t%d\t(0x%X)\n", bHeadInfo, bmpHead.Reserved, bmpHead.Reserved );
	sprintf( bHeadInfo, "%s数据起始位置:\t%d\t(0x%X)\n", bHeadInfo, bmpHead.Data_Offset, bmpHead.Data_Offset );
	sprintf( bHeadInfo, "%s位图信息头大小:\t%d\t(0x%X)\n", bHeadInfo, bmpHead.Head_Info_Size, bmpHead.Head_Info_Size );
	sprintf( bHeadInfo, "%s位图宽度:\t%d\t(0x%X)\n", bHeadInfo, bmpHead.Pic_Width, bmpHead.Pic_Width );
	sprintf( bHeadInfo, "%s位图高度:\t%d\t(0x%X)\n", bHeadInfo, bmpHead.Pic_High, bmpHead.Pic_High );
	sprintf( bHeadInfo, "%s位图页数:\t%d\t(0x%X)\n", bHeadInfo, bmpHead.Pic_Plans, bmpHead.Pic_Plans );
	sprintf( bHeadInfo, "%s颜色深度:\t%d\t(0x%X)\n", bHeadInfo, bmpHead.Color_Bit, bmpHead.Color_Bit );
	sprintf( bHeadInfo, "%s压缩方式:\t%d\t(0x%X)\n", bHeadInfo, bmpHead.Compress, bmpHead.Compress );
	sprintf( bHeadInfo, "%s数据大小:\t%d\t(0x%X)\n", bHeadInfo, bmpHead.Data_Size, bmpHead.Data_Size );
	sprintf( bHeadInfo, "%s水平分辨率:\t%d\t(0x%X)\n", bHeadInfo, bmpHead.Pic_Xppm, bmpHead.Pic_Xppm );
	sprintf( bHeadInfo, "%s垂直分辨率:\t%d\t(0x%X)\n", bHeadInfo, bmpHead.Pic_Yppm, bmpHead.Pic_Yppm );
	sprintf( bHeadInfo, "%s位图颜色数:\t%d\t(0x%X)\n", bHeadInfo, bmpHead.Color_Used, bmpHead.Color_Used );
	sprintf( bHeadInfo, "%s重要颜色数:\t%d\t(0x%X)\n", bHeadInfo, bmpHead.Color_Imp, bmpHead.Color_Imp );
	printf( "%s", bHeadInfo );
	
	free( bHeadInfo );
	return TRUE;
}
// 主函数
int main()
{
	FILE *in, *out;
	BMPHead bHead;
	if (( in = fopen( INFILENAME, "rb" )) == NULL )
	{
		printf( "打开文件 %s 时出错!\n", INFILENAME );
		system( "pause>nul" );
		return -1;
	}
	if ( ! ReadBMPHeadFromFile( in, bHead ) )	// 从文件读取BMP文件头
	{
		printf( "从文件读取BMP文件头到结构体失败!\n" );
		system( "pause>nul" );
		return -1;
	}
	if ( ! DisplayBMPHead( bHead ) )			// 显示BMP文件头
	{
		printf( "显示BMP文件头失败!\n" );
		system( "pause>nul" );
		return -1;
	}
	// 打开输出文件,读取BMP数据到该文件
	if (( out = fopen( OUTFILENAME, "wb" )) == NULL )
	{
		printf( "打开文件 %s 时出错!\n", OUTFILENAME );
		system( "pause>nul" );
		return -1;
	}
	if ( ! ReadBMPDataFromFile( in, out ) )		// 读取BMP数据到文件
	{
		printf( "读取BMP数据到文件失败!\n" );
		system( "pause>nul" );
		return -1;
	}
	else
	{
		printf( "\n\nBMP 数据文件已读取到文件 %s 中!!!\n", OUTFILENAME );
	}
	fclose( out );
	fclose( in );
	system( "pause>nul" );
	return 0;
}