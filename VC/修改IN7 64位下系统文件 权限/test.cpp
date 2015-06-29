
#include <Windows.h>
#include <atlstr.h>
#include <Aclapi.h>

//基本上全部来自对系统系统文件 takeown.exe 的逆向分析。
//函数声明
BOOL Is64();
BOOL GetTakeOwnershipPrivilege();
BOOL TakeOwnerShip(
	IN PCTSTR ptcsFilePath
	);
BOOL GetFilePrivilege (
	IN PCTSTR ptcsFilePath
	);
int _tmain(int argc, _TCHAR* argv[])
{
	CString strClacPath32 = TEXT("\\system32\\calc.exe");
	CString strClacPath64 = TEXT("\\Sysnative\\calc.exe");
	CString strClacPath;
	CString strTeamName = TEXT("panda");
	BOOL bRst;
	LPTSTR lpWinDir;
	printf_s("Start...\n");
	//获取系统目录
	lpWinDir = (LPTSTR)VirtualAlloc(NULL, 100, MEM_COMMIT, PAGE_READWRITE);
	if (lpWinDir == NULL)
	{
		printf_s("memory allocate failed\n");
		return -1;
	}
	GetWindowsDirectory(lpWinDir, 100);
	strClacPath = lpWinDir;
	//判断系统类型，确定calc.exe 路径
	if(Is64())
	{
		printf_s("64bit system...\n");
		strClacPath += strClacPath64;
	}
	else
	{
		printf_s("32bit system...\n");
		strClacPath += strClacPath32;
	}
	//获取为SeTakeOwnershipPrivilege 权限。提升本进程的权限
	bRst = GetTakeOwnershipPrivilege();
	if (!bRst)
	{
		printf_s("GetTakeOwnershipPrivilege Failed\n");
		return -1;
	}
	//获得文件所有权
	bRst = TakeOwnerShip(strClacPath.GetBuffer());
	if (!bRst)
	{
		printf_s("TakeOwnerShip Failed\n");
		return -1;
	}
	bRst = GetFilePrivilege(strClacPath.GetBuffer());//取得文件删除权限：
	if (!bRst)
	{
		printf_s("GetFilePrivilege Failed\n");
		return -1;
	}
	//修改文件
	HANDLE hFile = CreateFile(strClacPath.GetBuffer(), GENERIC_READ | GENERIC_WRITE, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE != hFile)
	{
		SetFilePointer(hFile, 0, NULL, FILE_END);
		DWORD dwWritten;
		bRst = WriteFile(hFile, strTeamName.GetBuffer(), strTeamName.GetLength()*2, &dwWritten, NULL);
		if (!bRst)
		{
			printf_s("WriteFile Failed\n");
			return -1;
		}
	}
	//Cleanup
	if (hFile != NULL)
		CloseHandle(hFile);
	if (lpWinDir != NULL)
		VirtualFree(lpWinDir, 100, MEM_DECOMMIT);
	printf_s("Finished! Press Enter to quit\n");
	getchar();
	return 0;
}
BOOL GetTakeOwnershipPrivilege()
	/*++
	Routine Description:
	为本进程赋予SeTakeOwnershipPrivilege 权限。
	Return Value:
	成功返回TRUE，失败返回FALSE
	--*/
{
	HANDLE hProc = NULL;
	HANDLE hToken = NULL;
	BOOL bRet = FALSE;
	do {
		hProc = OpenProcess(PROCESS_QUERY_INFORMATION,
			FALSE,
			GetCurrentProcessId()
			);
		if (!hProc)
			break;
		HANDLE hToken = NULL;
		bRet = OpenProcessToken(hProc,
			TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
			&hToken
			);
		if (!bRet)
			break;
		LUID Luid;
		bRet = LookupPrivilegeValue(NULL,
			TEXT("SeTakeOwnershipPrivilege"),//进程是SE_DEBUG_NAME
			&Luid
			);
		if (!bRet)
			break;
		TOKEN_PRIVILEGES NewTP;
		NewTP.Privileges[0].Luid.LowPart = Luid.LowPart;
		NewTP.Privileges[0].Luid.HighPart = Luid.HighPart;
		NewTP.PrivilegeCount = 1;
		NewTP.Privileges[0].Attributes = 2;//SE_PRIVILEGE_ENABLED
		bRet = AdjustTokenPrivileges(hToken, FALSE, &NewTP, 0, 0, NULL);
		if (!bRet) {
			break;
		}
		bRet = TRUE;
	} while (FALSE);
	if (hProc)
		CloseHandle(hProc);
	if (hToken)
		CloseHandle(hToken);
	return bRet;
}
BOOL TakeOwnerShip(
	IN PCTSTR ptcsFilePath
	)
	/*++
	Routine Description:
	取得某文件的所有者权限。
	Arguments:
	ptcsFilePath - 输入参数，要取得所有者权限的文件的完整路径.
	Return Value:
	成功返回TRUE，失败返回FALSE
	--*/
{
	SID_IDENTIFIER_AUTHORITY sia = {0, 0, 0, 0, 0, 5};
	PSID pSid = NULL;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	PACL pAcl = NULL;
	PSECURITY_DESCRIPTOR pSD = NULL;
	SECURITY_DESCRIPTOR sd;
	BOOL bRet = FALSE;
	do {
		if ( !ptcsFilePath )
			break;
		if (!AllocateAndInitializeSid(&sia, 0x2, 0x20, 0x220,
			0, 0, 0, 0, 0, 0, &pSid))
			break;
		if (!InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION))
			break;
		hFile = CreateFile( ptcsFilePath,
			FILE_SUPPORTS_ENCRYPTION,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS,
			NULL
			);
		if (hFile != INVALID_HANDLE_VALUE) {
			DWORD dwStatus = GetSecurityInfo(hFile,
				SE_FILE_OBJECT,
				DACL_SECURITY_INFORMATION,
				0,
				0,
				&pAcl,
				0,
				&pSD
				);
			if (dwStatus != ERROR_SUCCESS)
				break;
			if (!SetSecurityDescriptorDacl(&sd, TRUE, pAcl, FALSE))
				break;
		}
		if (!SetSecurityDescriptorOwner(&sd, pSid, FALSE))
			break;
		if (!SetFileSecurity(ptcsFilePath, OWNER_SECURITY_INFORMATION, &sd))
		{
			int temp=GetLastError();
			break;
		}
		bRet = TRUE;
	} while (FALSE);
	// over
	if (pSid)
		FreeSid(pSid);
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);
	if (pSD)
		LocalFree(pSD);
	return bRet;
}
BOOL GetFilePrivilege (
	IN PCTSTR ptcsFilePath
	)
	/*++
	Routine Description:
	取得删除文件的权限
	Arguments:
	ptcsFilePath - 输入参数，要取得所有者权限的文件的完整路径
	Return Value:
	成功返回TRUE，失败返回FALSE
	--*/
{
	SECURITY_DESCRIPTOR sd;
	if (!InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION))
		return FALSE;
	if (!SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE))
		return FALSE;
	if (!SetFileSecurity(ptcsFilePath, DACL_SECURITY_INFORMATION, &sd))
		return FALSE;
	return TRUE;
}
BOOL Is64()
	/************************************************************************/
	/* 判断系统为32 位或64 位*/
	/************************************************************************/
{
	SYSTEM_INFO si;
	GetNativeSystemInfo(&si);
	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
		si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 )
	{
		return TRUE;//64 位操作系统
	}
	else
	{
		return FALSE;// 32 位操作系统
	}
}