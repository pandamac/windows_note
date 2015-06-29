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

bool CPNGFile::CreatePNGFromPNG(char *szSrcFile, char *szDstFile, int nDstBitDepth)
{
	if ( !SetPNG( szSrcFile, nDstBitDepth ))
		return false;

	int bufLen = 5*m_idat.GetChunkDataLength();
	BYTE * buf = new BYTE[bufLen]; // assuming 500 compress rate

	if( buf == NULL )
		return false;

	BYTE *bufOrg = new BYTE[m_idat.GetChunkDataLength()];
	
	if( bufOrg == NULL )
		return false;
	
	m_idat.GetChunkData( bufOrg, m_idat.GetChunkDataLength() );
	int nRet = uncompress( buf, (unsigned long*)&bufLen, bufOrg, m_idat.GetChunkDataLength() );

	delete []bufOrg;

	DataConvert( &buf, bufLen, m_ihdr.GetWidth(), m_ihdr.GetColorDepth(), nDstBitDepth);

	m_ihdr.SetColorDepth(nDstBitDepth);

	unsigned long  nLen = (unsigned long)(bufLen * 1.2);

	bufOrg = new BYTE[nLen];
	
	if( bufOrg == NULL )
		return false;

	

	nRet = compress( bufOrg, &nLen, buf, bufLen );
	m_idat.SetChunkData( bufOrg, nLen );


	FILE *fp = fopen( szDstFile, "w+b" );
	if ( fp == NULL )
		return false;

	unsigned char szBuf[0x4000];

	fwrite( m_szHeader, 8, 1, fp );
	nLen=0x4000;
	nLen = m_ihdr.GetChunk( szBuf, nLen);
	fwrite( szBuf, nLen, 1, fp );
	nLen=0x4000;
	nLen = m_plte.GetChunk( szBuf, nLen);
	fwrite( szBuf, nLen, 1, fp );
	nLen=0x4000;
	nLen = m_trns.GetChunk( szBuf, nLen);
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

bool CPNGFile::SetPNG(char *szFilename, int nDstBitDepth)
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

	unsigned char *buf = new unsigned char[8];

	if( buf == NULL || fread( buf, 8, 1, fp) != 1 )
	{
		if ( buf != NULL )
			delete []buf;
		fclose( fp );
	}

	if(!(buf[0] == 0x89 && buf[1] == 0x050 && buf[2] == 0x04e && buf[3] == 0x047
		   && buf[4] == 0x00d && buf[5] == 0x00a && buf[6] == 0x01a && buf[7] == 0x00a ))
	{
		fclose(fp);
		delete []buf;
		return false;
	}		

	bool bEnd = false;
	while( !bEnd )
	{
		if( fread( buf, 8, 1, fp ) != 1 )
		{
			fclose(fp);
			delete []buf;
			return false;
		}

		if( buf[4] == 'I' && buf[5] == 'D' && buf[6] == 'A' && buf[7] == 'T' )
		{
			int nLen = m_idat.GetBufferintoDWORDWithNetworkOrder( buf );

			delete []buf;
			
			buf = new unsigned char[nLen+4];

			if( buf == NULL || fread( buf, nLen+4, 1, fp) != 1 )
			{
				if ( buf != NULL )
					delete []buf;
				fclose( fp );
			}
						
			m_idat.SetChunkData( buf, nLen);

			delete[]buf;
			buf = new unsigned char[8];

			if( buf == NULL )
			{
				fclose(fp);
				return false;
			}			
		}
		else if( buf[4] == 'I' && buf[5] == 'H' && buf[6] == 'D' && buf[7] == 'R' )
		{
			delete []buf;

			buf = new unsigned char[0xd+4];

			if( buf == NULL || fread( buf, 0xd+4, 1, fp) != 1 )
			{
				if ( buf != NULL )
					delete []buf;
				fclose( fp );
			}
			
			int width = m_idat.GetBufferintoDWORDWithNetworkOrder( buf );
			int height = m_idat.GetBufferintoDWORDWithNetworkOrder( buf+4 );
			
			m_ihdr.SetValue( (unsigned long)width, (unsigned long)height, buf[8], buf[9], buf[10], buf[11], buf[12] );

			delete[]buf;
			buf = new unsigned char[8];

			if( buf == NULL )
			{
				fclose(fp);
				return false;
			}
		}
		else if( buf[4] == 'P' && buf[5] == 'L' && buf[6] == 'T' && buf[7] == 'E' )
		{
			int nLen = m_idat.GetBufferintoDWORDWithNetworkOrder( buf );

			delete []buf;
			
			buf = new unsigned char[nLen+4];

			if( buf == NULL || fread( buf, nLen+4, 1, fp) != 1 )
			{
				if ( buf != NULL )
					delete []buf;
				fclose( fp );
			}
						
			m_plte.SetChunkData( buf, nLen);

			delete[]buf;
			buf = new unsigned char[8];

			if( buf == NULL )
			{
				fclose(fp);
				return false;
			}
		}
		else if( buf[4] == 't' && buf[5] == 'R' && buf[6] == 'N' && buf[7] == 'S' )
		{
			int nLen = m_idat.GetBufferintoDWORDWithNetworkOrder( buf );

			delete []buf;
			
			buf = new unsigned char[nLen+4];

			if( buf == NULL || fread( buf, nLen+4, 1, fp) != 1 )
			{
				if ( buf != NULL )
					delete []buf;
				fclose( fp );
			}
						
			m_trns.SetChunkData( buf, nLen);

			delete[]buf;
			buf = new unsigned char[8];

			if( buf == NULL )
			{
				fclose(fp);
				return false;
			}
		}
		else if( buf[4] == 'I' && buf[5] == 'E' && buf[6] == 'N' && buf[7] == 'D' )
		{
			delete []buf;
			fclose(fp);
			bEnd = true;
			continue;
		}
		else
		{
			int nSkip = m_idat.GetBufferintoDWORDWithNetworkOrder( buf );
			fseek( fp, nSkip+4, SEEK_CUR );
			continue;
		}

	}
	
	return bEnd;
}

bool CPNGFile::DataConvert( BYTE **src, int &nLen, int nWidth, int nSrcBit, int nDstBit)
{
	BYTE *sz = *src;
	int nHeight = nLen/((nWidth+1)/2 +1);

	BYTE *buf;
	int nBytesPerLine = (nWidth+1)/2+1;
	if( nSrcBit == 4 && nDstBit == 8 )
	{
		buf= new BYTE[ (nWidth+1) * nHeight ];
	}
	else
		return false;

	if ( NULL == buf )
		return false;

	if( nSrcBit == 4 && nDstBit == 8 )
	{
		for( int i=0; i<nHeight; ++i)
		{
			for( int j=0; j<((nWidth+1)/2); ++ j)
			{
				if( nWidth & 1 )
				{
					buf[i*(nWidth+1)+2*j] = sz[i*nBytesPerLine+j]&0xf;
					buf[i*(nWidth+1)+2*j+1] = sz[i*nBytesPerLine+j+1]>>4;
				}
				else
				{
					buf[i*(nWidth+1)+2*j+1] = sz[i*nBytesPerLine+j+1]>>4;
					buf[i*(nWidth+1)+2*j+1+1] = sz[i*nBytesPerLine+j+1]&0xf;
				}
				//if( j>0 )
				//{
				//	buf[j-1] == (buf[j-1]<<4) | buf[j]>>4;
				//	buf[j] = buf[j]<<4;
				//}
			}
		}
		
		delete []sz;
		*src = buf;
		nLen = (nWidth+1) * nHeight;
		return true;
	}
	else // Only support 4 to 8
		return false;

}
