#include "SupportPage.h"
#include "SettingsData.h"

#ifdef QT_NO_DEBUG
#include "ver.h"
#else
#include "ver1.h"
#endif

SupportPage::SupportPage(QWidget * parent)
: QGroupBox(parent)
{
   setupUi();
}

void SupportPage::setupUi()
{
   QVBoxLayout * vb = new QVBoxLayout;

	QLabel * lblCaption = new QLabel(this);
	lblCaption->setMinimumHeight(30);
   lblCaption->setText("<h3><center>" + tr("Support & Help") + "</center></h3>");
   lblCaption->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
   lblCaption->setStyleSheet("background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #d9ebfb, stop: 0.4 #a1d4fb,stop: 0.5 #a1d4fb, stop: 1.0 #d9ebfb); background-position: left top; border-style: solid; border-width: 1px; border-color: silver; border-radius: 3px; color: black;");

   vb->addWidget(lblCaption);
   
   QTabWidget * tab = new QTabWidget(this);
   SupportWidget * sup = new SupportWidget(this);
   HelpWidget * help = new HelpWidget(this);
   //VideoWidget * video = new VideoWidget(this);
   EulaWidget * eula = new EulaWidget(this);
   RoomWidget * rooms = new RoomWidget(this);

   tab->addTab(sup, tr("Support"));
   //tab->addTab(video, tr("Video Tutorial"));
   tab->addTab(help, tr("Help"));
   tab->addTab(rooms, tr("Room Settings"));
   tab->addTab(eula, tr("EULA"));
   vb->addWidget(tab);
   //vb->addStretch(1);
   setLayout(vb);
}

//
// SupportWidget
//
SupportWidget::SupportWidget(QWidget * parent)
: QWidget(parent)
{
   setupUi();
}

void SupportWidget::setupUi()
{
   QVBoxLayout * vb = new QVBoxLayout;
   setLayout(vb);

   QTextBrowser * lblContact = new QTextBrowser(this);
   lblContact->setOpenExternalLinks(true);
   QString supText = tr("<h3>We love to hear from the people who use our software.</h3>"
      "<font color=orange>Contact us</font>"
      "<p>"
      "Technical support is available by e-mail addressed to our support department at <a href=\"mailto:support@maverickpokerbot.com\" target=_blank>support@maverickpokerbot.com</a>. "
      "Please feel free to email us, normally we will reply to you within one business day."
      "<p>Please specify in your message the following details:<ul>"
      "<li> program version"
      "<li> processor type"
      "<li> operating system version"
      "<li> amount of memory available"
      "<li> active applications</ul>"
      "<p><font color=orange>Registration key reclamation</font><p>"
      "If you are a registered user of the program and for a certain reason you need to reclaim your registration key, please contact us at <a href=\"mailto:sales@maverickpokerbot.com\" target=_blank>sales@maverickpokerbot.com</a> subject <b>Registration key reclamation</b>,"
      "  message with the following information:<ul>"
      "<li> Your first and last names</li>"
      "<li> The e-mail address the registration message was sent to</li>"
      "<li>  Program name</li>"
      "<li>  License type</li>"
      "<li>  Purchase date</li>"
      "<li>  Purchase method</li>"
      "<li>  Other purchase details</li></ul>"
      "The new registration data will be sent to you within 24 hours."
      "We will send you a registration key by e-mail free of charge. If you use an e-mail address different from the one specified during the registration, please specify all your registration information, including your previous e-mail address. We will send you a registration key after we have checked the registration information.");

   lblContact->setHtml(supText);
   vb->addWidget(lblContact);

   QLabel * lblInfo = new QLabel(this);
   lblInfo->setTextInteractionFlags(Qt::TextSelectableByMouse);
   QDate dtRelease = QDate::fromString(RELEASE_DATE, "yyyy-MM-dd");
   
   lblInfo->setText(QString("Maverick Poker Bot (c) v. %1 by %2 All Right Reserved")
      .arg(VERSION).arg(dtRelease.toString(Qt::SystemLocaleShortDate)));
   lblInfo->setCursor(Qt::IBeamCursor);
   vb->addWidget(lblInfo);
}

//
// HelpWidget
//
HelpWidget::HelpWidget(QWidget * parent)
: QWidget(parent)
{
   setupUi();
}

void HelpWidget::setupUi()
{
   QVBoxLayout * vb = new QVBoxLayout;
   QTextBrowser * browser = new QTextBrowser(this);

   QString appExe = qApp->applicationDirPath();

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
   QString helpFile = QString(":/help/index_%1.htm").arg(suffix);
   if (!QFile::exists(helpFile))
      helpFile = ":/help/index_en.htm";

   //QString html;
   //QFile file(helpFile);
   //if (file.open(QIODevice::ReadOnly))
   //{
   //   html = QString(file.readAll());
   //}
   //browser->setHtml(html);
   browser->setSource(QUrl("qrc" + helpFile));
   vb->addWidget(browser);
   setLayout(vb);
}

//
// EulaWidget
//
EulaWidget::EulaWidget(QWidget * parent)
: QWidget(parent)
{
   setupUi();
}

void EulaWidget::setupUi()
{
   QVBoxLayout * vb = new QVBoxLayout;
   setLayout(vb);
   
   QTextEdit * plain = new QTextEdit(this);
   plain->setReadOnly(true);
   
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
   
   QString licFile = QString(":/text/license_%1.txt").arg(suffix);
   if (!QFile::exists(licFile))
   {
      licFile = ":/text/license_en.txt";
   }
   QFile file(licFile);
   
   file.open(QIODevice::ReadOnly | QIODevice::Text);
   if (file.isOpen())
   {
      QByteArray ba = file.readAll();
      file.close();
      QString str = QString::fromUtf8(ba.data());

      plain->setText(str);
   }

   vb->addWidget(plain);
}

//
// RoomWidget
//
RoomWidget::RoomWidget(QWidget * parent)
: QTabWidget(parent)
{
   setupUi();
}

void RoomWidget::setupUi()
{
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
   QString helpCake = QString(":/help/cake_%1.htm").arg(suffix);
   if (!QFile::exists(helpCake))
      helpCake = ":/help/cake_en.htm";

//#ifndef DEMO_MODE
   QTextBrowser * cakeText = new QTextBrowser(this);
   cakeText->setSource(QUrl("qrc" + helpCake));
   addTab(cakeText, "Cake Poker");
//#endif

   QTextBrowser * rspText = new QTextBrowser(this);
   QTextBrowser * acadText = new QTextBrowser(this);
   
   //QString helpRsp = ":/help/rsp.htm";
   //QString helpAcad = ":/help/acad.htm";

   QString helpRsp = QString(":/help/rsp_%1.htm").arg(suffix);
   if (!QFile::exists(helpRsp))
      helpRsp = ":/help/rsp_en.htm";

   QString helpAcad = QString(":/help/acad_%1.htm").arg(suffix);
   if (!QFile::exists(helpAcad))
      helpAcad = ":/help/acad_en.htm";

   rspText->setSource(QUrl("qrc" + helpRsp));
   acadText->setSource(QUrl("qrc" + helpAcad));
   
   addTab(rspText, "Red Star Poker");
   addTab(acadText, "Poker Academy Pro");
}


