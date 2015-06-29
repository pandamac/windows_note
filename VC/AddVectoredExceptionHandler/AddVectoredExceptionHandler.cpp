// AddVectoredExceptionHandler.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "AddVectoredExceptionHandler.h"

PVOID (CALLBACK* AddVectoredExceptionHandler)(ULONG FirstHandler,PVOID VectoredHandler);

LONG WINAPI VectoredHandler(
  PEXCEPTION_POINTERS ExceptionInfo
  )
{
	if (ExceptionInfo->ExceptionRecord->ExceptionAddress = (PVOID)0x00408EC8)
		//这个地址是JE跳向失败的第一个地址
	{
	//	MessageBox(NULL,"成功触发CC 异常 VEH","VEH",MB_ICONINFORMATION);
		
		ExceptionInfo->ContextRecord->Eip = 0x00408ED2;
		
		
		DWORD dwpProtect = 0;
		VirtualProtect((PVOID)0x00408EC8,0x1000,PAGE_EXECUTE_READWRITE,&dwpProtect);
		*(PBYTE)0x00408EC8 = 0x6A;
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	
	// 要注入的目标EXE, 自己可能也有许多异常处理.所以要返回继续扫描.
	return EXCEPTION_CONTINUE_SEARCH;
}
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			{
		//		MessageBox(NULL,"DLL注入成功","恭喜",MB_ICONINFORMATION);

				*(FARPROC*)&AddVectoredExceptionHandler = GetProcAddress(GetModuleHandle("kernel32.dll"),\
					"AddVectoredExceptionHandler");
				//添加VEH
				AddVectoredExceptionHandler(1,VectoredHandler);
				Sleep(1000);
				//改进内存分页权限，防止没有写权
				DWORD dwOldProtect = 0;
				VirtualProtect((PVOID)0x00408EC8,0x1000,PAGE_EXECUTE_READWRITE,&dwOldProtect);
				*(PBYTE)0x00408EC8 = 0xcc;

			}
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			{
		//		MessageBox(NULL,"DLL分离","注意",MB_ICONINFORMATION);
			}
			break;
    }
    return TRUE;
}


// This is an example of an exported variable
ADDVECTOREDEXCEPTIONHANDLER_API int nAddVectoredExceptionHandler=0;

// This is an example of an exported function.
ADDVECTOREDEXCEPTIONHANDLER_API int fnAddVectoredExceptionHandler(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see AddVectoredExceptionHandler.h for the class definition
CAddVectoredExceptionHandler::CAddVectoredExceptionHandler()
{ 
	return; 
}

