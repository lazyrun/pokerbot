#ifndef ORDERPAGE_H
#define ORDERPAGE_H

class EnterKeyDlg;
class OrderPage : public QScrollArea
{
Q_OBJECT
public:
   OrderPage(QWidget * parent = 0);
protected slots:
   void buynow();
   void enterKey();
signals:
   void unlock();
protected:
   void setupUi();
protected:
   QLabel * lblUnreg_;
   QLabel * lblRemains_;
   EnterKeyDlg * enterDlg_;
   QString addr_;
   QString addr_ru_;
};

class EnterKeyDlg : public QDialog
{
Q_OBJECT
public:
   EnterKeyDlg(QWidget * parent = 0);
protected slots:
   void accept();
   void keyFail();
   void keySuccess();
protected:
   void setupUi();
protected:
   QLineEdit * edtName_;
   QLineEdit * edtKey_;
};

#endif
