#ifndef MISCFUNCTIONS_H
#define MISCFUNCTIONS_H

#include <qobject.h>

class MiscFunctions : public QObject
{
	Q_OBJECT

public:
	static QString encodeQPrintable(const QString &src);
	static QString decodeQPrintable(const QString &src);
	static QString encodeBase64(const QString &src);
	static QString decodeBase64(const QString &src);
	static QString uniqueString();
	static QString rfcDate();
	static QString smtpAuthCramMd5(const QString &data, const QString &key);

protected:
	MiscFunctions() : QObject() { }

	static void encodeBase64Base(char *src, QString *dest, int len);
	static int decodeBase64Base(char *src, char *bufOut);

};

#endif
