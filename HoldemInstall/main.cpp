#include "Wizard.h"

int main(int argc, char *argv[])
{
   QApplication a(argc, argv);

   Wizard wiz;
   wiz.show();
   
   a.setQuitOnLastWindowClosed(true);
   return a.exec();
}

