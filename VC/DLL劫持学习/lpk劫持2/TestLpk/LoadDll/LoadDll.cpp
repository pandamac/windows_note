// LoadDll.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#pragma comment(lib, "LpkDll.lib")
extern "C" void Test();

int main(int argc, char* argv[])
{
	Test();
	return 0;
}

