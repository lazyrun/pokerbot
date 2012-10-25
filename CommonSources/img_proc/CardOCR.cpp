#include <QColor>
#include "CardOCR.h"
#include "BoolMatrix.h"
#include "ImageProc.h"

CardOCR::CardOCR()
{
   img_ = 0;
}

CardOCR::~CardOCR()
{
   delete img_;
}

QString CardOCR::suit(const QImage * img_wb, const QImage * img)
{
   if ((img_wb->width() != img->width()) ||
      (img_wb->height() != img->height()))
      return QString();

   const int w = img->width();
   const int h = img->height();
   
   QRgb clMinValue;
   qreal minVal = 1.0;
   for (int x = 0; x < w; ++x)
   {
      for (int y = 0; y < h; ++y)
      {
         QRgb rgb_wb = img_wb->pixel(x, y);
         //рассматриваем только черные точки
         if (rgb_wb == qRgb(0, 0, 0))
         {
            QRgb rgb = img->pixel(x, y);
            QColor cl(rgb);
            if (cl.valueF() < minVal)
            {
               minVal = cl.valueF();
               clMinValue = rgb;
               //qDebug() << x << y;
            }
         }
      }
   }
   
   if (isRed(clMinValue))
      return "h";
   else if (isBlue(clMinValue))
      return "d";
   else if (isGreen(clMinValue))
      return "c";
   else
      return "s";

   return QString();
}

bool CardOCR::isRed(const QRgb & rgb) const
{
   int red = qRed(rgb);
   int blue = qBlue(rgb);
   int green = qGreen(rgb);
   
   if ((red < 128) && (blue < 128) && (green < 128))
      return false;

   if ((red > blue * 2) && (red > green * 2))
   {
      return true;
   }
   return false;
}

bool CardOCR::isBlue(const QRgb & rgb) const
{
   int red = qRed(rgb);
   int blue = qBlue(rgb);
   int green = qGreen(rgb);
   
   if ((red < 128) && (blue < 128) && (green < 128))
      return false;

   if ((blue > red * 2) && (blue > green * 2))
   {
      return true;
   }
   
   return false;
}

bool CardOCR::isGreen(const QRgb & rgb) const
{
   int red = qRed(rgb);
   int blue = qBlue(rgb);
   int green = qGreen(rgb);
   
   if ((red < 128) && (blue < 128) && (green < 128))
      return false;

   if ((green > blue * 2) && (red > blue * 2))
   {
      return true;
   }
   
   return false;
}

QString CardOCR::nominal(const QImage * img)
{
   img_ = new QImage(*img);
   //toBlackWhiteMid(*img_, 128);
   //img_->save("cc.bmp");
   BoolMatrix imgMatrix(img_);
   //1 проверка - не является ли это 10
   if (isTen(imgMatrix))
   {
      return "T";
   }
   //не является 10
   //разбиваем на замкнутые области
   QList<PointList> areas = closedAreas(imgMatrix);

   int cnt = areas.count();
   
   if (cnt > 2)
   {
      //произошла ошибка что-то нужно делать
      return QString();
   }

   if (cnt == 2)
   {
      //это может быть 8 или Q
      //сравниваем площади
      PointList list0 = areas.at(0);
      PointList list1 = areas.at(1);
      qreal c0 = list0.count();
      qreal c1 = list1.count();
      qreal q = 0;  
      if (c0 > c1)
         q = c0 / c1;
      else
         q = c1 / c0;
      if (q < 2.0)
      {
         //8
         return "8";
      }
   }

   //построить профиль слева
   QVector<ProfileItem> leftProfile = scanLeftProfile(imgMatrix);
   //построить профиль справа
   QVector<ProfileItem> rightProfile = scanRightProfile(imgMatrix);

   //получили картину профиля
   //foreach (ProfileItem item, leftProfile)
   //{
   //   qDebug() << item.item << item.value;
   //}
   //qDebug() << "------";
   //foreach (ProfileItem item, rightProfile)
   //{
   //   qDebug() << item.item << item.value;
   //}
   
   bool isLeftAce   = isAceProfile(leftProfile);
   bool isRightAce  = isAceProfile(rightProfile);

   if (isLeftAce && ((cnt >= 1) || isRightAce))
      return "A";

   bool isLeftQueen   = isLeftQueenProfile(leftProfile);
   bool isRightQueen  = isRightQueenProfile(rightProfile);
   bool isRightSix    = isSixProfile(rightProfile);
   
   if (isLeftQueen && isRightQueen && cnt >= 1)
      return "Q";
   if (isLeftQueen && isRightSix && cnt >= 1)
      return "Q";

   bool isLeftSix   = isSixProfile(leftProfile);
   //bool isRightSix  = isSixProfile(rightProfile);

   if (isLeftSix && cnt >= 1)
      return "6";

   if (isRightSix && cnt >= 1)
      return "9";

   bool isLeftTwo  = isLeftTwoProfile(leftProfile);
   bool isRightTwo = isRightTwoProfile(rightProfile);
   if (isLeftTwo && isRightTwo)
      return "2";

   bool isLeftFive  = isLeftFiveProfile(leftProfile);
   bool isRightFive = isRightFiveProfile(rightProfile);
   if (isLeftFive && isRightFive)
      return "5";

   bool isLeftJack  = isLeftJackProfile(leftProfile);
   bool isRightJack = isSixProfile(rightProfile);//так и задумано
   if (isLeftJack && isRightJack)
      return "J";
   
   bool isLeftKing  = isLeftKingProfile(leftProfile);
   bool isRightKing = isRightKingProfile(rightProfile);
   if (isLeftKing && isRightKing)
      return "K";

   //7 перед 3
   //построить профиль снизу
   QVector<ProfileItem> downProfile = scanDownProfile(imgMatrix);
   //qDebug() << "------------";
   //foreach (ProfileItem item, downProfile)
   //{
   //   qDebug() << item.item << item.value;
   //}
   
   bool isRightSeven = isRightSevenProfile(rightProfile);
   bool isDownSeven  = isDownSevenProfile(downProfile);
   bool isRightThree6 = isSixProfile(rightProfile);//так и задумано
   bool isRightThree3 = isRightThreeProfile(rightProfile);
   if (isRightSeven && isDownSeven && !isRightThree6)
   {
      return "7";
   }
   
   //осталась только тройка с таким правым профилем
   //bool isRightThree = isSixProfile(rightProfile);//так и задумано
   if (isRightThree3 || isRightThree6)
      return "3";

   bool isLeftFour = isLeftFourProfile(leftProfile);
   if (isLeftFour)
      return "4";

   return QString();
}

QVector<ProfileItem> CardOCR::scanLeftProfile(const BoolMatrix & imgMatrix)
{
   //дальше сравнене по профилям
   //сканирование левого профиля
   const int w = imgMatrix.width();
   const int h = imgMatrix.height();
   int min_x = w, max_x = 0;

   QVector<int> x_array;
   for (int y = 0; y < h; ++y)
   {
      for (int x = 0; x < w; ++x)
      {
         if (imgMatrix.at(x, y) == 1)
         {
            if (x < min_x)
               min_x = x;
            if (x > max_x)
               max_x = x;
            x_array.push_back(x);
            break;
         }
      }
   }
   
   //определяем середину
   int x_border = min_x + (max_x - min_x) / 2;
   //формируем портрет профиля
   QVector<bool> prof;
   QVector<ProfileItem> profile;

   //qDebug() << x_array;
   int ccnt = 0;
   bool item = true;
   foreach (int x, x_array)
   {
      item = (x > x_border);
      if (prof.size() == 0)
      {
         prof.push_back(item);
         ccnt++;
      }
      else
      {
         if (prof.last() != item)
         {
            ProfileItem it;
            it.item = !item;
            it.value = qreal(ccnt) / qreal(h);
            profile.push_back(it);
            prof.push_back(item);
            ccnt = 1;
         }
         else
         {
            ccnt++;
         }
      }
   }

   if (ccnt != 0)
   {
      ProfileItem it;
      it.item = item;
      it.value = qreal(ccnt) / qreal(h);
      profile.push_back(it);
   }
   
   return profile;
}

QVector<ProfileItem> CardOCR::scanRightProfile(const BoolMatrix & imgMatrix)
{
   //сканирование правого профиля
   const int w = imgMatrix.width();
   const int h = imgMatrix.height();
   int min_x = w, max_x = 0;

   QVector<int> x_array;
   for (int y = 0; y < h; ++y)
   {
      for (int x = w - 1; x >= 0; --x)
      {
         if (imgMatrix.at(x, y) == 1)
         {
            if (x < min_x)
               min_x = x;
            if (x > max_x)
               max_x = x;
            x_array.push_back(x);
            break;
         }
      }
   }
   
   //определяем середину
   int x_border = min_x + (int)qRound(qreal(max_x - min_x) / 2. + 0.1);
   //формируем портрет профиля
   QVector<bool> prof;
   QVector<ProfileItem> profile;

   //qDebug() << x_array;
   int ccnt = 0;
   bool item = true;
   foreach (int x, x_array)
   {
      item = (x < x_border);
      if (prof.size() == 0)
      {
         prof.push_back(item);
         ccnt++;
      }
      else
      {
         if (prof.last() != item)
         {
            ProfileItem it;
            it.item = !item;
            it.value = qreal(ccnt) / qreal(h);
            profile.push_back(it);
            prof.push_back(item);
            ccnt = 1;
         }
         else
         {
            ccnt++;
         }
      }
   }

   if (ccnt != 0)
   {
      ProfileItem it;
      it.item = item;
      it.value = qreal(ccnt) / qreal(h);
      profile.push_back(it);
   }
   
   return profile;
}

QVector<ProfileItem> CardOCR::scanDownProfile(const BoolMatrix & imgMatrix)
{
   //сканирование нижнего профиля
   const int w = imgMatrix.width();
   const int h = imgMatrix.height();
   int min_y = h, max_y = 0;

   QVector<int> y_array;
   for (int x = 1; x < w; ++x)
   {
      for (int y = h - 1; y >= 0; --y)
      {
         if (imgMatrix.at(x, y) == 1)
         {
            if (y < min_y)
               min_y = y;
            if (y > max_y)
               max_y = y;
            y_array.push_back(y);
            break;
         }
      }
   }
   
   //определяем середину
   int y_border = min_y + (max_y - min_y) / 2;
   //формируем портрет профиля
   QVector<bool> prof;
   QVector<ProfileItem> profile;

   //qDebug() << x_array;
   int ccnt = 0;
   bool item = true;
   foreach (int y, y_array)
   {
      item = (y < y_border);
      if (prof.size() == 0)
      {
         prof.push_back(item);
         ccnt++;
      }
      else
      {
         if (prof.last() != item)
         {
            ProfileItem it;
            it.item = !item;
            it.value = qreal(ccnt) / qreal(w);
            profile.push_back(it);
            prof.push_back(item);
            ccnt = 1;
         }
         else
         {
            ccnt++;
         }
      }
   }

   if (ccnt != 0)
   {
      ProfileItem it;
      it.item = item;
      it.value = qreal(ccnt) / qreal(w);
      profile.push_back(it);
   }
   
   return profile;
}

bool CardOCR::isDownSevenProfile(const QVector<ProfileItem> & profile)
{
//false 0.625 
//true 0.25    
   if (profile.count() != 2)
      return false;
   if ((profile.at(0).item == false) &&
      (profile.at(1).item == true) &&
      (profile.at(1).value * 3. > profile.at(0).value))
      return true;

   return false;
}

bool CardOCR::isLeftFourProfile(const QVector<ProfileItem> & profile)
{
//true 0.181818 
//false 0.636364 
//true 0.0909091 
//***
//false 0.769231 
//true 0.153846    

   int idx = -1;
   if (profile.count() >= 2)
   {
      for (int i = 0; i < profile.count() - 1; i++)
      {
         if ((profile.at(i).item == false)   &&
             (profile.at(i + 1).item == true) )
         {
            idx = i;
            break;
         }
      }
   }
       
   if (idx == -1)
      return false;


   if (profile.at(idx).value > profile.at(idx + 1).value)
       return true;
      
   return false;
}

bool CardOCR::isRightThreeProfile(const QVector<ProfileItem> & profile)
{
//true 0.0588235 
//false 0.352941 
//true 0.0588235 
//false 0.411765 
//true 0.0588235 
  
   if (profile.count() < 3)
      return false;

   if (profile.count() == 5)
   {
      if ((profile.at(0).item == true)    &&
          (profile.at(1).item == false) &&
          (profile.at(2).item == true) && 
          (profile.at(3).item == false) && 
          (profile.at(4).item == true))
          return true;

   }
      
   return false;
}

bool CardOCR::isLeftThreeProfile(const QVector<ProfileItem> & profile)
{
//false 0.222222 
//true 0.444444 
//false 0.222222 
//true 0.0555556 
//------ 
//true 0.0555556 
//false 0.833333 
//true 0.0555556 
//"3c" "" 
//
//false 0.222222 
//true 0.444444 
//false 0.277778 
//------ 
//true 0.0555556 
//false 0.833333 
//true 0.0555556 
//"3d" "" 
//
//false 0.235294 
//true 0.470588 
//false 0.235294 
//------ 
//false 0.941176 
//"3h" "" 
//
//false 0.235294 
//true 0.411765 
//false 0.294118 
//------ 
//true 0.0588235 
//false 0.823529 
//true 0.0588235 
//"3s" "" 

   if (profile.count() < 3)
      return false;

   int idx = -1;
   if (profile.count() >= 3)
   {
      for (int i = 0; i < profile.count() - 2; i++)
      {
         if ((profile.at(i).item == false)    &&
             (profile.at(i + 1).item == true) &&
             (profile.at(i + 2).item == false))
         {
            idx = i;
            break;
         }
      }
   }
       
   if (idx == -1)
      return false;

   if ((profile.at(idx + 1).value > 0.35) && 
       (profile.at(idx).value < 0.30) &&
       (profile.at(idx + 2).value < 0.30))
       return true;
      
   return false;
}


bool CardOCR::isLeftFiveProfile(const QVector<ProfileItem> & profile)
{
//"2c" "" 
//false 0.555556 
//true 0.222222 
//false 0.166667 
// or
//true 0.0769231 
//false 0.384615 
//true 0.384615 
//false 0.0769231 
   if (profile.count() == 4)
   {
      if ((profile.at(0).item == true)    &&
          (profile.at(1).item == false) &&
          (profile.at(2).item == true) && 
          (profile.at(3).item == false))
      {
         if ((profile.at(1).value > profile.at(0).value) && 
             (profile.at(2).value > profile.at(3).value))
             return true;
      }
   }

   if (profile.count() < 3)
      return false;

   int idx = -1;
   if (profile.count() >= 3)
   {
      for (int i = 0; i < profile.count() - 2; i++)
      {
         if ((profile.at(i).item == false)    &&
             (profile.at(i + 1).item == true) &&
             (profile.at(i + 2).item == false))
         {
            idx = i;
            break;
         }
      }
   }
       
   if (idx == -1)
      return false;

   if ((profile.at(idx).value > 0.40) && 
       (profile.at(idx + 1).value < 0.35) &&
       (profile.at(idx + 2).value < 0.30))
       return true;
      
   return false;
}

bool CardOCR::isRightFiveProfile(const QVector<ProfileItem> & profile)
{
//false 0.111111 
//true 0.277778 
//false 0.555556 
//"5c" "" 
//false 0.111111 
//true 0.277778 
//false 0.555556 
//"5d" "" 
//false 0.117647 
//true 0.235294 
//false 0.588235 
//"5h" "" 
//false 0.117647 
//true 0.235294 
//false 0.588235 
//"5s" "" 
   if (profile.count() < 3)
      return false;

   int idx = -1;
   if (profile.count() >= 3)
   {
      for (int i = 0; i < profile.count() - 2; i++)
      {
         if ((profile.at(i).item == false)    &&
             (profile.at(i + 1).item == true) &&
             (profile.at(i + 2).item == false))
         {
            idx = i;
            break;
         }
      }
   }
       
   if (idx == -1)
      return false;

   if ((profile.at(idx + 2).value > 0.40) && 
       (profile.at(idx).value < 0.30) &&
       (profile.at(idx + 1).value < 0.33))
       return true;
      
   return false;

}

bool CardOCR::isLeftTwoProfile(const QVector<ProfileItem> & profile)
{
//"2h" "" 
//false 0.294118 
//true 0.294118 
//false 0.352941 
   if (profile.count() < 3)
      return false;

   int idx = -1;
   if (profile.count() >= 3)
   {
      for (int i = 0; i < profile.count() - 2; i++)
      {
         if ((profile.at(i).item == false)    &&
             (profile.at(i + 1).item == true) &&
             (profile.at(i + 2).item == false))
         {
            idx = i;
            break;
         }
      }
   }
       
   if (idx == -1)
      return false;

   if ((profile.at(idx + 2).value > profile.at(idx).value) && 
       (profile.at(idx + 2).value > profile.at(idx + 1).value) &&
       qAbs<qreal>(profile.at(idx).value - profile.at(idx + 1).value < 0.1))
       return true;
   
   //максимальное отклонение друг относительно друга не более 15%
   //qreal diff1 = qAbs(profile.at(idx).value - profile.at(idx + 1).value);
   //qreal diff2 = qAbs(profile.at(idx).value - profile.at(idx + 2).value);
   //qreal diff3 = qAbs(profile.at(idx + 1).value - profile.at(idx + 2).value);
   //qreal max_off = diff1;
   //if (diff2 > max_off)
   //   max_off = diff2;
   //if (diff3 > max_off)
   //   max_off = diff3;

   //if (max_off > 0.18)
   //   return false;
   
   return true;
}

bool CardOCR::isRightTwoProfile(const QVector<ProfileItem> & profile)
{
//"2c" "" 
//false 0.555556 
//true 0.222222 
//false 0.166667 
// or
//true 0.0769231 
//false 0.384615 
//true 0.384615 
//false 0.0769231 
   if (profile.count() == 4)
   {
      if ((profile.at(0).item == true)    &&
          (profile.at(1).item == false) &&
          (profile.at(2).item == true) && 
          (profile.at(3).item == false))
      {
         if ((profile.at(1).value > profile.at(0).value) && 
             (profile.at(2).value > profile.at(3).value))
             return true;
      }
   }

   if (profile.count() < 3)
      return false;

   int idx = -1;
   if (profile.count() >= 3)
   {
      for (int i = 0; i < profile.count() - 2; i++)
      {
         if ((profile.at(i).item == false)    &&
             (profile.at(i + 1).item == true) &&
             (profile.at(i + 2).item == false))
         {
            idx = i;
            break;
         }
      }
   }
       
   if (idx == -1)
      return false;

   if ((profile.at(idx).value > 0.40) && 
       (profile.at(idx + 1).value < 0.35) &&
       (profile.at(idx + 2).value < 0.20))
       return true;
      
   return false;
}

bool CardOCR::isAceProfile(const QVector<ProfileItem> & profile)
{
   //профиль должен состояь из 2х частей примерно равных
   bool isAce = false;
   if (profile.count() == 2)
   {
      if ((profile.at(0).value * 4. >  profile.at(1).value) &&
          (profile.at(1).value * 4. >  profile.at(0).value) &&
          (profile.at(0).item == true) && (profile.at(1).item == false))
          isAce = true;
   }
   return isAce;
}

bool CardOCR::isLeftKingProfile(const QVector<ProfileItem> & profile)
{
   if (profile.count() > 6)//бинаризация пустила волну
      return true;
   
   if (profile.count() > 2)
      return false;

   bool isKing = true;
   //поиск максимальной части
   int max_idx = 0;
   qreal maxval = 0.;
   for (int i = 0; i < profile.count(); ++i)
   {
      if (profile.at(i).value > maxval)
      {
         maxval = profile.at(i).value;
         max_idx = i;
      }
   }
   
   for (int i = 0; i < profile.count(); ++i)
   {
      if (i == max_idx)
         continue;
      
      if (profile.at(i).value * 5. >= maxval)
      {
         //условие не выполнено
         isKing = false;
         break;
      }
   }
   return isKing;
}

bool CardOCR::isRightSevenProfile(const QVector<ProfileItem> & profile)
{
//false 0.846154 
//true 0.0769231    
//or
//false 0.416667 
//true 0.5 
   if (profile.count() == 2)
   {
      if ((profile.at(0).item == false) && (profile.at(1).item == true) &&
         (profile.at(0).value > profile.at(1).value))
         return true;

      if ((profile.at(0).item == false) && (profile.at(1).item == true) &&
         qAbs(profile.at(0).value - profile.at(1).value) < 0.2)
         return true;

   }
   return false;
}

bool CardOCR::isRightKingProfile(const QVector<ProfileItem> & profile)
{
//false 0.235294 
//true 0.294118 
//false 0.411765    
   if (profile.count() < 3)
      return false;

   int idx = -1;
   if (profile.count() >= 3)
   {
      for (int i = 0; i < profile.count() - 2; i++)
      {
         if ((profile.at(i).item == false)    &&
             (profile.at(i + 1).item == true) &&
             (profile.at(i + 2).item == false))
         {
            idx = i;
            break;
         }
      }
   }
       
   if (idx == -1)
      return false;

   return true;
}

bool CardOCR::isLeftJackProfile(const QVector<ProfileItem> & profile)
{
   //true 0.846154 
   //false 0.0769231 
   //профиль должен состоять из 2х частей и первая сильно больше
   bool isJack = false;
   if (profile.count() == 2)
   {
      if ((profile.at(0).value >  profile.at(1).value * 3.) &&
          (profile.at(0).item == true) && (profile.at(1).item == false))
          isJack = true;
   }
   return isJack;
}

bool CardOCR::isLeftQueenProfile(const QVector<ProfileItem> & profile)
{
   //false 0.9 
   //true 0.05 
   //профиль должен состояь из 2х частей и первая сильно больше
   bool isQueen = false;
   if (profile.count() == 2)
   {
      if ((profile.at(0).value >  profile.at(1).value * 5.) &&
          (profile.at(0).item == false) && (profile.at(1).item == true))
          isQueen = true;
   }
   return isQueen;
}

bool CardOCR::isRightQueenProfile(const QVector<ProfileItem> & profile)
{
   //true 0.05 
   //false 0.9 
   //or
   //true 0.157895 
   //false 0.578947 
   //true 0.105263 
   //false 0.105263 
   bool isQueen = false;
   if (profile.count() == 2)
   {
      if ((profile.at(1).value >  profile.at(0).value * 5.) &&
          (profile.at(0).item == true) && (profile.at(1).item == false))
          isQueen = true;
   }
   else if (profile.count() == 4)
   {
      if ((profile.at(0).item == true) && (profile.at(1).item == false) &&
          (profile.at(2).item == true) && (profile.at(3).item == false) &&
          (profile.at(0).value < 0.3) && (profile.at(2).value < 0.3) &&
          (profile.at(3).value < 0.3) && (profile.at(1).value > 0.3))
          isQueen = true;
   }
   return isQueen;
}

bool CardOCR::isSixProfile(const QVector<ProfileItem> & profile)
{
   //описание профиля 6
   //     1) одна из частей сильно больше остальных 
   //или  2) всего одна часть и она почти равна всей длине стороны
   if (profile.count() > 6)//бинаризация пустила волну
      return true;

   bool isSix = true;
   if (profile.count() > 1)
   {
      //поиск максимальной части
      int max_idx = 0;
      qreal maxval = 0.;
      for (int i = 0; i < profile.count(); ++i)
      {
         if (profile.at(i).value > maxval)
         {
            maxval = profile.at(i).value;
            max_idx = i;
         }
      }
      
      for (int i = 0; i < profile.count(); ++i)
      {
         if (i == max_idx)
            continue;
         
         if (profile.at(i).value * 5. >= maxval)
         {
            //условие не выполнено
            isSix = false;
            break;
         }
      }
   }
   else if (profile.count() == 1)
   {
      if (profile.at(0).value < 0.75)
         isSix = false;
   }
   else
   {
      isSix = false;
   }

   return isSix;
}

bool CardOCR::isNineProfile(const QVector<ProfileItem> & /*profile*/)
{
   return false;
}


QList<PointList> CardOCR::closedAreas(const BoolMatrix & bmatr_in) const
{
   //матрица в которой уже готовые области закрашиваются черным
   BoolMatrix bmatr(bmatr_in);
   //
   //рабочая матрица для одной области
   BoolMatrix bm(bmatr);
   const int w = bm.width();
   const int h = bm.height();

   //отбираем точки с углом
   PointList startPoints;
   for (int y = 1; y < h - 1; ++y)
   {
      for (int x = 1; x < w - 1; ++x)
      {
         //белый
         if (bm.at(x, y) == 0)
         {
            //смотрим черный угол
            if (bm.at(x - 1, y)     == 1 &&
                //bm.at(x - 1, y - 1) == 1 &&
                bm.at(x, y - 1)     == 1)
               
            {
               startPoints << QPoint(x, y);
            }
         }
      }
   }
   
   if (startPoints.isEmpty())
   {
      return QList<PointList>();
   }
   QList<PointList> areas;
   PointList currentArea;
   QPoint pt = startPoints.at(0);

   //для всех стартовых точек строим замыкание
   for (int si = 0; si < startPoints.count(); ++si)
   {
      QPoint s = startPoints.at(si);
      //стартовая точка - белая
      if ((bm.at(s.x(), s.y()) == 0) && isCrux(bm, s.x(), s.y()))
      {
         QPoint pt = s;
         for (;;)
         {
            //8ми связность
            const QPoint pt1 = QPoint(pt.x(),      pt.y() - 1);
            const QPoint pt2 = QPoint(pt.x() + 1,  pt.y() - 1);
            const QPoint pt3 = QPoint(pt.x() + 1,  pt.y());
            const QPoint pt4 = QPoint(pt.x() + 1,  pt.y() + 1);
            const QPoint pt5 = QPoint(pt.x(),      pt.y() + 1);
            const QPoint pt6 = QPoint(pt.x() - 1,  pt.y() + 1);
            const QPoint pt7 = QPoint(pt.x() - 1,  pt.y());
            const QPoint pt8 = QPoint(pt.x() - 1,  pt.y() - 1);

            QList<QPoint> sublingPoints;
            sublingPoints << pt1 << pt2 << pt3 << pt4 << pt5 << pt6 << pt7 << pt8;

            //крутим в поисках черной точки
            int first_black = 0;
            if (bm.at(pt1.x(), pt1.y()) == 1)
            {
               foreach (QPoint spt, sublingPoints)
               {
                  if (bm.at(spt.x(), spt.y()) == 1)
                  {
                     first_black = sublingPoints.indexOf(spt);
                     break;
                  }
               }
            }
            else
            {
               first_black = 0;
            }
            QList<QPoint> swap1 = sublingPoints.mid(first_black);
            QList<QPoint> swap2 = sublingPoints.mid(0, first_black);
            QList<QPoint> circularedPoints;
            circularedPoints << swap1 << swap2;
            
            bool hasSibling = false;
            bool isBreak = false;
            foreach (QPoint spt, circularedPoints)
            {
               //ищем в окрестности белую точку
               if (bm.at(spt.x(), spt.y()) == 0)
               {
                  bm.set(pt.x(), pt.y(), 1);
                  if (isCrux(bmatr, spt.x(), spt.y()))
                  {
                     currentArea << pt;
                     //startPoints.removeOne(pt);
                     pt = spt;
                     hasSibling = true;
                     break;
                  }
                  else
                  {
                     //область разорвана - стартовая точка - лажа
                     //выходим и пробуем со следующей
                     //очищаем матрицу
                     bm = bmatr;
                     currentArea.clear();
                     isBreak = true;
                     break;
                  }
               }
            }
            if (isBreak)
               break;

            if (!hasSibling)
            {
               bm.set(pt.x(), pt.y(), 1);
               currentArea << pt;
               areas << currentArea;
               bmatr = bm;
               currentArea.clear();
               break;
            }
         }
      }
   }

   return areas;
}

bool CardOCR::isCrux(const BoolMatrix & bm, const int sx, const int sy) const
{
   int leftBound     = -1;
   int rightBound    = -1;
   int topBound      = -1;
   int bottomBound   = -1;
   for (int x = sx - 1; x >= 0; x--)
   {
      if (bm.at(x, sy) == 1)
      {
         leftBound = x;
         break;
      }
   }
   
   for (int x = sx + 1; x < bm.width(); x++)
   {
      if (bm.at(x, sy) == 1)
      {
         rightBound = x;
         break;
      }
   }

   for (int y = sy - 1; y >= 0; y--)
   {
      if (bm.at(sx, y) == 1)
      {
         topBound = y;
         break;
      }
   }
   
   for (int y = sy + 1; y < bm.height(); y++)
   {
      if (bm.at(sx, y) == 1)
      {
         bottomBound = y;
         break;
      }
   }
   
   if (leftBound != -1 && rightBound != -1 && topBound != -1 && bottomBound != -1)
   {
      if ((rightBound - leftBound > 2) || (bottomBound - topBound > 2))
      {
         return true;
      }
   }
   return false;
}

bool CardOCR::isTen(const BoolMatrix & imgMatrix) const
{
   //очерняем
   const int width  = imgMatrix.width();
   const int height = imgMatrix.height();

   //сканируем рисунок на предмет пробелов
   //int countLetters = 0;
   //int count = 0;

   //ищем первый черный пиксель
   int first_black_x = 0;
   for (int x = 0; x < width; ++x)
   {
      for (int y = 0; y < height; ++y)
      {
         if (imgMatrix.at(x, y) == 1)
         {
            first_black_x = x;
            x = width;
            break;
         }
      }
   }
   
   //первый черный пикс в самом конце?
   if (first_black_x == width - 1)
      return false;
   
   //ищем первую абс белую линию
   int first_white_x = 0;
   for (int x = first_black_x + 1; x < width; ++x)
   {
      int whites = 0;
      for (int y = 0; y < height; ++y)
      {
         if (imgMatrix.at(x, y) == 0)
         {
            whites++;
         }
         else
         {
            break;
         }
      }
      if (whites == height)
      {
         first_white_x = x;
         break;
      }
   }

   //белый пикс в самом конце?
   if (first_white_x == width - 1)
      return false;
   
   if (first_white_x <= first_black_x)
      return false;

   //ищем следом черный писк
   for (int x = first_white_x + 1; x < width; ++x)
   {
      for (int y = 0; y < height; ++y)
      {
         //нашелся черный пикс
         if (imgMatrix.at(x, y) == 1)
         {
            return true;
         }
      }
   }
   return false;
}