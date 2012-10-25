/*
 *------------------------------------------------------------------------
 * Copyright (c) 2000, Vladimir N. Bichigov.
 *------------------------------------------------------------------------
 * vbcfg.h
 *------------------------------------------------------------------------
 * Configuration of blib functions.
 *------------------------------------------------------------------------
 */

#ifndef __VBCFG_H
#define __VBCFG_H

//#if !defined(__WIN32__) && !defined(WIN32)
//	#include <unistd.h>
//#endif

#include <limits.h>

/*
Make your choice from list below

#define PIXEL_UCHAR
#define PIXEL_SHORT
#define PIXEL_USHORT
#define PIXEL_INT
#define PIXEL_DOUBLE
*/

#define PIXEL_UCHAR

/*
Uncomment choices below if image is looped, i.e. first row/colon adjoins to last row/colon.

#define IMG_ROW_LOOPED
#define IMG_COL_LOOPED
*/

#ifdef PIXEL_UCHAR
typedef unsigned char PIX;
typedef int BIN;
#define PIX_MIN 0
#define PIX_MAX UCHAR_MAX
#define PIX_MID (UCHAR_MAX/2)
#endif

#ifdef PIXEL_USHORT
typedef unsigned short PIX;
typedef int BIN;
#define PIX_MIN 0
#define PIX_MAX USHRT_MAX
#define PIX_MID (USHRT_MAX/2)
#endif

#ifdef PIXEL_SHORT
typedef signed short PIX;
typedef int BIN;
#define PIX_MIN SHRT_MIN
#define PIX_MAX SHRT_MAX
#define PIX_MID 0
#endif

#ifdef PIXEL_INT
typedef int PIX;
typedef int BIN;
#define PIX_MIN INT_MIN
#define PIX_MAX INT_MAX
#define PIX_MID 0
#endif

#ifdef PIXEL_DOUBLE
typedef double PIX;
typedef double BIN;
#define PIX_MID 0.0
#endif

#ifdef PIXEL_DOUBLE
#define CPIX(t) t
#else
#define CPIX(t) (PIX)(t+0.5)
#endif

#ifdef IMG_ROW_LOOPED
#define LROW(t,n) if (t < 0) t += n;
#define HROW(t,n) if (t >= n) t -= n;
#else
#define LROW(t,n) if (t < 0) t = 0;
#define HROW(t,n) if (t >= n) t = n-1;
#endif

#ifdef IMG_COL_LOOPED
#define LCOL(t,n) if (t < 0) t += n;
#define RCOL(t,n) if (t >= n) t -= n;
#else
#define LCOL(t,n) if (t < 0) t = 0;
#define RCOL(t,n) if (t >= n) t = n-1;
#endif

#endif /* __VBCFG_H */










