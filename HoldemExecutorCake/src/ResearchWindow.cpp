#include "ResearchWindow.h"
//#include "phash.h"
//#include "cphash.h"
//#include "DBManager.h"
#include "../CommonSources/enum.h"
#include "ImageProc.h"
#include "CardOCR_Cake.h"
#include "ProcCake.h"

ResearchWindow::ResearchWindow() 
: QMainWindow()
{
   //whiteRect();
   //whiteOnBlack();
   // ПЕРЕВОД В Ч/Б
   //toBlackWhiteImages();
   //проверка стадии стола
   //checkPreflop();
   //поиск карманных карт
   findHoleCards();
}

void ResearchWindow::whiteRect()
{
   //поиск карманных карт
   QString path = "../CardBase/Cake/white/tmp/";
   QDir rsDir(path);
   QFileInfoList list = rsDir.entryInfoList();
   for (int i = 0; i < list.size(); ++i) 
   {
      QFileInfo fileInfo = list.at(i);
      QString fl = fileInfo.baseName();
      QImage img(fileInfo.absoluteFilePath());
      if (img.isNull())
         continue;
      
      BoolMatrix bm(img, 200);

   
      QRect rect = findFirstWhiteRect(bm);
      QPainter painter(&img);
      painter.setPen(Qt::red);

      painter.drawRect(rect);
      img.save(QString("../CardBase/Cake/find/%1_red.bmp").arg(fl));

   }
   return;

   //for (int y = 0; y < bm.height(); y++)
   //{
   //   for (int x = 0; x < bm.width(); x++)
   //   {
   //      
   //      //if (bm.isWhite(x, y))
   //      //{
   //      //   bool inWhiteRect = false;
   //      //   foreach (QRect rect, whiteRects_)
   //      //   {
   //      //      if (rect.contains(QPoint(x, y), true))
   //      //      {
   //      //         inWhiteRect = true;
   //      //         break;
   //      //      }
   //      //   }
   //      //   //точка не входит ни в один из готовых прямоугольников
   //      //   if (!inWhiteRect)
   //      //   {
   //      //      //поиск для белой точки замкнутого прямоугольника
   //      //      findWhiteRect(bm, x, y);
   //      //   }
   //      //}
   //   }
   //}
   //отбросить пустые прямоугольники не содержащие черной хуеты
   //foreach (QRect what, whiteRects_)
   //{
   //   if (!hasBlack(bm, what))
   //   {
   //      whiteRects_.removeOne(what);
   //   }
   //}
   ////отсечь вложенные прямоугольники
   //foreach (QRect what, whiteRects_)
   //{
   //   foreach (QRect with, whiteRects_)
   //   {
   //      if (what == with)
   //         continue;
   //      if (with.contains(what))
   //      {
   //         whiteRects_.removeOne(with);
   //      }
   //   }
   //}

   ////QList<QRect> resList;
   ////foreach (QRect what, whiteRects_)
   ////{
   ////   QList<QRect> interList;
   ////   foreach (QRect with, whiteRects_)
   ////   {
   ////      if (what == with)
   ////         continue;
   ////      if (what.intersects(with))
   ////      {
   ////         interList << with;
   ////      }
   ////   }
   ////   if (interList.count())
   ////   {
   ////      QRect intRect = interList.at(0);
   ////      foreach (QRect r, interList)
   ////      {
   ////         if (r == intRect)
   ////            continue;
   ////         intRect = intRect.intersected(r);
   ////      }
   ////      resList << intRect;
   ////   }
   ////}

   ////для всех прямоугольников определить пересечение
   ////for (int i = 0; i < whiteRects_.count(); i++)
   ////{
   ////   QRect & what = whiteRects_[i];
   ////   for (int j = 0; j < whiteRects_.count(); j++)
   ////   {
   ////      if (i == j)
   ////         continue;
   ////      QRect & with = whiteRects_[j];
   ////      if (what.intersects(with))
   ////      {
   ////         what = what.intersected(with);
   ////         whiteRects_.removeOne(with);
   ////      }
   ////   }
   ////}
   //
   ////нарисуем все прямоугольники
   ////QPainter painter(&img);
   ////painter.setPen(Qt::red);

   ////foreach (QRect wr, whiteRects_)
   ////{
   ////   painter.drawRect(wr);
   ////}
   ////img.save("../CardBase/Cake/find/test_red.bmp");
   ////qDebug() << whiteRects_;
}

QRect ResearchWindow::findFirstWhiteRect(const BoolMatrix & bm)
{
   for (int y = 0; y < bm.height(); y++)
   {
      for (int x = 0; x < bm.width()/2; x++)
      {
         if (bm.isWhite(x, y))
         {
            QList<QRect> wRects;
            QRect rect;
            rect.setTopLeft(QPoint(x, y));
            for (int i = bm.width(); i > x + 10; i--)
            {
               for (int j = bm.height(); j > y + 10; j--)
               {
                  rect.setBottomRight(QPoint(i, j));
                  if (isWhiteBordered(bm, rect))
                  {
                     if (hasBlack(bm, rect))
                        wRects << rect;
                  }
               }
            }
            if (wRects.count())
            {
               //из всех прямоугольников отобрать минимальный
               int min_side_sum = bm.width() + bm.height();
               QRect min_rect;
               foreach (QRect wr, wRects)
               {
                  int sum = wr.width() + wr.height();
                  if (sum < min_side_sum)
                  {
                     min_side_sum = sum;
                     min_rect = wr;
                  }
               }
               if (min_rect.isValid())
                  return min_rect;
            }
         }
      }
   }
   return QRect();
}

QRect ResearchWindow::findFirstBlackRect(const BoolMatrix & bm)
{
   for (int y = 0; y < bm.height(); y++)
   {
      for (int x = 0; x < bm.width(); x++)
      {
         if (bm.isBlack(x, y))
         {
            QList<QRect> bRects;
            QRect rect;
            rect.setTopLeft(QPoint(x, y));
            for (int i = bm.width(); i > x + 10; i -= 2)
            {
               for (int j = bm.height(); j > y + 10; j -= 2)
               {
                  rect.setBottomRight(QPoint(i, j));
                  if (isBlackBordered(bm, rect))
                  {
                     if (hasWhite(bm, rect))
                        bRects << rect;
                  }
               }
            }
            if (bRects.count())
            {
               //из всех прямоугольников отобрать минимальный
               int min_side_sum = bm.width() + bm.height();
               QRect min_rect;
               foreach (QRect br, bRects)
               {
                  int sum = br.width() + br.height();
                  if (sum < min_side_sum)
                  {
                     min_side_sum = sum;
                     min_rect = br;
                  }
               }
               if (min_rect.isValid())
                  return min_rect;
            }
         }
      }
   }
   return QRect();
}

bool ResearchWindow::hasBlack(const BoolMatrix & bm, const QRect & rect)
{
   for (int x = rect.left(); x < rect.right(); x += 2)
   {
      for (int y = rect.top(); y < rect.bottom(); y += 2)
      {
         if (bm.isBlack(x, y))
            return true;
      }
   }
   return false;
}

bool ResearchWindow::hasWhite(const BoolMatrix & bm, const QRect & rect)
{
   for (int x = rect.left(); x < rect.right(); x += 2)
   {
      for (int y = rect.top(); y < rect.bottom(); y += 2)
      {
         if (bm.isWhite(x, y))
            return true;
      }
   }
   return false;
}


void ResearchWindow::findWhiteRect(const BoolMatrix & bm, int x, int y)
{
   QRect rect;
   QList<QRect> wRects;
   rect.setTopLeft(QPoint(x, y));
   for (int i = bm.width(); i > x + 10; i -= 2)
   {
      for (int j = bm.height(); j > y + 10; j -= 2)
      {
         rect.setBottomRight(QPoint(i, j));
         if (isWhiteBordered(bm, rect))
         {
            wRects << rect;
         }
      }
   }
   //из всех прямоугольников отобрать минимальный
   int min_side_sum = bm.width() + bm.height();
   QRect min_rect;
   foreach (QRect wr, wRects)
   {
      int sum = wr.width() + wr.height();
      if (sum < min_side_sum)
      {
         min_side_sum = sum;
         min_rect = wr;
      }
   }
   if (min_rect.isValid())
      whiteRects_ << min_rect;
}


bool ResearchWindow::isWhiteBordered(const BoolMatrix & bm, const QRect & rect)
{
   for (int x = rect.left(); x < rect.right(); x += 2)
   {
      if (bm.isBlack(x, rect.top()))
         return false;
   }
   for (int x = rect.left(); x < rect.right(); x += 2)
   {
      if (bm.isBlack(x, rect.bottom()))
         return false;
   }
   for (int y = rect.top(); y < rect.bottom(); y += 2)
   {
      if (bm.isBlack(rect.left(), y))
         return false;
   }
   for (int y = rect.top(); y < rect.bottom(); y += 2)
   {
      if (bm.isBlack(rect.right(), y))
         return false;
   }

   return true;
}

bool ResearchWindow::isBlackBordered(const BoolMatrix & bm, const QRect & rect)
{
   for (int x = rect.left(); x < rect.right(); x += 2)
   {
      if (bm.isWhite(x, rect.top()))
         return false;
   }
   for (int x = rect.left(); x < rect.right(); x += 2)
   {
      if (bm.isWhite(x, rect.bottom()))
         return false;
   }
   for (int y = rect.top(); y < rect.bottom(); y += 2)
   {
      if (bm.isWhite(rect.left(), y))
         return false;
   }
   for (int y = rect.top(); y < rect.bottom(); y += 2)
   {
      if (bm.isWhite(rect.right(), y))
         return false;
   }

   return true;
}

void ResearchWindow::whiteOnBlack()
{
   QString path = "../CardBase/Cake/find/test.bmp";
   QImage img(path);
   BoolMatrix bm(&img);
   
   QPoint begin;
   QPoint end;
   
   for (int x = 0; x < bm.width(); x++)
   {
      for (int y = 0; y < bm.height(); y++)
      {
         if (bm.isBlack(x, y))
         {
            //граничная точка
            bool border = isBorder(bm, x, y);
            if (border)
            {
               //отбор смежных граничных точек
               QList<QPoint> sib = siblingBorder(bm, x, y);
               qDebug() << sib;
               if (sib.count() >= 2)
               {
                  QPoint start = sib.at(0);
                  QPoint end   = sib.at(1);
                  //строить замыкание от start до end
                  border_.clear();
                  border_ << QPoint(x, y);
                  closure(bm, start, end);
               }
            }
         }
      }
   }
}

void ResearchWindow::closure(const BoolMatrix & bm, QPoint start, QPoint end)
{
   border_ << start;
   QList<QPoint> sibling = siblingBorder(bm, start.x(), start.y());
   //border_ << sibling;

   //if (sibling.count())
   //{
   //   QPoint nextPoint = sibling.at(0);

   //}
   //int c = 0;
   while (!sibling.contains(end))
   {
      QPoint nextPoint = sibling.at(0);
      border_ << nextPoint;

      sibling = siblingBorder(bm, nextPoint.x(), nextPoint.y());
      //border_ << sibling;
      //c++;
   }
   //

   //recursive(bm, start);
   
   //foreach (QPoint pt, sibling)
   //{
   //   border_ << siblingBorder(bm, pt.x(), pt.y());
   //}
   
   //do
   //{
   //   QList<QPoint> sib = siblingBorder(bm, 
   //} 
   //while(!border.isEmpty())
}

QList<QPoint> ResearchWindow::siblingBorder(const BoolMatrix & bm, int x, int y)
{
   int xs = (x == 0) ? 0 : x - 1;
   int xe = (x == bm.width() - 1) ? bm.width() - 1 : x + 1;
   int ys = (y == 0) ? 0 : y - 1;
   int ye = (y == bm.height() - 1) ? bm.height() - 1 : y + 1;
   QList<QPoint> sib;
   for (int i = xs; i <= xe; i++)
   {
      for (int j = ys; j <= ye; j++)
      {
         if ((i == x) && (j == y))
            continue;

         if (bm.isBlack(i, j))
         {
            if (isBorder(bm, i, j))
            {
               if (!border_.contains(QPoint(i, j)))
                  sib << QPoint(i, j);
            }
         }
      }
   }
   return sib;
}

bool ResearchWindow::isBorder(const BoolMatrix & bm, int x, int y)
{
   int xs = (x == 0) ? 0 : x - 1;
   int xe = (x == bm.width() - 1) ? bm.width() - 1 : x + 1;
   int ys = (y == 0) ? 0 : y - 1;
   int ye = (y == bm.height() - 1) ? bm.height() - 1 : y + 1;
   int white_cnt = 0;
   for (int i = xs; i <= xe; i++)
   {
      for (int j = ys; j <= ye; j++)
      {
         if ((i == x) && (j == y))
            continue;
         
         if (bm.isWhite(i, j))
            white_cnt++;
      }
   }
   return white_cnt >= 2;
}

void ResearchWindow::findHoleCards()
{
   //поиск карманных карт
   QString path = "../CardBase/Cake/white/";
   QDir rsDir(path);
   QFileInfoList list = rsDir.entryInfoList();
   for (int i = 0; i < list.size(); ++i) 
   {
      QFileInfo fileInfo = list.at(i);
      QString fl = fileInfo.baseName();
      QImage imgOrigin(fileInfo.absoluteFilePath());
      if (imgOrigin.isNull())
         continue;

      //ProcCake * cardProc = new ProcCake();
      //cardProc->setImage(imgOrigin);
      //bool ok = false;
      //QPair<QString, QString> holeCards = cardProc->getHoleCards(&ok);
      //qDebug() << holeCards.first << holeCards.second;
      //if (fl.left(2) != holeCards.first.left(1) + holeCards.second.left(1))
      //   qDebug() << "Alarm!" << fl << holeCards.first.left(1) + holeCards.second.left(1);

      //continue;

      QImage img(imgOrigin);
      //преобразование в ч/б
      toBlackWhite(img, 117);
      //уменьшение
      QImage imgSmall = img.scaled(128, 128, Qt::KeepAspectRatio);
         
      //очистка от шума
      QImage imgDenoised(imgSmall);
      denoise(imgDenoised, 5);//5 точек в окрестности
      denoise(imgDenoised, 3);
      
      imgDenoised.save(path + "help/den_" + fl + ".bmp");
      
      //матрица из маленькой картинки
      BoolMatrix bmSmall(&imgDenoised);

      //поиск центра группирования белых точек
      QPoint center = whiteCenter(bmSmall);

      //поиск первого белого символа на черном фоне
      QRect fbRect = findFirstBlackRect(bmSmall);
      //обкронать со всех сторон
      fbRect = cutBlack(bmSmall, fbRect);

      //центр группирования должен лежать в области
      while (!fbRect.contains(center))
      {
         //залить область черным цветом
         bmSmall.fillRect(fbRect, true);
         //поиск первого белого символа на черном фоне
         fbRect = findFirstBlackRect(bmSmall);
         if (!fbRect.isValid())
            break;
         //обкронать со всех сторон
         fbRect = cutBlack(bmSmall, fbRect);
      }
      if (!fbRect.isValid())//не удалось найти область
         continue;

      //преобразование из уменьшенной в исходную точку
      qreal yq = (qreal)img.height() / (qreal)imgSmall.height();
      qreal xq = (qreal)img.width() / (qreal)imgSmall.width();

      QPoint topLeft = QPoint(qRound((fbRect.topLeft().x() - 2) * xq),
                         qRound((fbRect.topLeft().y() - 2) * yq));
      QPoint bottomRight = QPoint(qRound((fbRect.bottomRight().x() - 2) * xq),
                               qRound((fbRect.bottomRight().y() - 2) * yq));
      QRect origin_cutted(topLeft, bottomRight);

      //получили локализованную картинку 
      QImage locImage;// = img.copy(origin_cutted);
      
      if (img.width() < 600)
      {
         locImage = imgOrigin.copy(origin_cutted);
         toBlackWhite(locImage, 160);
      }
      else
      {
         locImage = img.copy(origin_cutted);
      }

      BoolMatrix bmLoc(&locImage);
      //первая карта
      QRect rectFirst = findFirstWhiteRect(bmLoc);
      
      //поиск второй карты
      //отрезать половину
      //найти максимальную черную вертикальную полосу и образать по ней
      int bm2_start = bmLoc.width() / 2;
      //справа найти максимальную белую полосу
      int bm2_stop = bmLoc.width();
      int max_white = 0;
      for (int x = bmLoc.width(); x > bm2_start; x--)
      {
         int whiteLine = 0;
         for (int y = 0; y < bmLoc.height(); y++)
         {
            if (bmLoc.isWhite(x, y))
            {
               whiteLine++;
            }
         }
         if (whiteLine > max_white)
         {
            max_white = whiteLine;
            bm2_stop = x;
         }
      }

      int max_black = 0;
      int max_black_x = -1;
      for (int x = bm2_start; x < bm2_stop; x++)
      {
         int blackLine = 0;
         for (int y = 0; y < bmLoc.height(); y++)
         {
            if (bmLoc.isBlack(x, y))
            {
               blackLine++;
            }
         }
         if (blackLine > max_black)
         {
            max_black = blackLine;
            max_black_x = x;
         }
      }
      
      QRect secondCardRect = QRect(max_black_x, 0, 
         locImage.width() - max_black_x, 
         locImage.height());

      QImage locSecondImage = locImage.copy(secondCardRect);
      BoolMatrix bmLocSecond(&locSecondImage);
      QRect rectSecond = findFirstWhiteRect(bmLocSecond);
      
      rectSecond.setRight(rectSecond.right() + max_black_x);
      rectSecond.setLeft(rectSecond.left() + max_black_x);

      //QPainter painter(&locImage);
      //painter.setPen(Qt::red);
      //painter.drawRect(rectFirst);
      //painter.drawRect(rectSecond);
      locImage.save(path + "help/loc_" + fl + ".bmp");

      QImage firstImg = locImage.copy(rectFirst);
      firstImg.save(path + "help/first_" + fl + ".bmp");
      
      QImage secondImg = locImage.copy(rectSecond);
      secondImg.save(path + "help/second_" + fl + ".bmp");

      //fillImage(firstImg, 0, 0);
      //firstImg.save(path + "help/first_fill_" + fl + ".bmp");

      //fillImage(secondImg, 0, 0);
      //secondImg.save(path + "help/scond_fill_" + fl + ".bmp");

      //подсчет замкнутых областей
      //BoolMatrix bmFirst(&firstImg);
      //QList<int> cnt = closedAreas(bmFirst);

      //чистим изображение от одиночных пикселей
      unpixel(&firstImg);
      unpixel(&secondImg);
      secondImg.save(path + "help/second_unpix_" + fl + ".bmp");

      CardOCR ocr;
      //подаем черно-белую
      QString card1 = ocr.nominal(&firstImg);
      QString card2 = ocr.nominal(&secondImg);
      qDebug() << card1 << card2;
      if (fl.left(2) != card1 + card2)
         qDebug() << "Alarm!" << fl << card1 + card2;

      //преобразовать к исходной картинке
      rectFirst.setRight(rectFirst.right() + origin_cutted.left());
      rectFirst.setLeft(rectFirst.left() + origin_cutted.left());
      rectFirst.setBottom(rectFirst.bottom() + origin_cutted.top());
      rectFirst.setTop(rectFirst.top() + origin_cutted.top());

      rectSecond.setRight(rectSecond.right() + origin_cutted.left());
      rectSecond.setLeft(rectSecond.left() + origin_cutted.left());
      rectSecond.setBottom(rectSecond.bottom() + origin_cutted.top());
      rectSecond.setTop(rectSecond.top() + origin_cutted.top());

      QString suit1 = ocr.suit(&firstImg, &imgOrigin.copy(rectFirst));
      QString suit2 = ocr.suit(&secondImg, &imgOrigin.copy(rectSecond));


   }
}

void ResearchWindow::fillImage(QImage & img, int x, int y)
{
   if (x < 0 || y < 0) 
      return;

   if (img.pixel(x, y) == qRgb(255, 255, 255))
   {
      img.setPixel(x, y, qRgb(0, 0, 255));
      fillImage(img, x, y - 1);
      fillImage(img, x - 1, y);
      fillImage(img, x, y + 1);
      fillImage(img, x + 1, y);
   }
}

QList<int> ResearchWindow::closedAreas(const BoolMatrix & bm)
{
   BoolMatrix bmBak = bm;
  
   int wCount = -1;
   int wx = 0;
   int wy = 0;

   QList<int> counts;
   while (wCount != 0)
   {
      fillMatrix(bmBak, wx, wy);
      
      wx = -1;
      wy = -1;
      int whiteCount = 0;
      for (int x = 0; x < bmBak.width(); x++)
      {
         for (int y = 0; y < bmBak.height(); y++)
         {
            if (bmBak.isWhite(x, y))
            {
               if (wx == -1)
                  wx = x;
               if (wy == -1)
                  wy = y;
               whiteCount++;
            }
         }
      }
      if (wCount != -1)
         counts << wCount - whiteCount;

      wCount = whiteCount;
   }
   
   return counts;
}

void ResearchWindow::fillMatrix(BoolMatrix & bm, int x, int y)
{
   if (x < 0 || y < 0) 
      return;

   if (bm.isWhite(x, y))
   {
      bm.set(x, y, true);
      fillMatrix(bm, x, y - 1);
      fillMatrix(bm, x - 1, y);
      fillMatrix(bm, x, y + 1);
      fillMatrix(bm, x + 1, y);
   }
}

QPoint ResearchWindow::whiteCenter(const BoolMatrix & bm)
{
   const int side = 6;
   QPoint ptWhite;
   int maxWhite = 0;
   for (int x = 0; x < bm.width(); x++)
   {
      for (int y = 0; y < bm.height(); y++)
      {
         if (bm.isWhite(x, y))
         {
            //построить окрестность
            int left   = (x - side > 0) ? x - side : 0;
            int right  = (x + side < bm.width()) ? x + side : bm.width() - 1;
            int top    = (y - side > 0) ? y - side : 0;
            int bottom = (y + side < bm.height()) ? y + side : bm.height() - 1;
            
            int whiteCount = 0;
            for (int i = left; i < right; i++)
            {
               for (int j = top; j < bottom; j++)
               {
                  if (bm.isWhite(i, j))
                     whiteCount++;
               }
            }
            if (whiteCount > maxWhite)
            {
               maxWhite = whiteCount;
               ptWhite.setX(x);
               ptWhite.setY(y);
            }
         }
      }
   }
   return ptWhite;
}

QRect ResearchWindow::cutBlack(const BoolMatrix & bm, const QRect & br)
{
   QRect res;
   for (int y = br.top(); y < br.bottom(); y++)
   {
      for (int x = br.left(); x < br.right(); x++)
      {
         if (bm.isWhite(x, y))
         {
            res.setBottom(y);
         }
      }
   }

   for (int y = br.bottom(); y > br.top(); y--)
   {
      for (int x = br.left(); x < br.right(); x++)
      {
         if (bm.isWhite(x, y))
         {
            res.setTop(y);
         }
      }
   }

   for (int x = br.left(); x < br.right(); x++)
   {
      for (int y = br.top(); y < br.bottom(); y++)
      {
         if (bm.isWhite(x, y))
         {
            res.setRight(x);
         }
      }
   }

   for (int x = br.right(); x > br.left(); x--)
   {
      for (int y = br.top(); y < br.bottom(); y++)
      {
         if (bm.isWhite(x, y))
         {
            res.setLeft(x);
         }
      }
   }

   return res;
}

void ResearchWindow::checkPreflop()
{
   //проверка стадии стола
   QString path = "../CardBase/Cake/tables/";
   QDir rsDir(path);
   QFileInfoList list = rsDir.entryInfoList();
   for (int i = 0; i < list.size(); ++i) 
   {
      QFileInfo fileInfo = list.at(i);
      QString fl = fileInfo.baseName();
      QImage img(fileInfo.absoluteFilePath());
      if (img.isNull())
         continue;

      BoolMatrix bm(img, 200);
      
      //взять центр и от него отложить квадрат 20 на 20
      int x_c = img.width() >> 1;
      int y_c = img.height() >> 1;
      const int side = 20;
      bool isPreflop = true;
      for (int x = x_c - side; x < x_c + side; x++)
      {
         for (int y = y_c - side; y < y_c + side; y++)
         {
            if (bm.at(x, y) == 0)//белый
            {
               isPreflop = false;
               y = y_c + side;
               x = x_c + side;
            }
         }
      }
      if (isPreflop)
      {
         qDebug() << fl << "preflop";
      }
      else
      {
         qDebug() << fl << "post flop";
      }
      
   }
}

void ResearchWindow::toBlackWhiteImages()
{
   QString path = "../CardBase/Cake/find/tmp1/";
   QDir rsDir(path);
   QFileInfoList list = rsDir.entryInfoList();
   for (int i = 0; i < list.size(); ++i) 
   {
      QFileInfo fileInfo = list.at(i);
      QString fl = fileInfo.baseName();
      QImage img(fileInfo.absoluteFilePath());
      if (img.isNull())
         continue;

      toBlackWhite(img, 200);

      img.save(path + "bw_" + fl + ".bmp");
   }
}

void ResearchWindow::setMainWidget8()
{
   QDir rsDir("../CardBase/RSP/masstest/");
   //QDir rsDir("../CardBase/RSP/test/");
   QFileInfoList list = rsDir.entryInfoList();
   
   for (int i = 0; i < list.size(); ++i) 
   {
      QFileInfo fileInfo = list.at(i);
      //if (fileInfo.baseName().count() != 2)
      //   continue;
      
      QString fl = fileInfo.baseName();
      QImage img(fileInfo.absoluteFilePath());
      if (img.isNull())
         continue;
      
      //img = img.scaled(20, 60);
      QImage im(img);
      //toBlackWhite(im, 50);


      toBlackWhiteMid(im, 128);
      //toBlackWhiteByHue(im);
      //im.save("../CardBase/RSP/split/wb/" + fl + "_wb.bmp");
      //continue;

      const int w = im.width();
      const int h = im.height();
      int y_split = 0;
      //поиск первой черной полосы
      int y_black = 0;
      for (int y = 1; y < h; ++y)
      {
         for (int x = 0; x < w; ++x)
         {
            if (im.pixel(x, y) == qRgb(0, 0, 0))
            {
               y_black = y;
               y = h;
               break;
            }
         }
      }
      //поиск первой белой полосы
      for (int y = y_black; y < h; ++y)
      {
         int cnt_wt = 0;
         for (int x = 0; x < w; ++x)
         {
            if (im.pixel(x, y) == qRgb(255, 255, 255))
            {
               cnt_wt++;
            }
            else
            {
               break;
            }
         }
         if (cnt_wt == w)
         {
            y_split = y;
            break;
         }
      }
      //обжимаем картинки слева и справа
      QImage imc  = im.copy(0, 1, w, y_split);
      //toBlackWhite(imc, 200);
      int left_wt = 0;
      for (int x = 0; x < imc.width(); ++x)
      {
         for (int y = 0; y < imc.height(); ++y)
         {
            if (imc.pixel(x, y) != qRgb(255, 255, 255))
            {
               x = imc.width() + 1;
               break;
            }
         }
         if (x != imc.width() + 1)
            left_wt++;
      }

      int right_wt = 0;
      for (int x = imc.width() - 1; x > 0; --x)
      {
         for (int y = 0; y < imc.height(); ++y)
         {
            if (imc.pixel(x, y) != qRgb(255, 255, 255))
            {
               x = -1;
               break;
            }
         }
         if (x != -1)
            right_wt++;
      }
      int lw = (left_wt == 0) ? 1 : left_wt;
      int rw = (right_wt == 0) ? 1 : right_wt;

      QImage top  = img.copy(left_wt, 1, imc.width() - left_wt - right_wt, y_split);
      QImage topwb = im.copy(left_wt, 1, imc.width() - left_wt - right_wt, y_split);
      //QImage btm = img.copy(0, y_split, w, y_split);
      //top.save("../CardBase/RSP/split/" + fl + "_nm.bmp");
      //btm.save("../CardBase/RSP/split/" + fl + "_su.bmp");

      //скелетизация
      //toBlackWhite(top, 128);
      //skelet_zs(top);
      topwb.save("../CardBase/RSP/split/" + fl + "_nm.bmp");
      top.save("../CardBase/RSP/split/" + fl + "_nm1.bmp");

      CardOCR ocr;
      //подаем черно-белую
      QString card = ocr.nominal(&topwb) + ocr.suit(&topwb, &top);
      qDebug() << fl.left(2) << card;
      if (fl.left(2) != card)
         qDebug() << "Alarm!" << fl;
      
      //подаем черно-белую и цветную
      //qDebug() << fl << ocr.suit(&topwb, &top);
   }
}

void ResearchWindow::skelet_zs(QImage & img)
{
   BoolMatrix bm(img, 128);
   
   toBlackWhite(img, 128);
   
   const int sz = 9;
   const int w = bm.width();
   const int h = bm.height();
   for (int x = 0; x < w; ++x)
   {
      for (int y = 0; y < h; ++y)
      {
         if (bm.at(x, y) == 0)
            continue;

         bool p[sz + 1] = {0};
                                      p[1] = bm.at(x, y);
         (y != 0)                   ? p[2] = bm.at(x, y - 1)      : 0;
         (y != 0 && x < w - 1)      ? p[3] = bm.at(x + 1, y - 1)  : 0;
         (x < w - 1)                ? p[4] = bm.at(x + 1, y)      : 0;
         (x < w - 1 && y < h - 1)   ? p[5] = bm.at(x + 1, y + 1)  : 0;
         (y < h - 1)                ? p[6] = bm.at(x, y + 1)      : 0;
         (x != 0 && y < h - 1)      ? p[7] = bm.at(x - 1, y + 1)  : 0;
         (x != 0)                   ? p[8] = bm.at(x - 1, y)      : 0;
         (x != 0 && y != 0)         ? p[9] = bm.at(x - 1, y - 1)  : 0;
         
         // сумма
         int B = 0;
         for (int i = 2; i <= sz; ++i)
         {
            B += p[i];
         }
         //c)	P2*P4*P6=0
         int C = p[2] * p[4] * p[6];
         //d)	P4*P6*P8=0
         int D = p[4] * p[6] * p[8];
         //число конфигураций 01 в последовательности  P2,P3,P4,P5,P6,P7,P8,P9 , замыкая  эту цепочку на Р2 ,т.е. вокруг этого пикселя существует только один переход от 0 к 1.
         int A = 0;
         for (int i = 2; i <= sz; ++i)
         {
            int j = (i == sz) ? 2 : i + 1;
            if (p[i] == 0 && p[j] == 1)
               A++;
         }
         
         if (B <= 6 && B >= 2 && A == 1 /*&& !C && !D*/)
         {
            img.setPixel(x, y, qRgb(255, 255, 255));
            bm.set(x, y, 0);
         }
      }
   }
   
   return;
   for (int x = 0; x < w; ++x)
   {
      for (int y = 0; y < h; ++y)
      {
         if (bm.at(x, y) == 0)
            continue;

         bool p[sz + 1] = {0};
                                      p[1] = bm.at(x, y);
         (y != 0)                   ? p[2] = bm.at(x, y - 1)      : 0;
         (y != 0 && x < w - 1)      ? p[3] = bm.at(x + 1, y - 1)  : 0;
         (x < w - 1)                ? p[4] = bm.at(x + 1, y)      : 0;
         (x < w - 1 && y < h - 1)   ? p[5] = bm.at(x + 1, y + 1)  : 0;
         (y < h - 1)                ? p[6] = bm.at(x, y + 1)      : 0;
         (x != 0 && y < h - 1)      ? p[7] = bm.at(x - 1, y + 1)  : 0;
         (x != 0)                   ? p[8] = bm.at(x - 1, y)      : 0;
         (x != 0 && y != 0)         ? p[9] = bm.at(x - 1, y - 1)  : 0;
         
         // сумма
         int B = 0;
         for (int i = 2; i <= sz; ++i)
         {
            B += p[i];
         }
         //c)	P2*P4*P6=0
         int C = p[2] * p[4] * p[8];
         //d)	P4*P6*P8=0
         int D = p[2] * p[6] * p[8];
         //число конфигураций 01 в последовательности  P2,P3,P4,P5,P6,P7,P8,P9 , замыкая  эту цепочку на Р2 ,т.е. вокруг этого пикселя существует только один переход от 0 к 1.
         int A = 0;
         for (int i = 2; i <= sz; ++i)
         {
            int j = (i == sz) ? 2 : i + 1;
            if (p[i] == 0 && p[j] == 1)
               A++;
         }
         
         if (B <= 6 && B >= 2 && A == 1 && !C && !D)
         {
            img.setPixel(x, y, qRgb(255, 255, 255));
            bm.set(x, y, 0);
         }
      }
   }

}

void ResearchWindow::skelet(QImage & img)
{
   const BoolMatrix bm(img, 128);
   const int w = bm.width();
   const int h = bm.height();
   for (int x = 0; x < w; ++x)
   {
      for (int y = 0; y < h; ++y)
      {
         const int sz = 9;
         bool p[sz] = {0};
         p[0] = bm.at(x, y);
         (x != 0) ? p[1] = bm.at(x - 1, y) : 0;
         ((x != 0) && (y < h - 1)) ? p[2] = bm.at(x - 1, y + 1) : 0;
         (y < h - 1) ? p[3] = bm.at(x, y + 1) : 0;
         ((x < w - 1) && (y < h - 1)) ? p[4] = bm.at(x + 1, y + 1) : 0;
         (x < w - 1) ? p[5] = bm.at(x + 1, y) : 0;
         ((x < w - 1) && (y != 0)) ? p[6] = bm.at(x + 1, y - 1) : 0;
         (y != 0) ? p[7] = bm.at(x, y - 1) : 0;
         ((x != 0) && (y != 0)) ? p[8] = bm.at(x - 1, y - 1) : 0;

         int A = 0;
         int B = 0;

         for (int k = 1; k < sz; k++)
         {
            if (p[k] == 0 && p[k+1] == 1 && k < 8)
            { 
               A++;
            }
            else if (p[k] == 1)
            {
               B++;
            }
         }

         if (p[8] == 0 && p[1] == 1)
         {
            A++;
         }

         bool cond1 = (B > 1 && B < 7);
         bool cond2 = (A == 1);
         bool cond3 = (p[1] * p[3] * p[5] == 0);
         bool cond4 = (p[1] * p[5] * p[7] == 0);
         if (cond1 && cond2 && cond3 && cond4)
         {
            img.setPixel(x, y, qRgb(255, 255, 255));
         }
      }
   }
}

void ResearchWindow::setMainWidget7()
{
   QImage img1(QString("../CommonSources/cards/test/Jd_blue.bmp"));
   QImage img2(QString("../CommonSources/cards/test/Jd_red.bmp"));
   img1 = img1.scaled(10, 20);
   img1 = img1.scaled(15, 29);

   BoolMatrix bm1(img1, 200);
   BoolMatrix bm2(img2, 200);
   
   QString str = bm1.toString();
   BoolMatrix bm3(str);

   int minDiff = img1.width() * img1.height();
   int minIdx = -1;
   QDir rsDir("../CardBase/Acad/");
   QFileInfoList list = rsDir.entryInfoList();
   
   for (int i = 0; i < list.size(); ++i) 
   {
      QFileInfo fileInfo = list.at(i);
      if (fileInfo.baseName().count() != 2)
         continue;
      
      QImage img(fileInfo.absoluteFilePath());
      if (img.isNull())
         continue;
      
      BoolMatrix bm(img, 200);
      int diff = bm3 - bm;
      if (diff < minDiff)
      {
         minDiff = diff;
         minIdx = i;
      }
   }
   
   QFileInfo fileInfo = list.at(minIdx);
   QString fl = fileInfo.baseName();
   

   int tt=0;tt;


}

void ResearchWindow::setMainWidget6()
{
   QImage img(QString("../CommonSources/cards/test/%1.bmp").arg(33));
   toBlackWhite(img, 235);
   img.save("1.bmp");
   return;
   //int idx = 5;
   //QImage img(QString("../CommonSources/cards/trouble/table_%1.bmp").arg(idx));
   ////toBlackWhite(img);
   ////img.save("1.bmp");
   //CardProcessing cardProc(CardProcessing::RedStarPoker);
   //cardProc.setImage(img);
   //QPair<QRect, QRect> hole = cardProc.getHoleCards();
   //img.copy(hole.first).save(QString("hole1_%1.bmp").arg(idx));
   //img.copy(hole.second).save(QString("hole2_%1.bmp").arg(idx));

   //return;
   for (int i = 1; i <= 6; i++)
   {
      QImage img(QString("../CommonSources/cards/test/rsp/rsp/%1.bmp").arg(i));
      //toBlackWhite(img);
      //img.save("1.bmp");
      //CardProcessing cardProc();
      //cardProc.setImage(img);
      //bool ok;
      //QPair<QRect, QRect> hole = cardProc.getHoleCards(&ok);
      //img.copy(hole.first).save(QString("hole1_%1.bmp").arg(i));
      //img.copy(hole.second).save(QString("hole2_%1.bmp").arg(i));
   }
}

void ResearchWindow::setMainWidget5()
{
   QImage img("../CommonSources/cards/trouble/table_4.bmp");
   //CardProcessing cardProc();
   //cardProc.setImage(img);
   //qDebug() << cardProc.isPreflop();
   //qDebug() << cardProc.holdemLevel();
   
}

void ResearchWindow::setMainWidget4()
{
   QImage img("../CommonSources/cards/test/wh1.bmp");

   const int minFoldHue = 15;
   const int maxFoldHue = 30;
   const int minCallHue = 60;
   const int maxCallHue = 70;
   const int minCheckHue = 42;
   const int maxCheckHue = 58;
   const int hCount = 1000;
   QPoint ptFold, ptCall, ptCheck;   

   //ищем кнопку Fold - если есть значит наш ход
   //bool hasFold = false;
   //int hueFold = hueCount(img, minFoldHue, maxFoldHue, img.width() / 2, 0, hCount, ptFold);
   //if (hueFold > hCount)
   //   hasFold = true;
   //
   ////проверяем рядом кнопку КОЛЛ
   //bool hasCall = false;
   //int hueCall = hueCount(img, minCallHue, maxCallHue, img.width() / 2, 0, hCount, ptCall);
   //if (hueCall > hCount)
   //   hasCall = true;
   
   //построение гистограммы и авто определение оттенка стола 
   //значение hue - количество
   //QMap<int, int> hueGist;
   int width = img.width();
   int height = img.height();
   for (int x = 0; x < width; x++)
   {
      for (int y = 0; y < height; y++)
      {
         QRgb rgb = img.pixel(x, y);
         QColor cl(rgb);
         qDebug() << cl.value();
         
         int hu = cl.hue();
         //qDebug() << hu;
         //if (hu != -1)
         //   hueGist[hu]++;
      }
   }
   return;

   int maxHueCount = 0;
   int maxHueValue = 0;
   //foreach (int hue, hueGist.keys())
   //{
   //   if (hueGist[hue] > maxHueCount)
   //   {
   //      maxHueCount = hueGist[hue];
   //      maxHueValue = hue;
   //   }
   //}

   //отложим по +/- 10
   int minHue = maxHueValue - 10;
   int maxHue = maxHueValue + 10;

   for (int w = 0; w < width; w++)
   {
      for (int h = 0; h < height; h++)
      {
         QRgb rgb = img.pixel(w, h);
         QColor cl(rgb);
         //qDebug() << cl.hue();

         if (cl.hue() >= 13/*minHue*/ && cl.hue() <= 32/*maxHue*/) 
         {
            img.setPixel(w, h, qRgb(0, 0, 0));
            //matrix.set(w, h, true);
         }
         else
         {
            img.setPixel(w, h, qRgb(255, 255, 255));
            //matrix.set(w, h, false);
         }
      }

   }
   
   img.save("11.bmp");

   //qDebug() << maxHue;

}

void ResearchWindow::setMainWidget3()
{
   //определение статуса - префлоп /флоп
   //определяем центр
   QImage img("../CommonSources/cards/test/acad2.bmp");
   int width = img.width();
   int height = img.height();

   BoolMatrix matrix(img);
   for (int w = 0; w < img.width(); w++)
   {
      for (int h = 0; h < img.height(); h++)
      {
         QRgb rgb = img.pixel(w, h);
         QColor cl(rgb);
         //qDebug() << cl.hue();

         if (cl.hue() > -10/*125 */&& cl.hue() < 10/*155*/) 
         {
            //img.setPixel(w, h, qRgb(0, 0, 0));
            matrix.set(w, h, true);
         }
         else
         {
            //img.setPixel(w, h, qRgb(255, 255, 255));
            matrix.set(w, h, false);
         }
      }

   }
   
   //img.save("11.bmp");
   //return;

   //нарисуем крест для начала
   //qreal factor = qreal(height)/qreal(width);
   //for (int w = 0; w < width; w++)
   //{
   //   int h = int(w * factor);
   //   if (h < height)
   //   {
   //      img.setPixel(w, h, qRgb(255, 0, 0));
   //      img.setPixel(width - w - 1, h, qRgb(255, 0, 0));
   //   }
   //}
   //начинаем движение
   const int rectSize = 32;
   const int maxSize = 32 * 32;
   //отсекаем сверху и снизу чтобы остался только стол
   int width2 = width / 2;
   QPoint topTable;
   for (int h = 0; h < height; h += 16)
   {
      int blackCount = 0;
      for (int x = width2 - 16; x < width2 + 16; x++)
      {
         for (int y = h; y < h + rectSize; y++)
         {
            if (matrix.at(x, y) == true)
               blackCount++;
         }
      }
      //в процентном отношении - больше 33,3333 % должно быть заполнено
      if (blackCount > maxSize / 3)
      {
         //оно
         topTable.setX(width2);
         topTable.setY(h);
         break;
      }
   }

   //и снизу
   QPoint bottomTable;
   for (int h = height - rectSize - 1; h >= 0; h -= 16)
   {
      int blackCount = 0;
      for (int x = width2 - 16; x < width2 + 16; x++)
      {
         for (int y = h; y < h + rectSize; y++)
         {
            if (matrix.at(x, y) == true)
               blackCount++;
         }
      }
      //в процентном отношении - больше 33,3333 % должно быть заполнено
      if (blackCount > maxSize / 3)
      {
         //оно
         bottomTable.setX(width2);
         bottomTable.setY(h);
         break;
      }
   }
   //также слева и справа
   int height2 = height / 2;
   QPoint leftTable;
   for (int w = 0; w < width; w += 16)
   {
      int blackCount = 0;
      for (int x = w; x < w + rectSize; x++)
      {
         for (int y = height2 - 16; y < height2 + 16; y++)
         {
            if (matrix.at(x, y) == true)
               blackCount++;
         }
      }
      //в процентном отношении - больше 33,3333 % должно быть заполнено
      if (blackCount > maxSize / 3)
      {
         //оно
         leftTable.setX(w + 16);
         leftTable.setY(height2);
         break;
      }
   }

   //и справа
   QPoint rightTable;
   for (int w = width - rectSize - 1; w >= 0; w -= 16)
   {
      int blackCount = 0;
      for (int x = w; x < w + rectSize; x++)
      {
         for (int y = height2 - 16; y < height2 + 16; y++)
         {
            if (matrix.at(x, y) == true)
               blackCount++;
         }
      }
      //в процентном отношении - больше 33,3333 % должно быть заполнено
      if (blackCount > maxSize / 3)
      {
         //оно
         rightTable.setX(w - 16);
         rightTable.setY(height2);
         break;
      }
   }


   //смотрим
   QRect table(leftTable.x(), topTable.y(),
      rightTable.x() - leftTable.x(),
      bottomTable.y() - topTable.y());
   img.copy(table).save("11.bmp");
   return;

   //qreal factor = qreal(height)/qreal(width);
   QRect rect;
   for (int w = 0; w < width; w += 16)
   {
      //координаты левого верхнего угла
      //int h = int(w * factor);
      int h = w;
      if (h >= height)
         break;
      int blackCount = 0;
      for (int x = w; x < w + rectSize; x++)
      {
         for (int y = h; y < h + rectSize; y++)
         {
            if (matrix.at(x, y) == true)
               blackCount++;
         }
      }
      //в процентном отношении - больше 33,3333 % должно быть заполнено
      if (blackCount > maxSize / 3)
      {
         //оно
         rect.setX(w);
         rect.setY(h);
         rect.setWidth(rectSize);
         rect.setHeight(rectSize);
         break;
      }
   }
   QPoint leftPoint = rect.bottomRight();
   // высчитываем правую точку
   //QPoint rightPoint = leftPoint;
   //rightPoint.setX(width - leftPoint.x());
   ////высчитываем левую нижнюю точку

   //QPoint leftBottom = leftPoint;
   //leftBottom.setY(height - leftPoint.y());

   //и правую нижнюю
   QPoint rightBottom(width - leftPoint.x(), height - leftPoint.y());

   QRect flopRect(leftPoint, rightBottom);
   img.copy(flopRect).save("11.bmp");
}

void ResearchWindow::setMainWidget2()
{
   typedef QList<QPoint> Border;

   QImage img("../CommonSources/cards/trouble/table_5.bmp");
   
   int width  = img.width();
   int height = img.height();
   bool **matrix = new bool*[width];
   for (int w = 0; w < width; w++)
   {  
      matrix[w] = new bool[height];
   }
   for (int w = 0; w < width; w++)
   {
      for (int h = 0; h < height; h++)
      {
         QRgb rgb = img.pixel(w, h);
         if (qGray(rgb) < 240)
         {
            matrix[w][h] = 1;//черный
         }
         else
         {
            matrix[w][h] = 0;//белый
         }
      }
   }

   //поиск области с максимальным весовым коэффициентом
   const int dim = 32;
   const int dim2 = 16;
   QRect rect;
   int minWeight = dim * dim;
   for (int w = 0; w < width - dim; w+=dim2)
   {
      for (int h = 0; h < height; h+=dim2)
      {
         int sum = 0;
         for (int x = 0; x < dim2; x++)
         {
            for (int y = 0; y < dim2; y++)
            {
               sum += matrix[w + x][h + y];
            }
         }
         int weight = sum; 
         if (weight < minWeight)
         {
            minWeight = weight;
            rect.setX(w);
            rect.setY(h);
            rect.setWidth(dim);
            rect.setHeight(dim);
         }
      }
   }

   //получили наиболее белую область 
   /*
   относительно правого верхнего угла найдем ближайшие точки границы
   */
   //img.copy(rect).save("3.bmp");
   //return;
   
   //поиск граничных переходов
   /*
   матрица 5х5
   1 1 1 x 0
   1 1 1 x 0
   1 1 1 x 0
   1 1 1 x 0
   1 1 1 x 0
   */
   const int sz = 5;

   bool **border_matrix = new bool*[sz];
   for (int w = 0; w < sz; w++)
   {
      border_matrix[w] = new bool[sz];
   }
   border_matrix[0][0] = 1;
   border_matrix[0][1] = 1;
   border_matrix[0][2] = 1;
   border_matrix[0][3] = 1;
   border_matrix[0][4] = 1;

   border_matrix[1][0] = 1;
   border_matrix[1][1] = 1;
   border_matrix[1][2] = 1;
   border_matrix[1][3] = 1;
   border_matrix[1][4] = 1;

   border_matrix[2][0] = 1;
   border_matrix[2][1] = 1;
   border_matrix[2][2] = 1;
   border_matrix[2][3] = 1;
   border_matrix[2][4] = 1;

   border_matrix[3][0] = 1;
   border_matrix[3][1] = 1;
   border_matrix[3][2] = 1;
   border_matrix[3][3] = 1;
   border_matrix[3][4] = 1;

   border_matrix[4][0] = 0;
   border_matrix[4][1] = 0;
   border_matrix[4][2] = 0;
   border_matrix[4][3] = 0;
   border_matrix[4][4] = 0;

   Border borders;
   for (int w = 10; w < width - 10; w++)
   {
      for (int h = 10; h < height - 10; h++)
      {
         int matches = 0;
         for (int x = -3; x < 2; x++)
         {
            for (int y = -2;  y <= 2; y++)
            {
               if (matrix[w + x][h + y] ==
                   border_matrix[x + 3][y + 2])
                   matches++;
            }
         }
         if (matches == sz*sz)
         {
            borders << QPoint(w, h);
         }
      }
   }
   
   toBlackWhite(img);
   //foreach (QPoint pt, borders)
   //{
   //   img.setPixel(pt, qRgb(0, 255, 0));
   //}

   //img.save("4.bmp");
   //return;
   //построить массив связных линий
   /*
   QList<QPoint> - определяет границу
   */
   //typedef QList<QPoint> Border;
   //список вертикальных границ
   QList<Border> vertBorders;

   for (int b = 0; b < borders.count(); b++)
   {
      const QPoint & pt = borders.at(b);
      Border brd;
      brd << pt;
      for (int y = pt.y() + 1; y < height; y++)
      {
         QPoint dwn = pt;
         dwn.setY(y);
         if (borders.contains(dwn))
         {
            brd << dwn;
            //выкинуть точку dwn т.к. она будет входить в границу
            borders.removeOne(dwn);
         }
         else
         {
            break;
         }
      }
      if (brd.count() > 20)
      {
         vertBorders << brd;
      }
   }
   
   foreach (Border brd, vertBorders)
   {
      foreach (QPoint pt, brd)
      {
         img.setPixel(pt, qRgb(0, 255, 0));
      }
   }   
   //   
   //return;
   //поиск граничных переходов
   /*
   матрица 5х5
   1 1 X 1 1
   1 1 X 1 1
   1 1 Х 1 1
   1 1 X 1 1
   0 0 X 0 0
   */
   bool **border_matrix1 = new bool*[sz];
   for (int w = 0; w < sz; w++)
   {
      border_matrix1[w] = new bool[sz];
   }
   border_matrix1[0][0] = 1;
   border_matrix1[0][1] = 1;
   border_matrix1[0][2] = 1;
   border_matrix1[0][3] = 1;
   border_matrix1[0][4] = 0;

   border_matrix1[1][0] = 1;
   border_matrix1[1][1] = 1;
   border_matrix1[1][2] = 1;
   border_matrix1[1][3] = 1;
   border_matrix1[1][4] = 0;

   border_matrix1[2][0] = 1;
   border_matrix1[2][1] = 1;
   border_matrix1[2][2] = 1;
   border_matrix1[2][3] = 1;
   border_matrix1[2][4] = 0;

   border_matrix1[3][0] = 1;
   border_matrix1[3][1] = 1;
   border_matrix1[3][2] = 1;
   border_matrix1[3][3] = 1;
   border_matrix1[3][4] = 0;

   border_matrix1[4][0] = 1;
   border_matrix1[4][1] = 1;
   border_matrix1[4][2] = 1;
   border_matrix1[4][3] = 1;
   border_matrix1[4][4] = 0;

   borders.clear();
   for (int w = 10; w < width - 10; w++)
   {
      for (int h = 10; h < height - 10; h++)
      {
         int matches = 0;
         for (int x = -2; x <= 2; x++)
         {
            for (int y = -3;  y < 2; y++)
            {
               if (matrix[w + x][h + y] ==
                   border_matrix1[x + 2][y + 3])
                   matches++;
            }
         }
         if (matches == sz*sz)
         {
            borders << QPoint(w, h);
         }
      }
   }

   //список горизонтальных границ
   QList<Border> horzBorders;

   for (int b = 0; b < borders.count(); b++)
   {
      const QPoint & pt = borders.at(b);
      Border brd;
      brd << pt;
      for (int x = pt.x() + 1; x < width; x++)
      {
         QPoint lft = pt;
         lft.setX(x);
         if (borders.contains(lft))
         {
            brd << lft;
            //выкинуть точку lft
            borders.removeOne(lft);
         }
         else
         {
            break;
         }
      }
      if (brd.count() > 2)
         horzBorders << brd;
   }
   
   foreach (Border brd, horzBorders)
   {
      foreach (QPoint pt, brd)
      {
         img.setPixel(pt, qRgb(0, 255, 0));
      }
   }   
   img.save("2.bmp");
   //return;

   //ищем минимальное растояние до левой границы
   QPoint tl = rect.topLeft();
   qreal minDist = 0;
   int minVertIdx = -1;
   for (int i = 0; i < vertBorders.count(); i++)
   {
      Border brd = vertBorders.at(i);
      QPoint fst = brd.first();
      QPoint lst = brd.last();
      
      //точка границы должна быть левее
      //if (fst.x() >= tl.x())
      //   continue;

      qreal r1 = qSqrt(qPow(tl.x() - fst.x(), 2) + qPow(tl.y() - fst.y(), 2));
      qreal r2 = qSqrt(qPow(tl.x() - lst.x(), 2) + qPow(tl.y() - lst.y(), 2));
      qreal sum = r1 + r2;
      if (minDist == 0)
      {
         minDist = sum;
         minVertIdx = i;
      }
      else
      {
         if (sum < minDist)
         {
            minDist = sum;
            minVertIdx = i;
         }
      }
   }
   
   Border vBorder = vertBorders.at(minVertIdx);   
   // то же самое для горизонтальной границы
   //относительно верт границы ставим квадрат и ищем точку гориз границы которыа в него попадает
   //ищем к ней парную горизонтальную границу
   //определяем в ней область поиска
   int minHorzIdx = -1;
   QPoint bp = vBorder.first();
   QRect frect(bp.x(), bp.y() - 8, 8, 8);
   for (int i = 0; i < horzBorders.count(); i++)
   {
      Border hborder = horzBorders.at(i);
      //нашлась горизонтальная граница в близкой окрестности
      if (frect.contains(hborder.first()))
      {
         minHorzIdx = i;
         break;
      }
   }
   






   //minDist = 0;
   //int minHorzIdx = -1;
   //for (int i = 0; i < horzBorders.count(); i++)
   //{
   //   Border brd = horzBorders.at(i);
   //   QPoint fst = brd.first();
   //   QPoint lst = brd.last();
   //   //точка границы должна быть выше верхней точки вертикальной границы
   //   if (fst.y() >= vBorder.first().y())
   //      continue;
   //   
   //   qreal r1 = qSqrt(qPow(tl.x() - fst.x(), 2) + qPow(tl.y() - fst.y(), 2));
   //   qreal r2 = qSqrt(qPow(tl.x() - lst.x(), 2) + qPow(tl.y() - lst.y(), 2));
   //   qreal sum = r1 + r2;
   //   if (minDist == 0)
   //   {
   //      minDist = sum;
   //      minHorzIdx = i;
   //   }
   //   else
   //   {
   //      if (sum < minDist)
   //      {
   //         minDist = sum;
   //         minHorzIdx = i;
   //      }
   //   }
   //}

   //для границ находим их пересечение
   Border hBorder = horzBorders.at(minHorzIdx);   
   QRect r;
   r.setX(vBorder.first().x() + 1);
   r.setY(hBorder.first().y() + 1);
   r.setWidth(32);
   r.setHeight(32);
   
   //toBlackWhite(img);
   //img.save("2.bmp");
   //ищем границы карт 
   //ищем вертикальную границу второй карты
   int cardWidth = 5;
   int minDHeight = vBorder.count() * 75 / 100;
   for (int x = hBorder.first().x() + cardWidth; x < hBorder.first().x() + 32; x++)
   {
      int blackHeight = 0;
      for (int y = vBorder.first().y(); y < vBorder.last().y(); y++)
      {
         if (matrix[x][y] == 1)//black
            blackHeight++;
      }
      if (blackHeight >= minDHeight)
      {
         //its
         cardWidth = x - vBorder.first().x() - 1;
         break;
      }
   }
   r.setWidth(cardWidth);

   //затем пробегаем снизу от низа границы до тех пор пока не закончится белая полоса
   int cardHeight = 0;
   for (int y = vBorder.last().y() - 10; y >= vBorder.first().y(); y--)
   {
      for (int x = vBorder.first().x() + 2; x < vBorder.first().x() + cardWidth; x++)
      {
         if (matrix[x][y] == 1)
         {
            //попалась черная точка
            cardHeight = y - hBorder.first().y();
            y = 0;
            break;
         }
      }
   }
   //примерно тем же макаром ищем вторую карту
   r.setHeight(cardHeight);
   img.copy(r).save("4.bmp");
   //img.save("2.bmp");
}


void ResearchWindow::setMainWidget()
{
   ////считать изображение
   //ulong64 hash = 0;
   //ph_dct_imagehash("..\\CommonSources\\cards\\test\\test1.bmp", hash);
   //
   ////QImage img("../CommonSources/cards/test/2cbig2.bmp");
   ////qint64 hash = solveHash(img);

   //QDir rsDir("../CommonSources/cards/redstar/");
   //QFileInfoList list = rsDir.entryInfoList();
   //
   //for (int i = 0; i < list.size(); ++i) 
   //{
   //   QFileInfo fileInfo = list.at(i);
   //   if (fileInfo.baseName().count() != 2)
   //      continue;
   //   
   //   //QImage img1(fileInfo.absoluteFilePath());
   //   //qint64 hash1 = solveHash(img1);
   //   //short d = compareHash(hash, hash1);

   //   ulong64 hash1 = 0;
   //   ph_dct_imagehash(fileInfo.absoluteFilePath().toStdString().c_str(), hash1);
   //   int d = ph_hamming_distance(hash, hash1);
   //   qDebug() << fileInfo.baseName() << d;
   //}
   
   

   //ulong64 hash2 = 0;
   //ph_dct_imagehash("..\\CommonSources\\cards\\redstar\\2c.bmp", hash2);

   //int d1 = ph_hamming_distance(hash, hash1);
   //int d2 = ph_hamming_distance(hash, hash2);
   //int t = 0;
   //t;
   //что имеем
   //QImage img1("../CommonSources/cards/test/2cbig_thr.bmp");
   ////отсечь помехи по краям
   ////cutImage(img1);

   ////return;
   //qint64 hash = solveHash(img1);
   //
   ////сравнить с базой данных
   //DBManager * dbManager = new DBManager();
   //if (!dbManager->isGood())
   //{
   //   return;
   //}
   //short minDist = 64;
   //short minIdx = 0;
   //QSqlQuery select_query(dbManager->database());
   //select_query.prepare("SELECT * FROM REDSTAR");
   //if (select_query.exec())
   //{
   //   while (select_query.next())
   //   {
   //      QSqlRecord rec = select_query.record();
   //      qint64 chash = rec.value("HASH").toLongLong();
   //      short res = compareHash(hash, chash);
   //      if (res < minDist)
   //      {
   //         minDist = res;
   //         minIdx = rec.value("ID").toInt();
   //      }
   //   }
   //}
   //QString err = dbManager->database().lastError().text();
   //select_query.finish();
   //QString what;
   //select_query.prepare("SELECT NOMINAL, SUIT FROM REDSTAR WHERE ID=:id");
   //select_query.bindValue(":id", minIdx);
   //if (select_query.exec())
   //{
   //   if (select_query.first())
   //   {
   //      QSqlRecord rec = select_query.record();
   //      what = cardString(rec.value("NOMINAL").toInt(), rec.value("SUIT").toInt());
   //   }
   //}

   int tt=0;
   tt;
}

QString ResearchWindow::cardString(int nom, int suit)
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

void ResearchWindow::cutImage(QImage & img)
{
   //проверяем по вертикали
   int border = 128;
   int w = img.width();
   int h = img.height();
   for (int x = 0; x < w; x++)
   {
      int blackPixels = 0;
      for (int y = 0; y < h; y++)
      {
         if (qGray(img.pixel(x, y)) < border)
         {
            blackPixels++;
         }
      }
      if (blackPixels > h - 5)
      {
         //убрать эту линию
         img = img.copy(1, 0, w - 1, h);
         x--;
         w--;
      }
   }
   
   //проверяем по горизонтали
   w = img.width();
   for (int y = 0; y < h; y++)
   {
      int blackPixels = 0;
      for (int x = 0; x < w; x++)
      {
         if (qGray(img.pixel(x, y)) < border)
         {
            blackPixels++;
         }
      }
      if (blackPixels > w - 5)
      {
         //убрать эту линию
         img = img.copy(0, 1, w, h - 1);
         y--;
         h--;
      }
   }
}

void ResearchWindow::unpixel(QImage * image)
{
   BoolMatrix bm(image);
   for (int x = 0; x < bm.width(); x++)
   {
      for (int y = 0; y < bm.height(); y++)
      {
         if (bm.isBlack(x, y))
         {
            if (isTopOne(bm, x, y) || isLeftOne(bm, x, y) ||
                isRightOne(bm, x, y) || isBottomOne(bm, x, y))
            {
               image->setPixel(x, y, qRgb(255, 255, 255));
            }
         }
      }
   }
}

bool ResearchWindow::isTopOne(const BoolMatrix & bm, int cx, int cy)
{
   QPoint left(cx - 1, cy);
   QPoint lefttop(cx - 1, cy - 1);
   QPoint top(cx, cy - 1);
   QPoint topright(cx + 1, cy - 1);
   QPoint right(cx + 1, cy);
   
   bool isLeftWhite  = (cx == 0) ? true : false;
   bool isLTWhite    = (cx == 0 || cy == 0) ? true : false;
   bool isTopWhite   = (cy == 0) ? true : false;
   bool isTRWhite    = (cy == 0 || cx == bm.width() - 1) ? true : false;
   bool isRightWhite = (cx == bm.width() - 1) ? true : false;
   
   if (!isLeftWhite && bm.isWhite(left.x(), left.y()))
   {
      isLeftWhite = true;
   }
   if (!isLTWhite && bm.isWhite(lefttop.x(), lefttop.y()))
   {
      isLTWhite = true;
   }
   if (!isTopWhite && bm.isWhite(top.x(), top.y()))
   {
      isTopWhite = true;
   }
   if (!isTRWhite && bm.isWhite(topright.x(), topright.y()))
   {
      isTRWhite = true;
   }
   if (!isRightWhite && bm.isWhite(right.x(), right.y()))
   {
      isRightWhite = true;
   }

   if (isLeftWhite && isLTWhite && isTopWhite && isTRWhite && isRightWhite)
   {
      return true;
   }
   return false;
}

bool ResearchWindow::isLeftOne(const BoolMatrix & bm, int cx, int cy)
{
   QPoint bottom(cx, cy + 1);
   QPoint bottomleft(cx - 1, cy + 1);
   QPoint left(cx - 1, cy);
   QPoint lefttop(cx - 1, cy - 1);
   QPoint top(cx, cy - 1);
   
   bool isBottomWhite  = (cy == bm.height() - 1) ? true : false;
   bool isBLWhite    = (cx == 0 || cy == bm.height() - 1) ? true : false;
   bool isLeftWhite   = (cx == 0) ? true : false;
   bool isLTWhite    = (cy == 0 || cx == 0) ? true : false;
   bool isTopWhite = (cy == 0) ? true : false;
   
   if (!isBottomWhite && bm.isWhite(bottom.x(), bottom.y()))
   {
      isBottomWhite = true;
   }
   if (!isBLWhite && bm.isWhite(bottomleft.x(), bottomleft.y()))
   {
      isBLWhite = true;
   }
   if (!isLeftWhite && bm.isWhite(left.x(), left.y()))
   {
      isLeftWhite = true;
   }
   if (!isLTWhite && bm.isWhite(lefttop.x(), lefttop.y()))
   {
      isLTWhite = true;
   }
   if (!isTopWhite && bm.isWhite(top.x(), top.y()))
   {
      isTopWhite = true;
   }

   if (isBottomWhite && isBLWhite && isLeftWhite && isLTWhite && isTopWhite)
   {
      return true;
   }
   return false;
}

bool ResearchWindow::isRightOne(const BoolMatrix & bm, int cx, int cy) 
{
   QPoint top(cx, cy - 1);
   QPoint topright(cx + 1, cy - 1);
   QPoint right(cx + 1, cy);
   QPoint rightbottom(cx + 1, cy + 1);
   QPoint bottom(cx, cy + 1);
   
   bool isTopWhite  = (cy == 0) ? true : false;
   bool isTRWhite    = (cx == bm.width() - 1 || cy == 0) ? true : false;
   bool isRightWhite   = (cx == bm.width() - 1) ? true : false;
   bool isRBWhite    = (cx == bm.width() - 1 || cy == bm.height() - 1) ? true : false;
   bool isBottomWhite = (cy == bm.height() - 1) ? true : false;
   
   if (!isTopWhite && bm.isWhite(top.x(), top.y()))
   {
      isTopWhite = true;
   }
   if (!isTRWhite && bm.isWhite(topright.x(), topright.y()))
   {
      isTRWhite = true;
   }
   if (!isRightWhite && bm.isWhite(right.x(), right.y()))
   {
      isRightWhite = true;
   }
   if (!isRBWhite && bm.isWhite(rightbottom.x(), rightbottom.y()))
   {
      isRBWhite = true;
   }
   if (!isBottomWhite && bm.isWhite(bottom.x(), bottom.y()))
   {
      isBottomWhite = true;
   }

   if (isTopWhite && isTRWhite && isRightWhite && isRBWhite && isBottomWhite)
   {
      return true;
   }
   return false;
}

bool ResearchWindow::isBottomOne(const BoolMatrix & bm, int cx, int cy)
{
   QPoint left(cx - 1, cy);
   QPoint leftbottom(cx - 1, cy + 1);
   QPoint bottom(cx, cy + 1);
   QPoint bottomright(cx + 1, cy + 1);
   QPoint right(cx + 1, cy);
   
   bool isLeftWhite  = (cx == 0) ? true : false;
   bool isLBWhite    = (cx == 0 || cy == bm.height() - 1) ? true : false;
   bool isBottomWhite = (cy == bm.height() - 1) ? true : false;
   bool isBRWhite    = (cx == bm.width() - 1 || cy == bm.height() - 1) ? true : false;
   bool isRightWhite = (cx == bm.width() - 1) ? true : false;
   
   if (!isLeftWhite && bm.isWhite(left.x(), left.y()))
   {
      isLeftWhite = true;
   }
   if (!isLBWhite && bm.isWhite(leftbottom.x(), leftbottom.y()))
   {
      isLBWhite = true;
   }
   if (!isBottomWhite && bm.isWhite(bottom.x(), bottom.y()))
   {
      isBottomWhite = true;
   }
   if (!isBRWhite && bm.isWhite(bottomright.x(), bottomright.y()))
   {
      isBRWhite = true;
   }
   if (!isRightWhite && bm.isWhite(right.x(), right.y()))
   {
      isRightWhite = true;
   }

   if (isLeftWhite && isLBWhite && isBottomWhite && isBRWhite && isRightWhite)
   {
      return true;
   }
   return false;
}
