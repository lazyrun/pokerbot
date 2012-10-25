/*! \file unzip.h
    \brief �������� ��������� ��� ����������������
*/

#ifndef UNZIP_P__H
#define UNZIP_P__H

#include "unzip.h"
#include "zipentry_p.h"

#include <QtGlobal>


//! ����� ������ 256K
#define UNZIP_READ_BUFFER (256*1024)

/*! \class UnzipPrivate
   \brief ����� ��������� ���������� ��� ���������������� PKZip 2.0
*/
class UnzipPrivate
{
public:
	//! ����������� ����������� ����������
   UnzipPrivate();

	//! ������
	QString password;
   //! ���������� ��� �������������
	bool skipAllEncrypted;
   //! ���������
	QMap<QString,ZipEntryP*>* headers;
   //! ����������
	QIODevice* device;
   //! ����� 1
	char buffer1[UNZIP_READ_BUFFER];
   //! ����� 2
	char buffer2[UNZIP_READ_BUFFER];
   
   //! �����
	unsigned char* uBuffer;
   //! crc �������
	const quint32* crcTable;

	//! Central Directory (CD) offset
	quint32 cdOffset;
	//! End of Central Directory (EOCD) offset
	quint32 eocdOffset;

	//! ���������� ������� � Central Directory
	quint16 cdEntryCount;

	//! ���������� ������������ ������� ������� ���� ��������� �.�. ���� ������������� ������
	quint16 unsupportedEntryCount;
   //! �����������
	QString comment;

	//! ����������� Zip �����
   /*!
   \param device ���������� ������
   \return ��� ������
   */
   UnZip::ErrorCode openArchive(QIODevice* device);

	//! �������� ����� ������ ������ �������� �������.
   /*!
   \return ��� ������
   */
   UnZip::ErrorCode seekToCentralDirectory();
   //! ����������� ������ �������� �������.
   /*!
   \return ��� ������
   */
	UnZip::ErrorCode parseCentralDirectoryRecord();

	/*!
   ����������� ������ ���������� �������� � ������ �������� ������������ 
   ������ ������� ���� ���������������� �����,
   ����������� �� ���������� ���������� � ������� ������� 
   \param path ���� � ������
   \param entry �������� ������
   \return ��� ������
   */
   UnZip::ErrorCode parseLocalHeaderRecord(const QString& path, ZipEntryP& entry);

	//! ��������� ����� � ������������� ���������� ������.
   void closeArchive();

	//! ������� ����
   /*!
   \param path ���� � ������
   \param entry �������� ������
   \param dir ������� ����������
   \param options ����� ����������
   \return ��� ������
   */
   UnZip::ErrorCode extractFile(const QString& path, ZipEntryP& entry, const QDir& dir, UnZip::ExtractionOptions options);
	//! ������� ����
   /*!
   \param path ���� � ������
   \param entry �������� ������
   \param device ���������� ����������
   \param options ����� ����������
   \return ��� ������
   */
   UnZip::ErrorCode extractFile(const QString& path, ZipEntryP& entry, QIODevice* device, UnZip::ExtractionOptions options);

	//! ���� ������
   /*!
   \param keys ������������ �����
   \param file �������� ����
   \param header �������� ���������
   */
   UnZip::ErrorCode testPassword(quint32* keys, const QString& file, const ZipEntryP& header);
	
   //! ���� ������
   /*!
   \param header ���������
   \param keys �����
   \return ��������� �����
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
