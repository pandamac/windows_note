
#include "OFN.h"
#include <commdlg.h>

OFN::OFN()
{
	cPathSave[0] = cPathOpen[0] = 0;
	memset(&ofnOpen, 0, sizeof(ofnOpen));
	ofnOpen.lStructSize      = sizeof(ofnOpen);
	ofnOpen.hInstance        = GetModuleHandle(NULL);
	ofnOpen.nMaxFile         = sizeof(cPathOpen);
	ofnOpen.lpstrInitialDir  = ".";
	ofnOpen.lpstrFile        = cPathOpen;
	ofnOpen.lpstrTitle       = "Open file...";
	ofnOpen.Flags            = DEFAULT_OFN_OPEN;
	ofnOpen.lpstrFilter      = "*.*\0*.*\0";
	memcpy(&ofnSave, &ofnOpen, sizeof(ofnOpen));
	ofnSave.lpstrFile        = cPathSave;
	ofnSave.lpstrTitle       = "Save file to...";
	ofnSave.Flags            = DEFAULT_OFN_SAVE;
}

OFN::~OFN()
{}

BOOL OFN::GetOpenFilePath()
{
	return GetOpenFileName(&ofnOpen);
}

BOOL OFN::GetSaveFilePath()
{
	return GetSaveFileName(&ofnSave);
}