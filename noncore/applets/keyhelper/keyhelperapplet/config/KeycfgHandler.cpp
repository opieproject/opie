#include "KeycfgHandler.h"

KeycfgHandler::KeycfgHandler(QXmlReader* parser)
	: QXmlDefaultHandler()
{
	m_parser = parser;
	m_pModHandler = new ModifiersHandler();
	m_pMapHandler = new MappingsHandler();
	m_pExtHandler = new ExtensionsHandler();
	m_pRepHandler = new RepeaterHandler();
}

KeycfgHandler::~KeycfgHandler()
{
	delete m_pModHandler;
	delete m_pMapHandler;
	delete m_pExtHandler;
	delete m_pRepHandler;
}

bool KeycfgHandler::startElement(const QString& /* namespaceURI */,
	const QString& localName,
	const QString& /* qName */,
	const QXmlAttributes& /* attr */)
{
	if(localName == "modifiers"){
		m_pModHandler->setKeyModifiers(m_pModifiers);
		m_pModHandler->setKeyMappings(m_pMappings);
		m_pModHandler->dispose(m_parser, this);
	} else if(localName == "mappings"){
		m_pMapHandler->setKeyModifiers(m_pModifiers);
		m_pMapHandler->setKeyMappings(m_pMappings);
		m_pMapHandler->dispose(m_parser, this);
	} else if(localName == "extensions"){
		m_pExtHandler->setKeyModifiers(m_pModifiers);
		m_pExtHandler->setKeyExtensions(m_pExtensions);
		m_pExtHandler->dispose(m_parser, this);
	} else if(localName == "repeater"){
		m_pRepHandler->setKeyRepeater(m_pRepeater);
		m_pRepHandler->dispose(m_parser, this);
	}
	return(true);
}
