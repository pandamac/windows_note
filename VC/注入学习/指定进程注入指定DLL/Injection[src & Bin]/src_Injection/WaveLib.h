#ifndef _WAVELIB_H_
#define _WAVELIB_H_

#include <windows.h>
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#define	F_WO_ACTIVE			0x0001
#define	F_WO_NEED_UPDATE	0002
#define	F_WO_EFFECT			0x0004
#define	F_WO_ELLIPSE		0x0008

typedef struct _WAVE_OBJECT
{
	HWND	hWnd;
	DWORD	dwFlag;// 见 F_WO_xxx 组合
//********************************************************************
	HDC	hDcRender;
	HBITMAP	hBmpRender;
	LPVOID		lpDIBitsSource; // 原始像素数据
	LPVOID		lpDIBitsRender; // 用于显示到屏幕的像素数据
	LPVOID		lpWave1; // 水波能量数据缓冲1
	LPVOID		lpWave2; // 水波能量数据缓冲2
//********************************************************************
	DWORD	dwBmpWidth;
	DWORD	dwBmpHeight;
	DWORD	dwDIByteWidth;  //= (dwBmpWidth * 3 + 3) and ~3
	DWORD	dwWaveByteWidth; // = dwBmpWidth * 4
	DWORD	dwRandom;
//********************************************************************
// 特效参数
//********************************************************************
	DWORD	dwEffectType;
	DWORD	dwEffectParam1;
	DWORD	dwEffectParam2;
	DWORD	dwEffectParam3;
//********************************************************************
// 用于行船特效
//********************************************************************
	DWORD	dwEff2X;
	DWORD	dwEff2Y;
	DWORD	dwEff2XAdd;
	DWORD	dwEff2YAdd;
	DWORD	dwEff2Flip;
//********************************************************************
	BITMAPINFO		stBmpInfo;
}WAVE_OBJECT, *LPWAVE_OBJECT;
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
extern "C" 
{
 DWORD WINAPI _WaveDropStone(LPWAVE_OBJECT _lpWaveObject, 
	 DWORD _dwX,
	 DWORD _dwY,
	 DWORD _dwSize, 
	 DWORD _dwWeight);

 DWORD WINAPI _WaveEffect(LPWAVE_OBJECT _lpWaveObject,
			DWORD _dwType,
			DWORD _dwParam1,
			DWORD _dwParam2,
			DWORD _dwParam3);

 DWORD WINAPI _WaveFree(LPWAVE_OBJECT _lpWaveObject);

 DWORD WINAPI _WaveInit(LPWAVE_OBJECT _lpWaveObject,
			HWND _hWnd, 
			HBITMAP _hBmp,
			DWORD _dwSpeed,
			DWORD _dwType);

 DWORD WINAPI _WaveRandom(LPWAVE_OBJECT _lpWaveObject, DWORD _dwMax);

 DWORD WINAPI _WaveRandom16(LPWAVE_OBJECT _lpWaveObject);

 DWORD WINAPI _WaveRender(LPWAVE_OBJECT _lpWaveObject);

 DWORD WINAPI _WaveSpread(LPWAVE_OBJECT _lpWaveObject);

 DWORD WINAPI _WaveTimerProc(HWND _hWnd, UINT _uMsg, int _idEvent, DWORD _dwTime);

 DWORD WINAPI _WaveUpdateFrame(LPWAVE_OBJECT _lpWaveObject,HDC _hDc, BOOL _bIfForce);

}

BOOL WINAPI _Init(LPWAVE_OBJECT stlpWaveObj, HINSTANCE hInstance, HWND hWinMain, UINT uResID)
{
	BITMAP	stBmp = {0};
	RECT	stRect = {0};
	HBITMAP hBitMap = LoadBitmap(hInstance, MAKEINTRESOURCE(uResID));

	DWORD ret = _WaveInit(stlpWaveObj, hWinMain, hBitMap, 30, 0);


//********************************************************************
// 装入资源中的位图、初始化水波对象
//********************************************************************

		if (ret)
		{
			return FALSE;
		}


		DeleteObject(hBitMap);
//********************************************************************
// 将窗口大小修正到位图的大小
//********************************************************************

		GetWindowRect(hWinMain, &stRect);

		SetWindowPos(hWinMain,
			HWND_TOPMOST,
			0,
			0,
			stlpWaveObj->dwBmpWidth,
			stlpWaveObj->dwBmpHeight,
			SWP_NOMOVE);

		_WaveEffect(stlpWaveObj,1,5,4,250);
		_WaveEffect(stlpWaveObj,0,0,0,0);

		return TRUE;

}
#endif