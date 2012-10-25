/*! \file unzip.h
    \brief ���������������� 
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
   \brief ����� ��� ���������������� PKZip 2.0 zip
   ������������� � ���������� �������� �� �������������, �.�. ��� ����� ����� ����������� ������ ������.
*/
class UnZip
{
public:
   //! ���� ������
	enum ErrorCode
	{
		Ok, //!< ��� ������
		ZlibInit,//!< ������ ������������� zlib
		ZlibError,//!< ������ ��� ������ zlib
		OpenFailed,//!< ���������� ������� ��� ������� ����������
		PartiallyCorrupted,//!< ����� �������� ��������� - ��������� ����� ����� ���� ���������
		Corrupted,//!< ����� ���������
		WrongPassword,//!< �������� ������
		NoOpenArchive,//!< ����� ��� �� ��� ������
		FileNotFound,//!< ������������� ���� �� ������
		ReadFailed,//!< ������ ������ ����� �� ������
		WriteFailed,//!< ������ ������ ����� � �����
		SeekFailed,//!< ������ ��� ��������
		CreateDirFailed,//!< ������ ��� �������� ��������
		InvalidDevice,//!< ���� �������� ������ ���������� ��� ��������
		InvalidArchive,//!< �������� ��� ���������������� �����
		HeaderConsistencyError,//!< ������ � ��������� �� ��������� � ���, ��� ��������� ���������� � ������. ����� ����� ���� ���������.  

		Skip,//!< ����������
      SkipAll//!<  ���������� ���
	};

	//! ����� ���������� ������
   enum ExtractionOption
	{
		//! ��������� ���� (�� ���������)
		ExtractPaths = 0x0001,
		//! ������������ ���� � ��������� ��� ����� � ��������� �������
		SkipPaths = 0x0002
	};
	Q_DECLARE_FLAGS(ExtractionOptions, ExtractionOption)

	//! ����� ������
   enum CompressionMethod
	{
		NoCompression,//!< ��� ������
      Deflated,//!< ������
      UnknownCompression//!< ����������� ������
	};

	//! ��� �����
   enum FileType
	{
		File,//!< ����
      Directory//!< �������
	};

	//! ������ � zip-������
   typedef struct ZipEntry
	{
		//! �����������
      ZipEntry();
      //! ��� �����
		QString filename;
      //! �����������
		QString comment;
      //! ������ ������
		quint32 compressedSize;
      //! �������� ������
		quint32 uncompressedSize;
      //! ����� CRC32
		quint32 crc32;
      //! ���� �����������
		QDateTime lastModified;
      //! ����� ������
		CompressionMethod compression;
      //! ��� �����
		FileType type;
      //! ������� ���������������
		bool encrypted;
	};
   /*! �����������
      ������� ����� ������������.
   */
   UnZip();
   /*! ����������� ����������
   ��������� �������� ����� � ����������� �������.
   */
	virtual ~UnZip();
   //! ����� ������
	bool isOpen() const;
   //! ������� �����
   /*!
   \param filename ��� ����� ������
   \return ��� ������
   */
	ErrorCode openArchive(const QString& filename);
   //! ������� �����
   /*!
   \param device ��� ����������
   \return ��� ������
   */
   ErrorCode openArchive(QIODevice* device);
   //! ������� �����
	void closeArchive();
   //! �������� ����������� ������
   /*!
   \return �����������
   */
	QString archiveComment() const;

	//! ���������� ������������ �������� ������
   /*!
   \param c ��� ������
   \return �������� ������
   */
   QString formatError(UnZip::ErrorCode c) const;
   //! �������� �� ����� ������ ����
   /*!
   \param file �������� �����
   \return �������� ��� ���
   */
	bool contains(const QString& file) const;

	//! �������� ������ ������ � ������
   /*!
   \return ������ ������
   */
   QStringList fileList() const;
   //! ������ zip ������� � ������
   /*!
   \return ������ �������
   */
	QList<ZipEntry> entryList() const;

	//! ������� �� �� ������
   /*!
   \param dirname ���� ��� ����������
   \param options ����� ����������
   \return ��� ������ - ��������� ���������������� 
   */
   ErrorCode extractAll(const QString& dirname, ExtractionOptions options = ExtractPaths);
	//! ������� �� �� ������
   /*!
   \param dir ���� ��� ����������
   \param options ����� ����������
   \return ��� ������ - ��������� ���������������� 
   */
   ErrorCode extractAll(const QDir& dir, ExtractionOptions options = ExtractPaths);
	//! ������� ���� �� ������
   /*!
   \param filename ��� �����
   \param dirname ���� ��� ����������
   \param options ����� ����������
   \return ��� ������ - ��������� ���������������� 
   */
	ErrorCode extractFile(const QString& filename, const QString& dirname, ExtractionOptions options = ExtractPaths);
	//! ������� ���� �� ������
   /*!
   \param filename ��� �����
   \param dir ���� ��� ����������
   \param options ����� ����������
   \return ��� ������ - ��������� ���������������� 
   */
   ErrorCode extractFile(const QString& filename, const QDir& dir, ExtractionOptions options = ExtractPaths);
	//! ������� ���� �� ������
   /*!
   \param filename ��� �����
   \param device ���������� ��� ����������
   \param options ����� ����������
   \return ��� ������ - ��������� ���������������� 
   */
   ErrorCode extractFile(const QString& filename, QIODevice* device, ExtractionOptions options = ExtractPaths);
	//! ������� ����� �� ������
   /*!
   \param filenames ������ ������
   \param dirname ���� ��� ����������
   \param options ����� ����������
   \return ��� ������ - ��������� ���������������� 
   */
	ErrorCode extractFiles(const QStringList& filenames, const QString& dirname, ExtractionOptions options = ExtractPaths);
	//! ������� ����� �� ������
   /*!
   \param filenames ������ ������
   \param dir ���� ��� ����������
   \param options ����� ����������
   \return ��� ������ - ��������� ���������������� 
   */	
   ErrorCode extractFiles(const QStringList& filenames, const QDir& dir, ExtractionOptions options = ExtractPaths);

   //! ���������� ������ �� �����
   /*!
   \param pwd ������
   */
	void setPassword(const QString& pwd);

private:
   //! ���������� ��������� ��� ������ � �������
	UnzipPrivate* d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(UnZip::ExtractionOptions)

#endif // UNZIP__H
