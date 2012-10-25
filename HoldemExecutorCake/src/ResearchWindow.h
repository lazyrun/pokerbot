#ifndef MAINWINDOW_H
#define MAINWINDOW_H

class BoolMatrix;
class ResearchWindow : public QMainWindow
{
   Q_OBJECT
public:
   ResearchWindow();

private:
   void checkPreflop();
   void findHoleCards();
   void toBlackWhiteImages();
   void whiteOnBlack();
   void whiteRect();
   void findWhiteRect(const BoolMatrix & bm, int x, int y);
   QPoint whiteCenter(const BoolMatrix & bm);
   QList<int> closedAreas(const BoolMatrix & bm);
   void fillImage(QImage & img, int x, int y);
   void fillMatrix(BoolMatrix & bm, int x, int y);
   
   bool isWhiteBordered(const BoolMatrix & bm, const QRect & rect);
   bool isBlackBordered(const BoolMatrix & bm, const QRect & rect);
   
   bool hasBlack(const BoolMatrix & bm, const QRect & rect);
   bool hasWhite(const BoolMatrix & bm, const QRect & rect);
   
   QRect findFirstWhiteRect(const BoolMatrix & bm);
   QRect findFirstBlackRect(const BoolMatrix & bm);
   
   QRect cutBlack(const BoolMatrix & bm, const QRect & br);
   
   void unpixel(QImage * image);
   bool isTopOne(const BoolMatrix & bm, int cx, int cy);
   bool isLeftOne(const BoolMatrix & bm, int cx, int cy);
   bool isRightOne(const BoolMatrix & bm, int cx, int cy);
   bool isBottomOne(const BoolMatrix & bm, int cx, int cy);

   QList<QRect> whiteRects_;
private:
   bool isBorder(const BoolMatrix & bm, int x, int y);
   QList<QPoint> siblingBorder(const BoolMatrix & bm, int x, int y);
   void closure(const BoolMatrix & bm, QPoint start, QPoint end);
   QList<QPoint> border_;
   //Функции
   void setMainWidget();
   void setMainWidget2();
   void setMainWidget3();
   void setMainWidget4();
   void setMainWidget5();
   void setMainWidget6();
   void setMainWidget7();
   void setMainWidget8();
   QString cardString(int nom, int suit);
   void cutImage(QImage & img);
   void skelet(QImage & img);
   void skelet_zs(QImage & img);
};

#endif
