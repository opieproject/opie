#ifndef _EXTENSIONS_HANDLER_H_
#define _EXTENSIONS_HANDLER_H_

#include <qstring.h>
#include <qstringlist.h>
#include <qvaluelist.h>
#include <qxml.h>
#include "KeyModifiers.h"
#include "KeyExtensions.h"
#include "KeyNames.h"
#include "KHUtil.h"

class ExtensionsHandler : public QXmlDefaultHandler
{
public:
	void dispose(QXmlReader* parser, QXmlContentHandler* parent);

	void setKeyModifiers(KeyModifiers* mod)
	{
		m_pModifiers = mod;
	}
	void setKeyExtensions(KeyExtensions* ext)
	{
		m_pExtensions = ext;
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
	KeyExtensions* m_pExtensions;

	QString m_kind;
	int m_code;
	QStringList m_modlist;
	//int m_mask;

};

#endif /* _EXTENSIONS_HANDLER_H_ */
