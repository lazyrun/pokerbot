#include <shlobj.h>

#include <QtGui>
#include "Wizard.h"
#include "SettingsData.h"

static int removeFolder(QDir & dir);
static int clearFolder(QDir & dir);
static void selfDel();

BOOL IsUserAdmin( PBOOL pbAdmin )
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

Wizard::Wizard(QWidget * parent)
: QWizard(parent, Qt::Dialog | Qt::WindowSystemMenuHint)
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

   //Транслятор
   QTranslator * appTranslator = new QTranslator(this);
   appTranslator->load("HoldemUninstall_" + suffix, ":/translations");
   qApp->installTranslator(appTranslator);

   QTranslator * qtTranslator = new QTranslator(this);
   qtTranslator->load("qt_" + suffix, ":/translations");
   qApp->installTranslator(qtTranslator);

   BOOL IsAdmin = FALSE;
   BOOL success = IsUserAdmin(&IsAdmin);
   if (success && !IsAdmin)
   {
      //не админские права
      QMessageBox::warning(this, tr("Maverick Uninstall"), 
         tr("You must have administrative privileges to uninstall the program.\n"
         "Please run Uninstall with administrative privileges."));
      exit(0);
   }

   createIntroStep();
   createProgressStep();
   createFinishStep();
   setWindowTitle(tr("Uninstall - Maverick Poker Bot"));
   setFixedWidth(550);
   setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/water.png"));
   setOption(QWizard::NoBackButtonOnLastPage, true);
}

void Wizard::createIntroStep()
{
   QWizardPage * wp = new QWizardPage(this);
   wp->setTitle(tr("Uninstall"));
   QVBoxLayout * vl = new QVBoxLayout;
   QLabel * lbl = new QLabel(tr("Are you sure you want to completelly remove Maverick Poker Bot and all of its components?<p>Click Next to continue, or Cancel to exit."), this);
   lbl->setWordWrap(true);
   vl->addWidget(lbl);
   wp->setLayout(vl);
   addPage(wp);
   pages_.insert(Intro, wp);
}

void Wizard::createProgressStep()
{
   ProgressPage * pp = new ProgressPage(this);
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


void Wizard::reject()
{
   ProgressPage * pp = qobject_cast<ProgressPage *>(pages_[Progress]);
   if (pp)
   {
      pp->setPause(true);
   }
   
   hide();
   QDialog::reject();
}


// ProgressPage
ProgressPage::ProgressPage(QWidget * parent)
: QWizardPage(parent)
{
   setTitle(tr("Remove Maverick Poker Bot"));
   QVBoxLayout * vb = new QVBoxLayout;
   QLabel * lbl = new QLabel(tr("Uninstalling"), this);
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
   QTimer::singleShot(100, this, SLOT(uninstall()));
}

void ProgressPage::uninstall()
{
   //wizard()->next();
   //return;

   wizard()->button(QWizard::BackButton)->setEnabled(false);
   QString appDir = qApp->applicationDirPath();
   
   //удалить файлы
   QDir adir(appDir);
   QString baseDir = adir.dirName();
   
   //удалить из пуск и из старт
   bar_->setValue(33);

   //путь до рабочего стола
   //QString deskPath = QDesktopServices::storageLocation(
   //   QDesktopServices::DesktopLocation);

   wchar_t sPath[MAX_PATH] = {'\0'};
   LPWSTR wStartPath = (LPWSTR) sPath;
   SHGetFolderPath(0, CSIDL_COMMON_PROGRAMS, 0, SHGFP_TYPE_CURRENT, wStartPath);
    
   wchar_t dPath[MAX_PATH] = {'\0'};
   LPWSTR wDeskPath = (LPWSTR) dPath;
   SHGetFolderPath(0, CSIDL_COMMON_DESKTOPDIRECTORY, 0, SHGFP_TYPE_CURRENT, wDeskPath);

   QString deskPath = QString::fromWCharArray(dPath);
   QString appPath = QString::fromWCharArray(sPath);

   QFile::remove(deskPath + "\\" + baseDir + ".lnk");
   
   bar_->setValue(50);
   //QString appPath = QDesktopServices::storageLocation(
   //   QDesktopServices::ApplicationsLocation);

   QDir startDir(appPath + "\\" + baseDir);
   removeFolder(startDir);

   bar_->setValue(66);
   clearFolder(adir);

   bar_->setValue(100);
   
   wizard()->next();
}

void ProgressPage::setPause(bool p)
{
   paused_ = p;
}

// Finish Page

FinishPage::FinishPage(QWidget * parent)
: QWizardPage(parent)
{
   setTitle(tr("Removed"));
   QVBoxLayout * vb = new QVBoxLayout;
   QLabel * lbl = new QLabel(tr("Maverick Poker Bot was successfully removed from your computer."), this);

   vb->addWidget(lbl);
   vb->addStretch(1);
   setLayout(vb);
}

void FinishPage::initializePage()
{
   wizard()->button(QWizard::CancelButton)->setEnabled(false);
   selfDel();
}

bool FinishPage::validatePage ()
{
   return true;
}

void selfDel()
{
   QString tempDir = QDir::tempPath();
   QString temp = QDir::toNativeSeparators(tempDir + "/delhf.bat");
   tempDir = QDir::toNativeSeparators(tempDir);
   QString appDir = qApp->applicationDirPath();
   appDir = QDir::toNativeSeparators(appDir);
   QFile file(temp);
   if (file.open(QIODevice::WriteOnly | QIODevice::Text))
   {
      QString filePath = qApp->applicationFilePath();
      QString corePath = qApp->applicationDirPath() + "/QtCore4.dll";
      QString guiPath = qApp->applicationDirPath() + "/QtGui4.dll";
      QString xmlPath = qApp->applicationDirPath() + "/QtXml4.dll";
      QString netPath = qApp->applicationDirPath() + "/QtNetwork4.dll";
      QString msvcpPath = qApp->applicationDirPath() + "/msvcp90.dll";
      QString msvcrPath = qApp->applicationDirPath() + "/msvcr90.dll";

      filePath = QDir::toNativeSeparators(filePath);
      corePath = QDir::toNativeSeparators(corePath);
      guiPath = QDir::toNativeSeparators(guiPath);
      xmlPath = QDir::toNativeSeparators(xmlPath);
      netPath = QDir::toNativeSeparators(netPath);
      msvcpPath = QDir::toNativeSeparators(msvcpPath);
      msvcrPath = QDir::toNativeSeparators(msvcrPath);

      QTextStream out(&file);
      out << ":Repeat" << endl;
      out << QString("del \"%1\" ").arg(filePath) << endl;
      out << QString("del \"%1\" ").arg(corePath) << endl;
      out << QString("del \"%1\" ").arg(guiPath) << endl;
      out << QString("del \"%1\" ").arg(xmlPath) << endl;
      out << QString("del \"%1\" ").arg(netPath) << endl;
      out << QString("del \"%1\" ").arg(msvcpPath) << endl;
      out << QString("del \"%1\" ").arg(msvcrPath) << endl;

      out << QString("del \"%1\"").arg(tempDir + "\\delhf.js") << endl;
      out << QString("if exist \"%1\" goto Repeat").arg(filePath) << endl;
      out << QString("if exist \"%1\" goto Repeat").arg(corePath) << endl;
      out << QString("if exist \"%1\" goto Repeat").arg(guiPath) << endl;
      out << QString("if exist \"%1\" goto Repeat").arg(xmlPath) << endl;
      out << QString("if exist \"%1\" goto Repeat").arg(netPath) << endl;
      out << QString("if exist \"%1\" goto Repeat").arg(msvcpPath) << endl;
      out << QString("if exist \"%1\" goto Repeat").arg(msvcrPath) << endl;

      out << QString("if exist \"%1\" goto Repeat").arg(tempDir + "\\delhf.js") << endl;
      out << QString("rmdir \"%1\"").arg(appDir) << endl;
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
         
         QDir::setCurrent("C:\\");
         QDesktopServices::openUrl(
            QUrl(QString("file:///%1").arg(tempDir + "\\delhf.js"), 
            QUrl::TolerantMode));
      }
   }
}

//Функция очистки папки
int clearFolder(QDir & dir)
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

  return res;
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
