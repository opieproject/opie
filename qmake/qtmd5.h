#ifndef QTMD5_H
#define QTMD5_H
#include <qstring.h>
#include <qcstring.h>

void qtMD5(const QByteArray &src, unsigned char *digest);
QString qtMD5(const QByteArray &src);

#endif


