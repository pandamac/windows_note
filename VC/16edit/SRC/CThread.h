
/*****************************************************************************


  CThread
  -------

  Up to now a boring micro thread handling class.

  by yoda

  WWW:      y0da.cjb.net
  E-mail:   LordPE@gmx.net

  You are allowed to use this source code in your own projects if you mention
  my name.

*****************************************************************************/

#ifndef __CThread_h__
#define __CThread_h__

#include <windows.h>

//
// class CThread
//
class CThread
{

public:
	CThread();
	~CThread();
	BOOL        Cleanup();
	BOOL        Create(LPTHREAD_START_ROUTINE pProc, void* pArg, DWORD dwFlags);
	DWORD       GetThreadId();
	HANDLE      GetThreadHandle();

private:
	DWORD       dwThreadId;
	HANDLE      hThread;    // indicates whether a thread was created successfully

};

#endif