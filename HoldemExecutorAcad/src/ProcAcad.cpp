
#include "ProcAcad.h"

//
//ProcAcad
//
ProcAcad::ProcAcad()
: CardProcessing()
{
   threshold_           = 200;
   matrix_ = 0;
}

ProcAcad::~ProcAcad()
{
   delete matrix_;
   matrix_ = 0;
}

void ProcAcad::setImage(const QImage & img)
{
   delete matrix_;
   matrix_ = 0;
   matrix_ = new BoolMatrix(img, threshold_);
   //QImage im = img;
   //toBlackWhite(im, 200);
   //im.save("table.bmp");
   
   //���������� ������
   //is river
   static const int flw = 5;
   static const int sum = 25;
   static const int riverPt_x = 505;
   static const int riverPt_y = 300;
   static const int turnPt_x = 450;
   static const int turnPt_y = 300;
   static const int flopPt_x = 395;
   static const int flopPt_y = 300;

   //QPoint riverPt(505, 300);
   //QPoint turnPt(450, 300);
   //QPoint flopPt(395, 300);
   int rvCount = 0;
   int trnCount = 0;
   int flpCount = 0;
   for (int x = riverPt_x; x < riverPt_x + flw; ++x)
   {
      for (int y = riverPt_y; y < riverPt_y + flw; ++y)
      {
         if (matrix_->at(x, y) == 0)//�����
         {
            rvCount++;
         }
      }
   }

   if (rvCount == sum)
   {
      holdemLevel_ = River;
   }
   else
   {
      for (int x = turnPt_x; x < turnPt_x + flw; ++x)
      {
         for (int y = turnPt_y; y < turnPt_y + flw; ++y)
         {
            if (matrix_->at(x, y) == 0)//�����
            {
               trnCount++;
            }
         }
      }
      if (trnCount == sum)
      {
         holdemLevel_ = Turn;
      }
      else
      {
         for (int x = flopPt_x; x < flopPt_x + flw; ++x)
         {
            for (int y = flopPt_y; y < flopPt_y + flw; ++y)
            {
               if (matrix_->at(x, y) == 0)//�����
               {
                  flpCount++;
               }
            }
         }
         if (flpCount == sum)
         {
            holdemLevel_ = Flop;
         }
         else
         {
            holdemLevel_ = Preflop;
         }
      }
   }
}

QRect ProcAcad::flopRect(const QImage & img) const 
{
   //���������� ���� �����
   int tableHue = maxHue(img);
   //��������� �������
   //������� �� +/- 10
   int minHue = tableHue - 10;
   int maxHue = tableHue + 10;

   const int width  = img.width();
   const int height = img.height();
   
   BoolMatrix matrix(img);
   for (int w = 0; w < width; w++)
   {
      for (int h = 0; h < height; h++)
      {
         QRgb rgb = img.pixel(w, h);
         QColor cl(rgb);
         
         if (cl.hue() > minHue && cl.hue() < maxHue) 
         {
            matrix.set(w, h, true);
         }
         else
         {
            matrix.set(w, h, false);
         }
      }
   }

   //��������� �������
   //�������� ��������
   const int rectSize  = 32;
   const int rectSize2 = 16;
   const int maxSize   = rectSize * rectSize;
   const int width2 = width / 2;

   //�������� ������ � ����� ����� ������� ������ ����
   QPoint topTable;
   for (int h = 0; h < height; h += rectSize2)
   {
      int blackCount = 0;
      for (int x = width2 - rectSize2; x < width2 + rectSize2; x++)
      {
         for (int y = h; y < h + rectSize; y++)
         {
            if (matrix.at(x, y) == true)
               blackCount++;
         }
      }
      //� ���������� ��������� - ������ 33,3333 % ������ ���� ���������
      if (blackCount > maxSize / 3)
      {
         //���
         topTable.setX(width2);
         topTable.setY(h + rectSize);
         break;
      }
   }

   //� �����
   QPoint bottomTable;
   for (int h = height - rectSize - 1; h >= 0; h -= rectSize2)
   {
      int blackCount = 0;
      for (int x = width2 - rectSize2; x < width2 + rectSize2; x++)
      {
         for (int y = h; y < h + rectSize; y++)
         {
            if (matrix.at(x, y) == true)
               blackCount++;
         }
      }
      //� ���������� ��������� - ������ 33,3333 % ������ ���� ���������
      if (blackCount > maxSize / 3)
      {
         //���
         bottomTable.setX(width2);
         bottomTable.setY(h/* - rectSize*/);
         break;
      }
   }
   //����� ����� � ������
   const int height2 = height / 2;
   QPoint leftTable;
   for (int w = 0; w < width; w += rectSize2)
   {
      int blackCount = 0;
      for (int x = w; x < w + rectSize; x++)
      {
         for (int y = height2 - rectSize2; y < height2 + rectSize2; y++)
         {
            if (matrix.at(x, y) == true)
               blackCount++;
         }
      }
      //� ���������� ��������� - ������ 33,3333 % ������ ���� ���������
      if (blackCount > maxSize / 3)
      {
         //���
         leftTable.setX(w + rectSize*2);
         leftTable.setY(height2);
         break;
      }
   }

   //� ������
   QPoint rightTable;
   for (int w = width - rectSize - 1; w >= 0; w -= rectSize2)
   {
      int blackCount = 0;
      for (int x = w; x < w + rectSize; x++)
      {
         for (int y = height2 - rectSize2; y < height2 + rectSize2; y++)
         {
            if (matrix.at(x, y) == true)
               blackCount++;
         }
      }
      //� ���������� ��������� - ������ 33,3333 % ������ ���� ���������
      if (blackCount > maxSize / 3)
      {
         //���
         rightTable.setX(w - rectSize);
         rightTable.setY(height2);
         break;
      }
   }

   //�������
   QRect table(leftTable.x(), topTable.y(),
      rightTable.x() - leftTable.x(),
      bottomTable.y() - topTable.y());
   
   return table;
}


QPair<QRect, QRect> ProcAcad::getHoleCards(bool * ok)
{
   maxWhite_ = matrix_->findMaxWhiteRect(6);//���� ����� ������� 6�6
   //verticals_ = matrix_->verticalBorders(minSizeVertical_);
   //horizontals_ = matrix_->horizontalBorders(minSizeHorizontal_);
   
   QRect first;
   QRect second;
   if (matrix_ == 0)
   {
      *ok = false;
      return qMakePair<QRect, QRect>(first, second);
   }
   //if (!verticals_.count() || !horizontals_.count())
   //{
   //   *ok = false;
   //   return qMakePair<QRect, QRect>(first, second);
   //}
   int width = matrix_->width();
   int height = matrix_->height();
   //���� ����������� ��������� �� ����� �������
   QPoint tl = maxWhite_.topLeft();
   //���� ����������� �� ������� ������� � �� �����
   QPoint brd;
   for (int x = tl.x(); x > 10; --x)
   {
      if (matrix_->at(x, tl.y()) == 1)//������
      {
         brd.setX(x + 1);
         break;
      }
   }
   
   for (int y = tl.y(); y > 10; --y)
   {
      if (matrix_->at(brd.x() + 1, y) == 1)//������
      {
         brd.setY(y + 1);
         break;
      }
   }
   
   //qDebug() << brd;
   
   int tpY = brd.y();
   //���� ������ �������
   for (int y = brd.y() + 1; y < height; ++y)
   {
      if (matrix_->at(brd.x(), y) == 1)//������
      {
         //����� ������ �����
         tpY = y;
         break;
      }
   }
   int cw = 1;
   //������������ ������ ����� ���� ������ �����
   for (int x = brd.x() + 1; x < width; ++x)
   {
      if (matrix_->at(x, tpY) == 1)//������
      {
         //����� ������
         cw = x - brd.x();
         break;
      }
   }

   int ch = 1;
   //�������� ����������� ����� �����
   for (int y = tpY; y > brd.y(); --y)
   {
      for (int x = brd.x() + 1; x < brd.x() + cw - 1; ++x)
      {
         if (matrix_->at(x, y) == 1)//������
         {
            ch = y - brd.y() + 1;
            break;
         }
      }
      if (ch != 1)
         break;
   }

   //��������� ������������� ������ �����
   first.setX(brd.x());
   first.setY(brd.y());
   first.setWidth(cw);
   first.setHeight(ch);

   //���� ������ �����
   second.setX(first.x() + first.width() + 1);
   int tpX = first.x() +  first.width() * 2;
   for (int y = first.y() + 1; y < height; ++y)
   {
      if (matrix_->at(tpX, y) == 1)//black
      {
         second.setY(y + 1);
         break;
      }
   }
   second.setWidth(first.width());
   second.setHeight(first.height());
   //qreal minDist = 0;
   //int minVertIdx = -1;
   //for (int i = 0; i < verticals_.count(); i++)
   //{
   //   Border brd = verticals_.at(i);
   //   QPoint fst = brd.first();
   //   QPoint lst = brd.last();
   //   
   //   //����� ������� ������ ���� �����
   //   //if (fst.x() >= tl.x())
   //   //   continue;

   //   qreal r1 = qSqrt(qPow(tl.x() - fst.x(), 2) + qPow(tl.y() - fst.y(), 2));
   //   qreal r2 = qSqrt(qPow(tl.x() - lst.x(), 2) + qPow(tl.y() - lst.y(), 2));
   //   qreal sum = r1 + r2;
   //   if (minDist == 0)
   //   {
   //      minDist = sum;
   //      minVertIdx = i;
   //   }
   //   else
   //   {
   //      if (sum < minDist)
   //      {
   //         minDist = sum;
   //         minVertIdx = i;
   //      }
   //   }
   //}
   //
   //if (minVertIdx == -1)
   //{
   //   *ok = false;
   //   return qMakePair<QRect, QRect>(first, second);
   //}

   //Border vBorder = verticals_.at(minVertIdx);
   //// �� �� ����� ��� �������������� �������
   //int minHorzIdx = -1;
   ////���� � ��� ������ �������������� �������
   ////���������� � ��� ������� ������
   //QPoint bp = vBorder.first();
   //QRect frect(bp.x(), bp.y() - 8, 8, 8);
   //for (int i = 0; i < horizontals_.count(); i++)
   //{
   //   Border hborder = horizontals_.at(i);
   //   //������� �������������� ������� � ������� �����������
   //   if (frect.contains(hborder.first()))
   //   {
   //      minHorzIdx = i;
   //      break;
   //   }
   //}
   //
   //if (minHorzIdx == -1)
   //{
   //   *ok = false;
   //   return qMakePair<QRect, QRect>(first, second);
   //}

   ////��� ������ ������� �� �����������
   //Border hBorder = horizontals_.at(minHorzIdx);   
   //
   //first.setX(vBorder.first().x() + 1);
   //first.setY(hBorder.first().y() + 1);
   //first.setWidth(dim_);
   //first.setHeight(dim_);
   //
   ////���� ������� ���� 
   ////���� ������������ ������� ������ �����
   //int cardWidth = 5;
   //int minDHeight = vBorder.count() * 75 / 100;
   //for (int x = hBorder.first().x() + cardWidth; x < hBorder.first().x() + 32; x++)
   //{
   //   int blackHeight = 0;
   //   for (int y = vBorder.first().y(); y < vBorder.last().y(); y++)
   //   {
   //      if (matrix_->at(x, y) == 1)//black
   //         blackHeight++;
   //   }
   //   if (blackHeight >= minDHeight)
   //   {
   //      //its
   //      cardWidth = x - vBorder.first().x() - 1;
   //      break;
   //   }
   //}
   //first.setWidth(cardWidth);

   ////����� ��������� ����� �� ���� ������� �� ��� ��� ���� �� ���������� ����� ������
   //int cardHeight = 0;
   //for (int y = vBorder.last().y() - 10; y >= vBorder.first().y(); y--)
   //{
   //   for (int x = vBorder.first().x() + 2; x < vBorder.first().x() + cardWidth; x++)
   //   {
   //      if (matrix_->at(x, y) == 1)
   //      {
   //         //�������� ������ �����
   //         cardHeight = y - hBorder.first().y();
   //         y = 0;
   //         break;
   //      }
   //   }
   //}
   //first.setHeight(cardHeight);

   ////�������� ��� �� ������� ���� ������ �����
   ////���� ����� ������� ������ �����

   //second.setX(first.x() + first.width() + 1);
   ////�������� ������ �� �� �������
   //for (int y = first.y(); y < vBorder.last().y(); y++)
   //{
   //   int blackCount = 0;
   //   for (int x = second.x(); x < second.x() + hBorder.count(); x++)
   //   {
   //      //������� ���������� ������ �����
   //      //�� ������ ���� ����� ������� �� ������� � � ����� ������� ������ �����
   //      //������� ���������
   //      if (matrix_->at(x, y) == 0 && blackCount != 0)
   //         break;
   //      if (matrix_->at(x, y) == 1)
   //         blackCount++;
   //   }
   //   //qDebug() << y << blackCount;
   //   if (blackCount >= hBorder.count() * 90 / 100)
   //   {
   //      //���
   //      second.setY(y + 1);
   //      break;
   //   }
   //}
   ////������/������ �������� ������ ������/������ ������ �����
   //second.setWidth(cardWidth);
   //second.setHeight(cardHeight);
   //
   ////�������� �� ������� ������� ������� ������� �����
   ////������������
   //if (matrix_->at(second.bottomLeft().x(), second.bottomLeft().y()) == 1)
   //{
   //   //������� �������� ������ ���������
   //   for (int x = second.bottomLeft().x(); 
   //        x < second.bottomLeft().x() + second.width();
   //        x++)
   //   {
   //      if (matrix_->at(x, second.bottomLeft().y()) == 0)
   //      {
   //         //�����
   //         second.setX(x);
   //         second.setWidth(cardWidth);
   //         second.setHeight(cardHeight);
   //         break;
   //      }
   //   }
   //}
   //// �������� ������� �������������� �������
   //if (matrix_->at(second.topRight().x(), second.topRight().y()) == 1)
   //{
   //   //������� �������� ������ ���������
   //   for (int y = second.topRight().y(); 
   //        y < second.topRight().y() + second.height();
   //        y++)
   //   {
   //      if (matrix_->at(second.topRight().x(), y) == 0)
   //      {
   //         //�����
   //         second.setY(y);
   //         second.setWidth(cardWidth);
   //         second.setHeight(cardHeight);
   //         break;
   //      }
   //   }
   //}

   *ok = true;
   return qMakePair<QRect, QRect>(first, second);
}

int ProcAcad::countCheckLetters(const QImage & imgCheck)
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
