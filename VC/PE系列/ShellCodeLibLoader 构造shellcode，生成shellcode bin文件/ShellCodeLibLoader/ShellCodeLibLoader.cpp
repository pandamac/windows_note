/*
	ShellCodeTemplate v0.0.1: MessageBox demo
	Source code put in public domain by Didier Stevens, no Copyright
	Except for the code in ShellCodeInit, which is released under the GNU PUBLIC LICENSE v2.0
	http://didierstevens.com
	Use at your own risk

	Shortcommings, or todo's ;-)
		- find fix for function allignment

	History:
		2008/10/24: start
		2010/02/02: cleanup
*/

#include <windows.h>
#include <stdio.h>

#define KERNEL32_HASH 0x000d4e88
#define KERNEL32_LOADLIBRARYA_HASH 0x000d5786
#define KERNEL32_GETPROCADDRESSA_HASH 0x00348bfa

typedef HMODULE (WINAPI *TD_LoadLibraryA)(LPCTSTR lpFileName);
typedef FARPROC (WINAPI *TD_GetProcAddressA)(HMODULE hModule, LPCTSTR lpProcName);

// Add your API function pointer definitions here:
typedef int (WINAPI *TD_MessageBoxA)(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);

struct SHELL_CODE_CONTEXT
{
	TD_LoadLibraryA FP_LoadLibraryA;
	TD_GetProcAddressA FP_GetProcAddressA;

	char szEmptyString[1];

	// Add your module handles and API function pointer members here:
	HMODULE hmUSER32;
	TD_MessageBoxA FP_MessageBoxA;
};

void ShellCodeMain(void);
int WriteShellCode(LPCTSTR, PBYTE, size_t);
void *ShellCodeData(void);

void __declspec(naked) ShellCodeStart(void)
{
	__asm 
	{
		call ShellCodeMain
		ret
	}
}

#pragma warning(push)
#pragma warning(disable:4731)

void ShellCodeInit(TD_LoadLibraryA *pFP_LoadLibraryA, TD_GetProcAddressA *pFP_GetProcAddressA)
{
	TD_LoadLibraryA FP_LoadLibraryA;
	TD_GetProcAddressA FP_GetProcAddressA;

	// Shellcode functions to lookup API functions, based on 
	// The Shellcoder's Handbook http://eu.wiley.com/WileyCDA/WileyTitle/productCd-0764544683.html'
	// Released under the GNU PUBLIC LICENSE v2.0

	__asm
	{
		push KERNEL32_LOADLIBRARYA_HASH
		push KERNEL32_HASH
		call getfuncaddress
		mov FP_LoadLibraryA, eax

		push KERNEL32_GETPROCADDRESSA_HASH
		push KERNEL32_HASH
		call getfuncaddress
		mov FP_GetProcAddressA, eax

		jmp totheend

	getfuncaddress:
		push ebp
		mov ebp, esp
		push ebx
		push esi
		push edi
		push ecx
		push fs:[0x30]
		pop eax
		mov eax, [eax+0x0c]
		mov ecx, [eax+0x0c]
	nextinlist:
		mov edx, [ecx]
		mov eax, [ecx+0x30]
		push 0x02
		mov edi, [ebp+0x08]
		push edi
		push eax
		call hashit
		test eax, eax
		jz foundmodule
		mov ecx, edx
		jmp nextinlist
	foundmodule:
		mov eax, [ecx+0x18]
		push eax
		mov ebx, [eax+0x3c]
		add eax, ebx
		mov ebx, [eax+0x78]
		pop eax
		push eax
		add ebx, eax
		mov ecx, [ebx+28]
		mov edx, [ebx+32]
		mov ebx, [ebx+36]
		add ecx, eax
		add edx, eax
		add ebx, eax
	find_procedure:
		mov esi, [edx]
		pop eax
		push eax
		add esi, eax
		push 1
		push [ebp+12]
		push esi
		call hashit
		test eax, eax
		jz found_procedure
		add edx, 4
		add ebx, 2
		jmp find_procedure
	found_procedure:
		pop eax
		xor edx, edx
		mov dx, [ebx]
		shl edx, 2
		add ecx, edx
		add eax, [ecx]
		pop ecx
		pop edi
		pop esi
		pop ebx
		mov esp, ebp
		pop ebp
		ret 0x08

	hashit:
		push ebp
		mov ebp, esp
		push ecx
		push ebx
		push edx
		xor ecx,ecx
		xor ebx,ebx
		xor edx,edx
		mov eax, [ebp+0x08]
	hashloop:
		mov dl, [eax]
		or dl, 0x60
		add ebx, edx
		shl ebx, 0x01
		add eax, [ebp+16]
		mov cl, [eax]
		test cl, cl
		loopnz hashloop
		xor eax, eax
		mov ecx, [ebp+12]
		cmp ebx, ecx
		jz donehash
		inc eax
	donehash:
		pop edx
		pop ebx
		pop ecx
		mov esp, ebp
		pop ebp
		ret 12

	totheend:
	}

	*pFP_LoadLibraryA = FP_LoadLibraryA;
	*pFP_GetProcAddressA = FP_GetProcAddressA;
}

#pragma warning(pop)

// Write your custom code in this function.
// Add extra functions as needed.
void ShellCodePayload(SHELL_CODE_CONTEXT *pSCC)
{
	char szHello[] = {'H', 'e', 'l', 'l', 'o', '\0'};
	pSCC->FP_MessageBoxA(NULL, szHello, pSCC->szEmptyString, 0);
}

void ShellCodeMain(void)
{
	SHELL_CODE_CONTEXT scc;

	ShellCodeInit(&(scc.FP_LoadLibraryA), &(scc.FP_GetProcAddressA));

	scc.szEmptyString[0] = '\0';

	// Add your own API function initialization code here:
	char szuser32[] = {'u', 's', 'e', 'r', '3', '2', '\0'};
	char szMessageBoxA[] = {'M', 'e', 's', 's', 'a', 'g', 'e', 'B', 'o', 'x', 'A', '\0'};
	scc.hmUSER32 = scc.FP_LoadLibraryA(szuser32);
	scc.FP_MessageBoxA = (TD_MessageBoxA)scc.FP_GetProcAddressA(scc.hmUSER32, szMessageBoxA);
	
	ShellCodePayload(&scc);
}

int main(int argc, char **argv)
{
	size_t dwSize;
	char szBinFile[MAX_PATH];

	dwSize = (PBYTE)main - (PBYTE)ShellCodeStart;
	printf("Shellcode start = %p\n", ShellCodeStart);
	printf("Shellcode size = %08x\n", dwSize);
	sprintf_s(szBinFile, MAX_PATH, "%s.bin", argv[0]);
	printf("Shellcode file = %s\n", szBinFile);
	if (0 == WriteShellCode(szBinFile, (PBYTE)ShellCodeStart, dwSize))
		printf("Shellcode file creation successful\n");
	else
		printf("Shellcode file creation failed\n");

	// Calling ShellCodeMain to debug shellcode inside Visual Studio
	// Remove this call if you don't want to execute your shellcode inside Visual Studio
	ShellCodeMain();

	return 0;
}

// Function to extract and write the shellcode to a file
int WriteShellCode(LPCTSTR szFileName, PBYTE pbShellCode, size_t sShellCodeSize)
{
	FILE *pfBin;
	size_t sWritten;

	if (S_OK != fopen_s(&pfBin, szFileName, "wb"))
		return -1;
	sWritten = fwrite(pbShellCode, sShellCodeSize, 1, pfBin);
	fclose(pfBin);
	if (sWritten != 1)
		return -2;
	return 0;
}
