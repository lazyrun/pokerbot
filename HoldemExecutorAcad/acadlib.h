#ifndef ACADLIB_H
#define ACADLIB_H
#include <windows.h>

#ifdef __cplusplus

extern "C"
{
#endif
#ifndef EXEC_IMPEX
   #ifdef MAKE_EXEC_DLL
      #define EXEC_IMPEX   __declspec( dllexport )
   #elif USE_EXEC_DLL
      #define EXEC_IMPEX   __declspec( dllimport )
   #else
      #define EXEC_IMPEX
   #endif
#endif

typedef void (CALLBACK * showMessageCBFunc)(const char * hand);

EXEC_IMPEX void ExecStart(showMessageCBFunc fun);
EXEC_IMPEX void ExecStop();
EXEC_IMPEX void ExecKill();
EXEC_IMPEX void ExecName(char * buf, int sz);

#ifdef __cplusplus
}
#endif
#endif
