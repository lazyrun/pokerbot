#ifndef WIZARD_H
#define WIZARD_H

class LineEdit;

struct HoldemNames
{
   QString path;
   QString exePath;
   QString agent;
   QString uninstall;
   QString acad;
   QString rsp1;
};

class Wizard : public QWizard
{
   Q_OBJECT
public:
   enum PageType
   {
      Intro    = 0,
      Folder   = 1,
      Room     = 2,
      License  = 3,
      Progress = 4, 
      Finish   = 5
   };
   Wizard(QWidget * parent = 0);

protected slots:
   void folderSelected(const QString &);
   void installed(const HoldemNames & hns);
   void getFullVersion();
protected:
   void createIntroStep();
   void createFolderStep();
   void createRoomStep();
   void createFinishStep();
   void createLicenseStep();
   void createProgressStep();
   void closeEvent(QCloseEvent *event);
   virtual void reject();
   QMap<PageType, QWizardPage *> pages_;
};

class LicensePage : public QWizardPage
{
Q_OBJECT
public:
   LicensePage(QWidget * parent = 0);
   virtual void	initializePage ();   
   virtual bool	isComplete () const;
protected slots:
   void agreed(bool);
protected:
   bool isAgreed_;
   QRadioButton * radYes_;
   QRadioButton * radNo_;
};

class FolderPage : public QWizardPage
{
Q_OBJECT
public:
   FolderPage(QWidget * parent = 0);
   virtual bool validatePage();
signals:
   void folderSelected(const QString &);
protected slots:
   void browseDir();
   void aliased(const QString &);
   void random();
protected:
   QLineEdit * lePath_;
   QLineEdit * edtAlias_;
   QString folderPrefix_;
};

class ProgressPage : public QWizardPage
{
Q_OBJECT
public:
   ProgressPage(QWidget * parent = 0);
   void setSetupFolder(const QString & path);
   void setPause(bool);

   virtual bool	isComplete () const;
   virtual void	initializePage ();   
   virtual void	cleanupPage ();
   //virtual bool validatePage();
signals:
   void installed(const HoldemNames & hns);
protected slots:
   void extractZip();
protected:
   QString folder_;
   QString fullPath_;
   QString cannyBotName_;
   QProgressBar * bar_;
   bool cancelled_;
   bool paused_;
};

class FinishPage : public QWizardPage
{
Q_OBJECT
public:
   FinishPage(QWidget * parent = 0);
   //void setSetupFolder(const QString & path);
   //void setSetupFullPath(const QString & path);

   void setHoldemNames(const HoldemNames & hns);
   virtual void initializePage ();
   virtual bool validatePage ();
protected:
   //QString fullPath_;
   //QString folder_;
   QLabel * lblTo_;
   QCheckBox * checkCanny_;
   QCheckBox * checkDesk_;
   //QCheckBox * checkStart_;
   QCheckBox * checkRemove_;
   HoldemNames hns_;
};


class LineEditStyle : public QProxyStyle
{
public:
  LineEditStyle(QStyle *style = 0) : QProxyStyle(style) { }
  int pixelMetric(PixelMetric metric, 
     const QStyleOption *option = 0, 
     const QWidget *widget = 0) const;

 };

class LineEdit : public QLineEdit
{
   Q_OBJECT
public:
   LineEdit(QWidget *parent = 0);
};

#endif
