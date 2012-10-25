#include "Executor.h"
#include "ProcCake.h"
#include "CardOCR_Cake.h"
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

   screen_res_x_ = GetSystemMetrics(SM_CXSCREEN);//ѕолучить ширину экрана
   screen_res_y_ = GetSystemMetrics(SM_CYSCREEN);//ѕолучить высоту экрана
   
   cardProc_ = new ProcCake();
   alarm_ = new AlarmWidget();
}

Executor::~Executor()
{
   delete cardProc_;
   delete alarm_;
}

void Executor::start()
{
   //запускаем таймер
   //чтение настроек из конфигуратора
   
   QString regKey = QDir::current().dirName();
   QSettings settings(regKey, "Config");

   QString range = settings.value("CardRange").toString();
   playingCard_ = range.split(", ", QString::SkipEmptyParts);
   QVariant sett = settings.value("Settings");
   data_ = qvariant_cast<SettingsData>(sett);

   interval_ = data_.interval;
   //interval_ = 1000;
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
   static const QString psClass = "HwndWrapper";
   static const int minFoldHue = 353;
   static const int maxFoldHue = 357;
   static const int minCallHue = 60;
   static const int maxCallHue = 70;
   static const int minCheckHue = 45;
   static const int maxCheckHue = 49;

   //смотрим топовое окно
   
   WId FgWnd = GetForegroundWindow();
   char clName[MAX_WND_TEXT];
   GetClassNameA(FgWnd, clName, MAX_WND_TEXT);
   QString sClass(clName);
   if (!sClass.contains(psClass, Qt::CaseInsensitive))
   {
      return;
   }
   
   //qDebug() << sClass;

   //область кнопок
   QRect checkRect;
   QRect foldRect;
   bool hasCheck = false;
   bool hasFold  = false;
   bool hasCall  = false;

   RECT fgRect;
   GetClientRect(FgWnd, &fgRect);
   //GetWindowRect(BtnWnd, &btnRect);
   int fgHeight = fgRect.bottom - fgRect.top;
   int fgWidth  = fgRect.right  - fgRect.left;
   int fgWidth2 = fgWidth >> 1;
   int fgHeight2 = fgHeight >> 1;
   int fgHeight4 = fgHeight >> 2;

   HwndToTop(FgWnd);
   QPixmap pixBtn = QPixmap::grabWindow(FgWnd, fgWidth2, fgHeight2 + fgHeight4,
      fgWidth - fgWidth2, fgHeight - fgHeight2 - fgHeight4);
   QImage imgBtn  = pixBtn.toImage();
   
   
   ///////////debug
   //QString debugFile = "../CardBase/Cake/3.bmp";
   //WId FgWnd = GetForegroundWindow();
   ////область кнопок
   //QRect checkRect;
   //QRect foldRect;
   //bool hasCheck = false;
   //bool hasFold  = false;
   //bool hasCall  = false;

   //QImage img(debugFile);
   //int w2 = img.width() >> 1;
   //int h2 = img.height() >> 1;
   //int h4 = img.height() >> 2;
   //QImage imgBtn = img.copy(w2, h2 + h4, img.width() - w2, img.height() - h2 - h4);
   ////toBlackWhiteByHue(imgBtn, minCheckHue, maxCheckHue);
   //imgBtn.save("1.bmp");
   //
   ///////////////////////end debug

   //здесь нужна обработка нашего хода
   static const int rectMinH = 8;
   static const int rectMinW = 60;
   //Ќачинаем поиск с кнопки „ек
   //если она есть то однозначно наш ход
   hasCheck = ProcCake::checkHueButton(imgBtn, minCheckHue, maxCheckHue,
      rectMinW, rectMinH, checkRect);
   
   //ищем кнопку Fold
   hasFold = ProcCake::checkHueButton(imgBtn, minFoldHue, maxFoldHue, 
      rectMinW, rectMinH, foldRect);

   if (!hasCheck && !hasCheck)
   {
      //нет кнопок дл€ нажати€
      return;
   }

   int btnHeight = fgHeight4;
   int btnWidth = fgWidth2;

   //здесь наш ход
   QPixmap pixRoom = QPixmap::grabWindow(FgWnd);

   //////////////////debug
   //QPixmap pixRoom(debugFile);
   //pixRoom = pixRoom.copy(0, 0, pixRoom.width(), pixRoom.height());
   //////////////////end debug
   
   QRect rectRoom = pixRoom.rect();
   //обрезать по верхней границе кнопок
   if (hasFold)
   {
      rectRoom.setHeight(fgHeight2 + fgHeight4 + foldRect.top());
   }
   else if (hasCheck)
   {
      rectRoom.setHeight(fgHeight2 + fgHeight4 + checkRect.top());
   }
   else
   {
      rectRoom.setHeight(rectRoom.height() - btnHeight);
   }

   QImage imgRoom = pixRoom.copy(rectRoom).toImage();

    //Debugging {
   uint num = 0;
   num = QDateTime::currentDateTime().toTime_t();
   imgRoom.save(QString("test/table_%1.bmp").arg(num));
    //} Debugging

   // устанавливаем изображение дл€ обработки
   cardProc_->setImage(imgRoom);
   // если стади€ префлопа 
   if (cardProc_->isPreflop())
   {
      //qDebug() << "Preflop!";
      // получить карманные карты
      bool ok = false;
      QPair<QString, QString> holeCards = cardProc_->getHoleCards(&ok);

      //QPair<QRect, QRect> holeCards = cardProc_->getHoleCards(&ok);
      if (!ok)
         return;

       //Debugging {
      uint num = 0;
      num = QDateTime::currentDateTime().toTime_t();
      imgRoom.save(QString("test/table_%1.bmp").arg(num));
      imgBtn.save(QString("test/btns_%1.bmp").arg(num));
      //imgRoom.copy(holeCards.first).save(QString("test/first_%1.bmp").arg(num));
      //imgRoom.copy(holeCards.second).save(QString("test/second_%1.bmp").arg(num));
       //} Debugging

      //if (holeCards.first.isEmpty() || holeCards.second.isEmpty() ||
      //    holeCards.first.isNull() || holeCards.second.isNull())
      //    return;

      //QImage firstImg  = imgRoom.copy(holeCards.first);
      //QImage secondImg = imgRoom.copy(holeCards.second);

      //QString card1 = cardFromImage(firstImg);
      //QString card2 = cardFromImage(secondImg);

      QString card1 = holeCards.first;
      QString card2 = holeCards.second;

      if (card1.length() != 2 || card2.length() != 2)
         return;

      qDebug() << card1 << card2;
      //return;
      
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
            QString advice = tr("Fold/Check this hand: %1")
               .arg(card1 + " " + card2);
            
            if (cbFun)
               cbFun(advice.toStdString().c_str());
            return;
         }
         
         //foldOrCheck(FgWnd);
         //qDebug() << "Check Rect" << checkRect;
         int w = checkRect.width();
         int h = checkRect.height();
         checkRect.setX(btnWidth + checkRect.x());
         checkRect.setY(fgHeight2 + fgHeight4 + checkRect.y());
         checkRect.setWidth(w);
         checkRect.setHeight(h);
         //qDebug() << "New Check Rect" << checkRect;

         //qDebug() << "Fold Rect" << foldRect;
         w = foldRect.width();
         h = foldRect.height();
         foldRect.setX(btnWidth + foldRect.x());
         foldRect.setY(fgHeight2 + fgHeight4 + foldRect.y());
         foldRect.setWidth(w);
         foldRect.setHeight(h);
         //qDebug() << "New Fold Rect" << foldRect;
         if (hasCheck && !hasFold)
         {
            if (data_.checkBeep)
            {
               QString appExe = qApp->applicationDirPath();
               QSound::play(appExe + "/sounds/check.wav");
            }
            
            clickTo(FgWnd, checkRect);
            
            if (data_.showFolded)
            {
               QString sAction = tr("This hand has been checked: ");
               if (cbFun)
                  cbFun(QString(sAction + card1 + " " + card2).toStdString().c_str());
            }

            //обнулить кэш
            cache_.clear();
            lastIsFold_ = false;
         }
         else if (hasFold)
         {
            lastIsFold_ = true;
            if (data_.foldBeep)
            {
               QString appExe = qApp->applicationDirPath();
               QSound::play(appExe + "/sounds/fold.wav");
            }

            clickTo(FgWnd, foldRect);
            
            if (data_.showFolded)
            {
               QString sAction = tr("This hand has been folded: ");
               if (cbFun)
                  cbFun(QString(sAction + card1 + " " + card2).toStdString().c_str());
            }
         }
      }
   }
//код дл€ набивки базы карт
/*
   qDebug() << "Check Rect" << checkRect;
   int w = checkRect.width();
   int h = checkRect.height();
   checkRect.setX(btnWidth + checkRect.x());
   checkRect.setY(imgRoom.height() + checkRect.y());
   checkRect.setWidth(w);
   checkRect.setHeight(h);
   qDebug() << "New Check Rect" << checkRect;

   qDebug() << "Fold Rect" << foldRect;
   w = foldRect.width();
   h = foldRect.height();
   foldRect.setX(btnWidth + foldRect.x());
   foldRect.setY(imgRoom.height() + foldRect.y());
   foldRect.setWidth(w);
   foldRect.setHeight(h);
   qDebug() << "New Fold Rect" << foldRect;

   if (cardProc_->isPreflop())
   {
      if (hasCheck)
      {
         clickTo(FgWnd, checkRect);
      }
      else if (hasFold && hasCall)
      {
         clickTo(FgWnd, foldRect);
      }
   }
   else
   {
      clickTo(FgWnd, foldRect);
      clickTo(FgWnd, checkRect);

      qDebug() << "Flop at least!";
      return;
   }
*/
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
         //оно первое в z пор€дке
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
   if (!rect.isValid())
      return;

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
   Sleep(100);
   
   //вернуть координаты мышки откуда вз€ли
   mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, cX, cY, 0, 0);
}

QString Executor::cardFromImage(QImage & img)
{
   //QImage im(img);
   //toBlackWhiteMid(im, 128);

   //const int w = im.width();
   //const int h = im.height();
   //int y_split = 0;
   ////поиск первой черной полосы
   //int y_black = 0;
   //for (int y = 1; y < h; ++y)
   //{
   //   for (int x = 0; x < w; ++x)
   //   {
   //      if (im.pixel(x, y) == qRgb(0, 0, 0))
   //      {
   //         y_black = y;
   //         y = h;
   //         break;
   //      }
   //   }
   //}
   ////поиск первой белой полосы
   //for (int y = y_black; y < h; ++y)
   //{
   //   int cnt_wt = 0;
   //   for (int x = 0; x < w; ++x)
   //   {
   //      if (im.pixel(x, y) == qRgb(255, 255, 255))
   //      {
   //         cnt_wt++;
   //      }
   //      else
   //      {
   //         break;
   //      }
   //   }
   //   if (cnt_wt == w)
   //   {
   //      y_split = y;
   //      break;
   //   }
   //}
   ////обжимаем картинки слева и справа
   //QImage imc  = im.copy(0, 1, w, y_split);
   //
   //int left_wt = 0;
   //for (int x = 0; x < imc.width(); ++x)
   //{
   //   for (int y = 0; y < imc.height(); ++y)
   //   {
   //      if (imc.pixel(x, y) != qRgb(255, 255, 255))
   //      {
   //         x = imc.width() + 1;
   //         break;
   //      }
   //   }
   //   if (x != imc.width() + 1)
   //      left_wt++;
   //}

   //int right_wt = 0;
   //for (int x = imc.width() - 1; x > 0; --x)
   //{
   //   for (int y = 0; y < imc.height(); ++y)
   //   {
   //      if (imc.pixel(x, y) != qRgb(255, 255, 255))
   //      {
   //         x = -1;
   //         break;
   //      }
   //   }
   //   if (x != -1)
   //      right_wt++;
   //}

   //QImage top  = img.copy(left_wt, 1, imc.width() - left_wt - right_wt, y_split);
   //QImage topwb = im.copy(left_wt, 1, imc.width() - left_wt - right_wt, y_split);
   //topwb.save("../CardBase/Cake/split/1_nm.bmp");
   //top.save("../CardBase/Cake/split/2_nm1.bmp");

   QImage imgwb(img);
   toBlackWhite(imgwb, 128);
   CardOCR ocr;
   //подаем черно-белую
   QString card = ocr.nominal(&imgwb) +
      ocr.suit(&imgwb, &img);

   return card;
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

