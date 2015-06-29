// RCdemo.cpp: implementation of the CRCdemo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RCdemo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRCdemo::CRCdemo()
{

}

CRCdemo::~CRCdemo()
{

}

void CRCdemo::Init_CRC32_Table() 
{// Call this function only once to initialize the CRC table. 

      // This is the official polynomial used by CRC-32 
      // in PKZip, WinZip and Ethernet. 
      ULONG ulPolynomial = 0x04c11db7; 

      // 256 values representing ASCII character codes. 
      for(int i = 0; i <= 0xFF; i++) 
      { 
            crc32_table[i]=Reflect(i, 8) << 24; 
            for (int j = 0; j < 8; j++) 
                  crc32_table[i] = (crc32_table[i] << 1) ^ (crc32_table[i] & (1 << 31) ? ulPolynomial : 0); 
            crc32_table[i] = Reflect(crc32_table[i], 32); 
      } 
} 

ULONG CRCdemo::Reflect(ULONG ref, char ch) 
{// Used only by Init_CRC32_Table(). 

      ULONG value(0); 

      // Swap bit 0 for bit 7 
      // bit 1 for bit 6, etc. 
      for(int i = 1; i < (ch + 1); i++) 
      { 
            if(ref & 1) 
                  value |= 1 << (ch - i); 
            ref >>= 1; 
      } 
      return value; 
} 

int CRCdemo::Get_CRC(CString& text) 
{ // Pass a text string to this function and it will return the CRC.

      // Once the lookup table has been filled in by the two functions above, 
      // this function creates all CRCs using only the lookup table. 
      // Note that CString is an MFC class. 
      // If you don't have MFC, use the function below instead. 

      // Be sure to use unsigned variables, 
      // because negative values introduce high bits 
      // where zero bits are required. 

      // Start out with all bits set high. 
      ULONG  ulCRC(0xffffffff); 
      int len; 
      unsigned char* buffer; 

      // Get the length. 
      len = text.GetLength(); 
      // Save the text in the buffer. 
      buffer = (unsigned char*)(LPCTSTR)text; 
      // Perform the algorithm on each character 
      // in the string, using the lookup table values. 
      while(len--) 
            ulCRC = (ulCRC >> 8) ^ crc32_table[(ulCRC & 0xFF) ^ *buffer++]; 
      // Exclusive OR the result with the beginning value. 
      return ulCRC ^ 0xffffffff; 
} 



int CRCdemo::Get_CRC(char* text) 
{// Pass a text string to this function and it will return the CRC. 

      // Once the lookup table has been filled in by the two functions above, 
      // this function creates all CRCs using only the lookup table. 

      // Be sure to use unsigned variables, 
      // because negative values introduce high bits 
      // where zero bits are required. 

      // Start out with all bits set high. 
      ULONG  ulCRC(0xffffffff); 
      int len; 
      unsigned char* buffer; 

      // Get the length. 
      len = strlen(text); 
      // Save the text in the buffer. 
      buffer = (unsigned char*)text; 
      // Perform the algorithm on each character 
      // in the string, using the lookup table values. 
      while(len--) 
            ulCRC = (ulCRC >> 8) ^ crc32_table[(ulCRC & 0xFF) ^ *buffer++]; 
      // Exclusive OR the result with the beginning value. 
      return ulCRC ^ 0xffffffff; 
} 
  
