#ifndef SUPPORTPAGE_H
#define SUPPORTPAGE_H

#include <QtGui>

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

class EulaWidget : public QWidget
{
Q_OBJECT
public:
   EulaWidget(QWidget * parent = 0);
protected:
   void setupUi();
};

//class VideoPlayer;
//
//class VideoWidget : public QWidget
//{
//Q_OBJECT
//public:
//   VideoWidget(QWidget * parent = 0);
//protected:
//   void setupUi();
//protected slots:
//   void tutorial1();
//   void tutorial2();
//protected:
//   VideoPlayer * videoPlayer_;
//};


#endif
