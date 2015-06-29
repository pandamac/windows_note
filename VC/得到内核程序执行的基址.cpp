#include "stdafx.h"
#include <windows.h>
/*
typedef struct _SYSTEM_BASIC_INFORMATION
{
DWORD dwUnknown1; // 0
ULONG uKeMaximumIncrement; // x86: 0x0002625A or 0x00018730
ULONG uPageSize; // bytes
ULONG uMmNumberOfPhysicalPages;
ULONG uMmLowestPhysicalPage;
ULONG uMmHighestPhysicalPage;
ULONG uAllocationGranularity; // bytes
PVOID pLowestUserAddress;
PVOID pMmHighestUserAddress;
KAFFINITY uKeActiveProcessors;
BYTE bKeNumberProcessors;
BYTE bUnknown2;
WORD wUnknown3;
} SYSTEM_BASIC_INFORMATION, *PSYSTEM_BASIC_INFORMATION;

typedef struct _SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION
{
LARGE_INTEGER IdleTime;
LARGE_INTEGER KernelTime;
LARGE_INTEGER UserTime;
LARGE_INTEGER DpcTime;
LARGE_INTEGER InterruptTime;
DWORD InterruptCount;
DWORD dwUnknown1;
} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION, *PSYSTEM_PROCESSOR_PERFORMANCE_INFORMATION

typedef struct _SYSTEM_PERFORMANCE_INFORMATION
{
LARGE_INTEGER liIdleTime;
LARGE_INTEGER IoReadTransferCount;
LARGE_INTEGER IoWriteTransferCount;
LARGE_INTEGER IoOtherTransferCount;
ULONG IoReadOperationCount;
ULONG IoWriteOperationCount;
ULONG IoOtherOperationCount;
ULONG AvailablePages;
ULONG CommittedPages;
ULONG CommitLimit;
ULONG PeakCommitment;
ULONG PageFaultCount;
ULONG CopyOnWriteCount;
ULONG TransitionCount;
ULONG CacheTransitionCount;
ULONG DemandZeroCount;
ULONG PageReadCount;
ULONG PageReadIoCount;
ULONG CacheReadCount;
ULONG CacheIoCount;
ULONG DirtyPagesWriteCount;
ULONG DirtyWriteIoCount;
ULONG MappedPagesWriteCount;
ULONG MappedWriteIoCount;
ULONG PagedPoolPages;
ULONG NonPagedPoolPages;
ULONG PagedPoolAllocs;
ULONG PagedPoolFrees;
ULONG NonPagedPoolAllocs;
ULONG NonPagedPoolFrees;
ULONG FreeSystemPtes;
ULONG ResidentSystemCodePage;
ULONG TotalSystemDriverPages;
ULONG TotalSystemCodePages;
ULONG NonPagedPoolLookasideHits;
ULONG PagedPoolLookasideHits;
ULONG Spare3Count;
ULONG ResidentSystemCachePage;
ULONG ResidentPagedPoolPage;
ULONG ResidentSystemDriverPage;
ULONG CcFastReadNoWait;
ULONG CcFastReadWait;
ULONG CcFastReadResourceMiss;
ULONG CcFastReadNotPossible;
ULONG CcFastMdlReadNoWait;
ULONG CcFastMdlReadWait;
ULONG CcFastMdlReadResourceMiss;
ULONG CcFastMdlReadNotPossible;
ULONG CcMapDataNoWait;
ULONG CcMapDataWait;
ULONG CcMapDataNoWaitMiss;
ULONG CcMapDataWaitMiss;
ULONG CcPinMappedDataCount;
ULONG CcPinReadNoWait;
ULONG CcPinReadWait;
ULONG CcPinReadNoWaitMiss;
ULONG CcPinReadWaitMiss;
ULONG CcCopyReadNoWait;
ULONG CcCopyReadWait;
ULONG CcCopyReadNoWaitMiss;
ULONG CcCopyReadWaitMiss;
ULONG CcMdlReadNoWait;
ULONG CcMdlReadWait;
ULONG CcMdlReadNoWaitMiss;
ULONG CcMdlReadWaitMiss;
ULONG CcReadAheadIos;
ULONG CcLazyWriteIos;
ULONG CcLazyWritePages;
ULONG CcDataFlushes;
ULONG CcDataPages;
ULONG ContextSwitches;
ULONG FirstLevelTbFills;
ULONG SecondLevelTbFills;
ULONG SystemCalls;
}

我们在任务管理器中所见到的所有信息只使用了下面5个调用：
0    SystemBasicInformation
2    SystemPerformanceInformation
5    SystemProcessInformation
8    SystemProcessorPerformanceInformation
21   SystemFileCacheInformation
*/
typedef DWORD    SYSTEM_INFORMATION_CLASS;

typedef enum _SYSTEMINFOCLASS
{
	SystemBasicInformation, //0
	SystemProcessorInformation, // 1
	SystemPerformanceInformation, //2
	SystemTimeOfDayInformation, //3
	SystemPathInformation, //4 SystemNotImplemented1
	SystemProcessInformation, //5  per process SystemProcessesAndThreadsInformation
	SystemCallCountInformation, //6  SystemCallInformation
	SystemConfigurationInformation, //7    SystemDeviceInformation
	SystemProcessorPerformanceInformation, //8  per cpu SystemProcessorCounters
	SystemGlobalFlag, //SystemFlagsInformation
	SystemCallTimeInformation, //10
	SystemModuleInformation, //11
	/*
	SystemLockInformation, //12
	SystemStackTraceInformation, //13  SystemNotImplemented2
	SystemPagedPoolInformation, //14   checked build only
	SystemNonPagedPoolInformation, //15  checked build only
	SystemHandleInformation, //16
	SystemObjectInformation, //17   SystemObjectTypeInformation
	SystemPageFileInformation, //18  per page file
	SystemVdmInstemulInformation, //19  SystemVdmInstemulInformation
	SystemVdmBopInformation, //20
	SystemFileCacheInformation, //21
	SystemPoolTagInformation, //22
	SystemInterruptInformation, //23
	SystemDpcBehaviorInformation, //24
	SystemFullMemoryInformation, //25  checked build only
	SystemLoadGdiDriverInformation, //26  set mode only
	SystemUnloadGdiDriverInformation, //27  set mode only
	SystemTimeAdjustmentInformation, //28  writeable
	SystemSummaryMemoryInformation, //29  checked build only
	SystemNextEventIdInformation, //30  checked build only
	SystemEventIdsInformation, //31  checked build only
	SystemCrashDumpInformation, //32
	SystemExceptionInformation, //33
	SystemCrashDumpStateInformation, //34
	SystemKernelDebuggerInformation, //35
	SystemContextSwitchInformation, //36
	SystemRegistryQuotaInformation, //37
	SystemExtendServiceTableInformation, //38  set mode only  SystemAddDriver
	SystemPrioritySeperation, //39  set mode only    SystemPrioritySeparationInformation
	SystemPlugPlayBusInformation, //40  not implemented
	SystemDockInformation, //41  not implemented
	SystemPowerInformation, //42  XP only
	SystemProcessorSpeedInformation, //43  XP only
	SystemCurrentTimeZoneInformation, //44
	SystemLookasideInformation, //45
	SystemSetTimeSlipEvent, //46
	SystemCreateSession, // set mode only
	SystemDeleteSession, // set mode only
	SystemInvalidInfoClass1, // invalid info class
	SystemRangeStartInformation, // 0x0004 (fails if size != 4)
	SystemVerifierInformation,
	SystemAddVerifier,
	SystemSessionProcessesInformation, // checked build only
	MaxSystemInfoClass
	*/
} SYSTEMINFOCLASS, *PSYSTEMINFOCLASS;

#define MAXIMUM_FILENAME_LENGTH 255

typedef struct _SYSTEM_MODULE {
	ULONG                Reserved1;
	ULONG                Reserved2;
	PVOID                ImageBaseAddress;
	ULONG                ImageSize;
	ULONG                Flags;
	WORD                 Id;
	WORD                 Rank;
	WORD                 w018;
	WORD                 NameOffset;
	BYTE                 Name[MAXIMUM_FILENAME_LENGTH];
} SYSTEM_MODULE, *PSYSTEM_MODULE;

typedef struct _SYSTEM_MODULE_INFORMATION {
	ULONG                ModulesCount;
	SYSTEM_MODULE        Modules[0];
} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;

typedef NTSTATUS (WINAPI *NTQUERYSYSTEMINFORMATION)
(IN      SYSTEM_INFORMATION_CLASS SystemInformationClass,
 IN OUT  PVOID                    SystemInformation,
 IN      ULONG                    SystemInformationLength,
 OUT     PULONG                   ReturnLength  OPTIONAL);
NTQUERYSYSTEMINFORMATION NtQuerySystemInformation;


//#define _K_DEBUG

BOOL GetKernelBase(DWORD*  KernelBase, PCHAR KernelImageName)
{
	NtQuerySystemInformation = (NTQUERYSYSTEMINFORMATION)GetProcAddress(GetModuleHandle(_T("NTDLL")),"NtQuerySystemInformation");
	if (!NtQuerySystemInformation)
	{
		return 1;
	}

	PSYSTEM_MODULE_INFORMATION pModuleInfo;
	ULONG len;
	NtQuerySystemInformation(SystemModuleInformation,NULL,0,&len);

	pModuleInfo = (PSYSTEM_MODULE_INFORMATION)GlobalAlloc(GMEM_ZEROINIT,len);

	NtQuerySystemInformation(SystemModuleInformation,pModuleInfo,len,&len);

#ifdef _K_DEBUG
	for (int i=0;i<pModuleInfo->ModulesCount; i++)
	{ 
		printf("Driver Entry: %s at %p\n",pModuleInfo->Modules[i].Name,\
			pModuleInfo->Modules[i].ImageBaseAddress);
	}
#endif
	//    是内核执行程序的确切基地址
	strcpy(KernelImageName,(const char *)(pModuleInfo->Modules[0].Name));
	*KernelBase = (DWORD)(pModuleInfo->Modules[0].ImageBaseAddress);

	return 0;
}


int _tmain(int argc, _TCHAR* argv[])
{
	DWORD KernelBase;
	char KernelImageName[100];
	GetKernelBase(&KernelBase,KernelImageName);
	printf("driver entry: %s  at  0x%x\n",KernelImageName,KernelBase);
	return 0;
}