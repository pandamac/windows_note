// CalcCRC32.h: interface for the CCalcCRC32 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALCCRC32_H__6E2B641B_87E7_4105_A53E_DBE012C70180__INCLUDED_)
#define AFX_CALCCRC32_H__6E2B641B_87E7_4105_A53E_DBE012C70180__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define DO1(buf) crc = CRC_TABLE[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);

class CCalcCRC32  
{
	static const unsigned long CRC_TABLE[256];
public:
	unsigned long GetCRC32( unsigned char *szBuf, int nLen );
	CCalcCRC32();
	virtual ~CCalcCRC32();

};

//const unsigned long CCalcCRC32::CRC_TABLE[0] = (unsigned long)0x00000000L;
#endif // !defined(AFX_CALCCRC32_H__6E2B641B_87E7_4105_A53E_DBE012C70180__INCLUDED_)
