// IDATChunk.h: interface for the CIDATChunk class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IDATCHUNK_H__7CB01392_7754_44C0_B8B5_2DD4FD8159BD__INCLUDED_)
#define AFX_IDATCHUNK_H__7CB01392_7754_44C0_B8B5_2DD4FD8159BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Chunk.h"
#include <windows.h>

class CIDATChunk : public CChunk  
{
	bool SetImageData( unsigned char *szBuf, long nLen );
public:	
	unsigned long m_ulHeight;
	unsigned long m_ulWidth;
	bool SetBitmap( char *szFilename, int nDstBitDepth );
	bool SetBitmap( BITMAPINFO *pBmp, unsigned char *pData, int nDstBitDepth);
	bool SetBitmap( int nWidth, int nHeight, int nPadBytes, unsigned char *pData, int nDstBitDepth, BOOL bDither = FALSE );
	bool SetRawBMPData( long lWidth, long lHeight, int nBitDepth, unsigned char *szData, int nDstBitDepth );
	
	CIDATChunk();
	virtual ~CIDATChunk();

};

#endif // !defined(AFX_IDATCHUNK_H__7CB01392_7754_44C0_B8B5_2DD4FD8159BD__INCLUDED_)
