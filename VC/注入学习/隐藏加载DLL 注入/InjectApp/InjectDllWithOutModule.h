#pragma once
#include <Windows.h>
#include <stddef.h>
#include <assert.h>

struct _InjectData 
{
	char cLoad[13];                      //保存LoadLibraryA字符串
	char cGetAddr[15];                   //保存GetProcAddress字符串
	char cKernel32[13];                  //保存kernel32.dll字符串
	wchar_t wcKernel32[13];              //保存kernel32.dll宽字节字符串
	DWORD dwImageBase;
};

class CInjectDllWithOutModule
{
public:
	CInjectDllWithOutModule(const char * pName = NULL);
	~CInjectDllWithOutModule(void);
	//按照PE加载器的加载方式进行加载被注入的DLL到内存。
	bool LoadDll(const char * pName = NULL, BOOL bReadOnly = FALSE);
	//判断是否是DLL
	BOOL IsDll(const char * pName);
	BOOL InjectDllAndRun(DWORD dwProcessID, const char * pName);
private:
	//修正重定位数据
	void RelocateImage(DWORD dwRelocOffset);
	DWORD AlignmentNum(DWORD dwOperateNum, DWORD dwAlignment);
	//获得导出函数的RVA
	DWORD GetExportFuncAddr(const char *pFuncName);
	LPVOID m_pMem;   //保存本地加载DLL后的内存基址
	char m_cName[MAX_PATH]; //保存被加载的DLL的完整路径
	DWORD m_dwImageSize;    //保存DLL被加载后所占内存总大小
	BOOL m_bReadOnly;       //是否以只读的方式加载
};
