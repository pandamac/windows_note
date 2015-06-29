#include <windows.h>
#include <CommCtrl.h>
#include "WaveLib.h"
#include "resource.h"

#pragma comment(lib, "wavelib.lib")

extern  "C"  WINUSERAPI  BOOL  WINAPI  TrackMouseEvent(LPTRACKMOUSEEVENT  lpEventTrack);

WAVE_OBJECT stWaveObj = {0};

BOOL WINAPI DlgAboutProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{


	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			_Init(&stWaveObj, GetModuleHandle(NULL), hwnd, IDB_BMP_LOGO);

		}
		break;

	case WM_CLOSE:
		{
			_WaveFree(&stWaveObj);
			EndDialog(hwnd, 0);	
		}
		return TRUE;
	case WM_PAINT:
		{
			PAINTSTRUCT	ps = {0};
			HDC hdc = BeginPaint(hwnd, &ps);
			_WaveUpdateFrame(&stWaveObj, hdc,TRUE);
			EndPaint(hwnd, &ps);

		}
		break;
	case WM_LBUTTONDOWN:
		{
			SendMessage(hwnd, WM_NCLBUTTONDOWN, (WPARAM)HTCAPTION, 0);
		}
		break;
	case WM_LBUTTONUP:
		{
		
			_WaveDropStone(&stWaveObj, LOWORD(lParam), HIWORD(lParam), 2, 256);
		}
		break;
	case WM_RBUTTONDOWN:
		{
			switch (stWaveObj.dwEffectType)
			{
			case 0:
				_WaveEffect(&stWaveObj,1,5,4,250);
					break;
			case 1:
				_WaveEffect(&stWaveObj,3,250,4,8);
				break;
			case 3:
				_WaveEffect(&stWaveObj,2,4,2,180);
				break;
			default:
				_WaveEffect(&stWaveObj,0,0,0,0);
			}
		}
		break;
	case WM_MOUSEMOVE:
		{
			TRACKMOUSEEVENT stEventTrack = {0};

			stEventTrack.cbSize = sizeof (TRACKMOUSEEVENT);
			stEventTrack.dwFlags = TME_HOVER;
			stEventTrack.dwHoverTime = 300;
			stEventTrack.hwndTrack = hwnd;

			TrackMouseEvent(&stEventTrack);

			_WaveDropStone(&stWaveObj, LOWORD(lParam), HIWORD(lParam), 2, 256);
			
		}
		break;
	case WM_LBUTTONDBLCLK:
		{
			SendMessage(hwnd, WM_CLOSE, 0, 0);
		}
		break;
	case WM_MOUSEHOVER:
		{
			TRACKMOUSEEVENT stEventTrack = {0};

			stEventTrack.cbSize = sizeof (TRACKMOUSEEVENT);
			stEventTrack.dwFlags = TME_HOVER;
			stEventTrack.dwHoverTime = 300;
			stEventTrack.hwndTrack = hwnd;
			
			_WaveDropStone(&stWaveObj, LOWORD(lParam), HIWORD(lParam), 2, 96);
			
			TrackMouseEvent(&stEventTrack);
		}
		break;
	default:
		break;
	}

	return FALSE;
}
