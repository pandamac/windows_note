// TLS.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <windows.h>
#include <winnt.h>


void NTAPI tls_callback(PVOID h, DWORD reason, PVOID pv);//__stdcall

int PEB_BeingDebugged();

extern "C" PIMAGE_TLS_CALLBACK tls_callbacktbl[] = {tls_callback,0};


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.


	return 0;
}



void NTAPI tls_callback(PVOID h, DWORD reason, PVOID pv)//__stdcall
{
/*
其中第一个和第三个参数保留，第二个参数决定函数在那种情况下调用,
一共有四个选项DLL_PROCESS_ATTACH、DLL_THREAD_ATTACH、DLL_THREAD_DETACH和DLL_PROCESS_DETACH。
DLL_PROCESS_ATTACH，是指新进程创建时，在初始化主线程时执行
DLL_THREAD_ATTACH，是指在新进程初始化时执行，但是不包括主线程
DLL_THREAD_DETACH，是指在所有的线程终止时执行，但是同样不包括主线程
DLL_PROCESS_DETACH，是指在进程终止时执行。
*/
	if(reason==DLL_PROCESS_ATTACH)
	{
		if(PEB_BeingDebugged())
			MessageBox(NULL,"TLS CALLBACK FIND DEBUGER","TLS",MB_OK);
		else
			MessageBox(NULL,"TLS CALLBACK NOT FIND DEBUGER","TLS",MB_OK);
	}
	
	return;

};

int PEB_BeingDebugged()
{
	__asm
	{
		mov   eax , fs:[0x30]
		movzx eax, byte ptr [eax+2]
	}
	
}