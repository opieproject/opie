#include "ExtensionsHandler.h"

void ExtensionsHandler::dispose(QXmlReader* parser, QXmlContentHandler* parent)
{
	m_parser = parser;
	m_parent = parent;
	m_parser->setContentHandler(this);
}

bool ExtensionsHandler::startElement(const QString& /* namespaceURI */,
	const QString& localName,
	const QString& /* qName */,
	const QXmlAttributes& attr)
{
	if(localName == "define"){
		/* 変数初期化 */
		m_kind = QString::null;
		m_code = -1;
		//m_mask = 0;
		m_modlist.clear();
		for(int i=0; i<attr.length(); i++){
			if(attr.localName(i).lower() == "key"){
				/* keyname */
				if(attr.value(i) == "All"){
					m_code = 0;
				} else {
					m_code = KeyNames::getCode(attr.value(i));
				}
			} else if(attr.localName(i).lower() == "code"){
				/* keycode */
				m_code = KHUtil::hex2int(attr.value(i).lower());
			} else if(attr.localName(i).lower() == "kind"){
				/* extension kind */
				m_kind = attr.value(i);
			}
		}
	} else if(localName == "modifier"){
		/* modifier keys */
		for(int i=0; i<attr.length(); i++){
			if(attr.value(i).lower() == "on"){
				m_modlist.append(attr.localName(i));
				//m_mask |= m_pModifiers->getMask(attr.localName(i));
			}
		}
	}
	return(true);
}

bool ExtensionsHandler::endElement(const QString& /* namespaceURI */,
	const QString& localName,
	const QString& /* qName */)
{
	if(localName == "define"){
#if 0
		if(m_kind != QString::null
			&& (m_code > 0 || m_modlist.isEmpty() == false)){
#else
		if(m_kind != QString::null && m_code >= 0){
#endif
			/* assign extension */
			int keymask = 0;
			QValueList<int> modcodes;
			for(QStringList::Iterator it=m_modlist.begin();
					it!=m_modlist.end(); ++it){
				keymask |= m_pModifiers->getMask(*it);
				qDebug("mask[%s][%x][%s]", m_kind.latin1(), keymask, (*it).latin1());
				int code = KeyNames::getCode(*it);
				if(code > 0){
					modcodes.append(code);
				}
			}
			m_pExtensions->assign(m_kind, m_code, keymask, modcodes);
		}
	} else if(localName == "extensions"){
		m_pExtensions->init();
		/* return parent */
		m_parser->setContentHandler(m_parent);
	}
	return(true);
}
