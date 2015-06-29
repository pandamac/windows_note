#include "stdafx.h"

#define IOCTL_ADD_HIDDEN_FILE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x810,METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SET_FILE_HIDDEN CTL_CODE(FILE_DEVICE_UNKNOWN, 0x811,METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SET_FILE_SHOWN CTL_CODE(FILE_DEVICE_UNKNOWN, 0x812,METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_CLEAR_FILE_COUNT CTL_CODE(FILE_DEVICE_UNKNOWN, 0x813,METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HIDE_PROCESS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x815,METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SHOW_PROCESS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x816,METHOD_BUFFERED, FILE_ANY_ACCESS)

#pragma pack(1)
extern "C" typedef struct ServiceDescriptorEntry
{
    unsigned int *ServiceTableBase;
    unsigned int *ServiceCounterTableBase; //仅适用于checked build版本
    unsigned int NumberOfServices;
    unsigned char *ParamTableBase;
} ServiceDescriptorTableEntry_t, *PServiceDescriptorTableEntry_t;
#pragma pack()

extern "C" PServiceDescriptorTableEntry_t KeServiceDescriptorTable;

#define MAX_FILE_COUNT 50

typedef struct _FILE_HIDDEN_TABLE
{
    BOOLEAN bHiddenSwitch[MAX_FILE_COUNT];
    WCHAR szFolderName[MAX_FILE_COUNT][255];
}FILE_HIDDEN_TABLE, *PFILE_HIDDEN_TABLE;


VOID EnablePageProtect();
VOID DisablePageProtect();
VOID SetNtQueryDirectoryFileHook(BOOLEAN bHook );
VOID SetNtQuerySystemInformationHook(BOOLEAN bHook);

VOID SetProcessHidden(LPCWSTR lpProcessName, BOOLEAN bHidden);
ULONG GetSSDTAddr(ULONG uIndex);
ULONG GetSSDTFunctionAddr(ULONG uIndex);
BOOLEAN HookSSDTFunction(ULONG uIndex, ULONG uNewFunctionAddr, ULONG puOriginalFunctionAddr);
BOOLEAN UnHookSSDTFunction(ULONG uIndex, ULONG puOriginalFunctionAddr);

/***********************************************************************/
BOOLEAN CheckFileList(LPWSTR lpFileName);

void InitializeFileHiddenTable();
void AddFileToFileHiddenTable(LPCWSTR lpFileName);
void SetFileHiddenState(LPCWSTR lpFileName, BOOLEAN bHidden);
void ClearFileCount();
/***********************************************************************/

typedef enum WIN_VER_DETAIL {
    WINDOWS_VERSION_UNKNOWN,       // 0
    WINDOWS_VERSION_2K,
    WINDOWS_VERSION_XP,
    WINDOWS_VERSION_2K3,
    WINDOWS_VERSION_2K3_SP1_SP2,
    WINDOWS_VERSION_VISTA,
    WINDOWS_VERSION_7,
} WIN_VER_DETAIL;
typedef NTSTATUS (NTAPI * PFN_RtlGetVersion)(OUT PRTL_OSVERSIONINFOW lpVersionInformation);

VOID CheckWindowsVersion();
WIN_VER_DETAIL GetWindowsVersion();
ULONG GetFunctionAddr(PCWSTR FunctionName);


typedef struct _FILE_ID_BOTH_DIR_INFORMATION {
    ULONG         NextEntryOffset;
    ULONG         FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG         FileAttributes;
    ULONG         FileNameLength;
    ULONG         EaSize;
    CCHAR         ShortNameLength;
    WCHAR         ShortName[12];
    LARGE_INTEGER FileId;
    WCHAR         FileName[1];
} FILE_ID_BOTH_DIR_INFORMATION, *PFILE_ID_BOTH_DIR_INFORMATION;

typedef struct _FILE_FULL_DIR_INFORMATION {
    ULONG         NextEntryOffset;
    ULONG         FileIndex;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER EndOfFile;
    LARGE_INTEGER AllocationSize;
    ULONG         FileAttributes;
    ULONG         FileNameLength;
    ULONG         EaSize;
    WCHAR         FileName[1];
} FILE_FULL_DIR_INFORMATION, *PFILE_FULL_DIR_INFORMATION;

//以下为隐藏进程用的结构
typedef struct _SYSTEM_THREADS
{
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER CreateTime;
    ULONG WaitTime;
    PVOID StartAddress;
    CLIENT_ID ClientIs;
    KPRIORITY Priority;
    KPRIORITY BasePriority;
    ULONG ContextSwitchCount;
    ULONG ThreadState;
    KWAIT_REASON WaitReason;
}SYSTEM_THREADS, *PSYSTEM_THREADS;

typedef struct _SYSTEM_PROCESSES
{
    ULONG NextEntryDelta;
    ULONG ThreadCount;
    ULONG Reserved[6];
    LARGE_INTEGER CreateTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER KernelTime;
    UNICODE_STRING ProcessName;
    KPRIORITY BasePriority;
    ULONG ProcessId;
    ULONG InheritedFromProcessId;
    ULONG HandleCount;
    ULONG Reserved2[2];
    VM_COUNTERS VmCounters;
    IO_COUNTERS IoCounters; //windows 2000 only
    struct _SYSTEM_THREADS Threads[1];
}SYSTEM_PROCESSES, *PSYSTEM_PROCESSES;


NTSTATUS NewNtQuerySystemInformation(
    ULONG SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength
    );


typedef NTSTATUS (*PFNNtQuerySystemInformation)(
    ULONG SystemInformationCLass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength
    );


//先申明NtQueryDirectoryFile
NTSTATUS NewNtQueryDirectoryFile(
    HANDLE FileHandle,
    HANDLE Event,
    PIO_APC_ROUTINE ApcRoutine,
    PVOID ApcContext,
    PIO_STATUS_BLOCK IoStatusBlock,
    PVOID FileInformation,
    ULONG Length,
    FILE_INFORMATION_CLASS FileInformationClass,
    BOOLEAN ReturnSingleEntry,
    PUNICODE_STRING FileName,
    BOOLEAN RestartScan
    );


//定义NtQueryDirectoryFile的原型
typedef NTSTATUS (*PFNNtQueryDirectoryFile)(
    HANDLE FileHandle,
    HANDLE Event,
    PIO_APC_ROUTINE ApcRoutine,
    PVOID ApcContext,
    PIO_STATUS_BLOCK IoStatusBlock,
    PVOID FileInformation,
    ULONG Length,
    FILE_INFORMATION_CLASS FileInformationClass,
    BOOLEAN ReturnSingleEntry,
    PUNICODE_STRING FileName,
    BOOLEAN RestartScan
    );

