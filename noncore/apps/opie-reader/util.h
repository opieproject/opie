#ifndef __UTIL_H
#define __UTIL_H

#include <qstring.h>

QString filesize(unsigned long l);
QString percent(unsigned long pos, unsigned long len);
QString fmt(unsigned long pos, unsigned long len);
#endif
