// IHDRChunk.h: interface for the CIHDRChunk class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IHDRCHUNK_H__B7C10631_8EF9_407F_AF1B_C6F8482FB161__INCLUDED_)
#define AFX_IHDRCHUNK_H__B7C10631_8EF9_407F_AF1B_C6F8482FB161__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Chunk.h"

class CIHDRChunk : public CChunk  
{
public:
	void SetValue( unsigned long ulWidth, unsigned long ulHeight, unsigned char ucBitDepth, unsigned char ucColorType, unsigned char ucCompressionMode, unsigned char ucFilterMode, unsigned char ucInterlaceMethod );
	CIHDRChunk();
	virtual ~CIHDRChunk();

};

#endif // !defined(AFX_IHDRCHUNK_H__B7C10631_8EF9_407F_AF1B_C6F8482FB161__INCLUDED_)
