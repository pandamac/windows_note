#include <windows.h>
#include <winnt.h>
int _tls_index=0;
int _tls_start=0;
int _tls_end=0;
int __xl_a=0;
int __xl_z=0;

#pragma data_seg(".tls")
#pragma data_seg(".tls$ZZZ")
#pragma data_seg(".CRT$XLA")
#pragma data_seg(".CRT$XLZ")
#pragma data_seg(".rdata$T")

extern PIMAGE_TLS_CALLBACK tls_callbacktbl[];

IMAGE_TLS_DIRECTORY32 _tls_used=
{
					(DWORD)&_tls_start,
					(DWORD)&_tls_end,
					(DWORD)&_tls_index,
					(DWORD)tls_callbacktbl,
					0,
					0
};

/*将tlssup.c加入自己的工程  然后
右键点这个.c文件  
选择Setting->C/C++->Gategory->Precomliled Headers->Not using precompiled headers 
*/