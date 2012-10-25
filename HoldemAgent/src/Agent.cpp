#include <QMessageBox>

#include "Agent.h"
#include "qxtglobalshortcut.h"
#include "SettingsData.h"

static QSystemTrayIcon * trayIcon_;

void CALLBACK showMessageCallbackFunc(const char * hand)
{
   QString regKey = QDir::current().dirName();
   trayIcon_->showMessage(regKey, 
      QString(hand),
      QSystemTrayIcon::Information, 1000);
}

Agent::Agent(QObject * parent)
: QObject(parent)
{
   addr_ = "https://secure.payproglobal.com/orderpage.aspx?products=93056";
   addr_ru_ = "https://secure.payproglobal.com/orderpage.aspx?products=93056";
   qRegisterMetaType<SettingsData>("SettingsData");
   qRegisterMetaTypeStreamOperators<SettingsData>("SettingsData");

   qRegisterMetaType<Range>("Range");
   qRegisterMetaTypeStreamOperators<Range>("Range");

   QString regKey = QDir::current().dirName();
   QSettings settings(regKey, "Config");
   
   QVariant sett = settings.value("Settings");
   SettingsData data = qvariant_cast<SettingsData>(sett);
   QString suffix = "en";
   if (data.lang.length() == 2)
   {
      suffix = data.lang;
   }
   else
   {
      //определить локаль
      suffix = QLocale::system().name();
      suffix.truncate(2);
   }

   //Транслятор
   QTranslator * appTranslator = new QTranslator(this);
   appTranslator->load("HoldemAgent_" + suffix, ":/translations");
   qApp->installTranslator(appTranslator);

   QTranslator * qtTranslator = new QTranslator(this);
   qtTranslator->load("qt_" + suffix, ":/translations");
   qApp->installTranslator(qtTranslator);

   setupUi();
}

void Agent::setupUi()
{
   startShortcut_ = new QxtGlobalShortcut(this);
   startShortcut_->setShortcut(QKeySequence("Ctrl+F5"));
   startShortcut_->setEnabled(true);
   connect(startShortcut_, SIGNAL(activated()), this, SLOT(start()));

   stopShortcut_ = new QxtGlobalShortcut(this);
   stopShortcut_->setShortcut(QKeySequence("Ctrl+F6"));
   connect(stopShortcut_, SIGNAL(activated()), this, SLOT(stop()));
   stopShortcut_->setEnabled(false);

   restartShortcut_ = new QxtGlobalShortcut(this);
   restartShortcut_->setShortcut(QKeySequence("Ctrl+F7"));
   restartShortcut_->setEnabled(true);
   connect(restartShortcut_, SIGNAL(activated()), this, SLOT(restart()));
   
   QxtGlobalShortcut * confShortcut_ = new QxtGlobalShortcut(this);
   confShortcut_->setShortcut(QKeySequence("Ctrl+F9"));
   confShortcut_->setEnabled(true);
   connect(confShortcut_, SIGNAL(activated()), this, SLOT(config()));

   QxtGlobalShortcut * exitShortcut_ = new QxtGlobalShortcut(this);
   exitShortcut_->setShortcut(QKeySequence("Ctrl+F10"));
   confShortcut_->setEnabled(true);
   connect(exitShortcut_, SIGNAL(activated()), this, SLOT(exit()));

   createTrayIcon();
   trayIcon_->show();
}

void Agent::createTrayIcon()
{
   startAction_ = new QAction(tr("&Start"), this);
   connect(startAction_, SIGNAL(triggered()), this, SLOT(start()));
   startAction_->setShortcut(QKeySequence("Ctrl+F5"));
   
   stopAction_ = new QAction(tr("S&top"), this);
   connect(stopAction_, SIGNAL(triggered()), this, SLOT(stop()));
   stopAction_->setShortcut(QKeySequence("Ctrl+F6"));
   stopAction_->setEnabled(false);

   restartAction_ = new QAction(tr("&Restart"), this);
   connect(restartAction_, SIGNAL(triggered()), this, SLOT(restart()));
   restartAction_->setShortcut(QKeySequence("Ctrl+F7"));
   restartAction_->setEnabled(false);

   QAction * confAction = new QAction(tr("&Configure"), this);
   connect(confAction, SIGNAL(triggered()), this, SLOT(config()));
   confAction->setShortcut(QKeySequence("Ctrl+F9"));

   rangesAction_ = new QAction(tr("&Ranges"), this);
   rangesMenu_ = new QMenu(0);
   rangesAction_->setMenu(rangesMenu_);
   
   bool hasRanges = initFavoriteRanges();
   rangesAction_->setVisible(hasRanges);

   //connect(confAction, SIGNAL(triggered()), this, SLOT(config()));
   //confAction->setShortcut(QKeySequence("Ctrl+F9"));

   QAction * quitAction = new QAction(tr("&Quit"), this);
   connect(quitAction, SIGNAL(triggered()), this, SLOT(exit()));
   quitAction->setShortcut(QKeySequence("Ctrl+F10"));

   trayIconMenu_ = new QMenu(0);
   trayIconMenu_->addAction(startAction_);
   trayIconMenu_->addAction(stopAction_);
   trayIconMenu_->addAction(restartAction_);
   trayIconMenu_->addSeparator();
   trayIconMenu_->addAction(confAction);
   
   trayIconMenu_->addAction(rangesAction_);
   trayIconMenu_->addSeparator();
   trayIconMenu_->addAction(quitAction);

   trayIcon_ = new QSystemTrayIcon(this);
   trayIcon_->setContextMenu(trayIconMenu_);
   trayIcon_->setIcon(QIcon(":/icon/poker.png"));
   connect(trayIcon_, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
             this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
}

bool Agent::initFavoriteRanges()
{
   //читаем данные с реестра
   QString regKey = QDir::current().dirName();
   QSettings settings(regKey, "Ranges");
   QStringList keys = settings.allKeys();
   bool hasRanges = keys.count();
   
   foreach (QAction *act, rangesMenu_->actions())
   {
      rangesMenu_->removeAction(act);
      delete act;
   }

   rangesMenu_->clear();
   rangesAction_->setVisible(hasRanges);
   foreach (QxtGlobalShortcut* sCut, rangesShortcuts_)
   {
      delete sCut;
   }
   rangesShortcuts_.clear();


   foreach (QString key, keys)
   {
      QVariant varRng = settings.value(key);
      Range vrng = qvariant_cast<Range>(varRng);

      QAction * act = rangesMenu_->addAction(vrng.name, this, SLOT(rangeSelected()));
      rangeMap_.insert(act, vrng);
      if (!vrng.modifiers.isEmpty() && !vrng.hotkey.isEmpty())
      {
         QxtGlobalShortcut * qxtShortcut = new QxtGlobalShortcut(this);
         qxtShortcut->setShortcut(QKeySequence(vrng.modifiers + "+" + vrng.hotkey));
         qxtShortcut->setEnabled(true);
         connect(qxtShortcut, SIGNAL(activated()), this, SLOT(rangeSelected()));

         rangeQxtMap_.insert(qxtShortcut, vrng);
         rangesShortcuts_ << qxtShortcut;
         act->setShortcut(QKeySequence(vrng.modifiers + "+" + vrng.hotkey));
      }
   }

   return hasRanges;
}

void Agent::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
   if (reason == QSystemTrayIcon::DoubleClick)
   {
      //setWindowState(windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
      //show();
      //activateWindow();
   }
}

void Agent::load(const QString & module)
{
   QString regKey = QDir::current().dirName();
   QSettings settings(regKey, "Config");
   
   QVariant sett = settings.value("Settings");
   SettingsData data = qvariant_cast<SettingsData>(sett);
#ifdef DEMO_MODE   
   if (data.who.isEmpty())
   {
      QDate start = data.firstRun;
      QDate cd = QDate::currentDate();
      int days = 14;
      if (start.isValid())
      {
         days = 14 - (cd.toJulianDay() - start.toJulianDay());
         if (days <= 0)
         {
            trayIcon_->showMessage(regKey, 
               tr("Trial period has been expiried."),
               QSystemTrayIcon::Information, 5000);
            
            QTimer::singleShot(6000, this, SLOT(exit()));
            return;   
         }
      }
   }
#endif

   if (module.isEmpty())
   {
      trayIcon_->showMessage(regKey, tr("Poker-room is not specified."),
         QSystemTrayIcon::Information, 5000);
   
      //QTimer::singleShot(5000, this, SLOT(exit()));
      return;
   }
   
   if (loadedModules_.contains(module))
   {
      trayIcon_->showMessage(regKey, 
         tr("Agent for %1 is already loaded.").arg(module),
         QSystemTrayIcon::Information, 5000);

   }
   else
   {
      //проверка наличия библиотеки
      ExecStart f_start = (ExecStart) QLibrary::resolve(module, "ExecStart");
      ExecStop f_stop   = (ExecStop)  QLibrary::resolve(module, "ExecStop");
      ExecKill f_kill   = (ExecKill)  QLibrary::resolve(module, "ExecKill");
      ExecName f_name   = (ExecName)  QLibrary::resolve(module, "ExecName");
      if (f_start && f_stop && f_kill && f_name)
      {
         loadedModules_.append(module);
         
         f_startList_.append(f_start);
         f_stopList_.append(f_stop);
         f_killList_.append(f_kill);
         f_nameList_.append(f_name);

         char * name = new char[100];
         f_name(name, 100);
         trayIcon_->showMessage(regKey,
            tr("Agent for %1 has been loaded.").arg(name),
            QSystemTrayIcon::Information, 5000);
         trayIcon_->setToolTip(tr("Agent for %1.").arg(name));

         if (f_start)
         {
            f_start(showMessageCallbackFunc);
            stopShortcut_->setEnabled(true);
            stopAction_->setEnabled(true);

            restartShortcut_->setEnabled(true);
            restartAction_->setEnabled(true);

            startShortcut_->setEnabled(false);
            startAction_->setEnabled(false);
         }
      }
      else
      {
         trayIcon_->showMessage(regKey, 
            tr("Agent cannot load module %1. Wrong format of dll.").arg(module),
            QSystemTrayIcon::Information, 500);
         return;

      }
   }
}

void Agent::handleMessage(const QString & command)
{
   if (command.contains("Wake up!"))
   {
      QStringList mods = command.split("!");
      if (mods.count() > 1)
      {
         QString mod = mods.at(1);
         load(mod);
      }
   }
}

void Agent::start()
{
   //запуск всех функций
   foreach (ExecStart f_start, f_startList_)
   {
      if (f_start)
         f_start(showMessageCallbackFunc);
   }
   startAction_->setEnabled(false);
   stopAction_->setEnabled(true);
   restartAction_->setEnabled(true);

   startShortcut_->setEnabled(false);
   stopShortcut_->setEnabled(true);
   restartShortcut_->setEnabled(true);
}

void Agent::stop()
{
   //останов всех функций
   foreach (ExecStop f_stop, f_stopList_)
   {
      if (f_stop)
         f_stop();
   }
   startAction_->setEnabled(true);
   stopAction_->setEnabled(false);
   restartAction_->setEnabled(false);

   startShortcut_->setEnabled(true);
   stopShortcut_->setEnabled(false);
   restartShortcut_->setEnabled(false);
}

void Agent::restart()
{
   bool hasRanges = initFavoriteRanges();
   rangesAction_->setVisible(hasRanges);
   
   stop();
   start();
}

void Agent::exit()
{
   //уничтожение всех dll
   foreach (ExecKill f_kill, f_killList_)
   {
      if (f_kill)
         f_kill();
   }
#ifdef DEMO_MODE
   nagExit();
#endif
   qApp->quit();
}

void Agent::config()
{
   QString appDir = qApp->applicationDirPath();
   if (configPath_.isEmpty())
   {
      //ищем в каталоге приложения exe с именем == имени папки
      QString appExe = qApp->applicationFilePath();
      QFileInfo me(appExe);
      QDir dir(appDir);
      foreach (QString file, dir.entryList(QDir::Files))
      {
         QFileInfo fi(file);
         if (fi.suffix() == "exe")
         {
            if (fi.baseName() == dir.dirName())
            {
               //запустить
               configPath_ = fi.absoluteFilePath();
               break;
            }
         }
      }
   }
   if (!configPath_.isEmpty())
   {
      QProcess * confProc = new QProcess(0);
      confProc->setWorkingDirectory(appDir);
      confProc->start(configPath_, QStringList());
      connect(confProc, SIGNAL(finished(int, QProcess::ExitStatus)),
         this, SLOT(confFinished(int, QProcess::ExitStatus)));
      //QProcess::startDetached(configPath_, QStringList(), appDir);
   }
   else
   {
      trayIcon_->showMessage(tr("Maverick Poker Bot"), 
         tr("Cannot find the configurator. Please reinstall Maverick Poker Bot."),
         QSystemTrayIcon::Information, 5000);

   }
}

void Agent::confFinished(int, QProcess::ExitStatus)
{
   restart();
}

void Agent::nagExit()
{
   QString regKey = QDir::current().dirName();
   QSettings settings(regKey, "Config");
   
   QVariant sett = settings.value("Settings");
   SettingsData data = qvariant_cast<SettingsData>(sett);
   
   if (data.who.isEmpty())
   {
      QDate start = data.firstRun;
      QDate cd = QDate::currentDate();
      if (start.isValid())
      {
         int days = 2 - (cd.toJulianDay() - start.toJulianDay());
         if (days <= 0)
         {
            int res = QMessageBox::question(0, tr("Maverick Poker Bot"), 
               tr("Maverick Poker Bot is not registered. "
               "Please register it and you will no longer see this message. \n"
               "Do you want to register right now?"), 
               QMessageBox::Yes | QMessageBox::No);
            
            if (res == QMessageBox::Yes)
            {
               if (QLocale::system().name().contains("ru_"))
                  QDesktopServices::openUrl(QUrl(addr_ru_, QUrl::TolerantMode));
               else
                  QDesktopServices::openUrl(QUrl(addr_, QUrl::TolerantMode));
            }
         }
      }
   }
}

void Agent::rangeSelected()
{
   QxtGlobalShortcut * qxt = qobject_cast<QxtGlobalShortcut *>(sender());
   Range rng;
   if (qxt && rangeQxtMap_.contains(qxt))
   {
      rng = rangeQxtMap_.value(qxt);
   }
   else
   {
      QAction * act = qobject_cast<QAction *>(sender());
      if (act && rangeMap_.contains(act))
      {
         rng = rangeMap_.value(act);
      }
   }
   
   QString regKey = QDir::current().dirName();
   QSettings settings(regKey, "Config");
   settings.setValue("CardRange", rng.matrix);
   restart();

   trayIcon_->showMessage(regKey, 
      tr("The current range is switched to %1.").arg(rng.name),
      QSystemTrayIcon::Information, 5000);

}
