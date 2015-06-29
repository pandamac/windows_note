// SetUnhandledExceptionFilter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h> 

DWORD NewEip;
DWORD lpOldHandler;

typedef LPTOP_LEVEL_EXCEPTION_FILTER (_stdcall  *pSetUnhandledExceptionFilter)(
							 LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter 
															);
pSetUnhandledExceptionFilter lpSetUnhandledExceptionFilter;

/*
typedef LONG (WINAPI *PTOP_LEVEL_EXCEPTION_FILTER)(
struct _EXCEPTION_POINTERS *ExceptionInfo
    );
*/
LONG WINAPI TopUnhandledExceptionFilter(
										struct _EXCEPTION_POINTERS *ExceptionInfo
										)
{
	_asm pushad
		MessageBox(NULL,"回调函数被调用","回调函数",MB_OK);
	ExceptionInfo->ContextRecord->Eip=NewEip;
	lpSetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER )lpOldHandler);
	_asm popad
		return EXCEPTION_CONTINUE_EXECUTION;
}

/*void Bug() 
{ 
_asm{
mov  SafeAddr,offset NewEip  
int  3
NewEip:
}
}*/

int main()
{
	lpSetUnhandledExceptionFilter=
		(pSetUnhandledExceptionFilter)GetProcAddress(LoadLibrary(("kernel32.dll")),
		"SetUnhandledExceptionFilter"); 

	lpOldHandler=(DWORD)lpSetUnhandledExceptionFilter(TopUnhandledExceptionFilter);
	
	//  Bug();
	_asm{
		mov  NewEip,offset SafeAddr  
			int  3
SafeAddr:
	}
	
	MessageBox(NULL,"安全位置","成功回到安全位置",MB_OK);
	return 1;
}
