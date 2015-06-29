// IDATChunk.cpp: implementation of the CIDATChunk class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <windows.h>
#include <math.h>
#include "IDATChunk.h"
#include "zlib.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIDATChunk::CIDATChunk()
{
	this->SetChunkType( 0x49444154l);
}

CIDATChunk::~CIDATChunk()
{

}

bool CIDATChunk::SetImageData(unsigned char *szBuf, long nLen)
{
	long nDstLen = (long)(nLen*1.2+12);
	unsigned char *szNew = new unsigned char[nDstLen];
	if (szNew == NULL )
		return false;

	int nRet = compress( szNew, (unsigned long*)&nDstLen, szBuf, nLen );
	if ( nRet == 0 )
	{
		this->SetChunkData( szNew, nDstLen );
	}

	delete []szNew;

	if ( nRet == 0 )
		return true;
	else 
		return false;
}

bool CIDATChunk::SetRawBMPData(long lWidth, long lHeight, int nBitDepth, unsigned char *szData, int nDstGreyBitDepth)
{// This function will calculate the bitdepth of the source data and format it to the right format
 // the supported nDstGreyBitDepth = 1, 2, 4
	if ( nDstGreyBitDepth != 1
		&& nDstGreyBitDepth != 2
		&& nDstGreyBitDepth != 4 )
		return false;

	int nRealWidth;
	int nMask;
	switch( nDstGreyBitDepth)
	{
	case 1:
		nMask=1;
		nRealWidth = (lWidth +7)/8+1; // +1 for Filter byte
		break;
	case 2:
		nMask=3;
		nRealWidth = (lWidth +3)/4+1; // +1 for Filter byte
		break;
	case 4:
		nMask=0xf;
		nRealWidth = (lWidth +1)/2+1; // +1 for Filter byte
		break;
	default:
		return false;
	}

	unsigned char *pFinalData = new unsigned char[nRealWidth*lHeight];
	if ( pFinalData == NULL )
	{
		return false;
	}

	memset( pFinalData, 0, nRealWidth*lHeight);
	

	unsigned char byGray;

	for ( int i=0; i<lHeight; i++ )
	{
		for( int j=0; j<lWidth; j++)
		{
			byGray = szData[i*lWidth+j];
			switch( nDstGreyBitDepth )
			{
			case 1:
				byGray>>=7;
				pFinalData[1+nRealWidth*i+j/8] |= byGray<<(7-j%8);
				//paint.picture[ (i*130+j)/4 ] |= (byGray << ( 2* (3-(i*130 +j )%4 ) ));
				break;
			case 2:
				byGray>>=6;
				pFinalData[1+nRealWidth*i+j/4] |= byGray<<((3-j%4)<<1);
				break;
			case 4:
				byGray>>=4;
				pFinalData[1+nRealWidth*i+j/2] |= byGray<<((1-j%2)<<2);
				break;
			}
		}
	}

	bool nRet = this->SetImageData( pFinalData, nRealWidth*lHeight);
	delete []pFinalData;

	return nRet;
}

bool CIDATChunk::SetBitmap( char *szFilename, int nDstBitDepth )
{
	FILE *fp;
	unsigned long dwLength;
	if ( (fp = fopen( szFilename, "rb" ) ) == NULL )
	{
		return false;
	}

	if ( fseek( fp, 0, SEEK_SET ) != 0 )
	{
		fclose( fp );
		return false;
	}

	if ( fseek( fp, 0, SEEK_END ) != 0 )
	{
		fclose( fp );
		return false;
	}

	dwLength = ftell( fp );

	if ( fseek( fp, 0, SEEK_SET ) != 0 )
	{
		fclose( fp );
		return false;
	}

	unsigned char *pbyBuffer = new unsigned char[ sizeof(BITMAPFILEHEADER) ];
	if( pbyBuffer == NULL || fread( pbyBuffer, sizeof(BITMAPFILEHEADER), 1, fp ) != 1 )
	{
		if ( pbyBuffer != NULL )
			delete []pbyBuffer;
		fclose( fp );
		return false;
	}

	BITMAPFILEHEADER *pFileHead  = (BITMAPFILEHEADER *)pbyBuffer;
	if ( dwLength != pFileHead->bfSize )
	{
		delete []pbyBuffer;
		fclose( fp );
		return false;
	}

	if ( pFileHead->bfType != 0x4d42 )
	{
		delete []pbyBuffer;
		fclose( fp );
		return false;
	}
		
	unsigned long dwHeadSize = pFileHead->bfOffBits - sizeof(BITMAPFILEHEADER);
	unsigned long dwDataSize = dwLength - pFileHead->bfOffBits;

	delete []pbyBuffer;

	pbyBuffer = new unsigned char[dwHeadSize];
	if( pbyBuffer == NULL || fread( pbyBuffer, dwHeadSize, 1, fp ) != 1 )
	{
		if ( pbyBuffer != NULL )
			delete []pbyBuffer;		
		fclose( fp );
		return false;
	}
	

	unsigned char *pbyData = new unsigned char[ dwDataSize ];	
	if( pbyData == NULL || fread( pbyData, dwDataSize, 1, fp ) != 1 )
	{
		if ( pbyData != NULL )
			delete []pbyData;
		delete []pbyBuffer;				
		fclose( fp );
		return false;
	}

	fclose( fp );
	BITMAPINFO *pBmp = (BITMAPINFO *)pbyBuffer;	
	
	bool bRet = SetBitmap( pBmp, pbyData, nDstBitDepth );
	
	delete []pbyData;
	delete []pbyBuffer;		
	return bRet;
}

bool CIDATChunk::SetBitmap( BITMAPINFO *pBmp, unsigned char *pData, int nDstBitDepth)
{
	int nRealWidth = pBmp->bmiHeader.biWidth;
	m_ulWidth = nRealWidth;
	int nRealHeight = abs(pBmp->bmiHeader.biHeight);
	m_ulHeight = nRealHeight;

	int nLineWidth = ((pBmp->bmiHeader.biWidth * pBmp->bmiHeader.biBitCount / 8 + 3 )/4) * 4;		
	
	if ( pBmp->bmiHeader.biCompression != BI_RGB
		&& pBmp->bmiHeader.biCompression != BI_BITFIELDS )
		
	{
		return false;
	}

	unsigned char *pbyBuffer = new unsigned char[nLineWidth];

	if ( pbyBuffer == NULL )
		return false;
	// Added by Simon Li, 04/12/2001 Store the gray scale data of the whole picture 
	unsigned char *pGrayData = new unsigned char [nRealWidth*nRealHeight];

	if ( pGrayData == NULL )
	{
		delete[]pbyBuffer;
		return false;
	}

	int nMask[3] = {0,0,0};
	if ( pBmp->bmiHeader.biCompression == BI_BITFIELDS )
	{
		for ( int i=0; i<3; ++i)
		{
			unsigned long *pMask = (unsigned long *)pBmp->bmiColors;
			unsigned long dwTest = 1;
			while ( (dwTest & pMask[i]) == 0 )
			{
				nMask[i] ++;
				dwTest <<= 1;
			}

			while ( (dwTest & pMask[i]) != 0 )
			{
				nMask[i] ++;
				dwTest <<= 1;
				if ( nMask[i] == 16 && pBmp->bmiHeader.biBitCount == 16)
					break;
				else if ( nMask[i] == 32 && pBmp->bmiHeader.biBitCount == 32)
					break;
			}

			if ( nMask[i] <= 8 )
			{
				nMask[i] = nMask[i] - 8;
			}
			else if ( nMask[i] <= 16 )
			{
				nMask[i] = nMask[i]-8;
			}
			else if ( nMask[i] <= 24 )
			{
				nMask[i] = nMask[i] - 16;
			}
			else
			{
				nMask[i] = nMask[i] - 24;
			}
		}				

	}

	for ( int i=0; i<nRealHeight; ++i )
	{
		if ( pBmp->bmiHeader.biHeight < 0 )
		{
			memcpy( pbyBuffer, pData + i*nLineWidth, nLineWidth );
		}
		else
		{
			memcpy( pbyBuffer, pData + (pBmp->bmiHeader.biHeight - i - 1 )*nLineWidth, nLineWidth );
		}

		for ( int j=0; j<nRealWidth; ++j )
		{
			RGBQUAD rgb;
			unsigned short wData;
			switch( pBmp->bmiHeader.biBitCount )
			{
			case 1:
				if( pbyBuffer[j/8] & (1<<(7-j%8)) )
				{
					rgb.rgbBlue = pBmp->bmiColors[1].rgbBlue;
					rgb.rgbGreen = pBmp->bmiColors[1].rgbGreen;
					rgb.rgbRed = pBmp->bmiColors[1].rgbRed;
					rgb.rgbReserved = pBmp->bmiColors[1].rgbReserved;
				}
				else
				{
					rgb.rgbBlue = pBmp->bmiColors[0].rgbBlue;
					rgb.rgbGreen = pBmp->bmiColors[0].rgbGreen;
					rgb.rgbRed = pBmp->bmiColors[0].rgbRed;
					rgb.rgbReserved = pBmp->bmiColors[0].rgbReserved;
				}
				break;
			case 4:
				rgb.rgbBlue = pBmp->bmiColors[((pbyBuffer[j/2])>> (4-4*(j%2))) & 0xf].rgbBlue;
				rgb.rgbGreen = pBmp->bmiColors[((pbyBuffer[j/2])>> (4-4*(j%2))) & 0xf].rgbGreen;
				rgb.rgbRed = pBmp->bmiColors[((pbyBuffer[j/2])>> (4-4*(j%2))) & 0xf].rgbRed;
				rgb.rgbReserved = pBmp->bmiColors[((pbyBuffer[j/2])>> (4-4*(j%2))) & 0xf].rgbReserved;
				break;
			case 8:
				rgb.rgbBlue = pBmp->bmiColors[pbyBuffer[j]].rgbBlue;
				rgb.rgbGreen = pBmp->bmiColors[pbyBuffer[j]].rgbGreen;
				rgb.rgbRed = pBmp->bmiColors[pbyBuffer[j]].rgbRed;
				rgb.rgbReserved = pBmp->bmiColors[pbyBuffer[j]].rgbReserved;
				break;
			case 16:
				wData = *(unsigned short *)(pbyBuffer + 2*j);
				if ( pBmp->bmiHeader.biCompression != BI_BITFIELDS )
				{
					rgb.rgbBlue = ( wData & 0x1f ) << 3;
					rgb.rgbGreen = ( wData & 0x3e0 ) >> 2;
					rgb.rgbRed = ( wData & 0x7c00 ) >> 7;
					rgb.rgbReserved = 0;
				}
				else
				{
					unsigned long *pMask = (unsigned long *)pBmp->bmiColors;
					if ( nMask[2] > 0 )
						rgb.rgbBlue = (unsigned char)(( wData & pMask[2] ) >> nMask[2]);
					else
						rgb.rgbBlue = (unsigned char)(( wData & pMask[2] ) << nMask[2]);

					if ( nMask[1] > 0 )
						rgb.rgbGreen = (unsigned char)(( wData & pMask[1] ) >> nMask[1]);
					else
						rgb.rgbGreen = (unsigned char)(( wData & pMask[1] ) << nMask[1]);

					if ( nMask[0] > 0)
						rgb.rgbRed = (unsigned char)(( wData & pMask[0] ) >> nMask[0]);
					else
						rgb.rgbRed = (unsigned char)(( wData & pMask[0] ) << nMask[0]);

					rgb.rgbReserved = 0;
				}
				break;
			case 24:
				rgb.rgbBlue = *(pbyBuffer + 3*j);
				rgb.rgbGreen = *(pbyBuffer + 3*j + 1);
				rgb.rgbRed = *(pbyBuffer + 3*j + 2);
				rgb.rgbReserved = 0;
				break;
			case 32:
				if ( pBmp->bmiHeader.biCompression != BI_BITFIELDS )
				{
					rgb.rgbBlue = *(pbyBuffer + 4*j);
					rgb.rgbGreen = *(pbyBuffer + 4*j + 1);
					rgb.rgbRed = *(pbyBuffer + 4*j + 2);
					rgb.rgbReserved = 0;
				}
				else
				{
					unsigned long *pData = (unsigned long *)pbyBuffer;
					unsigned long dwData = pData[j];
					unsigned long *pMask = (unsigned long *)pBmp->bmiColors;

					if ( nMask[2] > 0 )
						rgb.rgbBlue = (unsigned char)( ( dwData & pMask[2] ) >> nMask[2] );
					else
						rgb.rgbBlue = (unsigned char)( ( dwData & pMask[2] ) << nMask[2] );

					if ( nMask[1] > 0 )
						rgb.rgbGreen = (unsigned char)( ( dwData & pMask[1] ) >> nMask[1] );
					else
						rgb.rgbGreen = (unsigned char)( ( dwData & pMask[1] ) << nMask[1] );

					if ( nMask[0] > 0 )
						rgb.rgbRed = (unsigned char)(( dwData & pMask[0] ) >> nMask[0] );
					else
						rgb.rgbRed = (unsigned char)(( dwData & pMask[0] ) << nMask[0] );

					rgb.rgbReserved = 0;
				}
				break;
			default:
				delete []pbyBuffer;
				delete []pGrayData;
				return false;
			}
			
			unsigned char byGray = (unsigned char)(rgb.rgbBlue * 0.11 + rgb.rgbGreen * 0.59 + rgb.rgbRed * 0.3);

	//		byGray = ((~byGray)>>6) & 0x3;
			pGrayData[i*nRealWidth + j] = byGray;
//			paint.picture[ (i*130+j)/4 ] |= (byGray << ( 2* (3-(i*130 +j )%4 ) ));
		}
	}

	this->SetRawBMPData( nRealWidth, nRealHeight, 8, pGrayData, nDstBitDepth );
	delete []pbyBuffer;
	delete []pGrayData;
	return true;
}


bool CIDATChunk::SetBitmap( int nWidth, int nHeight, int nPadBytes, unsigned char *pData, int nDstBitDepth, BOOL bDither)
{
	BYTE byDither[][4] = {	
							{0x00, 0x00, 0x00, 0x00 },
							{0x55, 0x00, 0x00, 0x00 },
							{0x55, 0x00, 0x55, 0x00 },
							{0x55, 0x55, 0x55, 0x00 },
							{0x55, 0x55, 0x55, 0x55 },
							{0xaa, 0x55, 0x55, 0x55 },
							{0xaa, 0x55, 0xaa, 0x55 },
							{0xaa, 0xaa, 0xaa, 0x55 },
							{0xaa, 0xaa, 0xaa, 0xaa },
							{0xff, 0xaa, 0xaa, 0xaa },
							{0xff, 0xaa, 0xff, 0xaa },
							{0xff, 0xff, 0xff, 0xaa },
							{0xff, 0xff, 0xff, 0xff }							
						 };
						

	int nRealWidth = nWidth;
	m_ulWidth = nRealWidth;
	int nRealHeight = nHeight;
	m_ulHeight = nRealHeight;

	int nLineWidth = ((nWidth + 3 )/4) * 4 * 3;		
	
	
	unsigned char *pbyBuffer = new unsigned char[nLineWidth];

	if ( pbyBuffer == NULL )
		return false;

	// Added by Simon Li, 04/12/2001 Store the gray scale data of the whole picture 
	unsigned char *pGrayData;
	if ( bDither )
	{
		pGrayData = new unsigned char [nRealWidth*nRealHeight*4];
	}
	else
	{
		pGrayData = new unsigned char [nRealWidth*nRealHeight];
	}


	if ( pGrayData == NULL )
	{
		delete[]pbyBuffer;
		return false;
	}

	RGBQUAD rgb;
	unsigned char byGray;
	for ( int i=0; i<nRealHeight; ++i )
	{
		if ( nHeight > 0 )
		{
			memcpy( pbyBuffer, pData + i*nRealWidth*3 + i*(nPadBytes) , nLineWidth );		
		}
		else
		{
			memcpy( pbyBuffer, pData + (nHeight - i - 1 )*nRealWidth*3 + (nHeight - i - 1 )*nPadBytes, nLineWidth );		
		}

		for ( int j=0; j<nRealWidth; ++j )
		{					
			rgb.rgbBlue = *(pbyBuffer + 3*j);
			rgb.rgbGreen = *(pbyBuffer + 3*j + 1);
			rgb.rgbRed = *(pbyBuffer + 3*j + 2);
			rgb.rgbReserved = 0;
			
			byGray = (unsigned char)(rgb.rgbBlue * 0.11 + rgb.rgbGreen * 0.59 + rgb.rgbRed * 0.3);

			if ( !bDither )
			{
				pGrayData[i*nRealWidth + j] = byGray;
			}
			else
			{			
				pGrayData[i*2*nRealWidth*2 + 2*j] = byDither[byGray/20][0];
				pGrayData[i*2*nRealWidth*2 + 2*j+1] = byDither[byGray/20][1];
				pGrayData[i*2*nRealWidth*2 + nRealWidth*2+ 2*j] = byDither[byGray/20][2];
				pGrayData[i*2*nRealWidth*2 + nRealWidth*2+ 2*j+1] = byDither[byGray/20][3];				
			}
		}
	}

	if ( bDither )
	{
		m_ulWidth *= 2;
		m_ulHeight *= 2;

		this->SetRawBMPData( nRealWidth*2, nRealHeight*2, 8, pGrayData, nDstBitDepth );
	}
	else
	{
		this->SetRawBMPData( nRealWidth, nRealHeight, 8, pGrayData, nDstBitDepth );
	}
	delete []pbyBuffer;
	delete []pGrayData;
	return true;
}


