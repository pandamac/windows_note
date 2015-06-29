
#include "CThread.h"

CThread::CThread()
{
	hThread     = 0;
	dwThreadId  = 0;
}

CThread::~CThread()
{
	Cleanup();
}

//
// returns:
// TRUE - if there was something to cleanup
//
BOOL CThread::Cleanup()
{
	if (hThread)
	{
		CloseHandle(hThread);
		dwThreadId = 0;
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CThread::Create(LPTHREAD_START_ROUTINE pProc, void* pArg, DWORD dwFlags)
{
	HANDLE hTh;

	Cleanup();
	hTh = CreateThread(NULL, 0, pProc, pArg, dwFlags, &dwThreadId);

	return (hTh != 0 ? TRUE : FALSE);
}

DWORD CThread::GetThreadId()
{
	if (hThread)
		return dwThreadId;
	else
		return 0;
}

HANDLE CThread::GetThreadHandle()
{
	return hThread;
}