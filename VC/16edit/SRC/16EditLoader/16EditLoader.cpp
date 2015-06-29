
/*****************************************************************************

  Example for a very simple 16Edit loader

  by yoda

*****************************************************************************/

#define  WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "OFN.h"
#include "..\16EditDll.h"
#include "..\macros.H"
#include "ArgX.h"

#pragma comment(linker,"/ENTRY:main /SUBSYSTEM:WINDOWS /FILEALIGN:512 /MERGE:.rdata=.text /MERGE:.data=.text /SECTION:.text,EWR /IGNORE:4078")

void main()
{
	OFN          ofn;
	HE_SETTINGS  s;
	char         *szCmdl;
	CHAR         **argv, **envp;
	int          argc;

	//
	// file path in cmd line ?
	//
	getmainargs(&argc, &argv, &envp);
	if (argc > 1)
		szCmdl = argv[1];
	else
	{
		//
		// get file path via user
		//
		if (!ofn.GetOpenFilePath())
			goto Exit; // ERR
		szCmdl = ofn.cPathOpen;
	}

	//
	// load file, ...
	//
	ZERO(s);
	s.dwMask       = HE_SET_INPUTFILE | HE_SET_MINIMIZETOTRAY | HE_SET_SAVEWINDOWPOSITION | \
		             HE_SET_RESTOREWINDOWPOSITION;
	s.szFilePath   = szCmdl;
	if (!HESpecifySettings(&s))
	{
		MessageBox(0, "File access error !", "16EditLoader", MB_ICONERROR);
		goto Exit; // ERR
	}

	//
	// enter window loop (with current thread)
	//
	HEEnterWindowLoop();

Exit:
	ExitProcess(0);
}


