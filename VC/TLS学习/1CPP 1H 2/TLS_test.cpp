// TLS_test.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "TLS_test.h"
#include <windows.h>
#include <winnt.h>

#pragma comment(linker, "/INCLUDE:__tls_used")
/*这是PIMAGE_TLS_CALLBACK()函数的原型，其中第一个和第三个参数保留，第二个参数决定函数在那种情况下*/
void NTAPI my_tls_callback1(PVOID h, DWORD reason, PVOID pv)
{
	/*一共有四个选项DLL_PROCESS_ATTACH、DLL_THREAD_ATTACH、DLL_THREAD_DETACH和DLL_PROCESS_DETACH。详见微软发布的《Microsoft Portable Executable and Common Object File Format Specification v8》*/
	//仅在进程初始化创建主线程时执行的代码
	if( reason == DLL_PROCESS_ATTACH ){
		MessageBox(NULL,"hi,this is tls callback","title",MB_OK);
	}
	return;
}
 //.CRT表明是使用C RunTime机制,$后面的XLA中X表示随机的标识,L表示是TLS callback section,A是任意的

#pragma data_seg(".CRT$XLB")
/*如果要定义多个TLS_CallBack函数，可以把下面这句写成：
PIMAGE_TLS_CALLBACK p_thread_callback [] = {tls_callback_A, tls_callback_B, tls_callback_C,0};
其中tls_callback_B和tls_callback_C应该是你定义好的其他TLS_callBack函数
*/
extern "C" PIMAGE_TLS_CALLBACK my_tls_callbacktbl[] = {TlsCallBackFunction1, 0};
#pragma data_seg()

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    MessageBox(NULL, TEXT("hello world"), TEXT("Tls test"), MB_OK);
	return 0;
}

void NTAPI TlsCallBackFunction1(PVOID h, DWORD reason, PVOID pv)
{
	/*一共有四个选项DLL_PROCESS_ATTACH、DLL_THREAD_ATTACH、DLL_THREAD_DETACH和DLL_PROCESS_DETACH。详见微软发布的《Microsoft Portable Executable and Common Object File Format Specification v8》*/
	//仅在进程初始化创建主线程时执行的代码
	if( reason == DLL_PROCESS_ATTACH ){
		MessageBox(NULL,"It's my test","tls test",MB_OK);
	}
	return;
}
