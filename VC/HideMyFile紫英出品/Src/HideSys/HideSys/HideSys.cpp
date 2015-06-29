#include "stdafx.h"
#include "SSDTHook.h"

extern ULONG g_OriginalNtQuerySystemInformation;

void HideSysUnload(IN PDRIVER_OBJECT DriverObject);
NTSTATUS HideSysCreateClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS HideSysDefaultHandler(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);

#ifdef __cplusplus
extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING  RegistryPath);
#endif

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING  RegistryPath)
{
	UNICODE_STRING DeviceName,Win32Device;
	PDEVICE_OBJECT DeviceObject = NULL;
	NTSTATUS status;
	unsigned i;

	RtlInitUnicodeString(&DeviceName,L"\\Device\\HideSys");
	RtlInitUnicodeString(&Win32Device,L"\\DosDevices\\HideSys");

	for (i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++)
		DriverObject->MajorFunction[i] = HideSysDefaultHandler;

	DriverObject->MajorFunction[IRP_MJ_CREATE] = HideSysCreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = HideSysCreateClose;
	
	DriverObject->DriverUnload = HideSysUnload;

    //KdBreakPoint();
    KdPrint(("Loaded Win7 Driver Successfully!\n"));

    CheckWindowsVersion();

    SetNtQueryDirectoryFileHook(TRUE);

	status = IoCreateDevice(DriverObject,
							0,
							&DeviceName,
							FILE_DEVICE_UNKNOWN,
							0,
							FALSE,
							&DeviceObject);
	if (!NT_SUCCESS(status))
		return status;
	if (!DeviceObject)
		return STATUS_UNEXPECTED_IO_ERROR;

	DeviceObject->Flags |= DO_BUFFERED_IO;
	DeviceObject->AlignmentRequirement = FILE_WORD_ALIGNMENT;
	status = IoCreateSymbolicLink(&Win32Device, &DeviceName);

	DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

    
	return STATUS_SUCCESS;
}

void HideSysUnload(IN PDRIVER_OBJECT DriverObject)
{
    if (g_OriginalNtQuerySystemInformation != 0)
    {
        SetNtQuerySystemInformationHook(FALSE);
    }

    SetNtQueryDirectoryFileHook(FALSE);

    KdPrint(("Driver Unload \n"));

	UNICODE_STRING Win32Device;
	RtlInitUnicodeString(&Win32Device,L"\\DosDevices\\HideSys");
	IoDeleteSymbolicLink(&Win32Device);
	IoDeleteDevice(DriverObject->DeviceObject);
}

NTSTATUS HideSysCreateClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS HideSysDefaultHandler(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
    NTSTATUS status = STATUS_SUCCESS;
    PVOID pInputBuffer = NULL;

    PIO_STACK_LOCATION pIoStack = IoGetCurrentIrpStackLocation(Irp);

    ULONG ulControlCode = pIoStack->Parameters.DeviceIoControl.IoControlCode;

    pInputBuffer = (PVOID)Irp->AssociatedIrp.SystemBuffer;


    switch(ulControlCode)
    {
    case IOCTL_ADD_HIDDEN_FILE:
        {
            //KdPrint(("ADD_HIDDEN_FILE = %S", (LPWSTR)pInputBuffer));
            AddFileToFileHiddenTable((LPWSTR)pInputBuffer);
            break;
        }
    case IOCTL_SET_FILE_HIDDEN:
        {
            //KdPrint(("SET_FILE_HIDE = %S", (LPWSTR)pInputBuffer));
            SetFileHiddenState((LPWSTR)pInputBuffer, TRUE);
            break;
        }
    case IOCTL_SET_FILE_SHOWN:
        {
            //KdPrint(("SET_FILE_SHOW = %S", (LPWSTR)pInputBuffer));
            SetFileHiddenState((LPWSTR)pInputBuffer, FALSE);
            break;
        }
    case IOCTL_CLEAR_FILE_COUNT:
        {
            //KdPrint(("CLEAR_FILE_COUNT"));
            ClearFileCount();
            break;
        }
    case IOCTL_HIDE_PROCESS:
        {
            //KdPrint(("HIDE_PROCESS = %S", (LPWSTR)pInputBuffer));
            SetProcessHidden((LPWSTR)pInputBuffer, TRUE);
            break;
        }
    case IOCTL_SHOW_PROCESS:
        {
            //KdPrint(("SHOW_PROCESS = %S", (LPWSTR)pInputBuffer));
            SetProcessHidden((LPWSTR)pInputBuffer, FALSE);
            break;
        }
    default:
        break;
    }


	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = 1;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return Irp->IoStatus.Status;
}
