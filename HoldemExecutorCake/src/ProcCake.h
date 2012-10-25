#ifndef PROCCAKE_H
#define PROCCAKE_H

#include "ImageProc.h"

class ProcCake : public CardProcessing
{
public:
   ProcCake();
   ~ProcCake();
   //! Задать изображение
   void setImage(const QImage & img);
   QPair<QString, QString> getHoleCards(bool * ok);
   static int countCheckLetters(const QImage & img);
   
   static bool checkHueButton(const QImage & img, int minHue, int maxHue, int rectSz);
   static bool checkHueButton(const QImage & imgBtn, int minHue, int maxHue, 
      int rectMinW, int rectMinH, QRect & rect);
protected:
   QImage * img_;
   
   QPoint whiteCenter(const BoolMatrix & bm);
   QRect findFirstBlackRect(const BoolMatrix & bm);
   QRect cutBlack(const BoolMatrix & bm, const QRect & br);
   QRect findFirstWhiteRect(const BoolMatrix & bm);
   bool isBlackBordered(const BoolMatrix & bm, const QRect & rect);
   bool hasWhite(const BoolMatrix & bm, const QRect & rect);
   bool isWhiteBordered(const BoolMatrix & bm, const QRect & rect);
   bool hasBlack(const BoolMatrix & bm, const QRect & rect);
};

#endif
