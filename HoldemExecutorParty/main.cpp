#include <QApplication>
#include "Executor.h"

int main(int argc, char *argv[])
{
   QApplication a(argc, argv);
   
   Executor ex;
   ex.start();
   return a.exec();
}

