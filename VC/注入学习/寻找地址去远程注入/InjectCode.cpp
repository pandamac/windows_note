// InjectCode.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>


struct _RemoteData 
{
	char cLoad[13];                      //保存LoadLibraryA字符串
	char cGetAddr[15];                   //保存GetProcAddress字符串
	char cUser32[11];                    //保存user32.dll字符串
	char cKernel32[13];                  //保存kernel32.dll字符串
	char cMessageBox[12];                //保存MessageBoxA字符串
	char cGetModuleFileName[19];         //保存GetModuleFileNameA字符串
	wchar_t wcKernel32[13];              //保存kernel32.dll宽字节字符串
};
typedef int (__stdcall *MESSAGEBOX)(HWND hWnd,LPCTSTR lpText,LPCTSTR lpCaption,UINT uType);
typedef DWORD (__stdcall *GETMODULEFILENAME)(HMODULE hModule,LPTSTR lpFilename,DWORD nSize);
typedef HMODULE (__stdcall *LOADLIBRARY)(LPCTSTR lpFileName);
typedef FARPROC (__stdcall *GETADDRESS)(HMODULE hModule,LPCSTR lpProcName);


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

static  void  WINAPI RemoteThread(_RemoteData *pParam)
{
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
		return ;
	}
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
	DWORD dwOrdinale = 0; //用于保存导出序号表中对应的序号。
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
			dwOrdinale = pNameOrdinalsTable[i];
			dwLoadLibraryAddress = pAddressOfFunctions[dwOrdinale];
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
			dwOrdinale = pNameOrdinalsTable[i];
			dwGetProcAddress = pAddressOfFunctions[dwOrdinale];
			bGetProcAddress = FALSE;
		}
	}
	LOADLIBRARY myLoadLib = (LOADLIBRARY)(pKernelBaseMem+dwLoadLibraryAddress);
	GETADDRESS myGetAddr = (GETADDRESS)(pKernelBaseMem+dwGetProcAddress);
	char cBuffer[128] = {0};
	((GETMODULEFILENAME)myGetAddr(myLoadLib(pParam->cKernel32), pParam->cGetModuleFileName))(NULL, cBuffer, 128);
	((MESSAGEBOX)myGetAddr(myLoadLib(pParam->cUser32), pParam->cMessageBox))(0,cBuffer,0,0);
}
static void AfterThread(){}

int main(int argc, char* argv[])
{
	void *pFun = NULL, *pParam = NULL;
	_RemoteData node;
	HANDLE hThread;
	strcpy(node.cLoad, "LoadLibraryA");
	strcpy(node.cGetAddr, "GetProcAddress");
	strcpy(node.cUser32, "user32.dll");
	strcpy(node.cMessageBox, "MessageBoxA");
	strcpy(node.cGetModuleFileName, "GetModuleFileNameA");
	strcpy(node.cKernel32, "kernel32.dll");
	wcscpy(node.wcKernel32, L"kernel32.dll");
	LPBYTE iBack = (LPBYTE) AfterThread;
	LPBYTE iBof = (LPBYTE) RemoteThread;
	int cbCodeSize;
	if (iBof > iBack)
	{
		cbCodeSize = (iBof - iBack);
	}
	else
	{
		cbCodeSize = (iBack - iBof);
	}	
	DWORD dwProcessID;
	printf("请输入要注入的进程ID：");
	scanf("%d", &dwProcessID);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, dwProcessID);
	if (hProcess == NULL)
	{
		printf("该进程无法注入！");
		return -1;
	}
	//在远程进程空间分配内存,从而接受要执行的代码
	pFun = VirtualAllocEx(hProcess, 0, cbCodeSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE );
	if (pFun == NULL)
	{
		printf("该进程无法注入！");
		return -1;
	}
	BOOL bSuccess;
	//拷贝要执行的代码到远程进程空间
	bSuccess = WriteProcessMemory(hProcess, pFun, RemoteThread, cbCodeSize, NULL);
	if (!bSuccess)
	{
		printf("该进程无法注入！");
		return -1;
	}
	pParam = VirtualAllocEx(hProcess, 0, sizeof(_RemoteData), MEM_COMMIT, PAGE_EXECUTE_READWRITE );
	if (pParam == NULL)
	{
		printf("该进程无法注入！");
		return -1;
	}
	bSuccess = WriteProcessMemory(hProcess, pParam, &node, sizeof(_RemoteData), NULL);
	if (!bSuccess)
	{
		printf("该进程无法注入！");
		return -1;
	}
	//创建远程进程,并执行拷贝过去的代码
	hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pFun, 
		pParam, 0, NULL);
	//等待远程进程代码执行完毕返回
	WaitForSingleObject(hThread ,INFINITE);
	//释放远程进程中分配的内存
	VirtualFreeEx(hProcess, pParam, 0, MEM_RELEASE);
	VirtualFreeEx(hProcess, pFun, 0, MEM_RELEASE);
	//关闭进程句柄
	CloseHandle(hProcess);
	//关闭线程句柄
	CloseHandle(hThread);
	return 0;
}

