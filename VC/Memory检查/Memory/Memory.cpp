// Memory.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string.h>

#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


	

int main(int argc, char* argv[])
{
	_CrtDumpMemoryLeaks();
	char *buff = new char[10];
	
	delete []buff;
	//	_CrtSetBreakAlloc(42);
	puts(buff);
	system("pause");
	return 0;
}

