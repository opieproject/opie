#include "RepeaterHandler.h"

void RepeaterHandler::dispose(QXmlReader* parser, QXmlContentHandler* parent)
{
	m_parser = parser;
	m_parent = parent;
	m_parser->setContentHandler(this);
}

bool RepeaterHandler::startElement(const QString& /* namespaceURI */,
	const QString& localName,
	const QString& /* qName */,
	const QXmlAttributes& attr)
{
	if(localName == "repeat"){
		for(int i=0; i<attr.length(); i++){
			if(attr.localName(i).lower() == "delay"){
				bool ok;
				int delay = attr.value(i).toInt(&ok);
				if(ok){
					m_pRepeater->setDelay(delay);
				}
			} else if(attr.localName(i).lower() == "period"){
				bool ok;
				int period = attr.value(i).toInt(&ok);
				if(ok){
					m_pRepeater->setPeriod(period);
				}
			} else if(attr.localName(i).lower() == "mode"){
				/* default mode */
				bool ok;
				int mode = attr.value(i).toInt(&ok);
				if(ok){
					m_pRepeater->setMode(mode);
				}
			}
		}
	} else if(localName == "define"){
		/* 変数初期化 */
		m_code = 0;
		m_enable = true;
		for(int i=0; i<attr.length(); i++){
			if(attr.localName(i).lower() == "key"){
				/* keyname */
				m_code = KeyNames::getCode(attr.value(i));
			} else if(attr.localName(i).lower() == "code"){
				/* keycode */
				m_code = KHUtil::hex2int(attr.value(i).lower());
			} else if(attr.localName(i).lower() == "enable"){
				/* enable/disable */
				if(attr.value(i).lower() == "false"){
					m_enable = false;
				} else {
					m_enable = true;
				}
			}
		}
	}
	return(true);
}

bool RepeaterHandler::endElement(const QString& /* namespaceURI */,
	const QString& localName,
	const QString& /* qName */)
{
	if(localName == "define"){
		if(m_code > 0){
			/* set repeat enable/disable */
			m_pRepeater->setRepeatable(m_code, m_enable);
		}
	} else if(localName == "repeater"){
		/* return parent */
		m_parser->setContentHandler(m_parent);
	}
	return(true);
}
