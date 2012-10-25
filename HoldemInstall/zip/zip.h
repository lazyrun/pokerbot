
#ifndef ZIP__H
#define ZIP__H

#include <QtGlobal>
#include <QMap>

#if defined(__WIN32__) || defined(WIN32)
   #include <zlib/zlib.h>
#else
   #include <zlib.h>
#endif

class ZipPrivate;

class QIODevice;
class QFile;
class QDir;
class QStringList;
class QString;


class Zip
{
public:
	enum ErrorCode
	{
		Ok,
		ZlibInit,
		ZlibError,
		FileExists,
		OpenFailed,
		NoOpenArchive,
		FileNotFound,
		ReadFailed,
		WriteFailed,
		SeekFailed
	};

	enum CompressionLevel
	{
		Store,
		Deflate1 = 1, Deflate2, Deflate3, Deflate4,
		Deflate5, Deflate6, Deflate7, Deflate8, Deflate9,
		AutoCPU, AutoMIME, AutoFull
	};

	enum CompressionOption
	{
		//! Does not preserve absolute paths in the zip file when adding a file/directory (default)
		RelativePaths = 0x0001,
		//! Preserve absolute paths
		AbsolutePaths = 0x0002,
		//! Do not store paths. All the files are put in the (evtl. user defined) root of the zip file
		IgnorePaths = 0x0004
	};
	Q_DECLARE_FLAGS(CompressionOptions, CompressionOption)

	Zip();
	virtual ~Zip();

	bool isOpen() const;

	void setPassword(const QString& pwd);
	void clearPassword();
	QString password() const;

	ErrorCode createArchive(const QString& file, bool overwrite = true);
	ErrorCode createArchive(QIODevice* device);

	QString archiveComment() const;
	void setArchiveComment(const QString& comment);

	ErrorCode addDirectoryContents(const QString& path, CompressionLevel level = AutoFull);
	ErrorCode addDirectoryContents(const QString& path, const QString& root, CompressionLevel level = AutoFull);

	ErrorCode addDirectory(const QString& path, CompressionOptions options = RelativePaths, CompressionLevel level = AutoFull);
	ErrorCode addDirectory(const QString& path, const QString& root, CompressionLevel level = AutoFull);
	ErrorCode addDirectory(const QString& path, const QString& root, CompressionOptions options = RelativePaths, CompressionLevel level = AutoFull);

	ErrorCode closeArchive();

	QString formatError(ErrorCode c) const;

private:
	ZipPrivate* d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Zip::CompressionOptions)

#endif // ZIP__H
