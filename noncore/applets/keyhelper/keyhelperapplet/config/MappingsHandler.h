#ifndef _MAPPINGS_HANDLER_H_
#define _MAPPINGS_HANDLER_H_

#include <qxml.h>
#include <qstring.h>
#include <qmap.h>
#include "KeyModifiers.h"
#include "KeyMappings.h"
#include "KeyNames.h"
#include "KHUtil.h"

class MappingsHandler : public QXmlDefaultHandler
{
public:
	void dispose(QXmlReader* parser, QXmlContentHandler* parent);

	void setKeyModifiers(KeyModifiers* mod)
	{
		m_pModifiers = mod;
	}
	void setKeyMappings(KeyMappings* map)
	{
		m_pMappings = map;
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

	KeyModifiers* m_pModifiers;
	KeyMappings* m_pMappings;

	int m_code;
	int m_mask;
	int m_mapcode;
	int m_unicode;
	QMap<QString,QString> m_mapmodifiers;
	QMap<QString,QString> m_mapunicodes;
};

#endif /* _MAPPINGS_HANDLER_H_ */
