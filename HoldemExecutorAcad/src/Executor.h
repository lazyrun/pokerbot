#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <windows.h>
#include "SettingsData.h"
#include "acadlib.h"

class ProcAcad;
class CardBase;
class AlarmWidget;
//class DBManager;

class Executor : public QObject
{
Q_OBJECT
public:
   Executor(showMessageCBFunc fun, QObject * parent = 0);
   Executor(QObject * parent = 0);
   ~Executor();
public slots:
   void start();
   void stop();
   void exit();
protected:
   int timer_id_;
   int interval_;
   int screen_res_x_;
   int screen_res_y_;

   QString cache_;
   ProcAcad * cardProc_;
   //DBManager * dbManager_;
   CardBase * cardBase_;
   QStringList playingCard_;
   SettingsData data_;
   AlarmWidget * alarm_;
   bool lastIsFold_;
protected:
   void init();
   //таймер
   void timerEvent(QTimerEvent * te);
   HWND findTables(const QString & tClass, HWND BeginHandle);
   void HwndToTop(WId hwnd);
   void clickCheck(WId hwnd);
   void clickFold(WId hwnd);
   void clickTo(WId hwnd, const QRect & rect);
   void clickTo(WId hwnd, int x, int y);
   void foldOrCheck(WId hwnd, bool realClick = true);
   //QString cardFromHash(unsigned long long hash);
   QString cardFromImage(QImage & img);
   QString cardString(int nom, int suit);
   QString cardRangeFromHoles(const QString & first, const QString & second);
};

//
class TimerKiller
{
public:
   TimerKiller(QObject * obj, int & tid, int interval)
      : interval_(interval), timer_id_(&tid), obj_(obj)
   {
      obj_->killTimer(tid);
      resp_ = true;
   }
   ~TimerKiller()
   {
      if (resp_)
         (*timer_id_) = obj_->startTimer(interval_);
   }
   void kill() { resp_ = false; }
protected:
   int interval_;
   int * timer_id_;
   QObject * obj_;
   bool resp_;
};

#endif
