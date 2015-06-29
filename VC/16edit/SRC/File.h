
/*****************************************************************************

  FILE
  ----

  This is a class for handling files and memory maps of them.

  by yoda

  WWW:      y0da.cjb.net
  E-mail:   LordPE@gmx.net

  You are allowed to use this source code in your own projects if you mention
  my name.

*****************************************************************************/

#ifndef __File_h__
#define __File_h__

#include <windows.h>

//
// constants
//
// modes for GetFileHandle
#define F_OPENEXISTING_R     0
#define F_OPENEXISTING_RW    1
#define F_CREATENEW          2
#define F_TRUNCATE           3

//
// class FILE
//
class FILE
{
public:
	FILE();
	~FILE();
	BOOL          GetFileHandle(char *szFilePath, DWORD dwMode);
	BOOL          GetFileHandleWithMaxAccess(char* szFilePath);
	BOOL          Destroy();
	HANDLE        GetHandle();
	BOOL          IsFileReadOnly();
	BOOL          MapFile();
	void*         GetMapPtr();
	BOOL          UnmapFile();
	BOOL          ReMapFile(DWORD dwNewSize);
	DWORD         GetMapSize();
	BOOL          IsMapped();
	DWORD         GetFSize();
	BOOL          FlushFileMap();
	static BOOL   FileExits(char* szFilePath);
	char*         GetFilePath();
	BOOL          Write(void* pBuff, DWORD dwc);
	BOOL          Read(void* pBuff, DWORD dwc);
	BOOL          SetFPointer(DWORD dwOff);
	BOOL          Truncate();
	void          SetMapPtrSize(void* ptr, DWORD dwSize);

private:
	DWORD         dwMapSize;
	void          *pMap;
	BOOL          bReadOnly;
	HANDLE        hFile;
	char          cFilePath[MAX_PATH];
};

#endif