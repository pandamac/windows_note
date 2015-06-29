//////////////////////////////////////////////////////////////////////////////////////
//																					//
// 日期:	2007/08/10																//
// 作者:	冷风																	//
// 说明:	服务程序框架 请改写mainfun函数															//
// 联系:	QQ 121121606 											//
//////////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <winsvc.h>
#include <stdio.h>
void main()
{
	char name[100];
	SC_HANDLE scm;
        SC_HANDLE service;
	SERVICE_STATUS status;
	printf("请输入要删除的服务名\n\n");
	scanf ("%s",&name);

	if((scm=OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE))==NULL)
	{
		printf("OpenSCManager Error\n");
	}
	service=OpenService(scm,name,SERVICE_ALL_ACCESS|DELETE);
	if (!service)
	{
		printf("OpenService error!\n");
		return;
	}
        BOOL isSuccess=QueryServiceStatus(service,&status);
	if (!isSuccess)
	{
		printf("QueryServiceStatus error!\n");
		return;
	}
       if ( status.dwCurrentState!=SERVICE_STOPPED )
	{
	    isSuccess=ControlService(service,SERVICE_CONTROL_STOP,&status);
	    if (!isSuccess )
		printf("Stop Service error!\n");
	    Sleep( 500 );
	}
    isSuccess=DeleteService(service);
    if (!isSuccess)
         printf("删除服务失败！\n");
    else
         printf("删除服务成功！\n");
    CloseServiceHandle(service );
    CloseServiceHandle(scm);
}