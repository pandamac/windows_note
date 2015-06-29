// MyPack.h: interface for the CMyPack class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYPACK_H__6F27A006_7B3E_48E6_ACF1_10CA1C050210__INCLUDED_)
#define AFX_MYPACK_H__6F27A006_7B3E_48E6_ACF1_10CA1C050210__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <Windows.h>
#include <string>
#include <tchar.h>
#include "Include/aplib.h"
#include <fstream>
#pragma comment(lib, "lib/aplib.lib")
using namespace std;
/*
*定义压缩后的PE节的结构
*VA:压缩前(或解压后)内存地址
*CompessVA:压缩后内存地址
*lpCompessData:压缩数据指针
*CompessSize:压缩数据大小
*/
typedef struct _CompessSection
{
	DWORD VA;             //压缩前(或解压后)内存地址
	DWORD CompessVA;      //压缩数据加载后的内存地址
	DWORD CompessSize;    //压缩后数据大小
	LPVOID lpCompessData; //压缩数据指针
}CompessSection, *PCompessSection;
//导入表中保存函数信息的结构体
typedef struct _FUNC_INFO
{
	TCHAR cFunctionName[256]; //导入函数名
	long lOrdinal;            //导入函数序号
	DWORD *pdwRva;            //导入函数地址
	_FUNC_INFO()
	{
		lOrdinal = 0;
		pdwRva = NULL;
		memset(cFunctionName, '\0', 256);
	}
}*pFuncInfo, FUNC_INFO;
//结构体中保存DLL信息的结构
typedef struct _DLL_INFO
{
	TCHAR cDllName[256];     //导入库名
	pFuncInfo pFunctionInfo; //从本DLL中导入的函数信息
	long lFuncNum;           //从本DLL中导入的函数个数
	_DLL_INFO()
	{
		pFunctionInfo = NULL;
		lFuncNum = 0;
		memset(cDllName, '\0', 256);
	}
}*pDllInfo, DLL_INFO;
//导入表中保存整个导入表信息的结构
typedef struct _IMPORT_INFO
{
	pDllInfo pDll_info;    //导入表中导入的所有库的信息
	long lDllNum;          //导入表中导入的库的个数
}*pImportInfo, IMPORT_INFO;

enum ERRORINFO{SUCCESS, FILE_NOT_EXIST, NOT_PE, LOAD_TO_MEM_ERROR, DEAL_SPILTH_ERROR, 
PARAMETER_ERROR, CREATE_TARGET_FILE_ERROR, ALREADY_PACKED, NOT_LOAD};

//该类声明如下：
class CPack  
{
public:
	CPack(BOOL bCompressResource = TRUE);
	~CPack();
public:
	//读取被加壳的程序文件按照PE加载器加载方式读到内存
	ERRORINFO LoadPE(TCHAR *pFileName);
	//生成加壳文件
	ERRORINFO GetFile(TCHAR *pPathName);

protected:
	//判断文件是否存在
	BOOL IsFileExist(char * strFileName = NULL);
	//判断被加壳程序是否为PE文件。
	BOOL IsPeFile(char *pFileName = NULL);
	//按照PE加载的方式(内存对齐粒度对齐方式)将文件读取到内存
	BOOL ReadFileToMem(TCHAR *pFileName = NULL);
	//保存附加数据
	BOOL DoSpilthData(TCHAR *pFileName = NULL);
	//清空重定位信息并保存重定位表RVA
	void ClearReloc();
	//判断文件是否为DLL。此壳不支持DLL文件加壳，如果是DLL则返回假。
	//压缩数据, 
	//注本压缩壳采用aplib压缩算法,使用的时候应该包含其头文件,并导入其静态库,如下: 
	//#include "aplib.h"
	//#pragma comment(lib, "aplib.lib")
	//压缩数据函数,返回被压缩后数据所在内存的指针
	PVOID CompressData(PVOID pSource, long lInLength, OUT long &lOutLenght);
	//压缩所有节
	void CompressSections();
	BOOL IsDll();
	//获得目录表信息函数,返回目录表大小,参数dwDataDirectoryOffset传出目录表的RVA
	DWORD GetDataDirectoryInfo(DWORD dwDataDirectory, DWORD &dwDataDirectoryOffset);
	//以dwAlignment 对齐dwOperateNum 值,也就是让dwOperateNum为dwAlignment的整数倍
	DWORD AlignmentNum(DWORD dwOperateNum, DWORD dwAlignment);
	//打造导入表
	void BuildImportTable(DWORD dwRva, pImportInfo pImport, PBYTE pMem);
	//计算指定要打造的导入表的大小
	DWORD ImportTableSize(pImportInfo pImport);
	//获得指定RVA所属节的指针
	IMAGE_SECTION_HEADER *GetSectionPtrByRva(DWORD dwRva);
	//获取指定目录所在节的索引
	int GetSectionPos(DWORD dwDataDirectory);
	//根据指定目录获得目录所属节信息
	IMAGE_SECTION_HEADER GetAppointSection(DWORD dwDataDirectory);
private:
	TCHAR *m_pMemPointer ;  //PE文件读到内存后的地址

	LPVOID m_pSplithData; //指向附加数据所在内存的指针
	DWORD m_dwSplitDataSize; //附加数据的大小
	TCHAR m_cFileName[256]; //PE文件的完整路径名
	IMAGE_DOS_HEADER *m_pPeDosHeader; //指向Dos头的指针
	IMAGE_NT_HEADERS *m_pPeNtHeader; //指向PE头的指针	
	IMAGE_SECTION_HEADER *m_pPeSectionHeader;	//指向节表头的指针
	int m_iAllSecMemSize; //所有节加载后内存后占的大小
	unsigned int m_iSecSize; //PE结构节表的总大小
	unsigned int m_iDosSize; //PE结构Dos头及Dos实体的大小
	unsigned int m_iNtSize; //PE结构Nt头的大小
	unsigned int m_iPeSize; //PE结构PE头的总大小

	int m_iMemAlignment;	//PE结构内存对齐粒度
	int m_iFileAlignment;	//PE结构文件对齐粒度	
	unsigned int m_iSecNum; //PE结构节的个数
	bool m_bLoadSuccess;    //是否成功加载
	IMAGE_SECTION_HEADER m_Resource; //保存资源节信息
	BOOL m_bCompressResource;  //是否压缩资源节
	CompessSection *m_pComSec; //压缩节指针
	int m_iComSecNum;          //压缩节个数
	DWORD m_dwRelocationTableRva; //重定位表的RVA
};


#endif // !defined(AFX_MYPACK_H__6F27A006_7B3E_48E6_ACF1_10CA1C050210__INCLUDED_)
