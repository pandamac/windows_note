//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
// 日期:	2007/08/08																		//
// 作者:	冷风																			//
// 说明:	十分惭愧本程序参照TOo2y哥的很多代码,如果有问题可以通过QQ给我联系:121121606		//
// 使用:	1.运行行记事本2.运行程序3.关闭程序试试会发现程序自动重起程序插入到notepad.exe进程//
//////////////////////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "PSAPI.H"
#pragma comment( lib, "PSAPI.LIB" )

HANDLE	CreateRemoteThreadProc(char* ProcessName);
DWORD	WINAPI WatchThread(LPVOID lparam);
DWORD	WINAPI remote(LPVOID pvparam);
DWORD	processtopid(char *processname);
BOOL	EnablePriv();
BOOL CreateNotepadProcess();

typedef struct _remoteparameter
{
	DWORD		rpWaitForSingleObject;
	DWORD		rpOpenProcess;
	DWORD       rpWinExec;
	DWORD	    rpProcessPID;           
	HANDLE		rpProcessHandle;
	char	    path[MAX_PATH];
}REMOTEPARAM;


int main(int argc, char* argv[])
{
	HANDLE RemoteThreadHandle;
	HANDLE LocateThreadHandle;
	

	EnablePriv();
	
	
	RemoteThreadHandle=CreateRemoteThreadProc("notepad.exe");
	
	LocateThreadHandle=CreateThread(NULL,NULL,WatchThread,(LPVOID)RemoteThreadHandle,NULL,NULL);
	
	WaitForSingleObject(LocateThreadHandle,INFINITE);
	
	//	WaitForSingleObject(RemoteThreadHandle,INFINITE);
	
	return 0;
}

BOOL CreateNotepadProcess()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si,sizeof(si));
	si.cb=sizeof(si);
	si.dwFlags=STARTF_USESHOWWINDOW;
	si.wShowWindow=SW_HIDE;
	ZeroMemory(&pi,sizeof(pi));
	
	BOOL flag = CreateProcess(NULL,"notepad.exe",NULL,NULL,FALSE,0,NULL,NULL,&si,&pi);
	if (!flag)
	{
		printf("错误代码: %d",GetLastError());
		return 0;
	}
	return 0;
}

HANDLE CreateRemoteThreadProc(char* ProcessName)
{
	HANDLE	ThreadHandle;
	char	FilePath[MAX_PATH];
	
	GetModuleFileName(NULL,FilePath,MAX_PATH);//得到文件所在路径
	
	int procID=processtopid(ProcessName);
	printf("The process pid is %d\n",procID);
	
	
	HINSTANCE         hkernel32;
	HANDLE            rphandle;
	char             *remotethr;
	char             *remotepar;
	int               cb;
	//Enables using the process handle in the CreateRemoteThread function to create a thread in the process.
	//Enables using the process handle in the VirtualProtectEx and WriteProcessMemory functions to modify the virtual memory of the process.
	//Enables using the process handle in the WriteProcessMemory function to write to the virtual memory of the process.
	rphandle=OpenProcess(PROCESS_CREATE_THREAD |     
		PROCESS_VM_OPERATION  |     
		PROCESS_VM_WRITE,           
		FALSE,procID);
	if(rphandle==NULL)
	{
		printf("Open Remote Process  is Error\n");
	}
	else
	{
		printf("open process is ok\n");
	}
	
	/*****************************************************************/
	/*将远程线程函数代码拷入目标进程*/
	/*****************************************************************/
	
	cb=sizeof(TCHAR)*4*1024;
	
	remotethr=(PTSTR)VirtualAllocEx(rphandle,NULL,cb,MEM_COMMIT,PAGE_EXECUTE_READWRITE); 
	if(remotethr==NULL)
	{
		printf("VirtualAllocEx for Thread Error\n");
		CloseHandle(rphandle);       
	}
	else
		printf("VirtualAllocEx is ok\n");
	
	
	if(WriteProcessMemory(rphandle,remotethr,(LPVOID)remote,cb,NULL)==FALSE)
	{
		printf("WriteProcessMemory for Thread Error\n");
		CloseHandle(rphandle);
	}
	else
		printf("WriteProcessMemory is ok\n");
	
	/*****************************************************************/
	/*将远程线程函数参数拷入目标进程*/
	/*这里需要重定位远程线程需要的API*/
	/*****************************************************************/
	
	REMOTEPARAM rp;
	memset((char*)&rp,0,sizeof(rp));
	
	hkernel32=GetModuleHandle("kernel32.dll");
	
	if(hkernel32==NULL)
	{
		printf("hKernel32 is Error\n");
	}
	
	rp.rpProcessPID			=GetCurrentProcessId();
	rp.rpOpenProcess		=(DWORD)GetProcAddress(hkernel32,"OpenProcess");
	rp.rpWinExec			=(DWORD)GetProcAddress(hkernel32,"WinExec");
	rp.rpWaitForSingleObject=(DWORD)GetProcAddress(hkernel32,"WaitForSingleObject");
	_tcscpy(rp.path,FilePath);	
	
	
	cb=sizeof(char)*sizeof(rp);
	remotepar=(PTSTR)VirtualAllocEx(rphandle,NULL,cb,MEM_COMMIT,PAGE_READWRITE);
	if(remotepar==NULL)
	{
		printf("VirtualAllocEx for Parameter Error\n");
		CloseHandle(rphandle);
	}
	
	if(WriteProcessMemory(rphandle,remotepar,(LPVOID)&rp,cb,NULL)==FALSE)
	{
		printf("WriteProcessMemory for Parameter Error\n");
		CloseHandle(rphandle);
	}
	
	
	
	/*****************************************************************/
	/*将远程线程注入目标进程*/
	/*****************************************************************/
	
	
	ThreadHandle=CreateRemoteThread(rphandle,NULL,0,(LPTHREAD_START_ROUTINE)remotethr,(LPVOID)remotepar,0,NULL);
	
	if(ThreadHandle==NULL)
	{
		printf("CreateRemotThreadHandle Error\n");
		CloseHandle(rphandle);
	}
	else
		printf("CreateRemotThreadHandle is ok\n");
	
	return ThreadHandle;
}








DWORD processtopid(char *processname)
{
	DWORD    lpidprocesses[1024],cbneeded,cprocesses;
	HANDLE   hprocess;
	HMODULE  hmodule;
	UINT     i;
	TCHAR    normalname[MAX_PATH]=("UnknownProcess");
    int flag= 0;

	if(!EnumProcesses(lpidprocesses,sizeof(lpidprocesses),&cbneeded))
	{//得到PID 数组
		return -1;  
	}
	cprocesses=cbneeded/sizeof(DWORD);
	for(i=0;i<cprocesses;i++)
	{
		hprocess=OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,FALSE,lpidprocesses[i]);
		if(hprocess)//Enables using the process handle in the ReadProcessMemory function to read from the virtual memory of the process.
		{
			if(EnumProcessModules(hprocess,&hmodule,sizeof(hmodule),&cbneeded))
			{
				GetModuleBaseName(hprocess,hmodule,normalname,sizeof(normalname));
				if(!strcmp(normalname,processname))  
				{
					CloseHandle(hprocess);
					flag = 1;
					return (lpidprocesses[i]);
				}
			}
		}
	}
	if (0 == flag)
	{
		CreateNotepadProcess();
		processtopid("notepad.exe");
	}
	CloseHandle(hprocess);
	return 0;
}

BOOL EnablePriv()
{
	HANDLE hToken;
	if ( OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&hToken) )
	{
		TOKEN_PRIVILEGES tkp;
		
		LookupPrivilegeValue( NULL,SE_DEBUG_NAME,&tkp.Privileges[0].Luid );	//修改进程权限
		tkp.PrivilegeCount=1;
		tkp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges( hToken,FALSE,&tkp,sizeof tkp,NULL,NULL );	//通知系统修改进程权限
	}
	return 0;
}



DWORD WINAPI remote(LPVOID pvparam)
{
	REMOTEPARAM *rp=(REMOTEPARAM*)pvparam;
	
	typedef UINT			(WINAPI *EWinExec)				(LPCSTR, UINT);
	typedef HANDLE			(WINAPI *EOpenProcess)			(DWORD, BOOL, DWORD);
	typedef DWORD			(WINAPI *EWaitForSingleObject)	(HANDLE, DWORD);
	
	
	EWinExec				tWinExec;
	EOpenProcess			tOpenProcess;
	EWaitForSingleObject	tWaitForSingleObject;
	
	
	tOpenProcess			=(EOpenProcess)rp->rpOpenProcess;
	tWaitForSingleObject	=(EWaitForSingleObject)rp->rpWaitForSingleObject;
	tWinExec				=(EWinExec)rp->rpWinExec;
	
	
	rp->rpProcessHandle = tOpenProcess(PROCESS_ALL_ACCESS,FALSE,rp->rpProcessPID);
	
	tWaitForSingleObject(rp->rpProcessHandle,INFINITE);
	
	tWinExec(rp->path, SW_SHOW);
	return 0;
}

DWORD	WINAPI WatchThread(LPVOID lparam)
{
	HANDLE	RemoteThreadHandle=(HANDLE)lparam;
	DWORD	ExitCode;
	
	
	
	while(true)
	{
		GetExitCodeThread(RemoteThreadHandle,&ExitCode);
		if(ExitCode!=STILL_ACTIVE)//检查线程是否真正结束了
		{
			printf("RemoteThreadHandle is over\n");
			RemoteThreadHandle=CreateRemoteThreadProc("notepad.exe");
		}
		Sleep(3000);	
	}
	return 0;
}
