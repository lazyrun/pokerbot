#include <shlobj.h>

#include "Wizard.h"
#include "unzip.h"

static int removeFolder(QDir & dir);
static void selfDel();
static QString generateKey();
static QString randomName();

static LPCTSTR ConvertToLPCTSTR(const QString & str)
{
   LPCTSTR lstr = (LPCTSTR)str.unicode();
   return lstr;
}

static HRESULT CreateLink(LPCTSTR lpszPathObj, LPCTSTR lpszPathLink,
                   LPCTSTR pszArgs, LPCTSTR lpszDesc, LPCTSTR lpszWdir)
{    
   HRESULT h_result;    
   IShellLink* psl;     
   CoInitialize( NULL );    
   h_result = CoCreateInstance(CLSID_ShellLink, NULL,    
      CLSCTX_INPROC_SERVER, 
      IID_IShellLink, 
      (LPVOID *) &psl);    
   if (SUCCEEDED(h_result))
   {        
      IPersistFile* ppf;         
      psl->SetPath(lpszPathObj);        
      psl->SetArguments(pszArgs);        
      psl->SetDescription(lpszDesc);         
      psl->SetWorkingDirectory(lpszWdir);
      h_result = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);         
      if (SUCCEEDED(h_result))
      {            
         h_result = ppf->Save(lpszPathLink, TRUE);
         ppf->Release();        
      }        
      psl->Release();    
   }    
   return h_result;
}

BOOL IsUserAdmin(PBOOL pbAdmin)
{
   PSID   psidAdministrators = NULL;
   SID_IDENTIFIER_AUTHORITY siaNtAuthority = SECURITY_NT_AUTHORITY;
   BOOL   bResult = FALSE;
    
   // инициализация идентификатора безопасности
   if( !AllocateAndInitializeSid( 
      &siaNtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
      DOMAIN_ALIAS_RID_ADMINS, 0,0,0,0,0,0, &psidAdministrators ) )
         return FALSE;

   // код для Windows2000
   bResult = CheckTokenMembership( NULL, psidAdministrators, pbAdmin );            
    

   if( psidAdministrators )
      FreeSid( psidAdministrators );

   return bResult;
}

static QString randomName()
{
   QString k = generateKey();
   if (k.isEmpty())
      return QString();

   UnZip::ErrorCode ec;
   UnZip uz;
   uz.setPassword(k);
   ec = uz.openArchive(":/images/rw.png");
   
   if (ec != UnZip::Ok)
   {
      return QString();
   }
   QBuffer *buffer = new QBuffer();
   buffer->open(QIODevice::WriteOnly);
   ec = uz.extractFile("dat/rw", buffer);
   QString res;
   if (ec == UnZip::Ok)
   {
      QByteArray ba = buffer->data();
      QString str(ba);
      QStringList lst = str.split('\n');
      if (lst.length() > 2)
      {
         int len = lst.length();
         //qsrand(QDateTime::currentDateTime().toTime_t());
         int idx1 = qrand() % len;
         res = lst.at(idx1).trimmed();
      }
   }
   uz.closeArchive();
   delete buffer;
   
   return res;
}

static QString generateKey()
{
   //взять хэш от файла
   //0ee4be54e5285ed791dd02b3d4a8ae7f
   QFile file(":/images/poker.png");
   QString shash;
   if (file.open(QIODevice::ReadOnly))
   {
      QCryptographicHash hash(QCryptographicHash::Md5);
      hash.addData(file.readAll());
      QByteArray ba = hash.result().toHex();
      shash = QString(ba);
      file.close();
   }
   return shash;
}

Wizard::Wizard(QWidget * parent)
: QWizard(parent, Qt::Dialog | Qt::WindowSystemMenuHint)
{
   //Транслятор
   QTranslator * appTranslator = new QTranslator(this);
   appTranslator->load(QString(":/translations/HoldemInstall_%1")
      .arg(QLocale::system().name()));
   qApp->installTranslator(appTranslator);

   QTranslator * qtTranslator = new QTranslator(this);
   qtTranslator->load(QString(":/translations/qt_%1")
      .arg(QLocale::system().name()));
   qApp->installTranslator(qtTranslator);

   BOOL IsAdmin = FALSE;
   BOOL success = IsUserAdmin(&IsAdmin);
   if (success && !IsAdmin)
   {
      //не админские права
      QMessageBox::warning(this, tr("Maverick Setup"), 
         tr("You must have administrative privileges to install the program.\n"
         "Please run the installer with administrative privileges."));
      exit(0);
   }
   qsrand(QDateTime::currentDateTime().toTime_t());
   createIntroStep();
   createLicenseStep();
   createFolderStep();
   //createRoomStep();
   createProgressStep();
   createFinishStep();
   setWindowTitle(tr("Setup - Maverick Poker Bot"));
   setFixedWidth(550);
   setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/water.png"));
   setOption(QWizard::NoBackButtonOnLastPage, true);
   //setWizardStyle(QWizard::ClassicStyle);
}

void Wizard::createIntroStep()
{
   QWizardPage * wp = new QWizardPage(this);
   wp->setTitle(tr("Welcome"));
   QVBoxLayout * vl = new QVBoxLayout;
   QLabel * lbl = new QLabel(tr("<h3>Welcome to the Maverick Setup Wizard.</h3><p><p><p>This will install Maverick Poker Bot on your computer.<p>It is recommended that you close all <b>Poker Rooms</b> before continuing.<p>Click Next to continue, or Cancel to exit Setup."), this);
   lbl->setWordWrap(true);
   vl->addWidget(lbl);

#ifdef DEMO_MODE
   QHBoxLayout * hb = new QHBoxLayout;
   QLabel * lblDemo = new QLabel(tr("<br><br><font color=red><h3>Demo Version</h3><p>Available poker-rooms:<br>Red Star Poker, Poker Academy Pro only.</font>"), this);
   lblDemo->setWordWrap(true);
   hb->addWidget(lblDemo);
   
   QPushButton * btnFull = new QPushButton(tr("&Get Full Version\n(Party Poker, Titan Poker, Cake Poker)"), this);
   btnFull->setFixedHeight(64);
   connect(btnFull, SIGNAL(clicked()), this, SLOT(getFullVersion()));
   hb->addWidget(btnFull, 0, Qt::AlignBottom);
   vl->addLayout(hb);
#endif
   wp->setLayout(vl);
   addPage(wp);
   pages_.insert(Intro, wp);
}

void Wizard::getFullVersion()
{
   QString addr = "https://www.plimus.com/jsp/buynow.jsp?contractId=2293801";
   QDesktopServices::openUrl(QUrl(addr, QUrl::TolerantMode));
}

void Wizard::createLicenseStep()
{
   LicensePage * lp = new LicensePage(this);
   addPage(lp);
   pages_.insert(License, lp);
}

void Wizard::createFolderStep()
{
   FolderPage * fp = new FolderPage(this);
   connect(fp, SIGNAL(folderSelected(const QString &)),
      this, SLOT(folderSelected(const QString &)));
   addPage(fp);
   pages_.insert(Folder, fp);
}

void Wizard::createProgressStep()
{
   ProgressPage * pp = new ProgressPage(this);
   connect(pp, SIGNAL(installed(const HoldemNames &)), 
      this, SLOT(installed(const HoldemNames &)));
   addPage(pp);
   pages_.insert(Progress, pp);
}

void Wizard::createFinishStep()
{
   FinishPage * fp = new FinishPage(this);
   
   fp->setFinalPage(true);
   addPage(fp);
   pages_.insert(Finish, fp);
}

//void Wizard::installed(const QString & folder, const QString & full)
void Wizard::installed(const HoldemNames & hns)
{
   FinishPage * fp = qobject_cast<FinishPage *>(pages_[Finish]);
   if (fp)
   {
      //fp->setSetupFolder(folder);
      //fp->setSetupFullPath(full);
      fp->setHoldemNames(hns);
   }
   next();

}

void Wizard::reject()
{
   ProgressPage * pp = qobject_cast<ProgressPage *>(pages_[Progress]);
   if (pp)
   {
      pp->setPause(true);
   }
   
   int res = QMessageBox::question(this, tr("Maverick Setup"), 
      tr("Do you want to cancel the Maverick Poker Bot Installation?"), 
      QMessageBox::Yes | QMessageBox::No);
   
   if (res == QMessageBox::Yes)
   {
      hide();
      QDialog::reject();
   }
   else
   {
      if (pp)
      {
         pp->setPause(false);
      }
   }
}

void Wizard::closeEvent(QCloseEvent *event)
{
   int res = QMessageBox::question(this, tr("Maverick Setup"), 
      tr("Do you want to cancel the Maverick Poker Bot Installation?"), 
      QMessageBox::Yes | QMessageBox::No);
   if (res == QMessageBox::Yes)
      event->accept();
   else
      event->ignore();
}

void Wizard::folderSelected(const QString & path)
{
   ProgressPage * pp = qobject_cast<ProgressPage *>(pages_[Progress]);
   if (pp)
   {
      pp->setSetupFolder(path);
   }
}

// LicensePage
LicensePage::LicensePage(QWidget * parent)
: QWizardPage(parent)
{
   isAgreed_ = false;
   setTitle(tr("License Agreement"));
   
   QVBoxLayout * vl = new QVBoxLayout;
   QTextEdit * plain = new QTextEdit(this);
   plain->setReadOnly(true);
   
   QString suffix;
   suffix = QLocale::system().name();
   suffix.truncate(2);
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
   radYes_ = new QRadioButton(tr("I &accept the agreement"), this);
   radNo_ = new QRadioButton(tr("I &don't accept the agreenemt"), this);
   connect(radYes_, SIGNAL(toggled(bool)), this, SLOT(agreed(bool)));

   QLabel * lbl = new QLabel(tr("Please read the following License Agreement. You must accept the terms of this agreement before continuing with the installation."), this);
   lbl->setWordWrap(true);
   vl->addWidget(lbl);
   vl->addWidget(plain);
   vl->addWidget(radYes_);
   vl->addWidget(radNo_);
   setLayout(vl);
}

void LicensePage::initializePage()
{
   radNo_->setChecked(true);
   isAgreed_ = false;
   emit completeChanged();
}

bool LicensePage::isComplete() const
{
   return isAgreed_;
}

void LicensePage::agreed(bool tg)
{
   isAgreed_ = tg;
   emit completeChanged();
}

//  FolderPage
FolderPage::FolderPage(QWidget * parent)
: QWizardPage(parent)
{
   setTitle(tr("Alias Selection"));
   QGridLayout *grLayout = new QGridLayout();

   QLabel * lblAlias = new QLabel(tr("You should specify an alias for Maverick Poker Bot in this step. Memorize the selected alias, you will need it while using Maverick Poker Bot. You can specify the alias yourself or you can use the \"Random\" button for the installation wizard to offer you some random value."), this);
   lblAlias->setWordWrap(true);

   edtAlias_ = new QLineEdit(this);
   edtAlias_->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9 ]+"), this));
   edtAlias_->setAlignment(Qt::AlignCenter);
   QFont fn = edtAlias_->font();
   fn.setBold(true);
   fn.setPointSize(24);
   edtAlias_->setFont(fn);
   edtAlias_->setMaxLength(120);
   connect(edtAlias_, SIGNAL(textChanged(const QString &)), 
      this, SLOT(aliased(const QString &)));
   
   //QLabel * lbl = new QLabel(tr("Select the folder where you would like Holdem Folder to be installed, then click Next.<p>The program requires at least 10 MB of disk space."), this);
   //lbl->setWordWrap(true);

   QLabel * lblPath = new QLabel(tr("&Path to Maverick Poker Bot:"), this);
   
   lePath_ = new QLineEdit(this);
   lePath_->setText("C:\\Program Files\\");
   lePath_->setEnabled(false);
   lblPath->setBuddy(lePath_);
   folderPrefix_ = "C:\\Program Files\\";

   QToolButton * btnRandom = new QToolButton(this);
   btnRandom->setIcon(QIcon(":/images/dice.png")); 
   btnRandom->setText(tr("&Random")); 
   btnRandom->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
   QPushButton * btnBrowse = new QPushButton(tr("&Browse..."), this);
   //btnBrowse->setFixedSize(20, 20);
   connect(btnBrowse, SIGNAL(clicked()), this, SLOT(browseDir()));
   connect(btnRandom, SIGNAL(clicked()), this, SLOT(random()));
   btnRandom->setMinimumHeight(edtAlias_->sizeHint().height());
   btnRandom->setMinimumWidth(btnBrowse->sizeHint().width());
   btnRandom->setIconSize(QSize(32, 32));
   
   int row = 0;
   //grLayout->addWidget(lbl,    row++, 0, 1, 2);

   //grLayout->addItem(new QSpacerItem(1, 20,
   //   QSizePolicy::Fixed, QSizePolicy::Fixed), row++, 0, 1, 2);
   grLayout->addWidget(lblAlias,  row++, 0, 1, 1);
   grLayout->addWidget(edtAlias_,  row, 0, 1, 1);
   grLayout->addWidget(btnRandom, row++, 1, 1, 1, Qt::AlignCenter);

   grLayout->addItem(new QSpacerItem(1, 20,
      QSizePolicy::Fixed, QSizePolicy::Fixed), row++, 0, 1, 2);

   grLayout->addWidget(lblPath,   row++, 0, 1, 2);
   grLayout->addWidget(lePath_,    row,   0, 1, 1);
   grLayout->addWidget(btnBrowse, row++, 1, 1, 1, Qt::AlignRight);

   setLayout(grLayout);
}

void FolderPage::aliased(const QString & alias)
{
   if (alias.isEmpty())
      lePath_->setText(folderPrefix_);
   else
      lePath_->setText(folderPrefix_ + alias + "\\");
}

void FolderPage::random()
{
   QString rand0 = randomName();
   QString rand1 = randomName();
   edtAlias_->setText(rand0 + " " + rand1);
}

void FolderPage::browseDir()
{
   QFileDialog::Options options = QFileDialog::DontResolveSymlinks |
      QFileDialog::ShowDirsOnly;

   QString directory = QFileDialog::getExistingDirectory(this,
                              tr("Select a folder"),
                              folderPrefix_,
                              options);
   if (!directory.isEmpty())
   {
      folderPrefix_ = QDir::toNativeSeparators(directory) + "\\";
      aliased(edtAlias_->text());
      //lePath_->setText(QDir::toNativeSeparators(directory));
   }
}

bool FolderPage::validatePage()
{
   if (edtAlias_->text().isEmpty())
   {
      QMessageBox::information(this, tr("Wrong Alias"), 
         tr("Please make up an alias for Maverick Poker Bot. You can use the \"Random\" button to generate a random alias automatically."));
      edtAlias_->setFocus();
      return false;
   }
   emit folderSelected(lePath_->text());
   return true;
}

// ProgressPage
ProgressPage::ProgressPage(QWidget * parent)
: QWizardPage(parent)
{
   setTitle(tr("Installation of Maverick Poker Bot"));
   QVBoxLayout * vb = new QVBoxLayout;
   QLabel * lbl = new QLabel(tr("Installing"), this);
   bar_ = new QProgressBar(this);
   vb->addWidget(lbl);
   vb->addWidget(bar_);
   vb->addStretch(1);
   setLayout(vb);
}

bool ProgressPage::isComplete () const
{
   return false;
}

void ProgressPage::initializePage ()
{
   cancelled_ = false;
   paused_ = false;
   //получить путь установки
   if (folder_.isEmpty())
   {
      QMessageBox::critical(this, tr("Setup Error"), 
         tr("There are an error during install.\nPlease contact to software developers.")); 
      exit(2);
      return;
   }
   QTimer::singleShot(100, this, SLOT(extractZip()));   
}

void ProgressPage::extractZip()
{
   wizard()->button(QWizard::BackButton)->setEnabled(false);
   //придумать случайное название 
   QString cannyFolder = randomName();
   if ((folder_.right(1) == "\\") || (folder_.right(1) == "/"))
   {
      folder_.chop(1);
   }
   fullPath_ = folder_;// + "\\" + cannyFolder;
   //QDir cannyDir(folder_);
   //cannyDir.mkdir(cannyFolder);
   if (!QDir().mkdir(folder_))
   {
      exit(1);
   }
   
   QDir fldr(folder_);
   cannyFolder = fldr.dirName();
   //разархивируем в каталог
   UnZip::ErrorCode ec;
   UnZip uz;
   ec = uz.openArchive(":/setup/setup.zip");
   
   if (ec != UnZip::Ok)
   {
      QMessageBox::critical(this, tr("Setup Error"), 
         tr("There are an error during install.\nPlease contact to software developers.")); 

      exit(3);
      return;
   }
   QList<UnZip::ZipEntry> list = uz.entryList();
   if (!list.isEmpty())
   {
      for (int i = 0; i < list.count(); i++)
      {
         while (paused_)
            qApp->processEvents();

         if (cancelled_)
         {
            uz.closeArchive();
            cancelled_ = false;
            return;
         }

         int percent = 100 * i / list.count();
         bar_->setValue(percent);
         qApp->processEvents();
         ec = uz.extractFile(list.at(i).filename, fullPath_);
         
         if (ec != UnZip::Ok)
         {
            QMessageBox::critical(this, tr("Setup Error"), 
               tr("There are an error during install.\nPlease contact to software developers.")); 
            uz.closeArchive();
            exit(4);
            return;
         }
      }
      bar_->setValue(100);
   }
   uz.closeArchive();

   //переименовать
   cannyBotName_ = fullPath_ + "\\" + cannyFolder + ".exe";
   if (!QFile::rename(fullPath_ + "\\HoldemFolder.exe", cannyBotName_))
   {
      QMessageBox::critical(this, tr("Setup Error"), 
            tr("There are an error during install.\nPlease contact to software developers.")); 
      exit(4);
      return;
   }
   
   QString agentName = fullPath_ + "\\" + randomName() + ".exe";
   if (!QFile::rename(fullPath_ + "\\HoldemAgent.exe", agentName))
   {
      QMessageBox::critical(this, tr("Setup Error"), 
            tr("There are an error during install.\nPlease contact to software developers.")); 
      exit(4);
      return;
   }

   QString uninstName = fullPath_ + "\\" + randomName() + ".exe";
   if (!QFile::rename(fullPath_ + "\\HoldemRemove.exe", uninstName))
   {
      QMessageBox::critical(this, tr("Setup Error"), 
            tr("There are an error during install.\nPlease contact to software developers.")); 
      exit(4);
      return;
   }

   QString acadName;
   if (QFile::exists(fullPath_ + "\\acad.dll"))
   {
      acadName = fullPath_ + "\\" + randomName() + ".dll";
      if (!QFile::rename(fullPath_ + "\\acad.dll", acadName))
      {
         QMessageBox::critical(this, tr("Setup Error"), 
               tr("There are an error during install.\nPlease contact to software developers.")); 
         exit(4);
         return;
      }
   }

   QString rsp1Name;
   if (QFile::exists(fullPath_ + "\\rsp1.dll"))
   {
      rsp1Name = fullPath_ + "\\" + randomName() + ".dll";
      if (!QFile::rename(fullPath_ + "\\rsp1.dll", rsp1Name))
      {
         QMessageBox::critical(this, tr("Setup Error"), 
               tr("There are an error during install.\nPlease contact to software developers.")); 
         exit(4);
         return;
      }
   }

   HoldemNames hns;
   hns.path = fullPath_;
   hns.exePath = cannyBotName_;
   hns.agent = agentName;
   hns.uninstall = uninstName;
   hns.acad = acadName;
   hns.rsp1 = rsp1Name;

   //запишем в реестр новые имена
   QSettings settings(cannyFolder, "Names");
   settings.setValue("acad", acadName);
   settings.setValue("rsp1", rsp1Name);
   settings.setValue("ag", agentName);
   
   emit installed(hns);
   //emit installed(fullPath_, cannyBotName_);
}

void ProgressPage::setSetupFolder(const QString & path)
{
   folder_ = path;
}

void ProgressPage::setPause(bool p)
{
   paused_ = p;
}

void ProgressPage::cleanupPage()
{
   if (!fullPath_.isEmpty())
   {
      removeFolder(QDir(fullPath_));
   }
   cancelled_ = true;
   paused_ = false;
}

// Finish Page

FinishPage::FinishPage(QWidget * parent)
: QWizardPage(parent)
{
   setTitle(tr("Completing the Maverick Setup Wizard"));
   QVBoxLayout * vb = new QVBoxLayout;
   QLabel * lbl = new QLabel(tr("Setup has finished installing Maverick Poker Bot on your computer. The application may be launched by selecting the installed icons."), this);
   lbl->setWordWrap(true);
   QLabel * lbl1 = new QLabel(tr("Maverick Poker Bot has been installed in the folder:"), this);
   lblTo_ = new QLabel(this);
   lblTo_->setWordWrap(true);
   //lblTo_->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
   //lblTo_->setMinimumHeight(24);
   //QLabel * lbl2 = new QLabel(tr("The shortcut with the same name was created on the Desktop.<p>This path was generated automatically for security purposes."), this);
   
   checkDesk_ = new QCheckBox(tr("Create a &desktop icon"), this);
   //checkStart_ = new QCheckBox(tr("Create a &folder in the Start menu"), this);
   checkCanny_ = new QCheckBox(tr("&Launch Maverick Poker Bot Configuration"), this);
   checkRemove_ = new QCheckBox(tr("Remove the &Installer of Maverick Poker Bot"), this);
   checkDesk_->setChecked(true);
   //checkStart_->setChecked(true);
   checkCanny_->setChecked(true);
   checkRemove_->setChecked(true);
   vb->addWidget(lbl);
   vb->addWidget(lbl1);
   vb->addWidget(lblTo_);
   //vb->addWidget(lbl2);
   vb->addSpacerItem(new QSpacerItem(1, 12));
   vb->addWidget(checkDesk_);
   //vb->addWidget(checkStart_);
   vb->addWidget(checkCanny_);
   vb->addWidget(checkRemove_);
   vb->addStretch(1);
   setLayout(vb);
}

//void FinishPage::setSetupFolder(const QString & path)
//{
//    folder_ = path;
//}
//
//void FinishPage::setSetupFullPath(const QString & path)
//{
//    fullPath_ = path;
//}

void FinishPage::setHoldemNames(const HoldemNames & hns)
{
   hns_ = hns;
}

void FinishPage::initializePage()
{
   lblTo_->setText("<b>" + hns_.path + "</b>");
}

bool FinishPage::validatePage ()
{
   qApp->setOverrideCursor(Qt::WaitCursor);
   QFileInfo fi(hns_.exePath);
   QString cannyFolder = fi.baseName();
   QString pFolder = fi.absolutePath();
   
   wchar_t sPath[MAX_PATH] = {'\0'};
   LPWSTR wStartPath = (LPWSTR) sPath;
   SHGetFolderPath(0, CSIDL_COMMON_PROGRAMS, 0, SHGFP_TYPE_CURRENT, wStartPath);
    
   wchar_t dPath[MAX_PATH] = {'\0'};
   LPWSTR wDeskPath = (LPWSTR) dPath;
   SHGetFolderPath(0, CSIDL_COMMON_DESKTOPDIRECTORY, 0, SHGFP_TYPE_CURRENT, wDeskPath);

   if (checkDesk_->isChecked())
   {
      //создать ярлык на рабочем столе
      QString linkName = cannyFolder + ".lnk";
      //путь до рабочего стола
      //QString deskPath = QDesktopServices::storageLocation(
      //   QDesktopServices::DesktopLocation);
      QString deskPath = QString::fromWCharArray(dPath);

      if (!((deskPath.right(1) == "\\") || (deskPath.right(1) == "/")))
      {
         deskPath += "\\";
      }
      linkName = deskPath + linkName;

      QFile::link(hns_.exePath, linkName);
   }

   //if (checkStart_->isChecked())
   {
      //создать папку в меню пусг
      //QString appPath = QDesktopServices::storageLocation(
      //   QDesktopServices::ApplicationsLocation);
      
      QString appPath = QString::fromWCharArray(sPath);
      QDir startDir(appPath);
      startDir.mkdir(cannyFolder);
      startDir.cd(cannyFolder);

      QString linkApp = startDir.absolutePath() + "/" + cannyFolder + ".lnk";
      QFile::link(hns_.exePath, linkApp);

      linkApp = startDir.absolutePath() + "/Uninstall.lnk";
      QFile::link(hns_.uninstall, linkApp);
   
      LPCTSTR lAgent = ConvertToLPCTSTR(hns_.agent);
      QString sDesc = "Run Agent";
      LPCTSTR lDesc = ConvertToLPCTSTR(sDesc);
      
      LPCTSTR lWdir = ConvertToLPCTSTR(hns_.path);

      if (!hns_.acad.isEmpty())
      {
         QFileInfo fi(hns_.acad);
         QString sAcad = "-\"" + fi.baseName() + "\"";
         LPCTSTR lAcad = ConvertToLPCTSTR(sAcad);
         linkApp = startDir.absolutePath() + "/Agent for Poker Academy.lnk";
         LPCTSTR lApp = ConvertToLPCTSTR(linkApp);
         CreateLink(lAgent, lApp, lAcad, lDesc, lWdir);
      }
      if (!hns_.rsp1.isEmpty())
      {
         QFileInfo fi(hns_.rsp1);
         QString sRsp1 = "-\"" + fi.baseName() + "\"";
         LPCTSTR lRsp1 = ConvertToLPCTSTR(sRsp1);
         linkApp = startDir.absolutePath() + "/Agent for Red Star Poker.lnk";
         LPCTSTR lApp = ConvertToLPCTSTR(linkApp);
         CreateLink(lAgent, lApp, lRsp1, lDesc, lWdir);
      }
   }

   if (checkCanny_->isChecked())
   {      
      //QProcess * proc = new QProcess(this);
      qApp->processEvents();
      if (QFile::exists(hns_.exePath))
      {
         QFileInfo fi(hns_.exePath);
         QDir::setCurrent(fi.absoluteDir().absolutePath());
         QProcess::startDetached("\"" + hns_.exePath + "\"");
      }
      qApp->processEvents();
      //proc->waitForStarted();
   }
   
   
   if (checkRemove_->isChecked())
   {
      //удалить себя
      selfDel();
   }
   qApp->restoreOverrideCursor();
   return true;
}

void selfDel()
{
   QString tempDir = QDir::tempPath();
   QString temp = QDir::toNativeSeparators(tempDir + "/delhf.bat");
   tempDir = QDir::toNativeSeparators(tempDir);
   
   QFile file(temp);
   if (file.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      QString filePath = qApp->applicationFilePath();
      filePath = QDir::toNativeSeparators(filePath);
      QTextStream out(&file);
      out << ":Repeat" << endl;
      out << QString("del \"%1\" ").arg(filePath) << endl;
      out << QString("del \"%1\"").arg(tempDir + "\\delhf.js") << endl;
      out << QString("if exist \"%1\" goto Repeat").arg(filePath) << endl;
      out << QString("if exist \"%1\" goto Repeat").arg(tempDir + "\\delhf.js") << endl;
      out << QString("del \"%1\"").arg(temp) << endl;

      file.close();
      
      //пишем скрипт
      QFile js(tempDir + "\\delhf.js");
      if (js.open(QIODevice::WriteOnly | QIODevice::Text))
      {
         temp.replace("\\", "\\\\");
         QTextStream out_js(&js);   
         out_js << "var activex = new ActiveXObject(\"WScript.Shell\");" << endl;
         out_js << QString("activex.Run(\"%1\", 0, true);").arg(temp) << endl;
         js.close();
         
         QDesktopServices::openUrl(
            QUrl(QString("file:///%1").arg(tempDir + "\\delhf.js"), 
            QUrl::TolerantMode));
         //QProcess::execute(tempDir + "\\delhf.js");
      }
      //var activex = new ActiveXObject("WScript.Shell");
      //activex.Run("path_to_example.bat", 0, true);

      //QProcess::startDetached(temp, QStringList() << "/f");
   }
}

//Функция удаления папки
int removeFolder(QDir & dir)
{
  int res = 0;
  //Получаем список каталогов
  QStringList lstDirs = dir.entryList(QDir::Dirs |
                  QDir::AllDirs |
                  QDir::NoDotAndDotDot);
  //Получаем список файлов 
  QStringList lstFiles = dir.entryList(QDir::Files);

  //Удаляем файлы
  foreach (QString entry, lstFiles)
  {
   QString entryAbsPath = dir.absolutePath() + "/" + entry;
   QFile::setPermissions(entryAbsPath, QFile::ReadOwner | QFile::WriteOwner);
   QFile::remove(entryAbsPath);
  }

  //Для папок делаем рекурсивный вызов  
  foreach (QString entry, lstDirs)
  {
   QString entryAbsPath = dir.absolutePath() + "/" + entry;
   QDir dr(entryAbsPath);
   removeFolder(dr);
  }

  //Удаляем обрабатываемую папку
  if (!QDir().rmdir(dir.absolutePath()))
  {
    res = 1;
  }
  return res;
}

int LineEditStyle::pixelMetric(PixelMetric metric, 
                               const QStyleOption *option, 
                               const QWidget *widget) const
{
  if (metric == QStyle::PM_TextCursorWidth)
    return 6;
  
  return QProxyStyle::pixelMetric(metric, option, widget);
}

LineEdit::LineEdit(QWidget *parent)
    : QLineEdit(parent)
{
  setStyle(new LineEditStyle(style()));
}
