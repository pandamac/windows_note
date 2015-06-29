#include "stdafx.h"

XLIB::CXHookAPI m_HookMessageBox;

//新的函数
int WINAPI myMessageBox(
  HWND hWnd, 
  LPCSTR lpText, 
  LPCSTR lpCaption, 
  UINT uType
)
{
	//调用源函数
	return m_HookMessageBox.CallFunction( 4,0,"hook后",lpCaption,uType );
}

//过虑函数
VOID __cdecl filter( XLIB::CStack_ESP* pStack )
{
	printf("0x%08x\n",pStack->GetReturnAddress());
	printf(" argu2 = %s\n",pStack->GetArgument(1));
}

int _tmain(int argc, _TCHAR* argv[])
{


	LPVOID lpAddr = XLIB::CXHookAPI::GetAddressByNameA( "user32.dll","MessageBoxA");
	
	//inline hook测试
	MessageBoxA( 0,"hook前",0,0);
	//m_HookMessageBox.InlineHookFunction( lpAddr,myMessageBox );

	//你在这里，还可以hook输出表，或者输入表等
	m_HookMessageBox.HookImportTable( GetModuleHandle(0),"user32.dll","MessageBoxA",myMessageBox);

	//过滤测试
	//m_HookMessageBox.InlineHookAddress( lpAddr,filter);

	MessageBoxA( 0,"hook前",0,0);

	system("pause");
	return 0;
}

