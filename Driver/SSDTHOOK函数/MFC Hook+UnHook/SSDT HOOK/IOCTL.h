/*
 *                                    SSDT操作程序
 *                                     Made By Adly
 *
 *                                   QQ:369046978
 *
 */
/////////////////////////////////////////////////////////////////////////
//IOCTL.h
#define	FILE_DRIVER_SSDT	0x0000420

#define	DRIVER_NAME		L"\\Device\\HookSSDT"
#define	DEVICE_NAME		L"\\DosDevices\\HookSSDT"

#ifndef CTL_CODE
#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)
#endif // CTL_CODE

#ifndef METHOD_NEITHER
#define METHOD_NEITHER 3
#endif // METHOD_NEITHER

//////////////////////////////////////////////////////////////////////////
//SSDT 结构体
#pragma pack(1)
typedef struct _tagSSDT {
    PVOID pvSSDTBase;
    PVOID pvServiceCounterTable;
    ULONG ulNumberOfServices;
    PVOID pvParamTableBase;
} SSDT, *PSSDT;
#pragma pack()
/////////////////////////////////////////////////////////////////////////
