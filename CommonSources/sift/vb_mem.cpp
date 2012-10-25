/*
 * OS dependent functions for WSQ library.
 * This file contains UNIX version.
 */
#include <stdlib.h>
#include <string.h>

#include "vb_mem.h"

/*----------------------------------------------------------------------------*/
void * vb_calloc(size_t n, size_t size)
{
  return((void*)calloc(n,size));
}
/*----------------------------------------------------------------------------*/
void * vb_malloc(size_t size)
{
  return((void*)malloc(size));
}
/*----------------------------------------------------------------------------*/
void vb_free(void *p)
{
  free(p);
}
/*----------------------------------------------------------------------------*/
void * vb_memmove(void *s1, void *s2, size_t n)
{
  return((void*)memmove(s1,s2,n));
}
/*----------------------------------------------------------------------------*/
void * vb_memset(void *s1, int c, size_t n)
{
  return((void*)memset(s1,c,n));
}
/*----------------------------------------------------------------------------*/
