// test3.cpp : Defines the entry point for the console application.
//
#include "stdio.h"
#include <windows.h>
void print()
{    
  #pragma comment(linker, "/entry:print")
  #pragma comment(linker, "/SECTION:.text,REW" )
  int mainCRTStartup();
  void main();
	char file[255];
  __asm
  {
    MOV   EAX,  OFFSET main
    MOV   BYTE PTR[EAX], 0xB8  //MOV  EAX, 0x
    MOV   DWORD PTR[EAX+1], OFFSET SHOWSTRING
    MOV   WORD PTR[EAX+5], 0xE0FF // JMP  EAX
  }

  mainCRTStartup();

SHOWSTRING:
  printf ("i am MentalDease\n");
  
	GetModuleFileName(NULL,file,255);
	puts(file);

  getchar();
  
}

void main()
{
}


