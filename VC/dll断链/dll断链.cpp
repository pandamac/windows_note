// dll断链.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>

typedef struct _UNICODE_STRING { 
	USHORT Length; 
	USHORT MaximumLength; 
	PWSTR  Buffer;                 //注意，这里为Unicode类型
} UNICODE_STRING, *PUNICODE_STRING;
// _PEB_LDR_DATA声明
typedef struct _PEB_LDR_DATA
{
    DWORD Length; // +0x00
    bool Initialized; // +0x04
    PVOID SsHandle; // +0x08
    LIST_ENTRY InLoadOrderModuleList; // +0x0c
    LIST_ENTRY InMemoryOrderModuleList; // +0x14
    LIST_ENTRY InInitializationOrderModuleList;// +0x1c
} PEB_LDR_DATA,*PPEB_LDR_DATA; // +0x24
typedef struct _LDR_DATA_TABLE_ENTRY
{
    LIST_ENTRY          InLoadOrderModuleList;
    LIST_ENTRY          InMemoryOrderModuleList;
    LIST_ENTRY          InInitializationOrderModuleList;
    void*               BaseAddress;
    void*               EntryPoint;  
    ULONG               SizeOfImage;
    UNICODE_STRING		 FullDllName;
    UNICODE_STRING      BaseDllName;
    ULONG               Flags;
    SHORT               LoadCount;
    SHORT               TlsIndex;
    HANDLE              SectionHandle;
    ULONG               CheckSum;
    ULONG               TimeDateStamp;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;



void Chainscission()
{
	PPEB_LDR_DATA pldr;
	PLDR_DATA_TABLE_ENTRY pldr_data;

	PLIST_ENTRY list_entry,list_entry2;

	__asm
	{
		xor eax,eax
		mov eax,fs:[0x30]	//ppeb
		mov eax,[eax+0x0c]	//pldr
		mov pldr,eax
	}
	list_entry2 = list_entry = pldr->InLoadOrderModuleList.Flink;

	do 
	{
		//根据一个结构体实例中的成员的地址，取到整个结构体实例的地址
		pldr_data = CONTAINING_RECORD(list_entry,_LDR_DATA_TABLE_ENTRY,InLoadOrderModuleList);
	//pLdrData = (PLDR_DATA_TABLE_ENTRY)pCur;
		if (pldr_data->BaseAddress == LoadLibrary("kernel32.dll"))
		{
			pldr_data->InInitializationOrderModuleList.Blink->Flink = pldr_data->InInitializationOrderModuleList.Flink;
			pldr_data->InInitializationOrderModuleList.Flink->Blink = pldr_data->InInitializationOrderModuleList.Blink;

			pldr_data->InLoadOrderModuleList.Blink->Flink = pldr_data->InLoadOrderModuleList.Flink;
			pldr_data->InLoadOrderModuleList.Flink->Blink = pldr_data->InLoadOrderModuleList.Blink;

			pldr_data->InMemoryOrderModuleList.Blink->Flink = pldr_data->InMemoryOrderModuleList.Flink;
			pldr_data->InMemoryOrderModuleList.Flink->Blink = pldr_data->InMemoryOrderModuleList.Blink;

		}
		list_entry = list_entry->Flink;

	} while (list_entry2 != list_entry);
	
}
int main(int argc, char* argv[])//exe去断链
{
	printf("进程PID：%d",GetCurrentProcessId());
	Chainscission();
	MessageBox(NULL,"进程断链","进程断链",MB_OK);
	return 0;
}

