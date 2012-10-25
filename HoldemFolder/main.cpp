#include <QtSingleApplication>

#ifdef QT_NO_DEBUG
#include "ver.h"
#else
#include "ver1.h"
#endif
#include "MainWindow.h"
#include "SettingsData.h"

int main(int argc, char *argv[])
{
   qRegisterMetaType<SettingsData>("SettingsData");
   qRegisterMetaTypeStreamOperators<SettingsData>("SettingsData");

   QtSingleApplication a("Maverick", argc, argv);
   if (a.sendMessage("Wake up!"))
   {
      return 0;
   }
   a.initialize();
   a.addLibraryPath(QString("%1/%2").arg(a.applicationDirPath())
      .arg("plugins"));

   QPixmap pixmap(":/images/splash.png");
   QSplashScreen splash(pixmap);
   splash.show();
   a.processEvents();   
   QDate dateRelease = QDate::fromString(RELEASE_DATE, "yyyy-MM-dd");
   splash.showMessage(QString("Maverick Poker Bot (c) v. %1 by %2 All Right Reserved")
      .arg(VERSION).arg(dateRelease.toString(Qt::SystemLocaleShortDate)),
      Qt::AlignBottom | Qt::AlignRight, Qt::white);

   MainWindow mainWindow(0, Qt::Dialog | Qt::WindowMinimizeButtonHint);
   splash.finish(&mainWindow);
   mainWindow.setSize();
   mainWindow.open();
   
   a.setActivationWindow(&mainWindow);
   QObject::connect(&a, SIGNAL(messageReceived(const QString&)),
     &mainWindow, SLOT(handleMessage(const QString&)));
   
   a.setQuitOnLastWindowClosed(true);
   return a.exec();
}
