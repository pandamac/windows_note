// GetKernelBaseAddr.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>

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

int _tmain(int argc, _TCHAR* argv[])
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
	PLDR_MODULE pLdrModule = (PLDR_MODULE)pLdrData->InLoadOrderModuleList. Flink;
	PLDR_MODULE pNext = pLdrModule; //获得保存模块信息链表头
	wchar_t *pWKernel32 = L"kernel32.dll";
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
			if (pNext->FullDllName.Buffer[iPos+i+1] == pWKernel32 [i])
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
	if (pKernelBaseMem != NULL)
	{
		printf("kernel32模块基址为：%x", pKernelBaseMem);
	}
	system("pause");
	return 0;
}

/*

		;find kernel32.dll
		find_kernel32:
		push esi
		xor ecx, ecx
		mov esi, fs:[ecx+0x30]
		mov esi, [esi + 0x0c]
		mov esi, [esi + 0x1c]
		next_module:
		mov eax, [esi + 0x8]
		mov edi,[esi+0x20]
		mov esi ,[esi]
		cmp [edi+12*2],cx
		jne next_module
		pop esi
			Ret
*/