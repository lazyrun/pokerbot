#include "MatrixWidget.h"

//
//CardButton
//
MatrixButton::MatrixButton(Suit suit, QWidget * parent)
: QPushButton(parent)
{
   setCheckable(true);
   if (suit == Suited)
   {
      setStyleSheet(".MatrixButton {"
         "font: bold; font-size: 12px;"
         "border: 1px solid #83AEE4;"
         "border-radius: 6px;"
         "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
         "stop: 0 #d9ebfb, stop: 1 #a1d4fb);"
         "}"
         "QPushButton:checked {"
         "border: 1px solid #d6e212;"
         "border-radius: 6px;"
         "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
         "stop: 0 #e3ff12, stop: 1 #dbff26);"
         "}");
   }
   else if (suit == Pair)
   {
      setStyleSheet(".MatrixButton {"
         "font: bold; font-size: 12px;"
         "border: 1px solid #9883e4;"
         "border-radius: 6px;"
         "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
         "stop: 0 #dad5fb, stop: 1 #ada4fc);"
         "}"
         "QPushButton:checked {"
         "border: 1px solid #d6e212;"
         "border-radius: 6px;"
         "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
         "stop: 0 #e3ff12, stop: 1 #dbff26);"
         "}");
   }
   else if (suit == Offsuited)
   {
      setStyleSheet(".MatrixButton {"
         "font: bold; font-size: 12px;"
         "border: 1px solid #db9b6a;"
         "border-radius: 6px;"
         "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
         "stop: 0 #fbe6d2, stop: 1 #fac49b);"
         "}"
         "QPushButton:checked {"
         "border: 1px solid #d6e212;"
         "border-radius: 6px;"
         "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
         "stop: 0 #e3ff12, stop: 1 #dbff26);"
         "}");
   }
}


//
//MatrixWidget
//
MatrixWidget::MatrixWidget(QWidget * parent, Qt::WindowFlags f) 
: QWidget(parent, f)
{
   pressedButton_ = 0;
   setupUi();
}

void MatrixWidget::setupUi()
{
   grid_ = new QGridLayout;
   grid_->setSizeConstraint(QLayout::SetFixedSize);
   grid_->setMargin(2);
   QStringList cList;
   cList << "A" << "K" << "Q" << "J" << "T" 
         << "9" << "8" << "7" << "6" << "5" 
         << "4" << "3" << "2";

   for (short i = 0; i < cList.count() - 1; i++)
   {
      for (short j = i + 1; j < cList.count(); j++)
      {
         MatrixButton * cb = new MatrixButton(MatrixButton::Offsuited, this);
         QString name = cList.at(i) + cList.at(j) + "o";
         cardsMap_.insert(name, cb);
         cb->setText(name);
         cb->setFixedSize(32, 32);
         grid_->addWidget(cb, j + 1, i + 1);
         connect(cb, SIGNAL(pressed()), this, SLOT(buttonPressed()));
      }
   }

   for (short i = 0; i < cList.count(); i++)
   {
      MatrixButton * cb = new MatrixButton(MatrixButton::Pair, this);
      QString name = cList.at(i) + cList.at(i);
      cardsMap_.insert(name, cb);
      cb->setText(name);
      cb->setFixedSize(32, 32);
      grid_->addWidget(cb, i + 1, i + 1);
      connect(cb, SIGNAL(pressed()), this, SLOT(buttonPressed()));
   }

   for (short i = 0; i < cList.count() - 1; i++)
   {
      for (short j = i + 1; j < cList.count(); j++)
      {
         MatrixButton * cb = new MatrixButton(MatrixButton::Suited, this);
         QString name = cList.at(i) + cList.at(j) + "s";
         cardsMap_.insert(name, cb);
         cb->setText(name);
         cb->setFixedSize(32, 32);
         grid_->addWidget(cb, i + 1, j + 1);
         connect(cb, SIGNAL(pressed()), this, SLOT(buttonPressed()));
      }
   }
   
   setLayout(grid_);
}

void MatrixWidget::clicked(int id)
{
   if (id < 100)
   {
      //select row
      for (int i = 1; i < grid_->columnCount(); i++)
      {
         checkButton(id, i);
      }
   }
   else
   {
      //select column
      for (int i = 1; i < grid_->rowCount(); i++)
      {
         checkButton(i, id - 100);
      }
   }
}

void MatrixWidget::buttonPressed()
{
   MatrixButton * mb = qobject_cast<MatrixButton *>(sender());
   if (mb)
   {
      pressedButton_ = mb;
      pressedState_ = mb->isChecked();
   }
   //grabMouse();
}

void MatrixWidget::mouseMoveEvent(QMouseEvent * e)
{
   QWidget * wid = QApplication::widgetAt(e->globalPos());
   MatrixButton * mb = qobject_cast<MatrixButton *>(wid);
   if (mb)
   {
      mb->setChecked(!pressedState_);
      if (pressedButton_)
         pressedButton_->setChecked(!pressedState_);
   }
   QWidget::mouseMoveEvent(e);
}

void MatrixWidget::mousePressEvent(QMouseEvent * e)
{
   //запомнить кнопачку с которой начали жмакать
   QWidget * wid = QApplication::widgetAt(e->globalPos());
   MatrixButton * mb = qobject_cast<MatrixButton *>(wid);
   if (mb)
   {
      pressedButton_ = mb;
      pressedState_ = mb->isChecked();
   }
   //grabMouse();
	QWidget::mousePressEvent(e);
}

void MatrixWidget::mouseReleaseEvent(QMouseEvent * e)
{
   QWidget * wid = QApplication::widgetAt(e->globalPos());
   MatrixButton * mb = qobject_cast<MatrixButton *>(wid);
   if (mb)
   {
      if (mb == pressedButton_)
      {
         //just click
         mb->setChecked(!pressedState_);
      }
   }
   pressedButton_ = 0;
   //releaseMouse();
	QWidget::mouseReleaseEvent(e);
}

void MatrixWidget::clear()
{
   for (int i = 1; i < grid_->rowCount(); i++)
   {
      for (int j = 1; j < grid_->columnCount(); j++)
      {
         checkButton(i, j, false);
      }
   }
}

void MatrixWidget::checkButton(int row, int col, bool state)
{
   QLayoutItem * item = grid_->itemAtPosition(row, col);
   if (item)
   {
      QWidget * wid = item->widget();
      if (wid)
      {
         MatrixButton * mb = qobject_cast<MatrixButton *>(wid);
         if (mb)
         {
            mb->setChecked(state);
         }
      }
   }
}

void MatrixWidget::selectRange(const QString & ranges)
{
   QStringList lst = ranges.split(",", QString::SkipEmptyParts);
   foreach (QString range, lst)
   {
      range = range.trimmed();
      // развернуть диапазон
      QStringList cards = parseRange(range);
      foreach (QString card, cards)
      {
         if (cardsMap_.contains(card))
         {
            MatrixButton * mb = cardsMap_[card];
            if (mb)
            {
               mb->setChecked(true);
            }
         }
      }
   }
}

QStringList MatrixWidget::parseRange(const QString & range)
{
   // "Ax", "Axs", "Axo" - любые тузы
   // "22+", "77+" - пары
   // "AT+", "23+" - от второй карты до верху
   // "AK", "AKs", "AKo" - точные карты
   // "A9s+" - одномастные от 9 до верху
   QStringList cards;
   cards << "" << "" << "2" << "3" << "4" << "5" << "6" << "7" << "8" << "9" 
         << "T" << "J" << "Q" << "K" << "A";

   enum Nominal
   {
      T = 10,
      J = 11, 
      Q = 12,
      K = 13,
      A = 14
   };

   QStringList lst;
   QString left = range.left(1);
   QString right = range.mid(1, 1);
   if (range.contains("x", Qt::CaseInsensitive))
   {
      for (int i = 2; i <= A; i++)
      {
         QString right = cards.at(i);
         QString card = left + cards.at(i);
         if (right == left)
         {
            lst << card;
            continue;
         }
         if (range.contains("o"))
         {
            lst << card + "o";
         }
         else if (range.contains("s"))
         {
            lst << card + "s";
         }
         else
         {
            lst << card + "o" << card + "s";
         }
      }
   }
   else if (range.contains("+", Qt::CaseInsensitive))
   {
      //пары
      if (left == right)
      {
         if (cards.contains(left))
         {
            int nom = cards.indexOf(left);
            for (int i = nom; i <= A; i++)
            {
               lst << cards.at(i) + cards.at(i);
            }
         }
      }
      else
      {
         //прочее
         // "AT+", "23+" - от второй карты до верху
         if (cards.contains(right))
         {
            int nom = cards.indexOf(right);
            for (int i = nom; i <= A; i++)
            {
               QString right = cards.at(i);
               QString card = left + cards.at(i);
               if (right == left)
               {
                  lst << card;
                  continue;
               }

               if (range.contains("o"))
               {
                  lst << card + "o";
               }
               else if (range.contains("s"))
               {
                  lst << card + "s";
               }
               else
               {
                  lst << card + "o" << card + "s";
               }
            }
         }
      }
   }
   else
   {
      if (left != right)
      {
         if (!range.contains("o") && !range.contains("s"))
         {
            lst << range + "o" << range + "s";
         }
         else
         {
            lst << range;
         }
      }
      else
      {
         lst << range;
      }
   }

   return lst;
}

void MatrixWidget::rangeSelected(const QString & range)
{
   clear();
   selectRange(range);
}

QString MatrixWidget::cardRange() const
{
   QString range;
   //собираем со всех кнопок
   foreach (QString key, cardsMap_.keys())
   {
      MatrixButton * mb = cardsMap_[key];
      if (mb && mb->isChecked())
      {
         range += ", " + mb->text();
      }
   }
   return range;
}

void MatrixWidget::setCardRange(const QString & range)
{
   selectRange(range);
}

