#include "MappingsHandler.h"

void MappingsHandler::dispose(QXmlReader* parser, QXmlContentHandler* parent)
{
	m_parser = parser;
	m_parent = parent;
	m_parser->setContentHandler(this);
}

bool MappingsHandler::startElement(const QString& /* namespaceURI */,
	const QString& localName,
	const QString& /* qName */,
	const QXmlAttributes& attr)
{
	if(localName == "define"){
		/* 変数初期化 */
		m_code = 0;
		m_mask = 0;
		m_mapcode = 0;
		m_unicode = 0;
		m_mapmodifiers.clear();
		m_mapunicodes.clear();
		for(int i=0; i<attr.length(); i++){
			if(attr.localName(i).lower() == "key"){
				/* keyname */
				m_code = KeyNames::getCode(attr.value(i));
			} else if(attr.localName(i).lower() == "code"){
				/* keycode */
				m_code = KHUtil::hex2int(attr.value(i).lower());
			}
		}
	} else if(localName == "modifier"){
		/* modifier keys */
		for(int i=0; i<attr.length(); i++){
			if(attr.value(i).lower() == "on"){
				m_mask |= m_pModifiers->getMask(attr.localName(i));
			}
		}
	} else if(localName == "map"){
		/* mapping key */
		for(int i=0; i<attr.length(); i++){
			if(attr.localName(i).lower() == "key"){
				/* keyname */
				m_mapcode = KeyNames::getCode(attr.value(i));
			} else if(attr.localName(i).lower() == "code"){
				/* keycode */
				m_mapcode = KHUtil::hex2int(attr.value(i).lower());
			}
		}
	} else if(localName == "map_modifier"){
		/* mapping modifiers */
		for(int i=0; i<attr.length(); i++){
			m_mapmodifiers[attr.localName(i)] = attr.value(i);
		}
	} else if(localName == "map_unicode"){
		/* mapping unicodes */
		for(int i=0; i<attr.length(); i++){
			if(attr.localName(i).lower() == "char"){
				/* unicode char */
				m_unicode = attr.value(i)[0].unicode();
			} else if(attr.localName(i).lower() == "code"){
				/* unicode code */
				m_unicode = KHUtil::hex2int(attr.value(i).lower());
			} else {
				m_mapunicodes[attr.localName(i)] = attr.value(i);
			}
		}
	}
	return(true);
}

bool MappingsHandler::endElement(const QString& /* namespaceURI */,
	const QString& localName,
	const QString& /* qName */)
{
	if(localName == "define"){
		if(m_code > 0){
			/* assign mapping */
			m_pMappings->assign(m_code, m_mask, m_mapcode);

			for(QMap<QString,QString>::Iterator it=m_mapmodifiers.begin();
					it!=m_mapmodifiers.end(); ++it){
				/* assign mapping modifier state */
				m_pMappings->assignModifier(it.key(), it.data());
			}

			if(m_unicode > 0){
				m_pMappings->assignUnicode(m_unicode);
			} else {
				for(QMap<QString,QString>::Iterator it=m_mapunicodes.begin();
						it!=m_mapunicodes.end(); ++it){
					/* assign mapping unicode */
					m_pMappings->assignUnicode(it.key(), it.data());
				}
			}
		}
	} else if(localName == "mappings"){
		/* return parent */
		m_parser->setContentHandler(m_parent);
	}
	return(true);
}
