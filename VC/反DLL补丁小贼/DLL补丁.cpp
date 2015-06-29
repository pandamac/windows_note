// DLL补丁.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "DLL补丁.h"

// 因为VC的SDK版本太老了,SDK中没有 AddVectoredExceptionHandler 这个函数.
// 所以要自己声明 定义...

// 声明
PVOID (CALLBACK *AddVectoredExceptionHandler)(ULONG FirstHandler, PVOID VectoredHandler);



// VEH 堆中  回调函数  要有返回值
LONG WINAPI VectoredHandler(
							PEXCEPTION_POINTERS ExceptionInfo
							)
{
	if(ExceptionInfo->ExceptionRecord->ExceptionAddress == (PVOID)0x0041530D)
	{
		MessageBox(NULL, "成功触发CC异常", ":)", MB_ICONINFORMATION);

		// 修改当前异常中断的EIP
		ExceptionInfo->ContextRecord->Eip = 0x00415216;

		// 改内存分页权限,防止没有写权
		DWORD dwOldProtect = 0;
		VirtualProtect((PVOID)0x0041530D, 0x1000, PAGE_EXECUTE_READWRITE, &dwOldProtect);
		
		// 这里开始修改CC,解发异常
		*(PBYTE)0x0041530D = 0x8b;

		// 表示我处理了异常.
		return EXCEPTION_CONTINUE_EXECUTION;
	}


	// 要注入的目标EXE, 自己可能也有许多异常处理.所以要返回继续扫描.
	return EXCEPTION_CONTINUE_SEARCH;
}

/*
SEH 栈中
_except()
{

}
*/


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			{
				MessageBox(NULL, "DLL注入成功", "恭喜", MB_ICONINFORMATION);


				// 获取 AddVectoredExceptionHandler 函数地址.
				*(FARPROC*) &AddVectoredExceptionHandler = GetProcAddress(GetModuleHandle("kernel32.dll"), 
													  "AddVectoredExceptionHandler");

				// 添加VEH
				AddVectoredExceptionHandler(1, VectoredHandler);

				// 改内存分页权限,防止没有写权
				DWORD dwOldProtect = 0;
				VirtualProtect((PVOID)0x0041530D, 0x1000, PAGE_EXECUTE_READWRITE, &dwOldProtect);

				// 这里开始修改CC,解发异常
				*(PBYTE)0x0041530D = 0xcc;


// 				DWORD dwOldProtect = 0;
// 				VirtualProtect((PVOID)0x00415211, 0x1000, PAGE_EXECUTE_READWRITE, &dwOldProtect);
// 				*(PBYTE)0x00415211 = 0x85;

				break;
			}
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			{
				MessageBox(NULL, "DLL进程分离", ":)", MB_ICONINFORMATION);
			}

			break;
    }
    return TRUE;
}


// This is an example of an exported variable
DLL_API int nDLL=0;

// This is an example of an exported function.
DLL_API int fnDLL(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see DLL补丁.h for the class definition
CDLL::CDLL()
{ 
	return; 
}

