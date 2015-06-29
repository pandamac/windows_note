#include "stdafx.h"

void testdriverUnload(IN PDRIVER_OBJECT DriverObject);
NTSTATUS testdriverCreateClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS testdriverDefaultHandler(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS DeviceIOControl(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);

#ifdef __cplusplus
extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING  RegistryPath);
#endif

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING  RegistryPath)
{
	UNICODE_STRING DeviceName,Win32Device;
	PDEVICE_OBJECT DeviceObject = NULL;
	NTSTATUS status;
	unsigned i;

	KdPrint(("welcome panda's driver!"));
	//__asm int 3;

	RtlInitUnicodeString(&DeviceName,L"\\Device\\testdriver0");
	RtlInitUnicodeString(&Win32Device,L"\\DosDevices\\testdriver0");

	for (i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++)
		DriverObject->MajorFunction[i] = testdriverDefaultHandler;

	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceIOControl;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = testdriverCreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = testdriverCreateClose;
	
	DriverObject->DriverUnload = testdriverUnload;
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

	DeviceObject->Flags |= DO_DIRECT_IO;
	DeviceObject->AlignmentRequirement = FILE_WORD_ALIGNMENT;
	status = IoCreateSymbolicLink(&Win32Device, &DeviceName);

	DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

	return STATUS_SUCCESS;
}

void testdriverUnload(IN PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING Win32Device;
	RtlInitUnicodeString(&Win32Device,L"\\DosDevices\\testdriver0");
	IoDeleteSymbolicLink(&Win32Device);
	IoDeleteDevice(DriverObject->DeviceObject);
}

NTSTATUS testdriverCreateClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS testdriverDefaultHandler(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	Irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return Irp->IoStatus.Status;
}
NTSTATUS DeviceIOControl(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	Irp->IoStatus.Information = 0;

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);

	Irp->IoStatus.Status = STATUS_SUCCESS;
	ULONG inputlength = stack->Parameters.DeviceIoControl.InputBufferLength;
	ULONG outputlength = stack->Parameters.DeviceIoControl.OutputBufferLength;
	ULONG ioControlCode = stack->Parameters.DeviceIoControl.IoControlCode;

	switch (ioControlCode)
	{
	case IOCTL_MethodNeither:
		//__asm int 3;
		PVOID type3inputbuffer = stack->Parameters.DeviceIoControl.Type3InputBuffer;
		PVOID UserBuffer = Irp->UserBuffer;
		KdPrint(("hello my IOCTL_METHODNETHER CALLED"));
		if (inputlength >=4 && outputlength >= 4)
		{
				
			*(ULONG*)UserBuffer = *(ULONG*)type3inputbuffer;//ÈÎÒâµØÖ·Ð´Â©¶´
			Irp->IoStatus.Information = sizeof(ULONG);
		}
		break;
	}

	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return Irp->IoStatus.Status;
}