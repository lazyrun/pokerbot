#include <QtSingleApplication>
#include "Agent.h"

int main(int argc, char *argv[])
{
   QString module;
   if (argc > 1)
   {
      //разобрать ключ и запустить нужный агент
      QString args(argv[1]);
      args.remove('-');
      module = args;
   }

   QtSingleApplication instance("HoldemAgent", argc, argv);

   if (instance.sendMessage(QString("Wake up!%1").arg(module)))
   {
      instance.quit();
      return 1;
   }

   Agent agent;
   agent.load(module);

   instance.initialize();

   QObject::connect(&instance, SIGNAL(messageReceived(const QString &)),
      &agent, SLOT(handleMessage(const QString &)));

   return instance.exec();
}

