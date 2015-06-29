
#include "CPathString.h"

CPathString::CPathString()
{}

CPathString::~CPathString()
{}

//
// Returns a pointer to the file name of a file path
//
char* CPathString::ExtractFileName(char* szDir)
{
	char* pCH;

	pCH = (char*)((DWORD)szDir + lstrlen(szDir) - 1);
	while (*pCH != '\\')
		pCH--;
	pCH++;

	return pCH;
}

//
// Pastes a NUL at the start of the file name of the specified file path
//
void CPathString::PathToDir(char* szPath)
{
	char *pCH;

	pCH = szPath;
	pCH += lstrlen(szPath);
	while (*pCH != '\\')
		--pCH;
	*pCH = 0;

	return;
}

//
// Pastes a backslash at the end of a string if there isn't already one
//
void CPathString::ForceEndBackslash(char* szStr)
{
	char *pCH;

	pCH = (char*)((DWORD)szStr + lstrlen(szStr) - 1);
	if (*pCH != '\\')
		lstrcat(szStr, "\\");

	return;
}


