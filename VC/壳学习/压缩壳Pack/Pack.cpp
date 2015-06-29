// Pack.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MyPack.h"

int main(int argc, char* argv[])
{ 

		CPack pack;

		if (pack.LoadPE("C:\\1\\2.exe") == SUCCESS)
		{
			if (pack.GetFile("C:\\1\\2.exe") == SUCCESS)
			{
				MessageBoxA(NULL, "¼Ó¿Ç³É¹¦", "¹§Ï²", MB_OK);
			}
		}
	return 0;
}

