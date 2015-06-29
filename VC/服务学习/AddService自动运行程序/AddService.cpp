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
main()
{
	char name[100];
	char info[200];
	char path[300];
	printf("请输入服务名\n\n");
	scanf ("%s",&name);
	printf("请输入服务描述\n\n");
	scanf ("%s",&info);
	printf("请输入程序路径\n\n");
	scanf ("%s",&path);
	SC_HANDLE manager=NULL;
	SC_HANDLE service=NULL;
	if((manager=OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE))==NULL)
	{
		printf("OpenSCManager Error");
	}
	service=CreateService(
			manager,name,info, 
			SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, 
			SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
			path, 0, 0, 0, 0, 0 );
	if(service)
	printf("服务创建成功\n\n");
	else
	printf("服务创建失败\n\n");
	CloseServiceHandle(service);
	CloseServiceHandle(manager);
}