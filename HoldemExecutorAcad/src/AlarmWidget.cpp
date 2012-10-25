#include <qt_windows.h>
#include "AlarmWidget.h"

AlarmWidget::AlarmWidget()
: QWidget(0, Qt::SplashScreen | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
{
   setWindowOpacity(0.5);
   setMouseTracking(true);
}

void AlarmWidget::paintEvent(QPaintEvent * )
{
   QPainter painter(this);
   QPen pen;
   pen.setWidth(3);
   pen.setColor(Qt::black);
   painter.setPen(pen);

   painter.setFont(QFont("Arial", 30));
   painter.drawText(rect(), Qt::AlignCenter, text_);   

   QRect r = rect();
   r.setLeft(r.left() + 1);
   r.setTop(r.top() + 1);
   r.setWidth(r.width() - 2);
   r.setHeight(r.height() - 2);
   painter.drawRect(r);
}

bool AlarmWidget::event(QEvent * ev)
{
   if (ev->type() == QEvent::MouseMove ||
       ev->type() == QEvent::MouseButtonPress ||
       ev->type() == QEvent::MouseButtonRelease ||
       ev->type() == QEvent::MouseButtonDblClick )	
   {
      QMouseEvent * mev = static_cast<QMouseEvent *>(ev);
      if (mev)
      {
         QRegion newmask = QRegion(this->rect()) - QRegion(mev->x(), 
            mev->y(), 1, 1);
         setMask(newmask);
      }
   }		
   return QWidget::event(ev);
}

void AlarmWidget::highlight(const QString & text, WId wid)
{
   text_ = text;
   wid_ = wid;
   tid_ = startTimer(500);
}

void AlarmWidget::timerEvent(QTimerEvent *)
{
   RECT rect;
   GetWindowRect(wid_, &rect);

   move(rect.left, rect.top);
   resize(rect.right - rect.left, rect.bottom - rect.top);

   static bool shown = true;
   if (shown)
      show();
   else
      hide();
   shown = !shown;
   
   static int cnt;
   cnt++;
   if (cnt >= 4)
   {
      killTimer(tid_);
      cnt = 0;
   }
}
