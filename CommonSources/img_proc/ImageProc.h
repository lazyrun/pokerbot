#ifndef IMAGEPROC_H
#define IMAGEPROC_H

#include "BoolMatrix.h"

//! Перевод в черно-белое по значению серого
void toBlackWhite(QImage & img, uchar threshold = 240);

//! Перевод в черно-белое по maxHue
void toBlackWhiteByHue(QImage & img);

void toBlackWhiteByHue(QImage & img, int minHue, int maxHue);

void toBlackWhiteMid(QImage & img, const int threshold);

//убрать шум
void denoise(QImage & img, uchar threshold);

//! Подсчет пикселей заданного оттенка
int hueCount(const QImage & img, int minHue, int maxHue, int fromw, int fromh, int thr, QPoint & pt);

//! Поиск максимального значения оттенка
int maxHue(const QImage & img);

class CardProcessing
{
public:
   enum HoldemLevel
   {
      Undefined,
      Preflop,
      Flop,
      Turn,
      River
   };

   CardProcessing();
   ~CardProcessing();
   //! Задать изображение
   void setImage(const QImage & img);
   //! Проверка на стадию префлопа
   bool isPreflop() const;
   //! Вернуть уровень
   HoldemLevel holdemLevel() const;
   //! Получить координаты карманных карт
   QPair<QRect, QRect> getHoleCards(bool * ok);
   
   uchar threshold() const {return threshold_; }
protected:
   //! Выделение области флопа
   QRect flopRect(const QImage & img) const;
protected:
   BoolMatrix * matrix_;
   QRect maxWhite_;
   QList<Border> verticals_;
   QList<Border> horizontals_;
   
   uchar threshold_;
   uchar dim_;
   uchar minSizeVertical_;
   uchar minSizeHorizontal_;

   HoldemLevel holdemLevel_;
};

#endif
