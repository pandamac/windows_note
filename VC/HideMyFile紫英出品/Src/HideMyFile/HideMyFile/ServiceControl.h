#include "stdafx.h"
#ifndef _SERVICE_H
#define _SERVICE_H

#include <Windows.h>

//获取服务运行状态
DWORD GetServiceStatus(LPCWSTR sServiceName);
//启动服务
BOOL StartServiceForUser(LPCWSTR sServiceName);
//停止服务
BOOL StopServiceForUser(LPCWSTR sServiceName);
//卸载服务
BOOL UninstallServiceFromSCM(LPCWSTR sServiceName);
//安装服务
BOOL InstallServiceToSCM(LPCWSTR sDriverFileFullName,LPCWSTR sServiceName,
										   LPCWSTR sDisplayName,DWORD dwStartType);

#endif

/*代码示例
int _tmain(int argc, _TCHAR* argv[])
{
		DWORD dwStartType = SERVICE_AUTO_START;//自动运行
		TCHAR DriverPath[MAX_PATH] = L"E:\\Test.sys";//驱动路径

		TCHAR *sDriverPath = DriverPath;
		TCHAR sServiceName[] = L"MyDriver";//服务名
		TCHAR sDlsplayName[] = L"MyDriver";//启动名

		InstallServiceToSCM(sDriverPath,sServiceName,sDlsplayName,dwStartType);//安装服务
		StartServiceForUser(sServiceName);//启动服务
		StopServiceForUser(sServiceName);//停止服务
		UninstallServiceFromSCM(sServiceName);//卸载服务

		return 0;
}
*/