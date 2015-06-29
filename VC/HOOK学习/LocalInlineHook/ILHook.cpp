#include "ILHook.h"

CILHook::CILHook()
{
    // 对成员变量的初始化
    m_pfnOrig = NULL;
    ZeroMemory(m_bOldBytes, 5);
    ZeroMemory(m_bNewBytes, 5);
}

CILHook::~CILHook()
{
    // 取消HOOK
    UnHook();
}

/*
函数名称：Hook
函数功能：对指定模块中的函数进行挂钩
参数说明：
    pszModuleName:模块名称
    pszFuncName:  函数名称
    pfnHookFunc:  钩子函数
*/
BOOL CILHook::Hook(LPSTR pszModuleName, LPSTR pszFuncName, PROC pfnHookFunc)
{
    BOOL bRet = FALSE;
    
    // 获取指定模块中函数的地址
    m_pfnOrig = (PROC)GetProcAddress(GetModuleHandle(pszModuleName), pszFuncName);

    if ( m_pfnOrig != NULL )
    {

        // 保存该地址处5个字节的内容
        DWORD dwNum = 0;
        ReadProcessMemory(GetCurrentProcess(), m_pfnOrig, m_bOldBytes, 5, &dwNum);

        // 构造JMP指令
        m_bNewBytes[0] = '\xe9';    // jmp Opcode
        // pfnHookFunc是我们HOOK后的目标地址
        // m_pfnOrig是原来的地址
        // 5是指令长度
        *(DWORD *)(m_bNewBytes + 1) = (DWORD)pfnHookFunc - (DWORD)m_pfnOrig - 5;

        // 将构造好的地址写入该地址处
        WriteProcessMemory(GetCurrentProcess(), m_pfnOrig, m_bNewBytes, 5, &dwNum);

        bRet = TRUE;
    }
    
    return bRet;
}

/*
函数名称：UnHook
函数功能：取消函数的挂钩
*/
VOID CILHook::UnHook()
{
    if ( m_pfnOrig != 0 )
    {
        DWORD dwNum = 0;
        WriteProcessMemory(GetCurrentProcess(), m_pfnOrig, m_bOldBytes, 5, &dwNum);
    }
}

/*
函数名称：ReHook
函数功能：重新对函数进行挂钩
*/
BOOL CILHook::ReHook()
{
    BOOL bRet = FALSE;

    if ( m_pfnOrig != 0 )
    {
        DWORD dwNum = 0;
        WriteProcessMemory(GetCurrentProcess(), m_pfnOrig, m_bNewBytes, 5, &dwNum);

        bRet = TRUE;
    }

    return bRet;
}