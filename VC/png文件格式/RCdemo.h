// RCdemo.h: interface for the CRCdemo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RCDEMO_H__3D79811C_006A_4DEB_9BA1_77C3FFB3B7ED__INCLUDED_)
#define AFX_RCDEMO_H__3D79811C_006A_4DEB_9BA1_77C3FFB3B7ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRCdemo  
{
public:
	CRCdemo();
	virtual ~CRCdemo();

protected: 
      ULONG crc32_table[256];  // Lookup table array 
      void Init_CRC32_Table();  // Builds lookup table array 
      ULONG Reflect(ULONG ref, char ch);  // Reflects CRC bits in the lookup table 
      int Get_CRC(CString& text);  // Creates a CRC from a text string 

};

#endif // !defined(AFX_RCDEMO_H__3D79811C_006A_4DEB_9BA1_77C3FFB3B7ED__INCLUDED_)
