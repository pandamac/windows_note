#include <windows.h>
#include <stdio.h>
#include <psapi.h>
#pragma comment( lib, "PSAPI.LIB" )


DWORD WINAPI WatchThread(LPVOID lparam);
DWORD WINAPI remote(LPVOID pvparam);
BOOL EnablePriv();


DWORD processtopid(char *processname);


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

	EnablePriv();

	int procID=processtopid("notepad.exe");
	printf("The Explorer pid is %d\n",procID);


	HINSTANCE         hkernel32;

    HANDLE            ethread;
	HANDLE            rphandle;
	char             *remotethr;
	char             *remotepar;
	int               cb;

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
	strcpy(rp.path,"c:\\pp.exe");	


	cb=sizeof(char)*sizeof(rp);
	remotepar=(PTSTR)VirtualAllocEx(rphandle,NULL,cb,MEM_COMMIT,PAGE_READWRITE);
	if(remotepar==NULL)
	{
		printf("VirtualAllocEx for Parameter Error\n");
		CloseHandle(rphandle);
	}

	if(WriteProcessMemory(rphandle,remotepar,(LPVOID)&rp,cb,NULL)==FALSE)
	{
		printf("WriteProcessMemory for Parameter Error:");
		CloseHandle(rphandle);
	}



	/*****************************************************************/
					/*将远程线程注入目标进程*/
	/*****************************************************************/

	
	ethread=CreateRemoteThread(rphandle,NULL,0,(LPTHREAD_START_ROUTINE)remotethr,(LPVOID)remotepar,0,NULL);
	
	if(ethread==NULL)
	{
		printf("CreateRemoteThread Error\n");
		CloseHandle(rphandle);
	}
	else
		printf("CreateRemoteThread is ok\n");


	WaitForSingleObject(ethread,INFINITE);

	return 0;
}




DWORD processtopid(char *processname)
{
	DWORD    lpidprocesses[1024],cbneeded,cprocesses;
	HANDLE   hprocess;
	HMODULE  hmodule;
	UINT     i;
	TCHAR    normalname[MAX_PATH]=("UnknownProcess");
    
	if(!EnumProcesses(lpidprocesses,sizeof(lpidprocesses),&cbneeded))
	{
		return -1;  
	}
	cprocesses=cbneeded/sizeof(DWORD);
	for(i=0;i<cprocesses;i++)
	{
		hprocess=OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,FALSE,lpidprocesses[i]);
		if(hprocess)
		{
			if(EnumProcessModules(hprocess,&hmodule,sizeof(hmodule),&cbneeded))
			{
				GetModuleBaseName(hprocess,hmodule,normalname,sizeof(normalname));
				if(!strcmp(normalname,processname))  
				{
					CloseHandle(hprocess);
					return (lpidprocesses[i]);
				}
			}
		}
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


	rp->rpProcessHandle=tOpenProcess(PROCESS_ALL_ACCESS,FALSE,rp->rpProcessPID);

	tWaitForSingleObject(rp->rpProcessHandle,INFINITE);
	
	tWinExec(rp->path, SW_SHOW);
	return 0;
}
