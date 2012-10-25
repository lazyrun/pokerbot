#include "acadlib.h"
#include "Executor.h"

static Executor * s_ex;

EXEC_IMPEX void ExecStart(showMessageCBFunc fun)
{
   s_ex = new Executor(fun);
   s_ex->start();
}

EXEC_IMPEX void ExecStop()
{
   s_ex->stop();
}

EXEC_IMPEX void ExecKill()
{
   s_ex->stop();
   delete s_ex;
}

EXEC_IMPEX void ExecName(char * buf, int sz)
{
   if (sz > 99)
      strcpy(buf, "Poker Academy");
}

