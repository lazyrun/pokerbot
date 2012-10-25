#ifndef SETTINGSDATA_H
#define SETTINGSDATA_H

struct SettingsData
{
   bool turnBeep;
   bool foldBeep;
   bool checkBeep;
   bool showFolded;
   bool advisorMode;
   QString who;
   QDate firstRun;
   QStringList regKeys;
   int interval;
   bool visualAlert;
   QString lang;
   QString room;
};
Q_DECLARE_METATYPE(SettingsData)

QDataStream &operator<<(QDataStream &out, const SettingsData &myObj);
QDataStream &operator>>(QDataStream &in, SettingsData &myObj);

#endif

