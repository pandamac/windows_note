#include "StdAfx.h"
#include "MyEdit.h"


CMyEdit::CMyEdit(void)
{
}


CMyEdit::~CMyEdit(void)
{
}
BEGIN_MESSAGE_MAP(CMyEdit, CEdit)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


void CMyEdit::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	TCHAR szFilePath[MAX_PATH+1];
	DragQueryFile(hDropInfo,0,szFilePath,MAX_PATH);
	SetWindowText(szFilePath);
	DragFinish(hDropInfo);

	CEdit::OnDropFiles(hDropInfo);
}
