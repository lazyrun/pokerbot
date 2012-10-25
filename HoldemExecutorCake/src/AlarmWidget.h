#ifndef ALARMWIDGET_H
#define ALARMWIDGET_H

class AlarmWidget : public QWidget
{
   Q_OBJECT
public:
   AlarmWidget();
   void highlight(const QString & text, WId wid);
protected:
   void paintEvent(QPaintEvent * p);
   bool event(QEvent * ev);
   void timerEvent(QTimerEvent *);
protected:
   WId wid_;
   QString text_;
   int tid_;
}; 


#endif
