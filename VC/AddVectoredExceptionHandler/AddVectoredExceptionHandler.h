
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the ADDVECTOREDEXCEPTIONHANDLER_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// ADDVECTOREDEXCEPTIONHANDLER_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef ADDVECTOREDEXCEPTIONHANDLER_EXPORTS
#define ADDVECTOREDEXCEPTIONHANDLER_API __declspec(dllexport)
#else
#define ADDVECTOREDEXCEPTIONHANDLER_API __declspec(dllimport)
#endif

// This class is exported from the AddVectoredExceptionHandler.dll
class ADDVECTOREDEXCEPTIONHANDLER_API CAddVectoredExceptionHandler {
public:
	CAddVectoredExceptionHandler(void);
	// TODO: add your methods here.
};

extern ADDVECTOREDEXCEPTIONHANDLER_API int nAddVectoredExceptionHandler;

ADDVECTOREDEXCEPTIONHANDLER_API int fnAddVectoredExceptionHandler(void);

