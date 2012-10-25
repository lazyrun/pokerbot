/*! \file DBManager.h
    \brief Менеджер БД
    \author Kiselev Kirill (kiselev@papillon.ru)
*/
#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QtSql>
/*! \class DBManager
   \brief Класс главного окна
*/
class DBManager : public QObject
{
   Q_OBJECT
public:
   //! Конструктор
   DBManager(const QString & dbName = "holdem.db");
   //! Деструктор
   ~DBManager();
   //! Успешно ли соединение с БД
   /*!
   \return Успешность
   */
   inline bool isGood() const { return db_is_good_; }
   //! Описание последней ошибки
   /*!
   \return Описание ошибки
   */
   inline QString lastError() const { return lastError_; }
   //! Вернуть ссылку на БД
   /*!
   \return Ссылка на БД
   */
   QSqlDatabase & database() { return db_; }
protected:
   QString dbName_;
   //! Установить соединение с БД
   /*!
   \return Результат соединения
   */
   bool connectDB();
   //! Проверить структуру БД
   /*!
   \return Результат проверки
   */
   bool CheckDBStruct();
   //! Расширить таблицу
   /*!
   \param tableName Имя таблицы
   \param columnName Имя столбца
   \param columnType Тип столбца
   \return Результат выполнения
   */
   bool alterTable(const QString & tableName,
                   const QString & columnName,
                   const QString & columnType);
   //! Признак корректности БД
   bool db_is_good_;
   //! Описание последней ошибки
   QString lastError_;
   //! БД
   QSqlDatabase db_;
};

#endif
