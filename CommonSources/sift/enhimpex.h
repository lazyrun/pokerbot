//  enhimpex.h

#ifndef enhimpex_h
#define enhimpex_h

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

#endif

