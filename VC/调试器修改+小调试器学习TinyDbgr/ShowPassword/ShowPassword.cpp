// ShowPassword.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>

// 需要设置INT3断点的位置
#define BP_VA   0x00401E9E//  |. E8 AD030000    CALL <JMP.&MSVCRTD.strcmp>               ; \strcmp


// INT3的机器码
const BYTE bInt3 = '\xCC';

int main(int argc, char* argv[])
{
    if ( argc != 2 )
    {
        printf("usage: %s crackme.exe \r\n", argv[0]);
        return -1;
    }

    // 保存文件名
    char *pszFileName = argv[1];

    // 启动信息
    STARTUPINFO si = { 0 };
    si.cb = sizeof(STARTUPINFO);
    GetStartupInfo(&si);

    // 进程信息
    PROCESS_INFORMATION pi = { 0 };

    // 创建被调试进程
    BOOL bRet = CreateProcess(pszFileName, 
                        NULL,
                        NULL,
                        NULL,
                        FALSE,
                        DEBUG_PROCESS | DEBUG_ONLY_THIS_PROCESS,
                        NULL,
                        NULL,
                        &si,
                        &pi);

    if ( bRet == FALSE )
    {
        printf("CreateProcess Error \r\n");
        return -1;
    }

    DEBUG_EVENT de = { 0 };
    CONTEXT context = { 0 };
    // 保存原始的机器码
    BYTE bOldByte = 0;
    DWORD dwReadWriteNum = 0;

    // 保存正确密码使用
    char pszPassword[MAXBYTE] = { 0 };
    // 保存错误密码使用
    char pszErrorPass[MAXBYTE] = { 0 };

    while ( TRUE )
    {
        // 获取调试事件
        WaitForDebugEvent(&de, INFINITE);

        // 判断事件类型
        switch ( de.dwDebugEventCode )
        {
            // 创建进程时的调试事件
        case CREATE_PROCESS_DEBUG_EVENT:
            {
                // 读取欲设置INT3断点处的机器码
                // 方便后面恢复
                ReadProcessMemory(pi.hProcess,
                                (LPVOID)BP_VA,
                                (LPVOID)&bOldByte,
                                sizeof(BYTE),
                                &dwReadWriteNum);

                // 将INT3的机器码0xCC写入断点处
                WriteProcessMemory(pi.hProcess,
                                (LPVOID)BP_VA,
                                (LPVOID)&bInt3,
                                sizeof(BYTE),
                                &dwReadWriteNum);
        	    break;
            }
            // 产生异常时的调试事件
        case EXCEPTION_DEBUG_EVENT:
            {
                // 判断异常类型
                switch ( de.u.Exception.ExceptionRecord.ExceptionCode )
                {
                    // INT3类型的异常
                case EXCEPTION_BREAKPOINT:
                    {
                        // 获取线程环境
                        context.ContextFlags = CONTEXT_FULL;
                        GetThreadContext(pi.hThread, &context);

                        // 判断是否断在我们设置的断点位置处
                        if ( (BP_VA + 1) == context.Eip )
                        {
                            // 读取正确的密码
                            ReadProcessMemory(pi.hProcess,
                                        (LPVOID)context.Edx,
                                        (LPVOID)pszPassword,
                                        MAXBYTE,
                                        &dwReadWriteNum);
                            // 读取错误密码
                            ReadProcessMemory(pi.hProcess,
                                        (LPVOID)context.Ecx,
                                        (LPVOID)pszErrorPass,
                                        MAXBYTE,
                                        &dwReadWriteNum);
                            
                            printf("你输入的密码是: %s \r\n", pszErrorPass);
                            printf("正确的密码是: %s \r\n", pszPassword);

                            // 因为我们的指令执行了INT3因此被中断
                            // INT3的机器指令长度为一个字节
                            // 因此我们需要将EIP减一来修正EIP
                            // EIP是指令指针寄存器
                            // 其中保存着下条要执行指令的地址
                            context.Eip --;

                            // 修正原来该地址的机器码
                            WriteProcessMemory(pi.hProcess, 
                                        (LPVOID)BP_VA,
                                        (LPVOID)&bOldByte,
                                        sizeof(BYTE),
                                        &dwReadWriteNum);
                            // 设置当前的线程环境
                            SetThreadContext(pi.hThread, &context);
                        }
                        break;
                    }
                }
            }
        }
        
		ContinueDebugEvent(de.dwProcessId,de.dwThreadId,DBG_CONTINUE);
    }

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

	return 0;
}