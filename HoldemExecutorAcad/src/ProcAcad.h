#ifndef PROCACAD_H
#define PROCACAD_H

#include "ImageProc.h"

class ProcAcad : public CardProcessing
{
public:
   ProcAcad();
   ~ProcAcad();
   //! Задать изображение
   void setImage(const QImage & img);
   QPair<QRect, QRect> getHoleCards(bool * ok);
   static int countCheckLetters(const QImage & img);
protected:
   //! Выделение области флопа
   QRect flopRect(const QImage & img) const;
};

#endif
