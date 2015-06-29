#ifndef __IOCTLS_H_
#define __IOCTLS_H_

#ifndef CTL_CODE
#pragma message("CTL_CODE undefined. Include winioctl.h or wdm.h")
#endif

#define IOCTL_TEST1 CTL_CODE(\
	FILE_DEVICE_UNKNOWN, \
	0x800, \
	METHOD_BUFFERED, \
	FILE_ANY_ACCESS)

#endif
