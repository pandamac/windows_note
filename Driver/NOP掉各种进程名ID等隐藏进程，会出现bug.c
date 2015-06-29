#include <ntddk.h>

PEPROCESS g_eprocess =NULL;
PLIST_ENTRY g_pList_Current =NULL;
NTSTATUS 
UnloadDriver(
             IN PDRIVER_OBJECT DriverObject
             )
{

    KdPrint(("退出driver 还原ActiveProcessLinks SessionProcessLinks .\n"));
       if( g_pList_Current!=NULL)
          {
             g_pList_Current->Blink->Flink=g_pList_Current;
             g_pList_Current->Flink->Blink=g_pList_Current;

          g_pList_Current=(ULONG)g_eprocess+0x0b4;
          g_pList_Current->Blink->Flink=g_pList_Current;
          g_pList_Current->Flink->Blink=g_pList_Current;
}
    return STATUS_SUCCESS;
}

NTSTATUS 
DriverEntry(
            IN PDRIVER_OBJECT driver, 
            IN PUNICODE_STRING  RegistryPath
            )
{
    PEPROCESS eprocess,Firsteprocess;
    ULONG ulProcessName;
    ULONG ulProcessID;
    PLIST_ENTRY pList_Current;


    KdPrint(("Welcome to My Driver\n"));
    driver->DriverUnload = UnloadDriver;

    eprocess = PsGetCurrentProcess();

    Firsteprocess = eprocess;
    if (eprocess == 0)
    {
        KdPrint(("PsGetCurrentProcess ERROR!"));
        return STATUS_SUCCESS;
    }
    while (eprocess != NULL)
    {
        ulProcessName = (ULONG)eprocess + 0x174;
        ulProcessID = *(ULONG*)((ULONG)eprocess + 0x84);
        KdPrint(("processname = %s,processid = %d \r \n", ulProcessName,ulProcessID));
        if (strcmp( (char*)(ULONG*)ulProcessName ,"notepad.exe") == 0 )
        {
            KdPrint(("PID:%lu\n", ulProcessID));
              *(ULONG*)((ULONG)pEprocess+0x84)=0;// +0x084 UniqueProcessId  : Ptr32 Void
              *(ULONG*)((ULONG)pEprocess+0x14c)=0;//父进程ID InheritedFromUniqueProcessId
              *(ULONG*)((ULONG)pEprocess+0x174)=0;//抹掉进程名 ImageFileName
              *(ULONG*)((ULONG)pEprocess)=0;//PEPROCESS->Pcb.Header.Type
              *(ULONG*)(*(ULONG*)((ULONG)pEprocess+0xc4) + 0x008)=0;//UniqueProcessID
           break;
        }
            eprocess = (PEPROCESS)(*(ULONG*)((ULONG)eprocess + 0x88) - 0x88);
        if (eprocess == Firsteprocess || (*(LONG*)((LONG)eprocess + 0x84)) <0)
        {
            break;
        }
    }
    return STATUS_SUCCESS;
}