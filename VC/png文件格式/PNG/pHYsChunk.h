// pHYsChunk.h: interface for the CpHYsChunk class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PHYSCHUNK_H__6F1D6EDA_59A5_447D_A719_C94AC09986CC__INCLUDED_)
#define AFX_PHYSCHUNK_H__6F1D6EDA_59A5_447D_A719_C94AC09986CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Chunk.h"

class CpHYsChunk : public CChunk  
{
	static const unsigned char m_szData[9];
public:
	CpHYsChunk();
	virtual ~CpHYsChunk();

};

#endif // !defined(AFX_PHYSCHUNK_H__6F1D6EDA_59A5_447D_A719_C94AC09986CC__INCLUDED_)
