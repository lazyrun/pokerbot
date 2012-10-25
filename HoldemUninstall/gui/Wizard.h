#ifndef WIZARD_H
#define WIZARD_H

#include <QWizard>
#include <QMap>

class QRadioButton;
class QProgressBar;
class QLabel;
class QCheckBox;


class Wizard : public QWizard
{
   Q_OBJECT
public:
   enum PageType
   {
      Intro    = 0,
      Progress = 4, 
      Finish   = 5
   };
   Wizard(QWidget * parent = 0);

protected:
   void createIntroStep();
   void createFinishStep();
   void createProgressStep();
   virtual void reject();
   QMap<PageType, QWizardPage *> pages_;
};

class ProgressPage : public QWizardPage
{
Q_OBJECT
public:
   ProgressPage(QWidget * parent = 0);
   void setPause(bool);

   virtual bool	isComplete () const;
   virtual void	initializePage ();   
   //virtual void	cleanupPage ();
signals:
   void installed(const QString &, const QString &);

protected slots:
   void uninstall();
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
   void setSetupFolder(const QString & path);
   void setSetupFullPath(const QString & path);

   virtual void initializePage ();
   virtual bool validatePage ();
protected:
   QString fullPath_;
   QString folder_;
   QLabel * lblTo_;
   QCheckBox * checkCanny_;
   QCheckBox * checkDesk_;
   QCheckBox * checkStart_;
   QCheckBox * checkRemove_;
};

#endif
