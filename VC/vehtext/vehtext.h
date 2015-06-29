
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the VEHTEXT_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// VEHTEXT_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef VEHTEXT_EXPORTS
#define VEHTEXT_API __declspec(dllexport)
#else
#define VEHTEXT_API __declspec(dllimport)
#endif

// This class is exported from the vehtext.dll
class VEHTEXT_API CVehtext {
public:
	CVehtext(void);
	// TODO: add your methods here.
};

extern VEHTEXT_API int nVehtext;

VEHTEXT_API int fnVehtext(void);

