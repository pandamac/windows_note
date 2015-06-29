// MyPack.cpp: implementation of the CMyPack class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma warning(disable:4786)
#pragma warning(disable:4996)
#include "MyPack.h"
#include "Include/CopyResource.h"
#pragma comment(lib, "Lib/CopyResource.lib")

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

///构造函数
CPack::CPack(BOOL bCompressResource)
: m_dwRelocationTableRva(0)
{
	//成员缓冲区清零
	RtlZeroMemory(m_cFileName, 256);
	//成员指针初始化为空值
	m_pMemPointer = NULL; //指向PE加载后的指针
	m_pSplithData = NULL; //指向保存覆盖数据的指针,如果有,没有就为空
	m_bCompressResource = bCompressResource;
	m_pComSec = NULL;
	m_bLoadSuccess = FALSE;
	m_iComSecNum = 0;
	m_dwRelocationTableRva = 0;
}

//析构函数释放内存
CPack::~CPack()
{
	if (m_pMemPointer  != NULL)
	{
		GlobalFree(m_pMemPointer  );
		m_pMemPointer  = NULL;
	}
	if (m_pSplithData != NULL)
	{
		GlobalFree(m_pSplithData);
		m_pSplithData = NULL;
	}
	for(int i=0; i<m_iComSecNum; i++)
	{
		free(m_pComSec->lpCompessData);
	}
	if (m_pComSec != NULL)
	{
		delete []m_pComSec;
		m_pComSec = NULL;
	}
}

//判断文件是否存在
BOOL CPack::IsFileExist(char * pFileName)
{
	bool bResult = false;
	WIN32_FIND_DATA findData;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	char *pTempPath = NULL;
	if (pFileName == NULL)
	{
		pTempPath = m_cFileName;
	}
	else
	{
		pTempPath = pFileName;
	}
	if ((hFile = FindFirstFile(pTempPath, &findData)) != INVALID_HANDLE_VALUE)
	{
		bResult = true;
	}
	else
	{	
		if (ERROR_FILE_NOT_FOUND == GetLastError())
		{
			bResult = false;
		}
	}
	FindClose(hFile);
	return bResult;
}

//判断文件是否为PE格式
BOOL CPack::IsPeFile(TCHAR *pFileName)
{
	BOOL bRet = FALSE;
	char *pTempPath = NULL;
	if (pFileName == NULL)
	{
		pTempPath = m_cFileName;
	}
	else
	{
		pTempPath = pFileName;
	}
	//创建指定文件句柄
	HANDLE hFile = CreateFile(pTempPath, GENERIC_READ , FILE_SHARE_READ, NULL, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (!hFile || hFile == INVALID_HANDLE_VALUE)
	{	
		return FALSE;
	}
	WORD dwTempRead, dwSize = 0;
	DWORD dwReadInFactSize;
	//读取文件开始两字节,即MZ头
	BOOL bRead = ReadFile(hFile, &dwTempRead, sizeof(WORD), &dwReadInFactSize, NULL);
	if (!bRead || sizeof(WORD) != dwReadInFactSize)
	{
		goto OVER;
	}
	if (dwTempRead != 0x5a4d)
	{
		goto OVER;
	}
	//得到e_lfanew 成员在IMAGE_DOS_HEADER 结构中的偏移
	dwSize = offsetof(IMAGE_DOS_HEADER, e_lfanew);
	//将文件指针移动到得到e_lfanew成员
	SetFilePointer(hFile, dwSize, NULL, FILE_BEGIN);
	//读取得到e_lfanew成员的内容,也就是PE头在文件中的偏移
	bRead = ReadFile(hFile, &dwTempRead, sizeof(WORD), &dwReadInFactSize, NULL);
	if (!bRead || sizeof(WORD) != dwReadInFactSize)
	{
		goto OVER;
	}
	//将指针移动到PE头
	SetFilePointer(hFile, dwTempRead, NULL, FILE_BEGIN);
	//读取PE标志
	bRead = ReadFile(hFile, &dwTempRead, sizeof(WORD), &dwReadInFactSize, NULL);
	if (!bRead || sizeof(WORD) != dwReadInFactSize)
	{
		goto OVER;
	}
	if (dwTempRead != 0x4550)
	{
		goto OVER;
	}
	bRet = TRUE;
OVER:
	CloseHandle(hFile);
	//该文件属于PE格式,返回TRUE.
	return bRet;	
}

//以dwAlignment 对齐dwOperateNum 值,也就是让dwOperateNum为dwAlignment的整数倍
DWORD CPack::AlignmentNum(DWORD dwOperateNum, DWORD dwAlignment)
{
	if (dwAlignment == 0)
	{
		return dwOperateNum;
	}
	int iTemp = dwOperateNum % dwAlignment;
	if (iTemp)
	{
		return dwOperateNum + dwAlignment - iTemp;
	}
	return dwOperateNum;
}

//根据Rva,返回Rva所属节的节表指针,如果不在节中,则返回-1
IMAGE_SECTION_HEADER *CPack::GetSectionPtrByRva(DWORD dwRva)
{
	for (unsigned int i = 0; i < m_iSecNum; i++)
	{
		if (dwRva >= m_pPeSectionHeader[i].VirtualAddress && 
			dwRva < m_pPeSectionHeader[i].VirtualAddress + m_pPeSectionHeader[i].Misc.VirtualSize)
		{
			return &m_pPeSectionHeader[i];
		}
	}
	return NULL;
}

//获得数据目录某项信息,返回目录大小,参数传出目录RVA.
DWORD CPack::GetDataDirectoryInfo(DWORD dwDataDirectory, DWORD &dwDataDirectoryOffset)
{
	if (dwDataDirectory > 15)
	{
		return -1;
	}
	dwDataDirectoryOffset = m_pPeNtHeader->OptionalHeader.DataDirectory[dwDataDirectory].VirtualAddress;
	return m_pPeNtHeader->OptionalHeader.DataDirectory[dwDataDirectory].Size;
}

//处理附加数据
BOOL CPack::DoSpilthData(TCHAR *pFileName)
{
	BOOL bRet = FALSE;
	char *pTempPath = NULL;
	if (pFileName == NULL)
	{
		pTempPath = m_cFileName;
	}
	else
	{
		pTempPath = pFileName;
	}
	//创建指定文件句柄
	HANDLE hFile = CreateFile(pTempPath, GENERIC_READ , FILE_SHARE_READ, NULL, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (!hFile || hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}		
	//得到文件的实际大小
	unsigned int iTempFileSize = GetFileSize(hFile, NULL);
	//得到PE文件的有效大小
	unsigned int iTempPeSize = m_pPeSectionHeader[m_iSecNum-1].PointerToRawData + 
		m_pPeSectionHeader[m_iSecNum-1].SizeOfRawData;
	//如果PE文件有效大小小于实际大小,那么包含附加数(overlay),要保存附加数据
	unsigned int iDescrepancy = iTempFileSize - iTempPeSize;
	if (iDescrepancy <= 0)
	{
		bRet = TRUE;
		goto OVER;
	}
	else
	{
		SetFilePointer(hFile, iTempPeSize, NULL, FILE_BEGIN);
		m_dwSplitDataSize = iDescrepancy;
		m_pSplithData = GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, iDescrepancy);
		if (m_pSplithData == NULL)
		{
			goto OVER;
		}
		DWORD dwInfactRead;
		BOOL bRead = ReadFile(hFile, m_pSplithData, iDescrepancy, &dwInfactRead, NULL);
		if (!bRead || iDescrepancy != dwInfactRead)
		{
			goto OVER;
		}
		bRet = TRUE;
	}
OVER:
	CloseHandle(hFile);
	return bRet;
}

//按照PE加载器加载的方式将程序读入内存
BOOL CPack::ReadFileToMem(TCHAR *pFileName)
{
	BOOL bRet = FALSE;
	unsigned int i = 0;
	char *pTempPath = NULL;
	DWORD dwTempSize = 0;
	long lTempSize = 0;
	IMAGE_NT_HEADERS TempNtHeader = {0};
	if (pFileName == NULL)
	{
		pTempPath = m_cFileName;
	}
	else
	{
		pTempPath = pFileName;
	}
	HANDLE hFile = CreateFile(pTempPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (!hFile || hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	//指向PE结构中的Dos头的e_lfanew项,从而得到Nt头的文件偏移。
	SetFilePointer(hFile, offsetof(IMAGE_DOS_HEADER, e_lfanew), NULL, FILE_BEGIN);
	DWORD dwTempRead = 0, dwReadInFactSize = 0;
	//读取得到Nt头的在文件中的偏移。
	BOOL bRead = ReadFile(hFile, &dwTempRead, sizeof(DWORD), &dwReadInFactSize, NULL);
	m_iDosSize = dwTempRead;
	if (!bRead || sizeof(DWORD) != dwReadInFactSize)
	{
		goto OVER;
	}
	//移动文件指针到PE结构的Nt头。
	SetFilePointer(hFile, dwTempRead, NULL, FILE_BEGIN);
	//读取得到Nt头的内容
	bRead = ReadFile(hFile, &TempNtHeader, sizeof(IMAGE_NT_HEADERS), &dwReadInFactSize, NULL);
	if (!bRead || sizeof(IMAGE_NT_HEADERS) != dwReadInFactSize)
	{
		goto OVER;
	}
	//内存对齐粒度
	m_iMemAlignment = TempNtHeader.OptionalHeader.SectionAlignment;
	//文件对齐粒度
	m_iFileAlignment = TempNtHeader.OptionalHeader.FileAlignment;
	//得到该PE文件加载到内存后的总大小
	lTempSize = AlignmentNum(TempNtHeader.OptionalHeader.ImageBase, m_iMemAlignment);
	//准备内存，保存按照Windows加载方式加载的PE数据
	m_pMemPointer = (char *)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, lTempSize);
	if (NULL == m_pMemPointer)
	{ 
		goto OVER;
	}
	//将文件指针指向文件首
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	//节的个数
	m_iSecNum = TempNtHeader.FileHeader.NumberOfSections;
	//节表头的大小
	m_iSecSize = sizeof(IMAGE_SECTION_HEADER) * m_iSecNum;
	//NT头的大小
	m_iNtSize = 4 + sizeof(IMAGE_FILE_HEADER) +
		TempNtHeader.FileHeader.SizeOfOptionalHeader;
	//PE头总大小=Dos头+Nt头+节表头
	m_iPeSize = m_iDosSize /*Dos 头的大小*/ + 
		m_iNtSize  /*Nt 头的大小*/ + 
		m_iSecSize /*SectionTable 头的大小*/ ;
	//读取PE头
	bRead = ReadFile(hFile, m_pMemPointer, m_iPeSize, &dwReadInFactSize, NULL);
	if (!bRead || m_iPeSize != dwReadInFactSize)
	{
		goto OVER;
	}

	//m_pPeNtHeader 指向PE结构的NT头
	m_pPeNtHeader = (PIMAGE_NT_HEADERS)((BYTE *)m_pMemPointer + m_iDosSize);
	//m_pSectionHeader 指向PE结构的节表头
	m_pPeSectionHeader = (PIMAGE_SECTION_HEADER)((BYTE *)m_pMemPointer + m_iDosSize + m_iNtSize);
	m_pPeSectionHeader = (PIMAGE_SECTION_HEADER)((BYTE *)&m_pPeNtHeader->OptionalHeader
		+ m_pPeNtHeader->FileHeader.SizeOfOptionalHeader);
	//循环读取各个节
	//并计算所有节加载后所需的内存大小
	for (i = 0; i < m_iSecNum; i++)
	{
		//将文件指针指向节的开始处
		SetFilePointer(hFile, m_pPeSectionHeader[i].PointerToRawData , NULL, FILE_BEGIN);
		//读取整个节的内容
		ReadFile(hFile, (BYTE *)m_pMemPointer + AlignmentNum(m_pPeSectionHeader[i].VirtualAddress, m_iMemAlignment),
			m_pPeSectionHeader[i].SizeOfRawData, &dwReadInFactSize, NULL);
		dwTempSize += AlignmentNum(m_pPeSectionHeader[i].Misc.VirtualSize, m_iMemAlignment);
	}
	//保存资源节
	m_Resource = GetAppointSection(IMAGE_DIRECTORY_ENTRY_RESOURCE);
	m_iAllSecMemSize = dwTempSize;
	bRet = TRUE;
OVER:
	CloseHandle(hFile);
	return bRet;
}

//封装加载文件到内存的函数
ERRORINFO CPack::LoadPE(TCHAR *pFileName)
{
	if (pFileName == NULL)
	{
		return PARAMETER_ERROR;
	}
	//保存文件名
	_tcscpy(m_cFileName, pFileName);
	//判断文件是否存在
	if (!IsFileExist())
	{
		return FILE_NOT_EXIST;
	}
	//判断是否为PE文件,是则继续,否则返回假
	if (!IsPeFile())
	{
		return NOT_PE;
	}
	//读取文件到内存
	if (!ReadFileToMem())
	{
		return LOAD_TO_MEM_ERROR;
	}
	//处理附加数据
	if (!DoSpilthData())
	{
		return DEAL_SPILTH_ERROR;
	}
	m_bLoadSuccess = TRUE;
	return SUCCESS;
}

//判断是否为DLL
BOOL CPack::IsDll()
{
	return IMAGE_FILE_DLL & m_pPeNtHeader->FileHeader.Characteristics
		? TRUE : FALSE;
}


//清除重定位信息
void CPack::ClearReloc()
{
	if (!IsDll())  //如果是EXE程序,还需要清除重定位数据.
	{
		DWORD dwRelocDirectoryOffset;//接受重定位的信息的RVA,
		DWORD dwRelocDirectorySize;  //接受重定位的信息的大小
		//获得重定位目录信息
		dwRelocDirectorySize = GetDataDirectoryInfo(IMAGE_DIRECTORY_ENTRY_BASERELOC, 
			dwRelocDirectoryOffset);
		//如果为空,则说明不存在重定位信息,直接返回
		if (dwRelocDirectoryOffset == NULL || dwRelocDirectorySize == 0)
		{
			return;
		}
		else //进行重定位信息的清除
		{
			//根据数据目录的RVA得到数据目录所属节表的指针
			DWORD dwSec = (DWORD)GetSectionPtrByRva(m_pPeNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
			//重定位目录单独成节
			if (((PIMAGE_SECTION_HEADER)dwSec)->VirtualAddress == dwRelocDirectoryOffset)
			{
				m_iSecNum--;  //总节数减一
				//所有节所占内存数要减去重定位节所占内存
				m_iAllSecMemSize -= AlignmentNum(((PIMAGE_SECTION_HEADER)dwSec)->Misc.VirtualSize, m_iMemAlignment);
				//将重定位节表头清除
				RtlZeroMemory((BYTE *)dwSec, sizeof(IMAGE_SECTION_HEADER));
			}
			else
			{
				//将所有重定位数据置0
				RtlZeroMemory((BYTE *)m_pMemPointer+dwRelocDirectoryOffset, dwRelocDirectorySize);
			}
		}
	}
	//保存重定位表的RVA
	m_dwRelocationTableRva = m_pPeNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress;
	//将重定位目录大小和偏移设为0.
	m_pPeNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress = 0;
	m_pPeNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size = 0;
	return;
}

//获得某目录信息所属节表,如果不存在该目录，则返回空节
IMAGE_SECTION_HEADER CPack::GetAppointSection(DWORD dwDataDirectory)
{
	IMAGE_SECTION_HEADER temp = {0};
	DWORD dwResourceDirectoryOffset;//接受目录信息的RVA,
	//获得目录信息的Rva
	GetDataDirectoryInfo(dwDataDirectory, dwResourceDirectoryOffset);
	//获得该Rva所属节的节表指针
	DWORD dwResourcePointer = (DWORD)GetSectionPtrByRva(dwResourceDirectoryOffset);
	for (unsigned int i = 0; i < m_iSecNum; i++)
	{
		//如果当前节表指针等于Rva所属节表指针,那么找到
		if (dwResourcePointer == (unsigned long)&m_pPeSectionHeader[i])
		{
			return m_pPeSectionHeader[i];
		}
	}
	return temp;
}

//用APlib压缩引擎进行数据压缩
PVOID CPack::CompressData(PVOID pSource, long lInLength, OUT long &lOutLenght)
{
	BYTE *packed, *workmem;
	/* allocate memory */
	if ((packed  = (BYTE *) malloc(aP_max_packed_size(lInLength))) == NULL ||
		(workmem = (BYTE *) malloc(aP_workmem_size(lInLength)))    == NULL)
	{
		return NULL;
	}
	lOutLenght = aP_pack(pSource, packed, lInLength, workmem, NULL, NULL);
	if (lOutLenght == APLIB_ERROR)
	{
		return NULL;
	}
	if (NULL != workmem)
	{
		free(workmem);
		workmem = NULL;
	}
	return packed;
}

//获得某目录信息所属节表的数组序号,如果不存在该目录，则返回-1
int CPack::GetSectionPos(DWORD dwDataDirectory)
{
	DWORD dwResourceDirectoryOffset;//接受某信息的RVA,
	//获得某信息的Rva
	GetDataDirectoryInfo(dwDataDirectory, dwResourceDirectoryOffset);
	//获得该Rva所属节的节表指针
	DWORD dwResourcePointer = (DWORD)GetSectionPtrByRva(dwResourceDirectoryOffset);
	unsigned int i = 0;
	for (; i < m_iSecNum; i++)
	{
		//如果当前节表指针等于Rva所属节表指针,那么找到
		if (dwResourcePointer == (unsigned long)&m_pPeSectionHeader[i])
		{
			break;
		}
	}
	if (i == m_iSecNum)
	{
		return -1;
	}
	return i;
}

//压缩节
void CPack::CompressSections()
{
	int iCompressRva  = 0;
	//得到最后一个节的地址,因为压缩节要放到正常节后面.
	int iTempLastSecRva = m_pPeSectionHeader[m_iSecNum-1].VirtualAddress;
	//得到最后一个节的大小，需要内存对齐
	int iTempLastSecSize = AlignmentNum(m_pPeSectionHeader[m_iSecNum-1].Misc.VirtualSize, m_iMemAlignment);
	//压缩节的起始地址
	iCompressRva = iTempLastSecRva + iTempLastSecSize;	


	int iCompressSecNum = m_iSecNum;
	//得到资源所在节的索引
	int iResourcePos = GetSectionPos(IMAGE_DIRECTORY_ENTRY_RESOURCE);
	//如果有资源，并且不要求压缩资源节，则被压缩节的个数要减一。
	if (iResourcePos != -1 && !m_bCompressResource)  
	{
		iCompressSecNum -= 1;
	}
	//为各个节的压缩信息准备内存
	m_pComSec = new CompessSection[iCompressSecNum];
	//找到资源所属节
	int iPos = 0;
	int j = 0;
	for (unsigned int i = 0; i < m_iSecNum; i++)
	{
		//如果是资源节则并且要求不压缩则直接跳过
		if (i == iResourcePos && !m_bCompressResource)
		{
			iPos++;
			continue;
		}
		else  
		{
			if (m_pPeSectionHeader[iPos].SizeOfRawData == 0)
			{   //如果是空节则跳过.
				iPos++;
				continue;
			}
			long lCompressSize = 0;
			PVOID pCompressData;
			PVOID pInData = (BYTE *)m_pMemPointer + m_pPeSectionHeader[iPos].VirtualAddress;
			pCompressData = CompressData(pInData, 
				m_pPeSectionHeader[iPos].Misc.VirtualSize,
				lCompressSize);
			//压缩数据指针
			m_pComSec[j].lpCompessData = pCompressData;
			//解压后的内存地址
			m_pComSec[j].VA = m_pPeSectionHeader[iPos].VirtualAddress;
			//	pComSec[i].Size = m_pPeSectionHeader[iPos].Misc.VirtualSize;
			//	int iTempComSize = m_pe->AlignmentNum(iCompressRva, m_pe->GetMemAlignment());
			//压缩数据加载后的内存地址
			m_pComSec[j].CompessVA = iCompressRva;
			//压缩数据的大小
			m_pComSec[j].CompessSize = lCompressSize;
			//得到下一个节压缩数据加载后的内存地址
			iCompressRva += AlignmentNum(lCompressSize, m_iMemAlignment);
			//释放压缩函数内分配的内存
			iPos++;
			j++;
		}
	}
}


//构建按照指定导入库和要导入的函数构造导入表
void CPack::BuildImportTable(DWORD dwRva, pImportInfo pImport, PBYTE pMem)
{ ////biaozhi rva ,pImportInfo ,chushihua
	//导入表各成员,第一个:函数名地址数组的地址
	//第四个dll名的地址,第五个IAT的地址
	//分别计算下列值:
	//1.导入表的大小
	DWORD ImportSize = 0;
	//导入表结构,导入几个dll就有几+1个成员，每个成员5个DWORD
	//因为要以全零结尾。
	ImportSize = (pImport->lDllNum+1) * 5 * sizeof(DWORD);

	//IAT的大小，有多少个函数就有多少+1(全零结尾)个IAT成员，每个成员是一个DWORD
	int iTempIAT = 0;
	//2.IAT的大小
	DWORD IatSize = 0;
	int i = 0;
	for (; i < pImport->lDllNum; i++)
	{
		iTempIAT += pImport->pDll_info[i].lFuncNum + 1;
	}
	//IAT的大小等于个数*单个成员大小
	IatSize = iTempIAT * sizeof(DWORD);
	//3.函数名地址数组大小等于IAT大小
	//下面开始完成整个导入表体。
/*
导入表结构：
Iat1s 00000000 Iat2s 00000000 ...
ImprotTable
Func1Addrs 00000000 Func2Addrs 00000000 ...
Func1Str 00 Func2Str 00 Dll1Str 00 Func1Str 00 Func2Str 00 Dll2Str ......
*/
	DWORD IatRva = dwRva;

	//the address of Func1Addrs Destination:to write IID
	DWORD FuncNameTableRva = dwRva + IatSize + ImportSize;

//there is memory so use these to write
	DWORD DllNameRva = dwRva + 2*IatSize + ImportSize;
	DWORD FuncsStrRva = dwRva + 2*IatSize + ImportSize;

//the following is the Temporary buffer
	BYTE * dwStringPos = pMem + 2*IatSize + ImportSize;//there need to write
	BYTE * dwIatPos = pMem;
	BYTE * dwImportTablePos = pMem + IatSize;
	BYTE * dwFuncNamesRvaPos = pMem + IatSize + ImportSize;
	for (i = 0; i < pImport->lDllNum; i++)
	{
		int j = 0;
		for (; j < pImport->pDll_info[i].lFuncNum; j++)
		{
			dwStringPos += 2;   //留出放置函数序号的位置
			int iTemp = strlen(pImport->pDll_info[i].pFunctionInfo[j].cFunctionName);
			//写入函数名
			memcpy(dwStringPos, pImport->pDll_info[i].pFunctionInfo[j].cFunctionName, iTemp);
			//写入IAT和函数名地址数据
			memcpy(dwIatPos, &FuncsStrRva, sizeof(DWORD));//写入Iat1s 中 值为 Func1Str的地址
			memcpy(dwFuncNamesRvaPos, &FuncsStrRva, sizeof(DWORD));//写入Func1Addrs 中 值为 Func1Str的地址
			//下一个函数的IAT ->Iat2s
			dwIatPos += sizeof(DWORD);	
			//下一个函数名字的Rva ->Func2Addrs
			dwFuncNamesRvaPos += sizeof(DWORD);	
			//下一个函数名字的Rva ->Func2Str
			FuncsStrRva += iTemp + 1/*(00结尾)*/+ 2/*序号*/;
			//下一个函数的位置
			dwStringPos += iTemp + 1/*(00结尾)*/;

			DllNameRva += iTemp + 1/*(00结尾)*/+ 2/*序号*/;
		}
		dwIatPos += sizeof(DWORD);  //一个全零结尾
		dwFuncNamesRvaPos += sizeof(DWORD);	//一个全零结尾	
		int iTemp = strlen(pImport->pDll_info[i].cDllName) + 1/*(00结尾)*/;
		//写入DLL名
		memcpy(dwStringPos, pImport->pDll_info[i].cDllName, iTemp);
		dwStringPos += iTemp; //越过DLL名
		FuncsStrRva += iTemp;
		//写入导入表
		//写入函数名数组表地址
		memcpy(dwImportTablePos, &FuncNameTableRva, sizeof(DWORD));
		//FuncNameTableRva += (j+1) * sizeof(DWORD);
		dwImportTablePos += 3 * sizeof(DWORD);//qian 3 tiao guo
		memcpy(dwImportTablePos, &DllNameRva, sizeof(DWORD));
		//DllNameRva += iTemp;
		dwImportTablePos += sizeof(DWORD);
		memcpy(dwImportTablePos, &IatRva, sizeof(DWORD));//the address of Iat1s write to FirstThunk
		//dwImportTablePos += sizeof(DWORD);
		//IatRva += (j+1) * sizeof(DWORD);
	}
}

//计算导入表的大小
DWORD CPack::ImportTableSize(pImportInfo pImport)
{
	DWORD ImportSize = 0;
	//导入表结构,导入几个dll就有几+1个成员，每个成员个DWORD
	//因为要以全零结尾。
	ImportSize = (pImport->lDllNum+1) * 5 * sizeof(DWORD);
	//IAT的大小，有多少个函数就有多少+1(全零结尾)个IAT成员，每个成员是一个DWORD
	int iTempIAT = 0;
	int iTempString = 0;
	for (int i = 0; i < pImport->lDllNum; i++)
	{
		iTempIAT += pImport->pDll_info[i].lFuncNum + 1;
		iTempString += strlen(pImport->pDll_info[i].cDllName) + 1/*(00结尾)*/;
		for (int j = 0; j < pImport->pDll_info[i].lFuncNum; j++)
		{
			iTempString += strlen(pImport->pDll_info[i].pFunctionInfo[j].cFunctionName) + 1/*(00结尾)*/ + 2/*序号*/;
		}
	}
	ImportSize += 2 *iTempIAT * sizeof(DWORD) + iTempString;
	//存放字符串的空间大小
	return ImportSize;
}

//解压代码数组
BYTE decode[] = 
"\x60\x8B\x74\x24\x24\x8B\x7C\x24\x28\xFC\xB2\x80\x8A\x06\x83\xC6\x01\x88\x07\x83\xC7\x01\xBB\x02\x00\x00\x00\x00\xD2\x75\x05\x8A"
"\x16\x46\x10\xD2\x73\xE6\x00\xD2\x75\x05\x8A\x16\x46\x10\xD2\x73\x4F\x31\xC0\x00\xD2\x75\x05\x8A\x16\x46\x10\xD2\x0F\x83\xDB\x00"
"\x00\x00\x00\xD2\x75\x05\x8A\x16\x46\x10\xD2\x11\xC0\x00\xD2\x75\x05\x8A\x16\x46\x10\xD2\x11\xC0\x00\xD2\x75\x05\x8A\x16\x46\x10"
"\xD2\x11\xC0\x00\xD2\x75\x05\x8A\x16\x46\x10\xD2\x11\xC0\x74\x06\x89\xFB\x29\xC3\x8A\x03\x88\x07\x47\xBB\x02\x00\x00\x00\xEB\x9B"
"\xB8\x01\x00\x00\x00\x00\xD2\x75\x05\x8A\x16\x46\x10\xD2\x11\xC0\x00\xD2\x75\x05\x8A\x16\x46\x10\xD2\x72\xEA\x29\xD8\xBB\x01\x00"
"\x00\x00\x75\x28\xB9\x01\x00\x00\x00\x00\xD2\x75\x05\x8A\x16\x46\x10\xD2\x11\xC9\x00\xD2\x75\x05\x8A\x16\x46\x10\xD2\x72\xEA\x56"
"\x89\xFE\x29\xEE\xF3\xA4\x5E\xE9\x4F\xFF\xFF\xFF\x48\xC1\xE0\x08\x8A\x06\x46\x89\xC5\xB9\x01\x00\x00\x00\x00\xD2\x75\x05\x8A\x16"
"\x46\x10\xD2\x11\xC9\x00\xD2\x75\x05\x8A\x16\x46\x10\xD2\x72\xEA\x3D\x00\x7D\x00\x00\x83\xD9\xFF\x3D\x00\x05\x00\x00\x83\xD9\xFF"
"\x3D\x80\x00\x00\x00\x83\xD1\x00\x3D\x80\x00\x00\x00\x83\xD1\x00\x56\x89\xFE\x29\xC6\xF3\xA4\x5E\xE9\xFE\xFE\xFF\xFF\x8A\x06\x46"
"\x31\xC9\xC0\xE8\x01\x74\x17\x83\xD1\x02\x89\xC5\x56\x89\xFE\x29\xC6\xF3\xA4\x5E\xBB\x01\x00\x00\x00\xE9\xDD\xFE\xFF\xFF\x2B\x7C"
"\x24\x28\x89\x7C\x24\x1C\x61\xC3";

//外壳代码数组
BYTE code[] = 
"\x53\x50\x60\xE8\x00\x00\x00\x00\x58\x8B\xEC\x81\xEC\x00\x01\x00\x00\x89\x45\xF8\x8B\x50\xEC\x85\xD2\x74"
"\x1E\x8B\x4D\x2C\x89\x4D\x98\x83\xFA\x01\x75\x09\xC7\x40\xEC\x02\x00\x00\x00\xEB\x17\x83\xFA\x02\x53\x0F"
"\x84\x9C\x01\x00\x00\x64\x8B\x1D\x30\x00\x00\x00\x8B\x5B\x08\x89\x5D\x98\x8B\x40\xE0\x83\xC0\x20\x8B\x75"
"\xF8\x2B\xF0\x36\x8B\x0E\x85\xC9\x74\x21\x8B\x45\x98\x36\x03\x06\x50\x8B\x45\x98\x36\x03\x46\x04\x50\x8B"
"\x45\xF8\x2D\xEF\x01\x00\x00\xFF\xD0\x83\xC4\x08\x83\xC6\x08\xEB\xD8\x83\xC6\x04\x8B\x06\x89\x45\xA4\x83"
"\xC6\x04\x8B\x45\x98\x8B\x1E\x03\xD8\x8B\x7D\x98\x53\x83\xC3\x0C\x8B\x1B\x85\xDB\x74\x5A\x03\xDF\x53\x8B"
"\x45\xF8\xFF\x90\x5A\xFF\xFF\xFF\x8B\xF0\x5B\x53\x8B\x03\x85\xC0\x75\x05\x8B\x4B\x10\xEB\x02\x8B\x0B\x8B"
"\x5B\x10\x03\xCF\x03\xDF\x8B\x01\x85\xC0\x74\x2A\xA9\x00\x00\x00\x80\x75\x07\x03\xC7\x83\xC0\x02\xEB\x05"
"\x25\xFF\xFF\xFF\x7F\x51\x50\x56\x8B\x45\xF8\xFF\x90\x5E\xFF\xFF\xFF\x89\x03\x59\x83\xC1\x04\x83\xC3\x04"
"\xEB\xD0\x5B\x83\xC3\x14\xEB\x9C\x8D\x04\x24\x50\x6A\x04\x68\x00\x20\x00\x00\x57\x8B\x45\xF8\xFF\x90\x62"
"\xFF\xFF\xFF\x8B\xC7\x8B\x58\x3C\x03\xC3\x8B\x4D\xF8\x8B\x59\xE4\x89\x98\x88\x00\x00\x00\x8B\x59\xE8\x89"
"\x98\x8C\x00\x00\x00\x8B\x4D\xF8\x8B\x59\xF0\x8B\x45\x98\x03\xD8\x89\x5D\xEC\x8B\x45\xA4\x2B\xF8\x85\xFF"
"\x0F\x84\x97\x00\x00\x00\x89\x7D\xD4\x8B\x45\xEC\x83\x38\x00\x0F\x84\x88\x00\x00\x00\x8B\x45\xEC\x83\xC0"
"\x08\x89\x45\xC8\x8B\x45\xEC\x8B\x48\x04\x83\xE9\x08\xD1\xE9\x89\x4D\xBC\xC7\x45\x8C\x00\x00\x00\x00\xEB"
"\x09\x8B\x45\x8C\x83\xC0\x01\x89\x45\x8C\x8B\x45\x8C\x3B\x45\xBC\x73\x46\x8B\x45\x8C\x8B\x4D\xC8\x0F\xB7"
"\x14\x41\x89\x55\xB0\x8B\x45\xB0\x25\x00\xF0\x00\x00\x3D\x00\x30\x00\x00\x74\x02\xEB\xD1\x8B\x45\xB0\x25"
"\xFF\x0F\x00\x00\x89\x45\xB0\x8B\x45\xEC\x8B\x4D\x98\x03\x08\x03\x4D\xB0\x89\x4D\xB0\x8B\x45\xB0\x8B\x08"
"\x03\x4D\xD4\x8B\x55\xB0\x89\x0A\xEB\xA9\x8B\x45\xEC\x8B\x4D\xEC\x03\x48\x04\x89\x4D\xEC\xE9\x6C\xFF\xFF"
"\xFF\x5B\x8B\x45\xF8\x89\x45\x20\x8B\x5D\x98\x89\x5D\x24\x81\xC4\x00\x01\x00\x00\x61\x58\x5B\x03\x58\xF4"
"\xFF\xE3";
//用来保存解压参数的结构体
typedef struct Data 
{
	PBYTE pOut; //解压后存放的位置
	PBYTE pIn;  //解压数据所在位置
}SECDATA;

//生成加壳文件
ERRORINFO CPack::GetFile(TCHAR *pPathName)
{
	//一、是否可以生成加壳文件
	if(!m_bLoadSuccess)
	{
		return NOT_LOAD;
	}
	//判断文件是否已经加壳
	if (*(PBYTE)((PBYTE)m_pMemPointer + 3) == (BYTE)0x1)
	{
		return ALREADY_PACKED;
	}
	else
	{
		//写入加壳标记
		*(PBYTE)((PBYTE)m_pMemPointer + 3) = 0x1;
	}
	//二、处理重定位数据，EXE不需要，清除掉。DLL需要清除重定位目录。自己进行重定位。
	ClearReloc();
	//三、压缩各个节。
	CompressSections();
	//四、创建目标文件，并写入临时PE头。
	HANDLE hFile = CreateFile(pPathName, GENERIC_WRITE, 0, NULL, 
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (!hFile || hFile == INVALID_HANDLE_VALUE)
	{
		return CREATE_TARGET_FILE_ERROR;
	}
	//得到PE头的大小
	DWORD dwPeHeaderSize = 0;
	dwPeHeaderSize = AlignmentNum(m_iPeSize, m_iMemAlignment);
	DWORD dwWrite = 0;
	//写入PE头
	WriteFile(hFile, m_pMemPointer, dwPeHeaderSize, &dwWrite, NULL);
	//用于计算加壳后文件的镜像大小的变量
	DWORD dwImageSize = 0;
	//将PE头的大小加入到计算镜像大小的变量中
	dwImageSize += dwPeHeaderSize;
	//五.修正刚刚写入的PE头中的节表头
	//准备占位节信息
	char *pSectionName = "WBCPACK";
	IMAGE_SECTION_HEADER tempSec = {0};
	strcpy((char *)tempSec.Name, pSectionName);
	//占位节的内存起始RVA,紧随PE头之后
	tempSec.VirtualAddress = dwPeHeaderSize;
	if (m_Resource.PointerToRawData == NULL || m_bCompressResource)
	{
		tempSec.Misc.VirtualSize = m_iAllSecMemSize;
	}
	else
	{
		tempSec.Misc.VirtualSize = m_iAllSecMemSize - 
			AlignmentNum(m_Resource.Misc.VirtualSize, m_iMemAlignment);
	}
	//将占位节的内存大小加入到计算镜像大小的变量中
	dwImageSize += tempSec.Misc.VirtualSize;
	tempSec.PointerToRawData = dwPeHeaderSize;
	tempSec.SizeOfRawData = 0;
	tempSec.Characteristics = 0x80000000; //节属性为可写
	//将文件指针移动到节表头的起始处,即PE结构NT头的尾部
	DWORD dwMove = 0;
	dwMove = SetFilePointer(hFile, m_iNtSize + m_iDosSize, NULL, FILE_BEGIN);
	//写入节表头的第一个节头,即占位节头 
	WriteFile(hFile, &tempSec, sizeof(tempSec), &dwWrite, NULL);
	DWORD dwFileTemp = 0; //用于记录下一个节的文件偏移的变量
	dwFileTemp = dwPeHeaderSize; 
	int iPressSecNum =  m_iSecNum; //记录实际被压缩的节的个数
	//如果存在资源，并且不要求压缩资源节，则把资源节写在第二个节
	if (m_Resource.VirtualAddress != 0 && !m_bCompressResource)  
	{
		//写入节表头的第二个节头,即资源头
		strcpy((char *)tempSec.Name, (char *)m_Resource.Name);
		tempSec.VirtualAddress = m_Resource.VirtualAddress;
		tempSec.Misc.VirtualSize = m_Resource.Misc.VirtualSize;
		tempSec.PointerToRawData = dwPeHeaderSize;
		tempSec.SizeOfRawData = m_Resource.SizeOfRawData;
		tempSec.Characteristics = m_Resource.Characteristics;
		dwImageSize += AlignmentNum(tempSec.Misc.VirtualSize,m_iMemAlignment);
		//写入资源节头
		WriteFile(hFile, &tempSec, sizeof(tempSec), &dwWrite, NULL);
		dwFileTemp += AlignmentNum(m_Resource.SizeOfRawData, m_iFileAlignment);
		iPressSecNum -= 1;
	}
	//写入各个压缩节的节头
	int i = 0;
	for (; i < iPressSecNum; i++)
	{
		strcpy((char *)tempSec.Name, pSectionName);
		tempSec.VirtualAddress = m_pComSec[i].CompessVA;
		tempSec.Misc.VirtualSize = m_pComSec[i].CompessSize;
		tempSec.PointerToRawData = dwFileTemp;
		tempSec.SizeOfRawData = AlignmentNum(m_pComSec[i].CompessSize, 
			m_iFileAlignment);
		tempSec.Characteristics = 0x80000000; //节属性为可写
		dwImageSize += AlignmentNum(tempSec.Misc.VirtualSize,m_iMemAlignment);
		WriteFile(hFile, &tempSec, sizeof(tempSec), &dwWrite, NULL);
		dwFileTemp += AlignmentNum(m_pComSec[i].CompessSize, m_iFileAlignment);
	}
	//六.写入配置信息节的节头
	/*
	配置节的长度：
	1.解压参数		  PressSecNum * 8 + 4  字节
	2.基址			  4 字节
	3.导入表RVA       4 字节
	4.解压代码        0x149 字节           DecompressCodeSize
	5.导入表		  0x**  字节           ImportTableSize
	6.相对长度        4 字节               RelativeNum
	7.资源信息        8 字节               ResourceInfoSize 
	8.DLL信息         4字节
	9.重定位表RVA     4字节
	10.oep(RVA)       4 字节               OepSize
	11.外壳代码	      0x** 字节						
	*/
	//统计配置节的长度
	DWORD dwConfigSize = 0;
	//构造外壳导入表
	pImportInfo pInfo = new IMPORT_INFO;
	pInfo->lDllNum = 1;
	pInfo->pDll_info = new DLL_INFO[1];
	pInfo->pDll_info[0].pFunctionInfo = new FUNC_INFO[3];
	strcpy(pInfo->pDll_info[0].cDllName, "kernel32.dll");
	pInfo->pDll_info[0].lFuncNum = 3;
	strcpy(pInfo->pDll_info[0].pFunctionInfo[0].cFunctionName, "LoadLibraryA");
	strcpy(pInfo->pDll_info[0].pFunctionInfo[1].cFunctionName, "GetProcAddress");
	strcpy(pInfo->pDll_info[0].pFunctionInfo[2].cFunctionName, "VirtualProtect");
	DWORD dwRva = (iPressSecNum + 1) * m_iMemAlignment;
	//写入配置空间节头
	//得到最后一个压缩节Rva
	DWORD dwLastCompessVA = m_pComSec[iPressSecNum-1].CompessVA;
	//最后一个压缩解的大小
	DWORD dwLastCompessSize = m_pComSec[iPressSecNum-1].CompessSize;
	DWORD dwLastSectionMemSize = dwLastCompessVA + AlignmentNum(dwLastCompessSize, 
		m_iMemAlignment);
	DWORD dwConfigSecStart = dwLastSectionMemSize;
	DWORD dwSecParamSize = sizeof(SECDATA)*(iPressSecNum);
	//计算导入表的大小
	int iImportTableSize = ImportTableSize(pInfo);	
	dwConfigSize = dwSecParamSize + 4/*解压参数结束标志*/ + 4/*基址*/ + 
		4/*导入表RVA*/ + sizeof(decode)/*解压代码大小*/ + iImportTableSize
		+ 4/*相对值*/ + 8/*资源信息*/+ 4/*DLL信息*/ + 4/*重定位表RVA*/+ 4/*OEP*/+
		sizeof(code)/*外壳代码大小*/;
	strcpy((char *)tempSec.Name, pSectionName);
	tempSec.VirtualAddress = dwLastSectionMemSize;
	tempSec.Misc.VirtualSize = dwConfigSize;  //?
	tempSec.PointerToRawData = dwFileTemp; 
	tempSec.SizeOfRawData = dwConfigSize;    //因为是最后一个节，可以不用对齐
	tempSec.Characteristics = 0xE0000020;   //配置空间可写
	dwImageSize += AlignmentNum(tempSec.Misc.VirtualSize,m_iMemAlignment);
	WriteFile(hFile, &tempSec, sizeof(tempSec), &dwWrite, NULL);
	//七.写入各个节数据。
	//将文件指针移动到节开始。
	dwMove = SetFilePointer(hFile, dwPeHeaderSize, NULL, FILE_BEGIN);

	//如果含有资源节,并且资源节没要求压缩,那么将资源节数据写入PE头后
	DWORD dwMoveFact = dwPeHeaderSize;
	if (m_Resource.SizeOfRawData != NULL && !m_bCompressResource)
	{
		//写入资源节
		WriteFile(hFile, (PVOID)((BYTE *)m_pMemPointer + m_Resource.VirtualAddress), 
			m_Resource.SizeOfRawData, &dwWrite, NULL);
		//资源节的文件对齐大小
		DWORD dwResourceSize = AlignmentNum(m_Resource.SizeOfRawData,
			m_iFileAlignment);
		dwMove = SetFilePointer(hFile, dwPeHeaderSize + dwResourceSize, NULL, FILE_BEGIN);
		//文件头+资源节的文件对齐大小
		dwMoveFact += dwResourceSize;
	}
	//写入各个压缩节数据
	for (i = 0; i < iPressSecNum; i++)
	{	
		WriteFile(hFile, m_pComSec[i].lpCompessData, 
			m_pComSec[i].CompessSize, &dwWrite, NULL);
		dwMoveFact += AlignmentNum(m_pComSec[i].CompessSize,
			m_iFileAlignment);
		dwMove = SetFilePointer(hFile, dwMoveFact, NULL, FILE_BEGIN);
	}
	//
	//八.完成最后一个节也就是配置节
	/*
	配置节的长度：
	1.解压参数		  PressSecNum * 8 + 4  字节
	2.基址			  4 字节
	3.导入表RVA       4 字节
	4.解压代码        0x149 字节           DecompressCodeSize
	5.导入表		  0x**  字节           ImportTableSize
	6.相对长度        4 字节               RelativeNum
	7.资源信息        8 字节               ResourceInfoSize 
	8.DLL信息         4字节
	9.重定位表RVA     4字节
	10.oep(RVA)       4 字节               OepSize
	11.外壳代码	      0x** 字节						
	*/
	//用于记录配置节前五部分的总长度的变量
	DWORD dwSize = 0;
	//为解压参数分配内存,并且进行赋值.
	SECDATA *pSecDatas = new SECDATA[iPressSecNum];
	for (i = 0; i < iPressSecNum; i++)
	{
		pSecDatas[i].pIn = (PBYTE)m_pComSec[i].CompessVA;
		pSecDatas[i].pOut = (PBYTE)m_pComSec[i].VA;
	}
	//写入解压时需要的数据指针及存放地址
	WriteFile(hFile, pSecDatas, 
		sizeof(SECDATA)*(iPressSecNum), &dwWrite, NULL);
	if (pSecDatas)
	{
		delete []pSecDatas;
	}
	dwSize += sizeof(SECDATA)*(iPressSecNum);
	//写入解压参数结束标志
	DWORD dwOverFlat = 0;
	WriteFile(hFile, &dwOverFlat, 4, &dwWrite, NULL);
	dwSize += 4;
	DWORD dwImageBase = m_pPeNtHeader->OptionalHeader.ImageBase;
	//写入优先加载基址
	WriteFile(hFile, &dwImageBase, 4, &dwWrite, NULL);
	dwSize += 4;
	DWORD dwImportVa = m_pPeNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
	//写入导入表地址
	WriteFile(hFile, &dwImportVa, 4, &dwWrite, NULL);
	dwSize += 4;
	//写入解压缩代码
	WriteFile(hFile, &decode, sizeof(decode), &dwWrite, NULL);
	dwSize += sizeof(decode);

	//构造外壳程序使用的导入表
	BYTE *pImportTable = new BYTE[iImportTableSize];
	memset(pImportTable, 0, iImportTableSize);
	DWORD dwImportRVA = dwConfigSecStart  + dwSecParamSize + 4/*解压参数结束标志*/ + 4/*基址*/ + 
		4/*导入表RVA*/ + sizeof(decode)/*解压代码大小*/;
	BuildImportTable(dwImportRVA ,pInfo, pImportTable);//biaozhi rva ,pImportInfo ,chushihua
	//写入导入表 
	WriteFile(hFile, pImportTable, iImportTableSize, &dwWrite, NULL);
	dwSize+= iImportTableSize;//用于记录配置节前五部分的总长度的变量
	delete []pImportTable;
	//写入相对长度
	WriteFile(hFile, &dwSize, 4, &dwWrite, NULL);
	//写入资源信息
	//********************* new 写入原始资源目录RVA和资源目录大小 **************
	DWORD dwResRva = m_pPeNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress;
	WriteFile(hFile, &dwResRva, 4, &dwWrite, NULL);
	DWORD dwResSize = m_pPeNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].Size;
	WriteFile(hFile, &dwResSize, 4, &dwWrite, NULL);
	//********************* new 写入原始资源目录RVA和资源目录大小 **************
	//写入DLL标志
	DWORD dwDllFlag = 0;
	if(IsDll())
	{
		dwDllFlag = 1;
	}
	WriteFile(hFile, &dwDllFlag, 4, &dwWrite, NULL);
	//写入重定位目录RVA
	WriteFile(hFile, &m_dwRelocationTableRva, 4, &dwWrite, NULL);
	//写oep
	DWORD dwOep = m_pPeNtHeader->OptionalHeader.AddressOfEntryPoint;
	WriteFile(hFile, &dwOep, 4, &dwWrite, NULL);
	//写入外壳代码
	WriteFile(hFile, &code, sizeof(code), &dwWrite, NULL);
	//九.修正pe头
	//得到加壳后的入口
	DWORD dwEntryPoint = dwConfigSecStart +  dwSecParamSize + 4/*解压参数结束标志*/ + 4/*基址*/ + 
		4/*导入表RVA*/ + sizeof(decode)/*解压代码大小*/ + iImportTableSize
		+ 4/*相对值*/ + 8/*资源信息*/+ 4/*DLL信息*/ + 4/*重定位表RVA*/ + 4/*OEP*/;
	//将程序入口改为外壳代码的入口
	m_pPeNtHeader->OptionalHeader.AddressOfEntryPoint = dwEntryPoint;
	//得到新导入表的RVA地址
	int iIatSize = 0;
	for (i=0; i < pInfo->lDllNum; i++)
	{
		iIatSize += pInfo->pDll_info[i].lFuncNum + 1;
	}
	DWORD dwImportAdd = dwConfigSecStart  + dwSecParamSize + 4/*解压参数结束标志*/ + 4/*基址*/ + 
		4/*导入表RVA*/ + sizeof(decode)/*解压代码大小*/+ iIatSize * sizeof(DWORD);//IID rva
	//更新加壳后程序的导入表地址
	m_pPeNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress = dwImportAdd;
	m_pPeNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size = iImportTableSize;

	//更新节个数
	m_pPeNtHeader->FileHeader.NumberOfSections = m_iSecNum + 2;
	//更新镜像大小
	m_pPeNtHeader->OptionalHeader.SizeOfImage = dwImageSize;
	dwMove = SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	//将更新后的PE头写入文件
	WriteFile(hFile, m_pMemPointer, m_iDosSize + m_iNtSize, &dwWrite, NULL);
	CloseHandle(hFile);
	if(m_bCompressResource && !IsDll())
	{
		UpdateIconAndVersion(m_cFileName, pPathName);
	}
	//写入附加数据
	if (m_pSplithData)
	{
		hFile = CreateFile(pPathName, GENERIC_WRITE, 0, NULL, 
			OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		dwMove = SetFilePointer(hFile, 0, NULL, FILE_END);
		WriteFile(hFile, m_pSplithData, m_dwSplitDataSize, &dwWrite, NULL);
		CloseHandle(hFile);
	}
	for (i=0; i<pInfo->lDllNum; i++)
	{
		delete []pInfo->pDll_info[i].pFunctionInfo;
	}
	delete []pInfo->pDll_info;
	delete []pInfo;
	return SUCCESS;
}
