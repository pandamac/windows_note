// tRNS.h: interface for the CtRNS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRNS_H__EC643192_B91B_48B8_B90B_AD88A85C1CE7__INCLUDED_)
#define AFX_TRNS_H__EC643192_B91B_48B8_B90B_AD88A85C1CE7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Chunk.h"

class CtRNSChunk : public CChunk  
{	
public:
	int GetTransIndexNum();
	int GetTransIndex( int nIndex );
	void SetTransIndex( int nIndex );
	CtRNSChunk();
	virtual ~CtRNSChunk();

};

#endif // !defined(AFX_TRNS_H__EC643192_B91B_48B8_B90B_AD88A85C1CE7__INCLUDED_)
