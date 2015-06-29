
/*****************************************************************************

  ArgX.h
  ------

  Makes the "__getmainargs" routine and by this way the well-known "argc" and
  "argv" variables accessible without using the "main" EntryPoint in VC.

  Note:
  Don't forget to include msvcrt.lib (release) or msvcrtd.lib (debug) if it
  isn't already present in the library list.

  by yoda

  WWW:      y0da.cjb.net
  E-mail:   LordPE@gmx.net

  You are allowed to use this class in your own projects if you keep this
  trademark.

*****************************************************************************/

#ifndef __ArgX_h__
#define __ArgX_h__

#include <windows.h>

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

void getmainargs(int *pargc, char ***pargv, char ***envp);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __ArgX_h__