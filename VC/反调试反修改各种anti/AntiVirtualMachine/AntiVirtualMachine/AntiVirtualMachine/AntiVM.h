#pragma once
#include <windows.h>
#include <tchar.h>

class AntiVM
{
public:

	AntiVM(void);
	~AntiVM(void);
	BOOL IsInsideVMWare_By_In();
	BOOL IsInsideVmVare_By_RDTSC();
	BOOL IsInsideVmVare_By_MAC();
	BOOL IsInsideVmVare_By_Reg();
	BOOL IsInsideVmVare_By_Process();
	BOOL IsInsideVmVare_By_STR();
	BOOL IsInsideVmVare_By_LDT();
	BOOL IsInsideVmVare_By_GDT();
};
