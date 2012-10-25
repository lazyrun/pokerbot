#include "ProcTitan.h"

//
//ProcTitan
//
ProcTitan::ProcTitan()
: CardProcessing()
{
   threshold_ = 230;
   matrix_ = 0;
   minBright_ = 237;
   maxBright_ = 254;
}

ProcTitan::~ProcTitan()
{
   delete matrix_;
   matrix_ = 0;
}

void ProcTitan::setImage(const QImage & img)
{
   delete matrix_;
   matrix_ = 0;
   matrix_ = new BoolMatrix(img, threshold_/*, minBright_, maxBright_*/);
   //QImage im = img;
   //toBlackWhite(im, 230);
   //im.save("table.bmp");
   
   //находим центр стола и относительно него пытаемся заполнить черный квадрат
   const int wid2 = matrix_->width() >> 1;
   const int hei2 = matrix_->height() >> 1;
   const int sz2 = 8;
   
   holdemLevel_ = Preflop;
   for (int x = wid2 - sz2; x < wid2 + sz2; ++x)
   {
      for (int y = hei2 - sz2; y < hei2 + sz2; ++y)
      {
         if (matrix_->at(x, y) == 0)//белый не наш цвет
         {
            //на столе валяются карты
            holdemLevel_ = Flop;
            x = wid2 + sz2;
            break;
         }
      }
   }
}

QPair<QRect, QRect> ProcTitan::getHoleCards(bool * ok)
{
   maxWhite_ = matrix_->findMaxWhiteRect(8, 10);//ищем белый квадрат 6х6

   //qDebug() << maxWhite_;

   QRect first;
   QRect second;
   if (matrix_ == 0)
   {
      *ok = false;
      return qMakePair<QRect, QRect>(first, second);
   }

   int width = matrix_->width();
   int height = matrix_->height();
   //ищем минимальное растояние до левой границы
   QPoint tl = maxWhite_.bottomLeft();
   
   int left_x = 0;
   for (int x = tl.x(); x > 10; --x)
   {
      if (matrix_->at(x, tl.y()) == 1)//черный
      {
         left_x = x + 1;
         break;
      }
   }
   
   //опускаемся вниз
   int bottom_y = 0;
   for (int y = maxWhite_.y(); y < height - 10; ++y)
   {
      if (matrix_->at(left_x, y) == 1)//черный
      {
         bottom_y = y - 10;
         break;
      }
   }

   //определяем ширину
   int cw = 0;
   for (int x = left_x; x < width; ++x)
   {
      if (matrix_->at(x, bottom_y) == 1)//черный
      {
         cw = x - left_x;
         break;
      }
   }

   //поднимаемся вверх
   int top_y = 0;
   for (int y = maxWhite_.y(); y > 10; --y)
   {
      if (matrix_->at(left_x, y) == 1)//черный
      {
         top_y = y + 1;
         break;
      }
   }
   
   //ограничивем снизу вверх
   int brd_bottom = 0;
   for (int y = bottom_y - 1; y > 10; --y)
   {
      for (int x = left_x; x < left_x + cw; ++x)
      {
         if (matrix_->at(x, y) == 1)//черный
         {
            brd_bottom = y;
            y = 0;
            break;
         }
      }
   }

   //ограничиваем слева направо
   int brd_left = 0;
   for (int x = left_x; x < left_x + cw; ++x)
   {
      for (int y = brd_bottom; y > top_y; --y)
      {
         if (matrix_->at(x, y) == 1)//черный
         {
            brd_left = x;
            x = left_x + cw;
            break;
         }
      }
   }

   //поднимаемся по границе второй карты вверх
   int brd_y = 0;
   int scan_x = left_x + cw;
   for (int y = brd_bottom; y > 10; --y)
   {
      if (matrix_->at(scan_x, y) == 0)//белый
      {
         brd_y = y;
         break;
      }
   }
   for (int y = brd_y; y > 10; --y)
   {
      if (matrix_->at(scan_x, y) == 1)//черный
      {
         brd_y = y + 1;
         break;
      }
   }

   //опускаемся от границы до начала рисунка
   int brd_top = 0;
   int stop_x = scan_x - (cw >> 1);
   for (int y = brd_y; y < brd_bottom; ++y)
   {
      for (int x = scan_x; x > stop_x; --x)
      {
         if (matrix_->at(x, y) == 1)//черный
         {
            brd_top = y;
            y = brd_bottom;
            break;
         }
      }
   }
   first.setX(brd_left);
   first.setY(brd_top);
   first.setWidth(left_x + cw - brd_left);
   first.setHeight(brd_bottom - brd_top);
   
   //поиск второй карты
   //толщина границы второй карты
   int brd_fat = 0;
   for (int x = left_x + cw; x < width -10; ++x)
   {
      if (matrix_->at(x, brd_bottom) == 0)//белый
      {
         brd_fat = x - (left_x + cw) - 1;
         break;
      }
   }

   //зная границу спускаемся вниз по белому до черного
   scan_x += brd_fat + 1;
   int temp_y = 0;
   for (int y = brd_bottom; y < height - 10; ++y)
   {
      if (matrix_->at(scan_x, y) == 1)
      {
         temp_y = y - 1;
         break;
      }
   }

   //поднимаемся снизу
   for (int y = temp_y; y > 10; --y)
   {
      for (int x = scan_x; x < scan_x + first.width(); ++x)
      {
         if (matrix_->at(x, y) == 1)
         {
            temp_y = y;
            y = 0;
            break;
         }
      }
   }
   //ограничили снизу
   brd_bottom = temp_y;
   
   //поднимаемся вверх
   for (int y = brd_bottom; y > 10; --y)
   {
      if (matrix_->at(scan_x, y) == 1)
      {
         temp_y = y + 1;
         break;
      }
   }
   //сканируем слева направо
   for (int x = scan_x; x < scan_x + first.width(); ++x)
   {
      for (int y = brd_bottom; y > temp_y; --y)
      {
         if (matrix_->at(x, y) == 1)
         {
            brd_left = x;
            x = scan_x + first.width();
            break;
         }
      }
   }
   
   //поднимаемся по левому краю вверх
   for (int y = brd_bottom; y > 10; --y)
   {
      if (matrix_->at(brd_left - 1, y) == 1)
      {
         temp_y = y + 1;
         break;
      }
   }

   //сканируем вниз
   for (int y = temp_y; y < brd_bottom; ++y)
   {
      for (int x = brd_left - 1; x < brd_left - 1 + first.width(); ++x) 
      {
         if (matrix_->at(x, y) == 1)
         {
            temp_y = y;
            y = brd_bottom;
            break;
         }
      }
   }
   brd_top = temp_y;

   second.setX(brd_left);
   second.setY(brd_top);
   second.setWidth(first.width());
   second.setHeight(brd_bottom - brd_top);

   *ok = true;
   return qMakePair<QRect, QRect>(first, second);
}

int ProcTitan::countCheckLetters(const QImage & imgCheck)
{
   //очерняем
   const int width = imgCheck.width();
   const int height = imgCheck.height();

   BoolMatrix checkMatrix(imgCheck);
   for (int x = 0; x < width; ++x)
   {
      for (int y = 0; y < height; ++y)
      {
         QRgb rgb = imgCheck.pixel(x, y);
         QColor cl(rgb);
         if (cl.hue() >= 13 && cl.hue() <= 33) 
         {
            checkMatrix.set(x, y, false);
            //imgCheck.setPixel(x, y, qRgb(0, 0, 0));
         }
         else
         {
            checkMatrix.set(x, y, true);
            //imgCheck.setPixel(x, y, qRgb(255, 255, 255));
         }
      }
   }

   //сканируем рисунок на предмет пробелов
   int countLetters = 0;
   int count = 0;

   bool isBlackLinePrev = true;
   bool isBlackLineCurr = true;
   bool isLetterFinish  = false;

   QPoint ptLeft, ptRight;
   for (int w = 0; w < width; ++w)
   {
      isLetterFinish = false;
      isBlackLineCurr = true;
      for (int h = 0; h < height; ++h)
      {
         if (checkMatrix.at(w, h) == true)
         {
            isBlackLineCurr = false;
            break;
         }
      }
      if ((isBlackLinePrev == true) && (isBlackLineCurr == false))
      {
         countLetters++;
         isBlackLinePrev = false;
      }
      else if ((isBlackLinePrev == false) && (isBlackLineCurr == true))
      {
         isBlackLinePrev = true;
         isLetterFinish = true;
      }
      if (isLetterFinish)
      {
         count++;
      }
   }
   
   return count;
}

bool ProcTitan::checkHueButton(const QImage & imgBtn, int minHue, int maxHue, int rectMin)
{
   const int btnWidth = imgBtn.width();
   const int btnHeight = imgBtn.height();
   bool hasBtn = false;

   bool blackRect = false;
   for (int x = 0; x < btnWidth; x += 2)
   {
      for (int y = 0; y < btnHeight; y += 2)
      {
         QRgb rgb = imgBtn.pixel(x, y);
         QColor cl(rgb);
         if ((cl.hue() >= minHue && cl.hue() <= maxHue))
         {
            blackRect = true;
            //попробовать найти маленький черный квадратик
            for (int rx = 0; rx < rectMin; ++rx)
            {
               for (int ry = 0; ry < rectMin; ++ry)
               {
                  QRgb rgb = imgBtn.pixel(x + rx, y + ry);
                  QColor cl(rgb);
                  if (!(cl.hue() >= minHue && cl.hue() <= maxHue))
                  {
                     //не наш пиксель
                     rx = rectMin;
                     blackRect = false;
                     break;
                  }
               }
            }
            if (!blackRect)
            {
               // не то ищем дальше
               continue;
            }
            else
            {
               //есть черный квадратик
               hasBtn = true;
               x = btnWidth;
               break;
            }
         }
      }
   }
   return hasBtn;
}

bool ProcTitan::checkHueButton(const QImage & imgBtn, int minHue, int maxHue, int rectMin, QRect & rect)
{
   const int btnWidth = imgBtn.width();
   const int btnHeight = imgBtn.height();
   bool hasBtn = false;

   bool blackRect = false;
   for (int x = 0; x < btnWidth; x += 2)
   {
      for (int y = 0; y < btnHeight; y += 2)
      {
         QRgb rgb = imgBtn.pixel(x, y);
         QColor cl(rgb);
         if ((cl.hue() >= minHue && cl.hue() <= maxHue))
         {
            blackRect = true;
            //попробовать найти маленький черный квадратик
            for (int rx = 0; rx < rectMin; ++rx)
            {
               for (int ry = 0; ry < rectMin; ++ry)
               {
                  QRgb rgb = imgBtn.pixel(x + rx, y + ry);
                  QColor cl(rgb);
                  if (!(cl.hue() >= minHue && cl.hue() <= maxHue))
                  {
                     //не наш пиксель
                     rx = rectMin;
                     blackRect = false;
                     break;
                  }
               }
            }
            if (!blackRect)
            {
               // не то ищем дальше
               continue;
            }
            else
            {
               //есть черный квадратик
               hasBtn = true;
               rect.setX(x);
               rect.setY(y);
               x = btnWidth;
               break;
            }
         }
      }
   }
   
   for (int x = rect.x(); x < btnWidth; ++x)
   {
      QRgb rgb = imgBtn.pixel(x, rect.y());
      QColor cl(rgb);
      if (!(cl.hue() >= minHue && cl.hue() <= maxHue))
      {
         rect.setWidth(x - rect.x());
         break;
      }
   }

   for (int y = rect.y(); y < btnHeight; ++y)
   {
      QRgb rgb = imgBtn.pixel(rect.x(), y);
      QColor cl(rgb);
      if (!(cl.hue() >= minHue && cl.hue() <= maxHue))
      {
         rect.setHeight(y - rect.y());
         break;
      }
   }
   
   return hasBtn;
}

