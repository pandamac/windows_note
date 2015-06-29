////////////////////////////////////////////////
// KeyHookApp.cpp文件

#include "resource.h"
#include "KeyHook.h"
#include "HookDll.h"

#pragma comment(lib, "HookDll")

CMyApp theApp;

BOOL CMyApp::InitInstance()
{
	CMainDialog dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	return FALSE;
}

CMainDialog::CMainDialog(CWnd* pParentWnd):CDialog(IDD_MAIN, pParentWnd)
{
}
//
BEGIN_MESSAGE_MAP(CMainDialog, CDialog)
ON_MESSAGE(HM_KEY, OnHookKey)
END_MESSAGE_MAP()

BOOL CMainDialog::OnInitDialog()
{
	//初始化对话框
	CDialog::OnInitDialog();
	SetIcon(theApp.LoadIcon(IDI_MAIN), FALSE);
	::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 
		0, 0, SWP_NOSIZE|SWP_NOREDRAW|SWP_NOMOVE);

	// 安装钩子
	if(!SetKeyHook(TRUE, 0, m_hWnd))
		MessageBox("安装钩子失败！");

	return TRUE;
}


void CMainDialog::OnCancel()
{
	// 卸载钩子
	SetKeyHook(FALSE);
	CDialog::OnCancel();
	return;
}

long CMainDialog::OnHookKey(WPARAM wParam, LPARAM lParam)
{
	// 此时参数wParam为用户按键的虚拟键码，
	// lParam参数包含按键的重复次数、扫描码、前一个按键状态等信息

	char szKey[80];
	::GetKeyNameText(lParam, szKey, 80);

	CString strItem;
	strItem.Format(" 用户按键：%s \r\n", szKey);
	// 添加到编辑框中
	CString strEdit;
	GetDlgItem(IDC_KEYMSG)->GetWindowText(strEdit);
	GetDlgItem(IDC_KEYMSG)->SetWindowText(strItem + strEdit);

	::MessageBeep(MB_OK);
	return 0;
}


