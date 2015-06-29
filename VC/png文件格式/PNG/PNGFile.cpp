// PNGFile.cpp: implementation of the CPNGFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "PNGFile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const unsigned char CPNGFile::m_szHeader[8] = {
	'\x089','\x050','\x04e','\x047','\x00d','\x00a','\x01a','\x00a' };


CPNGFile::CPNGFile()
{

}

CPNGFile::~CPNGFile()
{

}

bool CPNGFile::CreatePNGFromBMP(char *szSrcFile, char *szDstFile, int nDstBitDepth)
{
	if ( !m_idat.SetBitmap( szSrcFile, nDstBitDepth ))
		return false;

	m_ihdr.SetValue( m_idat.m_ulWidth, m_idat.m_ulHeight, nDstBitDepth, 0, 0, 0, 0 );

	FILE *fp = fopen( szDstFile, "w+b" );
	if ( fp == NULL )
		return false;

	unsigned char szBuf[0x4000];

	fwrite( m_szHeader, 8, 1, fp );
	long nLen=0x4000;
	nLen = m_ihdr.GetChunk( szBuf, nLen);
	fwrite( szBuf, nLen, 1, fp );
	nLen=0x4000;
	nLen = m_phys.GetChunk( szBuf, nLen);
	fwrite( szBuf, nLen, 1, fp );
	nLen=0x4000;
	nLen = m_idat.GetChunk( szBuf, nLen);
	fwrite( szBuf, nLen, 1, fp );
	nLen=0x4000;
	nLen = m_iend.GetChunk( szBuf, nLen);
	fwrite( szBuf, nLen, 1, fp );

	fclose(fp);
	return true;
}


bool CPNGFile::CreatePNGFromRawRGB(int nWidth, int nHeight, int nPadBytes, BYTE *pData, BYTE *pDstData, int &nDstLen, BOOL bDither )
{	
	if ( !m_idat.SetBitmap( nWidth, nHeight, nPadBytes, pData, 2, bDither) )
		return false;

	m_ihdr.SetValue( m_idat.m_ulWidth, m_idat.m_ulHeight, 2, 0, 0, 0, 0 );

	int nLen = 0;
	int nPoint = 0;

	nLen += 8;
	if ( nLen > nDstLen )
	{
		return false;
	}
	 	
	memcpy( pDstData+nPoint, m_szHeader, 8 );
	nPoint+= 8;

	
	nLen += m_ihdr.GetChunkLength();
	if ( nLen > nDstLen )
	{
		return false;
	}

	long nDataLen=nDstLen - nPoint;
	nDataLen = m_ihdr.GetChunk( pDstData+nPoint, nDataLen);
	nPoint += nDataLen;

	nLen += m_phys.GetChunkLength();
	if ( nLen > nDstLen )
	{
		return false;
	}

	nDataLen=nDstLen - nPoint;
	nDataLen = m_phys.GetChunk( pDstData+nPoint, nDataLen);
	nPoint += nDataLen;
	
	nLen += m_idat.GetChunkLength();
	if ( nLen > nDstLen )
	{
		return false;
	}

	nDataLen=nDstLen - nPoint;
	nDataLen = m_idat.GetChunk( pDstData+nPoint, nDataLen);	
	nPoint += nDataLen;
	
	nLen += m_iend.GetChunkLength();
	if ( nLen > nDstLen )
	{
		return false;
	}

	nDataLen=nDstLen - nPoint;
	nDataLen = m_iend.GetChunk( pDstData+nPoint, nDataLen);
	nPoint += nDataLen;

	nDstLen = nPoint;
	return true;
}
