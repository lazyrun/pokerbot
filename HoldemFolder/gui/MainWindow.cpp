#include "MainWindow.h"
#include "SpectrumEditor.h"
#include "SettingsPage.h"
#include "SettingsData.h"
#include "OrderPage.h"
#include "SupportPage.h"

static void information(QWidget *parent, const QString &title,
                 const QString &text, const QString &detailedText)
{
#if QT_VERSION >= 0x040600
    QScopedPointer<QMessageBox> messageBox(new QMessageBox(parent));
#else
    QSharedPointer<QMessageBox> messageBox(new QMessageBox(parent));
#endif
    if (parent)
        messageBox->setWindowModality(Qt::WindowModal);
    messageBox->setWindowFlags(Qt::Dialog | Qt::WindowSystemMenuHint);
    messageBox->setWindowTitle(QString("%1 - %2")
            .arg(QApplication::applicationName()).arg(title));
    messageBox->setText(text);
    if (!detailedText.isEmpty())
        messageBox->setInformativeText(detailedText);
    messageBox->setIcon(QMessageBox::Information);
    messageBox->addButton(QMessageBox::Ok);
    messageBox->exec();
}

MainWindow::MainWindow(QWidget * parent, Qt::WindowFlags flags) 
: QDialog(parent, flags)
{
   addr_ = "http://maverickpokerbot.com/buynow/";
   addr_ru_ = "http://ru.maverickpokerbot.com/buynow/";

   QApplication::setApplicationName(tr("Maverick"));
   appTranslator_ = new QTranslator(this);
   QString regKey = QDir::current().dirName();
   QSettings settings(regKey, "Config");

   QVariant sett = settings.value("Settings");
   SettingsData data = qvariant_cast<SettingsData>(sett);
   if (data.lang.length() == 2)
   {
      appTranslator_->load("HoldemFolder_" + data.lang, ":/translations");
      qApp->installTranslator(appTranslator_);

      QTranslator * qtTranslator = new QTranslator(this);
      qtTranslator->load("qt_" + data.lang, ":/translations");
      qApp->installTranslator(qtTranslator);
   }
   else
   {
      //определить локаль
      appTranslator_->load("HoldemFolder_" + QLocale::system().name(), 
         ":/translations");
      qApp->installTranslator(appTranslator_);
      
      QTranslator * qtTranslator = new QTranslator(this);
      qtTranslator->load("qt_" + QLocale::system().name(), ":/translations");
      qApp->installTranslator(qtTranslator);
   }

   firstShow_ = true;
   //√лавное окно
   setMainWidget();
   QString title = tr("Maverick Poker Bot Configuration");
#ifdef DEMO_MODE
   title += " - Trial";
#endif
   setWindowTitle(title);
   setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
   
   //setAttribute(Qt::WA_DontShowOnScreen, true);
   //show();
   QTimer::singleShot(0, this, SLOT(loadConf()));
   //QTimer::singleShot(0, this, SLOT(setSize()));
   //QTimer::singleShot(0, this, SLOT(show()));
}

void MainWindow::setSize()
{
   //setAttribute(Qt::WA_DontShowOnScreen, true);
   quint16 screenWidth, width, screenHeight, height, x, y;
   QSize windowSize;
   screenWidth = QApplication::desktop()->width();
   screenHeight = QApplication::desktop()->height();
   windowSize = sizeHint();
   width = windowSize.width();
   height = windowSize.height();
   x = (screenWidth - width) / 2;
   y = (screenHeight - height) / 2;
   move(x, y);
   //setAttribute( Qt::WA_DontShowOnScreen, false);
   //setAttribute( Qt::WA_WState_ExplicitShowHide, false);
   //QTimer::singleShot(0, this, SLOT(show()));
}

void MainWindow::setMainWidget()
{
   listBar_ = new QToolBar(this);
   listBar_->setAllowedAreas(Qt::NoToolBarArea);
   listBar_->setIconSize(QSize(64, 64));
   listBar_->setMovable(false);
   listBar_->setFloatable(false);
   listBar_->setStyleSheet(".QToolBar{border: none;}");
   //listBar_->setToolButtonStyle(Qt::ToolButtonTextOnly);
   listBar_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
   listBar_->setOrientation(Qt::Vertical);
   connect(listBar_, SIGNAL(actionTriggered(QAction *)),
           this, SLOT(listToggled(QAction *)));

   actionGroup_ = new QActionGroup(this);

   QAction * act = listBar_->addAction(QIcon(":/images/card.png"), tr("Range"));
   act->setCheckable(true);
   act->setShortcut(QKeySequence("F5"));
   act->setToolTip(tr("Specify the playing card range"));

   QFont fn = act->font();
   fn.setPointSize(10);
   act->setFont(fn);
   actionGroup_->addAction(act);

   act = listBar_->addAction(QIcon(":/images/settings.png"), tr("Settings"));
   act->setCheckable(true);
   act->setShortcut(QKeySequence("F6"));
   act->setFont(fn);
   actionGroup_->addAction(act);
   act->setToolTip(tr("Additional settings"));
   
   act = listBar_->addAction(QIcon(":/images/support.png"), tr("Support"));
   act->setCheckable(true);
   act->setShortcut(QKeySequence("F8"));
   act->setFont(fn);
   actionGroup_->addAction(act);
   act->setToolTip(tr("Support Information, Help"));

#ifdef DEMO_MODE   
   act = listBar_->addAction(QIcon(":/images/credit_card.png"), tr("Get Now"));
   act->setCheckable(true);
   act->setShortcut(QKeySequence("F7"));
   act->setFont(fn);
   actionGroup_->addAction(act);
   act->setToolTip(tr("Get the Full Version"));
#endif

   act = listBar_->addAction(QIcon(":/images/poker.png"), tr("Start"));
   act->setShortcut(QKeySequence("F9"));
   act->setFont(fn);
   act->setToolTip(tr("Start Playing"));
   actionGroup_->addAction(act);
   connect(act, SIGNAL(triggered()), this, SLOT(startPlay()));

   //listBar_->addSeparator();

   QWidget * widStretch = new QWidget(this);
   //widStretch->setMinimumHeight(64);
   widStretch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
   listBar_->addWidget(widStretch);

   //listBar_->addSeparator();
   //act = listBar_->addAction(QIcon(":/images/save.png"), tr("Save"));
   //act->setCheckable(false);
   //act->setShortcut(QKeySequence("F9"));
   //act->setFont(fn);
   //act->setToolTip(tr("Save settings"));
   //actionGroup_->addAction(act);
   //connect(act, SIGNAL(triggered()), this, SLOT(saveConf()));

   act = listBar_->addAction(QIcon(":/images/exit2.png"), tr("Exit"));
   act->setCheckable(false);
   act->setShortcut(QKeySequence("F10"));
   act->setFont(fn);
   act->setToolTip(tr("Exit"));
   actionGroup_->addAction(act);
   connect(act, SIGNAL(triggered()), this, SLOT(cancel()));

   spectrum_ = new SpectrumEditor(this);
   spectrum_->setFont(fn);
   
   QFrame * gbx = new QFrame(this);
   QVBoxLayout * vl = new QVBoxLayout;
   vl->setMargin(0);
   vl->addWidget(listBar_);
   gbx->setLayout(vl);

   stacked_ = new QStackedWidget(this);
   stacked_->addWidget(spectrum_);

   settingsPage_ = new SettingsPage(this);
   settingsPage_->setFont(fn);
   stacked_->addWidget(settingsPage_);
   
   SupportPage * supportPage = new SupportPage(this);
   supportPage->setFont(fn);
   stacked_->addWidget(supportPage);

#ifdef DEMO_MODE
   OrderPage * orderPage = new OrderPage(this);
   orderPage->setFont(fn);
   connect(orderPage, SIGNAL(unlock()), this, SLOT(unlock()));
   stacked_->addWidget(orderPage);
#endif

   //stacked_->addWidget(new QWidget());
   
   QHBoxLayout * hl = new QHBoxLayout;
   hl->setMargin(6);
   hl->setSpacing(2);
   hl->addWidget(gbx, 0);
   hl->addWidget(stacked_, 1);

   QVBoxLayout * vlmain = new QVBoxLayout;
   vlmain->setSizeConstraint(QLayout::SetFixedSize);
   vlmain->setMargin(0);
   vlmain->addLayout(hl);
   
   setLayout(vlmain);
}

void MainWindow::showEvent(QShowEvent *)
{
   setUpdatesEnabled(false);
   actionGroup_->actions().at(0)->setChecked(true);
   QList<QAction *> la = actionGroup_->actions();
   int idx = stacked_->currentIndex();
   if (idx == -1)
      idx = 0;
   la.at(idx)->setChecked(true);
   
   if (!firstShow_)
   {
      setUpdatesEnabled(true);
      return;
   }
   firstShow_ = false;
   
   if (actionGroup_)
   {
      int maxWidth = 0;
      foreach (QAction * a, la)
      {
         QWidget * w = listBar_->widgetForAction(a);
         if (w->width() > maxWidth)
         {
            maxWidth = w->width();
            if (maxWidth < w->height())
            {
               maxWidth = w->height();
            }
         }
      }

      foreach (QAction * a, la)
      {
         //int height = listBar_->widgetForAction(a)->height();
         listBar_->widgetForAction(a)->setMinimumWidth(maxWidth);
      }
   }

#ifdef DEMO_MODE   
   QString regKey = QDir::current().dirName();
   QSettings settings(regKey, "Config");

   QVariant sett = settings.value("Settings");
   SettingsData data = qvariant_cast<SettingsData>(sett);
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
            stacked_->setCurrentIndex(3);
            foreach (QAction * a, la)
            {
               a->setEnabled(false);
            }
            listBar_->actions().at(listBar_->actions().count() - 1)
               ->setEnabled(true);

         }
      }
   }
#endif
   setUpdatesEnabled(true);
}

void MainWindow::listToggled(QAction * act)
{
   int idx = actionGroup_->actions().indexOf(act);
   stacked_->setCurrentIndex(idx);
}

void MainWindow::saveConf()
{
   QString regKey = QDir::current().dirName();
   QSettings settings(regKey, "Config");

   QString range = spectrum_->cardRange();
   settings.setValue("CardRange", range);

   QVariant sett = settingsPage_->value();
   settings.setValue("Settings", sett);

   //QTimer::singleShot(1000, this, SLOT(enableSave()));
   //information(this, tr("Configurator"), tr("The configuration has been saved."), 
   //   tr("Now you can close the settings and run the Holdem Agent."));
}

void MainWindow::loadConf()
{
   QString regKey = QDir::current().dirName();
   QSettings settings(regKey, "Config");

   QString range = settings.value("CardRange").toString();
   spectrum_->setCardRange(range);

   QVariant sett = settings.value("Settings");
   settingsPage_->setValue(sett);
   
   //
   //stacked_->setCurrentIndex(2);
}

void MainWindow::cancel()
{
   close();
}

void MainWindow::handleMessage(const QString & msg)
{
   if (msg == "Wake up!")
   {
      setWindowState(windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
      show();
      activateWindow();
   }
}

void MainWindow::unlock()
{
   actionGroup_->actions().at(2)->setChecked(true);
   QList<QAction *> la = actionGroup_->actions();
   foreach (QAction * a, la)
   {
      a->setEnabled(true);
   }
}

void MainWindow::closeEvent(QCloseEvent *)
{
   saveConf();

#ifdef DEMO_MODE
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
            int res = QMessageBox::question(0, tr("Maverick Demo Version"), 
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
#endif
}

void MainWindow::startPlay()
{
   saveConf();
   //узнать что за рум
   QString regKey = QDir::current().dirName();
   QSettings settings(regKey, "Config");
   
   QVariant sett = settings.value("Settings");
   SettingsData data = qvariant_cast<SettingsData>(sett);

   QString room = data.room;
   if (room.isEmpty())
   {
      QMessageBox::warning(0, tr("Maverick"), 
         tr("Poker room is not specified.\n"
         "Please specify a poker room in the Settings section."));

      return;
   }
   
   QSettings snames(regKey, "Names");
   QString roomname = snames.value(room).toString();
   QString agent = snames.value("ag").toString();
   QFileInfo fiAgent(agent);

   if (roomname.isEmpty() || agent.isEmpty() || !fiAgent.exists())
   {
      QMessageBox::critical(0, tr("Maverick"), 
         tr("Poker room alias is not specified.\n"
         "Please reinstall the software or contact to sowtware developers."));

      return;
   }

   QProcess::startDetached("\"" + agent + "\"", QStringList() << roomname);
}
