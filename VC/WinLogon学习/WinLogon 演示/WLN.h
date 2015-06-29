
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the WLN_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// WLN_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef WLN_EXPORTS
#define WLN_API __declspec(dllexport)
#else
#define WLN_API __declspec(dllimport)
#endif

typedef struct _WLX_NOTIFICATION_INFO {
	ULONG	Size;
	ULONG	Flags;
	PWSTR	UserName;
	PWSTR	Domain;
	PWSTR	WindowStation;
	HANDLE	hToken;
	HDESK	hDesktop;
	void*	pStatusCallback;
} WLX_NOTIFICATION_INFO, *PWLX_NOTIFICATION_INFO;

extern "C" WLN_API int fnStartSS(PWLX_NOTIFICATION_INFO pI);
extern "C" WLN_API int fnStopSS(PWLX_NOTIFICATION_INFO pI);
