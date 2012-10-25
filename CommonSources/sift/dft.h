/*
 *------------------------------------------------------------------------------
 * Copyright (C) 2000, Vladimir N. Bichigov.
 *------------------------------------------------------------------------------
 * dft.h
 *------------------------------------------------------------------------------
 *  This file contains proto's for one-two dimensional Fourier Transform func's.
 *------------------------------------------------------------------------------
 */

#ifndef __DFT_H
#define __DFT_H

#define DIRECT  1
#define INVERSE 0

// To build DLL in Windows define macro MAKE_ENHANCE_DLL
#if defined(__WIN32__) || defined(WIN32)
# if !defined(ENHANCE_IMPEX)
#  if defined(MAKE_ENHANCE_DLL)
#   define ENHANCE_IMPEX  _export
#  else
#   define ENHANCE_IMPEX
#  endif
# endif
#else
# define ENHANCE_IMPEX
#endif

struct complex_def {
  double Re;
  double Im;
};

typedef	struct complex_def complex;


#ifdef __cplusplus
extern "C" {
#endif

void ENHANCE_IMPEX fft(complex*,int,int,int);
int  ENHANCE_IMPEX rfft(complex*,int,int);
int  ENHANCE_IMPEX irfft(complex*,int,int);
void ENHANCE_IMPEX dft1d(int,complex*,complex*);
void ENHANCE_IMPEX idft1d(int,complex*,complex*);
int  ENHANCE_IMPEX dft2d(int,int,complex*,complex*);
int  ENHANCE_IMPEX fft2d(int,int,complex*,int);
int  ENHANCE_IMPEX rfft2d(int,int,complex*);
int  ENHANCE_IMPEX irfft2d(int,int,complex*);
int  ENHANCE_IMPEX idft2d(int,int,complex*,complex*);

#ifdef __cplusplus
}
#endif

#endif /* __DFT_H */
