#ifndef KEYPROC_H
#define KEYPROC_H

class AbstractProc : public QObject
{
Q_OBJECT
public:
   AbstractProc(){}
   virtual void checkKey(const QString & name, const QString & key) = 0;
   virtual QString hash(const QString & name) = 0;
public slots:
   virtual void write(const QString & name, const QString & key) = 0;
protected slots:
   virtual void checkAgain(const QString & name, const QString & key) = 0;
signals:
   void isOk(const QString & name, const QString & key);
   void notOk();
};

class AcadProc : public AbstractProc
{
   Q_OBJECT
public:
   AcadProc();
   virtual void checkKey(const QString & name, const QString & key);
   virtual QString hash(const QString & name);
signals:
   void check(const QString & name, const QString & key);

protected slots:
   virtual void checkAgain(const QString & name, const QString & key);
public slots:   
   virtual void write(const QString & name, const QString & key);
};


#endif
