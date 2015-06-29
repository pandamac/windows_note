///////////////////////////////////////////
// MainDir.hÎÄ¼þ	// 07MainDir

#include <afxwin.h>	


class CMyApp : public CWinApp
{
public:
	BOOL InitInstance();
};

class CMainDialog : public CDialog
{
public:
	CMainDialog(CWnd* pParentWnd = NULL);

protected:
	virtual BOOL OnInitDialog();
	virtual void OnCancel();

	afx_msg long OnHookKey(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};