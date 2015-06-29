#pragma once

class CWindowsService
{
public:
	CWindowsService(void);
public:
	~CWindowsService(void);
	enum SEV_STATUS
	{
		SEV_ERROR	= 0x00,
		SEV_NO		= 0x01,
		SEV_HAVE	= 0x02,
		SEV_RUNING	= 0x03,
		SEV_STOPED	= 0x04
	};
public:
	BOOL AddService(const char*pSourceName,const char*pServiceName,const char*pDisName,const char*pPara);
	BOOL RemoveService(const char*pServiceName);
	BYTE CheckServiceStatus(const char*pServiceName);
	BOOL StartSevice(const char*pServiceName);
	BOOL StopSevice(const char*pServiceName);
	BOOL GetSevicePath(const char*pServiceName,CString &strServicePath);
	BOOL GetCurPath(CString &strCurPath);
};

