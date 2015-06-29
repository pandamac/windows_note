
/*****************************************************************************


  CPathString
  -----------

  Some routines to work with path strings.

  by yoda

  WWW:      y0da.cjb.net
  E-mail:   LordPE@gmx.net

  You are allowed to use this source code in your own projects if you mention
  my name.

*****************************************************************************/

#ifndef __CPathString_h__
#define __CPathString_h__

#include <windows.h>

//
// CPathString
//
class CPathString
{
public:
	CPathString();
	~CPathString();

	static char* ExtractFileName(char* szDir);
	static void  PathToDir(char* szPath);
	static void  ForceEndBackslash(char* szStr);
};

#endif