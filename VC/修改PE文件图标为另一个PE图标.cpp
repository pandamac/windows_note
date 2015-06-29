#include "stdafx.h"
#include <Windows.h>


BOOL CALLBACK EnumResIconProc(
	__in  HMODULE hModule,
	__in  LPCTSTR lpszType,
	__in  LPTSTR lpszName,//automatic
	__in  LONG_PTR lParam //virus file
	)
{
	HRSRC hRsrc = FindResource(hModule,lpszName,RT_ICON);
	HGLOBAL  hGlobal = LoadResource(hModule,hRsrc);
	LPVOID lpData = 	LockResource(hGlobal );
	DWORD cbData = SizeofResource(hModule,hRsrc);

	if (lpData == NULL)
	{
		throw("Could not lock Icon.");
	}
	//if (lpszType == 3)
	//{
		HANDLE handle = BeginUpdateResource((LPCWSTR)lParam,0);
		if (handle == NULL)
		{
			throw("Could not BeginUpdateResource");
		}
		BOOL flag = UpdateResource(handle,lpszType,lpszName,MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),lpData,cbData);

		if (flag == NULL) throw("Could not UpdateResource");

		if (!EndUpdateResource(handle,0)) throw("Could not UpdateResource");
	
	//}
	return 0;
}
void ChangeIcon(wchar_t * strSourcePath,wchar_t * strDesPath)
{

	HMODULE  handle = LoadLibrary(strSourcePath);

	EnumResourceNames(handle, RT_ICON, (ENUMRESNAMEPROC)EnumResIconProc, (LONG_PTR)strDesPath);

	FreeLibrary(handle);  
}

int _tmain(int argc, _TCHAR* argv[])
{
	wchar_t strSourcePath[MAX_PATH];
	wchar_t strDesPath[MAX_PATH];

	GetCurrentDirectory(MAX_PATH,strSourcePath);
	GetCurrentDirectory(MAX_PATH,strDesPath);

	wcscat(strSourcePath,TEXT("\\2.exe"));//2->1 icon ¸´ÖÆ
	wcscat(strDesPath,TEXT("\\1.exe"));

	ChangeIcon(strSourcePath,strDesPath);
	return 0;
}