// IHDRChunk.cpp: implementation of the CIHDRChunk class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IHDRChunk.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIHDRChunk::CIHDRChunk()
{
	this->SetChunkType( 0x49484452l);
}

CIHDRChunk::~CIHDRChunk()
{

}


void CIHDRChunk::SetValue(unsigned long ulWidth, unsigned long ulHeight, unsigned char ucBitDepth, unsigned char ucColorType, unsigned char ucCompressionMode, unsigned char ucFilterMode, unsigned char ucInterlaceMethod)
{
	unsigned char szData[13];
	this->PutDWORDintoBufferWithNetworkOrder( szData, ulWidth );
	this->PutDWORDintoBufferWithNetworkOrder( szData+4, ulHeight );
	szData[8] = ucBitDepth;
	szData[9] = ucColorType;
	szData[10] = ucCompressionMode;
	szData[11] = ucFilterMode;
	szData[12] = ucInterlaceMethod;

	this->SetChunkData( szData, 13 );
}
