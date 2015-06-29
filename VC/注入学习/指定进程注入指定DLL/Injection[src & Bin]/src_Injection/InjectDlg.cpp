#include <windows.h>
#include <windowsX.h>
#include <CommCtrl.h>
#include <tlhelp32.h>
#include <stdio.h>
#include "PSAPI.H"

#include "AboutDlg.h"
#include "ToolFunction.h"
#include "InjectDlg.h"
#include "resource.h"

#pragma comment(lib, "PSAPI.LIB")

HWND g_hListPN = NULL; //进程ListView句柄
HWND g_hListMD = NULL; //进程模块句柄
BOOL bIsAsc	= FALSE;  //是否升序排列
DWORD g_dwPID = 0; //选中进程PID

DWORD WINAPI Brower(LPVOID lParam)
{
	HWND hListView = (HWND)lParam;

	int iSelItem = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);
	
	DWORD dwPID = 0;
	TCHAR strPID[128] = {0};
	TCHAR strImagePath[MAX_PATH] = {0};
	TCHAR strCmdLine[MAX_PATH * 2] = {0};
	
	ListView_GetItemText(hListView, iSelItem, 0, strPID, 127);
	ListView_GetItemText(hListView, iSelItem, 2, strImagePath, MAX_PATH - 1);
	sscanf(strPID, TEXT("%d"), &dwPID);
	
	if ((dwPID != 0) && (dwPID != 4) && lstrlen(strImagePath))
	{
		TCHAR strWinDir[MAX_PATH] = {0};
		
		GetWindowsDirectory(strWinDir, sizeof(strWinDir) * sizeof (TCHAR));
		wsprintf(strCmdLine, TEXT("%s\\Explorer /select,%s"), strWinDir, strImagePath);
		
		STARTUPINFO si = {0};
		si.cb = sizeof (STARTUPINFO);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_SHOW;
		
		GetStartupInfo(&si);
		
		PROCESS_INFORMATION pi = {0};
		
		CreateProcess(NULL,
			strCmdLine,
			NULL, 
			NULL, 
			FALSE,
			0, 
			NULL, 
			NULL, 
			&si,
			&pi);
	}
			
	return 0;
}
DWORD WINAPI BatchUnLoadModule(LPVOID lParam)
{
	HWND hwnd = (HWND) lParam;
	
	int Counts	= ListView_GetItemCount(g_hListMD);
	
	BOOL	bIsSelected;

	TCHAR	strBaseAddr[128] = {0};
	DWORD dwBaseAddr = 0;
	
	while (Counts > 0)
	{
		bIsSelected = (BOOL)ListView_GetCheckState(g_hListMD, --Counts);
		
		if (bIsSelected)
		{
			ListView_GetItemText(g_hListMD, Counts, 0, strBaseAddr, 127);
			
			sscanf(strBaseAddr, TEXT("%d"), &dwBaseAddr);
			
			UnmapViewOfModule(g_dwPID, (DWORD)dwBaseAddr);
			
		}
		
	}

	return 0;
}
DWORD WINAPI BatchTerminateProcess(LPVOID lParam)
{
	HWND hwnd = (HWND) lParam;

	int Counts	= ListView_GetItemCount(g_hListPN);
	
	BOOL	bIsSelected;
	HANDLE	hProcess = NULL;
	TCHAR	strPID[128] = {0};
	DWORD dwPID = 0;
	
	while (Counts > 0)
	{
		bIsSelected = (BOOL)ListView_GetCheckState(g_hListPN, --Counts);
		
		if (bIsSelected)
		{
			ListView_GetItemText(g_hListPN, Counts, 0, strPID, 127);

			sscanf(strPID, TEXT("%d"), &dwPID);

			if ((dwPID == 0) || (dwPID ==4))
				continue;

			HANDLE hProcess = OpenProcess (PROCESS_TERMINATE, TRUE, dwPID);
			
			TerminateProcess(hProcess, 1);
			
			CloseHandle(hProcess);


		}
		
	}

// 刷新界面显示

	Sleep(200);
	
	WaitForSingleObject(CreateThread(NULL, 0, GetPNThread, hwnd, 0, NULL),
		30);
	
	CreateThread(NULL, 0, GetMDThread, hwnd, 0, NULL);	
	
	return 0;
}
DWORD WINAPI InjectThread(LPVOID lParam)
{
	HWND hwnd = (HWND)lParam;

	TCHAR strDll[MAX_PATH] = {0};
	OPENFILENAME ofn = {0};

	ofn.lStructSize = sizeof (OPENFILENAME);
	ofn.Flags		= OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
	ofn.nMaxFile	= MAX_PATH - 1;
	ofn.lpstrFile	= strDll;
	ofn.lpstrFilter = TEXT("*.dll\0*.dll\0\0");

	if (!GetOpenFileName(&ofn))
	{
		return 0;
	}


	if (InjectIt(strDll, g_dwPID))
	{
		MessageBox(hwnd, TEXT("注入成功!"), TEXT("SUCCESS"), MB_OK);
	}

	CreateThread(NULL, 0, GetMDThread, hwnd, 0, NULL);

	return 0;
}

DWORD WINAPI GetPNThread(LPVOID lParam)
{
    BOOL b = FALSE;
	BOOL blnRet = FALSE;
    HANDLE hnd = NULL;
    PROCESSENTRY32 pe = {0};

	HWND hwnd = (HWND)lParam;
	TCHAR strCountOfProcess[MAX_PATH] = {0};
	TCHAR strImagePath[MAX_PATH] = {0};

	LV_ITEMA	lvItem = {0};

    //得到进程快照
    hnd = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    pe.dwSize = sizeof(pe);
    b=Process32First(hnd, &pe);

	ListView_DeleteAllItems(g_hListPN);

    while(b)
    {
		TCHAR strTmpPID[128] = {0};
	
		if ((pe.th32ProcessID != 0) && (pe.th32ProcessID != 4))
		{
			HINSTANCE hProc = (HINSTANCE)OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 
															FALSE, 
															pe.th32ProcessID); 
		
			GetModuleFileNameEx(hProc, NULL, strImagePath, MAX_PATH - 1);
			CloseHandle(hProc);
		}

		wsprintf(strTmpPID, TEXT("%4d"), pe.th32ProcessID);
		// 向ListView插入项
		lvItem.iItem = 0;
		lvItem.mask	= LVIF_TEXT;
		ListView_InsertItem(g_hListPN, &lvItem);
		// 输出到ListView

        ListView_SetItemText(g_hListPN, 0, 0, strTmpPID); // PID
        ListView_SetItemText(g_hListPN, 0, 1, pe.szExeFile); // 进程名称
		ListView_SetItemText(g_hListPN, 0, 2, strImagePath); // 进程完整路径

		b=Process32Next(hnd,&pe);
    }

	CloseHandle(hnd);
	
	wsprintf(strCountOfProcess, TEXT("共获取 %d 个进程"), ListView_GetItemCount(g_hListPN));
	SetWindowText(hwnd, strCountOfProcess);

	return 0;
}

DWORD WINAPI GetMDThread(LPVOID lParam)
{
	HWND hwnd = (HWND)lParam;
	DWORD dwPID = 0;

	TCHAR strTmpPID[128] = {0};
	TCHAR strTmpBaseAddr[128] = {0};
	
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE; 
	MODULEENTRY32 me32 = {0}; 

	TCHAR strMDInfo[255] = {0};
	LV_ITEMA	lvItem = {0};
	
	DWORD dwSeleted = ListView_GetNextItem(g_hListPN, -1, LVNI_SELECTED);

	ListView_GetItemText(g_hListPN, dwSeleted, 0, strTmpPID, 127);

	g_dwPID = dwPID = atol(strTmpPID);

//	忽略PID为0和4的进程

	if ((dwPID == 0) || (dwPID == 4))
	{
		ListView_DeleteAllItems(g_hListMD);
		wsprintf(strMDInfo, TEXT("%s"), TEXT(""));
		SetDlgItemText(hwnd, IDC_STMD, strMDInfo);
		
		return 0;
	}

	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID); 

	if(hModuleSnap == INVALID_HANDLE_VALUE ) 
	{ 
   
		return 0; 
	}

	me32.dwSize = sizeof( MODULEENTRY32 ); 

	if(!Module32First(hModuleSnap, &me32)) 
	{ 

		CloseHandle(hModuleSnap);

		return 0; 
	}

	ListView_DeleteAllItems(g_hListMD);

	do 
	{ 

		wsprintf(strTmpBaseAddr, TEXT("%8X"), me32.modBaseAddr);
		// 向ListView插入项
		lvItem.iItem = 0;
		lvItem.mask	= LVIF_TEXT;
		ListView_InsertItem(g_hListMD, &lvItem);
		// 输出到ListView

        ListView_SetItemText(g_hListMD, 0, 0, strTmpBaseAddr); // 模块基地址
        ListView_SetItemText(g_hListMD, 0, 1, me32.szModule); // 模块名称
		ListView_SetItemText(g_hListMD, 0, 2, me32.szExePath); // 模块完整路径
	} while(Module32Next(hModuleSnap, &me32));


	CloseHandle(hModuleSnap); 

	TCHAR strExeName[MAX_PATH] = {0};

	ListView_GetItemText(g_hListPN, dwSeleted, 1, strExeName, MAX_PATH - 1);
	wsprintf(strMDInfo, TEXT("共找到 %d 个模块 [ 主进程PID为: %d - %s ]"),
		ListView_GetItemCount(g_hListMD), g_dwPID, strExeName);
	SetDlgItemText(hwnd, IDC_STMD, strMDInfo);

	return 0;
}
////////////////////////////////////////////////////////////////////////////////
// 
int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    TCHAR    szParam1[MAX_PATH];
    TCHAR    szParam2[MAX_PATH];

    LPNMLISTVIEW    lpnmlv = (LPNMLISTVIEW)lParamSort;
	
	ListView_GetItemText(lpnmlv->hdr.hwndFrom, lParam1, lpnmlv->iSubItem, szParam1, MAX_PATH);
	ListView_GetItemText(lpnmlv->hdr.hwndFrom, lParam2, lpnmlv->iSubItem, szParam2, MAX_PATH);

	if (bIsAsc)
	{
		return lstrcmp(szParam1, szParam2); //升序
	}
	else
	{
		return lstrcmp(szParam2, szParam1); //降序
	}

	
}
////////////////////////////////////////////////////////////////////////////////
//  DlgMain_OnInitDialog
BOOL DlgMain_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	HINSTANCE hIns = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);

	HICON hIcon = LoadIcon(hIns, MAKEINTRESOURCE(IDI_ICON_MAIN));

	SendMessage(hwnd, WM_SETICON, (WPARAM)ICON_BIG, (LPARAM)hIcon);

	SendMessage(hwnd, WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)hIcon);

	if(!EnableDebugPriv(SE_DEBUG_NAME))
    {
        // Add Privilege Error
		MessageBox(hwnd, TEXT("提升自身权限失败！"), TEXT("Error"), MB_OK | MB_ICONERROR);
        return FALSE;
    }
	else
	{
		//  初始化ListView控件       /////////////////
		g_hListPN = GetDlgItem(hwnd, IDC_LSTPN);
		g_hListMD = GetDlgItem(hwnd, IDC_LSTMODULE);

		LV_COLUMN	lvInfo = {0} ;
	// 初始化  进程 ListView 控件//////////////////////////	
		lvInfo.cchTextMax = MAX_PATH ;
		lvInfo.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH ;
		lvInfo.iSubItem = 0 ;
		lvInfo.pszText = TEXT("进程ID") ;
		lvInfo.cx = 65;

		SendMessage(g_hListPN, LVM_SETEXTENDEDLISTVIEWSTYLE, (WPARAM)(int)0,
				(LPARAM)(LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES));
			
		ListView_InsertColumn(g_hListPN, 0, &lvInfo);

		lvInfo.cx = 155;
		lvInfo.pszText = TEXT("进程名称");
	
		ListView_InsertColumn(g_hListPN, 1, &lvInfo);

		lvInfo.cx = 235;
		lvInfo.pszText = TEXT("进程完整路径");
	
		ListView_InsertColumn(g_hListPN, 2, &lvInfo);
		/////////////////////////////////////////////
 
		CreateThread(NULL, 0, GetPNThread, hwnd, 0, NULL);
		/////////////////////////////////////////////
		// 初始化 模块ListView 
		lvInfo.pszText = TEXT("映像基地址") ;
		lvInfo.cx = 105;
		SendMessage(g_hListMD, LVM_SETEXTENDEDLISTVIEWSTYLE, (WPARAM)(int)0,
				(LPARAM)(LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES));
			
		ListView_InsertColumn(g_hListMD, 0, &lvInfo);

		lvInfo.cx = 105;
		lvInfo.pszText = TEXT("模块名称");
		ListView_InsertColumn(g_hListMD, 1, &lvInfo);

		lvInfo.cx = 235;
		lvInfo.pszText = TEXT("模块完整路径");
		ListView_InsertColumn(g_hListMD, 2, &lvInfo);
		//////////////////////////////////////////////////////////////////////////
		// 加载 状态栏
		HWND hSBar = CreateStatusWindow(WS_VISIBLE | WS_CHILD |  WS_BORDER,
			NULL,
			hwnd,
			6000 /*状态栏ID*/);
		
		int iSBWidths[]	= {200,-1};
		
		SendMessage(hSBar,
			SB_SETPARTS,
			(WPARAM)(sizeof(iSBWidths)/sizeof(int)),
			(LPARAM)(LPINT)iSBWidths);
		
		SendMessage(hSBar, SB_SETTEXT, (WPARAM)0, (LPARAM)TEXT("http://hi.baidu.com/cntrump"));
		SendMessage(hSBar, SB_SETTEXT, (WPARAM)1, (LPARAM)TEXT("By cntrump\t\tCopyright 2009"));


	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
//  DlgMain_OnNotify
VOID WINAPI DlgMain_OnNotify(HWND hwnd, LPNMHDR lpStuNotify, LRESULT *Result)
{

	switch (lpStuNotify->idFrom)
	{
	case IDC_LSTPN: // 进程ListView
		{
			switch (lpStuNotify->code)
			{
			case LVN_COLUMNCLICK: //单击表头
				{
			///////   执行排序  ///////////////
					if (!bIsAsc)
					{
						bIsAsc = TRUE; //升序排列
					}
					else
					{
						bIsAsc = FALSE; //降序排列
					}

				//	ListView_SortItemsEx(g_hListPN, CompareFunc, lpStuNotify);	
					SendMessage(g_hListPN, 
						(int)LVM_SORTITEMSEX, 
						(WPARAM)lpStuNotify, 
						(LPARAM)CompareFunc);
          ////////////////////////////////////////////////
				}
				break;

			case NM_RCLICK:
				{
					HMENU	hMainMenu = NULL;
					HMENU	hSubMenu = NULL;
					POINT	stPos = {0};
					int		iSelItem = 0;

					// 点击右键也更新模块ListView的内容
					CreateThread(NULL, 0, GetMDThread, hwnd, 0, NULL);

					int		Count = ListView_GetItemCount(g_hListPN);
					iSelItem = ListView_GetNextItem(g_hListPN, -1, LVNI_SELECTED);
					
					if ((Count > 0) && (iSelItem != -1))
					{
						GetCursorPos(&stPos);
						hMainMenu = LoadMenu((HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), 
							MAKEINTRESOURCE(IDR_MENU_PN));
						
						hSubMenu = GetSubMenu(hMainMenu, 0);
						
						TrackPopupMenu(hSubMenu, 
							TPM_LEFTALIGN,
							stPos.x,
							stPos.y,
							NULL,
							hwnd, //做为主对话框的子菜单
							NULL);
						
						DestroyMenu(hSubMenu);
						
					}
					
				}
				break;
			case NM_CLICK:
				{
					CreateThread(NULL, 0, GetMDThread, hwnd, 0, NULL);
				}
				break;
			default:
				break;
				
			}
		}
		break;
	case IDC_LSTMODULE: //模块ListView
		{
			switch (lpStuNotify->code)
			{
			case LVN_COLUMNCLICK: //单击表头
				{
			///////   执行排序  ///////////////
					if (!bIsAsc)
					{
						bIsAsc = TRUE; //升序排列
					}
					else
					{
						bIsAsc = FALSE; //降序排列
					}

				//	ListView_SortItemsEx(g_hListPN, CompareFunc, lpStuNotify);	
					SendMessage(g_hListMD, 
						(int)LVM_SORTITEMSEX, 
						(WPARAM)lpStuNotify, 
						(LPARAM)CompareFunc);
          ////////////////////////////////////////////////
				}
				break;
			case NM_RCLICK:
				{
					HMENU	hMainMenu = NULL;
					HMENU	hSubMenu = NULL;
					POINT	stPos = {0};
					int		iSelItem = 0;
					
					int		Count = ListView_GetItemCount(g_hListMD);
					iSelItem = ListView_GetNextItem(g_hListMD, -1, LVNI_SELECTED);
					
					if ((Count > 0) && (iSelItem != -1))
					{
						GetCursorPos(&stPos);
						hMainMenu = LoadMenu((HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), 
							MAKEINTRESOURCE(IDR_MENU_MD));

						hSubMenu = GetSubMenu(hMainMenu, 0);
						
						TrackPopupMenu(hSubMenu, 
							TPM_LEFTALIGN,
							stPos.x,
							stPos.y,
							NULL,
							hwnd, //做为主对话框的子菜单
							NULL);
						
						DestroyMenu(hSubMenu);
						
					}
					
				}
				break;
			default:
				break;
				
			}		
		}
		break;
	default :
		break;
	}

//	Result = NULL;

	return;
}
////////////////////////////////////////////////////////////////////////////////
//  DlgMain_OnCommand
void DlgMain_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
	case ID_MENU_ABOUT:
		{
			DialogBox((HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), 
				MAKEINTRESOURCE(IDD_DLG_ABOUT), hwnd, DlgAboutProc);
		}
		break;
	case ID_MENU_LOOKUPDLL:
		{
			Brower(g_hListMD);
		}
		break;
	case ID_MENU_LOOKUPEXE:
		{
			Brower(g_hListPN);
		}
		break;
	case ID_MENU_KILLMUTIMD:
		{
			CreateThread(NULL, 0, BatchUnLoadModule, hwnd, 0, NULL);
		}
		break;
	case ID_MENU_KILLMUTIPN:
		{
			CreateThread(NULL, 0, BatchTerminateProcess, hwnd, 0, NULL);
		}
		break;
	case ID_MENU_EXIT:
		{
			SendMessage(hwnd, WM_CLOSE, (WPARAM)0, (LPARAM)0);
		}
		break;
	case ID_MENU_FLUSH: //刷新进程
		{
			CreateThread(NULL, 0, GetPNThread, hwnd, 0, NULL);
		}
		break;
	case ID_MENU_INJECT:
		{
			CreateThread(NULL, 0, InjectThread, hwnd, 0, NULL);
		}
		break;
	case ID_MENU_KILL:
		{

			HANDLE hProcess = OpenProcess (PROCESS_TERMINATE, TRUE, g_dwPID);

			TerminateProcess(hProcess, 1);

			CloseHandle(hProcess);

			Sleep(200);
			
			WaitForSingleObject(CreateThread(NULL, 0, GetPNThread, hwnd, 0, NULL),
				30);

			CreateThread(NULL, 0, GetMDThread, hwnd, 0, NULL);
			

			
		}
		break;

//////////// 处理菜单命令 ////////////////
	case ID_MENU_UNLOAD:
		{
			TCHAR strBaseAddr[128] = {0};
			ListView_GetItemText(g_hListMD,
				ListView_GetNextItem(g_hListMD, -1, LVNI_SELECTED),
				0, strBaseAddr, 127);
			
			DWORD dwBaseAddr = 0;

			sscanf(strBaseAddr, TEXT("%X"), &dwBaseAddr);

			if (UnmapViewOfModule(g_dwPID, (DWORD)dwBaseAddr))
			{
				MessageBox(hwnd, TEXT("卸载成功！"), TEXT("SUCCESS"), MB_OK);
			}
			
		}
		break;
	}

	return;
}

////////////////////////////////////////////////////////////////////////////////
//  DlgMain_OnClose
void DlgMain_OnClose(HWND hwnd)
{
	EndDialog(hwnd, 0);
}

BOOL WINAPI DlgProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	HANDLE_MSG(hDlg, WM_COMMAND, DlgMain_OnCommand);
	HANDLE_MSG(hDlg, WM_CLOSE, DlgMain_OnClose);
	HANDLE_MSG(hDlg, WM_INITDIALOG, DlgMain_OnInitDialog);
	
	case WM_NOTIFY:
	{
		DlgMain_OnNotify(hDlg, (LPNMHDR)lParam, (LRESULT *)wParam);
	}
	return TRUE;
	
	}

	return FALSE;
}