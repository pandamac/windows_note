#ifdef __cplusplus
extern "C"
{
#endif
#include <ntddk.h>
#ifdef __cplusplus
}
#endif

typedef struct _ServiceDescriptorTable {
    unsigned int* ServiceTableBase; //System Service Dispatch Table 的基地址  
    unsigned int* ServiceCounterTable;
    //包含着 SSDT 中每个服务被调用次数的计数器。这个计数器一般由sysenter 更新。 
    unsigned int NumberOfServices;//由 ServiceTableBase 描述的服务的数目。  
    unsigned char* ParamTableBase; //包含每个系统服务参数字节数表的基地址-系统服务参数表 
}*PServiceDescriptorTable;  

extern "C" extern PServiceDescriptorTable KeServiceDescriptorTable;

typedef NTSTATUS(*MYNTOPENPROCESS)(
    OUT PHANDLE             ProcessHandle,
    IN ACCESS_MASK          AccessMask,
    IN POBJECT_ATTRIBUTES   ObjectAttributes,
    IN PCLIENT_ID           ClientId );//定义一个函数指针，用于下面对O_NtOpenProcess进行强制转换

ULONG O_NtOpenProcess = 0; 

extern "C" NTSTATUS
    PsLookupProcessByProcessId(
    IN HANDLE ProcessId,
    OUT PEPROCESS *Process
    );

void PageProtectOff()//关闭页面保护
{
    __asm{
        cli
            mov  eax,cr0
            and  eax,not 10000h
            mov  cr0,eax
    }
}
void PageProtectOn()//打开页面保护
{
    __asm{
        mov  eax,cr0
            or   eax,10000h
            mov  cr0,eax
            sti
    }
}

// 判断是否打开的是自己想保护的进程
BOOLEAN ProtectProcess(HANDLE ProcessId,char *str_ProtectObjName)
{
    NTSTATUS status;
    PEPROCESS process_obj;

    if(!MmIsAddressValid(str_ProtectObjName))//这个条件是用来判断目标进程名是否有效
    {
        return FALSE;
    }
    if(ProcessId == 0)//这个条件是用来排除System Idle Process进程的干扰
    {
        return FALSE;
    }
    status=PsLookupProcessByProcessId(ProcessId, &process_obj);//这句用来获取目标进程的EPROCESS结构
    if(!NT_SUCCESS(status))
    {
        KdPrint(("我错了，这个是错误号:%X---这个是进程ID:%d\n",status,ProcessId));
        return FALSE;
    }
    if(!strcmp((char *)process_obj + 0x174, str_ProtectObjName))//进行比较
    {
        ObDereferenceObject(process_obj);//对象计数器减1，为了恢复对象管理器计数，便于回收
        return TRUE;
    }
    ObDereferenceObject(process_obj);
    return FALSE;
    //使用了PsLookupProcessByProcessId后，必须用ObDereferenceObject回收！
    //PsLookupProcessByProcessId获取指定进程的EPROCESS结构,目的就是获取进程名
}
NTSTATUS MyNtOpenProcess (
    __out PHANDLE ProcessHandle,
    __in ACCESS_MASK DesiredAccess,
    __in POBJECT_ATTRIBUTES ObjectAttributes,
    __in_opt PCLIENT_ID ClientId
    )
{
    if(ProtectProcess(ClientId->UniqueProcess,"notepad.exe"))    // 如果打开的进程是目标进程
    {
        KdPrint(("%s打开文件失败\n",(char *)PsGetCurrentProcess()+0x174));//dt _EPROCESS 可以查看EPROCESS结构
        return STATUS_UNSUCCESSFUL;    
		// 这是一个关键，设置STATUS_UNSUCCESSFUL，CE就会提示打开不成功
    }
    
    return ((MYNTOPENPROCESS)O_NtOpenProcess)(ProcessHandle,
		//处理完自己的任务后，调用原来的函数，让其它进程正常工作
        DesiredAccess,
        ObjectAttributes,
        ClientId);
}


VOID UnHookSsdt()
{
    PageProtectOff();
    KeServiceDescriptorTable->ServiceTableBase[122] = O_NtOpenProcess; //恢复ssdt中原来的函数地址
    PageProtectOn();
}
NTSTATUS HookSsdt()
{
    // 1、在ssdt_hook()函数中保存要hook函数的（NtOpenProcess）地址
    O_NtOpenProcess = KeServiceDescriptorTable->ServiceTableBase[122];
    PageProtectOff();
    // 2、将原来ssdt中所要hook的函数地址换成我们自己的函数地址
    KeServiceDescriptorTable->ServiceTableBase[122] = (unsigned int)MyNtOpenProcess;
    // 此时我们用CE打开被保护的进程，就会调用我们自己的函数
    PageProtectOn();
    
    return STATUS_SUCCESS;
}
VOID DriverUnload(PDRIVER_OBJECT pDriverObject)
{
    UnHookSsdt();
    KdPrint(("Driver Unload Success !\n"));
}

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath )
{
    KdPrint(("Welcome to My Driver\n"));
    pDriverObject->DriverUnload = DriverUnload;
    /* 所有函数的地址
    for (int i = 0; i < KeServiceDescriptorTable->NumberOfServices; i++)
    {
	KdPrint(("NumberOfService[%d]-------%X\n", i, KeServiceDescriptorTable->ServiceTableBase[i]));
}*/
    HookSsdt();
    return STATUS_SUCCESS;
}