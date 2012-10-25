#include "BoolMatrix.h"

BoolMatrix::BoolMatrix()
{
   matrix_ = 0;
   bits_ = 0;
}

BoolMatrix::BoolMatrix(const BoolMatrix & rhs)
{
   width_  = rhs.width();
   height_ = rhs.height();
   matrix_ = new bool* [width_];
   bits_ = new bool[width_ * height_];
   for (int i = 0; i < width_; ++i)
   {
      matrix_[i] = bits_ + i * height_;
   }
   
   for (int x = 0; x < width_; ++x)
   {
      for (int y = 0; y < height_; ++y)
      {
         matrix_[x][y] = rhs.matrix_[x][y];
      }
   }
}

BoolMatrix & BoolMatrix::operator=(const BoolMatrix & rhs)
{
   delete [] matrix_;
   delete [] bits_;

   width_  = rhs.width();
   height_ = rhs.height();
   matrix_ = new bool* [width_];
   bits_ = new bool[width_ * height_];
   for (int i = 0; i < width_; ++i)
   {
      matrix_[i] = bits_ + i * height_;
   }
   
   for (int x = 0; x < width_; ++x)
   {
      for (int y = 0; y < height_; ++y)
      {
         matrix_[x][y] = rhs.matrix_[x][y];
      }
   }

   return (*this);
}

BoolMatrix::BoolMatrix(const QImage & img)
{
   width_  = img.width();
   height_ = img.height();

   matrix_ = new bool* [width_];
   bits_ = new bool[width_ * height_];
   for (int i = 0; i < width_; ++i)
   {
      matrix_[i] = bits_ + i * height_;
   }
}

BoolMatrix::BoolMatrix(const QString & str)
{
   width_  = 0;
   height_ = 0;

   QStringList lst = str.split(":");
   if (lst.count() == 3)
   {
      bool ok = false;
      width_ = lst.at(0).toInt(&ok);
      if (!ok)
         return;
      height_ = lst.at(1).toInt(&ok);
      if (!ok)
         return;
      
      QString data = lst.at(2);
      int cnt = data.count();
      if (width_ * height_ != cnt)
         return;

	   matrix_ = new bool* [width_];
      bits_ = new bool[width_ * height_];
      for (int i = 0; i < width_; ++i)
      {
         matrix_[i] = bits_ + i * height_;
      }

      for (int y = 0; y < height_; ++y)
      {
         for (int x = 0; x < width_; ++x)
         {
            bool val = data.at(x + y * width_).digitValue();
            if (ok)
               matrix_[x][y] = val;
         }
      }
   }
}

void BoolMatrix::set(int x, int y, bool value)
{
   if (x < width_ && y < height_)
      matrix_[x][y] = value;
}

BoolMatrix::BoolMatrix(const QImage * img)
{
   width_  = img->width();
   height_ = img->height();

	matrix_ = new bool* [width_];
   bits_ = new bool[width_ * height_];
   for (int i = 0; i < width_; ++i)
   {
      matrix_[i] = bits_ + i * height_;
   }

   for (int w = 0; w < width_; ++w)
   {
      for (int h = 0; h < height_; ++h)
      {
         if (img->pixel(w, h) == qRgb(0, 0, 0))
         {
            matrix_[w][h] = 1;//черный
         }
         else
         {
            matrix_[w][h] = 0;//белый
         }
      }
   }
}

BoolMatrix::BoolMatrix(const QImage & img, uchar threshold)
{
   width_  = img.width();
   height_ = img.height();

	matrix_ = new bool* [width_];
   bits_ = new bool[width_ * height_];
   for (int i = 0; i < width_; ++i)
   {
      matrix_[i] = bits_ + i * height_;
   }

   for (int w = 0; w < width_; ++w)
   {
      for (int h = 0; h < height_; ++h)
      {
         QRgb rgb = img.pixel(w, h);
         QColor cl(rgb);
         //if (cl.hue() >= 13 && cl.hue() <= 33) 

         if (qGray(rgb) < threshold)
         {
            matrix_[w][h] = 1;//черный
         }
         else
         {
            matrix_[w][h] = 0;//белый
         }
      }
   }
}

BoolMatrix::BoolMatrix(const QImage & img, uchar grayThresh, uint minBright, uint maxBright)
{
   width_  = img.width();
   height_ = img.height();

	matrix_ = new bool* [width_];
   bits_ = new bool[width_ * height_];
   for (int i = 0; i < width_; ++i)
   {
      matrix_[i] = bits_ + i * height_;
   }

   for (int w = 0; w < width_; ++w)
   {
      for (int h = 0; h < height_; ++h)
      {
         QRgb rgb = img.pixel(w, h);
         QColor cl(rgb);

         if (qGray(rgb) < grayThresh)
         {
            matrix_[w][h] = 1;//черный
         }
         else
         {
            if (cl.value() >= (int)minBright && cl.value() <= (int)maxBright) 
            {
               matrix_[w][h] = 0;//белый
            }
            else
            {
               matrix_[w][h] = 1;
            }
         }
      }
   }
}

BoolMatrix::~BoolMatrix()
{
   delete [] matrix_;
   delete [] bits_;
}

bool BoolMatrix::at(int x, int y) const
{
   if ((x >= width_ || y >= height_) ||
      (x < 0 || y < 0))
      return false;
   return matrix_[x][y];
}

bool BoolMatrix::isWhite(int x, int y) const
{
   if ((x >= width_ || y >= height_) ||
      (x < 0 || y < 0))
      return false;
   return matrix_[x][y] == 0;
}

bool BoolMatrix::isBlack(int x, int y) const
{
   if ((x >= width_ || y >= height_) ||
      (x < 0 || y < 0))
      return false;
   return matrix_[x][y] == 1;
}

void BoolMatrix::fillRect(const QRect & rect, bool color)
{
   for (int x = rect.left(); x <= rect.right(); x++)
   {
      for (int y = rect.top(); y <= rect.bottom(); y++)
      {
         matrix_[x][y] = color;
      }
   }
}

QRect BoolMatrix::findMaxWhiteRect(int dim_x, int dim_y) const
{
   if (dim_y == -1)
      dim_y = dim_x;
   QRect rect;
   int maxWeight = dim_x * dim_y;
   for (int w = dim_x; w < width_ - dim_x; w += 2)
   {
      for (int h = dim_y; h < height_ - dim_y; h += 2)
      {
         int sum = 0;
         if (matrix_[w][h] == 0)//белый
         {
            for (int x = 0; x < dim_x; ++x)
            {
               for (int y = 0; y < dim_y; ++y)
               {
                  sum += !matrix_[w + x][h + y];
               }
            }
            if (sum == maxWeight)
            {
               rect.setX(w);
               rect.setY(h);
               rect.setWidth(dim_x);
               rect.setHeight(dim_y);
               
               //проверить этот прямоугольник
               QPoint topLeft = rect.topLeft();
               QPoint bottomLeft = rect.bottomLeft();
               QPoint centerLeft = rect.bottomLeft();
               centerLeft.setY(rect.center().y());

               int d1 = distForLeftBlack(topLeft);
               int d2 = distForLeftBlack(bottomLeft);
               int d3 = distForLeftBlack(centerLeft);
               if ((d1 != d2) || (d1 != d3))
               {
                  h += dim_y;
                  continue;
               }
               return rect;
            }
         }
      }
   }
   return rect;
}

int BoolMatrix::distForLeftBlack(const QPoint & pt) const
{
   int dist = 0;
   for (int x = pt.x(); x > 10; --x)
   {
      if (matrix_[x][pt.y()] == 1)//черный
      {
         dist = x - pt.x();
         break;
      }
   }
   return dist;
}

QList<Border> BoolMatrix::verticalBorders(int minSize) const
{
   //поиск граничных переходов
   /*
   матрица 5х5
   1 1 1 1 0
   1 1 1 1 0
   1 1 1 х 0
   1 1 1 1 0
   1 1 1 1 0
   */
   const int sz = 5;
   bool border_matrix[sz][sz];
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
   for (int w = sz; w < width_ - sz; w++)
   {
      for (int h = sz; h < height_ - sz; h++)
      {
         int matches = 0;
         for (int x = -3; x < 2; x++)
         {
            for (int y = -2;  y <= 2; y++)
            {
               if (matrix_[w + x][h + y] ==
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
   
   //построить массив связных линий
   //список вертикальных границ
   QList<Border> vertBorders;

   for (int b = 0; b < borders.count(); b++)
   {
      const QPoint & pt = borders.at(b);
      Border brd;
      brd << pt;
      for (int y = pt.y() + 1; y < height_; y++)
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
      if (brd.count() >= minSize)
      {
         vertBorders << brd;
      }
   }

   return vertBorders;
}

QList<Border> BoolMatrix::horizontalBorders(int minSize) const
{
   //поиск граничных переходов
   /*
   матрица 5х5
   1 1 1 1 1
   1 1 1 1 1
   1 1 1 1 1
   1 1 X 1 1
   0 0 0 0 0
   */
   const int sz = 5;
   bool border_matrix[sz][sz];

   border_matrix[0][0] = 1;
   border_matrix[0][1] = 1;
   border_matrix[0][2] = 1;
   border_matrix[0][3] = 1;
   border_matrix[0][4] = 0;

   border_matrix[1][0] = 1;
   border_matrix[1][1] = 1;
   border_matrix[1][2] = 1;
   border_matrix[1][3] = 1;
   border_matrix[1][4] = 0;

   border_matrix[2][0] = 1;
   border_matrix[2][1] = 1;
   border_matrix[2][2] = 1;
   border_matrix[2][3] = 1;
   border_matrix[2][4] = 0;

   border_matrix[3][0] = 1;
   border_matrix[3][1] = 1;
   border_matrix[3][2] = 1;
   border_matrix[3][3] = 1;
   border_matrix[3][4] = 0;

   border_matrix[4][0] = 1;
   border_matrix[4][1] = 1;
   border_matrix[4][2] = 1;
   border_matrix[4][3] = 1;
   border_matrix[4][4] = 0;

   Border borders;
   for (int w = sz; w < width_ - sz; w++)
   {
      for (int h = sz; h < height_ - sz; h++)
      {
         int matches = 0;
         for (int x = -2; x <= 2; x++)
         {
            for (int y = -3;  y < 2; y++)
            {
               if (matrix_[w + x][h + y] ==
                   border_matrix[x + 2][y + 3])
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
      for (int x = pt.x() + 1; x < width_; x++)
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
      if (brd.count() >= minSize)
         horzBorders << brd;
   }

   return horzBorders;
}

int BoolMatrix::operator-(const BoolMatrix & rhs)
{
   int w = width_  <= rhs.width_  ? width_  : rhs.width_;
   int h = height_ <= rhs.height_ ? height_ : rhs.height_;

   int count = 0;
   for (int x = 0; x < w; ++x)
   {
      for (int y = 0; y < h; ++y)
      {
         if (matrix_[x][y] != rhs.matrix_[x][y])
         {
            count++;
         }
      }
   }
   
   return count;
}

QString BoolMatrix::toString() const
{
   QString res;
   res = QString("%1:%2:").arg(width_).arg(height_);
   for (int y = 0; y < height_; ++y)
   {
      for (int x = 0; x < width_; ++x)
      {
         res += QString::number(at(x, y));
      }
   }
   
   return res;
}

int BoolMatrix::minDiff(const BoolMatrix & rhs)
{
   int w = width_  <= rhs.width_  ? width_  : rhs.width_;
   int h = height_ <= rhs.height_ ? height_ : rhs.height_;

   int min = 0;
   for (int x = 0; x < w; ++x)
   {
      for (int y = 0; y < h; ++y)
      {
         if (matrix_[x][y] != rhs.matrix_[x][y])
         {
            min++;
         }
      }
   }

   int count = 0;
   for (int x = 1; x < w; ++x)
   {
      for (int y = 0; y < h; ++y)
      {
         if (matrix_[x][y] != rhs.matrix_[x - 1][y])
         {
            count++;
         }
      }
   }
   if (count < min)
      min = count;
   
   count = 0;
   for (int x = 0; x < w; ++x)
   {
      for (int y = 1; y < h; ++y)
      {
         if (matrix_[x][y] != rhs.matrix_[x][y - 1])
         {
            count++;
         }
      }
   }
   if (count < min)
      min = count;

   count = 0;
   for (int x = 1; x < w; ++x)
   {
      for (int y = 1; y < h; ++y)
      {
         if (matrix_[x][y] != rhs.matrix_[x - 1][y - 1])
         {
            count++;
         }
      }
   }
   if (count < min)
      min = count;

   count = 0;
   for (int x = 1; x < w; ++x)
   {
      for (int y = 0; y < h; ++y)
      {
         if (matrix_[x - 1][y] != rhs.matrix_[x][y])
         {
            count++;
         }
      }
   }
   if (count < min)
      min = count;

   count = 0;
   for (int x = 0; x < w; ++x)
   {
      for (int y = 1; y < h; ++y)
      {
         if (matrix_[x][y - 1] != rhs.matrix_[x][y])
         {
            count++;
         }
      }
   }
   if (count < min)
      min = count;

   count = 0;
   for (int x = 1; x < w; ++x)
   {
      for (int y = 1; y < h; ++y)
      {
         if (matrix_[x - 1][y - 1] != rhs.matrix_[x][y])
         {
            count++;
         }
      }
   }
   if (count < min)
      min = count;

   return min;
}
