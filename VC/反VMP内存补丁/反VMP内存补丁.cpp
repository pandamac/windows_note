// 反VMP内存补丁.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>

int main(int argc, char* argv[])
{
	//以挂起的方式启动进程
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	memset(&pi,0,sizeof(pi));
	memset(&si,0,sizeof(si));
	BOOL lRet= CreateProcess("5Star.exe",NULL,NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,NULL,&si,&pi);
	if (lRet==FALSE)
	{
		MessageBoxA(0,"Not Find File!",0,0);
		return 0;
	}
	BYTE command[]={0xEB,0xFE};//2个字节让代码一直jmp当前地址

	HMODULE hModule=GetModuleHandle("user32.dll");

	FARPROC lApi = GetProcAddress(hModule,"CreateWindowExA");//以CreateWindowExA为例

	WriteProcessMemory(pi.hProcess,(LPVOID)lApi,&command,2,NULL);//向API写入EBFE

	ResumeThread(pi.hThread);//恢复线程
	BYTE OldApi[2]={0x8B,0xFF};

	CONTEXT context;
	context.ContextFlags=CONTEXT_FULL;
	while(1)//等待到达CreateWindowExA
	{
		GetThreadContext(pi.hThread,&context);
		if (context.Eip==(UINT)lApi)
		{
			SuspendThread(pi.hThread);//当到达API时，挂起进程
			break;
		}
	}
	//此处进行Patch
	DWORD address = 0x00408EC2;
	BYTE WriteData[] = {0x74,0x0E,0x90,0x90,0x90,0x90};

	WriteProcessMemory(pi.hProcess,(LPVOID)address,&WriteData,6,NULL);
	WriteProcessMemory(pi.hProcess,(LPVOID)lApi,&OldApi,2,NULL);//恢复原指令
  ResumeThread(pi.hThread);
return 0;
}