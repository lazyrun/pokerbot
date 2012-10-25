/*! \file unzip.h
    \brief Разархивирование 
*/

#ifndef UNZIP__H
#define UNZIP__H

#include <QtGlobal>
#include <QMap>
#include <QDateTime>

#if defined(__WIN32__) || defined(WIN32)
   #include <zlib/zlib.h>
#else
   #include <zlib.h>
#endif

class UnzipPrivate;
class QIODevice;
class QFile;
class QDir;
class QStringList;
class QString;

/*! \class UnZip
   \brief Класс для разархивирования PKZip 2.0 zip
   Совместимость с последними версиями не гарантируется, т.к. они могут иметь неизвестные методы сжатия.
*/
class UnZip
{
public:
   //! Коды ошибок
	enum ErrorCode
	{
		Ok, //!< Нет ошибки
		ZlibInit,//!< Ошибка инициализации zlib
		ZlibError,//!< Ошибка при работе zlib
		OpenFailed,//!< Невозможно создать или открыть устройство
		PartiallyCorrupted,//!< Архив частично поврежден - некоторые файлы могут быть извлечены
		Corrupted,//!< Архив поврежден
		WrongPassword,//!< Неверный пароль
		NoOpenArchive,//!< Архив еще не был открыт
		FileNotFound,//!< Запрашиваемый файл не найден
		ReadFailed,//!< Ошибка чтения файла из архива
		WriteFailed,//!< Ошибка записи файла в архив
		SeekFailed,//!< Ошибка при смещении
		CreateDirFailed,//!< Ошибка при создании каталога
		InvalidDevice,//!< Было передано пустое устройство как параметр
		InvalidArchive,//!< Неверный или неподдерживаемый архив
		HeaderConsistencyError,//!< Записи в заголовке не совпадают с тем, что находится фактически в архиве. Архив может быть поврежден.  

		Skip,//!< Пропустить
      SkipAll//!<  Пропустить все
	};

	//! Опции извлечения файлов
   enum ExtractionOption
	{
		//! Извлекать пути (по умолчанию)
		ExtractPaths = 0x0001,
		//! Игнорировать пути и извлекать все файлы в некоторый каталог
		SkipPaths = 0x0002
	};
	Q_DECLARE_FLAGS(ExtractionOptions, ExtractionOption)

	//! Метод сжатия
   enum CompressionMethod
	{
		NoCompression,//!< Без сжатия
      Deflated,//!< Сжатие
      UnknownCompression//!< Неизвестное сжатие
	};

	//! Тип файла
   enum FileType
	{
		File,//!< Файл
      Directory//!< Каталог
	};

	//! Запись о zip-архиве
   typedef struct ZipEntry
	{
		//! Конструктор
      ZipEntry();
      //! Имя файла
		QString filename;
      //! Комментарий
		QString comment;
      //! Сжатый размер
		quint32 compressedSize;
      //! Несжатый размер
		quint32 uncompressedSize;
      //! Сумма CRC32
		quint32 crc32;
      //! Дата модификации
		QDateTime lastModified;
      //! Метод сжатия
		CompressionMethod compression;
      //! Тип файла
		FileType type;
      //! Признак зашифрованности
		bool encrypted;
	};
   /*! Конструктор
      Создает новый декомпрессор.
   */
   UnZip();
   /*! Виртуальный деструктор
   Закрывает открытый архив и освобождает ресурсы.
   */
	virtual ~UnZip();
   //! Архив открыт
	bool isOpen() const;
   //! Открыть архив
   /*!
   \param filename Имя файла архива
   \return Код ошибки
   */
	ErrorCode openArchive(const QString& filename);
   //! Открыть архив
   /*!
   \param device Имя устройства
   \return Код ошибки
   */
   ErrorCode openArchive(QIODevice* device);
   //! Закрыть архив
	void closeArchive();
   //! Получить комментарий архива
   /*!
   \return Комментарий
   */
	QString archiveComment() const;

	//! Возвращает переведенное описание ошибки
   /*!
   \param c Код ошибки
   \return Описание ошибки
   */
   QString formatError(UnZip::ErrorCode c) const;
   //! Содержит ли архив данный файл
   /*!
   \param file Название файла
   \return Содержит или нет
   */
	bool contains(const QString& file) const;

	//! Получить список файлов в архиве
   /*!
   \return Список файлов
   */
   QStringList fileList() const;
   //! Список zip записей в архиве
   /*!
   \return Список записей
   */
	QList<ZipEntry> entryList() const;

	//! Извлечь всё из архива
   /*!
   \param dirname Путь для извлечения
   \param options Опции извлечения
   \return Код ошибки - результат разархивирования 
   */
   ErrorCode extractAll(const QString& dirname, ExtractionOptions options = ExtractPaths);
	//! Извлечь всё из архива
   /*!
   \param dir Путь для извлечения
   \param options Опции извлечения
   \return Код ошибки - результат разархивирования 
   */
   ErrorCode extractAll(const QDir& dir, ExtractionOptions options = ExtractPaths);
	//! Извлечь файл из архива
   /*!
   \param filename Имя файла
   \param dirname Путь для извлечения
   \param options Опции извлечения
   \return Код ошибки - результат разархивирования 
   */
	ErrorCode extractFile(const QString& filename, const QString& dirname, ExtractionOptions options = ExtractPaths);
	//! Извлечь файл из архива
   /*!
   \param filename Имя файла
   \param dir Путь для извлечения
   \param options Опции извлечения
   \return Код ошибки - результат разархивирования 
   */
   ErrorCode extractFile(const QString& filename, const QDir& dir, ExtractionOptions options = ExtractPaths);
	//! Извлечь файл из архива
   /*!
   \param filename Имя файла
   \param device Устройство для извлечения
   \param options Опции извлечения
   \return Код ошибки - результат разархивирования 
   */
   ErrorCode extractFile(const QString& filename, QIODevice* device, ExtractionOptions options = ExtractPaths);
	//! Извлечь файлы из архива
   /*!
   \param filenames Список файлов
   \param dirname Путь для извлечения
   \param options Опции извлечения
   \return Код ошибки - результат разархивирования 
   */
	ErrorCode extractFiles(const QStringList& filenames, const QString& dirname, ExtractionOptions options = ExtractPaths);
	//! Извлечь файлы из архива
   /*!
   \param filenames Список файлов
   \param dir Путь для извлечения
   \param options Опции извлечения
   \return Код ошибки - результат разархивирования 
   */	
   ErrorCode extractFiles(const QStringList& filenames, const QDir& dir, ExtractionOptions options = ExtractPaths);

   //! Установить пароль на архив
   /*!
   \param pwd Пароль
   */
	void setPassword(const QString& pwd);

private:
   //! Внутренний интерфейс для работы с архивом
	UnzipPrivate* d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(UnZip::ExtractionOptions)

#endif // UNZIP__H
