// ExeOfInjectDll.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>

BOOL EnablePrivilege()//提升当前进程权限
{
    HANDLE hToken = NULL;
    BOOL bFlag = FALSE;
	
    if(OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&hToken))//获取进程的令牌信息
    {
        TOKEN_PRIVILEGES tp;
        tp.PrivilegeCount = 1;
        if (!LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&tp.Privileges[0].Luid))//
        {
            CloseHandle(hToken);
            return FALSE;
        }
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        if (!AdjustTokenPrivileges(hToken ,FALSE ,&tp, sizeof(tp), NULL, NULL))
        {
            //AfxMessageBox("修改权限失败");
            return FALSE;
        }
    }
    CloseHandle(hToken);
    return TRUE;
}

int main(int argc, char* argv[])//调用DLL
{
	EnablePrivilege();
	char szDllPath[] = "g:\\beiyong\\biancheng\\test\\dll断链\\injectdll断链\\debug\\injectdll断链.dll";
						//"injectdll断链.dll";
	HMODULE handle = LoadLibrary(szDllPath);
	
	
	return 0;
}

