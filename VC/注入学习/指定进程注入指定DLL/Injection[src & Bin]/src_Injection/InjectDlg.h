#ifndef _INJECT_H_
#define _INJECT_H_

#include <windows.h>

#define LVM_SORTITEMSEX         (LVM_FIRST+81) 

////////////////////////////////////////////////////////////////////////////
//
DWORD WINAPI InjectThread(LPVOID lParam);
int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
DWORD WINAPI GetMDThread(LPVOID lParam);
DWORD WINAPI GetPNThread(LPVOID lParam);
///////////////////////////////////////////////////////////////////////////
// 窗体消息处理函数  

void DlgMain_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void DlgMain_OnClose(HWND hwnd);
BOOL WINAPI DlgProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
VOID WINAPI DlgMain_OnNotify(HWND hwnd, LPNMHDR lpStuNotify, LRESULT *Result);


#endif