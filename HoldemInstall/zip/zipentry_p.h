#ifndef ZIPENTRY_P__H
#define ZIPENTRY_P__H

#include <QtGlobal>
#include <QString>

class ZipEntryP
{
public:
	ZipEntryP()
	{
		lhOffset = 0;
		dataOffset = 0;
		gpFlag[0] = gpFlag[1] = 0;
		compMethod = 0;
		modTime[0] = modTime[1] = 0;
		modDate[0] = modDate[1] = 0;
		crc = 0;
		szComp = szUncomp = 0;
		lhEntryChecked = false;
	}

	quint32 lhOffset;			// Offset of the local header record for this entry
	quint32 dataOffset;			// Offset of the file data for this entry
	unsigned char gpFlag[2];	// General purpose flag
	quint16 compMethod;			// Compression method
	unsigned char modTime[2];	// Last modified time
	unsigned char modDate[2];	// Last modified date
	quint32 crc;				// CRC32
	quint32 szComp;				// Compressed file size
	quint32 szUncomp;			// Uncompressed file size
	QString comment;			// File comment

	bool lhEntryChecked;		// Is true if the local header record for this entry has been parsed

	inline bool isEncrypted() const { return gpFlag[0] & 0x01; }
	inline bool hasDataDescriptor() const { return gpFlag[0] & 0x08; }
};

#endif // OSDAB_ZIPENTRY_P__H
