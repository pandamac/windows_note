#include <WINDOWS.H>
#include <SHLWAPI.H>
#include "resource.h"

//封装的内存映射文件需要用到的结构
typedef struct  
{
	PTCHAR szDll;	//文件名指针
	DWORD dwDesiredAccess;	//CreateFile参数
	DWORD dwShareMode;	//CreateFile参数
	DWORD dwCreationDisposition;	//CreateFile参数
	DWORD flProtect;	//CreateFileMaping参数
	DWORD dwDesiredAccessMap;	//MapViewOfFile参数
	HANDLE hFile;	//打开后的文件句柄
	HANDLE hFileMap;	//Map句柄
}STFILEMAP,*LPSTFILEMAP;

//传递给线程需要用到的结构体信息
typedef struct
{
	PTCHAR lpszExportDll;	//需要复制导出表信息的DLL文件名指针
	PTCHAR lpszTargetDll;	//需要添加中转导出表信息的DLL文件名指针
	PTCHAR lpszTransitName;	//中转DLL名
	PTCHAR lpszSaveName;	//保存的文件名指针
	DWORD dwIsAddSection;	//是否使用新增节方式，为0则使用扩大最后一个节的方式
	HANDLE hDlg;
}STPARAM,*LPSTPARAM;

//函数声明
DWORD WINAPI AddExportThread (lpParameter);
INT_PTR CALLBACK DialogProc (hWnd,uMsg,wParam,lParam);
VOID * WINAPI RvaToOffset (IMAGE_DOS_HEADER *lpFile,VOID *Rva);
LPVOID WINAPI MapFile (LPSTFILEMAP lpFileMap);
BOOL WINAPI FreeMap(LPVOID lpFileMap,LPSTFILEMAP lpstFileMap);
DWORD CalExportLength(LPVOID lpFile,DWORD dwTransitNameLen);
DWORD WINAPI AddSection(PTCHAR lpszDll,DWORD dwLength);
DWORD WINAPI ExpandLastSection(PTCHAR lpszDll,DWORD dwLength);
BOOL WINAPI CopyExportToBuffer(PIMAGE_DOS_HEADER lpFile,DWORD lpExport,DWORD lpBuffer,LPSTR lpTransitName,DWORD Rva);