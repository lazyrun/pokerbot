#include "OrderPage.h"
#include "SettingsData.h"
#include "KeyProc.h"
//#include "aqp.hpp"

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
    messageBox->setWindowTitle(QString("%1 - %2")
            .arg(QApplication::applicationName()).arg(title));
    messageBox->setText(text);
    if (!detailedText.isEmpty())
        messageBox->setInformativeText(detailedText);
    messageBox->setIcon(QMessageBox::Information);
    messageBox->addButton(QMessageBox::Ok);
    messageBox->exec();
}

OrderPage::OrderPage(QWidget * parent)
: QScrollArea(parent)
{
   addr_ = "http://maverickpokerbot.com/buynow/";
   addr_ru_ = "http://ru.maverickpokerbot.com/buynow/";
   setupUi();
}

void OrderPage::setupUi()
{
   QVBoxLayout * vb = new QVBoxLayout;
   //vb->setSizeConstraint(QLayout::SetMinAndMaxSize);

	QLabel * lblCaption = new QLabel(this);
	lblCaption->setMinimumHeight(30);
   lblCaption->setText("<h3><center>" + tr("Registration") + "</center></h3>");
   lblCaption->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
   lblCaption->setStyleSheet("background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #d9ebfb, stop: 0.4 #a1d4fb,stop: 0.5 #a1d4fb, stop: 1.0 #d9ebfb); background-position: left top; border-style: solid; border-width: 1px; border-color: silver; border-radius: 3px; color: black;");

   vb->addWidget(lblCaption);

   lblUnreg_ = new QLabel(this);

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
         if (days > 14)
         {
            days = 14;
         }
         else if (days <= 0)
         {
            days = 0;
         }
      }
      else
      {
         data.firstRun = cd;
         settings.setValue("Settings", QVariant::fromValue(data));
      }
      
      lblUnreg_->setText(QString("<h1><center>%1</center></h1>"
         "<h2><center><font color=red>%2</font> ").arg(tr("DEMO VERSION")).arg(days)
         + tr("days left") 
         + QString("</center></h2>"));
   }
   else
   {
      lblUnreg_->setText(
         QString("<h1><center>%1 <font color=red>%2</font></center></h1>")
         .arg(tr("Registered by")).arg(data.who));
   }

   lblUnreg_->setContentsMargins(6, 6, 6, 6);
   lblUnreg_->setWordWrap(true);
   QPalette p;
   QColor cl; 
   cl = p.color(QPalette::Light);
   QString s1 = cl.name();
   
   cl = p.color(QPalette::Button);
   QString s2 = cl.name();

   cl = p.color(QPalette::Midlight);
   QString s3 = cl.name();

   lblUnreg_->setStyleSheet(QString(".QLabel{background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 0.2 %2, stop: 0.4 %2, stop: 0.8 %3, stop: 1.0 %4); border: solid; border-color: silver; border-width: 1px; }").arg(s1).arg(s3).arg(s2).arg(s1));

   //lblUnreg_->setFrameStyle(QFrame::Panel | QFrame::Sunken);
   //lblRemains_ = new QLabel(this);
   //lblRemains_->setText(QString("<h2><center>Remains <font color=red>%1</font> days</center></h2>").arg(14));
   vb->addWidget(lblUnreg_, 0);
   //vb->addWidget(lblRemains_);
   vb->addSpacerItem(new QSpacerItem(1, 12));
   //vb->addWidget(lblLine);

   QLabel * lblText = new QLabel(this);
   lblText->setWordWrap(true);
   lblText->setText(tr("To get the full version of <b>Maverick Poker Bot</b> &copy; click the Buy Now button or use the direct link below."));
      
   QLineEdit * edtAddr = new QLineEdit(this);
   if (QLocale::system().name().contains("ru_"))   
      edtAddr->setText(addr_ru_);
   else
      edtAddr->setText(addr_);

   edtAddr->setReadOnly(true);
   edtAddr->selectAll();
   edtAddr->setCursor(Qt::IBeamCursor);

   QTextEdit * plain = new QTextEdit(this);
   plain->setReadOnly(true);

   plain->setWordWrapMode(QTextOption::WordWrap);
   plain->setText(tr("On payment approval, you will receive the confirmation email containing a link to download the full version. Download and install it and you will get the full version.<p>"
   "We use Plimus to handle our electronic purchases, to ensure that your online purchase is secure and easy to use. Plimus accepts payments by Credit Cards (Visa, MasterCard, Eurocard, American Express, Discover, JCB), PayPal, Fax Ordering, Postal Mail, Phone, Purchase Order.")
   );

   QPushButton * btnOrder = new QPushButton(this);
   btnOrder->setIcon(QIcon(":/images/buynow.png"));
   btnOrder->setIconSize(QSize(128, 128));
   btnOrder->setFixedSize(132, 132);
   btnOrder->setToolTip(tr("Buy the Full Version"));

   connect(btnOrder, SIGNAL(clicked()), this, SLOT(buynow()));
   
   //QPushButton * btnKey = new QPushButton(this);
   //btnKey->setIcon(QIcon(":/images/lickey.png"));
   //btnKey->setIconSize(QSize(128, 128));
   //btnKey->setFixedSize(132, 132);
   //btnKey->setToolTip(tr("Enter the license key"));

   //connect(btnKey, SIGNAL(clicked()), this, SLOT(enterKey()));

   //QGroupBox * gbReg = new QGroupBox(tr("Get Registration"), this);
   //gbReg->setFlat(true);
   //gbReg->setAlignment(Qt::AlignHCenter);

   QHBoxLayout * hb = new QHBoxLayout;
   hb->addWidget(btnOrder);
   //hb->addWidget(btnKey);

   //QHBoxLayout * hbVisa = new QHBoxLayout;
   //QLabel * lblVisa = new QLabel(this);
   //lblVisa->setPixmap(QPixmap(":/images/visa.png"));
   ////QLabel * lblPlimus = new QLabel(this);
   ////lblPlimus->setPixmap(QPixmap(":/images/plimus_logo.png"));
   ////QLabel * lblVerisign = new QLabel(this);
   ////lblVerisign->setPixmap(QPixmap(":/images/verisign.png"));

   //hbVisa->addWidget(lblVisa);
   ////hbVisa->addWidget(lblPlimus);
   ////hbVisa->addWidget(lblVerisign);
   //hbVisa->addStretch(1);
   //vb->addWidget(gbReg);
   vb->addLayout(hb, 0);
   vb->addWidget(lblText, 0);
   vb->addWidget(edtAddr, 0);
   vb->addWidget(plain, 1);
   //vb->addStretch(1);
   //vb->addLayout(hbVisa);
   //vb->addStretch(2);
   QWidget * w = new QWidget(this);
   w->setLayout(vb);
   setWidgetResizable(true);
   setWidget(w);

   enterDlg_ = new EnterKeyDlg(this->parentWidget());
}

void OrderPage::buynow()
{
   if (QLocale::system().name().contains("ru_"))
      QDesktopServices::openUrl(QUrl(addr_ru_, QUrl::TolerantMode));
   else
      QDesktopServices::openUrl(QUrl(addr_, QUrl::TolerantMode));
   
   qApp->processEvents();
}

void OrderPage::enterKey()
{
   enterDlg_->exec();
   if (enterDlg_->result() == QDialog::Accepted)
   {
      QString regKey = QDir::current().dirName();
      QSettings settings(regKey, "Config");

      QVariant sett = settings.value("Settings");
      SettingsData data = qvariant_cast<SettingsData>(sett);

      lblUnreg_->setText(QString("<h1><center>%1 <font color=red>%2</font></center></h1>")
         .arg(tr("Registered by")).arg(data.who));
      
      emit unlock();
   }
}

//
//
EnterKeyDlg::EnterKeyDlg(QWidget * parent)
: QDialog(parent, Qt::Dialog | Qt::WindowSystemMenuHint)
{
   setupUi();
}

void EnterKeyDlg::setupUi()
{
   QVBoxLayout * vb = new QVBoxLayout;
   setLayout(vb);
   
   QLabel * lblName = new QLabel(tr("License Name:"), this);
   QLabel * lblKey = new QLabel(tr("License Key:"), this);

   edtName_ = new QLineEdit(this);
   edtKey_  = new QLineEdit(this);

   QDialogButtonBox * box = new QDialogButtonBox(this);
   box->addButton(tr("&OK"), QDialogButtonBox::AcceptRole);
   box->addButton(tr("&Cancel"), QDialogButtonBox::RejectRole);
   connect(box, SIGNAL(accepted()), this, SLOT(accept()));
   connect(box, SIGNAL(rejected()), this, SLOT(reject()));

   vb->setSpacing(3);
   vb->addWidget(lblName);
   vb->addWidget(edtName_);
   vb->addWidget(lblKey);
   vb->addWidget(edtKey_);
   vb->addSpacerItem(new QSpacerItem(1, 6));
   vb->addWidget(box);

   setWindowTitle(tr("Enter the license key"));
   
   setFixedSize(QSize(300, vb->sizeHint().height()));
}

void EnterKeyDlg::accept()
{
   AbstractProc * proc = new AcadProc();
   connect(proc, SIGNAL(isOk(const QString &, const QString &)), 
      proc, SLOT(write(const QString &, const QString &)), Qt::QueuedConnection);
   connect(proc, SIGNAL(isOk(const QString &, const QString &)), 
      this, SLOT(keySuccess()));
   connect(proc, SIGNAL(notOk()), this, SLOT(keyFail()));
   proc->checkKey(edtName_->text(), edtKey_->text());
}

void EnterKeyDlg::keyFail()
{
   information(this, tr("Wrong Key"), tr("Your license key is wrong."), 
      tr("Please check your license key. Use copy/paste to insert your key in input field."));
   edtKey_->setFocus();
   edtKey_->selectAll();
}

void EnterKeyDlg::keySuccess()
{
   information(this, tr("Thank you"), tr("Thank you for your registration."), 
      tr("Registration has been successful."));
   QDialog::accept();
}

