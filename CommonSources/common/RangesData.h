#ifndef RANGESDATA_H
#define RANGESDATA_H

struct Range
{
   QString matrix;
   QString name;
   QString modifiers;
   QString hotkey;
};
Q_DECLARE_METATYPE(Range)

QDataStream &operator<<(QDataStream &out, const Range &myObj);
QDataStream &operator>>(QDataStream &in, Range &myObj);

#endif

