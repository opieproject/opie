#include "ModifiersHandler.h"

void ModifiersHandler::dispose(QXmlReader* parser, QXmlContentHandler* parent)
{
	m_parser = parser;
	m_parent = parent;
	m_parser->setContentHandler(this);
}

bool ModifiersHandler::startElement(const QString& /* namespaceURI */,
	const QString& localName,
	const QString& /* qName */,
	const QXmlAttributes& attr)
{
	if(localName == "define"){
		/* 変数初期化 */
		m_type = QString::null;
		m_code = 0;
		m_mask = 0;
		m_release.clear();
		m_toggle = false;
		m_mapping = false;
		for(int i=0; i<attr.length(); i++){
			if(attr.localName(i).lower() == "key"){
				/* keyname */
				m_code = KeyNames::getCode(attr.value(i));
			} else if(attr.localName(i).lower() == "code"){
				/* keycode */
				m_code = KHUtil::hex2int(attr.value(i).lower());
			} else if(attr.localName(i).lower() == "type"){
				/* modifier type */
				m_type = attr.value(i);
			} else if(attr.localName(i).lower() == "mapping"){
				/* auto mapping */
				if(attr.value(i).lower() == "true"){
					m_mapping = true;
				}
			} else if(attr.localName(i).lower() == "toggle"){
				/* toggle mode */
				if(attr.value(i).lower() == "true"){
					m_toggle = true;
				}
			}
		}
	} else if(localName == "modifier"){
		/* modifier keys */
		for(int i=0; i<attr.length(); i++){
			if(attr.value(i).lower() == "on"){
				m_mask |= m_pModifiers->getMask(attr.localName(i));
			}
		}
	} else if(localName == "release"){
		/* release keys */
		int code = 0;
		for(int i=0; i<attr.length(); i++){
			if(attr.localName(i) == "key"){
				/* keyname */
				code = KeyNames::getCode(attr.value(i));
			} else if(attr.localName(i) == "code"){
				/* keycode */
				code = KHUtil::hex2int(attr.value(i).lower());
			}
		}
		if(code > 0){
			m_release.append(code);
		}
	}
	return(true);
}

bool ModifiersHandler::endElement(const QString& /* namespaceURI */,
	const QString& localName,
	const QString& /* qName */)
{
	if(localName == "define"){
		if(m_type != QString::null && m_code > 0){
			/* assign modifier */
			m_pModifiers->assign(m_type, m_code, m_mask, m_toggle);
			for(QValueList<int>::Iterator it=m_release.begin();
					it!=m_release.end(); ++it){
				/* assign modifier release keys */
				m_pModifiers->assignRelease(*it);
			}
			int code = KeyNames::getCode(m_type);
			if(code <= 0){
				qDebug("setCode[%s][%x]", m_type.latin1(), m_code);
				KeyNames::setCode(m_type, m_code);
			}
			if(m_mapping){
				/* auto mapping */
				m_pMappings->assign(m_code, m_mask, code, 0);
			}
		}
	} else if(localName == "modifiers"){
		/* return parent */
		m_parser->setContentHandler(m_parent);
	}
	return(true);
}
