#pragma once
#include <windows.h>
#include <Tlhelp32.h>
#include <tchar.h>

class Anti_Debug
{
public:
	Anti_Debug(void);
	~Anti_Debug(void);
	BOOL Anti_Debug_ProcessName();
	BOOL Anti_Debug_ClassName();
	BOOL Anti_Debug_ParentProcess();
	BOOL Anti_Debug_FuncInt3(char* szFuncName);
	BOOL Anti_Debug_CodeSectionInt3();
	BOOL Anti_Debug_NtInformationProcess();
	BOOL Anti_Debug_GetTickCount();
	BOOL Anti_Debug_GetStartupInfo();
	BOOL Anti_Debug_Checkremotedebuggerpresent();
};
