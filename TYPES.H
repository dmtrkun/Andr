/*
 *  Copyright Satec Ltd 2001. All rights reserved.
 *
 *  Common types definitions and macros
 *
 *  Revision history:
 */

#ifndef _TYPES_H_
#define TYPES_H_

#ifndef FALSE
#define FALSE	0
#endif

#ifndef TRUE
#define TRUE	1
#endif

#ifndef false
#define false 	FALSE
#endif

#ifndef true
#define true 		TRUE
#endif

//typedef __int64 llong;
//typedef unsigned __int64 ullong;

/*
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
*/

#define uchar unsigned char
#define ushort unsigned short
#define uint unsigned int


//typedef unsigned char BYTE;
//typedef unsigned short WORD;
//typedef unsigned long DWORD;

#if 1
#ifndef BYTE
#define BYTE unsigned char
#endif
#ifndef WORD
#define WORD unsigned short
#endif
#ifndef DWORD
#define DWORD unsigned long
#endif
#endif

#ifndef bool
#define bool unsigned char
#endif

#define MAXBYTE    0xff
#define MAXWORD    0xffff
#define MAXDWORD   0xffffffff
#define MAXSHORT	 0x7f
#define MAXINT     0x7fff
#define MAXLONG    0x7fffffff

#define PI         3.141592653589793

#define CRITICAL_ERROR  (g_dwmDiagStatus & (1 << DIAG_CRIT_ERROR))

#define LOBYTE(w)       ((uchar)(w))
#define HIBYTE(w)   		((uchar) (((ushort) (w) >> 8) & 0xFF))

#define STORESTRUCT(type,num) struct {type Setup[num]; short cchk;}

#endif


