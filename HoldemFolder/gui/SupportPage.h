#ifndef SUPPORTPAGE_H
#define SUPPORTPAGE_H

class SupportPage : public QGroupBox
{
Q_OBJECT
public:
   SupportPage(QWidget * parent = 0);
protected:
   void setupUi();
};

class SupportWidget : public QWidget
{
Q_OBJECT
public:
   SupportWidget(QWidget * parent = 0);
protected:
   void setupUi();
};

class HelpWidget : public QWidget
{
Q_OBJECT
public:
   HelpWidget(QWidget * parent = 0);
protected:
   void setupUi();
};

class RoomWidget : public QTabWidget
{
Q_OBJECT
public:
   RoomWidget(QWidget * parent = 0);
protected:
   void setupUi();
};

class EulaWidget : public QWidget
{
Q_OBJECT
public:
   EulaWidget(QWidget * parent = 0);
protected:
   void setupUi();
};

#endif
