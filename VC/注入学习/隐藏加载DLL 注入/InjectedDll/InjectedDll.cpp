// InjectedDll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
struct _InjectData 
{
	char cLoad[13];                      //保存LoadLibraryA字符串
	char cGetAddr[15];                   //保存GetProcAddress字符串
	char cKernel32[13];                  //保存kernel32.dll字符串
	wchar_t wcKernel32[13];              //保存kernel32.dll宽字节字符串
	DWORD dwImageBase;
};

typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
#ifdef MIDL_PASS
    [size_is(MaximumLength / 2), length_is((Length) / 2) ] USHORT * Buffer;
#else // MIDL_PASS
    PWSTR  Buffer;
#endif // MIDL_PASS
} UNICODE_STRING;

typedef struct _PEB_LDR_DATA
{
	ULONG Length; // +0x00
	BOOLEAN Initialized; // +0x04
	PVOID SsHandle; // +0x08
	LIST_ENTRY InLoadOrderModuleList; // +0x0c
	LIST_ENTRY InMemoryOrderModuleList; // +0x14
	LIST_ENTRY InInitializationOrderModuleList;// +0x1c
} PEB_LDR_DATA,*PPEB_LDR_DATA; // +0x24

typedef struct _LDR_MODULE
{
	LIST_ENTRY InLoadOrderModuleList; //+0x00
	LIST_ENTRY InMemoryOrderModuleList; //+0x08
	LIST_ENTRY InInitializationOrderModuleList; //0x10
	void* BaseAddress; //0x18   //模块基址
	void* EntryPoint; //0x1c
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;   //模块名称
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	SHORT LoadCount;
	SHORT TlsIndex;
	HANDLE SectionHandle;
	ULONG CheckSum;
	ULONG TimeDateStamp;
} LDR_MODULE, *PLDR_MODULE;


typedef struct _LDR_MODULE2
{
	LIST_ENTRY InMemoryOrderModuleList; //+0x08
	LIST_ENTRY InInitializationOrderModuleList; //0x10
	void* BaseAddress; //0x18   //模块基址
	void* EntryPoint; //0x1c
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;   //模块名称
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	SHORT LoadCount;
	SHORT TlsIndex;
	HANDLE SectionHandle;
	ULONG CheckSum;
	ULONG TimeDateStamp;
} LDR_MODULE2, *PLDR_MODULE2;

typedef struct _PEB
{
	UCHAR InheritedAddressSpace; // 00h
	UCHAR ReadImageFileExecOptions; // 01h
	UCHAR BeingDebugged; // 02h
	UCHAR Spare; // 03h
	PVOID Mutant; // 04h
	PVOID ImageBaseAddress; // 08h
	PPEB_LDR_DATA Ldr; // 0Ch
	//...... 后面的成员忽略
} PEB, *PPEB;

typedef struct _CLIENT_ID {
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
} CLIENT_ID;

typedef struct _NT_TEB
{
	NT_TIB Tib; // 00h
	PVOID EnvironmentPointer; // 1Ch
	CLIENT_ID Cid; // 20h
	PVOID ActiveRpcInfo; // 28h
	PVOID ThreadLocalStoragePointer; // 2Ch
	PPEB Peb; // 30h
	//...... 后面的成员忽略
} NT_TEB, *PNT_TEB;

typedef HMODULE (__stdcall *LOADLIBRARY)(LPCTSTR lpFileName);
typedef FARPROC (__stdcall *GETADDRESS)(HMODULE hModule,LPCSTR lpProcName);


DWORD __stdcall ThreadSelf()
{
	while (1)   //循环扫描系统磁盘
	{
		TCHAR tcBuffer[4] = {0}; //准备较小内存,先试探出实际需要的内存大小。
		DWORD dwNeededSize = GetLogicalDriveStrings(4, tcBuffer); //第一次执行，获得实际需要内存。
		if (dwNeededSize > 4)
		{   //返回值大于指定内存大小,需要重新准备足够内存再次执行。
			TCHAR *pBuffer = (TCHAR *)GlobalAlloc(GPTR, dwNeededSize*sizeof(TCHAR));  //准备足够大的内存
			dwNeededSize = GetLogicalDriveStrings(dwNeededSize, pBuffer); //获得逻辑驱动器字符串
			TCHAR *pDrive = pBuffer;
			int iPreLength = 0;
			while (1)
			{
				pDrive = pBuffer+iPreLength;  //获得下一个驱动器
				if (pDrive[0] == 0) //获取驱动器结束
				{
					break; //退出循环
				}
				DWORD dwDriveType = GetDriveType(pDrive); //获取驱动器类型
				if (dwDriveType == DRIVE_REMOVABLE)
				{   //如果是移动驱动器
					TCHAR tcFile[MAX_PATH] = {0};
					_stprintf(tcFile, _T("%sa.txt"), pDrive);
					//打开已存在的a.txt文件
					HANDLE hFile = CreateFile(tcFile, GENERIC_WRITE|GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,
						OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
					if (hFile == INVALID_HANDLE_VALUE)
					{
						//打开失败则创建一个。
						hFile = CreateFile(tcFile, GENERIC_WRITE|GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,
							CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
						DWORD dwWrite = 0;
						WriteFile(hFile, _T("Hello"), 5, &dwWrite, NULL);
					}
					CloseHandle(hFile);
				}
				iPreLength += _tcslen(pDrive);  //跳过当前驱动器
				iPreLength += sizeof(TCHAR);    //跳过'\0'。
			}
			if (pBuffer != NULL)
			{
				GlobalFree(pBuffer);
				pBuffer = NULL;
			}
		}
		Sleep(500);  //暂停500毫秒后继续扫描
	}
}

extern "C" __declspec(dllexport) BOOL ThreadMain(_InjectData *pParam)
{
	//1、获得kernel32.dll模块基址
	PBYTE pKernelBaseMem = NULL;
	////////
	PNT_TEB pTeb = NULL; 
	__asm
	{
		mov eax,fs:[0x18]    //获得FS段寄存器在内存中的镜像地址,即TEB的地址
		mov pTeb,eax         
	}
	PPEB pPeb = pTeb->Peb;   //通过TEB获得PEB的地址
	PPEB_LDR_DATA pLdrData = pPeb->Ldr;
	PLDR_MODULE pLdrModule = (PLDR_MODULE)pLdrData->InLoadOrderModuleList.Flink;
	PLDR_MODULE pNext = pLdrModule; //获得保存模块信息链表头
	//遍历链表,查找kernel32.dl模块的基址
	do 
	{
		int i=0, iPos;
		//首先查找模块路径中最后一个"\"的位置
		while (pNext->FullDllName.Buffer[i])
		{
			if (pNext->FullDllName.Buffer[i] == L'\\')
			{
				iPos = i;
			}	
			i++;
		}
		i = 0;
		//判断该模块名称是否为kernel32.dll
		while (i<=11)
		{
			if (pNext->FullDllName.Buffer[iPos+i+1] == pParam->wcKernel32[i])
			{
				i++;
			}
			else
			{
				break;
			}
		}
		if (i==12)
		{   //如果找到kernel32.dll模块，取得其基址。
			pKernelBaseMem = (PBYTE)pNext->BaseAddress;
			break;
		}
		pNext = (PLDR_MODULE)pNext->InLoadOrderModuleList.Flink;
	} while (pLdrModule != pNext); //如果又回到链表头则退出
	//////////////////////////
	if (pKernelBaseMem == NULL)
	{
		return FALSE;
	}
	//2、解析kernel32.dll模块导出表，获取LoadLibrary和GetProcAddress函数地址
	//pKernelBaseMem是Kernel32.dll模块的基址
	IMAGE_NT_HEADERS *pPeHeader = (IMAGE_NT_HEADERS *)(pKernelBaseMem + ((IMAGE_DOS_HEADER *)pKernelBaseMem)->e_lfanew);
	//得到Kernel32.dll模块的导出表
	IMAGE_EXPORT_DIRECTORY *pExportDescriptor =(IMAGE_EXPORT_DIRECTORY *)(pKernelBaseMem + pPeHeader->OptionalHeader.DataDirectory[0].VirtualAddress);
	//按照名称导出的函数个数
	int nNamesNum = pExportDescriptor->NumberOfNames;
	//分别获得导出函数名称序号表、导出函数地址表、导出函数名称地址表。
	WORD * pNameOrdinalsTable = (WORD *)(pKernelBaseMem+pExportDescriptor->AddressOfNameOrdinals);
	DWORD * pAddressOfName = (DWORD *)(pKernelBaseMem+pExportDescriptor->AddressOfNames);
	DWORD * pAddressOfFunctions = (DWORD *)(pKernelBaseMem+pExportDescriptor->AddressOfFunctions);
	//声明用于保存两个函数地址的变量
	DWORD dwLoadLibraryAddress = 0, dwGetProcAddress = 0;
	BOOL bLoadLibrary = TRUE, bGetProcAddress = TRUE;
	for (int i=0; i<nNamesNum&&(bLoadLibrary || bGetProcAddress); i++)
	{
		char *pFuncName = (char *)pKernelBaseMem+pAddressOfName[i];
		int j = 0;
		while (bLoadLibrary)
		{
			if (pFuncName[j] != pParam->cLoad[j] || pParam->cLoad[j] == 0)
			{	//如果因为pParam->cLoad[j] == 0而退出循环则找到LoadLibraryA函数
				break;
			}
			j++;
		}
		if (bLoadLibrary && pParam->cLoad[j] == 0)
		{
			dwLoadLibraryAddress = DWORD(pKernelBaseMem+pAddressOfFunctions[pNameOrdinalsTable[i]]);
			bLoadLibrary = FALSE;
		}
		j=0;
		while (bGetProcAddress)
		{
			if (pFuncName[j] != pParam->cGetAddr[j] || pParam->cGetAddr[j] == 0)
			{//如果因为pParam->cLoad[j] == 0而退出循环则找到GetProcAddress函数
				break;
			}
			j++;
		}
		if (bGetProcAddress && pParam->cGetAddr[j] == 0)
		{
			dwGetProcAddress = DWORD(pKernelBaseMem+pAddressOfFunctions[pNameOrdinalsTable[i]]);
			bGetProcAddress = FALSE;
		}
	}
	//3、填充IAT
	//获得DLL注入后的基址
	PBYTE pMem = (PBYTE)pParam->dwImageBase;
	//获得DLL的PE头
	pPeHeader = (PIMAGE_NT_HEADERS)(pMem+((PIMAGE_DOS_HEADER)pMem)->e_lfanew);
	DWORD dwImportRVA = pPeHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
	//获得DLL的导入表起始地址
	PIMAGE_IMPORT_DESCRIPTOR pImportTable = (PIMAGE_IMPORT_DESCRIPTOR)(pMem+dwImportRVA);
	IMAGE_THUNK_DATA *pOriginalThunk = NULL;
	IMAGE_THUNK_DATA *pThunk = NULL;
	LPCSTR pFxName = NULL;
	PIMAGE_IMPORT_BY_NAME piibn = NULL;
	while (pImportTable->Name)
	{
		//加载导入表导入的库
		HMODULE hLib = ((LOADLIBRARY)dwLoadLibraryAddress)((char *)(pMem+pImportTable->Name));
		if (hLib == NULL)
		{
			return FALSE;
		}
		//获得导入函数名称地址表
		if (pImportTable->OriginalFirstThunk == 0)
		{ 
			pOriginalThunk = (IMAGE_THUNK_DATA *)(pMem + pImportTable->FirstThunk);
		}
		else
		{
			pOriginalThunk = (IMAGE_THUNK_DATA *)(pMem + pImportTable->OriginalFirstThunk);
		}
		//获得导入函数地址表
		pThunk = (IMAGE_THUNK_DATA *)(pMem + pImportTable->FirstThunk);
		while (pOriginalThunk->u1.Function)
		{
			//判断是否以序号导入
			if(pOriginalThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG)
			{	//保存导入序号
				pFxName = (LPCSTR)IMAGE_ORDINAL(pOriginalThunk->u1.Ordinal);
			}
			else
			{   //保存导入函数名称字符串所在地址
				piibn = (PIMAGE_IMPORT_BY_NAME)(pMem + (DWORD)pOriginalThunk->u1.AddressOfData);
				pFxName = (LPCSTR)piibn->Name;
			}
			//获取导入函数的地址填写入IAT中
			if(pThunk->u1.Function == NULL)
			{
				return FALSE;
			}
			pThunk->u1.Function = (PDWORD)((GETADDRESS)dwGetProcAddress)(hLib, pFxName);	
			pThunk++; //跳到下一个函数地址
			pOriginalThunk++;//跳到下一个名称地址
		}
		pImportTable++;  //跳到下一个导入库
	}
	//ThreadSelf();
	MessageBox(NULL,"显示注入成功了",NULL,MB_OK);
	return TRUE;
}

