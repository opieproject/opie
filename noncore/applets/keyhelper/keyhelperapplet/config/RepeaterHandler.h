#ifndef _REPEATER_HANDLER_H_
#define _REPEATER_HANDLER_H_

#include <qstring.h>
#include <qvaluelist.h>
#include <qxml.h>
#include "KeyRepeater.h"
#include "KeyNames.h"
#include "KHUtil.h"

class RepeaterHandler : public QXmlDefaultHandler
{
public:
	void dispose(QXmlReader* parser, QXmlContentHandler* parent);

	void setKeyRepeater(KeyRepeater* rep)
	{
		m_pRepeater = rep;
	}

	bool startElement(const QString& namespaceURI,
		const QString& localName,
		const QString& qName,
		const QXmlAttributes& atts);
	bool endElement(const QString& namespaceURI,
		const QString& localName,
		const QString& qName);
private:
	QXmlContentHandler* m_parent;
	QXmlReader* m_parser;

	KeyRepeater* m_pRepeater;

	int m_code;
	bool m_enable;

};

#endif /* _REPEATER_HANDLER_H_ */
