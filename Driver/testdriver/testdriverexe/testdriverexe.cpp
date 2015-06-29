// testdriverexe.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include "ntapi.h"
#include "CTL_CODE.h"

#pragma comment(linker,"/defaultlib:ntdll.lib")  
BOOL g_IsShellcodeCalled = 0;
#define KERNEL_NAME_LENGTH 0x0D
#define FILE_OPEN_IF 3
#define OBJ_CASE_INSENSITIVE 0x00000040L


#define IOCTL_Method  0x88102004  //11
#define IOCTL_Method2  0x88102008 
#define IOCTL_Method3  0x8810200C
#define IOCTL_Method4   0x88102007  //NOTING
#define IOCTL_Method5 0x88102010


int _tmain(int argc, _TCHAR* argv[])
{
	
	UNICODE_STRING DeviceName = {0};
	RtlInitUnicodeString(&DeviceName,L"\\Device\\Hctf");

	OBJECT_ATTRIBUTES obj_attribute;
	IO_STATUS_BLOCK io_status_block;

	obj_attribute.Attributes = OBJ_CASE_INSENSITIVE;
	obj_attribute.Length = sizeof(OBJECT_ATTRIBUTES);
	obj_attribute.ObjectName = &DeviceName;
	obj_attribute.RootDirectory = 0;
	obj_attribute.SecurityDescriptor = NULL;
	obj_attribute.SecurityQualityOfService = NULL;

	HANDLE hDevice = NULL;
	NTSTATUS Ntstatus = NtCreateFile(&hDevice,FILE_READ_DATA | FILE_WRITE_DATA,&obj_attribute,&io_status_block,NULL,0,\
		FILE_SHARE_READ | FILE_SHARE_WRITE,\
		FILE_OPEN_IF,//If the file already exists, open it. If it does not, create the given file.
		0,
		NULL,NULL);
	if (Ntstatus)
		{
			printf("没有找到drivr\n");
			//return 1;
		}
	/*char string1[] = "WelcHCTF";*/

	
	char outputdata[100];
	char inputdata[100]="^lejAJ]O";
	
	memset(outputdata,2,100);
	
	printf("send irp1\n");
	 Ntstatus = NtDeviceIoControlFile(hDevice,\
		NULL,NULL,NULL,&io_status_block,\
		IOCTL_Method,
		&inputdata,\
		100,\
		&outputdata,\
		100);
	if (Ntstatus) //return 1;
	printf("data: %s",outputdata);
	//^lejAJ]OMNIII}
	char inputdata2[100]= "MNIII";
	Ntstatus = NtDeviceIoControlFile(hDevice,\
		NULL,NULL,NULL,&io_status_block,\
		IOCTL_Method,
		&inputdata2,\
		100,\
		&outputdata,\
		100);
	if (Ntstatus) return 1;
	printf("data: %s",outputdata);

	char inputdata3[100]= "111111";
	Ntstatus = NtDeviceIoControlFile(hDevice,\
		NULL,NULL,NULL,&io_status_block,\
		IOCTL_Method2,
		&inputdata3,\
		100,\
		&outputdata,\
		100);
	if (Ntstatus) return 1;
	printf("data: %s",outputdata);

	char inputdata4[100]= "222222";
	Ntstatus = NtDeviceIoControlFile(hDevice,\
		NULL,NULL,NULL,&io_status_block,\
		IOCTL_Method3,
		&inputdata4,
		100,\
		&outputdata,\
		100);
	if (Ntstatus) return 1;
	printf("data: %s",outputdata);

	char inputdata5[100]= "333333";
	Ntstatus = NtDeviceIoControlFile(hDevice,\
		NULL,NULL,NULL,&io_status_block,\
		IOCTL_Method4,
		&inputdata5,
		100,\
		&outputdata,\
		100);
	if (Ntstatus) return 1;
	printf("data: %s",outputdata);
	
	char inputdata6[100]= "VMP 2.12}";//7 wei
	Ntstatus = NtDeviceIoControlFile(hDevice,\
		NULL,NULL,NULL,&io_status_block,\
		IOCTL_Method5,
		&inputdata6,
		100,\
		&outputdata,\
		100);
	if (Ntstatus) return 1;
	printf("data: %s",outputdata);
	//NonPagedPool 
ret:

// 	//关闭句柄
// 	if(hDevice)
// 	{
// 		Ntstatus = NtClose(hDevice);
// 		if(Ntstatus)
// 		{
// 			printf("NtClose failed! NtStatus=%.8X\n", Ntstatus);    
// 		}
// 	}
	return 0;
}

