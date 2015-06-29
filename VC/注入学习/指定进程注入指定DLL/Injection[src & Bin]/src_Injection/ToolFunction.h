#ifndef _TOOLFUNC_H_
#define _TOOLFUNC_H_

#include <windows.h>

BOOL WINAPI UnmapViewOfModule (DWORD dwProcessId, DWORD dwBaseAddr);
BOOL WINAPI EnableDebugPriv(LPCTSTR name);//提提权函数
BOOL WINAPI InjectIt(LPCTSTR DllPath, const DWORD dwRemoteProcessld);//注入主函数
DWORD WINAPI getpid(LPSTR pn);//得到进程pid

#endif