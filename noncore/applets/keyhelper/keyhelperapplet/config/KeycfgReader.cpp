#include "KeycfgReader.h"

KeycfgReader::KeycfgReader()
{
	qDebug("KeycfgReader::KeycfgReader()");
}

KeycfgReader::~KeycfgReader()
{
	qDebug("KeycfgReader::~KeycfgReader()");
}

bool KeycfgReader::load()
{
	QString file;
	ConfigEx& cfg = ConfigEx::getInstance("keyhelper");

	cfg.setGroup("Global");

	file = cfg.readEntry("XmlFile");
	if(file.length() == 0 || !QFile::exists(file)){
		/* default */
		file = QString(getenv( "HOME" )) + QString("/Settings/keyhelper.xml");
	}
	return(load(file));
}

bool KeycfgReader::load(const QString& path)
{
	bool success = false;

	KeyNames::reset();

	QXmlSimpleReader parser;
	KeycfgHandler handler(&parser);
	KeycfgErrorHandler errhandler;
	QFile file(path);

	qWarning("KeycfgReader::load()[%s]", path.latin1());

	if(file.exists()){
		QXmlInputSource source(file);

		handler.setKeyModifiers(m_pModifiers);
		handler.setKeyMappings(m_pMappings);
		handler.setKeyExtensions(m_pExtensions);
		handler.setKeyRepeater(m_pRepeater);

		parser.setContentHandler(&handler);
		parser.setErrorHandler(&errhandler);
		success = parser.parse(source);

		file.close();
	}

	if(success){
		m_pModifiers->statistics();
		m_pMappings->statistics();
		m_pExtensions->statistics();
		m_pRepeater->statistics();
	} else {
		m_pModifiers->reset();
		m_pMappings->reset();
		m_pExtensions->reset();
		m_pRepeater->reset();
	}
	KeyNames::clearCode();

	return(success);
}
