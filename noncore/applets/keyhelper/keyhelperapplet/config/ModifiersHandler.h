#ifndef _MODIFIERS_HANDLER_H_
#define _MODIFIERS_HANDLER_H_

#include <qstring.h>
#include <qvaluelist.h>
#include <qxml.h>
#include "KeyModifiers.h"
#include "KeyMappings.h"
#include "KeyNames.h"
#include "KHUtil.h"

class ModifiersHandler : public QXmlDefaultHandler
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

	QString m_type;
	int m_code;
	int m_mask;
	QValueList<int> m_release;
	bool m_mapping;
	bool m_toggle;
};

#endif /* _MODIFIERS_HANDLER_H_ */
