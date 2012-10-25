/*! \file DBManager.h
    \brief �������� ��
    \author Kiselev Kirill (kiselev@papillon.ru)
*/
#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QtSql>
/*! \class DBManager
   \brief ����� �������� ����
*/
class DBManager : public QObject
{
   Q_OBJECT
public:
   //! �����������
   DBManager(const QString & dbName = "holdem.db");
   //! ����������
   ~DBManager();
   //! ������� �� ���������� � ��
   /*!
   \return ����������
   */
   inline bool isGood() const { return db_is_good_; }
   //! �������� ��������� ������
   /*!
   \return �������� ������
   */
   inline QString lastError() const { return lastError_; }
   //! ������� ������ �� ��
   /*!
   \return ������ �� ��
   */
   QSqlDatabase & database() { return db_; }
protected:
   QString dbName_;
   //! ���������� ���������� � ��
   /*!
   \return ��������� ����������
   */
   bool connectDB();
   //! ��������� ��������� ��
   /*!
   \return ��������� ��������
   */
   bool CheckDBStruct();
   //! ��������� �������
   /*!
   \param tableName ��� �������
   \param columnName ��� �������
   \param columnType ��� �������
   \return ��������� ����������
   */
   bool alterTable(const QString & tableName,
                   const QString & columnName,
                   const QString & columnType);
   //! ������� ������������ ��
   bool db_is_good_;
   //! �������� ��������� ������
   QString lastError_;
   //! ��
   QSqlDatabase db_;
};

#endif
