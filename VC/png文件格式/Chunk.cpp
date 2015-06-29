// Chunk.cpp: implementation of the CChunk class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Chunk.h"
#include <memory.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CChunk::CChunk()
{
	m_dwChunkType = 0L;
	m_dwChunkLength = 12L; // 4+4+4   Length field(4) + Type field(4) + CRC Field(4)
	m_szBuffer = NULL;
}

CChunk::~CChunk()
{
	if( m_szBuffer != NULL )
	{
		delete []m_szBuffer;
	}
}

bool CChunk::SetChunkData(unsigned char *szBuf, long nLen)
{
	if( m_szBuffer != NULL )
	{
		m_dwChunkLength = 12; // 4+4+4
		delete []m_szBuffer;
	}

	m_szBuffer = new unsigned char [nLen];
	if ( m_szBuffer == NULL )
		return false;

	memcpy( m_szBuffer, szBuf, nLen );

	m_dwChunkLength += nLen;
	return true;
}

long CChunk::GetChunkData(unsigned char *szBuf, long nLen)
{
	long nCount = m_dwChunkLength-12;
	if ( nLen == 0 )
	{// nLen == 0 return needed buffer size
		return nCount;
	}

	if( szBuf == NULL )
		return 0;

	
	if ( nLen < nCount )
		return 0;
	else
	{
		memcpy( szBuf, m_szBuffer, nCount );
		return nCount;
	}
}

void CChunk::SetChunkType( unsigned long ulType ) 
{ 
	m_dwChunkType = ulType; 
}


unsigned long CChunk::GetChunkType()
{
	return m_dwChunkType;
}


long CChunk::GetChunkDataLength()
{
	return m_dwChunkLength - 12;
}


long CChunk::GetChunkLength()
{
	return m_dwChunkLength;
}


long CChunk::GetChunk(unsigned char *szBuf, long nLen)
{
	if ( nLen == 0 )
	{// nLen == 0 return needed buffer size
		return m_dwChunkLength;
	}

	if( szBuf == NULL )
		return 0;

	
	if ( nLen < (long)m_dwChunkLength )
		return 0;
	else
	{
		PutDWORDintoBufferWithNetworkOrder( szBuf, m_dwChunkLength-12 );
		PutDWORDintoBufferWithNetworkOrder( szBuf+4, m_dwChunkType );
		memcpy( szBuf+8, m_szBuffer, m_dwChunkLength-12 );
		PutDWORDintoBufferWithNetworkOrder( szBuf+m_dwChunkLength-4, m_crc.GetCRC32( szBuf+4, m_dwChunkLength-8) ); 
		return m_dwChunkLength;
	}	
}

void CChunk::PutDWORDintoBufferWithNetworkOrder(unsigned char *szBuf, unsigned long dword)
{
	szBuf[0] = (unsigned char)(dword >> 24);
	szBuf[1] = (unsigned char)(dword >> 16);
	szBuf[2] = (unsigned char)(dword >> 8);
	szBuf[3] = (unsigned char)dword ;
}


int CChunk::GetBufferintoDWORDWithNetworkOrder(unsigned char *szBuf)
{
	int i;
	i = szBuf[0];
	i = i<<8;
	i |= szBuf[1];
	i = i<<8;
	i |= szBuf[2];
	i = i<<8;
	i |= szBuf[3];

	return i;
}
