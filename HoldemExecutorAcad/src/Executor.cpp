#include "Executor.h"
#include "ProcAcad.h"
#include "CardBase.h"
#include "Hooker.h"
#include "AlarmWidget.h"

#ifdef EXECUTOR_EXE
#include "qxtglobalshortcut.h"
#endif

#define MAX_WND_TEXT 1000
#define ABSOLUTE_COORD 65535

static showMessageCBFunc cbFun;

Executor::Executor(showMessageCBFunc fun, QObject * parent)
: QObject(parent)
{
   cbFun = fun;
   init();
}

Executor::Executor(QObject * parent)
: QObject(parent)
{
   init();
}

void Executor::init()
{
   qRegisterMetaType<SettingsData>("SettingsData");
   qRegisterMetaTypeStreamOperators<SettingsData>("SettingsData");

#ifdef EXECUTOR_EXE
   //установка глобальных хоткеев
   QxtGlobalShortcut * startShortcut = new QxtGlobalShortcut(this);
   startShortcut->setShortcut(QKeySequence("Ctrl+F5"));
   connect(startShortcut, SIGNAL(activated()), 
      this, SLOT(start()));

   QxtGlobalShortcut *stopShortcut = new QxtGlobalShortcut(this);
   stopShortcut->setShortcut(QKeySequence("Ctrl+F6"));
   connect(stopShortcut, SIGNAL(activated()), 
      this, SLOT(stop()));
   
   QxtGlobalShortcut *exitShortcut = new QxtGlobalShortcut(this);
   exitShortcut->setShortcut(QKeySequence("Ctrl+F10"));
   connect(exitShortcut, SIGNAL(activated()), 
      this, SLOT(exit()));
#endif

   screen_res_x_ = GetSystemMetrics(SM_CXSCREEN);//Получить ширину экрана
   screen_res_y_ = GetSystemMetrics(SM_CYSCREEN);//Получить высоту экрана

   cardBase_ = new CardBase();
   cardProc_ = new ProcAcad();
   alarm_ = new AlarmWidget();
}

Executor::~Executor()
{
   delete cardBase_;
   delete cardProc_;
   delete alarm_;
}

void Executor::start()
{
   //чтение настроек из конфигуратора
   //QSettings settings("Holdem Folder", "Config");
   QString regKey = QDir::current().dirName();
   QSettings settings(regKey, "Config");

   QString range = settings.value("CardRange").toString();
   playingCard_ = range.split(", ", QString::SkipEmptyParts);
   QVariant sett = settings.value("Settings");
   data_ = qvariant_cast<SettingsData>(sett);

   interval_ = data_.interval;

   lastIsFold_ = false;
   //запускаем таймер
   timer_id_ = startTimer(interval_);
}

void Executor::stop()
{
   if (timer_id_)
      killTimer(timer_id_);
}

void Executor::exit()
{
   QCoreApplication::quit();
}

void Executor::timerEvent(QTimerEvent *)
{
   TimerKiller killer(this, timer_id_, interval_);
   
   //ищем топовое окно заданного класса
   static const QString psClass = "SunAwtFrame";
   static const int minFoldHue = 13;
   static const int maxFoldHue = 33;
   static const int maxFoldBlack = 400;

   //смотрим топовое окно
   WId FgWnd = GetForegroundWindow();
   char clName[MAX_WND_TEXT];
   GetClassNameA(FgWnd, clName, MAX_WND_TEXT);
   QString sClass(clName);
   if (sClass != psClass)
   {
      return;
   }

   //возьмем кнопку фолд
   static int foldRect_x = 160;
   static int foldRect_y = 655;
   static int foldRect_sz = 16;
   //делаем скрин области кнопок
   
   HwndToTop(FgWnd);

   QPixmap pixFold = QPixmap::grabWindow(FgWnd, foldRect_x, foldRect_y, foldRect_sz, foldRect_sz);
   QImage imgFold  = pixFold.toImage();

   for (int x = 0; x < foldRect_sz; ++x)
   {
      for (int y = 0; y < foldRect_sz; ++y)
      {
         QRgb rgb = imgFold.pixel(x, y);
         QColor cl(rgb);
         if (!(cl.hue() >= minFoldHue && cl.hue() <= maxFoldHue))
            return;
      }
   }
   
   //наш ход
   // устанавливаем изображение для обработки
   //обрезаем изображение
   HwndToTop(FgWnd);
   RECT roomRect;
   GetClientRect(FgWnd, &roomRect);
   
   int start_x = 0;
   int start_y = 50;
   const int width = roomRect.right - roomRect.left - 210;
   const int height = roomRect.bottom - roomRect.top - 60;
   
   QPixmap pixRoom = QPixmap::grabWindow(FgWnd, start_x, start_y,
      width, height);
   QImage imgRoom  = pixRoom.toImage();
   
   //imgRoom.save("1.bmp");

   cardProc_->setImage(imgRoom);
   //CardProcessing::HoldemLevel hl = cardProc_->holdemLevel();
   // если стадия префлопа 
   if (cardProc_->isPreflop())
   {
      // получить карманные карты
      bool ok = false;
      QPair<QRect, QRect> holeCards = cardProc_->getHoleCards(&ok);
      if (!ok)
         return;
      
      QImage firstImg  = imgRoom.copy(holeCards.first);
      //firstImg.save("1.bmp");
      //QImage firstImg("Jd_blue.bmp");
      QImage secondImg = imgRoom.copy(holeCards.second);
      //secondImg.save("2.bmp");
      //QImage secondImg("Jd_red.bmp");
      //toBlackWhite(firstImg, 200);
      //toBlackWhite(secondImg, 200);

      //debugging{
      //uint num = 0;
      //num = QDateTime::currentDateTime().toTime_t();
      //QString path1 = QString("test/f%1.bmp").arg(num);
      //QString path2 = QString("test/s%1.bmp").arg(num);
      //firstImg.save(path1);
      //secondImg.save(path2);
      //}debugging

      QString card1 = cardFromImage(firstImg);
      QString card2 = cardFromImage(secondImg);
      //сохранить карты в кэш
      QString joined = card1 + card2;
      if (joined == cache_ && !lastIsFold_)
      {
         //хорошие карты - человек думает, не мешать
         return;
      } 
      else
      {
         cache_ = joined;
      }
      //qDebug() << card1 << card2;

      /// Debugging {
      //uint num = 0;
      //num = QDateTime::currentDateTime().toTime_t();
      //imgRoom.save(QString("test/table_%1.bmp").arg(num));
      /// } Debugging

      /// Debugging {
      //imgRoom.copy(holeCards.first).save(QString("test/f%1.bmp").arg(num));
      //imgRoom.copy(holeCards.second).save(QString("test/s%1.bmp").arg(num));
      /// } Debugging
      //return;
      
      //преобразовать карты
      QString range = cardRangeFromHoles(card1, card2);
      //qDebug() << range;
      if (playingCard_.contains(range))
      {
         //panic!
         lastIsFold_ = false;
         if (data_.visualAlert)
         {
            alarm_->highlight(joined, FgWnd);
         }

         if (data_.turnBeep)
         {
            QString appExe = qApp->applicationDirPath();
            QSound::play(appExe + "/sounds/turn.wav");
         }
      }
      else
      {
         if (data_.advisorMode)
         {
            foldOrCheck(FgWnd, false);
            lastIsFold_ = false;
            cache_ = joined;

            QString advice = tr("Fold/Check this hand: %1")
               .arg(card1 + " " + card2);
            cbFun(advice.toStdString().c_str());
         }
         else
         {
            foldOrCheck(FgWnd);

            if (data_.showFolded)
            {
               QString sAction = tr("This hand has been folded: ");
               if (!lastIsFold_)
                  sAction = tr("This hand has been checked: ");
               cbFun(QString(sAction + card1 + " " + card2).toStdString().c_str());
            }
         }
      }
   }
}

HWND Executor::findTables(const QString & tClass, HWND BeginHandle)
{
   //найти окно
   HWND CurrWnd = NULL;
   CurrWnd = GetWindow(BeginHandle, GW_HWNDFIRST);
   
   while (CurrWnd) 
   {
      char clName[MAX_WND_TEXT];
      GetClassNameA(CurrWnd, clName, MAX_WND_TEXT);

      QString sClass(clName);
      if (sClass == tClass)
      {
         //вернуть первое 
         //оно первое в z порядке
         return CurrWnd;
      }
      CurrWnd = GetWindow(CurrWnd, GW_HWNDNEXT);
   }
   return NULL;
}

void Executor::HwndToTop(WId hwnd)
{
   BringWindowToTop(hwnd);
   SetActiveWindow(hwnd);
   SetFocus(hwnd);
   Sleep(100);
}

void Executor::clickCheck(WId hwnd)
{
   const QRect checkRect(354, 656, 100, 20);
   clickTo(hwnd, checkRect);
}

void Executor::clickFold(WId hwnd)
{
   const QRect foldRect(160, 655, 100, 20);
   clickTo(hwnd, foldRect);
}

void Executor::clickTo(WId hwnd, const QRect & rect)
{
   int x = rect.left(), y = rect.top();
   x += qrand() % rect.width();
   y += qrand() % rect.height();
   clickTo(hwnd, x, y);
}

void Executor::clickTo(WId hwnd, int x, int y)
{
   RECT rect;
   GetWindowRect(hwnd, &rect);

   static int x_border = 0;
   static int y_border = 0;
   if (x_border == 0)
   {
      QRect wndRect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
      RECT clrect;
      GetClientRect(hwnd, &clrect);
      QRect clntRect(clrect.left, clrect.top, clrect.right - clrect.left, clrect.bottom - clrect.top);
      x_border = (int)(wndRect.width() - clntRect.width()) / 2.;
      y_border = wndRect.height() - clntRect.height() - x_border;
   }

   x += rect.left + x_border;
   y += rect.top + y_border;
   HwndToTop(hwnd);

   long X = (long)(x * ABSOLUTE_COORD)/screen_res_x_;
   long Y = (long)(y * ABSOLUTE_COORD)/screen_res_y_;

   Hooker hook(Hooker::Mouse);
   //запомнить текущие координаты мышки
   POINT pt;
   GetCursorPos(&pt);
   long cX = (long)(pt.x * ABSOLUTE_COORD)/screen_res_x_;
   long cY = (long)(pt.y * ABSOLUTE_COORD)/screen_res_y_;
   
   mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, X, Y, 0, 0);
   Sleep(100);
   mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN, X, Y, 0, 0);
   Sleep(100);
   mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP, X, Y, 0, 0);

   //вернуть координаты мышки откуда взяли
   mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, cX, cY, 0, 0);

}

void Executor::foldOrCheck(WId hwnd, bool realClick)
{
   //фолдим, если можем - чекаем
   //вырезаем буквы с кнопки чек
   //и считаем их количество
   static int checkRect_x = 354;
   static int checkRect_y = 656;
   static int checkRect_w = 100;
   static int checkRect_h = 20;
   HwndToTop(hwnd);
   QPixmap pixCheck = QPixmap::grabWindow(hwnd, checkRect_x, checkRect_y, checkRect_w, checkRect_h);
   QImage imgCheck  = pixCheck.toImage();
   
   int checkLetters = ProcAcad::countCheckLetters(imgCheck);
   QString appExe = qApp->applicationDirPath();
   if ((checkLetters == 5) || (checkLetters == 3))//check / чек
   {
      if (data_.checkBeep)
      {
         QSound::play(appExe + "/sounds/check.wav");
      }
      if (realClick)
      {
         //нажать чек
         clickCheck(hwnd);
      }
      //обнулить кэш
      cache_.clear();
      lastIsFold_ = false;

   }
   else
   {
      lastIsFold_ = true;
      if (data_.foldBeep)
      {
         QSound::play(appExe + "/sounds/fold.wav");
      }

      if (realClick)
      {
         //нажать фолд
         clickFold(hwnd);
      }
   }
}

QString Executor::cardFromImage(QImage & img)
{
   int imgW = img.width();
   int imgH = img.height();

   int minDist = imgW * imgH;
   int minIdx = 0;
   
   BoolMatrix * whatMatrix = new BoolMatrix(img, 200);
   const int baseCount = cardBase_->count();
   for (int i = 0; i < baseCount; ++i)
   {
      const BoolMatrix * bm = cardBase_->matrix(i);
      //изображение подтягивается под эталон
      if ((bm->width()  != imgW) ||
          (bm->height() != imgH) )
      {
         img = img.scaled(imgW, imgH);
         delete whatMatrix;
         whatMatrix = new BoolMatrix(img, 200);
      }
      int res = *whatMatrix - *bm;

      if (res < minDist)
      {
         minDist = res;
         minIdx = i;
      }
      if (res == 0)
         break;
   }
   delete whatMatrix;
   
   return cardBase_->nominal(minIdx);
}


QString Executor::cardString(int nom, int suit)
{
   QString res;
   QStringList nomList;
   nomList << "" << "2" << "3" << "4" << "5" << "6" << "7" 
      << "8" << "9" << "T" << "J" << "Q" << "K" << "A";

   QStringList suiList;
   suiList << "" << "h" << "d" << "c" << "s";

   res = nomList.at(nom) + suiList.at(suit);
   return res;
}

QString Executor::cardRangeFromHoles(const QString & f, const QString & s)
{
   QString range;
   QStringList nomList;
   nomList << "" << "2" << "3" << "4" << "5" << "6" << "7" 
      << "8" << "9" << "T" << "J" << "Q" << "K" << "A";

   QString fNom  = f.left(1);
   QString fSuit = f.right(1);
   QString sNom  = s.left(1);
   QString sSuit = s.right(1);
   int n1 = nomList.indexOf(fNom);
   int n2 = nomList.indexOf(sNom);
   if (n2 > n1)
   {
      QString swap = sNom;
      sNom = fNom;
      fNom = swap;
      
      swap = sSuit;
      sSuit = fSuit;
      fSuit = swap;
   }
   if (fNom == sNom)
   {
      range = fNom + sNom;
   }
   else
   {
      if (fSuit == sSuit)
      {
         range = fNom + sNom + "s";
      }
      else
      {
         range = fNom + sNom + "o";
      }
   }
   return range;
}
