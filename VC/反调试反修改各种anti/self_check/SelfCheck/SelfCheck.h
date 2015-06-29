#pragma once
#include <windows.h>

class SelfCheck
{
public:
	SelfCheck(void);
	~SelfCheck(void);
	BOOL IsFileModified_By_FileSize();
	BOOL IsFileModified_By_CRC32();
};
