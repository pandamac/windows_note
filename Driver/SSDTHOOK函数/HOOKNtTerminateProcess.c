#include <ntddk.h>

typedef struct _SystemServiceDescriptorTable 
{ 
    PVOID ServiceTableBase; //SSDT表的基地址
    PULONG ServiceCounterTableBase; //指向另一个索引表，该表包含了每个服务表项被调用的次数
    ULONG NumberOfService; //当前系统所支持的服务个数
    ULONG ParamTableBase; //包含了每个服务所需的参数字节数
}SystemServiceDescriptorTable,*PSystemServiceDescriptorTable; 
extern "C" PSystemServiceDescriptorTable KeServiceDescriptorTable; //导出函数，DDK的头文件中并未声明

typedef NTSTATUS (*NTTERNIMATEPROCESS)( IN HANDLE  ProcessHandle,
IN NTSTATUS  ExitStatus );  //定义一个函数指针
NTTERNIMATEPROCESS pRealNtTerminateAddr;
ULONG RealServiceAddress;  //接受被hook的函数地址
CHAR *TerminateName = "notepad.exe";  //这里就是我们的记事本进程名

extern "C" __declspec(dllimport) UCHAR* PsGetProcessImageFileName( IN PEPROCESS Process );  

BOOLEAN IsProtect(CHAR *temp)  //判断正在结束的进程是否是我们要保护的记事本进程
{
    ULONG len = strcmp(TerminateName, temp);
    if(!len)	  
        return TRUE;
    return FALSE;
}

NTSTATUS MyNtTerminateProcess(IN HANDLE  ProcessHandle, IN NTSTATUS  ExitStatus)//我们自己的NtTerminateProcess
{
    PEPROCESS process;  //接受通过ProcessHandle返回的进程
    NTSTATUS status;  
    CHAR *pName;  //接受进程的进程名 
    status = ObReferenceObjectByHandle(ProcessHandle,FILE_READ_DATA,0
                                      ,KernelMode,(PVOID *)&process,NULL);  //获取进程

    if(!NT_SUCCESS(status))  
	return (NTSTATUS)(NTTERNIMATEPROCESS)pRealNtTerminateAddr(ProcessHandle, ExitStatus);
    pName = (CHAR*)PsGetProcessImageFileName(process);  //获取进程名

    if(IsProtect(pName))  //判断是否是我们要保护的进程
	return STATUS_ACCESS_DENIED;
    return (NTSTATUS)(NTTERNIMATEPROCESS)pRealNtTerminateAddr(ProcessHandle, ExitStatus);
}

NTSTATUS Hook()
{
    ULONG Address;
    KdPrint(("entry Hook.........\n"));
    Address = (ULONG)KeServiceDescriptorTable->ServiceTableBase + 0x101 * 4;//0x101可以用windbg获得
    RealServiceAddress = *(ULONG*)Address;  
    pRealNtTerminateAddr = (NTTERNIMATEPROCESS)RealServiceAddress;

    //开启SSDT 表为可写
    _asm
    {
        cli;
        mov eax, cr0;
        and eax, not 10000h;
        mov  cr0, eax;
    }

    *((ULONG*)Address) = (ULONG)MyNtTerminateProcess;  //替换为自己的NtTerminateProcess函数
    //设置SSDT表位只读
    _asm
    {
       mov eax, cr0;
       or eax, 10000h;
       mov cr0, eax;
       sti;  
    }

}
VOID UnHook()//把SSDT中的NtTerminateprocess函数还原
{
    ULONG Address;
    Address = (ULONG)KeServiceDescriptorTable->ServiceTableBase + 0x101 * 4;
    __asm 
    { 
        cli
        mov eax, cr0;
        and eax, not 10000h ;
        mov cr0, eax ;
    } 
    *((ULONG*)Address) = (ULONG)RealServiceAddress; 
    __asm 
    { 
        mov eax, cr0 ;
        or eax, 10000h ;
        mov cr0, eax ;
        sti;
    } 
}


VOID Unload(PDRIVER_OBJECT driver)
{
    UNREFERENCED_PARAMETER(driver);
    UnHook();
    DbgPrint(("Unload success........."));
}
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING str)
{
    NTSTATUS status;
    DbgPrint("hello ProcessProtect.......\n");
    UNREFERENCED_PARAMETER(str);
    driver->DriverUnload = Unload;
    status = Hook();
    return STATUS_SUCCESS;
}
