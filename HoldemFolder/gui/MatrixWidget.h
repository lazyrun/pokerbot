#ifndef MATRIXWIDGET_H
#define MATRIXWIDGET_H

class MatrixButton : public QPushButton
{
Q_OBJECT
public:
   enum Suit
   {
      Offsuited,
      Pair,
      Suited
   };
   MatrixButton(Suit suit, QWidget * parent = 0);
};

class MatrixWidget : public QWidget
{
Q_OBJECT
public:
   MatrixWidget(QWidget * parent = 0, Qt::WindowFlags f = 0);
   QString cardRange() const;
   void setCardRange(const QString & range);
   void clear();
   void selectRange(const QString & ranges);
protected slots:
   void buttonPressed();
   void clicked(int);
   void rangeSelected(const QString & range);
protected:
   void setupUi();
	void mouseMoveEvent(QMouseEvent * e);
   void mousePressEvent(QMouseEvent * e);
	void mouseReleaseEvent(QMouseEvent * e);
   void checkButton(int row, int col, bool state = true);
   QStringList parseRange(const QString & range);
protected:
   MatrixButton * pressedButton_;
   bool pressedState_;
   QSignalMapper * signalMapper_;
   QGridLayout * grid_;
   QMap<QString, MatrixButton *> cardsMap_;
};

#endif

