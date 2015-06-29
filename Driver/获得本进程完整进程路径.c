
#define MAXPATHLEN 256
#define DWORD unsigned long
#define WORD unsigned short
#define BOOL unsigned long
#define BYTE unsigned char


UNICODE_STRING uParentProcessName;
PCWSTR ParentProcessName;

//获得当前进程完整路径
PCWSTR GetCurrentProcessFileName() 
{ 
	DWORD dwAddress = (DWORD)PsGetCurrentProcess(); 
	//对于父进程，在PEPROCESS保存进程名，通过自定义的一个函数当前进程可以得到完整的路径
	DWORD dwAddress1;
	if(dwAddress == 0 || dwAddress == 0xFFFFFFFF)  
		return NULL; 
	dwAddress += 0x1B0; // +0x1b0 Peb              : Ptr32 _PEB
	if((dwAddress = *(DWORD*)dwAddress) == 0)
	 return 0; 
	dwAddress += 0x10; //+0x010 ProcessParameters : Ptr32 _RTL_USER_PROCESS_PA
	if((dwAddress = *(DWORD*)dwAddress) == 0) 
		return 0; 
	dwAddress1 = dwAddress;//2000
	dwAddress += 0x3C; 
	if((dwAddress = *(DWORD*)dwAddress) == 0) 
		return 0; 
	if (dwAddress < dwAddress1) 
		dwAddress = dwAddress+dwAddress1;
	KdPrint(("Current Process Full Path Name: %ws\n", (PCWSTR)dwAddress)); 
	return (PCWSTR)dwAddress; 
} 
	ParentProcessName=GetCurrentProcessFileName();

RtlInitUnicodeString(&uParentProcessName,ParentProcessName);

RtlUnicodeStringToAnsiString(&aParentProcessName,&uParentProcessName,TRUE);