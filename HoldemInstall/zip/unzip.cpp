#include "unzip.h"
#include "unzip_p.h"
#include "zipentry_p.h"

#include <QString>
#include <QStringList>
#include <QDir>
#include <QFile>
#include <QCoreApplication>

//! ������ ���������� ��������� (�������� ���������, �������� ���� ���������� �����)
#define UNZIP_LOCAL_HEADER_SIZE 26
//! ������ ������ �������� �������
#define UNZIP_CD_ENTRY_SIZE_NS 42
//! ������ ����������� ������ (�������� ���������)
#define UNZIP_DD_SIZE 12
//! ������ ��������� �������� ������� (������� ���������, �������� ���� ���������� �����)
#define UNZIP_EOCD_SIZE 22
//! ������ ������������� ���������
#define UNZIP_LOCAL_ENC_HEADER_SIZE 12

//! ��������� �������� ������ ������ �������� ������� (�������� ���������)
#define UNZIP_CD_OFF_VERSION 0
#define UNZIP_CD_OFF_GPFLAG 4
#define UNZIP_CD_OFF_CMETHOD 6
#define UNZIP_CD_OFF_MODT 8
#define UNZIP_CD_OFF_MODD 10
#define UNZIP_CD_OFF_CRC32 12
#define UNZIP_CD_OFF_CSIZE 16
#define UNZIP_CD_OFF_USIZE 20
#define UNZIP_CD_OFF_NAMELEN 24
#define UNZIP_CD_OFF_XLEN 26
#define UNZIP_CD_OFF_COMMLEN 28
#define UNZIP_CD_OFF_LHOFFSET 38

//! ��������� �������� ������ ������ ���������� ��������� (�������� ���������)
#define UNZIP_LH_OFF_VERSION 0
#define UNZIP_LH_OFF_GPFLAG 2
#define UNZIP_LH_OFF_CMETHOD 4
#define UNZIP_LH_OFF_MODT 6
#define UNZIP_LH_OFF_MODD 8
#define UNZIP_LH_OFF_CRC32 10
#define UNZIP_LH_OFF_CSIZE 14
#define UNZIP_LH_OFF_USIZE 18
#define UNZIP_LH_OFF_NAMELEN 22
#define UNZIP_LH_OFF_XLEN 24

//! ��������� �������� ������ ������ ����������� ������ (�������� ���������)
#define UNZIP_DD_OFF_CRC32 0
#define UNZIP_DD_OFF_CSIZE 4
#define UNZIP_DD_OFF_USIZE 8

//! ��������� �������� ������ ������ ��������� �������� ������� (�������� ���������)
#define UNZIP_EOCD_OFF_ENTRIES 6
#define UNZIP_EOCD_OFF_CDOFF 12
#define UNZIP_EOCD_OFF_COMMLEN 16

/*!
	������������ ������, ����������� ���� API.
	0x1B = 2.7 --> ������ ������������� ������ �� ������ 2.0 ������������ (0x14)
	������ �  2.1 �� 2.7 ����� �� �������������� ��-�� ����������� ������� ������
	������ ���� 2.7 ����� ����� ������������� ������ ���������
*/
#define UNZIP_VERSION 0x1B
//! ������ ������������� ������������� �� ���� ������
#define UNZIP_VERSION_STRICT 0x14

//! CRC32 ������������
#define CRC32(c, b) crcTable[((int)c^b) & 0xff] ^ (c >> 8)

//! ��������, ��� ��������� ���� ��� ��� ���� ��������.
#define UNZIP_CHECK_FOR_VALID_DATA \
	{\
		if (headers != 0)\
		{\
			ec = headers->size() != 0 ? UnZip::PartiallyCorrupted : UnZip::Corrupted;\
			break;\
		}\
		else\
		{\
			delete device;\
			device = 0;\
			ec = UnZip::Corrupted;\
			break;\
		}\
	}


/************************************************************************
 Public interface
*************************************************************************/

UnZip::UnZip()
{
	d = new UnzipPrivate;
}
//------------------------------------------------------------------------
UnZip::~UnZip()
{
	closeArchive();
	delete d;
}
//------------------------------------------------------------------------

bool UnZip::isOpen() const
{
	return d->device != 0;
}
//------------------------------------------------------------------------
UnZip::ErrorCode UnZip::openArchive(const QString& filename)
{
	QFile* file = new QFile(filename);

	if (!file->exists()) {
		delete file;
		return UnZip::FileNotFound;
	}

	if (!file->open(QIODevice::ReadOnly)) {
		delete file;
		return UnZip::OpenFailed;
	}

	return openArchive(file);
}

//------------------------------------------------------------------------
UnZip::ErrorCode UnZip::openArchive(QIODevice* device)
{
	if (device == 0)
	{
		return UnZip::InvalidDevice;
	}

	return d->openArchive(device);
}

//------------------------------------------------------------------------
void UnZip::closeArchive()
{
	d->closeArchive();
}
//------------------------------------------------------------------------
QString UnZip::archiveComment() const
{
	if (d->device == 0)
		return QString();
	return d->comment;
}

//------------------------------------------------------------------------

QString UnZip::formatError(UnZip::ErrorCode c) const
{
	switch (c)
	{
	case Ok: return QCoreApplication::translate("UnZip", "ZIP operation completed successfully."); break;
	case ZlibInit: return QCoreApplication::translate("UnZip", "Failed to initialize or load zlib library."); break;
	case ZlibError: return QCoreApplication::translate("UnZip", "zlib library error."); break;
	case OpenFailed: return QCoreApplication::translate("UnZip", "Unable to create or open file."); break;
	case PartiallyCorrupted: return QCoreApplication::translate("UnZip", "Partially corrupted archive. Some files might be extracted."); break;
	case Corrupted: return QCoreApplication::translate("UnZip", "Corrupted archive."); break;
	case WrongPassword: return QCoreApplication::translate("UnZip", "Wrong password."); break;
	case NoOpenArchive: return QCoreApplication::translate("UnZip", "No archive has been created yet."); break;
	case FileNotFound: return QCoreApplication::translate("UnZip", "File or directory does not exist."); break;
	case ReadFailed: return QCoreApplication::translate("UnZip", "File read error."); break;
	case WriteFailed: return QCoreApplication::translate("UnZip", "File write error."); break;
	case SeekFailed: return QCoreApplication::translate("UnZip", "File seek error."); break;
	case CreateDirFailed: return QCoreApplication::translate("UnZip", "Unable to create a directory."); break;
	case InvalidDevice: return QCoreApplication::translate("UnZip", "Invalid device."); break;
	case InvalidArchive: return QCoreApplication::translate("UnZip", "Invalid or incompatible zip archive."); break;
	case HeaderConsistencyError: return QCoreApplication::translate("UnZip", "Inconsistent headers. Archive might be corrupted."); break;
	default: ;
	}

	return QCoreApplication::translate("UnZip", "Unknown error.");
}

//------------------------------------------------------------------------
bool UnZip::contains(const QString& file) const
{
	if (d->headers == 0)
		return false;

	return d->headers->contains(file);
}

//------------------------------------------------------------------------
QStringList UnZip::fileList() const
{
	return d->headers == 0 ? QStringList() : d->headers->keys();
}

//------------------------------------------------------------------------

QList<UnZip::ZipEntry> UnZip::entryList() const
{
	QList<UnZip::ZipEntry> list;

	if (d->headers != 0)
	{
		for (QMap<QString,ZipEntryP*>::ConstIterator it = d->headers->constBegin(); it != d->headers->constEnd(); ++it)
		{
			const ZipEntryP* entry = it.value();
			Q_ASSERT(entry != 0);

			ZipEntry z;

			z.filename = it.key();
			if (!entry->comment.isEmpty())
				z.comment = entry->comment;
			z.compressedSize = entry->szComp;
			z.uncompressedSize = entry->szUncomp;
			z.crc32 = entry->crc;
			z.lastModified = d->convertDateTime(entry->modDate, entry->modTime);

			z.compression = entry->compMethod == 0 ? NoCompression : entry->compMethod == 8 ? Deflated : UnknownCompression;
			z.type = z.filename.endsWith("/") ? Directory : File;

			z.encrypted = entry->isEncrypted();

			list.append(z);
		}
	}

	return list;
}

//------------------------------------------------------------------------
UnZip::ErrorCode UnZip::extractAll(const QString& dirname, ExtractionOptions options)
{
	return extractAll(QDir(dirname), options);
}

//------------------------------------------------------------------------

UnZip::ErrorCode UnZip::extractAll(const QDir& dir, ExtractionOptions options)
{
   //��� �����������, ���� �� ��� �� ������� �����
	if (d->device == 0)
		return NoOpenArchive;

	if (d->headers == 0)
		return Ok;

	bool end = false;
	for (QMap<QString,ZipEntryP*>::Iterator itr = d->headers->begin(); itr != d->headers->end(); ++itr)
	{
		ZipEntryP* entry = itr.value();
		Q_ASSERT(entry != 0);

		if ((entry->isEncrypted()) && d->skipAllEncrypted)
			continue;

		switch (d->extractFile(itr.key(), *entry, dir, options))
		{
		case Corrupted:
			d->headers->erase(itr++);
			if (itr == d->headers->end())
				end = true;
			break;
		case CreateDirFailed:
			break;
		case Skip:
			break;
		case SkipAll:
			d->skipAllEncrypted = true;
			break;
		default:
			;
		}

		if (end)
			break;
	}

	return Ok;
}

//------------------------------------------------------------------------

UnZip::ErrorCode UnZip::extractFile(const QString& filename, const QString& dirname, ExtractionOptions options)
{
	return extractFile(filename, QDir(dirname), options);
}

//------------------------------------------------------------------------

UnZip::ErrorCode UnZip::extractFile(const QString& filename, const QDir& dir, ExtractionOptions options)
{
	QMap<QString,ZipEntryP*>::Iterator itr = d->headers->find(filename);
	if (itr != d->headers->end())
	{
		ZipEntryP* entry = itr.value();
		Q_ASSERT(entry != 0);
		return d->extractFile(itr.key(), *entry, dir, options);
	}

	return FileNotFound;
}

//------------------------------------------------------------------------

UnZip::ErrorCode UnZip::extractFile(const QString& filename, QIODevice* dev, ExtractionOptions options)
{
	if (dev == 0)
		return InvalidDevice;

	QMap<QString,ZipEntryP*>::Iterator itr = d->headers->find(filename);
	if (itr != d->headers->end()) {
		ZipEntryP* entry = itr.value();
		Q_ASSERT(entry != 0);
		return d->extractFile(itr.key(), *entry, dev, options);
	}

	return FileNotFound;
}

//------------------------------------------------------------------------

UnZip::ErrorCode UnZip::extractFiles(const QStringList& filenames, const QString& dirname, ExtractionOptions options)
{
	QDir dir(dirname);
	ErrorCode ec;

	for (QStringList::ConstIterator itr = filenames.constBegin(); itr != filenames.constEnd(); ++itr)
	{
		ec = extractFile(*itr, dir, options);
		if (ec == FileNotFound)
			continue;
		if (ec != Ok)
			return ec;
	}

	return Ok;
}

//------------------------------------------------------------------------

UnZip::ErrorCode UnZip::extractFiles(const QStringList& filenames, const QDir& dir, ExtractionOptions options)
{
	ErrorCode ec;

	for (QStringList::ConstIterator itr = filenames.constBegin(); itr != filenames.constEnd(); ++itr)
	{
		ec = extractFile(*itr, dir, options);
		if (ec == FileNotFound)
			continue;
		if (ec != Ok)
			return ec;
	}

	return Ok;
}

//------------------------------------------------------------------------

void UnZip::setPassword(const QString& pwd)
{
	d->password = pwd;
}

//------------------------------------------------------------------------

UnZip::ZipEntry::ZipEntry()
{
	compressedSize = uncompressedSize = crc32 = 0;
	compression = NoCompression;
	type = File;
	encrypted = false;
}


/************************************************************************
 Private interface
*************************************************************************/

//! ����������� ����������� ����������
//------------------------------------------------------------------------
UnzipPrivate::UnzipPrivate()
{
	skipAllEncrypted = false;
	headers = 0;
	device = 0;

	uBuffer = (unsigned char*) buffer1;
	crcTable = (quint32*) get_crc_table();

	cdOffset = eocdOffset = 0;
	cdEntryCount = 0;
	unsupportedEntryCount = 0;
}


//! ����������� Zip �����
//------------------------------------------------------------------------
UnZip::ErrorCode UnzipPrivate::openArchive(QIODevice* dev)
{
	Q_ASSERT(dev != 0);

	if (device != 0)
		closeArchive();

	device = dev;

	if (!(device->isOpen() || device->open(QIODevice::ReadOnly)))
	{
		delete device;
		device = 0;

		return UnZip::OpenFailed;
	}

	UnZip::ErrorCode ec;

	ec = seekToCentralDirectory();
	if (ec != UnZip::Ok)
	{
		closeArchive();
		return ec;
	}

	if (cdEntryCount == 0)
	{
		return UnZip::Ok;
	}

	bool continueParsing = true;

	while (continueParsing)
	{
		if (device->read(buffer1, 4) != 4)
			UNZIP_CHECK_FOR_VALID_DATA

		if (! (buffer1[0] == 'P' && buffer1[1] == 'K' && buffer1[2] == 0x01  && buffer1[3] == 0x02) )
			break;

		if ( (ec = parseCentralDirectoryRecord()) != UnZip::Ok )
			break;
	}

	if (ec != UnZip::Ok)
		closeArchive();

	return ec;
}

/*
   ����������� ������ ���������� �������� � ������ �������� ������������ 
   ������ ������� ���� ���������������� �����,
   ����������� �� ���������� ���������� � ������� ������� 

	local file header signature     4 bytes  (0x04034b50)
	version needed to extract       2 bytes
	general purpose bit flag        2 bytes
	compression method              2 bytes
	last mod file time              2 bytes
	last mod file date              2 bytes
	crc-32                          4 bytes
	compressed size                 4 bytes
	uncompressed size               4 bytes
	file name length                2 bytes
	extra field length              2 bytes

	file name (variable size)
	extra field (variable size)
*/
//------------------------------------------------------------------------
UnZip::ErrorCode UnzipPrivate::parseLocalHeaderRecord(const QString& path, ZipEntryP& entry)
{
	if (!device->seek(entry.lhOffset))
		return UnZip::SeekFailed;

	// ���� ���������
	if (device->read(buffer1, 4) != 4)
		return UnZip::ReadFailed;

	if ((buffer1[0] != 'P') || (buffer1[1] != 'K') || (buffer1[2] != 0x03) || (buffer1[3] != 0x04))
		return UnZip::InvalidArchive;

	if (device->read(buffer1, UNZIP_LOCAL_HEADER_SIZE) != UNZIP_LOCAL_HEADER_SIZE)
		return UnZip::ReadFailed;

	/*
      ��������� ������ �������� ����������� ���-����
		"��� 3: ���� ���� ��� ����������, �� ���� crc-32, ������ ������
		� �������� ������ ��������������� � ���� � ��������� ���������.
      ���������� �������� �������� � ����������� ������ � ������� ����� �� ������� �������."
	*/
	bool hasDataDescriptor = entry.hasDataDescriptor();

	bool checkFailed = false;

	if (!checkFailed)
		checkFailed = entry.compMethod != getUShort(uBuffer, UNZIP_LH_OFF_CMETHOD);
	if (!checkFailed)
		checkFailed = entry.gpFlag[0] != uBuffer[UNZIP_LH_OFF_GPFLAG];
	if (!checkFailed)
		checkFailed = entry.gpFlag[1] != uBuffer[UNZIP_LH_OFF_GPFLAG + 1];
	if (!checkFailed)
		checkFailed = entry.modTime[0] != uBuffer[UNZIP_LH_OFF_MODT];
	if (!checkFailed)
		checkFailed = entry.modTime[1] != uBuffer[UNZIP_LH_OFF_MODT + 1];
	if (!checkFailed)
		checkFailed = entry.modDate[0] != uBuffer[UNZIP_LH_OFF_MODD];
	if (!checkFailed)
		checkFailed = entry.modDate[1] != uBuffer[UNZIP_LH_OFF_MODD + 1];
	if (!hasDataDescriptor)
	{
		if (!checkFailed)
			checkFailed = entry.crc != getULong(uBuffer, UNZIP_LH_OFF_CRC32);
		if (!checkFailed)
			checkFailed = entry.szComp != getULong(uBuffer, UNZIP_LH_OFF_CSIZE);
		if (!checkFailed)
			checkFailed = entry.szUncomp != getULong(uBuffer, UNZIP_LH_OFF_USIZE);
	}

	if (checkFailed)
		return UnZip::HeaderConsistencyError;

	// ��������� ��� �����
	quint16 szName = getUShort(uBuffer, UNZIP_LH_OFF_NAMELEN);
	if (szName == 0)
		return UnZip::HeaderConsistencyError;

	if (device->read(buffer2, szName) != szName)
		return UnZip::ReadFailed;

	QString filename = QString::fromAscii(buffer2, szName);
	if (filename != path)
	{
		return UnZip::HeaderConsistencyError;
	}

	// ���������� �������������� ����
	quint16 szExtra = getUShort(uBuffer, UNZIP_LH_OFF_XLEN);
	if (szExtra != 0)
	{
		if (!device->seek(device->pos() + szExtra))
			return UnZip::SeekFailed;
	}

	entry.dataOffset = device->pos();

	if (hasDataDescriptor)
	{
		/*
			���������� ������ ����� ��� ������������ ���������: PK\7\8
         �� �������� ���������� ������ ������ ��������� �� ������, ������������� 
         � ������ �������� �������.
		*/
		if (!device->seek(device->pos() + entry.szComp))
			return UnZip::SeekFailed;

		// ������ 4 ����� � ��������� �������� �� ��� ���������� ����������� ������
		if (device->read(buffer2, 4) != 4)
			return UnZip::ReadFailed;

		bool hasSignature = buffer2[0] == 'P' && buffer2[1] == 'K' && buffer2[2] == 0x07 && buffer2[3] == 0x08;
		if (hasSignature)
		{
			if (device->read(buffer2, UNZIP_DD_SIZE) != UNZIP_DD_SIZE)
				return UnZip::ReadFailed;
		}
		else
		{
			if (device->read(buffer2 + 4, UNZIP_DD_SIZE - 4) != UNZIP_DD_SIZE - 4)
				return UnZip::ReadFailed;
		}

		// crc, ������ ������, �������� ������
		if (
			entry.crc != getULong((unsigned char*)buffer2, UNZIP_DD_OFF_CRC32) ||
			entry.szComp != getULong((unsigned char*)buffer2, UNZIP_DD_OFF_CSIZE) ||
			entry.szUncomp != getULong((unsigned char*)buffer2, UNZIP_DD_OFF_USIZE)
			)
			return UnZip::HeaderConsistencyError;
	}

	return UnZip::Ok;
}
//------------------------------------------------------------------------
/*! �������� ����� ������ ������ �������� �������.

	��������� � ����� ����� � ������� ������ "End Of Central Directory"
	��������� PK\5\6.

	end of central dir signature    4 bytes  (0x06054b50)
	number of this disk             2 bytes
	number of the disk with the
	start of the central directory  2 bytes
	total number of entries in the
	central directory on this disk  2 bytes
	total number of entries in
	the central directory           2 bytes
	size of the central directory   4 bytes
	offset of start of central
	directory with respect to
	the starting disk number        4 bytes
	.ZIP file comment length        2 bytes
	--- SIZE UNTIL HERE: UNZIP_EOCD_SIZE ---
	.ZIP file comment       (variable size)
*/
UnZip::ErrorCode UnzipPrivate::seekToCentralDirectory()
{
	qint64 length = device->size();
	qint64 offset = length - UNZIP_EOCD_SIZE;

	if (length < UNZIP_EOCD_SIZE)
		return UnZip::InvalidArchive;

	if (!device->seek( offset ))
		return UnZip::SeekFailed;

	if (device->read(buffer1, UNZIP_EOCD_SIZE) != UNZIP_EOCD_SIZE)
		return UnZip::ReadFailed;

	bool eocdFound = (buffer1[0] == 'P' && buffer1[1] == 'K' && buffer1[2] == 0x05 && buffer1[3] == 0x06);

	if (eocdFound)
	{
		// Zip ���� �� ����� �����������
		eocdOffset = offset;
	}
	else
	{
		qint64 read;
		char* p = 0;

		offset -= UNZIP_EOCD_SIZE;

		if (offset <= 0)
			return UnZip::InvalidArchive;

		if (!device->seek( offset ))
			return UnZip::SeekFailed;

		while ((read = device->read(buffer1, UNZIP_EOCD_SIZE)) >= 0)
		{
			if ( (p = strstr(buffer1, "PK\5\6")) != 0)
			{
				//����� ������ ������ EOCD �� ����� ��������� ��� ���������
				device->seek( offset + (p - buffer1) );
				eocdFound = true;
				eocdOffset = offset + (p - buffer1);

				// ������ EOCD ������
				if (device->read(buffer1, UNZIP_EOCD_SIZE) != UNZIP_EOCD_SIZE)
					return UnZip::ReadFailed;

				break;
			}

			offset -= 1 /*UNZIP_EOCD_SIZE*/;
			if (offset <= 0)
				return UnZip::InvalidArchive;

			if (!device->seek( offset ))
				return UnZip::SeekFailed;
		}
	}

	if (!eocdFound)
		return UnZip::InvalidArchive;

	// ������ EOCD ����� ���������� CD ��������
	offset = getULong((const unsigned char*)buffer1, UNZIP_EOCD_OFF_CDOFF + 4);

	cdOffset = offset;

	cdEntryCount = getUShort((const unsigned char*)buffer1, UNZIP_EOCD_OFF_ENTRIES + 4);

	quint16 commentLength = getUShort((const unsigned char*)buffer1, UNZIP_EOCD_OFF_COMMLEN + 4);
	if (commentLength != 0)
	{
		QByteArray c = device->read(commentLength);
		if (c.count() != commentLength)
			return UnZip::ReadFailed;

		comment = c;
	}

	// �������� �� ������ ������ �������� �������
	if (!device->seek( cdOffset ))
		return UnZip::SeekFailed;

	return UnZip::Ok;
}

/*!
	����������� ������ �������� �������.

	Central Directory record structure:

	[file header 1]
	.
	.
	.
	[file header n]
	[digital signature] // PKZip 6.2 or later only

	File header:

	central file header signature   4 bytes  (0x02014b50)
	version made by                 2 bytes
	version needed to extract       2 bytes
	general purpose bit flag        2 bytes
	compression method              2 bytes
	last mod file time              2 bytes
	last mod file date              2 bytes
	crc-32                          4 bytes
	compressed size                 4 bytes
	uncompressed size               4 bytes
	file name length                2 bytes
	extra field length              2 bytes
	file comment length             2 bytes
	disk number start               2 bytes
	internal file attributes        2 bytes
	external file attributes        4 bytes
	relative offset of local header 4 bytes

	file name (variable size)
	extra field (variable size)
	file comment (variable size)
*/
//-----------------------------------------------------------------------
UnZip::ErrorCode UnzipPrivate::parseCentralDirectoryRecord()
{
	// ������ CD ������
	if (device->read(buffer1, UNZIP_CD_ENTRY_SIZE_NS) != UNZIP_CD_ENTRY_SIZE_NS)
		return UnZip::ReadFailed;

	bool skipEntry = false;

	// �������� ��� ����������, ��� �� ����� ���������� ������������� ���������
	quint16 compMethod = getUShort(uBuffer, UNZIP_CD_OFF_CMETHOD);

   // �������� ���������� ������ ���� �����, ��� �� ����� ���������� ������ ������� 
   //���� �����
	quint16 szName = getUShort(uBuffer, UNZIP_CD_OFF_NAMELEN);
	quint16 szExtra = getUShort(uBuffer, UNZIP_CD_OFF_XLEN);
	quint16 szComment = getUShort(uBuffer, UNZIP_CD_OFF_COMMLEN);

	quint32 skipLength = szName + szExtra + szComment;

	UnZip::ErrorCode ec = UnZip::Ok;

	if ((compMethod != 0) && (compMethod != 8))
	{
		skipEntry = true;
	}

   // ������ ��������� ����� ���� ���������� ���� ������ ������ ��� UNZIP_VERSION
	if (!skipEntry && buffer1[UNZIP_CD_OFF_VERSION] > UNZIP_VERSION)
	{
		skipEntry = true;
	}

	if (!skipEntry && szName == 0)
	{
		skipEntry = true;
	}

	if (!skipEntry && device->read(buffer2, szName) != szName)
	{
		ec = UnZip::ReadFailed;
		skipEntry = true;
	}

	if (skipEntry)
	{
		if (ec == UnZip::Ok)
		{
			if (!device->seek( device->pos() + skipLength ))
				ec = UnZip::SeekFailed;

			unsupportedEntryCount++;
		}

		return ec;
	}

	QString filename = QString::fromAscii(buffer2, szName);

	ZipEntryP* h = new ZipEntryP;
	h->compMethod = compMethod;

	h->gpFlag[0] = buffer1[UNZIP_CD_OFF_GPFLAG];
	h->gpFlag[1] = buffer1[UNZIP_CD_OFF_GPFLAG + 1];

	h->modTime[0] = buffer1[UNZIP_CD_OFF_MODT];
	h->modTime[1] = buffer1[UNZIP_CD_OFF_MODT + 1];

	h->modDate[0] = buffer1[UNZIP_CD_OFF_MODD];
	h->modDate[1] = buffer1[UNZIP_CD_OFF_MODD + 1];

	h->crc = getULong(uBuffer, UNZIP_CD_OFF_CRC32);
	h->szComp = getULong(uBuffer, UNZIP_CD_OFF_CSIZE);
	h->szUncomp = getULong(uBuffer, UNZIP_CD_OFF_USIZE);

	// ���������� �������������� ����
	if (szExtra != 0)
	{
		if (!device->seek( device->pos() + szExtra ))
		{
			delete h;
			return UnZip::SeekFailed;
		}
	}

	// ������ ���� �����������
	if (szComment != 0)
	{
		if (device->read(buffer2, szComment) != szComment)
		{
			delete h;
			return UnZip::ReadFailed;
		}

		h->comment = QString::fromAscii(buffer2, szComment);
	}

	h->lhOffset = getULong(uBuffer, UNZIP_CD_OFF_LHOFFSET);

	if (headers == 0)
		headers = new QMap<QString, ZipEntryP*>();
	headers->insert(filename, h);

	return UnZip::Ok;
}
//------------------------------------------------------------------------
//! ��������� ����� � ������������� ���������� ������.
void UnzipPrivate::closeArchive()
{
	if (device == 0)
		return;

	skipAllEncrypted = false;

	if (headers != 0)
	{
		qDeleteAll(*headers);
		delete headers;
		headers = 0;
	}

	delete device; device = 0;

	cdOffset = eocdOffset = 0;
	cdEntryCount = 0;
	unsupportedEntryCount = 0;

	comment.clear();
}
//------------------------------------------------------------------------
UnZip::ErrorCode UnzipPrivate::extractFile(const QString& path, ZipEntryP& entry, const QDir& dir, UnZip::ExtractionOptions options)
{
	QString name(path);
	QString dirname;
	QString directory;

	int pos = name.lastIndexOf('/');

	// ��� ������ �������
	if (pos == name.length() - 1)
	{
		if (options.testFlag(UnZip::SkipPaths))
			return UnZip::Ok;

		directory = QString("%1/%2").arg(dir.absolutePath()).arg(QDir::cleanPath(name));
		if (!createDirectory(directory))
		{
			return UnZip::CreateDirFailed;
		}

		return UnZip::Ok;
	}

	// ������� ���� �� ������
	if (pos > 0)
	{
		// �������� ����� ��������
		dirname = name.left(pos);
		if (options.testFlag(UnZip::SkipPaths))
		{
			directory = dir.absolutePath();
		}
		else
		{
			directory = QString("%1/%2").arg(dir.absolutePath()).arg(QDir::cleanPath(dirname));
			if (!createDirectory(directory))
			{
				return UnZip::CreateDirFailed;
			}
		}
		name = name.right(name.length() - pos - 1);
	} else directory = dir.absolutePath();

	name = QString("%1/%2").arg(directory).arg(name);

	QFile outFile(name);

	if (!outFile.open(QIODevice::WriteOnly))
	{
		return UnZip::OpenFailed;
	}

	UnZip::ErrorCode ec = extractFile(path, entry, &outFile, options);

	outFile.close();

	if (ec != UnZip::Ok)
	{
		outFile.remove();
	}

	return ec;
}


UnZip::ErrorCode UnzipPrivate::extractFile(const QString& path, ZipEntryP& entry, QIODevice* dev, UnZip::ExtractionOptions options)
{
	Q_UNUSED(options);
	Q_ASSERT(dev != 0);

	if (!entry.lhEntryChecked)
	{
		UnZip::ErrorCode ec = parseLocalHeaderRecord(path, entry);
		entry.lhEntryChecked = true;

		if (ec != UnZip::Ok)
			return ec;
	}

	if (!device->seek(entry.dataOffset))
		return UnZip::SeekFailed;

	// ����� ����������
	quint32 keys[3];

	if (entry.isEncrypted())
	{
		UnZip::ErrorCode e = testPassword(keys, path, entry);
		if (e != UnZip::Ok)
		{
			return e;
		}// ������ ��������� ����������
		entry.szComp -= UNZIP_LOCAL_ENC_HEADER_SIZE; 
	}

	if (entry.szComp == 0)
	{
		if (entry.crc != 0)
			return UnZip::Corrupted;

		return UnZip::Ok;
	}

	uInt rep = entry.szComp / UNZIP_READ_BUFFER;
	uInt rem = entry.szComp % UNZIP_READ_BUFFER;
	uInt cur = 0;

	// ������� ������
	qint64 read;
	quint64 tot = 0;

	quint32 myCRC = crc32(0L, Z_NULL, 0);

	if (entry.compMethod == 0)
	{
		while ( (read = device->read(buffer1, cur < rep ? UNZIP_READ_BUFFER : rem)) > 0 )
		{
			if (entry.isEncrypted())
				decryptBytes(keys, buffer1, read);

			myCRC = crc32(myCRC, uBuffer, read);

			if (dev->write(buffer1, read) != read)
				return UnZip::WriteFailed;

			cur++;
			tot += read;

			if (tot == entry.szComp)
				break;
		}

		if (read < 0)
			return UnZip::ReadFailed;
	}
	else if (entry.compMethod == 8)
	{
		/* Allocate inflate state */
		z_stream zstr;
		zstr.zalloc = Z_NULL;
		zstr.zfree = Z_NULL;
		zstr.opaque = Z_NULL;
		zstr.next_in = Z_NULL;
		zstr.avail_in = 0;

		int zret;

		// ���������� inflateInit2 � ������������� windowBits ����� �������� ����� ������������
		if ( (zret = inflateInit2_(&zstr, -MAX_WBITS, ZLIB_VERSION, sizeof(z_stream))) != Z_OK )
			return UnZip::ZlibError;

		int szDecomp;

		// ����������� �� ����� �����
		do
		{
			read = device->read(buffer1, cur < rep ? UNZIP_READ_BUFFER : rem);
			if (read == 0)
				break;
			if (read < 0)
			{
				(void)inflateEnd(&zstr);
				return UnZip::ReadFailed;
			}

			if (entry.isEncrypted())
				decryptBytes(keys, buffer1, read);

			cur++;
			tot += read;

			zstr.avail_in = (uInt) read;
			zstr.next_in = (Bytef*) buffer1;


			do {
				zstr.avail_out = UNZIP_READ_BUFFER;
				zstr.next_out = (Bytef*) buffer2;;

				zret = inflate(&zstr, Z_NO_FLUSH);

				switch (zret) {
					case Z_NEED_DICT:
					case Z_DATA_ERROR:
					case Z_MEM_ERROR:
						inflateEnd(&zstr);
						return UnZip::WriteFailed;
					default:
						;
				}

				szDecomp = UNZIP_READ_BUFFER - zstr.avail_out;
				if (dev->write(buffer2, szDecomp) != szDecomp)
				{
					inflateEnd(&zstr);
					return UnZip::ZlibError;
				}

				myCRC = crc32(myCRC, (const Bytef*) buffer2, szDecomp);

			} while (zstr.avail_out == 0);

		}
		while (zret != Z_STREAM_END);

		inflateEnd(&zstr);
	}

	if (myCRC != entry.crc)
		return UnZip::Corrupted;

	return UnZip::Ok;
}
//------------------------------------------------------------------------
//! ������� ����� ������� � ��� ����������� ������������ ��������.
bool UnzipPrivate::createDirectory(const QString& path)
{
	QDir d(path);
	if (!d.exists())
	{
		int sep = path.lastIndexOf("/");
		if (sep <= 0) return true;

		if (!createDirectory(path.left(sep)))
			return false;

		if (!d.mkdir(path))
		{
			return false;
		}
	}

	return true;
}
//------------------------------------------------------------------------
/*!
	������ quint32 (4 �����) ������� �� ������ ������� � �������� ���������
*/
quint32 UnzipPrivate::getULong(const unsigned char* data, quint32 offset) const
{
	quint32 res = (quint32) data[offset];
	res |= (((quint32)data[offset+1]) << 8);
	res |= (((quint32)data[offset+2]) << 16);
	res |= (((quint32)data[offset+3]) << 24);

	return res;
}
//------------------------------------------------------------------------
/*!
   ������ quint64 (8 ����) ������� �� ������ ������� � �������� ���������
*/
quint64 UnzipPrivate::getULLong(const unsigned char* data, quint32 offset) const
{
	quint64 res = (quint64) data[offset];
	res |= (((quint64)data[offset+1]) << 8);
	res |= (((quint64)data[offset+2]) << 16);
	res |= (((quint64)data[offset+3]) << 24);
	res |= (((quint64)data[offset+1]) << 32);
	res |= (((quint64)data[offset+2]) << 40);
	res |= (((quint64)data[offset+3]) << 48);
	res |= (((quint64)data[offset+3]) << 56);

	return res;
}
//------------------------------------------------------------------------
/*!
   ������ quint16 (2 �����) ������� �� ������ ������� � �������� ���������
*/
quint16 UnzipPrivate::getUShort(const unsigned char* data, quint32 offset) const
{
	return (quint16) data[offset] | (((quint16)data[offset+1]) << 8);
}
//------------------------------------------------------------------------
/*!
   ���������� ��������� ���� � ��������������� ������������������
 */
int UnzipPrivate::decryptByte(quint32 key2) const
{
	quint16 temp = ((quint16)(key2) & 0xffff) | 2;
	return (int)(((temp * (temp ^ 1)) >> 8) & 0xff);
}
//------------------------------------------------------------------------
/*!
   ��������� ������������ �����
 */
void UnzipPrivate::updateKeys(quint32* keys, int c) const
{
	keys[0] = CRC32(keys[0], c);
	keys[1] += keys[0] & 0xff;
	keys[1] = keys[1] * 134775813L + 1;
	keys[2] = CRC32(keys[2], ((int)keys[1]) >> 24);
}
//------------------------------------------------------------------------
/*!
	�������������� ������������ ����� � ��������� ��������� �������� ��������� ������
 */
void UnzipPrivate::initKeys(const QString& pwd, quint32* keys) const
{
	keys[0] = 305419896L;
	keys[1] = 591751049L;
	keys[2] = 878082192L;

	QByteArray pwdBytes = pwd.toAscii();
	int sz = pwdBytes.size();
	const char* ascii = pwdBytes.data();

	for (int i=0; i<sz; ++i)
		updateKeys(keys, (int)ascii[i]);
}
//------------------------------------------------------------------------
/*!
	�������� ����������� ������ ��� ���������� �����.
*/
UnZip::ErrorCode UnzipPrivate::testPassword(quint32* keys, const QString& file, const ZipEntryP& header)
{
	Q_UNUSED(file);

	// ������ ������������ �����
	if (device->read(buffer1, 12) != 12)
		return UnZip::Corrupted;

	initKeys(password, keys);
	if (testKeys(header, keys))
		return UnZip::Ok;

	return UnZip::Skip;
}
//------------------------------------------------------------------------
/*!
   ��������� ����� ������ �� ������������ ���������.
*/
bool UnzipPrivate::testKeys(const ZipEntryP& header, quint32* keys)
{
	char lastByte;

	// ������������ ������������� ���������
	for (int i=0; i<11; ++i)
		updateKeys(keys, lastByte = buffer1[i] ^ decryptByte(keys[2]));
	updateKeys(keys, lastByte = buffer1[11] ^ decryptByte(keys[2]));

	char c = ((header.gpFlag[0] & 0x08) == 8) ? header.modTime[1] : header.crc >> 24;

	return (lastByte == c);
}
//------------------------------------------------------------------------
/*!
   �������������� ������ 
*/
void UnzipPrivate::decryptBytes(quint32* keys, char* buffer, qint64 read)
{
	for (int i=0; i<(int)read; ++i)
		updateKeys(keys, buffer[i] ^= decryptByte(keys[2]));
}
//------------------------------------------------------------------------
/*!
   ������������ ���� � ����� �� ZIP ������� � ������ QDateTime
*/
QDateTime UnzipPrivate::convertDateTime(const unsigned char date[2], const unsigned char time[2]) const
{
	QDateTime dt;

	// ������� PKZip low-byte to high-byte �������

	// ����: 7 ��� = ��� � 1980, 4 ��� = �����, 5 ��� = ����
	quint16 year = (date[1] >> 1) & 127;
	quint16 month = ((date[1] << 3) & 14) | ((date[0] >> 5) & 7);
	quint16 day = date[0] & 31;

	// �����: 5 ��� - ���, 6 ��� - ������, 5 ��� - �������
	quint16 hour = (time[1] >> 3) & 31;
	quint16 minutes = ((time[1] << 3) & 56) | ((time[0] >> 5) & 7);
	quint16 seconds = (time[0] & 31) * 2;

	dt.setDate(QDate(1980 + year, month, day));
	dt.setTime(QTime(hour, minutes, seconds));
	return dt;
}
