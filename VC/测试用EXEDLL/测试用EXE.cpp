// ≤‚ ‘”√EXE.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>


int main(int argc, char* argv[])
{

	printf("Hello World!\n");
	
	HMODULE hMod = LoadLibraryA("1.dll");
	PROC addr = GetProcAddress(hMod,"FuncInDll");
	(*addr)();

	printf("%d",addr);
	while (hMod)
	{
		Sleep(1000);
		printf(".");
	}
	return 0;
}

