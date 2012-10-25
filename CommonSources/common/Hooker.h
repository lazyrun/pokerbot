#ifndef HOOKER_H
#define HOOKER_H

#include "windows.h"

//
class Hooker
{
public:
   enum HookType {Mouse, Keyboard};
   Hooker(HookType);
   ~Hooker();
protected:
   HHOOK g_hhook;
};

#endif
