#ifdef __cplusplus
extern "C"
{
#endif
#include <ntddk.h>
#ifdef __cplusplus
}
#endif

extern "C"
NTSTATUS
PsLookupProcessByProcessId(
						   IN  HANDLE ProcessId,
						   OUT PEPROCESS *Process
						   );


VOID
ProcessRoutine(
								   IN HANDLE  ParentId,
								   IN HANDLE  ProcessId,
								   IN BOOLEAN  Create
								   )
{
	PEPROCESS peprocess;

	PsLookupProcessByProcessId(ProcessId,&peprocess);
	if (Create)
	{
		KdPrint(("ProcessID = %d,ProcessName = %s, Create = %d 进程创建",ProcessId,(char*)peprocess+0x174,Create));
	}
	else
		KdPrint(("ProcessID = %d,ProcessName = %s, Create = %d 进程退出",ProcessId,(char*)peprocess+0x174,Create));
	
}
VOID DriverUnload(PDRIVER_OBJECT pDriverObject)
{
	PsSetCreateProcessNotifyRoutine(PCREATE_PROCESS_NOTIFY_ROUTINE(ProcessRoutine),TRUE);

    KdPrint(("Driver Unload Success !\n"));
}

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath )
{
  KdPrint(("Welcome to My Driver\n"));
  pDriverObject->DriverUnload = DriverUnload;
  
  NTSTATUS status = STATUS_SUCCESS;

PsSetCreateProcessNotifyRoutine(PCREATE_PROCESS_NOTIFY_ROUTINE(ProcessRoutine),FALSE);


  return STATUS_SUCCESS;
}
