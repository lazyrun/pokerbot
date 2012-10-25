#include "RangesData.h"

QDataStream &operator<<(QDataStream &out, const Range &myObj)
{
   out << myObj.matrix << myObj.name << myObj.modifiers << myObj.hotkey;
   return out;
}

QDataStream &operator>>(QDataStream &in, Range &myObj)
{
   in >> myObj.matrix >> myObj.name >> myObj.modifiers >> 
      myObj.hotkey;
   return in;
}

