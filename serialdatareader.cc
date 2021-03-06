/*-
 * Copyright (c) 2012 Caoimhe Chaos <caoimhechaos@protonmail.com>,
 *                    Ancient Solutions. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions  of source code must retain  the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions  in   binary  form  must   reproduce  the  above
 *    copyright  notice, this  list  of conditions  and the  following
 *    disclaimer in the  documentation and/or other materials provided
 *    with the distribution.
 *
 * THIS  SOFTWARE IS  PROVIDED BY  ANCIENT SOLUTIONS  AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO,  THE IMPLIED WARRANTIES OF  MERCHANTABILITY AND FITNESS
 * FOR A  PARTICULAR PURPOSE  ARE DISCLAIMED.  IN  NO EVENT  SHALL THE
 * FOUNDATION  OR CONTRIBUTORS  BE  LIABLE FOR  ANY DIRECT,  INDIRECT,
 * INCIDENTAL,   SPECIAL,    EXEMPLARY,   OR   CONSEQUENTIAL   DAMAGES
 * (INCLUDING, BUT NOT LIMITED  TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE,  DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT  LIABILITY,  OR  TORT  (INCLUDING NEGLIGENCE  OR  OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <QtCore/QByteArray>
#include <QtCore/QFile>
#include <QtCore/QString>

#include <arpa/inet.h>

#include <exception>
#include <zlib.h>

#include "serialdatareader.h"

namespace serialdata
{
SerialDataReaderException::SerialDataReaderException(const QString& msg)
: msg_(msg)
{
}

SerialDataReaderException::SerialDataReaderException(const std::string& msg)
: msg_(msg.c_str())
{
}

SerialDataReaderException::~SerialDataReaderException() noexcept (true) {
}

QString
SerialDataReaderException::String() throw ()
{
	return msg_;
}

SerialDataReaderCorruptionException::SerialDataReaderCorruptionException(
		const std::string& msg)
: SerialDataReaderException(msg)
{
}

SerialDataReaderCorruptionException::~SerialDataReaderCorruptionException()
noexcept (true)
{
}

SerialDataReaderEndReachedException::SerialDataReaderEndReachedException()
: SerialDataReaderException(QString("End of data reached"))
{}

SerialDataReaderEndReachedException::~SerialDataReaderEndReachedException()
noexcept (true)
{}

SerialDataReader::SerialDataReader(QString file)
	throw (SerialDataReaderException)
: path_(file), handle_(file)
{
	if (!handle_.open(QIODevice::ReadOnly))
		throw new SerialDataReaderException("Unable to open " +
				file + ": " + handle_.errorString());
}

SerialDataReader::~SerialDataReader() noexcept (true)
{
	handle_.close();
}

QByteArray
SerialDataReader::ReadRecord(int64_t* offset)
{
	int64_t off = handle_.pos();
	QByteArray ra_data;
	uint32_t lenbuf = 0UL;
	uint32_t length = 0UL;
	uint32_t checksum = 0UL;
	qint64 rv;

	if ((rv = handle_.read(reinterpret_cast<char*>(&lenbuf), 4)) == 0)
		throw new SerialDataReaderEndReachedException();

	if (rv < 0)
		throw new SerialDataReaderException("Unable to read header: "
				+ handle_.errorString());

	length = ntohl(lenbuf);
	if (!length)
		throw new SerialDataReaderException(
				QString("Data has no length?"));

	if ((rv = handle_.read(reinterpret_cast<char*>(&lenbuf), 4)) == 0)
		throw new SerialDataReaderEndReachedException();

	if (rv < 0)
		throw new SerialDataReaderException("Unable to read checksum: "
				+ handle_.errorString());

	checksum = ntohl(lenbuf);

	ra_data = handle_.read(length);
	if (ra_data.length() != length)
		throw new SerialDataReaderException("Unable to read data: " +
				handle_.errorString());

	if (checksum)
	{
		uint32_t state = 0UL;

		crc32(state, (unsigned char*) ra_data.constData(),
			       	ra_data.length());

		if (state != checksum)
			throw new SerialDataReaderCorruptionException(
					std::string("Data corrupted (CRC32 "
						"mismatch)"));
	}

	if (offset)
		*offset = off;

	return ra_data;
}

QByteArray
SerialDataReader::ReadRecordAt(int64_t pos)
{
	if (!handle_.seek(pos))
		throw new SerialDataReaderException(QString(
					"Invalid position"));
	return ReadRecord();
}

}  // namespace serialdata
