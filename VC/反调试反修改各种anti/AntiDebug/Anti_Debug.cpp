#include "StdAfx.h"
#include "Anti_Debug.h"

Anti_Debug::Anti_Debug(void)
{
}

Anti_Debug::~Anti_Debug(void)
{
}

// 进程名
BOOL Anti_Debug::Anti_Debug_ProcessName()
{
	BOOL bResult = FALSE;
	TCHAR szOLLYDBG[] = _T("OLLYDBG.EXE");  //要检测的OD进程名
	TCHAR sz52[] = _T("吾爱破解.EXE");
	TCHAR szICey[] = _T("ICEYOD.EXE");
	TCHAR szODICE[] = _T("OLLYICE.EXE");
	TCHAR szWINDBG[] = _T("WINGUARD.EXE");
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnap)
	{
		return FALSE;
	}
	if (Process32First(hSnap, &pe))
	{
		do
		{	// 不区分大小写
			if (lstrcmpi(szOLLYDBG, pe.szExeFile) == 0 || 
				lstrcmpi(sz52, pe.szExeFile) == 0||
				lstrcmpi(szICey, pe.szExeFile) == 0 ||
				lstrcmpi(szODICE, pe.szExeFile) == 0||
				lstrcmpi(szWINDBG, pe.szExeFile) == 0)
			{
				bResult = TRUE;
			}
		} while (Process32Next(hSnap, &pe));
	}
	CloseHandle(hSnap);
	if (bResult)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

// 窗口类名
BOOL Anti_Debug::Anti_Debug_ClassName()
{
	TCHAR szWindowClass[] = _T("OLLYDBG");

	if(::FindWindow(szWindowClass, NULL)) //第二个参数为NULL表示查询所有窗口类名是OLLYDBG
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


// 父进程
BOOL Anti_Debug::Anti_Debug_ParentProcess()
{
	BOOL bResult = FALSE;
	TCHAR szCurrentAndParentIdAndExplorerId[MAX_PATH] = {0};//定义当前程序ID、它的父窗口及桌面进程ID

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);

	DWORD dwExplorerId; 
	//获取explorer.exe进程的ID，桌面进程的类名是"Progman"
	//注意返回的进程ID是在GetWindowThreadProcessId函数的第二个参数
	//GetWindowThreadProcessId函数返回的是创建explorer.exe线程的句柄
	::GetWindowThreadProcessId(::FindWindow(_T("Progman"), NULL), &dwExplorerId);

	Process32First(hSnap, &pe);
	do
	{
		if (GetCurrentProcessId() == pe.th32ProcessID)  //遍历到当前进程
		{
			//当前进程的父进程ID如果不是explorer.exe进程的ID号，一般表示被调试了
			if (dwExplorerId != pe.th32ParentProcessID)
			{
				bResult = TRUE;
			}
		}

	} while (Process32Next(hSnap, &pe));

	/*wsprintf(szCurrentAndParentIdAndExplorerId, _T("当前ID：%u，父进程ID：%u，桌面进程ID：%u"), 
	GetCurrentProcessId(), 
	pe.th32ParentProcessID,
	dwExplorerId);

	::MessageBox(NULL, szCurrentAndParentIdAndExplorerId, _T("结果"), 0);*/

	if (bResult)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	CloseHandle(hSnap);
}

// 函数int3断点
BOOL Anti_Debug::Anti_Debug_FuncInt3(char* FuncName) 
{
	BOOL bODFound = FALSE;
	DWORD dwAddr;
	dwAddr = (DWORD)GetProcAddress(LoadLibrary(_T("user32.dll")), FuncName);

	__asm
	{
		cld	// clear Direction flag
			mov edi, dwAddr 
			mov ecx, 100h //ecx为循环次数
			mov al, 055h	
			xor al, 099h //al为循环次数
			repne scasb //执行edi++和ecx--操作，[edi]和al比较，相等就不再进行edi++和ecx--操作
			cmp ecx, 0h	
			jz ODNotFound
			mov bODFound, 1
ODNotFound:
	}
	if (bODFound)
	{
		return TRUE;
	} 
	else
	{
		return FALSE;
	}
}

// code段检测
BOOL Anti_Debug::Anti_Debug_CodeSectionInt3()
{
	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS32 pNtHeaders;
	PIMAGE_SECTION_HEADER pSectionHeader;
	DWORD dwBaseImage = (DWORD)GetModuleHandle(NULL); //获取当前进程的基址


	pDosHeader = (PIMAGE_DOS_HEADER)dwBaseImage;
	pNtHeaders = (PIMAGE_NT_HEADERS32)((DWORD)pDosHeader + pDosHeader->e_lfanew);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pNtHeaders + sizeof(pNtHeaders->Signature) + sizeof(IMAGE_FILE_HEADER) + 
		(WORD)pNtHeaders->FileHeader.SizeOfOptionalHeader);

	DWORD dwAddr = pSectionHeader->VirtualAddress + dwBaseImage; //获取代码段在内存中的地址
	DWORD dwCodeSize = pSectionHeader->SizeOfRawData;    //获取代码段对齐后的大小
	BOOL bFound = FALSE;

	__asm
	{
			cld               ;检测代码开始
			mov edi, dwAddr
			mov ecx, dwCodeSize //检测整个代码段
			mov al, 055h 
			xor al, 099h
			repne scasb
			cmp ecx, 0h	//等于0表示没有检测到cc
			jz     NotFound
			mov bFound,1
NotFound:             
	}
	if (bFound)
	{
		return TRUE;
	} 
	else
	{
		return FALSE;
	}
}

//定义PROCESSINFOCLASS结构体
typedef enum _PROCESSINFOCLASS {
	ProcessBasicInformation, // 0 Y N
	ProcessQuotaLimits, // 1 Y Y
	ProcessIoCounters, // 2 Y N
	ProcessVmCounters, // 3 Y N
	ProcessTimes, // 4 Y N
	ProcessBasePriority, // 5 N Y
	ProcessRaisePriority, // 6 N Y
	ProcessDebugPort, // 7 Y Y
	ProcessExceptionPort, // 8 N Y
	ProcessAccessToken, // 9 N Y
	ProcessLdtInformation, // 10 Y Y
	ProcessLdtSize, // 11 N Y
	ProcessDefaultHardErrorMode, // 12 Y Y
	ProcessIoPortHandlers, // 13 N Y
	ProcessPooledUsageAndLimits, // 14 Y N
	ProcessWorkingSetWatch, // 15 Y Y
	ProcessUserModeIOPL, // 16 N Y
	ProcessEnableAlignmentFaultFixup, // 17 N Y
	ProcessPriorityClass, // 18 N Y
	ProcessWx86Information, // 19 Y N
	ProcessHandleCount, // 20 Y N
	ProcessAffinityMask, // 21 N Y
	ProcessPriorityBoost, // 22 Y Y
	ProcessDeviceMap,// 23 Y Y
	ProcessSessionInformation, // 24 Y Y
	ProcessForegroundInformation, // 25 N Y
	ProcessWow64Information // 26 Y N
} PROCESSINFOCLASS;

//自定义NtQueryInformationProcess函数指针
typedef NTSTATUS (WINAPI *MYFUNC)
( 
 __in          HANDLE ProcessHandle,
 __in          PROCESSINFOCLASS ProcessInformationClass,
 __out         PVOID ProcessInformation,
 __in          ULONG ProcessInformationLength,
 __out_opt     PULONG ReturnLength
 );

//CheckRemoteDebuggerPresent内部实现原理NtQueryInformationProcess
//必须自己定义
BOOL Anti_Debug::Anti_Debug_NtInformationProcess()
{
	HANDLE hProcess = GetCurrentProcess();
	DWORD dwResult;

	MYFUNC NtQueryInformationProcess = (MYFUNC)GetProcAddress(::LoadLibrary(_T("ntdll.dll")), 
		"NtQueryInformationProcess");
	NtQueryInformationProcess(hProcess, ProcessDebugPort, &dwResult, 4, 0);
	if (dwResult != 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL Anti_Debug::Anti_Debug_Checkremotedebuggerpresent()
{
	HANDLE hProcess = ::GetCurrentProcess();
	BOOL bDebuggerPresent = FALSE;

	CheckRemoteDebuggerPresent(hProcess, &bDebuggerPresent);
	if (TRUE == bDebuggerPresent)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//原理:Windows操作系统中的explorer.exe创建进程的时候会把STARTUPINFO结构中的值设为0,
//而非explorer.exe创建进程的时候会忽略这个结构中的值，也就是结构中的值不为0，
//所以可以利用这个来判断OD是否在调试程序.
BOOL Anti_Debug::Anti_Debug_GetStartupInfo()
{
	STARTUPINFO info = {0};
	GetStartupInfo(&info); //获取结构体的信息

	if (info.dwX!=0 || info.dwY!=0 || info.dwXCountChars!=0 || info.dwYCountChars!=0
		|| info.dwFillAttribute!=0 || info.dwXSize!=0 || info.dwYSize!=0)
	{
		return TRUE;
	}
	else
	{
		FALSE;
	}
}



//必须单步调试才能成功   
//个人感觉没啥用
BOOL Anti_Debug::Anti_Debug_GetTickCount()
{
	DWORD start = GetTickCount();
	//一些指令
	GetCurrentProcessId();
	GetCurrentProcessId();
	GetCurrentProcessId();
	GetCurrentProcessId();
	/*
	sleep(50)  比如也可以换成这样的
	*/
	DWORD end = GetTickCount();
	if ((end - start) > 100)
	{
		return TRUE;
	} 
	else
	{
		return FALSE;
	}
}