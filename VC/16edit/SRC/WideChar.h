

/*****************************************************************************

  WideChar
  --------

  Some routines to work with UNICODE strings.

  by yoda

  WWW:      y0da.cjb.net
  E-mail:   LordPE@gmx.net

  You are allowed to use this source code in your own projects if you mention
  my name.

*****************************************************************************/

#ifndef __WideChar_h__
#define __WideChar_h__

#include <windows.h>

//
// WideChar class
//
class WideChar
{
public:
	WideChar();
	~WideChar();

	static BOOL SingleToWideCharStr(IN PSTR szStr, OUT PWSTR szWideStrBuff, IN DWORD dwcBuff);
	static BOOL WideToSingleCharStr(IN PWSTR szWideStr, OUT PSTR szStrBuff, IN DWORD dwcBuff);
};

#endif