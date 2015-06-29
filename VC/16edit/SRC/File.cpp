
#include <malloc.h>
#include "File.h"

FILE::FILE()
{
	Destroy();
}

FILE::~FILE()
{
	// Cleanup
	Destroy();
}

//
// Open a file in the specified mode
//
BOOL FILE::GetFileHandle(char *szFilePath, DWORD dwMode)
{
	DWORD  dwFlags, dwAccess, dwShare;

	// destroy if sth is already loaded
	Destroy();

	// save path
	lstrcpy(cFilePath, szFilePath);

	// handle flags
	switch(dwMode)
	{
	case F_OPENEXISTING_R:
		bReadOnly = TRUE;
		dwFlags = OPEN_EXISTING;
		dwAccess = GENERIC_READ;
		dwShare = FILE_SHARE_READ;
		break;

	case F_OPENEXISTING_RW:
		bReadOnly = FALSE;
		dwFlags = OPEN_EXISTING;
		dwAccess = GENERIC_READ | GENERIC_WRITE;
		dwShare = FILE_SHARE_READ | FILE_SHARE_WRITE;
		break;

	case F_CREATENEW:
		bReadOnly = FALSE;
		dwFlags = CREATE_ALWAYS;
		dwAccess = GENERIC_READ | GENERIC_WRITE;
		dwShare = FILE_SHARE_READ | FILE_SHARE_WRITE;
		break;

	case F_TRUNCATE: // RW
		bReadOnly = FALSE;
		dwFlags = TRUNCATE_EXISTING;
		dwAccess = GENERIC_READ | GENERIC_WRITE;
		dwShare = FILE_SHARE_READ | FILE_SHARE_WRITE;
		break;
	}

	hFile = CreateFile(
		szFilePath,
		dwAccess,
		dwShare,
		NULL,
		dwFlags,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile == (HANDLE)-1)
		return FALSE;

	return TRUE;
}

//
// opens a file with RW access if possible else just R
BOOL FILE::GetFileHandleWithMaxAccess(char* szFilePath)
{
	BOOL bRet;

	bRet = GetFileHandle(szFilePath, F_OPENEXISTING_RW);
	if (!bRet)
		bRet = GetFileHandle(szFilePath, F_OPENEXISTING_R);

	return bRet;
}

//
// Cleanup routine
//
BOOL FILE::Destroy()
{
	BOOL bRet = FALSE;

	// cleanup
	if (hFile != INVALID_HANDLE_VALUE)
		if (CloseHandle(hFile))
			bRet = TRUE;
	if (pMap)
		free(pMap);

	// adjust variables
	hFile        = INVALID_HANDLE_VALUE;
	pMap         = NULL;
	dwMapSize    = 0;

	return bRet;
}

//
// returns:
// INVALID_HANDLE_VALUE - if no file is loaded
//
HANDLE FILE::GetHandle()
{
	return hFile;
}

BOOL FILE::IsFileReadOnly()
{
	return bReadOnly;
}

BOOL FILE::MapFile()
{
	DWORD dw, dwFSize;

	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	dwFSize = GetFSize();

	// map file
	pMap = malloc( dwFSize );
	if (!pMap)
		return FALSE;
	if (!ReadFile(hFile, pMap, dwFSize, &dw, NULL))
		return FALSE;

	// set vars
	dwMapSize = dwFSize;

	return TRUE;
}

void* FILE::GetMapPtr()
{
	return pMap;
}

BOOL FILE::UnmapFile()
{
	if (!pMap)
		return FALSE;

	free(pMap);
	pMap       = NULL;
	dwMapSize  = 0;

	return TRUE;
}

//
// change size of file memory
//
BOOL FILE::ReMapFile(DWORD dwNewSize)
{
	if (!pMap)
		return FALSE; // ERR

	pMap = realloc(pMap, dwNewSize);
	if (pMap)
	{
		pMap      = NULL;
		dwMapSize = dwNewSize;
		return TRUE; // OK
	}
	else
		return FALSE; // ERR
}

//
// returns:
// -1 in the case of an error
//
DWORD FILE::GetMapSize()
{
	if (!pMap)
		return (DWORD)-1; // ERR

	return dwMapSize;
}

BOOL FILE::IsMapped()
{
	return pMap != NULL ? TRUE : FALSE;
}

//
// copy mapping memory to file
//
BOOL FILE::FlushFileMap()
{
	if ( !Truncate() )
		return FALSE; // ERR

	return Write(pMap, dwMapSize);
}

BOOL FILE::FileExits(char* szFilePath)
{
	FILE *f = new FILE;

	return f->GetFileHandle(szFilePath, F_OPENEXISTING_R);
}

DWORD FILE::GetFSize()
{
	return GetFileSize(hFile, NULL);
}
	
//
// returns:
// NULL - if no file is loaded
//	
char* FILE::GetFilePath()
{
	if (hFile == (HANDLE)-1)
		return NULL;

	return cFilePath;
}

//
// write to file
//
BOOL FILE::Write(void* pBuff, DWORD dwc)
{
	DWORD  dwRet;
	BOOL   bRet;

	bRet = WriteFile(hFile, pBuff, dwc, &dwRet, NULL);
	
	return (bRet && dwc == dwRet) ? TRUE : FALSE;
}

//
// read from file
//
BOOL FILE::Read(void* pBuff, DWORD dwc)
{
	DWORD  dwRet;
	BOOL   bRet;

	bRet = ReadFile(hFile, pBuff, dwc, &dwRet, NULL);
	
	return (bRet && dwc == dwRet) ? TRUE : FALSE;
}

//
// set file pointer
//
BOOL FILE::SetFPointer(DWORD dwOff)
{
	DWORD dwRet;

	dwRet = SetFilePointer(hFile, dwOff, NULL, FILE_BEGIN);

	return (dwRet != (DWORD)-1) ? TRUE : FALSE;
}

BOOL FILE::Truncate()
{
	if (!SetFPointer(0))
		return FALSE; // ERR
	if (!SetEndOfFile(hFile))
		return FALSE; // ERR

	return TRUE; // OK
}

void FILE::SetMapPtrSize(void* ptr, DWORD dwSize)
{
	pMap      = ptr;
	dwMapSize = dwSize;

	return;
}