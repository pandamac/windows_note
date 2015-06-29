#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#pragma comment(lib,"imm32.lib")

#pragma data_seg("mysechx")
HHOOK g_hKeyboard=NULL;
HWND g_hMainhWnd=NULL;
char g_IMEString[256]="";
bool g_HOOKflag=false;
#pragma data_seg()

class clsHookInfo
{
private:
	BYTE mbytOldCode[5];
	BYTE mbytNewCode[5];
	int mlngFunAddr;
	bool hookBeOK;
public:
	bool HookApi(LPCTSTR strDllName,LPCTSTR strFunName,int lngFunAddr)
	{
		hookBeOK=false;
		HMODULE hModule=NULL;
		int dwJmpAddr=0;
		hModule=GetModuleHandle(strDllName);
		if (hModule==NULL) {return false;}
		mlngFunAddr=(int)GetProcAddress(hModule,strFunName);
		if (mlngFunAddr==NULL) {return false;}
		CopyMemory(mbytOldCode,(const void *)mlngFunAddr,5);
		mbytNewCode[0]=0xE9;
		dwJmpAddr=lngFunAddr-mlngFunAddr-5;
		CopyMemory(&mbytNewCode[1],&dwJmpAddr,4);
		hookBeOK=true;
		HookStatus(true);
		return true;
	}
	bool HookStatus(bool blnIsHook)
	{
		DWORD oldACC,newACC;
		if (!hookBeOK) {return false;}
		if (blnIsHook)
		{
			VirtualProtect((LPVOID)mlngFunAddr,5,PAGE_WRITECOPY,&oldACC);
			CopyMemory((void *)mlngFunAddr,mbytNewCode,5);
			VirtualProtect((LPVOID)mlngFunAddr,5,oldACC,&newACC);
		}
		else
		{
			VirtualProtect((LPVOID)mlngFunAddr,5,PAGE_WRITECOPY,&oldACC);
			CopyMemory((void *)mlngFunAddr,mbytOldCode,5);
			VirtualProtect((LPVOID)mlngFunAddr,5,oldACC,&newACC);
		}
		return true;
	}
};

clsHookInfo g_myHOOK1;
clsHookInfo g_myHOOK2;
char g_StringBuf1[256]="";
char g_StringBuf2[256]="";
UINT WM_HXWDLLWX_QQBTX=NULL;
UINT WM_HXWDLLWX_HOOKKEY=NULL;

BOOL MByteToWChar(LPCSTR lpcszStr,LPWSTR lpwszStr,DWORD dwSize)
{
    DWORD dwMinSize;
    dwMinSize = MultiByteToWideChar(CP_ACP, 0, lpcszStr, -1, NULL, 0);
    if(dwSize < dwMinSize)
    {
     return FALSE;
    }
    MultiByteToWideChar (CP_ACP, 0, lpcszStr, -1, lpwszStr, dwMinSize); 
    return TRUE;
}

BOOL WCharToMByte(LPCWSTR lpcwszStr,LPSTR lpszStr,DWORD dwSize)
{
   DWORD dwMinSize;
   dwMinSize = WideCharToMultiByte(CP_OEMCP,NULL,lpcwszStr,-1,NULL,0,NULL,FALSE);
   if(dwSize < dwMinSize)
   {
    return FALSE;
   }
   WideCharToMultiByte(CP_OEMCP,NULL,lpcwszStr,-1,lpszStr,dwSize,NULL,FALSE);
   return TRUE;
}


LONG WINAPI myImmGetCompositionStringA(HIMC hIMC, DWORD dwIndex, LPVOID lpBuf, DWORD dwBufLen)
{
	LONG myReturn=0;
	LONG tempLen=0;
	int StrBufLen=0;
	if (dwIndex==GCS_RESULTSTR && g_HOOKflag)
	{
		memset(g_IMEString,0,256);
		tempLen=255;
		g_myHOOK1.HookStatus(false);
		ImmGetCompositionStringA(hIMC,GCS_RESULTSTR,g_IMEString,tempLen);
		g_myHOOK1.HookStatus(true);
		g_IMEString[tempLen]=0;
		if (lstrcmp(g_IMEString,g_StringBuf1)!=0)
		{
			memset(g_StringBuf1,0,256);
			lstrcpy(g_StringBuf1,g_IMEString);
			SendMessage(g_hMainhWnd,WM_HXWDLLWX_QQBTX,0,0);
			memset(g_StringBuf2,0,256);
			lstrcpy(g_StringBuf2,g_IMEString);
		}
		if (lpBuf==NULL || dwBufLen==0)
		{
			myReturn=lstrlen(g_StringBuf2);
		}
		else
		{
			StrBufLen=lstrlen(g_StringBuf2);
			memset(lpBuf,0,dwBufLen);
			if (dwBufLen>=StrBufLen) 
			{
				dwBufLen=StrBufLen;
				myReturn=StrBufLen;
			}
			else
			{
				myReturn=0;
			}
			memcpy(lpBuf,g_StringBuf2,dwBufLen);
		}
	}
	else
	{
		g_myHOOK1.HookStatus(false);
		myReturn=ImmGetCompositionStringA(hIMC,dwIndex,lpBuf,dwBufLen);
		g_myHOOK1.HookStatus(true);
	}
	return myReturn;
}

LONG WINAPI myImmGetCompositionStringW(HIMC hIMC, DWORD dwIndex, LPVOID lpBuf, DWORD dwBufLen)
{
	LONG myReturn=0;
	LONG tempLen=0;
	int StrBufLen=0;
	if (dwIndex==GCS_RESULTSTR && g_HOOKflag)
	{
		memset(g_IMEString,0,256);
		tempLen=254;
		g_myHOOK2.HookStatus(false);
		ImmGetCompositionStringW(hIMC,GCS_RESULTSTR,g_IMEString,tempLen);
		g_myHOOK2.HookStatus(true);
		g_IMEString[254]=0;
		g_IMEString[255]=0;
		if (lstrcmpW((wchar_t *)g_IMEString,(wchar_t *)g_StringBuf1)!=0)
		{
			memset(g_StringBuf1,0,256);
			lstrcpyW((wchar_t *)g_StringBuf1,(wchar_t *)g_IMEString);
			WCharToMByte((wchar_t *)g_StringBuf1,g_IMEString,256);
			SendMessage(g_hMainhWnd,WM_HXWDLLWX_QQBTX,0,0);
			memset(g_StringBuf2,0,256);
			MByteToWChar(g_IMEString,(wchar_t *)g_StringBuf2,128);
		}
		if (lpBuf==NULL || dwBufLen==0)
		{
			myReturn=lstrlenW((wchar_t *)g_StringBuf2)*2;
		}
		else
		{
			StrBufLen=lstrlenW((wchar_t *)g_StringBuf2)*2;
			memset(lpBuf,0,dwBufLen);
			if (dwBufLen>=StrBufLen) 
			{
				dwBufLen=StrBufLen;
				myReturn=StrBufLen;
			}
			else
			{
				myReturn=0;
			}
			memcpy(lpBuf,g_StringBuf2,dwBufLen);
		}
	}
	else
	{
		g_myHOOK2.HookStatus(false);
		myReturn=ImmGetCompositionStringW(hIMC,dwIndex,lpBuf,dwBufLen);
		g_myHOOK2.HookStatus(true);
	}
	return myReturn;
}


BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
{
   switch(fdwReason)
    {
      case DLL_PROCESS_ATTACH:
		  WM_HXWDLLWX_QQBTX=RegisterWindowMessage("WM_HXWDLLWX_QQBTX");
		  WM_HXWDLLWX_HOOKKEY=RegisterWindowMessage("WM_HXWDLLWX_HOOKKEY");
		  g_myHOOK1.HookApi("imm32.dll","ImmGetCompositionStringA",(int)myImmGetCompositionStringA);
		  g_myHOOK2.HookApi("imm32.dll","ImmGetCompositionStringW",(int)myImmGetCompositionStringW);
        break;
	  case DLL_THREAD_ATTACH:
		 break;
	  case DLL_THREAD_DETACH:
		 break;
      case DLL_PROCESS_DETACH:
		  g_myHOOK1.HookStatus(false);
		  g_myHOOK2.HookStatus(false);
        break;
      default:
        break;
    }
	return true;
}


LRESULT CALLBACK KeyboardProc(int code,WPARAM wParam,LPARAM lParam)
{
	if (code==0 && g_HOOKflag)
	{
		SendMessage(g_hMainhWnd,WM_HXWDLLWX_HOOKKEY,wParam,lParam);
	}
	return CallNextHookEx(g_hKeyboard,code,wParam,lParam);
}

void setHOOK()
{
	g_hKeyboard=SetWindowsHookEx(WH_KEYBOARD,KeyboardProc,GetModuleHandle("hxwdllwx.dll"),0);
}

int WINAPI DLLstartHOOK(HWND myhWnd)
{
	if (g_hMainhWnd==NULL)
	{
		g_hMainhWnd=myhWnd;
	}
	return 1;
}

int WINAPI DLLstopHOOK()
{
	if (g_hKeyboard!=NULL)
	{
		UnhookWindowsHookEx(g_hKeyboard);
		g_hKeyboard=NULL;
	}
	return 1;
}

int WINAPI DLLsetHOOKState(bool myState)
{
	if (myState)
	{
		if (g_hKeyboard==NULL)
		{
			setHOOK();
		}
		g_HOOKflag=true;
	}
	else
	{
		g_HOOKflag=false;
	}
	return 1;
}

LPCTSTR WINAPI DLLGetPubString()
{
	char tmpStr[258]="";
	lstrcpy(tmpStr,g_IMEString);
	tmpStr[256]=0;
	tmpStr[257]=0;
	return (LPCTSTR)SysAllocString((const OLECHAR *)tmpStr);
}

int WINAPI DLLSetPubString(LPCTSTR tmpStr)
{
	memset(g_IMEString,0,256);
	if (lstrlen(tmpStr)>255)
	{
		lstrcpyn(g_IMEString,tmpStr,256);
	}
	else
	{
		lstrcpy(g_IMEString,tmpStr);
	}
	return 1;
}

