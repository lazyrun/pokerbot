#ifndef ENUM_H
#define ENUM_H

//! Перечисление - номинал карты
enum CardNominal
{
   Unnominaled = 0,
   Two   = 1,
   Three = 2,
   Four  = 3,
   Five  = 4,
   Six   = 5,
   Seven = 6,
   Eight = 7,
   Nine  = 8,
   Ten   = 9,
   Jack  = 10,
   Queen = 11,
   King  = 12,
   Ace   = 13
};
//! Перечисление - масть карты
enum CardSuit
{
   Unsuited = 0,
   Hearts   = 1, //!< Черви
   Diamonds = 2, //!< Бубны
   Clubs    = 3, //!< Трефы
   Spades   = 4  //!< Пики
};

#endif