#include "SettingsData.h"

QDataStream &operator<<(QDataStream &out, const SettingsData &myObj)
{
   out << myObj.turnBeep << myObj.foldBeep << myObj.checkBeep << myObj.regKeys 
      << myObj.who << myObj.firstRun << myObj.interval << myObj.visualAlert 
      << myObj.showFolded
      << myObj.advisorMode
      << myObj.lang
      << myObj.room;
   return out;
}

QDataStream &operator>>(QDataStream &in, SettingsData &myObj)
{
   in >> myObj.turnBeep >> myObj.foldBeep >> myObj.checkBeep >> 
      myObj.regKeys >> myObj.who >> myObj.firstRun >> myObj.interval 
      >> myObj.visualAlert 
      >> myObj.showFolded 
      >> myObj.advisorMode 
      >> myObj.lang >> myObj.room;
   return in;
}
