#include <windows.h>
#include <tlhelp32.h>
#include "PSAPI.H"

#include "ToolFunction.h"

#pragma comment(lib, "PSAPI.LIB")

typedef ULONG (WINAPI *PFNNtUnmapViewOfSection)( IN HANDLE ProcessHandle,IN PVOID BaseAddress );

BOOL WINAPI UnmapViewOfModule (DWORD dwProcessId, DWORD dwBaseAddr)
{
	MODULEENTRY32 me32 = {0};
	
	me32.dwSize = sizeof(MODULEENTRY32);

    HMODULE hModule = GetModuleHandle ( TEXT("ntdll.dll") ) ;
    if ( hModule == NULL )
        hModule = LoadLibrary ( TEXT("ntdll.dll") ) ;

    PFNNtUnmapViewOfSection pfnNtUnmapViewOfSection = (PFNNtUnmapViewOfSection)GetProcAddress ( hModule, "NtUnmapViewOfSection" ) ;
    
    HANDLE hProcess = OpenProcess ( PROCESS_ALL_ACCESS, TRUE, dwProcessId ) ;
	HANDLE hModuleSnap   =  CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, dwProcessId);

	if( hModuleSnap == INVALID_HANDLE_VALUE ) 
	{ 
		
		return FALSE; 
	}

	
	if( !Module32First( hModuleSnap, &me32 ) ) 
	{ 
		
		CloseHandle( hModuleSnap );
		return FALSE; 
	}

	do 
	{
		if (!(dwBaseAddr ^ (DWORD)me32.modBaseAddr))
		{
			break;
		}
	} while (Module32Next(hModuleSnap, &me32));


    ULONG    ret = pfnNtUnmapViewOfSection ( hProcess, me32.modBaseAddr) ;
    CloseHandle ( hProcess ) ;
	CloseHandle(hModuleSnap);
    return ret ? FALSE : TRUE ;
}

BOOL WINAPI EnableDebugPriv(LPCTSTR name)//提提权函数
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tp;
    LUID luid;
    //打开进程令牌环
    if(!OpenProcessToken(GetCurrentProcess(),
			TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,
			&hToken))
    {
        //  OpenProcessToken Error
            return FALSE;
    }
    //获得进程本地唯一ID
    if(!LookupPrivilegeValue(NULL, name, &luid))
    {
        //  LookupPrivivlegeValue Error;
		return FALSE;
    }

    tp.PrivilegeCount=1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    tp.Privileges[0].Luid=luid;
    
	//调整权限  
	if(!AdjustTokenPrivileges(hToken, 0, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
    {
         // AdjustTokenPrivileges Error
        return FALSE;
    }
    
	return TRUE;
}

BOOL WINAPI InjectIt(LPCTSTR DllPath, const DWORD dwRemoteProcessld)//注入主函数
{
    HANDLE hrp = NULL;

    if((hrp=OpenProcess(PROCESS_CREATE_THREAD|//允许远程创建线程
				PROCESS_VM_OPERATION|//允许远程VM操作
			   PROCESS_VM_WRITE,//允许远程VM写
				FALSE,dwRemoteProcessld)) == NULL)
    {
        // OpenProcess Error
        return FALSE;
    }

    LPTSTR psLibFileRemote = NULL;

    //使用VirtualAllocEx函数在远程进程的内存地址空间分配DLL文件名缓冲
    psLibFileRemote=(LPTSTR)VirtualAllocEx(hrp, NULL, lstrlen(DllPath)+1,
								  MEM_COMMIT, PAGE_READWRITE);

    if(psLibFileRemote == NULL)
    {
        // VirtualAllocEx Error
        return FALSE;
    }

    //使用WriteProcessMemory函数将DLL的路径名复制到远程的内存空间
    if(WriteProcessMemory(hrp, psLibFileRemote, (void *)DllPath, lstrlen(DllPath)+1, NULL) == 0)
    {
        // WriteProcessMemory Error
        return FALSE;
    }

    //计算LoadLibraryA的入口地址
    PTHREAD_START_ROUTINE pfnStartAddr=(PTHREAD_START_ROUTINE)
        GetProcAddress(GetModuleHandle(TEXT("Kernel32")),"LoadLibraryA");

    if(pfnStartAddr == NULL)
    {
        // GetProcAddress Error!
        return FALSE;
    }
    //pfnStartAddr地址就是LoadLibraryA的入口地址


    HANDLE hrt = NULL;

    if((hrt = CreateRemoteThread(hrp,
        NULL,
        0,
        pfnStartAddr,
        psLibFileRemote,
        0,
        NULL)) == NULL)
    {
        // CreateRemote Error
        return FALSE;
    }
    return TRUE;
}

DWORD WINAPI getpid(LPSTR pn)//得到进程pid
{
    BOOL b = FALSE;
    HANDLE hnd = NULL;
    PROCESSENTRY32 pe = {0};

    //得到进程快照
    hnd=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    pe.dwSize=sizeof(pe);
    b=Process32First(hnd, &pe);

    while(b)
    {
        if(stricmp(pn,pe.szExeFile)==0)
            return pe.th32ProcessID;

        b=Process32Next(hnd,&pe);
    }

	return (-1);
}
