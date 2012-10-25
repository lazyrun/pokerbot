/*! \file unzip.h
    \brief Закрытый интерфейс для разархивирования
*/

#ifndef UNZIP_P__H
#define UNZIP_P__H

#include "unzip.h"
#include "zipentry_p.h"

#include <QtGlobal>


//! Буфер чтения 256K
#define UNZIP_READ_BUFFER (256*1024)

/*! \class UnzipPrivate
   \brief Класс закрытого интерфейса для разархивирования PKZip 2.0
*/
class UnzipPrivate
{
public:
	//! Конструктор внутреннего интерфейса
   UnzipPrivate();

	//! Пароль
	QString password;
   //! Пропустить все зашифрованные
	bool skipAllEncrypted;
   //! Заголовки
	QMap<QString,ZipEntryP*>* headers;
   //! Устройство
	QIODevice* device;
   //! Буфер 1
	char buffer1[UNZIP_READ_BUFFER];
   //! Буфер 2
	char buffer2[UNZIP_READ_BUFFER];
   
   //! Буфер
	unsigned char* uBuffer;
   //! crc таблица
	const quint32* crcTable;

	//! Central Directory (CD) offset
	quint32 cdOffset;
	//! End of Central Directory (EOCD) offset
	quint32 eocdOffset;

	//! Количество записей в Central Directory
	quint16 cdEntryCount;

	//! Количество обнаруженных записей которые были пропущены т.к. имею несовместимый формат
	quint16 unsupportedEntryCount;
   //! Комментарий
	QString comment;

	//! Анализирует Zip архив
   /*!
   \param device Устройство чтения
   \return Код ошибки
   */
   UnZip::ErrorCode openArchive(QIODevice* device);

	//! Пытается найти начало записи главного перечня.
   /*!
   \return Код ошибки
   */
   UnZip::ErrorCode seekToCentralDirectory();
   //! Анализирует запись главного перечня.
   /*!
   \return Код ошибки
   */
	UnZip::ErrorCode parseCentralDirectoryRecord();

	/*!
   Анализирует запись локального загловка и делает проверку соответствия 
   записи которая была проанализирована ранее,
   основываясь на информации хранящейся в главном перечне 
   \param path Путь к архиву
   \param entry Архивная запись
   \return Код ошибки
   */
   UnZip::ErrorCode parseLocalHeaderRecord(const QString& path, ZipEntryP& entry);

	//! Закрывает архив и перезагружает внутренний статус.
   void closeArchive();

	//! Извлечь файл
   /*!
   \param path Путь к архиву
   \param entry Архивная запись
   \param dir Каталог назначения
   \param options Опции извлечения
   \return Код ошибки
   */
   UnZip::ErrorCode extractFile(const QString& path, ZipEntryP& entry, const QDir& dir, UnZip::ExtractionOptions options);
	//! Извлечь файл
   /*!
   \param path Путь к архиву
   \param entry Архивная запись
   \param device Устройство назначения
   \param options Опции извлечения
   \return Код ошибки
   */
   UnZip::ErrorCode extractFile(const QString& path, ZipEntryP& entry, QIODevice* device, UnZip::ExtractionOptions options);

	//! Тест пароля
   /*!
   \param keys Шифровальные ключи
   \param file Архивный файл
   \param header Архивный заголовок
   */
   UnZip::ErrorCode testPassword(quint32* keys, const QString& file, const ZipEntryP& header);
	
   //! Тест ключей
   /*!
   \param header Заголовок
   \param keys Ключи
   \return Результат теста
   */
   bool testKeys(const ZipEntryP& header, quint32* keys);

	bool createDirectory(const QString& path);

	inline void decryptBytes(quint32* keys, char* buffer, qint64 read);

	inline quint32 getULong(const unsigned char* data, quint32 offset) const;
	inline quint64 getULLong(const unsigned char* data, quint32 offset) const;
	inline quint16 getUShort(const unsigned char* data, quint32 offset) const;
	inline int decryptByte(quint32 key2) const;
	inline void updateKeys(quint32* keys, int c) const;
	inline void initKeys(const QString& pwd, quint32* keys) const;

	inline QDateTime convertDateTime(const unsigned char date[2], const unsigned char time[2]) const;
};

#endif // UNZIP_P__H
