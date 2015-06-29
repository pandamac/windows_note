#include "stdafx.h"
#include "ServiceControl.h"
#include <winsvc.h>
//////////Code By Sonsie~~^_^ 2012-06-22////////////////

/////////////////////////////////////////////////////////////////
//函数：GetServiceStatus
//参数：sServiceName - 服务名称
//说明：获取服务运行状态
//////////////////////////////////////////////////////////////////
DWORD GetServiceStatus(LPCWSTR sServiceName)
{
	SC_HANDLE hSCManager = NULL;//服务管理器句柄
	SC_HANDLE hService = NULL;//服务句柄
	SERVICE_STATUS SS = {0};//状态值
	DWORD dwErrorStaus  = SERVICE_STOPPED;//错误属性

	//打开服务管理器
	hSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if (NULL == hSCManager) goto Err;
	//打开服务
	hService = OpenService(hSCManager,sServiceName,SERVICE_ALL_ACCESS);
	if (NULL == hService) goto Err;
	//获取服务状态
	if (ControlService(hService,SERVICE_CONTROL_INTERROGATE,&SS) == FALSE)  goto Err;
	//关闭句柄
	CloseServiceHandle(hSCManager);
	CloseServiceHandle(hService);
	//返回状态字
	return SS.dwCurrentState;
Err:
	if (NULL != hSCManager) CloseServiceHandle(hSCManager);
	if (NULL != hService) CloseServiceHandle(hService);
	return FALSE;
}

/////////////////////////////////////////////////////////////////
//函数：StartServiceForUser
//参数：sServiceName - 服务名称
//说明：启动服务
//////////////////////////////////////////////////////////////////
BOOL StartServiceForUser(LPCWSTR sServiceName)
{
	SC_HANDLE hSCManager = NULL;//服务管理器句柄
	SC_HANDLE hService = NULL;//服务句柄
	//服务处于运行状态则返回
	if (GetServiceStatus(sServiceName) == SERVICE_RUNNING) return TRUE;
	//打开服务管理器
	hSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if (NULL == hSCManager) goto Err;
	//打开服务
	hService = OpenService(hSCManager,sServiceName,SERVICE_ALL_ACCESS);
	if (NULL == hService) goto Err;
	//启动服务
	if (StartService(hService,0,NULL) == FALSE)  goto Err;
	//关闭句柄
	CloseServiceHandle(hSCManager);
	CloseServiceHandle(hService);
	return TRUE;
Err:
	if (NULL != hSCManager) CloseServiceHandle(hSCManager);
	if (NULL != hService) CloseServiceHandle(hService);
	return FALSE;
}
/////////////////////////////////////////////////////////////////
//函数：StopServiceForUser
//参数：sServiceName - 服务名称
//说明：停止服务
//////////////////////////////////////////////////////////////////
BOOL StopServiceForUser(LPCWSTR sServiceName)
{
	SC_HANDLE hSCManager = NULL;//服务管理器句柄
	SC_HANDLE hService = NULL;//服务句柄
	SERVICE_STATUS SS = {0};//状态值
	
	//打开服务管理器
	hSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if (NULL == hSCManager) goto Err;
	//打开服务
	hService = OpenService(hSCManager,sServiceName,SERVICE_ALL_ACCESS);
	if (NULL == hService) goto Err;
	//控制服务
	if (ControlService(hService,SERVICE_CONTROL_STOP,&SS) == FALSE)  goto Err;
	//关闭句柄
	CloseServiceHandle(hSCManager);
	CloseServiceHandle(hService);
	return TRUE;
Err:
	if (NULL != hSCManager) CloseServiceHandle(hSCManager);
	if (NULL != hService) CloseServiceHandle(hService);
	return FALSE;
}
/////////////////////////////////////////////////////////////////
//函数：UninstallServiceFromSCM
//参数：sServiceName - 服务名称
//说明：卸载服务
//////////////////////////////////////////////////////////////////
BOOL UninstallServiceFromSCM(LPCWSTR sServiceName)
{
	SC_HANDLE hSCManager = NULL;//服务管理器句柄
	SC_HANDLE hService = NULL;//服务句柄
	//卸载前先停止服务
	StopServiceForUser(sServiceName);
	//打开服务管理器
	hSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if (NULL == hSCManager) goto Err;
	//打开服务
	hService = OpenService(hSCManager,sServiceName,SERVICE_ALL_ACCESS);
	if (NULL == hService) goto Err;
	//删除服务
	if (DeleteService(hService) == FALSE) goto Err;
	//关闭句柄
	CloseServiceHandle(hSCManager);
	CloseServiceHandle(hService);
	return TRUE;
Err:
		if (NULL != hSCManager) CloseServiceHandle(hSCManager);
		if (NULL != hService) CloseServiceHandle(hService);
		return FALSE;
}
/////////////////////////////////////////////////////////////////
//函数：UninstallServiceFromSCM
//参数：sDriverFileFullName - 驱动文件路径
//			 sServiceName - 服务名称
//		     sDisplayName - 服务显示的名称
//			 dwStartType - 启动方式
//说明：卸载服务
//////////////////////////////////////////////////////////////////
BOOL InstallServiceToSCM(LPCWSTR sDriverFileFullName,LPCWSTR sServiceName,LPCWSTR sDisplayName,DWORD dwStartType)
{
	SC_HANDLE hSCManager = NULL;//服务管理器句柄
	SC_HANDLE hService = NULL;//服务句柄
	//安装前先卸载服务
	UninstallServiceFromSCM(sServiceName);
	//打开服务管理器
	hSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if (NULL == hSCManager) goto Err;
	//创建服务
	hService = CreateService(hSCManager,sServiceName,	sDisplayName,SERVICE_ALL_ACCESS,SERVICE_KERNEL_DRIVER, dwStartType,
											SERVICE_ERROR_NORMAL,sDriverFileFullName,NULL,NULL,NULL,NULL,NULL);
	if (NULL == hService) goto Err;

	//关闭句柄
	CloseServiceHandle(hSCManager);
	CloseServiceHandle(hService);
	return TRUE;
Err:
	if (NULL != hSCManager) CloseServiceHandle(hSCManager);
	if (NULL != hService) CloseServiceHandle(hService);
	return FALSE;
}

