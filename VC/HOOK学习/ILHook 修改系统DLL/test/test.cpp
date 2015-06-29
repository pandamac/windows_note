// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ILHook.h"
	CILHook item;

typedef int (WINAPI * MyMessageBox)(
   HWND hWnd,          // handle to owner window
   LPCTSTR lpText,     // text in message box
   LPCTSTR lpCaption,  // message box title
   UINT uType          // message box style
   );
PROC proc = (PROC)MessageBoxA;

int WINAPI MyMessageBox2(
						 HWND hWnd,          // handle to owner window
						 LPCTSTR lpText,     // text in message box
						 LPCTSTR lpCaption,  // message box title
						 UINT uType          // message box style
						 )
{
	item.UnHook();
	return ((MyMessageBox)proc)(hWnd,"被我hook了","哈哈",uType);
}

int main(int argc, char* argv[])
{

	item.Hook("user32.dll","MessageBoxA",(PROC)MyMessageBox2);
	
	MessageBox(NULL,"没有被HOOK","`````",MB_OK);
	return 0;
}
