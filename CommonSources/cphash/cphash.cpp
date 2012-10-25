#include <QtCore>
#include <QImage>

qint64 solveHash(const QImage & img)
{
   //уменьшаем до пропорций 8х8
   QImage img1 = img.scaled(8, 8);
   //переводим в градации серого и находим среднее значение
   int avr = 0;
   for (int x = 0; x < img1.width(); x++)
   {
      for (int y = 0; y < img1.height(); y++)
      {
         QRgb rgb = img1.pixel(x, y);
         avr += qGray(rgb);
      }
   }
   avr /= 64;

   qint64 hash = 0;
   for (int x = 0; x < img1.width(); x++)
   {
      for (int y = 0; y < img1.height() - 1; y++)
      {
         QRgb rgb = img1.pixel(x, y);
         int gr = qGray(rgb);
         if (gr < avr)
         {
            //0
            hash = hash << 1;
         }
         else
         {
            //1
            hash = hash | 1;
            hash = hash << 1;
         }
      }
   }

   return hash;
}

short compareHash(const qint64 & hash1, const qint64 & hash2)
{
   qint64 hx = hash1 ^ hash2;

   short cnt = 0;
   while (hx)
   {
      qint64 hxs = hx >> 1;
      hxs = hxs << 1;
      if (hxs != hx)
         cnt++;
      hx = hx >> 1;
   }
   return cnt;
}

