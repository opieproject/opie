#ifndef __UTIL_H
#define __UTIL_H

#include <qstring.h>
#include <qdir.h>
#include <stdlib.h>

QString filesize(unsigned long l);
QString percent(unsigned long pos, unsigned long len);
QString fmt(unsigned long pos, unsigned long len);

/**
 * class with utility inline function(s)
 * (in contrast to global functions they become available by just
 * including util.h)
 */
class QTReaderUtil
{
  public:

   /**
	 * searches for a specific direcory inside OpieReader installation
	 *
	 * @param subdir    name of sub directory inside installation directory which is needed
	 * @return full path name including that sub directory
	 *
	 * search order is (on all platforms):
	 * - OPIEDIR
	 * - QTDIR
	 * - READERDIR
	 */
	static QString getPluginPath( const char *subdir = "codecs/" )
	{
		QString dirname;
		dirname = getenv("OPIEDIR");
		dirname += "/plugins/reader/";
		dirname += subdir;
		if (QDir(dirname).exists())
			return dirname;

		dirname = getenv("QTDIR");
		dirname += "/plugins/reader/";
		dirname += subdir;
		if (QDir(dirname).exists())
			return dirname;

		dirname = getenv("READERDIR");
		dirname += "/";
		dirname += subdir;
		if (QDir(dirname).exists())
			return dirname;

		return "";	 
	}
}; 

#endif
