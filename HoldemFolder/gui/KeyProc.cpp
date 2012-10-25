#include "SettingsData.h"
#include "KeyProc.h"


AcadProc::AcadProc()
: AbstractProc()
{
   connect(this, SIGNAL(check(const QString &, const QString &)), 
      this, SLOT(checkAgain(const QString &, const QString &)),
      Qt::QueuedConnection);
}

void AcadProc::checkKey(const QString & name, const QString & key)
{
   emit check(name, key);
}

QString AcadProc::hash(const QString & name)
{
   return name.toUpper().trimmed();
}

//qqqqqqqqqqqq qqqqqqqqqqqqqqq qqqqqqqqqqqqqqq
//RSP102bea2dd9ec4de3fb478e1c5727170c222
//1
//c4ca4238a0b923820dcc509a6f75849b
//ACDc4ca4238a0b923820dcc509a6f75849b1111111111
void AcadProc::checkAgain(const QString & name, const QString & key)
{
   QCryptographicHash hashed(QCryptographicHash::Md5);
   QString hn = hash(name);
   hashed.addData(hn.toAscii());
   hn = hashed.result().toHex();
   if (QString::compare(hn, key.mid(3, 32), Qt::CaseInsensitive) == 0)  
   {
      emit isOk(name, key);
   }
   else
   {
      emit notOk();
   }
}

void AcadProc::write(const QString & name, const QString & key)
{
   //имя и ключ шифруются и записываются в реестр
   QString regKey = QDir::current().dirName();
   QSettings settings(regKey, "Config");

   static quint32 cipher = 073; //шифр
   QVariant sett = settings.value("Settings");
   SettingsData data = qvariant_cast<SettingsData>(sett);
   QString k = key.mid(0, 3) + key.mid(35);
   QByteArray arr(k.toUtf8());	
   for(int i = 0; i < arr.size(); i++)		
      arr[i] = arr[i] ^ cipher;
   k = QString::fromAscii(arr.toBase64());

   data.regKeys.append(k);
   data.who = name;
   settings.setValue("Settings", QVariant::fromValue(data));
}

