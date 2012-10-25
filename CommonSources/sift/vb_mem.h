/*
 *------------------------------------------------------------------------
 * Copyright (c) 2000, Vladimir N. Bichigov.
 *------------------------------------------------------------------------
 * vb_mem.h
 *------------------------------------------------------------------------
 * System dependent functions.
 *------------------------------------------------------------------------
 */

#ifndef __VB_MEM_H
#define __VB_MEM_H

// To build DLL in Windows define macro MAKE_ENHANCE_DLL
#if defined(__WIN32__) || defined(WIN32)
# if !defined(ENHANCE_IMPEX)
#  if defined(MAKE_ENHANCE_DLL)
#   define ENHANCE_IMPEX  __declspec(dllexport)
#  else
#   define ENHANCE_IMPEX
#  endif
# endif
#else
# define ENHANCE_IMPEX
#endif

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

ENHANCE_IMPEX void * vb_calloc(size_t nitems, size_t size);
ENHANCE_IMPEX void * vb_malloc(size_t);
ENHANCE_IMPEX void   vb_free(void*);
ENHANCE_IMPEX void * vb_memmove(void*,void*,size_t);
ENHANCE_IMPEX void * vb_memset(void*,int,size_t);

#ifdef __cplusplus
}
#endif

#endif /* __VB_MEM_H */
