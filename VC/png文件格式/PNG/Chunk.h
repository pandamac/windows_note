// Chunk.h: interface for the CChunk class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHUNK_H__AB0E83DC_022A_4A9B_BF28_C2B53307F623__INCLUDED_)
#define AFX_CHUNK_H__AB0E83DC_022A_4A9B_BF28_C2B53307F623__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CalcCRC32.h"

class CChunk  
{
	// Will be used to calculate the CRC32 of the chunk
	CCalcCRC32 m_crc;
	// ChunkSize : minimum length = 12 4+4+4
	unsigned long m_dwChunkLength;
	// ChunkType: such as IHDR, IDAT, IEND, pHYs etc.
	unsigned long m_dwChunkType;
	// ChunkData: Data portion of the chunk
	unsigned char *m_szBuffer;
	// CRC of ChunkData
	unsigned long m_dwCRC;	
public:
	// Get Final Chunk information
	long GetChunk( unsigned char *szBuf, long nLen );
	// Get Final chunk length
	long GetChunkLength();
	// Get length of the chunk's data portion
	long GetChunkDataLength();
	// Get the data portion of the Chunk
	long GetChunkData( unsigned char *szBuf, long nLen );
	// Get the chunk type
	unsigned long GetChunkType();
	// Set the chunk type
	void SetChunkType( unsigned long ulType );
	// Set the data portion information
	bool SetChunkData( unsigned char *szBuf, long nLen );
	CChunk();
	virtual ~CChunk();	
protected:
	// put the dword information into the buffer according to the network order
	void PutDWORDintoBufferWithNetworkOrder( unsigned char *szBuf, unsigned long dword );
};

#endif // !defined(AFX_CHUNK_H__AB0E83DC_022A_4A9B_BF28_C2B53307F623__INCLUDED_)
