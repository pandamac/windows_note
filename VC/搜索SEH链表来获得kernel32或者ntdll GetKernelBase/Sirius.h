//Sirius.h 
// Version 1.00
// Sirius
// April 11th,2012
// elovespace@163.com
#ifndef _SIRIUS_H__
#define _SIRIUS_H__
#ifdef NDEBUG
#pragma optimize("gsy",on)
#pragma comment(linker,"/RELEASE")
#pragma data_seg(".Sirius")
#pragma data_seg()
#pragma comment(linker,"/Entry:main")
#pragma comment(linker,"/MERGE:.rdata=.Sirius")
#pragma comment(linker,"/MERGE:.text=.Sirius")
#pragma comment(linker,"/MERGE:.reloc=.Sirius")
#pragma comment(linker,"/MERGE:.data=.Sirius")
#pragma comment(linker,"/Section:.Sirius,RWE")
#pragma comment(lib,"msvcrt.lib")
#pragma comment(linker,"/Version:1.00")
#if(_MSG_VER < 1300) 
#pragma comment(linker,"/IGNORE:4078")
#pragma comment(linker,"/OPT:NOWIN98")
#endif
#endif
#endif
#define WIN32_LEAN_AND_MEAN