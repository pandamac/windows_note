// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>


int main(int argc, char* argv[])
{
	HMODULE handle = LoadLibrary("HOOK.dll");
	
	while (1)
	{
		MessageBox(NULL,"正常","正常",MB_OK);
	}
	return 0;
}

