#include <windows.h>
#include "AddExport.h"
#include <iostream.h>
#include <shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")
HINSTANCE hInst;	//保存实例句柄

/*
创建HijackDll的线程
*/
DWORD WINAPI AddExportThread (lpParameter)
{
	LPVOID	lpMapOfExportDll;
	LPVOID	lpMapOfDupDll;
	STFILEMAP	FileMapOfDupDll;
	STFILEMAP	FileMapOfExportDll;

	BOOL	bCopy;
	DWORD	dwRva;
	DWORD	dwLen;
	DWORD	lpBuff;
	
	PIMAGE_NT_HEADERS lpNtHead;
	PIMAGE_OPTIONAL_HEADER32 lpOptionHead;
	PIMAGE_DATA_DIRECTORY lpDataDir;
	PIMAGE_EXPORT_DIRECTORY lpExport;
	PIMAGE_DOS_HEADER lpDos;

	LPSTPARAM lpstParam = lpParameter;

	//拷贝目标DLL副本
	bCopy = CopyFileA(lpstParam->lpszTargetDll,lpstParam->lpszSaveName,TRUE);
	if (!bCopy)
	{
		return MessageBox(NULL,TEXT ("创建目标文件副本失败!"),TEXT ("Error!"),MB_OK);
	}

	//内存映射提供导出表的DLL文件
	FileMapOfExportDll.szDll = lpstParam->lpszExportDll;
	FileMapOfExportDll.dwDesiredAccess = GENERIC_READ;
	FileMapOfExportDll.dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
	FileMapOfExportDll.dwCreationDisposition = OPEN_EXISTING;
	FileMapOfExportDll.flProtect = PAGE_READONLY;
	FileMapOfExportDll.dwDesiredAccessMap = FILE_MAP_READ;
	lpMapOfExportDll = MapFile(&FileMapOfExportDll);
	if (!lpMapOfExportDll)
	{
		return MessageBox(NULL,TEXT("打开需要复制的DLL文件出错"),TEXT("Error!"),MB_OK);
	}
	//计算导出表所有信息需要的空间大小
	dwLen = CalExportLength(lpMapOfExportDll,lstrlen(lpstParam->lpszTransitName));
	if (dwLen == 0)
	{
		FreeMap(lpMapOfExportDll,&FileMapOfExportDll);
		return MessageBox(NULL,TEXT("计算导出表大小出错"),TEXT("Error!"),MB_OK);
	}
	if (lpstParam->dwIsAddSection)
	{
		//为目标文件副本增加新节
		FileMapOfDupDll.szDll = lpstParam->lpszSaveName;
		dwRva = AddSection(FileMapOfDupDll.szDll,dwLen);	//得到新增节的Rva地址，以便后续复制导出表信息时使用
		if (!dwRva)
		{
			FreeMap(lpMapOfExportDll,&FileMapOfExportDll);
			DeleteFile(FileMapOfDupDll.szDll);
			return MessageBox(NULL,TEXT("为目标文件副本增加新节失败，请尝试使用扩大最后一个节选项!"),TEXT("Error!"),MB_OK);
		}
	}
	else
	{
		//为目标文件扩大最后一个节
		FileMapOfDupDll.szDll = lpstParam->lpszSaveName;
		dwRva = ExpandLastSection(FileMapOfDupDll.szDll,dwLen);	//得到新增节的Rva地址，以便后续复制导出表信息时使用
		if (!dwRva)
		{
			FreeMap(lpMapOfExportDll,&FileMapOfExportDll);
			DeleteFile(FileMapOfDupDll.szDll);
			return MessageBox(NULL,TEXT("为目标文件副本扩大最后一个节失败!"),TEXT("Error!"),MB_OK);
		}
	}

	//拷贝导出表相关数据

	lpNtHead = (PIMAGE_NT_HEADERS32)((byte *)lpMapOfExportDll + ((PIMAGE_DOS_HEADER)lpMapOfExportDll)->e_lfanew);
	lpOptionHead = (PIMAGE_OPTIONAL_HEADER32)((byte *)lpNtHead + sizeof (DWORD) + sizeof (IMAGE_FILE_HEADER));
	//得到数据目录的第一个成员地址，也就是导出表目录
	lpDataDir = (PIMAGE_DATA_DIRECTORY)(&lpOptionHead->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]);
	//得到导出表在内存中的地址
	lpExport = RvaToOffset(lpMapOfExportDll,lpDataDir->VirtualAddress);
	//内存映射生成的DLL文件
	FileMapOfDupDll.dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
	FileMapOfDupDll.dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
	FileMapOfDupDll.dwCreationDisposition = OPEN_EXISTING;
	FileMapOfDupDll.flProtect = PAGE_READWRITE;
	FileMapOfDupDll.dwDesiredAccessMap = FILE_MAP_READ | FILE_MAP_WRITE;
	lpMapOfDupDll = MapFile(&FileMapOfDupDll);
	//将前面得到的生成的DLL文件新增节的Rva地址转化为内存中的地址
	lpBuff = RvaToOffset(lpMapOfDupDll,dwRva);
	lpDos = lpMapOfExportDll;
	//拷贝所有导出表信息，转化为中转函数的形式
	CopyExportToBuffer(lpDos,lpExport,lpBuff,lpstParam->lpszTransitName,dwRva);
	FreeMap(lpMapOfExportDll,&FileMapOfExportDll);
	FreeMap(lpMapOfDupDll,&FileMapOfDupDll);
	MessageBox(lpstParam->hDlg,TEXT ("生成HijackDll成功!"),TEXT ("Success!"),MB_OK);
	return 0;
}

INT_PTR CALLBACK DialogProc (hWnd,uMsg,wParam,lParam)
{
	OPENFILENAME stOpenExportDll;
	OPENFILENAME stOpenTargetDll;
	OPENFILENAME stSaveFile;
	static TCHAR szExportExt[] = "Dll Files(*.dll)\0*.dll\0All Files(*.*)\0*.*\0";
	static TCHAR szExportPath[MAX_PATH];
	static TCHAR szTargetExt[] = "Dll Files(*.dll)\0*.dll\0All Files(*.*)\0*.*\0";
	static TCHAR szSaveExt[] = "Dll Files(*.dll)\0*.dll\0All Files(*.*)\0*.*\0";
	static TCHAR szTargetPath[MAX_PATH];
	static TCHAR szTransitName[MAX_PATH];
	static TCHAR szSaveName[MAX_PATH];
	DWORD dwThreadId;
	static STPARAM stParameter;

	switch (uMsg)
	{
	case (WM_COMMAND):
		switch (LOWORD(wParam))
		{
		case (IDC_EXPORTDLL):
			break;
		case (IDC_TARGETDLL):
			break;
		case (IDC_EXPORTDLL_BROWSE):
			RtlZeroMemory(&stOpenExportDll,sizeof (OPENFILENAME));
			stOpenExportDll.lStructSize = sizeof (OPENFILENAME);
			stOpenExportDll.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
			stOpenExportDll.hwndOwner = hWnd;
			stOpenExportDll.lpstrFilter = szExportExt;
			stOpenExportDll.lpstrFile = szExportPath;
			stOpenExportDll.nMaxFile = MAX_PATH * sizeof(TCHAR);
			if (GetOpenFileName(&stOpenExportDll))
				SetDlgItemText(hWnd,IDC_EXPORTDLL,szExportPath);
			break;
		case (IDC_TARGETDLL_BROWSE):
			RtlZeroMemory(&stOpenTargetDll,sizeof (OPENFILENAME));
			stOpenTargetDll.lStructSize = sizeof (OPENFILENAME);
			stOpenTargetDll.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
			stOpenTargetDll.hwndOwner = hWnd;
			stOpenTargetDll.lpstrFilter = szTargetExt;
			stOpenTargetDll.lpstrFile = szTargetPath;
			stOpenTargetDll.nMaxFile = MAX_PATH * sizeof(TCHAR);
			if (GetOpenFileName(&stOpenTargetDll))
				SetDlgItemText(hWnd,IDC_TARGETDLL,szTargetPath);
			break;
		case (IDC_TRANSITNAME):
			break;
		case (IDC_ADDSECTION):
			CheckRadioButton(hWnd,IDC_ADDSECTION,IDC_EXPANDSECTION,IDC_ADDSECTION);
			break;
		case (IDC_EXPANDSECTION):
			CheckRadioButton(hWnd,IDC_ADDSECTION,IDC_EXPANDSECTION,IDC_EXPANDSECTION);
			break;
		case (IDOK):
			//检测所填项是否为空
			if (!GetDlgItemText(hWnd,IDC_EXPORTDLL,szExportPath,MAX_PATH * sizeof(TCHAR)) ||
				!GetDlgItemText(hWnd,IDC_TARGETDLL,szTargetPath,MAX_PATH * sizeof(TCHAR)) ||
				!GetDlgItemText(hWnd,IDC_TRANSITNAME,szTransitName,MAX_PATH * sizeof(TCHAR)))
				MessageBox(hWnd,TEXT ("您有项目未填写!"),TEXT ("Error!"),MB_OK);
			//检测所填项是否为存在的文件
			else if (!PathFileExists(szExportPath) ||
					!PathFileExists(szTargetPath))
				MessageBox(hWnd,TEXT ("您填写的文件不存在!"),TEXT ("Error!"),MB_OK);
			else
			{
				RtlZeroMemory(&stSaveFile,sizeof (OPENFILENAME));
				stSaveFile.lStructSize = sizeof (OPENFILENAME);
				stSaveFile.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
				stSaveFile.hwndOwner = hWnd;
				stSaveFile.lpstrFilter = szSaveExt;
				stSaveFile.lpstrFile = szSaveName;
				stSaveFile.nMaxFile = MAX_PATH * sizeof(TCHAR);
				//获取需要保存的文件名
				if (!GetSaveFileName(&stSaveFile))
					return TRUE;
				stParameter.lpszSaveName = szSaveName;
				stParameter.dwIsAddSection = IsDlgButtonChecked(hWnd,IDC_ADDSECTION);
				stParameter.lpszExportDll = szExportPath;
				stParameter.lpszTargetDll = szTargetPath;
				stParameter.lpszTransitName = szTransitName;
				stParameter.hDlg = hWnd;
				//创建线程生成文件
				CreateThread(NULL,NULL,AddExportThread,&stParameter,0,&dwThreadId);
				CloseHandle(dwThreadId);
			}
			break;
		case (IDCANCEL):
			EndDialog(hWnd,NULL);
			break;
		}
		return TRUE;
		break;
	case (WM_INITDIALOG):
		SendMessage(hWnd,WM_SETICON,ICON_BIG,LoadIcon(hInst,IDI_ICON1));
		SendDlgItemMessage(hWnd,IDC_EXPORTDLL,EM_SETLIMITTEXT,MAX_PATH * sizeof(TCHAR),NULL);
		SendDlgItemMessage(hWnd,IDC_TARGETDLL,EM_SETLIMITTEXT,MAX_PATH * sizeof(TCHAR),NULL);
		SendDlgItemMessage(hWnd,IDC_TRANSITNAME,EM_SETLIMITTEXT,MAX_PATH * sizeof(TCHAR),NULL);
		CheckDlgButton(hWnd,IDC_ADDSECTION,BST_CHECKED);
		return TRUE;
		break;
	case (WM_CLOSE):
		EndDialog(hWnd,NULL);
		return TRUE;
		break;
	default:
		return FALSE;
	}
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{

	hInst = hInstance;
	DialogBoxParam(hInstance,DLG_MAIN,NULL,DialogProc,NULL);
	ExitProcess(NULL);

}
