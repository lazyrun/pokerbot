#include "SupportPage.h"
//#include "VideoPlayer.h"
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
   tab->addTab(sup, tr("Support"));
   //tab->addTab(video, tr("Video Tutorial"));
   tab->addTab(help, tr("Help"));
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
   QString supText = "<h3>We love to hear from the people who use our software.</h3>"
      "<font color=orange>Contact us</font>"
      "<p>"
      "Technical support is available by e-mail addressed to our support department at <a href=\"mailto:support@holdemfold.com\" target=_blank>support@holdemfold.com</a>. "
      "Please feel free to email us, normally we will reply to you within one business day."
      "<p>Please specify in your message the following details:<ul>"
      "<li> program version"
      "<li> processor type"
      "<li> operating system version"
      "<li> amount of memory available"
      "<li> active applications</ul>"
      "<p><font color=orange>Registration key reclamation</font><p>"
      "If you are a registered user of the program and for a certain reason you need to reclaim your registration key, please contact us at <a href=\"mailto:sales@holdemfold.com\" target=_blank>sales@holdemfold.com</a> subject <b>Registration key reclamation</b>,"
      "  message with the following information:<ul>"
      "<li> Your first and last names</li>"
      "<li> The e-mail address the registration message was sent to</li>"
      "<li>  Program name</li>"
      "<li>  License type</li>"
      "<li>  Purchase date</li>"
      "<li>  Purchase method</li>"
      "<li>  Other purchase details</li></ul>"
      "The new registration data will be sent to you within 24 hours."
      "We will send you a registration key by e-mail free of charge. If you use an e-mail address different from the one specified during the registration, please specify all your registration information, including your previous e-mail address. We will send you a registration key after we have checked the registration information.";

   lblContact->setHtml(supText);
   vb->addWidget(lblContact);

   QLabel * lblInfo = new QLabel(this);
   lblInfo->setTextInteractionFlags(Qt::TextSelectableByMouse);
   QDate dtRelease = QDate::fromString(RELEASE_DATE, "yyyy-MM-dd");
   
   lblInfo->setText(QString("Holdem Folder (c) v. %1 by %2 All Right Reserved")
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

   browser->setSource(QUrl("file:///" + appExe + "/tutorial/help/index.htm"));
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
   
   QFile file(":/text/license.txt");
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
// VideoWidget
//
//VideoWidget::VideoWidget(QWidget * parent)
//: QWidget(parent)
//{
//   setupUi();
//}
//
//void VideoWidget::setupUi()
//{
//   //videoPlayer_ = new VideoPlayer(this->parentWidget()->parentWidget());
//   //videoPlayer_->setFixedSize(800, 600);
//   videoPlayer_ = 0;
//   QGridLayout * vb = new QGridLayout;
//   setLayout(vb);
//
//   QToolButton * lblTut1 = new QToolButton(this);
//   QToolButton * lblTut2 = new QToolButton(this);
//   
//   lblTut1->setIcon(QIcon(":/images/tut1.png"));
//   lblTut1->setIconSize(QSize(146, 146));
//   lblTut1->setCursor(Qt::PointingHandCursor);
//   lblTut2->setCursor(Qt::PointingHandCursor);
//   //lblTut1->setAutoRaise(true);
//   //lblTut2->setAutoRaise(true);
//   //lblTut1->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
//   //lblTut2->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
//   lblTut1->setFixedSize(QSize(150, 150));
//   lblTut2->setFixedSize(QSize(150, 150));
//   connect(lblTut1, SIGNAL(clicked()), this, SLOT(tutorial1()));
//   connect(lblTut2, SIGNAL(clicked()), this, SLOT(tutorial2()));
//
//   QLabel * lblTut11 = new QLabel(tr("Configuring HoldemFolder"), this);
//   QLabel * lblTut22 = new QLabel(tr("Playing on Poker Academy Pro"), this);
//   //lblTut11->setWordWrap(true);
//   //lblTut22->setWordWrap(true);
//   lblTut11->setAlignment(Qt::AlignCenter);
//   lblTut22->setAlignment(Qt::AlignCenter);
//
//   vb->addItem(new QSpacerItem(1, 24), 0, 0);
//   vb->addWidget(lblTut1, 1, 0, Qt::AlignCenter);
//   vb->addWidget(lblTut2, 1, 1, Qt::AlignCenter);
//   vb->addWidget(lblTut11, 2, 0, Qt::AlignCenter);
//   vb->addWidget(lblTut22, 2, 1, Qt::AlignCenter);
//   vb->setRowStretch(3, 1);
//}
//
//void VideoWidget::tutorial1()
//{
//   if (videoPlayer_ == 0)
//   {
//      qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
//      videoPlayer_ = new VideoPlayer(this->parentWidget()->parentWidget());
//      videoPlayer_->setFixedSize(800, 600);
//      qApp->restoreOverrideCursor();
//   }
//
//   QString appExe = qApp->applicationDirPath();
//   videoPlayer_->openVideo(appExe + "/tutorial/settings.avi");
//   videoPlayer_->exec();
//}
//
//void VideoWidget::tutorial2()
//{
//   if (videoPlayer_ == 0)
//   {
//      qApp->setOverrideCursor(QCursor(Qt::WaitCursor));
//      videoPlayer_ = new VideoPlayer(this->parentWidget()->parentWidget());
//      videoPlayer_->setFixedSize(800, 600);
//      qApp->restoreOverrideCursor();
//   }
//
//   QString appExe = qApp->applicationDirPath();
//   videoPlayer_->openVideo(appExe + "/tutorial/Video.avi");
//
//   videoPlayer_->exec();
//}
//
