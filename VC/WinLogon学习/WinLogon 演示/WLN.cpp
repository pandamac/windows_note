// WLN.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "WLN.h"
#include <stdio.h>

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

int WLN_API fnStartSS(PWLX_NOTIFICATION_INFO pI)
{
	FILE *fp = fopen("c:\\tempwln.txt","a");
	fprintf(fp,"启动屏保%ld\n",GetTickCount());
	fclose(fp);
	return 1;
}
int WLN_API fnStopSS(PWLX_NOTIFICATION_INFO pI)
{
	FILE *fp = fopen("c:\\tempwln.txt","a");
	fprintf(fp,"结束屏保 %ld\n",GetTickCount());
	fclose(fp);
	return 1;
}

