/*
 *                                 SSDT操作程序导出函数
 *                                     Made By Adly
 *
 *                                     QQ:369046978
 *
 */
/////////////////////////////////
//SSDT.h
///////////////////////////////////////////////////
#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include "..\\IOCTL.h"

#pragma comment( lib, "shlwapi" )

#define		EXE_DRIVER_NAME		_T("SSDT")
#define		DISPLAY_NAME		_T("SSDT Driver")
#define		SystemModuleInfo	0x0B
//#define		QuerySize			0xFA00

typedef ULONG \
(__stdcall *pNtQuerySystemInformationProto)( \
			IN ULONG SysInfoClass,
			IN OUT PVOID SystemInformation,
			IN ULONG SystemInformationLength,
			OUT PULONG nRet
			);

#pragma pack( push, 1 )
typedef struct _SSDTEntry
{
	BYTE byMov;		//B8 => MOV EAX, XXXX
	ULONG ulIndex;	//Service Number
} SSDTEntry, *pSSDTEntry;
#pragma pack( pop )

typedef struct _SYSTEM_MODULE_INFORMATION { 
    ULONG Reserved[2]; 
    PVOID Base; 
    ULONG Size; 
    ULONG Flags; 
    USHORT Index; 
    USHORT Unknown; 
    USHORT LoadCount; 
    USHORT ModuleNameOffset; 
    CHAR ImageName[256]; 
} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;

typedef struct _tagSysModuleList {
    ULONG ulCount;
    SYSTEM_MODULE_INFORMATION smi[1];
} SYSMODULELIST, *PSYSMODULELIST;

typedef struct _SSDTSaveTable		//保存SSDT信息
{
	ULONG ulServiceNumber;				//服务号
	ULONG ulCurrentFunctionAddress;		//当前函数地址
	ULONG ulOriginalFunctionAddress;	//原始函数地址
	char ServiceFunctionName[MAX_PATH+1];	//服务函数名
	char ModuleName[MAX_PATH+1];		//模块名
} SSDTSaveTable, *pSSDTSaveTable;
///////////////////////////////////////////////////
//              函数声明
///////////////////////////////////////////////////
//加载驱动
HANDLE LoadDriver( IN LPCTSTR lpFileName );
//卸载驱动
void UnloadDriver( IN HANDLE hDriver );
//得到SSDT
BOOL GetSSDT( IN HANDLE hDriver, OUT PSSDT ssdt );
//设置SSDT
BOOL SetSSDT( IN HANDLE hDriver, IN PSSDT ssdt );
//得到SSDT表中HOOK的地址
BOOL GetHook( IN HANDLE hDriver, IN ULONG ulIndex, OUT PULONG ulAddr );
//设置SSDT表中HOOK的地址
BOOL SetHook( IN HANDLE hDriver, IN ULONG ulIndex, IN OUT PULONG ulAddr );
//枚举SSDT
BOOL EnumSSDT( IN HANDLE hDriver );
//恢复SSDT
BOOL ReSSDT( IN HANDLE hDriver );
//恢复SSDT并去掉非系统表
BOOL ReSSDTAndThrowSpilth( IN HANDLE hDriver );

///////////////////////////////////////////////////