#ifndef _KEYCFG_ERROR_HANDLER_H_
#define _KEYCFG_ERROR_HANDLER_H_

#include <qstring.h>
#include <qxml.h>

class KeycfgErrorHandler : public QXmlErrorHandler
{
public:
	KeycfgErrorHandler(){}
	virtual ~KeycfgErrorHandler(){}

	bool warning(const QXmlParseException& exception);
	bool error(const QXmlParseException& exception);
	bool fatalError(const QXmlParseException& exception);
	QString errorString();
private:
	void setErrorString(const QString& header,
		const QXmlParseException& exception);
	QString m_errstr;
};

#endif /* _KEYCFG_ERROR_HANDLER_H_ */
