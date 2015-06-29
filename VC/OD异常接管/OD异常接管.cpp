#include <windows.h>
#include <stdio.h>

int main(void)
{
	//接受异常
	_try
	{
		FARPROC proAPI = GetProcAddress(LoadLibrary("user32.dll"), "MessageBoxA");

		//修改下内存属性
		DWORD dwOldProtect = 0;
		VirtualProtect(proAPI, 0x1000, PAGE_EXECUTE_READWRITE, &dwOldProtect);

		//修改为0xCC
		*(BYTE *)proAPI = 0xCC; //如果不修改内存属性，会提示访问异常
		
		//此时触发异常
		MessageBoxA(NULL, "触发异常", "-(", MB_ICONERROR);

	}

	//处理异常
	_except(1)
	{
		MessageBoxW(NULL, L"我是异常", L"-)", MB_ICONINFORMATION);
	}
	return 0;
}