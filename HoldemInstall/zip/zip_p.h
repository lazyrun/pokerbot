
#ifndef ZIP_P__H
#define ZIP_P__H

#include "zip.h"
#include "zipentry_p.h"

#include <QtGlobal>
#include <QFileInfo>

/*!
	zLib authors suggest using larger buffers (128K or 256K) for (de)compression (especially for inflate())
	we use a 256K buffer here - if you want to use this code on a pre-iceage mainframe please change it ;)
*/
#define ZIP_READ_BUFFER (256*1024)

class ZipPrivate
{
public:
	ZipPrivate();
	virtual ~ZipPrivate();

	QMap<QString,ZipEntryP*>* headers;

	QIODevice* device;

	char buffer1[ZIP_READ_BUFFER];
	char buffer2[ZIP_READ_BUFFER];

	unsigned char* uBuffer;

	const quint32* crcTable;

	QString comment;
	QString password;

	Zip::ErrorCode createArchive(QIODevice* device);
	Zip::ErrorCode closeArchive();
	void reset();

	bool zLibInit();

	Zip::ErrorCode createEntry(const QFileInfo& file, const QString& root, Zip::CompressionLevel level);
	Zip::CompressionLevel detectCompressionByMime(const QString& ext);

	inline void encryptBytes(quint32* keys, char* buffer, qint64 read);

	inline void setULong(quint32 v, char* buffer, unsigned int offset);
	inline void updateKeys(quint32* keys, int c) const;
	inline void initKeys(quint32* keys) const;
	inline int decryptByte(quint32 key2) const;

	inline QString extractRoot(const QString& p);
};

#endif // ZIP_P__H
