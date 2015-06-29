// AntiVirtualM.cpp : Defines the entry point for the console application.
//

#include "AntiVM.h"

int main(int argc, char* argv[])
{
	AntiVM antiVm;
	if (antiVm.IsInsideVmVare_By_RDTSC())
	{
		MessageBoxA(0,"In Vmware Or be debugged by RDTSC", "-)", MB_ICONINFORMATION);
	}
	if (antiVm.IsInsideVMWare_By_In())
	{
		MessageBoxA(0,"In Vmware by In", "-)", MB_ICONINFORMATION);
	}
	if (antiVm.IsInsideVmVare_By_MAC())
	{
		MessageBoxA(0,"In Vmware by MAC", "-)", MB_ICONINFORMATION);
	}
	if (antiVm.IsInsideVmVare_By_Reg())
	{
		MessageBoxA(0,"In Vmware by Reg", "-) by JoyChou", MB_ICONINFORMATION);
	}
	if (antiVm.IsInsideVmVare_By_Process())
	{
		MessageBoxA(0,"In Vmware by Process", "-) by JoyChou", MB_ICONINFORMATION);
	}
	if (antiVm.IsInsideVmVare_By_STR())
	{
		MessageBoxA(0,"In Vmware by STR", "-) by JoyChou", MB_ICONINFORMATION);
	}
	// LDTʧЧ
	/*if (antiVm.IsInsideVmVare_By_LDT())
	{
		MessageBoxA(0,"In Vmware by LDT", "-) by JoyChou", MB_ICONINFORMATION);
	}*/
	if (antiVm.IsInsideVmVare_By_GDT())
	{
		MessageBoxA(0,"In Vmware by GDT", "-) by JoyChou", MB_ICONINFORMATION);
	}
	else
	{
		MessageBoxA(NULL, "Real Machine", "-)", MB_ICONINFORMATION);
	}
	
	return 0;
}

