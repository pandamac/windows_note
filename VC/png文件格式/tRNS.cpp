// tRNS.cpp: implementation of the CtRNS class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tRNS.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CtRNSChunk::CtRNSChunk()
{
	this->SetChunkType( 0x74524e53l);
}

CtRNSChunk::~CtRNSChunk()
{

}

void CtRNSChunk::SetTransIndex(int nIndex)
{
	if ( nIndex < 0 || nIndex >= this->GetChunkDataLength() )
		return;


	for ( int i=0; i<this->GetChunkDataLength(); ++i )
	{
		if( this->m_szBuffer[i] != 0xff )
		{
			m_szBuffer[i] = 0xff;
		}
	}

	this->m_szBuffer[nIndex] = 0;
}

int CtRNSChunk::GetTransIndex( int nIndex )
{
	if ( nIndex < 0 || nIndex >= this->GetChunkDataLength() )
		return -1;

	return this->m_szBuffer[nIndex];
}

int CtRNSChunk::GetTransIndexNum()
{
	return this->GetChunkDataLength();
}
