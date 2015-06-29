// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__D272B7A0_42FC_4577_B0C4_91366446490B__INCLUDED_)
#define AFX_STDAFX_H__D272B7A0_42FC_4577_B0C4_91366446490B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <stdio.h>
#include "calcCRC32.h"
#include "chunk.h"
#include "ihdrchunk.h"
#include "iendchunk.h"
#include "pHYsChunk.h"
#include "IDATChunk.h"
#include "pngfile.h"
#include "zlib.h"
// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__D272B7A0_42FC_4577_B0C4_91366446490B__INCLUDED_)
