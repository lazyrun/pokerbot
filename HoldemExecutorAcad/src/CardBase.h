#ifndef CARDBASE_H
#define CARDBASE_H

#include <QList>
#include "BoolMatrix.h"

class CardBase
{
public:
   CardBase();
   ~CardBase();
   int count() const { return cards.count(); }
   const BoolMatrix * matrix(int idx) const { return cards.at(idx).matrix; }
   QString nominal(int idx) const { return cards.at(idx).nominal; }
protected:
   struct Card
   {
      BoolMatrix * matrix;
      QString nominal;
   };
   QList<Card> cards;

};

inline CardBase::CardBase()
{
   #include "cards.h"
}

inline CardBase::~CardBase()
{
   for (int i = 0; i < cards.count(); i++)
   {
      delete cards.at(i).matrix;
   }
}

#endif
