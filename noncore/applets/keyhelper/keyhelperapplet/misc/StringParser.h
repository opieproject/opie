#ifndef _STRING_PARSER_H_
#define _STRING_PARSER_H_

#include <qstringlist.h>

class StringParser
{
public:
	static QStringList split(const QChar& sep, const QString& str,
		bool allowEmptyEntries=FALSE);
	static QString join(const QChar& sep, const QStringList& list);
};

#endif /* _STRING_PARSER_H_ */
