#include "ntddk.h"
#include <ntddkbd.h>
#include "stdarg.h"
#include "stdio.h"
#include "ctrl2cap.h"
#include "ntddkbd.h"
#undef WIN2K

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
/*
这是只适用于具有C链接-这是声明的函数，函数声明为extern“C”联动与规范。
如果你尝试使用这个程序在C + +的联动功能，产生一个编译错误。
*/
#pragma alloc_text (PAGE, Ctrl2capDispatchGeneral)
#if WIN2K
#pragma alloc_text (PAGE, Ctrl2capAddDevice)
#pragma alloc_text (PAGE, Ctrl2capUnload)
#pragma alloc_text (PAGE, Ctrl2capPnP)
#pragma alloc_text (PAGE, Ctrl2capPower)
#endif // WIN2K
#endif // ALLOC_PRAGMA



NTSTATUS DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING RegistryPath 
    )
{
    ULONG                  i;

    DbgPrint (("Ctrl2cap.SYS: entering DriverEntry\n"));

 
    for (i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)//0x1b个IRP进行过滤
  {
    
        DriverObject->MajorFunction[i] = Ctrl2capDispatchGeneral;
    }

    DriverObject->MajorFunction[IRP_MJ_READ] = Ctrl2capDispatchRead;

#if WIN2K
    //
    // Power IRPs are the only ones we have to handle specially under
    // Win2k since they require the special PoCallDriver and 
    // PoStartNextPowerIrp function calls.
    //
  DbgPrint (("WIN2K \n"));
    DriverObject->MajorFunction [IRP_MJ_POWER]  = Ctrl2capPower;

    //
    // The only reason we need to handle PnP IRPs is to know when
    // a device we've attached to disappears (is removed).
    //
    DriverObject->MajorFunction [IRP_MJ_PNP]  = Ctrl2capPnP;

    //
    // Under Win2K we get told about the presence of keyboard 
    // devices through our AddDevice entry point.
    //
    DriverObject->DriverUnload = Ctrl2capUnload;
    DriverObject->DriverExtension->AddDevice = Ctrl2capAddDevice;

    return STATUS_SUCCESS;

#else  // WIN2K

    //负责挂载过滤驱动
    return Ctrl2capInit( DriverObject );
#endif // WIN2K
}

NTSTATUS Ctrl2capInit( 
    IN PDRIVER_OBJECT DriverObject 
    )
{
    CCHAR         ntNameBuffer[64];
    STRING          ntNameString;
    UNICODE_STRING    ntUnicodeString;
    PDEVICE_OBJECT    device;
    NTSTATUS          status;
    PDEVICE_EXTENSION devExt;
    WCHAR             messageBuffer[]  = L"Ctrl2cap Initialized\n";
    UNICODE_STRING    messageUnicodeString;

  DbgPrint (("Ctrl2capInit \n"));
    //
    // Only hook onto the first keyboard's chain.
    //
    sprintf( ntNameBuffer, "\\Device\\KeyboardClass0" );
    RtlInitAnsiString( &ntNameString, ntNameBuffer );
    RtlAnsiStringToUnicodeString( &ntUnicodeString, &ntNameString, TRUE );

   //创建设备对象
    status = IoCreateDevice( DriverObject,
                             sizeof(DEVICE_EXTENSION),
                             NULL,
                             FILE_DEVICE_KEYBOARD,
                             0,
                             FALSE,
                             &device );

    if( !NT_SUCCESS(status) ) 
  {

        DbgPrint(("Ctrl2cap: Keyboard hook failed to create device!\n"));

        RtlFreeUnicodeString( &ntUnicodeString );
        return STATUS_SUCCESS;
    }

    RtlZeroMemory(device->DeviceExtension, sizeof(DEVICE_EXTENSION));

    devExt = (PDEVICE_EXTENSION) device->DeviceExtension;
   
    //
    // Keyboard uses buffered I/O so we must as well.
    //
    device->Flags |= DO_BUFFERED_IO;
    device->Flags &= ~DO_DEVICE_INITIALIZING;

    //
    // Attach to the keyboard chain.
    //
    status = IoAttachDevice( device, &ntUnicodeString, &devExt->TopOfStack );
  //Pointer to caller-allocated storage for a pointer. 
  //On return, contains a pointer to the target device object if the attachment succeeds.
    
  if( !NT_SUCCESS(status) )
  {

        DbgPrint(("Ctrl2cap: Connect with keyboard failed!\n"));
        IoDeleteDevice( device );
        RtlFreeUnicodeString( &ntUnicodeString );
        return STATUS_SUCCESS;
    }

    //
    // Done! Just free our string and be on our way...
    //
    RtlFreeUnicodeString( &ntUnicodeString );
    DbgPrint(("Ctrl2cap: Successfully connected to keyboard device\n"));

    //
    // This line simply demonstrates how a driver can print
    // stuff to the bluescreen during system initialization. 
    //
    RtlInitUnicodeString (&messageUnicodeString,
                          messageBuffer );
    ZwDisplayString( &messageUnicodeString );

    return STATUS_SUCCESS;
}


#if WIN2K
//----------------------------------------------------------------------
//
// Ctrl2capAddDevice
//
// The PnP Manager calls us for each keyboard present on the system.
// We attach to each one so that we can flip caps lock to controls.
//
// Called: WIN2K
//
//----------------------------------------------------------------------
NTSTATUS
Ctrl2capAddDevice(
    IN PDRIVER_OBJECT   Driver,
    IN PDEVICE_OBJECT   PDO
    )
{
    PDEVICE_EXTENSION        devExt;
    IO_ERROR_LOG_PACKET      errorLogEntry;
    PDEVICE_OBJECT           device;
    NTSTATUS                 status = STATUS_SUCCESS;

    //
    // Create a filter device and attach it to the device stack.
    
    //
    
    DbgPrint(("Ctrl2capAddDevice\n"));
    status = IoCreateDevice(Driver,                   
                            sizeof(DEVICE_EXTENSION), 
                            NULL,                    
                            FILE_DEVICE_KEYBOARD,   
                            0,                     
                            FALSE,                
                            &device              
                            );

    if (!NT_SUCCESS(status)) {

        return (status);
    }

    RtlZeroMemory(device->DeviceExtension, sizeof(DEVICE_EXTENSION));

    devExt = (PDEVICE_EXTENSION) device->DeviceExtension;
    devExt->TopOfStack = IoAttachDeviceToDeviceStack(device, PDO);

    ASSERT(devExt->TopOfStack);

    device->Flags |= (DO_BUFFERED_IO | DO_POWER_PAGABLE);
    device->Flags &= ~DO_DEVICE_INITIALIZING;
    return status;
}


//----------------------------------------------------------------------
//
// Ctrl2capPnP
//
// We have to handle PnP IRPs so that we detach from target
// devices when appropriate.
//
// Called: WIN2K
//
//----------------------------------------------------------------------
NTSTATUS Ctrl2capPnP(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp 
    )
{
    PDEVICE_EXTENSION           devExt; 
    PIO_STACK_LOCATION          irpStack;
    NTSTATUS                    status = STATUS_SUCCESS;
    KIRQL                       oldIrql;
    KEVENT                      event;        

    devExt = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    irpStack = IoGetCurrentIrpStackLocation(Irp);

    switch (irpStack->MinorFunction) {
    case IRP_MN_REMOVE_DEVICE:
        
        //
        // Detach from the target device after passing the IRP
        // down the devnode stack.
        //
        IoSkipCurrentIrpStackLocation(Irp);
        IoCallDriver(devExt->TopOfStack, Irp);

        IoDetachDevice(devExt->TopOfStack); 
        IoDeleteDevice(DeviceObject);

        status = STATUS_SUCCESS;
        break;

    case IRP_MN_SURPRISE_REMOVAL:

        //
        // Same as a remove device, but don't call IoDetach or IoDeleteDevice.
        //
        IoSkipCurrentIrpStackLocation(Irp);
        status = IoCallDriver(devExt->TopOfStack, Irp);
        break;

    case IRP_MN_START_DEVICE: 
    case IRP_MN_QUERY_REMOVE_DEVICE:
    case IRP_MN_QUERY_STOP_DEVICE:
    case IRP_MN_CANCEL_REMOVE_DEVICE:
    case IRP_MN_CANCEL_STOP_DEVICE:
    case IRP_MN_FILTER_RESOURCE_REQUIREMENTS: 
    case IRP_MN_STOP_DEVICE:
    case IRP_MN_QUERY_DEVICE_RELATIONS:
    case IRP_MN_QUERY_INTERFACE:
    case IRP_MN_QUERY_CAPABILITIES:
    case IRP_MN_QUERY_DEVICE_TEXT:
    case IRP_MN_QUERY_RESOURCES:
    case IRP_MN_QUERY_RESOURCE_REQUIREMENTS:
    case IRP_MN_READ_CONFIG:
    case IRP_MN_WRITE_CONFIG:
    case IRP_MN_EJECT:
    case IRP_MN_SET_LOCK:
    case IRP_MN_QUERY_ID:
    case IRP_MN_QUERY_PNP_DEVICE_STATE:
    default:
        //
        // Pass these through untouched
        //
        IoSkipCurrentIrpStackLocation(Irp);
        status = IoCallDriver(devExt->TopOfStack, Irp);
        break;
    }

    return status;
}


//----------------------------------------------------------------------
//
// Ctrl2capPower
//
// We have to handle Power IRPs specially.
//
// Called: WIN2K
//
//----------------------------------------------------------------------
NTSTATUS Ctrl2capPower(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp 
    )
{
    PDEVICE_EXTENSION   devExt;
    
    devExt = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;

    //
    // Let the next power IRP out of the gate
    // 
    PoStartNextPowerIrp( Irp );
    
    //
    // Pass this power IRP to the keyboard class driver
    //
    IoSkipCurrentIrpStackLocation( Irp );
    
    return PoCallDriver( devExt->TopOfStack, Irp );
}


//----------------------------------------------------------------------
// 
// Ctrl2capUnload
//
// Our Win2K PnP unload function. We don't need to do anything.
//
// Called: WIN2K
//
//----------------------------------------------------------------------
VOID
Ctrl2capUnload(
   IN PDRIVER_OBJECT Driver
   )
{
  KdPrint(("Ctrl2capUnload"));
    UNREFERENCED_PARAMETER(Driver);

    ASSERT(NULL == Driver->DeviceObject);
}
#endif // WIN2K


//----------------------------------------------------------------------
// 
// Ctrl2capReadComplete
//
// Gets control after a read operation has completed.
//
// Called: WIN2K, NT4
//
//----------------------------------------------------------------------
NTSTATUS Ctrl2capReadComplete( //完成例程
    IN PDEVICE_OBJECT DeviceObject, 
    IN PIRP Irp,
    IN PVOID Context 
    )
{
    PIO_STACK_LOCATION        IrpSp;
    PKEYBOARD_INPUT_DATA      KeyData;
    int                       numKeys, i;

   
    IrpSp = IoGetCurrentIrpStackLocation( Irp );
    if( NT_SUCCESS( Irp->IoStatus.Status ) ) {

        //
        // Do caps-lock down and caps-lock up. Note that
        // just frobbing the MakeCode handles both the up-key
        // and down-key cases since the up/down information is specified
        // seperately in the Flags field of the keyboard input data 
        // (0 means key-down, 1 means key-up).
        //
        KeyData = Irp->AssociatedIrp.SystemBuffer;
        numKeys = Irp->IoStatus.Information / sizeof(KEYBOARD_INPUT_DATA);

        for( i = 0; i < numKeys; i++ ) {

            DbgPrint(("ScanCode: %x ", KeyData[i].MakeCode ));
            DbgPrint(("%s\n", KeyData[i].Flags ? "Up" : "Down" ));

            if( KeyData[i].MakeCode == CAPS_LOCK) 
      {//修改键盘扫描码
                KeyData[i].MakeCode = LCONTROL;
            } 
        }
    }


    if( Irp->PendingReturned ) 
  {//如果需要阻塞  调用这个函数
        IoMarkIrpPending( Irp );
    }
    return Irp->IoStatus.Status;
}


//----------------------------------------------------------------------
//
// Ctrl2capDispatchRead
//
// Sets up to look at the read request completion so that we can
// massage the input queue on IO completion.
//
// Called: WIN2K, NT4
//
//----------------------------------------------------------------------
NTSTATUS Ctrl2capDispatchRead( 
    IN PDEVICE_OBJECT DeviceObject, 
    IN PIRP Irp )
{
    PDEVICE_EXTENSION   devExt;
    PIO_STACK_LOCATION  currentIrpStack;
    PIO_STACK_LOCATION  nextIrpStack;

    //
    // Gather our variables.
    //
    devExt = (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
    nextIrpStack = IoGetNextIrpStackLocation(Irp);    

    //
    // Push params down for keyboard class driver.
    //
    *nextIrpStack = *currentIrpStack;

    //  
    // Set the completion callback, so we can "frob" the keyboard data.
    //      
    IoSetCompletionRoutine( Irp, Ctrl2capReadComplete, 
                            DeviceObject, TRUE, TRUE, TRUE );

    //
    // Return the results of the call to the keyboard class driver.
    //调用底层驱动  
  //The IoCallDriver routine sends an IRP to the driver associated with a specified device object.

    return IoCallDriver( devExt->TopOfStack, Irp );
}

NTSTATUS Ctrl2capDispatchGeneral(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp 
    )
{

#if WIN2K
    IoSkipCurrentIrpStackLocation(Irp);
#else // WIN2K
   
    Irp->CurrentLocation++;
    Irp->Tail.Overlay.CurrentStackLocation++;
#endif // WIN2K
    return IoCallDriver(((PDEVICE_EXTENSION) DeviceObject->DeviceExtension)->TopOfStack, Irp);
}

