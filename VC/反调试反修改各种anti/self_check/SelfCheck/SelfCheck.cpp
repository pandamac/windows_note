#include "SelfCheck.h"
#include <tchar.h>

SelfCheck::SelfCheck(void)
{
}

SelfCheck::~SelfCheck(void)
{
}

//原理：首先需设置一个文件大小，再获取文件大小，如果两个相等，则文件没有被修改
//通常，我们先加一个壳，得到文件大小，设置dwRealSize大小，再获取文件大小
//此时，如果我们脱完课，大小肯定会发生变化，这就叫自校验
BOOL SelfCheck::IsFileModified_By_FileSize()
{
	BOOL bRet = FALSE;
	DWORD dwSize = 0;

	//文件当前大小
	DWORD dwCurrentFileSize = 0;

	//文件实际大小（一般是加壳后的大小）
	DWORD dwRealSize = 5632;
	TCHAR szFileName[MAX_PATH];

	//获取自身文件名
	GetModuleFileName(NULL, szFileName, MAX_PATH);

	//打开文件
	HANDLE hFile = CreateFile(
					szFileName, 
					GENERIC_READ, 
					FILE_SHARE_READ, 
					NULL, 
					OPEN_ALWAYS, 
					FILE_ATTRIBUTE_NORMAL, 
					NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	
	//获取文件大小
	dwCurrentFileSize = GetFileSize(hFile, NULL);

	if (dwCurrentFileSize == dwRealSize)
	{
		bRet = FALSE;
	}
	else
	{
		bRet = TRUE;
	}
	return bRet;
}



BOOL IsFileModified_By_CRC();
DWORD CRC32(BYTE*,DWORD);

// 原理：首先我们用add2crc32.exe工具将CRC32校验值写入到文件的PE头的前四个字节
// 接着就读取PE头的前四个字节，如果两者不相等就表示文件被修改了，这种方法比较准确点，不过当然也是比较好过的。
BOOL SelfCheck::IsFileModified_By_CRC32()
{
	TCHAR szFileName[MAX_PATH] = {0};

	// 获得当前文件名
	GetModuleFileName(NULL, szFileName, MAX_PATH);
	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS pNtHeader;

	// 打开文件
	HANDLE hFile = CreateFile(
		szFileName, 
		GENERIC_READ, 
		FILE_SHARE_READ, 
		NULL, 
		OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	//获取文件大小
	DWORD dwCurrentFileSize = 0;
	dwCurrentFileSize = GetFileSize(hFile, NULL);
	if (dwCurrentFileSize == 0xFFFFFFFF)
	{
		return FALSE;
	}
		
	
	DWORD dwReaded = 0;
	TCHAR *pBuffer = new TCHAR[dwCurrentFileSize];
	DWORD dwOriginalCRC32 = 0; // 原始CRC32值

	//读取硬盘上整个文件
	ReadFile(hFile, pBuffer, dwCurrentFileSize, &dwReaded, NULL);

	pDosHeader = (PIMAGE_DOS_HEADER)pBuffer;
	pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)pDosHeader + pDosHeader->e_lfanew);

	//获取CRC值，我们自定义在Nt头也就是“PE00”前四个字节
	dwOriginalCRC32 = *(DWORD*)((DWORD)pNtHeader - 4);
	
	//文件大小减去Nt头前面的大小，也就是从Nt开始
	dwCurrentFileSize = dwCurrentFileSize - pDosHeader->e_lfanew;
	if (dwOriginalCRC32 == CRC32((BYTE*)pNtHeader, dwCurrentFileSize))
	{
		return FALSE;	//没有被修改
	}
	else
	{
		return TRUE;	//已经被修改
	}
	CloseHandle(hFile);
	delete[] pBuffer;
	


}
////////////////////////////////////////////////////////////////
// 计算字符串的CRC32值
// 参数：欲计算CRC32值字符串的首地址和大小
// 返回值: 返回CRC32值

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
