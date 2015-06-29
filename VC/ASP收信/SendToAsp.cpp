// SendToAsp.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SendToAsp.h"
#include "SendToAspDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSendToAspApp

BEGIN_MESSAGE_MAP(CSendToAspApp, CWinApp)
	//{{AFX_MSG_MAP(CSendToAspApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendToAspApp construction

CSendToAspApp::CSendToAspApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSendToAspApp object

CSendToAspApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSendToAspApp initialization

BOOL CSendToAspApp::InitInstance()
{
	// Standard initialization

	CSendToAspDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
