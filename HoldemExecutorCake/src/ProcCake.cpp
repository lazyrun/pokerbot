
#include "ProcCake.h"
#include "CardOCR_Cake.h"

//
//ProcCake
//
ProcCake::ProcCake()
: CardProcessing()
{
   threshold_ = 220;
   matrix_ = 0;
   img_ = 0;
}

ProcCake::~ProcCake()
{
   delete matrix_;
   delete img_;
}

void ProcCake::setImage(const QImage & img)
{
   delete matrix_;
   matrix_ = 0;
   matrix_ = new BoolMatrix(img, threshold_);
   
   delete img_;
   img_ = new QImage(img);

   BoolMatrix & bm = *matrix_;
   //����� ����� � �� ���� �������� ������� 20 �� 20
   int x_c = img.width() >> 1;
   int y_c = img.height() >> 1;
   const int side = 20;
   bool isPreflop = true;
   for (int x = x_c - side; x < x_c + side; x++)
   {
      for (int y = y_c - side; y < y_c + side; y++)
      {
         if (bm.at(x, y) == 0)//�����
         {
            isPreflop = false;
            y = y_c + side;
            x = x_c + side;
         }
      }
   }
   
   if (isPreflop)
   {
      holdemLevel_ = Preflop;
   }
   else
   {
      holdemLevel_ = Flop;
   }
}

QPair<QString, QString> ProcCake::getHoleCards(bool * ok)
{
   if (matrix_ == 0)
   {
      *ok = false;
      return qMakePair<QString, QString>(QString(), QString());
   }

   QImage imgOrigin(*img_);
   QImage & img = *img_;
   //�������������� � �/�
   toBlackWhite(img, 117);
   //����������
   QImage imgSmall = img.scaled(128, 128, Qt::KeepAspectRatio);
      
   //������� �� ����
   QImage imgDenoised(imgSmall);
   denoise(imgDenoised, 5);//5 ����� � �����������
   denoise(imgDenoised, 3);
   
   //imgDenoised.save(path + "den_" + fl + ".bmp");
   
   //������� �� ��������� ��������
   BoolMatrix bmSmall(&imgDenoised);

   //����� ������ ������������� ����� �����
   QPoint center = whiteCenter(bmSmall);

   //����� ������� ������ ������� �� ������ ����
   QRect fbRect = findFirstBlackRect(bmSmall);
   //��������� �� ���� ������
   fbRect = cutBlack(bmSmall, fbRect);

   //����� ������������� ������ ������ � �������
   while (!fbRect.contains(center))
   {
      //������ ������� ������ ������
      bmSmall.fillRect(fbRect, true);
      //����� ������� ������ ������� �� ������ ����
      fbRect = findFirstBlackRect(bmSmall);
      if (!fbRect.isValid())
         break;
      //��������� �� ���� ������
      fbRect = cutBlack(bmSmall, fbRect);
   }
   if (!fbRect.isValid())//�� ������� ����� �������
   {
      *ok = false;
      return qMakePair<QString, QString>(QString(), QString());
   }

   //�������������� �� ����������� � �������� �����
   qreal yq = (qreal)img.height() / (qreal)imgSmall.height();
   qreal xq = (qreal)img.width() / (qreal)imgSmall.width();

   QPoint topLeft = QPoint(qRound((fbRect.topLeft().x() - 2) * xq),
                      qRound((fbRect.topLeft().y() - 2) * yq));
   QPoint bottomRight = QPoint(qRound((fbRect.bottomRight().x() - 2) * xq),
                            qRound((fbRect.bottomRight().y() - 2) * yq));
   QRect origin_cutted(topLeft, bottomRight);

   //�������� �������������� �������� 
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
   
   //QImage locImage = imgOrigin.copy(origin_cutted);
   //toBlackWhite(locImage, 128);

   BoolMatrix bmLoc(&locImage);
   //������ �����
   QRect rectFirst = findFirstWhiteRect(bmLoc);
   
   //����� ������ �����
   //�������� ��������
   //����� ������������ ������ ������������ ������ � �������� �� ���
   int bm2_start = bmLoc.width() / 2;
   //������ ����� ������������ ����� ������
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

   QPainter painter(&locImage);
   painter.setPen(Qt::red);

   painter.drawRect(rectFirst);
   painter.drawRect(rectSecond);
   //locImage.save("test/help/loc.bmp");

   QImage firstImg = locImage.copy(rectFirst);
   //firstImg.save("test/help/first.bmp");
   
   QImage secondImg = locImage.copy(rectSecond);
   //secondImg.save("test/help/second.bmp");

   CardOCR ocr;
   //������ �����-�����
   QString card1 = ocr.nominal(&firstImg);
   QString card2 = ocr.nominal(&secondImg);

   //������������� � �������� ��������
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

   *ok = true;
   return qMakePair<QString, QString>(card1 + suit1, card2 + suit2);
}

QPoint ProcCake::whiteCenter(const BoolMatrix & bm)
{
   const int side = 10;
   QPoint ptWhite;
   int maxWhite = 0;
   for (int x = 0; x < bm.width(); x++)
   {
      for (int y = 0; y < bm.height(); y++)
      {
         if (bm.isWhite(x, y))
         {
            //��������� �����������
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

QRect ProcCake::findFirstBlackRect(const BoolMatrix & bm)
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
               //�� ���� ��������������� �������� �����������
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

bool ProcCake::hasWhite(const BoolMatrix & bm, const QRect & rect)
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

bool ProcCake::isBlackBordered(const BoolMatrix & bm, const QRect & rect)
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


QRect ProcCake::cutBlack(const BoolMatrix & bm, const QRect & br)
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

QRect ProcCake::findFirstWhiteRect(const BoolMatrix & bm)
{
   for (int y = 0; y < bm.height(); y++)
   {
      for (int x = 0; x < bm.width() / 2; x++)
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
               //�� ���� ��������������� �������� �����������
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

bool ProcCake::hasBlack(const BoolMatrix & bm, const QRect & rect)
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

bool ProcCake::isWhiteBordered(const BoolMatrix & bm, const QRect & rect)
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

/*
QPair<QRect, QRect> ProcCake::getHoleCards(bool * ok)
{
   maxWhite_ = matrix_->findMaxWhiteRect(8, 10);//���� ����� ������� 6�6

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
   //���� ����������� ��������� �� ����� �������
   QPoint tl = maxWhite_.bottomLeft();
   
   int left_x = 0;
   for (int x = tl.x(); x > 10; --x)
   {
      if (matrix_->at(x, tl.y()) == 1)//������
      {
         left_x = x + 1;
         break;
      }
   }
   
   //���������� ����
   int bottom_y = 0;
   for (int y = maxWhite_.y(); y < height - 10; ++y)
   {
      if (matrix_->at(left_x, y) == 1)//������
      {
         bottom_y = y - 10;
         break;
      }
   }

   //���������� ������
   int cw = 0;
   for (int x = left_x; x < width; ++x)
   {
      if (matrix_->at(x, bottom_y) == 1)//������
      {
         cw = x - left_x;
         break;
      }
   }

   //����������� �����
   int top_y = 0;
   for (int y = maxWhite_.y(); y > 10; --y)
   {
      if (matrix_->at(left_x, y) == 1)//������
      {
         top_y = y + 1;
         break;
      }
   }
   
   //����������� ����� �����
   int brd_bottom = 0;
   for (int y = bottom_y - 1; y > 10; --y)
   {
      for (int x = left_x; x < left_x + cw; ++x)
      {
         if (matrix_->at(x, y) == 1)//������
         {
            brd_bottom = y;
            y = 0;
            break;
         }
      }
   }

   //������������ ����� �������
   int brd_left = 0;
   for (int x = left_x; x < left_x + cw; ++x)
   {
      for (int y = brd_bottom; y > top_y; --y)
      {
         if (matrix_->at(x, y) == 1)//������
         {
            brd_left = x;
            x = left_x + cw;
            break;
         }
      }
   }

   //����������� �� ������� ������ ����� �����
   int brd_y = 0;
   int scan_x = left_x + cw;
   for (int y = brd_bottom; y > 10; --y)
   {
      if (matrix_->at(scan_x, y) == 0)//�����
      {
         brd_y = y;
         break;
      }
   }
   for (int y = brd_y; y > 10; --y)
   {
      if (matrix_->at(scan_x, y) == 1)//������
      {
         brd_y = y + 1;
         break;
      }
   }

   //���������� �� ������� �� ������ �������
   int brd_top = 0;
   int stop_x = scan_x - (cw >> 1);
   for (int y = brd_y; y < brd_bottom; ++y)
   {
      for (int x = scan_x; x > stop_x; --x)
      {
         if (matrix_->at(x, y) == 1)//������
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
   
   //����� ������ �����
   //������� ������� ������ �����
   int brd_fat = 0;
   for (int x = left_x + cw; x < width -10; ++x)
   {
      if (matrix_->at(x, brd_bottom) == 0)//�����
      {
         brd_fat = x - (left_x + cw) - 1;
         break;
      }
   }

   //���� ������� ���������� ���� �� ������ �� �������
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

   //����������� �����
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
   //���������� �����
   brd_bottom = temp_y;
   
   //����������� �����
   for (int y = brd_bottom; y > 10; --y)
   {
      if (matrix_->at(scan_x, y) == 1)
      {
         temp_y = y + 1;
         break;
      }
   }
   //��������� ����� �������
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
   
   //����������� �� ������ ���� �����
   for (int y = brd_bottom; y > 10; --y)
   {
      if (matrix_->at(brd_left - 1, y) == 1)
      {
         temp_y = y + 1;
         break;
      }
   }

   //��������� ����
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
*/
int ProcCake::countCheckLetters(const QImage & imgCheck)
{
   //��������
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

   //��������� ������� �� ������� ��������
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

bool ProcCake::checkHueButton(const QImage & imgBtn, int minHue, int maxHue, int rectMin)
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
            //����������� ����� ��������� ������ ���������
            for (int rx = 0; rx < rectMin; ++rx)
            {
               for (int ry = 0; ry < rectMin; ++ry)
               {
                  QRgb rgb = imgBtn.pixel(x + rx, y + ry);
                  QColor cl(rgb);
                  if (!(cl.hue() >= minHue && cl.hue() <= maxHue))
                  {
                     //�� ��� �������
                     rx = rectMin;
                     blackRect = false;
                     break;
                  }
               }
            }
            if (!blackRect)
            {
               // �� �� ���� ������
               continue;
            }
            else
            {
               //���� ������ ���������
               hasBtn = true;
               x = btnWidth;
               break;
            }
         }
      }
   }
   return hasBtn;
}

bool ProcCake::checkHueButton(const QImage & imgBtn, int minHue, int maxHue, 
                              int rectMinW, int rectMinH, QRect & rect)
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
            //����������� ����� ������� ������ �������������
            for (int rx = 0; rx < rectMinW; ++rx)
            {
               for (int ry = 0; ry < rectMinH; ++ry)
               {
                  QRgb rgb = imgBtn.pixel(x + rx, y + ry);
                  QColor cl(rgb);
                  if (!(cl.hue() >= minHue && cl.hue() <= maxHue))
                  {
                     //�� ��� �������
                     rx = rectMinW;
                     blackRect = false;
                     break;
                  }
               }
            }
            if (!blackRect)
            {
               // �� �� ���� ������
               continue;
            }
            else
            {
               //���� ������ ���������
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

