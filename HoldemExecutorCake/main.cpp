#include <QApplication>
#include "Executor.h"

#ifdef RESEARCH
#include "ResearchWindow.h"
#endif

int main(int argc, char *argv[])
{
   QApplication a(argc, argv);
   
#ifdef RESEARCH
   ResearchWindow rw;
   rw.show();
#else
   Executor ex;
   ex.start();
#endif
   return a.exec();
}

