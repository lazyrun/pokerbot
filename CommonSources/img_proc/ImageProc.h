#ifndef IMAGEPROC_H
#define IMAGEPROC_H

#include "BoolMatrix.h"

//! ������� � �����-����� �� �������� ������
void toBlackWhite(QImage & img, uchar threshold = 240);

//! ������� � �����-����� �� maxHue
void toBlackWhiteByHue(QImage & img);

void toBlackWhiteByHue(QImage & img, int minHue, int maxHue);

void toBlackWhiteMid(QImage & img, const int threshold);

//������ ���
void denoise(QImage & img, uchar threshold);

//! ������� �������� ��������� �������
int hueCount(const QImage & img, int minHue, int maxHue, int fromw, int fromh, int thr, QPoint & pt);

//! ����� ������������� �������� �������
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
   //! ������ �����������
   void setImage(const QImage & img);
   //! �������� �� ������ ��������
   bool isPreflop() const;
   //! ������� �������
   HoldemLevel holdemLevel() const;
   //! �������� ���������� ��������� ����
   QPair<QRect, QRect> getHoleCards(bool * ok);
   
   uchar threshold() const {return threshold_; }
protected:
   //! ��������� ������� �����
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
