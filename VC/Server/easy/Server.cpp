// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <tchar.h>
#include <process.h>


void Install();
void UnInstall();
BOOL IsInstall();

void Run();
void WINAPI ServiceMain(DWORD dwArgc,LPTSTR *lpszArgv);
void WINAPI Handler(DWORD fdwControl);

SERVICE_STATUS g_status;
SERVICE_STATUS_HANDLE service_status_handle;
SERVICE_DESCRIPTION description;

HANDLE g_hEvent = NULL;

#define lpServiceName _T("First Server")
#define lpDisplayName _T("First Server show!")

int main(int argc, char* argv[])
{

	g_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	g_status.dwCurrentState = SERVICE_STOPPED;
	// 设置服务可以使用的控制
	// 如果希望服务启动后不能停止，去掉SERVICE_ACCEPT_STOP
	// SERVICE_ACCEPT_PAUSE_CONTINUE是服务可以“暂停/继续”
	
	g_status .dwCheckPoint = 0;
	g_status.dwControlsAccepted = \
		SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN |\
		SERVICE_ACCEPT_PAUSE_CONTINUE;
	g_status.dwServiceSpecificExitCode = 0;
	g_status.dwWaitHint = 0;
	g_status.dwWin32ExitCode = 0;

	//创建初始为有信号的手动内核事件。
	g_hEvent = CreateEvent(NULL,TRUE,TRUE,"Pause");
	
	if (argv[1] != NULL && _tcsicmp(argv[1],_T("Install")) == 0)
	{
		Install();
	}
	else if(argv[1] != NULL && _tcsicmp(argv[1],_T("UnInstall")) == 0)
	{
		UnInstall();
	}
	else
	{
		SERVICE_TABLE_ENTRY lpServiceTable[]=
		{
			{lpServiceName,ServiceMain},
			{NULL,NULL}
		};
		StartServiceCtrlDispatcher(lpServiceTable);
	}
	CloseHandle(g_hEvent);
//	system("pause");
	return 0;
}
BOOL IsInstall()
{
	BOOL bResult = FALSE;
	
	SC_HANDLE sc_handle = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if (sc_handle != NULL)
	{
		SC_HANDLE sc_hService = OpenService(sc_handle,\
			lpServiceName,\
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
void Install()
{
	if (IsInstall())
	{
		return ;
	}
	SC_HANDLE sc_managerhandle = OpenSCManager(NULL,NULL,\
		SC_MANAGER_ALL_ACCESS);
	if (sc_managerhandle == NULL)
	{
		_tprintf(_T("OpenSCManager错误！"));
		return;
	}
	TCHAR szFilePath[MAX_PATH];
    DWORD dwLength = GetModuleFileName(NULL,szFilePath,MAX_PATH);
	//装载一个程序实例的句柄。如果该参数为NULL，
   //该函数返回该当前应用程序全路径	
	
  //判断程序路径是否包含空格,如果包含则给路径加上引号.
   if (_tcschr(szFilePath, ' ') != NULL)
   {
	   dwLength += 3;
	   TCHAR *lpFilePath = new TCHAR[dwLength];
	   if (lpFilePath != NULL)
	   {
		   _stprintf(lpFilePath, _T("\"%s\""), szFilePath);
		   _tcscpy(szFilePath, lpFilePath);
		   delete []lpFilePath;
	   }
	}
	//创建一个手动启动的服务 
	SC_HANDLE sc_handle = CreateService(sc_managerhandle,\
		lpServiceName,\
		lpDisplayName,\
		SERVICE_ALL_ACCESS,SERVICE_WIN32_OWN_PROCESS,
		SERVICE_AUTO_START,SERVICE_ERROR_IGNORE,\
		szFilePath,NULL,NULL,NULL,NULL,NULL);
	if (sc_handle == NULL)
	{
		CloseServiceHandle(sc_managerhandle);
			_tprintf("CreateService错误！");

		//return ;
	}


	//创建失败
    if(sc_handle==NULL)
    {		
        DWORD dwErrorCode = GetLastError();
        if(dwErrorCode!=ERROR_SERVICE_EXISTS)
        {
            _tprintf("CreateService Error !\n");
            CloseServiceHandle(sc_handle);
			return ;
        }
		//如果服务存在
        else
        {
			//假如服务已存在则打开服务
            sc_handle=OpenService(sc_managerhandle,\
				lpServiceName,SERVICE_START); //服务名
			//打开失败
            if(sc_handle==NULL)
            {
				_tprintf("OpenService Error !\n");              
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
        if(dwErrorCode == ERROR_SERVICE_ALREADY_RUNNING)
        {
			_tprintf("StartService Error !\n");
			CloseServiceHandle(sc_managerhandle);  
			CloseServiceHandle(sc_handle);
			return ;
        }
    } 
	//查询服务状态
    while(QueryServiceStatus(sc_handle,&g_status)!=0)           
    {   //服务是否在初始化阶段
        if(g_status.dwCurrentState==SERVICE_START_PENDING)
        {
            Sleep(100);
        }
        else
        {
            break;
        }
    }
	//查询服务状态，看有没有启动成功
    if(g_status.dwCurrentState!=SERVICE_RUNNING)
    {
        _tprintf("Install service Failed\n");
    }
    else
    {
		_tprintf("Install service Successed\n");
    }

	CloseServiceHandle(sc_handle);
	CloseServiceHandle(sc_managerhandle);
	//system("pause");
	return ;
}
void UnInstall()
{
	if (!IsInstall())
	{
		_tprintf(_T("UnInstall错误！"));
		return ;
	}

	SC_HANDLE sc_handle = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if (sc_handle == NULL)
	{
		_tprintf("UnInstall OpenSCManager 失败" );
		return ;
	}
    SC_HANDLE sc_service = OpenService(sc_handle,lpServiceName,DELETE | \
		SERVICE_STOP);
	if (sc_service == NULL)
	{
		CloseServiceHandle(sc_handle);
		return ;
	}
	SERVICE_STATUS status;
	//首先停止服务,确保服务能够立即被删除.
	ControlService(sc_service,SERVICE_CONTROL_STOP,&status);

	BOOL bResult = DeleteService(sc_service);
    CloseServiceHandle(sc_handle);
    CloseServiceHandle(sc_service);
	if (!bResult)
	{
				_tprintf(_T("DeleteService错误！"));
	}
//	system("pause");
}
void WINAPI ServiceMain(DWORD dwArgc,LPTSTR *lpszArgv)
{
	service_status_handle = RegisterServiceCtrlHandler(lpServiceName,Handler);
	if (service_status_handle == NULL)
	{
		return ;
	}
	g_status.dwCurrentState = SERVICE_START_PENDING;
	SetServiceStatus(service_status_handle,&g_status);
	g_status.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(service_status_handle,&g_status);
	
	Run();
	
	g_status.dwWin32ExitCode = S_OK;
	g_status.dwCheckPoint = 0;
	g_status.dwWaitHint = 0;
	g_status.dwCurrentState = SERVICE_STOPPED;//not running
	
	//设置服务状态为停止，从而退出服务
	SetServiceStatus(service_status_handle,&g_status);
}
void WINAPI Handler(DWORD fdwControl)
{
	switch (fdwControl)
	{
	case SERVICE_CONTROL_STOP:
		//Notifies a service that it should stop. 
		{
			g_status.dwCurrentState = SERVICE_STOP_PENDING;
			SetServiceStatus(service_status_handle,&g_status);
			
			g_status.dwCurrentState = SERVICE_STOPPED;
			SetServiceStatus(service_status_handle,&g_status);
		}
		break;
	case SERVICE_CONTROL_PAUSE:
		//Notifies a service that it should pause. 
		{
			g_status.dwCurrentState = SERVICE_CONTINUE_PENDING;
			SetServiceStatus(service_status_handle,&g_status);
			ResetEvent(g_hEvent); //变为无信号//通知RUN函数开始等待
			g_status.dwCurrentState = SERVICE_PAUSED;
			SetServiceStatus(service_status_handle,&g_status);
		}	
		break;
	case SERVICE_CONTROL_CONTINUE:
		//Notifies a paused service that it should resume.
		{
			g_status.dwCurrentState = SERVICE_CONTINUE_PENDING;
			SetServiceStatus(service_status_handle,&g_status);
			SetEvent(g_hEvent);//通知RUN函数继续执行
			g_status.dwCurrentState = SERVICE_RUNNING;
			SetServiceStatus(service_status_handle,&g_status);
		}
		break;
	case SERVICE_CONTROL_INTERROGATE:
		//Notifies a service that it should report its current 
		//status information to the service control manager.
		{
			break;
		}
	case SERVICE_CONTROL_SHUTDOWN:
		//Notifies a service that the system is shutting down 
		//so the service can perform cleanup tasks. 
		{
			g_status.dwCurrentState = SERVICE_STOP_PENDING;
			SetServiceStatus(service_status_handle,&g_status);
			
			g_status.dwCurrentState = SERVICE_STOPPED;
			SetServiceStatus(service_status_handle,&g_status);
		}
		break;
	default:
		break;
	}
	
}
void Run()
{
	while (1)   //循环扫描系统磁盘
	{
		TCHAR tcBuffer[4] = {0}; //准备较小内存,先试探出实际需要的内存大小。
		DWORD dwNeededSize = GetLogicalDriveStrings(4, tcBuffer); //第一次执行，获得实际需要内存。
		if (dwNeededSize > 4)
		{   //返回值大于指定内存大小,需要重新准备足够内存再次执行。
			TCHAR *pBuffer = new TCHAR[dwNeededSize];  //准备足够大的内存
			dwNeededSize = GetLogicalDriveStrings(dwNeededSize, pBuffer); //获得逻辑驱动器字符串
			TCHAR *pDrive = pBuffer;
			int iPreLength = 0;
			while (1)
			{
				pDrive = pBuffer+iPreLength;  //获得下一个驱动器
				if (pDrive[0] == 0) //获取驱动器结束
				{
					break; //退出循环
				}
				DWORD dwDriveType = GetDriveType(pDrive); //获取驱动器类型
				if (dwDriveType == DRIVE_FIXED)
				{   //如果是移动驱动器
					TCHAR tcFile[MAX_PATH] = {0};
					_stprintf(tcFile, _T("%sa.txt"), pDrive);
					//打开已存在的a.txt文件
					HANDLE hFile = CreateFile(tcFile, \
						GENERIC_WRITE|GENERIC_READ, \
						FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,
						OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
					if (hFile == INVALID_HANDLE_VALUE)
					{
						//打开失败则创建一个。
						hFile = CreateFile(tcFile, GENERIC_WRITE|GENERIC_READ,\
							FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,
							CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
						DWORD dwWrite = 0;
						WriteFile(hFile, _T("Hello"), 5, &dwWrite, NULL);
					}
					CloseHandle(hFile);
				}
				iPreLength += _tcslen(pDrive);  //跳过当前驱动器
				iPreLength += sizeof(TCHAR);    //跳过'\0'。
			}
			if (pBuffer != NULL)
			{
				delete []pBuffer;  //释放内存
				pBuffer = NULL;
			}
		}
		Sleep(500);  //暂停500毫秒后继续扫描
		//如何g_hEvent无信号则暂停执行
		WaitForSingleObject(g_hEvent, INFINITE);
	}
}