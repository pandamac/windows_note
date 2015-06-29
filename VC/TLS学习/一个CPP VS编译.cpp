#include "stdafx.h"
#include <windows.h>
#include <winnt.h>
//下面这行告诉链接器在PE文件中要创建TLS目录
#pragma comment(linker, "/INCLUDE:__tls_used")
/*这是PIMAGE_TLS_CALLBACK()函数的原型，其中第一个和第三个参数保留，第二个参数决定函数在那种情况下*/
void NTAPI my_tls_callback(PVOID h, DWORD reason, PVOID pv)
{
/*一共有四个选项DLL_PROCESS_ATTACH、DLL_THREAD_ATTACH、DLL_THREAD_DETACH和DLL_PROCESS_DETACH。详见微软发布的《Microsoft Portable Executable and Common Object File Format Specification v8》*/
//仅在进程初始化创建主线程时执行的代码
	if( reason == DLL_PROCESS_ATTACH ){
		MessageBox(NULL,L"hi,this is tls callback",L"title",MB_OK);
	}
	return;
}
/*下面这段是创建一个tls段
".CRT$XLB"的含义是：
.CRT表明是使用C RunTime机制
$后面的XLB中
X表示随机的标识
L表示是TLS callback section
B可以被换成B到Y的任意一个字母，但是不能使用".CRT$XLA"和".CRT$XLZ"
因为".CRT$XLA"和".CRT$XLZ"是用于tlssup.obj的
*/
#pragma data_seg(".CRT$XLB")
/*如果要定义多个TLS_CallBack函数，可以把下面这句写成：
PIMAGE_TLS_CALLBACK p_thread_callback [] = {tls_callback_A, tls_callback_B, tls_callback_C,0};
其中tls_callback_B和tls_callback_C应该是你定义好的其他TLS_callBack函数
*/
PIMAGE_TLS_CALLBACK p_thread_callback = my_tls_callback;
#pragma data_seg()

int main(void)
{
	MessageBox(NULL,L"hi,this is main()",L"title",MB_OK);
  return 0;
}
