#include "stdafx.h"  
#include <windows.h>  
  
typedef BOOL (WINAPI* ISWOW64PROCESS)(
								    HANDLE hProcess,
								    PBOOL Wow64Process);
ISWOW64PROCESS IsWow64Process;

BOOL IsWow64()
{
	BOOL ret = FALSE;
   IsWow64Process = (ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")),"IsWow64Process");

   if (IsWow64Process)
   {
	   IsWow64Process(GetCurrentProcess(),&ret);
	   /*
	   Wow64Process
	   指向一个bool值，如果进程运行在64位下，该值设置为true。如果进程运行在32位系统下，该值将被设置为false。
       如果该进程是一个64位应用程序，运行在64位系统上，该值也被设置为false。
	   */
   }
	return ret;
}
int main(int argc, char* argv[])  
{  
	if (IsWow64())
	{
		printf("64\n");
	}
	else
		printf("32\n");
    return 0;  
}  