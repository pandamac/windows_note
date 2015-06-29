#ifdef __cplusplus
extern "C"
{
#endif
#include <ntddk.h>
#ifdef __cplusplus
}
#endif


typedef struct _SERVICE_DESCRIPTOR_TABLE
{
    PULONG ServiceTableBase;
    PULONG ServiceCounterTableBase;
    ULONG  NumberOfServices;
    PUCHAR ParamTableBase;
}SERVICE_DESCRIPTOR_TABLE, *PSERVICE_DESCRIPTOR_TABLE;

extern "C" PSERVICE_DESCRIPTOR_TABLE KeServiceDescriptorTable;

typedef NTSTATUS
(*NtCreateProcessEx)(
           __out PHANDLE ProcessHandle,
           __in ACCESS_MASK DesiredAccess,
           __in_opt POBJECT_ATTRIBUTES ObjectAttributes,
           __in HANDLE ParentProcess,
           __in ULONG Flags,
           __in_opt HANDLE SectionHandle,
           __in_opt HANDLE DebugPort,
           __in_opt HANDLE ExceptionPort,
           __in ULONG JobMemberLevel
           );

NtCreateProcessEx OldNtCreateProcessEx = 0;

ULONG ulOldNtOpenProcessAddr;
ULONG ulSSDT;
ULONG ulNtCreateProcessExAddr;

unsigned char bOldBytes[5]; //函数入口代码
unsigned char bNewBytes[5]; //Inline 代码

#define IOCTL_ON CTL_CODE(FILE_DEVICE_UNKNOWN,0x800,METHOD_IN_DIRECT,FILE_ANY_ACCESS)
#define IOCTL_OFF CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_IN_DIRECT,FILE_ANY_ACCESS)



VOID UN_PROTECT()
{
  __asm
  {
    cli
      mov eax,cr0
      and eax,not 10000h //把16位去掉
      mov cr0,eax
  }
}
VOID RE_PROTECT()
{
  __asm
  {
    
    mov eax,cr0
      or eax, 10000h
      mov cr0,eax
      sti
  }
}
VOID HookCreateProcessEx()
{
	UN_PROTECT();
    if (OldNtCreateProcessEx != 0)
    {
		RtlCopyMemory((PVOID)OldNtCreateProcessEx,(CONST PVOID)bNewBytes,5);
    }
	RE_PROTECT();
}
VOID UnHookCreateProcessEx()
{
	UN_PROTECT();
	if (OldNtCreateProcessEx != 0)
	{
		RtlCopyMemory((PVOID)OldNtCreateProcessEx,(CONST PVOID)bOldBytes,5);
	}
	RE_PROTECT();
}


extern"C" __declspec(dllimport)UCHAR*PsGetProcessImageFileName(IN PEPROCESS Process);
extern "C" NTSTATUS
PsLookupProcessByProcessId(
               IN HANDLE ProcessId,
               OUT PEPROCESS *Process
               );

NTSTATUS
MyNtCreateProcessEx(
          __out PHANDLE ProcessHandle,
          __in ACCESS_MASK DesiredAccess,
          __in_opt POBJECT_ATTRIBUTES ObjectAttributes,
          __in HANDLE ParentProcess,
          __in ULONG Flags,
          __in_opt HANDLE SectionHandle,
          __in_opt HANDLE DebugPort,
          __in_opt HANDLE ExceptionPort,
          __in ULONG JobMemberLevel
          )
{ 
  NTSTATUS tagRet = STATUS_SUCCESS;
  PEPROCESS   lpEprocess;

  KdPrint(("\n\nENTER mY cREATEprocess  no CreateProcessEx"));
  
  
  DbgPrint("Create Process");
  
  
  UnHookCreateProcessEx();
	  
	  tagRet = OldNtCreateProcessEx(ProcessHandle, DesiredAccess, ObjectAttributes,
	  ParentProcess, Flags, SectionHandle, DebugPort, ExceptionPort, JobMemberLevel);
  
  
  DbgPrint("handle value = %x", ProcessHandle);
  
  tagRet = ObReferenceObjectByHandle(*ProcessHandle, NULL, NULL, KernelMode, (PVOID*)&lpEprocess, NULL);
  
  if(!NT_SUCCESS(tagRet))
  {
    DbgPrint("ObReferenceObjectByHandle failure~");
    
    DbgPrint("rc value = %x", tagRet);
  }
  else
  {
    DbgPrint("ObReferenceObjectByHandle success~~");
    ObDereferenceObject(&lpEprocess);
  }
  
  
  UCHAR *lpFileName = PsGetProcessImageFileName(lpEprocess);
  
  if(lpFileName)
  {
    DbgPrint("PsGetProcessImageFileName success...");
    DbgPrint((PCSTR)lpFileName);
  }
  else
  {
    DbgPrint("PsGetProcessImageFileName failure...");
  }
  
  HookCreateProcessEx();
    return tagRet; 

  
}


VOID InlineHookCreateProcessEx()
{

  ulSSDT = (ULONG)KeServiceDescriptorTable->ServiceTableBase;
  ulNtCreateProcessExAddr = ulSSDT + 0x30 * 4; //地址指针
  
  OldNtCreateProcessEx =  (NtCreateProcessEx) *(PULONG)(ulSSDT + 0x30 * 4);
  
  RtlCopyMemory((PVOID)bOldBytes,(CONST PVOID)OldNtCreateProcessEx,5);
  
  bNewBytes[0] = '\xE9';  //jmp
  *(PULONG)&bNewBytes[1] = (ULONG)MyNtCreateProcessEx - (ULONG)OldNtCreateProcessEx - 5;

}



NTSTATUS
CreateRoutine(
          IN PDEVICE_OBJECT  DeviceObject,
          IN PIRP  Irp
          )
{
  KdPrint(("enter my createRountie"));

  return STATUS_SUCCESS;
}
NTSTATUS
DispatchRoutine(
        IN PDEVICE_OBJECT  DeviceObject,
        IN PIRP  Irp
        )
{
  KdPrint(("enter my DispatchRoutine"));
  NTSTATUS status = STATUS_SUCCESS;
  
  PIO_STACK_LOCATION pStack = IoGetCurrentIrpStackLocation(Irp);
  
  ULONG ulcode = pStack->Parameters.DeviceIoControl.IoControlCode;
  
  switch (ulcode)
  {
  case IOCTL_ON:
    {
      HookCreateProcessEx();
      break;
    }
  case IOCTL_OFF:
    {
      UnHookCreateProcessEx();
      break;
    }
  }
  Irp->IoStatus.Status = status;
  Irp->IoStatus.Information = 0;
  IoCompleteRequest(Irp,IO_NO_INCREMENT );
  
  
  return status;
}
VOID DriverUnload(PDRIVER_OBJECT pDriverObject)
{
	UNICODE_STRING linkname;
	
	RtlInitUnicodeString(&linkname,L"\\??\\HookNtCreateProcessEx");
	
	IoDeleteSymbolicLink(&linkname);
	
	PDEVICE_OBJECT pDevobj = pDriverObject->DeviceObject;
	
	IoDeleteDevice(pDevobj);
	UnHookCreateProcessEx();
    KdPrint(("Driver Unload Success !\n"));
}
extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath )
{
  KdPrint(("Welcome to My Driver\n"));
  pDriverObject->DriverUnload = DriverUnload;
  
  PDEVICE_OBJECT pdev = NULL;
  NTSTATUS status = STATUS_SUCCESS;

  pDriverObject->MajorFunction[IRP_MJ_CREATE] = CreateRoutine;
  pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchRoutine;
  
  InlineHookCreateProcessEx();
  
  UNICODE_STRING DevName,LinkName;

  RtlInitUnicodeString(&DevName,L"\\Device\\HookNtCreateProcessEx");


  status = IoCreateDevice(pDriverObject,\
    0,\
    &DevName,\
    FILE_DEVICE_UNKNOWN,\
    0,\
    TRUE,\
    &pdev
    );
  if (!NT_SUCCESS(status))
  {
    KdPrint(("IoCreateDevice ERROR!"));
    return status;
  }
  KdPrint(("Welcome to IoCreateDevice\n"));
  pdev->Flags |= DO_DIRECT_IO;
  pdev->DriverObject = pDriverObject;
  
  RtlInitUnicodeString(&LinkName,L"\\??\\HookNtCreateProcessEx");
  
  status = IoCreateSymbolicLink(&LinkName,&DevName);
  KdPrint(("Welcome to IoCreateSymbolicLink\n"));
  if (!NT_SUCCESS(status))
  {
	  KdPrint(("IoCreateSymbolicLink ERROR!"));
	  IoDeleteSymbolicLink(&LinkName);
	  return status;
  }
  return STATUS_SUCCESS;
}
