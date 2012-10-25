
#include "ImageProc.h"

int hueCount(const QImage & img, int minHue, int maxHue, int fromw, int fromh, int thr, QPoint & pt)
{
   int hue = 0;
   int imgw = img.width();
   int imgh = img.height();
   if (fromw >= imgw)
      fromw = 0;
   if (fromh >= imgh)
      fromh = 0;

   for (int h = fromh; h < imgh; h++)
   {
      for (int w = fromw; w < imgw; w++)
      {
         QRgb rgb = img.pixel(w, h);
         QColor cl(rgb);
         if (cl.hue() > minHue && cl.hue() < maxHue) 
         {
            hue++;
            if (hue > thr)
            {
               //��������� ��������� QPoint
               int x = fromw + (qrand() % (w - fromw)) + 10;
               int y = fromh + (qrand() % (h - fromh)) + 10;
               pt.setX(x);
               pt.setY(y);
               return hue;
            }
         }
      }
   }
   return hue;
}

int maxHue(const QImage & img)
{
   QMap<int, int> hueGist;
   QMap<int, int> hueGray;

   const int width = img.width();
   const int height = img.height();
   for (int x = 0; x < width; ++x)
   {
      for (int y = 0; y < height; ++y)
      {
         QRgb rgb = img.pixel(x, y);
         QColor cl(rgb);
         if (cl.red() > 200 && cl.green() > 200 && cl.blue() > 200)
            continue;
         
         int gr = qGray(rgb);
         //int hu = cl.hue();
         //if (hu != -1)
         //   hueGist[hu]++;
         hueGray[gr]++;
      }
   }
   //qDebug() << hueGist;
   //qDebug() << hueGray;

   int maxHueCount = 0;
   int maxHueValue = 0;
   int maxGray = 0;
   foreach (int hue, hueGray.keys())
   {
      if (hueGray[hue] > maxHueCount)
      {
         maxHueCount = hueGray[hue];
         maxHueValue = hue;
         maxGray = hueGray[hue];
      }
   }
   return maxHueValue;
   //return maxGray;
}

void toBlackWhiteMid(QImage & img, const int threshold)
{
   int grWhite = 0, grBlack = 0;
   int points = 0;
   const int thr = threshold >> 1;
   const int w = img.width();
   const int h = img.height();
   int maxGray = 0;
   int minGray = 256;

   QVector<QPoint> pts;
   for (int x = 0; x < w; ++x)
   {
      for (int y = 0; y < h; ++y)
      {
         QRgb rgb = img.pixel(x, y);
         int gr = qGray(rgb);
         if (gr > threshold)//�����
            continue;
         
         points++;
         pts.push_back(QPoint(x, y));
         if (gr > maxGray)
            maxGray = gr;
         if (gr < minGray)
            minGray = gr;
      }
   }
   
   int midGray = ((maxGray - minGray) >> 1) + minGray;
   int cb = 0, cw = 0;
   foreach (QPoint pt, pts)
   {
      QRgb rgb = img.pixel(pt);
      int gr = qGray(rgb);

      if (gr < midGray)
      {
         grBlack += gr;//����� � �������
         cb++;
      }
      else
      {
         grWhite += gr;//����� � ������
         cw++;
      }
   }
   if (points == 0)
      return;
   int th = (grBlack - grWhite) / points;
   int mid = thr - th;
   mid += midGray;
   if (mid > threshold)
      mid = threshold;
   toBlackWhite(img, mid);
}

void toBlackWhiteByHue(QImage & img, int minHue, int maxHue)
{
   const int width = img.width();
   const int height = img.height();
   
   for (int w = 0; w < width; ++w)
   {
      for (int h = 0; h < height; ++h)
      {
         QRgb rgb = img.pixel(w, h);
         QColor cl(rgb);
         int hu = cl.hue();
         if (hu >= minHue && hu <= maxHue)
         {
            img.setPixel(w,h, qRgb(0, 0, 0));
         }     
         else
         {
            img.setPixel(w,h, qRgb(255, 255, 255));
         }
      }
   }
}

void toBlackWhiteByHue(QImage & img)
{
   int maxH = maxHue(img);
   const int width = img.width();
   const int height = img.height();
   
   for (int w = 0; w < width; ++w)
   {
      for (int h = 0; h < height; ++h)
      {
         QRgb rgb = img.pixel(w, h);
         QColor cl(rgb);
         //int hu = cl.hue();
         int gr = qGray(rgb);
         if ((gr < maxH + 20) && (gr > maxH - 20))
         {
            img.setPixel(w,h, qRgb(0, 0, 0));
         }     
         else
         {
            img.setPixel(w,h, qRgb(255, 255, 255));
         }
      }
   }
}

void denoise(QImage & img, uchar threshold)
{
   //������ ��������� ����� �������
   for (int x = 0; x < img.width(); x++)
   {
      for (int y = 0; y < img.height(); y++)
      {
         if (img.pixel(x, y) == qRgb(255, 255, 255))
         {
            int xdb = (x == 0) ? 0 : x - 1;
            int xde = (x == img.width() - 1) ? img.width() - 1 : x + 1;
            int ydb = (y == 0) ? 0 : y - 1;
            int yde = (y == img.height() - 1) ? img.height() - 1 : y + 1;
            int wpix = 0;
            for (int xx = xdb; xx <= xde; xx++)
            {
               for (int yy = ydb; yy <= yde; yy++)
               {
                  if (img.pixel(xx, yy) == qRgb(255, 255, 255))
                  {
                     wpix++;
                  }
               }
            }
            if (wpix < threshold)
            {
               //������� �����
               img.setPixel(x, y, qRgb(0, 0, 0));
            }
         }
      }
   }
}

void toBlackWhite(QImage & img, uchar threshold)
{
   for (int w = 0; w < img.width(); w++)
   {
      for (int h = 0; h < img.height(); h++)
      {
         QRgb rgb = img.pixel(w, h);
         QColor cl(rgb);
         if (qGray(rgb) < threshold)
         {
            img.setPixel(w,h, qRgb(0, 0, 0));
         }     
         else
         {
            //if (cl.value() >= 237 && cl.value() <= 254) 
               img.setPixel(w,h, qRgb(255, 255, 255));
            //else
            //   img.setPixel(w,h, qRgb(0, 0, 0));
         }
      }
   }
}

//
//CardProcessing
//
CardProcessing::CardProcessing()
{
   threshold_           = 240;
   dim_                 = 32;
   minSizeVertical_     = 20;
   minSizeHorizontal_   = 2;

   matrix_ = 0;
}

CardProcessing::~CardProcessing()
{
   delete matrix_;
}

void CardProcessing::setImage(const QImage & img)
{
   delete matrix_;
   matrix_ = 0;
   matrix_ = new BoolMatrix(img, threshold_);
   maxWhite_ = matrix_->findMaxWhiteRect(dim_);
   verticals_ = matrix_->verticalBorders(minSizeVertical_);
   horizontals_ = matrix_->horizontalBorders(minSizeHorizontal_);
   
   //���������� ������
   QRect table = flopRect(img);
   /// Debugging {
   //img.copy(table).save("22.bmp");
   //img.copy(maxWhite_).save("33.bmp");
   /// } Debugging 
   // ������� ������� ������������ ������ ���� ������
   int vertBord = 0;
   bool hasBorder = false;
   foreach (Border border, verticals_)
   {
      if (table.contains(border.first()) && table.contains(border.last()))
      {
         //���� �� ������� ���� 1 ������� 
         hasBorder = true;
         //���� � ��� ������ �������������� �������
         //���������� � ��� ������� ������
         QPoint bp = border.first();
         QRect frect(bp.x(), bp.y() - 8, 8, 8);
         foreach (Border hborder, horizontals_)
         {
            //������� �������������� ������� � ������� �����������
            if (frect.contains(hborder.first()))
            {
               vertBord++;
               break;
            }
         }
      }
   }
   
   holdemLevel_ = Undefined;
   if (hasBorder)
   {
      if (vertBord == 0)
      {
         holdemLevel_ = Preflop;
      }
      else
      {
         if (vertBord == 3)
         {
            holdemLevel_ = Flop;
         }
         else if (vertBord == 4)
         {
            holdemLevel_ = Turn;
         }
         else if (vertBord == 5)
         {
            holdemLevel_ = River;
         }
      }
   }
   else//��� �� ����� �������
   {
      holdemLevel_ = Preflop;
   }
}

QRect CardProcessing::flopRect(const QImage & img) const 
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

bool CardProcessing::isPreflop() const
{
   return holdemLevel_ == Preflop;
}

CardProcessing::HoldemLevel CardProcessing::holdemLevel() const
{
   return holdemLevel_;
}

QPair<QRect, QRect> CardProcessing::getHoleCards(bool * ok)
{
   QRect first;
   QRect second;
   if (matrix_ == 0)
   {
      *ok = false;
      return qMakePair<QRect, QRect>(first, second);
   }
   if (!verticals_.count() || !horizontals_.count())
   {
      *ok = false;
      return qMakePair<QRect, QRect>(first, second);
   }
   //���� ����������� ��������� �� ����� �������
   QPoint tl = maxWhite_.topLeft();
   qreal minDist = 0;
   int minVertIdx = -1;
   for (int i = 0; i < verticals_.count(); i++)
   {
      Border brd = verticals_.at(i);
      QPoint fst = brd.first();
      QPoint lst = brd.last();
      
      //����� ������� ������ ���� �����
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
   
   if (minVertIdx == -1)
   {
      *ok = false;
      return qMakePair<QRect, QRect>(first, second);
   }

   Border vBorder = verticals_.at(minVertIdx);
   // �� �� ����� ��� �������������� �������
   int minHorzIdx = -1;
   //���� � ��� ������ �������������� �������
   //���������� � ��� ������� ������
   QPoint bp = vBorder.first();
   QRect frect(bp.x(), bp.y() - 8, 8, 8);
   for (int i = 0; i < horizontals_.count(); i++)
   {
      Border hborder = horizontals_.at(i);
      //������� �������������� ������� � ������� �����������
      if (frect.contains(hborder.first()))
      {
         minHorzIdx = i;
         break;
      }
   }

   //for (int i = 0; i < horizontals_.count(); i++)
   //{
   //   Border brd = horizontals_.at(i);
   //   QPoint fst = brd.first();
   //   QPoint lst = brd.last();
   //   //����� ������� ������ ���� ���� ������� ����� ������������ �������
   //   if (fst.y() >= verticals_.at(minVertIdx).at(0).y())
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
   
   if (minHorzIdx == -1)
   {
      *ok = false;
      return qMakePair<QRect, QRect>(first, second);
   }

   //��� ������ ������� �� �����������
   Border hBorder = horizontals_.at(minHorzIdx);   
   
   first.setX(vBorder.first().x() + 1);
   first.setY(hBorder.first().y() + 1);
   first.setWidth(dim_);
   first.setHeight(dim_);
   
   //���� ������� ���� 
   //���� ������������ ������� ������ �����
   int cardWidth = 5;
   int minDHeight = vBorder.count() * 75 / 100;
   for (int x = hBorder.first().x() + cardWidth; x < hBorder.first().x() + 32; x++)
   {
      int blackHeight = 0;
      for (int y = vBorder.first().y(); y < vBorder.last().y(); y++)
      {
         if (matrix_->at(x, y) == 1)//black
            blackHeight++;
      }
      if (blackHeight >= minDHeight)
      {
         //its
         cardWidth = x - vBorder.first().x() - 1;
         break;
      }
   }
   first.setWidth(cardWidth);

   //����� ��������� ����� �� ���� ������� �� ��� ��� ���� �� ���������� ����� ������
   int cardHeight = 0;
   for (int y = vBorder.last().y() - 10; y >= vBorder.first().y(); y--)
   {
      for (int x = vBorder.first().x() + 2; x < vBorder.first().x() + cardWidth; x++)
      {
         if (matrix_->at(x, y) == 1)
         {
            //�������� ������ �����
            cardHeight = y - hBorder.first().y();
            y = 0;
            break;
         }
      }
   }
   first.setHeight(cardHeight);

   //�������� ��� �� ������� ���� ������ �����
   //���� ����� ������� ������ �����

   second.setX(first.x() + first.width() + 1);
   //�������� ������ �� �� �������
   for (int y = first.y(); y < vBorder.last().y(); y++)
   {
      int blackCount = 0;
      for (int x = second.x(); x < second.x() + hBorder.count(); x++)
      {
         //������� ���������� ������ �����
         //�� ������ ���� ����� ������� �� ������� � � ����� ������� ������ �����
         //������� ���������
         if (matrix_->at(x, y) == 0 && blackCount != 0)
            break;
         if (matrix_->at(x, y) == 1)
            blackCount++;
      }
      //qDebug() << y << blackCount;
      if (blackCount >= hBorder.count() * 90 / 100)
      {
         //���
         second.setY(y + 1);
         break;
      }
   }
   //������/������ �������� ������ ������/������ ������ �����
   second.setWidth(cardWidth);
   second.setHeight(cardHeight);
   
   //�������� �� ������� ������� ������� ������� �����
   //������������
   if (matrix_->at(second.bottomLeft().x(), second.bottomLeft().y()) == 1)
   {
      //������� �������� ������ ���������
      for (int x = second.bottomLeft().x(); 
           x < second.bottomLeft().x() + second.width();
           x++)
      {
         if (matrix_->at(x, second.bottomLeft().y()) == 0)
         {
            //�����
            second.setX(x);
            second.setWidth(cardWidth);
            second.setHeight(cardHeight);
            break;
         }
      }
   }
   // �������� ������� �������������� �������
   if (matrix_->at(second.topRight().x(), second.topRight().y()) == 1)
   {
      //������� �������� ������ ���������
      for (int y = second.topRight().y(); 
           y < second.topRight().y() + second.height();
           y++)
      {
         if (matrix_->at(second.topRight().x(), y) == 0)
         {
            //�����
            second.setY(y);
            second.setWidth(cardWidth);
            second.setHeight(cardHeight);
            break;
         }
      }
   }

   *ok = true;
   return qMakePair<QRect, QRect>(first, second);
}

