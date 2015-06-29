///////////////////////////////////////////
// KeyHookLib.h文件

// 定义函数修饰宏，方便引用本DLL工程的导出函数
#ifdef KEYHOOKLIB_EXPORTS
#define KEYHOOKLIB_API __declspec(dllexport)
#else
#define KEYHOOKLIB_API __declspec(dllimport)
#endif

// 自定义与主程序通信的消息
#define HM_KEY WM_USER + 101

// 声明要导出的函数
BOOL KEYHOOKLIB_API WINAPI SetKeyHook(BOOL bInstall, 
				      DWORD dwThreadId = 0, HWND hWndCaller = NULL);



