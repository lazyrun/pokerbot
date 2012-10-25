/*! \file DBManager.cpp
    \brief Менеджер БД
*/

#include "DBManager.h"


DBManager::DBManager(const QString & dbName)
: dbName_(dbName)
{
   connectDB();
}

DBManager::~DBManager()
{
   db_.close();
}

bool DBManager::connectDB()
{
   db_is_good_ = false;

   QString driver = "QSQLITE";
   QString connect = dbName_;
   QString user;
   QString passwd;
   
   if (!QSqlDatabase::isDriverAvailable(driver))
   {
      QString err_msg = tr("Database driver not installed");
      lastError_ = QString("\"%1\" : %2").arg(driver)
         .arg(err_msg);

      return false;
   }
   else
   {
      db_ = QSqlDatabase::addDatabase(driver, dbName_);
      db_.setDatabaseName(connect);
      
      if (user.length())
      {
         db_.setUserName(user);
      }
      if (passwd.length())
      {
         db_.setPassword(passwd);
      }
      if (!db_.open())
      {
         QString err_msg = tr("Can't establish connection to");
         lastError_ = QString("%1 : %2").arg(err_msg)
            .arg(connect);
         return false;
      }
      db_is_good_ = (CheckDBStruct() == 1);
   }
   return db_is_good_;
}

bool DBManager::CheckDBStruct()
{
   QStringList tables = db_.tables();
   //проверка таблицы Статистика
   if (tables.contains("CARDS"))
   {
      //проверить структуру таблицы - все ли нужные столбцы присутствуют
      QSqlRecord rec = db_.record("CARDS");
      if (!(rec.contains("ID")          &&
            rec.contains("HASH")   &&
            rec.contains("NOMINAL")    &&
            rec.contains("SUIT")       ))
      {
         //таблица есть но ее структура не соответствует текущему положению дел
         //расширить таблицу недостающими столбцами
         bool resAlter = false;   
         if (!rec.contains("HASH"))
            resAlter = alterTable("CARDS", "HASH", "TEXT");
         if (!rec.contains("NOMINAL"))
            resAlter = alterTable("CARDS", "NOMINAL", "INTEGER");
         if (!rec.contains("SUIT"))
            resAlter = alterTable("CARDS", "SUIT", "INTEGER");

         if (!resAlter)
         {
            QString err_msg = tr("The table %1 is obsolete").arg("CARDS");
            lastError_ = QString("%1").arg(err_msg);
            return false;
         }
      }
   }
   else//таблицы не существует (БД новая)
   {
      QString create_sql = QString("CREATE TABLE CARDS (ID INTEGER PRIMARY KEY AUTOINCREMENT, HASH TEXT, NOMINAL INTEGER, SUIT INTEGER)");
      db_.exec(create_sql);
      if (db_.lastError().type() != QSqlError::NoError)
      {
         QString err_msg = tr("Can't create table %1").arg("CARDS");
         lastError_ = QString("%1 : %2").arg(err_msg).arg(QSqlDatabase::database().lastError().text());
         return false;
      }
   }

   QString create_idx = QString("CREATE INDEX IDX_CARDS ON CARDS(HASH)");
   db_.exec(create_idx);

   return true;
}

bool DBManager::alterTable(const QString & tableName,
                           const QString & columnName,
                           const QString & columnType)
{
   QString alter_sql = QString("ALTER TABLE %1 ADD %2 %3").arg(tableName).arg(columnName).arg(columnType);
      
   db_.exec(alter_sql);
   QString se = db_.lastError().text();
   return se.trimmed().isEmpty();
}

