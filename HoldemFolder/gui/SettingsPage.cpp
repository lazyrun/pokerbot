#include "SettingsPage.h"
#include "SettingsData.h"

SettingsPage::SettingsPage(QWidget * parent)
: QGroupBox(parent)
{
   setupUi();
}

void SettingsPage::setupUi()
{
   QVBoxLayout * vb = new QVBoxLayout;
   QGridLayout * grid = new QGridLayout;

	QLabel * lblCaption = new QLabel(this);
	lblCaption->setMinimumHeight(30);
   lblCaption->setText("<h3><center>" + tr("Additional Settings") + "</center></h3>");
   lblCaption->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
   lblCaption->setStyleSheet("background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #d9ebfb, stop: 0.4 #a1d4fb,stop: 0.5 #a1d4fb, stop: 1.0 #d9ebfb); background-position: left top; border-style: solid; border-width: 1px; border-color: silver; border-radius: 3px; color: black;");

   btnTurn_  = new QToolButton(this);
   btnFold_  = new QToolButton(this);
   btnCheck_ = new QToolButton(this);

   btnTurn_->setIcon(QIcon(":/images/speaker.png"));
   btnFold_->setIcon(QIcon(":/images/speaker.png"));
   btnCheck_->setIcon(QIcon(":/images/speaker.png"));

   btnTurn_->setAutoRaise(true);
   btnFold_->setAutoRaise(true); 
   btnCheck_->setAutoRaise(true);
   
   btnTurn_->setToolTip(tr("Soundcheck"));
   btnFold_->setToolTip(tr("Soundcheck")); 
   btnCheck_->setToolTip(tr("Soundcheck"));
   
   btnTurn_->setEnabled(false);
   btnFold_->setEnabled(false); 
   btnCheck_->setEnabled(false);
   connect(btnTurn_, SIGNAL(clicked()), this, SLOT(testTurn()));
   connect(btnFold_, SIGNAL(clicked()), this, SLOT(testFold()));
   connect(btnCheck_, SIGNAL(clicked()), this, SLOT(testCheck()));

   chkTurn_ = new QCheckBox(tr("&Play a sound when you have a good hand"), this);
   chkFold_ = new QCheckBox(tr("Play a sound when the cards are &folded"), this);
   chkCheck_ = new QCheckBox(tr("Play a sound when Maverick &checks"), this);
   chkVisual_ = new QCheckBox(tr("&Highlight the table with a good hand"), this);
   chkShowFolded_ = new QCheckBox(tr("&Show folded hands"), this);
   chkAdvisor_ = new QCheckBox(tr("&Advisor mode (only tips, no actual actions)"),
      this);
   //chkKeep_ = new QCheckBox(tr("&Keep a log of the folder"), this);
   
   connect(chkTurn_, SIGNAL(toggled(bool)), this, SLOT(turnSwitch(bool)));
   connect(chkFold_, SIGNAL(toggled(bool)), this, SLOT(foldSwitch(bool)));
   connect(chkCheck_, SIGNAL(toggled(bool)), this, SLOT(checkSwitch(bool)));

   spnInterval_ = new QDoubleSpinBox(this);
   spnInterval_->setMinimum(0.1);
   spnInterval_->setMaximum(5);
   spnInterval_->setSingleStep(0.1);
   spnInterval_->setMinimumWidth(80);
   spnInterval_->setValue(1.00);
   spnInterval_->setDecimals(1);

   QLabel * lblInterval = new QLabel(tr("Process tables every (&sec.):"), this);
   lblInterval->setBuddy(spnInterval_);
   
   QLabel * lblRoom = new QLabel(tr("&Poker room:"), this);
   cmbRoom_ = new QComboBox(this);
   cmbRoom_->setEditable(false);
#ifndef DEMO_MODE
   cmbRoom_->addItem("Party Poker", "party");
   cmbRoom_->addItem("Titan Poker", "titan");
#endif
   cmbRoom_->addItem("Cake Poker", "cake");
   cmbRoom_->addItem("Red Star Poker", "rsp1");
   cmbRoom_->addItem("Poker Academy Pro", "acad");
   cmbRoom_->setMinimumWidth(120);
   lblRoom->setBuddy(cmbRoom_);

   QLabel * lblLang = new QLabel(tr("&Interface language:"), this);
   cmbLang_ = new QComboBox(this);
   cmbLang_->setEditable(false);
   cmbLang_->addItem("English", "en");
   cmbLang_->setMinimumWidth(120);
   lblLang->setBuddy(cmbLang_);
   
   QString appExe = qApp->applicationDirPath();
   QDir dir(":/translations");
   QStringList files = dir.entryList(QDir::Files);
   foreach (QString file, files)
   {
      if (file.right(2) == "qm" && file.contains("HoldemFolder_"))
      {
         int idx = file.indexOf("_");
         QString loc = file.mid(idx + 1, 2);
         QLocale locale(loc);
         cmbLang_->addItem(QLocale::languageToString(locale.language()), loc);
      }
   }

   vb->addWidget(lblCaption);
   vb->addSpacerItem(new QSpacerItem(1, 12));
   vb->addLayout(grid);
   grid->setContentsMargins(6, 6, 6, 6);
   int row = 0;
   grid->addWidget(chkTurn_, row, 0, Qt::AlignLeft);
   grid->addWidget(btnTurn_, row++, 1, Qt::AlignLeft);
   
   grid->addWidget(chkFold_, row, 0, Qt::AlignLeft);
   grid->addWidget(btnFold_, row++, 1, Qt::AlignLeft);

   grid->addWidget(chkCheck_, row, 0, Qt::AlignLeft);
   grid->addWidget(btnCheck_, row++, 1, Qt::AlignLeft);
   
   grid->addWidget(chkVisual_, row++, 0, 1, 2, Qt::AlignLeft);
   grid->addWidget(chkShowFolded_, row++, 0, 1, 2, Qt::AlignLeft);
   grid->addWidget(chkAdvisor_, row++, 0, 1, 2, Qt::AlignLeft);
   
   QGridLayout * grid1 = new QGridLayout;
   grid1->setMargin(0);
   grid1->addWidget(lblInterval,    0, 0, Qt::AlignLeft);
   grid1->addWidget(spnInterval_,   0, 1, Qt::AlignLeft);
   grid->addLayout(grid1, row++, 0);

   QGridLayout * grid2 = new QGridLayout;
   grid2->setMargin(0);
   grid2->addWidget(lblRoom,        0, 0, Qt::AlignLeft);
   grid2->addWidget(cmbRoom_,       0, 1, Qt::AlignLeft);
   //grid->addLayout(grid2, row++, 0);

   //QGridLayout * grid3 = new QGridLayout;
   //grid3->setMargin(0);
   grid2->addWidget(lblLang,        1, 0, Qt::AlignLeft);
   grid2->addWidget(cmbLang_,       1, 1, Qt::AlignLeft);
   grid->addLayout(grid2, row++, 0);

   
   grid->setColumnStretch(1, 1);
   //grid->addWidget(chkKeep_, row++, 0, 1, 2);

   vb->addStretch(1);
   setLayout(vb);

   //audioOutput_ = new Phonon::AudioOutput(this);
   //mediaObject_ = new Phonon::MediaObject(this);
   //Phonon::createPath(mediaObject_, audioOutput_);

}

void SettingsPage::testTurn()
{
   //mediaObject_->setCurrentSource(QString(":/sounds/turn.wav"));
   //mediaObject_->play();

   QString appExe = qApp->applicationDirPath();
   QSound::play(appExe + "/sounds/turn.wav");
}

void SettingsPage::testFold()
{
   QString appExe = qApp->applicationDirPath();
   QSound::play(appExe + "/sounds/fold.wav");
}

void SettingsPage::testCheck()
{
   QString appExe = qApp->applicationDirPath();
   QSound::play(appExe + "/sounds/check.wav");
}

void SettingsPage::turnSwitch(bool tg)
{
   btnTurn_->setEnabled(tg);
}

void SettingsPage::foldSwitch(bool tg)
{
   btnFold_->setEnabled(tg);
}

void SettingsPage::checkSwitch(bool tg)
{
   btnCheck_->setEnabled(tg);
}

QVariant SettingsPage::value() const
{
   QString regKey = QDir::current().dirName();
   QSettings settings(regKey, "Config");

   QVariant sett = settings.value("Settings");
   SettingsData data = qvariant_cast<SettingsData>(sett);
   
   data.checkBeep = chkCheck_->isChecked();
   data.foldBeep = chkFold_->isChecked();
   data.turnBeep = chkTurn_->isChecked();
   data.visualAlert = chkVisual_->isChecked();
   data.showFolded = chkShowFolded_->isChecked();
   data.advisorMode = chkAdvisor_->isChecked();
   data.interval = static_cast<int>( spnInterval_->value() * 1000. );
   data.lang = cmbLang_->itemData(cmbLang_->currentIndex()).toString();
   data.room = cmbRoom_->itemData(cmbRoom_->currentIndex()).toString();
   return QVariant::fromValue<SettingsData>(data);
}

void SettingsPage::setValue(const QVariant & value)
{
   SettingsData data = qvariant_cast<SettingsData>(value);
   if (data.interval > 5000 || data.interval < 100)
   {
      data.turnBeep = true;
      data.checkBeep = true;
      data.foldBeep = true;
      data.visualAlert = true;
      data.interval = 1000;
      data.lang = "";
#ifdef DEMO_MODE      
      data.room = "cake";
#else
      data.room = "party";
#endif
      data.showFolded = true;
      data.advisorMode = false;
   }
   if (data.lang.isEmpty())
   {
      data.lang = "en";
      QString suffix = QLocale::system().name();
      suffix.truncate(2);
      
      if (cmbLang_->findData(suffix) != -1)
         data.lang = suffix;
   }

   chkCheck_->setChecked(data.checkBeep);
   chkFold_->setChecked(data.foldBeep);
   chkTurn_->setChecked(data.turnBeep);
   chkVisual_->setChecked(data.visualAlert);
   chkShowFolded_->setChecked(data.showFolded);
   chkAdvisor_->setChecked(data.advisorMode);
   spnInterval_->setValue(static_cast<double>(data.interval / 1000.));
   cmbLang_->setCurrentIndex(cmbLang_->findData(data.lang));
   
   int idx = cmbRoom_->findData(data.room);
   if (idx == -1)
      idx = 0;
   cmbRoom_->setCurrentIndex(idx);
}

