#ifndef PROCACAD_H
#define PROCACAD_H

#include "ImageProc.h"

class ProcAcad : public CardProcessing
{
public:
   ProcAcad();
   ~ProcAcad();
   //! ������ �����������
   void setImage(const QImage & img);
   QPair<QRect, QRect> getHoleCards(bool * ok);
   static int countCheckLetters(const QImage & img);
protected:
   //! ��������� ������� �����
   QRect flopRect(const QImage & img) const;
};

#endif
