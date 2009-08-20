#ifndef _KEYCFG_HANDLER_H_
#define _KEYCFG_HANDLER_H_

#include <qxml.h>
#include <qstring.h>
#include "ModifiersHandler.h"
#include "MappingsHandler.h"
#include "ExtensionsHandler.h"
#include "RepeaterHandler.h"

class KeycfgHandler : public QXmlDefaultHandler
{
public:
	KeycfgHandler(QXmlReader* parser);
	~KeycfgHandler();

	void dispose(QXmlReader* parser, QXmlContentHandler* parent);

	void setKeyModifiers(KeyModifiers* mod)
	{
		m_pModifiers = mod;
	}
	void setKeyMappings(KeyMappings* map)
	{
		m_pMappings = map;
	}
	void setKeyExtensions(KeyExtensions* ext)
	{
		m_pExtensions = ext;
	}
	void setKeyRepeater(KeyRepeater* rep)
	{
		m_pRepeater = rep;
	}


	bool startElement(const QString& namespaceURI,
		const QString& localName,
		const QString& qName,
		const QXmlAttributes& atts);
private:
	QXmlContentHandler* m_parent;
	QXmlReader* m_parser;

	ModifiersHandler* m_pModHandler;
	MappingsHandler* m_pMapHandler;
	ExtensionsHandler* m_pExtHandler;
	RepeaterHandler* m_pRepHandler;

	KeyModifiers* m_pModifiers;
	KeyMappings* m_pMappings;
	KeyExtensions* m_pExtensions;
	KeyRepeater* m_pRepeater;
};

#endif /* _KEYCFG_HANDLER_H_ */
