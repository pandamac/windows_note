#include "stdio.h"

void print()
{
	#pragma comment(linker,"/entry:print")
	#pragma comment(lib,"msvcrt.lib")
	puts("hello world");
}
void main()
{

}