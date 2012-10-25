#ifndef PROC770_H
#define PROC770_H

#include "ImageProc.h"

class Proc770 : public CardProcessing
{
public:
   Proc770();
   ~Proc770();
   //! ������ �����������
   void setImage(const QImage & img);
   QPair<QRect, QRect> getHoleCards(bool * ok);
   static int countCheckLetters(const QImage & img);
   
   static bool checkHueButton(const QImage & img, int minHue, int maxHue, int rectSz);
   static bool checkHueButton(const QImage & imgBtn, int minHue, int maxHue, int rectMin, QRect & rect);
protected:
   uint minBright_;
   uint maxBright_;
};

#endif
