// md5.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TCMd5Tool.h"

TCMd5Tool md5;
int main(int argc, char* argv[])
{
	std::string mystring;
	md5.algorithm("123456789abcdefg",16,mystring);
	int len = mystring.length();
	unsigned char   *p12 = new unsigned char[16];
	strcpy((char *)p12,mystring.c_str());
	for(int i=0;i<16;i++)
	{
		printf("%x",p12[i]);
	}
	//9408F8DA307C543595E92DED30CF4193
	return 0;
}

