#include <windows.h>
#include <ntsecapi.h>
#include <stdio.h>

#define SystemProcessesAndThreadsInformation	5

// 动态调用
typedef DWORD (WINAPI *ZWQUERYSYSTEMINFORMATION) (DWORD,
												  PVOID,
												  DWORD,
												  PDWORD);

// 结构定义
typedef struct _SYSTEM_PROCESS_INFORMATION{
	DWORD		NextEntryDelta;
	DWORD		ThreadCount;
	DWORD		Reserved1[6];
	FILETIME	ftCreateTime;
	FILETIME	ftUserTime;
	FILETIME	ftKernelTime;
	UNICODE_STRING ProcessName;
	DWORD		BasePriority;
	DWORD		ProcessId;
	DWORD		InheritedFromProcessId;
	DWORD		HandleCount;
	DWORD		Reserved2[2];
	DWORD		VmCounters;
	DWORD		dCommitCharge;
	PVOID		ThreadInfos[1];
}SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;


int main()
{
	// 导出函数
	HMODULE hNtDll = GetModuleHandle("ntdll.dll");
	ZWQUERYSYSTEMINFORMATION ZwQuerySystemInformation = (ZWQUERYSYSTEMINFORMATION)GetProcAddress(hNtDll,"ZwQuerySystemInformation");

	ULONG  cbBuffer = 0x10000;
	LPVOID pBuffer  = NULL;

	pBuffer = malloc(cbBuffer);
	if(pBuffer == NULL)
		return -1;

	// 获取进程信息
	ZwQuerySystemInformation(SystemProcessesAndThreadsInformation,pBuffer,cbBuffer,NULL);
	// 指针指向链表头部
	PSYSTEM_PROCESS_INFORMATION pInfo = (PSYSTEM_PROCESS_INFORMATION)pBuffer;

	// 输出结果
	for(;;)
	{
		printf("PID:%d  \t%ls\n",pInfo->ProcessId,pInfo->ProcessName.Buffer);

		if(pInfo->NextEntryDelta == 0)
			break;

		// 读取下一个节点
		pInfo = (PSYSTEM_PROCESS_INFORMATION)(((PUCHAR)pInfo)+pInfo->NextEntryDelta);
	}
	// 释放缓冲区
	free(pBuffer);

	return 0;
}