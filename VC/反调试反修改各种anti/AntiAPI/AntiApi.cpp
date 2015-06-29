#include "StdAfx.h"
#include "AntiApi.h"
#include "windows.h"
#include <STDLIB.H>

CAntiApi::CAntiApi(void)
{
}

CAntiApi::~CAntiApi(void)
{
}
void CAntiApi::AntiCC(char * m_ModuleName,char * m_ApiName)
{
	HMODULE hModule = GetModuleHandleA(m_ModuleName);

	FARPROC lpAPI = GetProcAddress(hModule, m_ApiName);

	_try
	{
		for (int i = 0; i < 0x100; i++)
		{
			if (*((PBYTE)lpAPI+i) == 0xcc)	//检测到断点
			{
				*((PBYTE)lpAPI+i) = 0xcc;	//抛出异常
			}
		}


	}

	//处理异常
	_except(1)	//EXCEPTION_EXECUTE_HANDLER = 1
	{
		ExitProcess(0);
	}

}

void CAntiApi::AntiPage_Protect(char * m_ModuleName, char * m_ApiName)
{
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
	HMODULE hModule = GetModuleHandleA(m_ModuleName);
	FARPROC lpApi = GetProcAddress(hModule, m_ApiName);
	DWORD dwOldProtect = 0;
	VirtualProtectEx(handle, (LPVOID)lpApi, 0x1000, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	
}

void CAntiApi::AntiHardware()
{
	// TODO: Add your control notification handler code here
	CONTEXT m_context = {CONTEXT_DEBUG_REGISTERS};
	HANDLE hThread = GetCurrentThread();
	GetThreadContext(hThread,&m_context);

	m_context.Dr0 = 0;
	m_context.Dr1 = 0;
	m_context.Dr2 = 0;
	m_context.Dr3 = 0;
	//m_context.Dr6 = 0;
	m_context.Dr7 = 0;

	SetThreadContext(hThread, &m_context);

}