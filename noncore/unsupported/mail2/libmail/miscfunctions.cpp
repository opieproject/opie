#include <qdatetime.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "miscfunctions.h"
#include "md5.h"

QString MiscFunctions::encodeQPrintable(const QString &src)
{
	// TODO: implent encodeQPrintable
	return src;
}

QString MiscFunctions::decodeQPrintable(const QString &src)
{
	QString out;

	for (unsigned int i = 0; i <= src.length(); i++) {
		if (src[i] == '=') {
			if (src[i+1] == "\n") {
				i += 1;
			} else {
				QString temp = QString("%1%2").arg(src[i+1]).arg(src[i+2]);
				int number = temp.toInt(0, 16);

				out += QChar(number);
				i += 2;
			}
		} else {
			out += src[i];
		}
	}
	return out;
}

QString MiscFunctions::encodeBase64(const QString &src)
{
	char *dataPtr = (char *) src.latin1();
	int len = src.length();
	int count = 0;
	QString temp = "";

	while (len > 0) {
		if (len < 3) {
			encodeBase64Base(dataPtr, &temp, len);
			len = 0;
		} else {
			encodeBase64Base(dataPtr, &temp, 3);
			len -= 3;
			dataPtr += 3;
			count += 4;
		}
		if (count > 72) {
			count = 0;
			temp += "\n";
		}
	}

	return temp;	
}

void MiscFunctions::encodeBase64Base(char *src, QString *dest, int len)
{
 	QString temp;
 	uchar c;
 	uchar bufOut[4];

	bufOut[0] = src[0];
	bufOut[0] >>= 2;

	bufOut[1] = src[0];
	bufOut[1] = bufOut[1] & (1 + 2);
	bufOut[1] <<= 4;
	if (len > 1) c = src[1];
	else c = 0;

	c = c & (16 + 32 + 64 + 128);
	c >>= 4;
	bufOut[1] = bufOut[1] | c;

	bufOut[2] = src[1];
	bufOut[2] = bufOut[2] & (1 + 2 + 4 + 8);
	bufOut[2] <<= 2;
	if (len > 2) c = src[2];
	else c = 0;

	c >>= 6;
	bufOut[2] = bufOut[2] | c;

	bufOut[3] = src[2];
	bufOut[3] = bufOut[3] & (1 + 2 + 4 + 8 + 16 + 32);

	if (len == 1) {
		bufOut[2] = 64;
		bufOut[3] = 64;
	}
	if (len == 2) {
		bufOut[3] = 64;
	}
	for (int x = 0; x < 4; x++) {
		if (bufOut[x] <= 25)
			bufOut[x] += (uint) 'A';
		else if (bufOut[x] >= 26 && bufOut[x] <= 51)
			bufOut[x] += (uint) 'a' - 26;
		else if (bufOut[x] >= 52 && bufOut[x] <= 61)
			bufOut[x] += (uint) '0' - 52;
		else if (bufOut[x] == 62)
			bufOut[x] = '+';
		else if (bufOut[x] == 63)
			bufOut[x] = '/';
		else if (bufOut[x] == 64)
			bufOut[x] = '=';

		dest->append(bufOut[x]);
	}
}

QString MiscFunctions::decodeBase64(const QString &src)
{
	char plain[4];
	char *destPtr;
	QByteArray buffer;
	uint bufCount = 0, pos = 0, decodedCount, x;
	
	buffer.resize(src.length() * 3 / 4);
	destPtr = buffer.data();
	
	while (pos < src.length()) {
		decodedCount = 4;
		x = 0;
		while ((x < 4) && (pos < src.length())) {
			plain[x] = src[pos].latin1();
			pos++;
			if (plain[x] == '\r' || plain[x] == '\n' || plain[x] == ' ')
				x--;
			x++;
		}
		if (x > 1) {
			decodedCount = decodeBase64Base(plain, destPtr);
			destPtr += decodedCount;
			bufCount += decodedCount;
		}
	}
	
	buffer.resize(bufCount);
	return QString(buffer);
}

int MiscFunctions::decodeBase64Base(char *src, char *bufOut)
{
	char c, z;
	char li[4];
	int processed;

	for (int x = 0; x < 4; x++) {
		c = src[x];
	
		if ( (int) c >= 'A' && (int) c <= 'Z')
			li[x] = (int) c - (int) 'A';
		if ( (int) c >= 'a' && (int) c <= 'z')
			li[x] = (int) c - (int) 'a' + 26;
		if ( (int) c >= '0' && (int) c <= '9')
			li[x] = (int) c - (int) '0' + 52;
		if (c == '+')
			li[x] = 62;
		if (c == '/')
			li[x] = 63;
	}
	
	processed = 1;
	bufOut[0] = (char) li[0] & (32+16+8+4+2+1);
	bufOut[0] <<= 2;
	z = li[1] >> 4;
	bufOut[0] = bufOut[0] | z;
		
	if (src[2] != '=') {
		bufOut[1] = (char) li[1] & (8+4+2+1);
		bufOut[1] <<= 4;
		z = li[2] >> 2;
		bufOut[1] = bufOut[1] | z;
		processed++;

		if (src[3] != '=') {
			bufOut[2] = (char) li[2] & (2+1);
			bufOut[2] <<= 6;
			z = li[3];
			bufOut[2] = bufOut[2] | z;
			processed++;
		}
	}
	return processed;
}

QString MiscFunctions::uniqueString()
{
	QString uniqueString = QDate::currentDate().toString();
	uniqueString	    += QTime::currentTime().toString();
	uniqueString	    += QString("%1").arg(rand());

	unsigned char md[16];

	MD5_CTX ctx;
	MD5_Init(&ctx);
	MD5_Update(&ctx, (unsigned char *)uniqueString.latin1(), uniqueString.length());
	MD5_Final(md, &ctx);

	char hash[16];
	for (unsigned int i = 0; i < sizeof(md); i++)
		sprintf(hash + 2 * i, "%02x", md[i]);

	return hash;
}

QString MiscFunctions::rfcDate()
{
	time_t t = time(NULL);
	tm *time = localtime(&t);
	QString pm, tzh, tzm, ths, tms, tss;

	time->tm_gmtoff < 0 ? pm = "-" : pm = "+";
	int h = abs(time->tm_gmtoff) / 3600;
	int m = (abs(time->tm_gmtoff) - h * 3600) / 60;
	h < 10 ? tzh = QString("0%1").arg(h) : tzh = QString("%1").arg(h);
	m < 10 ? tzm = QString("0%1").arg(m) : tzm = QString("%1").arg(m);

	int th = time->tm_hour;
	int tm = time->tm_min;
	int ts = time->tm_sec;
	th < 10 ? ths = QString("0%1").arg(th) : ths = QString("%1").arg(th);
	tm < 10 ? tms = QString("0%1").arg(tm) : tms = QString("%1").arg(tm);
	ts < 10 ? tss = QString("0%1").arg(ts) : tss = QString("%1").arg(ts);

 	QString month = QDate().monthName(time->tm_mon + 1);
	QString dayna = QDate().dayName(time->tm_wday);
	QString tzone = pm + tzh + tzm;

	return QString("%1, %2 %3 %4 %5:%6:%7 %8")
		.arg(dayna)
		.arg(time->tm_mday)
		.arg(month)
		.arg(time->tm_year + 1900)
		.arg(ths)
		.arg(tms)
		.arg(tss)
		.arg(tzone);
}

QString MiscFunctions::smtpAuthCramMd5(const QString &data, const QString &key)
{
	MD5_CTX context;
	unsigned char k_ipad[65];
	unsigned char k_opad[65];
	unsigned char tk[16];
	unsigned char digest[16];
	unsigned char *key_int = (unsigned char *)key.latin1();
	char hash[33];

	if (key.length() > 64) {
		MD5_CTX tctx;
		MD5_Init(&tctx);
		MD5_Update(&tctx, key_int, sizeof(key_int));
		MD5_Final(tk, &tctx);

		key_int = tk;
	}	

	bzero(k_ipad, sizeof k_ipad);
	bzero(k_opad, sizeof k_opad);
	bcopy(key_int, k_ipad, sizeof(key_int));
	bcopy(key_int, k_opad, sizeof(key_int));

	for (int i = 0; i < 64; i++) {
		k_ipad[i] ^= 0x36;
		k_opad[i] ^= 0x5c;
	}

	MD5_Init(&context);
	MD5_Update(&context, k_ipad, 64);
	MD5_Update(&context, (unsigned char *)data.latin1(), data.length());
	MD5_Final(digest, &context);

	MD5_Init(&context);
	MD5_Update(&context, k_opad, 64);
	MD5_Update(&context, digest, 16);
	MD5_Final(digest, &context);

	for (unsigned int i = 0; i < sizeof(digest); i++)
		sprintf (hash + 2 * i, "%02x", digest[i]);

	return hash;
}

