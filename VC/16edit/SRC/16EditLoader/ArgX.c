
#include "ArgX.h"

//
// prototypes
//

/*
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
*/

void __cdecl __getmainargs(int *pargc, char ***pargv, char ***penvp, BOOL dowildcard, DWORD *pnewmode);

/*
#ifdef __cplusplus
}
#endif // __cplusplus
*/


void getmainargs(int *pargc, char ***pargv, char ***envp)
{
	DWORD dw = 0;

	__getmainargs(pargc, pargv, envp, FALSE, &dw);

	return;
}