#ifndef _KHUTIL_H_
#define _KHUTIL_H_

#include <qstring.h>
#include <qstringlist.h>

class KHUtil
{
public:
	static int hex2int(const QString& hexstr, bool* ok=NULL);
	static const QStringList parseArgs(const QString& arguments);
	static const QString currentApp();
};

#endif /* _KHUTIL_H_ */
