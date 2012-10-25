#ifndef CARDOCR_H
#define CARDOCR_H

#include <QImage>
#include <QtCore>

class BoolMatrix;
typedef QList<QPoint> PointList;

struct ProfileItem
{
   //отклонение
   bool item;
   //величина выраженная в процентах ко всей области
   qreal value;
};

class CardOCR
{
public:
   CardOCR();
   ~CardOCR();
   QString nominal(const QImage * img_wb);
   QString suit(const QImage * img_wb, const QImage * img);
protected:
   bool isTen(const BoolMatrix &) const;
   bool isSixProfile(const QVector<ProfileItem> & profile);
   bool isLeftQueenProfile(const QVector<ProfileItem> & profile);
   bool isRightQueenProfile(const QVector<ProfileItem> & profile);
   bool isNineProfile(const QVector<ProfileItem> & profile);
   bool isAceProfile(const QVector<ProfileItem> & profile);
   bool isLeftTwoProfile(const QVector<ProfileItem> & profile);
   bool isRightTwoProfile(const QVector<ProfileItem> & profile);
   bool isRightFiveProfile(const QVector<ProfileItem> & profile);
   bool isLeftFiveProfile(const QVector<ProfileItem> & profile);
   bool isLeftThreeProfile(const QVector<ProfileItem> & profile);
   bool isRightThreeProfile(const QVector<ProfileItem> & profile);
   bool isLeftJackProfile(const QVector<ProfileItem> & profile);
   bool isLeftKingProfile(const QVector<ProfileItem> & profile);
   bool isRightKingProfile(const QVector<ProfileItem> & profile);
   bool isRightSevenProfile(const QVector<ProfileItem> & profile);
   bool isDownSevenProfile(const QVector<ProfileItem> & profile);
   bool isLeftFourProfile(const QVector<ProfileItem> & profile);
   
   bool isRed(const QRgb & rgb) const;
   bool isBlue(const QRgb & rgb) const;
   bool isGreen(const QRgb & rgb) const;
   QVector<ProfileItem> scanLeftProfile(const BoolMatrix & imgMatrix);
   QVector<ProfileItem> scanRightProfile(const BoolMatrix & imgMatrix);
   QVector<ProfileItem> scanDownProfile(const BoolMatrix & imgMatrix);

   QList<PointList> closedAreas(const BoolMatrix &) const;
   bool isCrux(const BoolMatrix & bm, const int x, const int y) const;
   QImage * img_;
};

#endif
