#ifndef CTL_CODE
#define CTL_CODE(DeviceType,Function,Method,Access)(\
	((DeviceType)<<16) | ((Access) << 14) | ((Function) << 2 ) | (Method)\
	)
#endif

#define IOCTL_UserInputBuffer CTL_CODE(\
	FILE_DEVICE_UNKNOWN,\
	0x800,\
	METHOD_BUFFERED,FILE_ANY_ACCESS)
//相对安全， pIrp->AssociatedIrp.SystemBuffer 为输入输出缓冲区数据  用UCHAR指针指向
//输入输出长度由PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation( pIrp );
//ULONG inputlen = stack->parameters.DeviceIoControl.InputBufferLength;决定   避免内核态直接操作用户态内存地址的问题

// #define IOCTL_MethodNeither CTL_CODE(\
// 	FILE_DEVICE_UNKNOWN,\
// 	0x801,\
// 	METHOD_NEITHER,\
// 	FILE_ANY_ACCESS)
//跟userbuffer完全不同，直接操作R3的输入内存stack->param.deviceiocontrol.type3inputbuffer 输入缓冲地址 使用前用 ProbeForRead
//pirp->userbuffer 输出缓冲区  ProbeForWrite( 


#define IOCTL_IN_DRECT CTL_CODE(\
	FILE_DEVICE_UNKNOWN,\
	0x802,\
	METHOD_IN_DIRECT,\
	FILE_ANY_ACCESS)
//系统依然对R3输入 进行缓冲，但对R3输出没有缓冲 内核中锁定，R3在驱动完成IO之前都是无法访问的 输入和userbuff一样
// Mmgetsystemaddressformdlsafe(pirp->mdladdress) 映射进内核内存地址 然后可以直接写入 最终解除锁定
