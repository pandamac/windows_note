// PNGFile.h: interface for the CPNGFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PNGFILE_H__5DC96E73_0CAA_45A2_9702_8B25A5F279BD__INCLUDED_)
#define AFX_PNGFILE_H__5DC96E73_0CAA_45A2_9702_8B25A5F279BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CalcCRC32.h"
#include "chunk.h"
#include "idatchunk.h"
#include "ihdrchunk.h"
#include "physchunk.h"
#include "iendchunk.h"

class CPNGFile  
{
	CIHDRChunk m_ihdr;
	CpHYsChunk m_phys;
	CIDATChunk m_idat;
	CIENDChunk m_iend;

	static const unsigned char m_szHeader[8];
public:
	bool CreatePNGFromBMP( char *szSrcFile, char *szDstFile, int nDstBitDepth);
	bool CreatePNGFromRawRGB(int nWidth, int nHeight, int nPadBytes, BYTE *pData, BYTE *pDstData, int &nDstLen, BOOL bDither );
	CPNGFile();
	virtual ~CPNGFile();

};

#endif // !defined(AFX_PNGFILE_H__5DC96E73_0CAA_45A2_9702_8B25A5F279BD__INCLUDED_)
