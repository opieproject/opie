#include "ConfigEx.h"

ConfigEx::ConfigEx(const QString& name, Domain domain)
	: Config(name, domain)
{
	m_charset = "utf8";
	decode();
	m_lastRead = QDateTime::currentDateTime();
}

#if 0
void ConfigEx::removeComment()
{
	for(QMap<QString,ConfigGroup>::Iterator it=groups.begin();
			it!=groups.end(); ++it){
		QStringList removeList;
		for(ConfigGroup::Iterator it2=(*it).begin();
				it2!=(*it).end(); ++it2){
			if(it2.key()[0] == '#'){
				QString key = it2.key();
				removeList.append(it2.key());
			}
		}
		for(QStringList::Iterator it3=removeList.begin();
				it3!=removeList.end(); ++it3){
			(*it).remove(*it3);
		}
	}
}
#endif

void ConfigEx::decode()
{
	QString group = getGroup();
	setGroup("Global");
	QString charset = readEntry("CharSet", "utf8");
	qWarning("ConfigEx::decode()[%s][%s]", charset.latin1(), m_charset.latin1());
	setGroup(group);
	if(charset != m_charset){
		m_charset = charset;
		read();
	}
	//removeComment();
}

void ConfigEx::read()
{
    qWarning("ConfigEx::read()");
    groups.clear();
    changed = FALSE;

    if ( !QFileInfo( filename ).exists() ) {
	git = groups.end();
	return;
    }

    QFile f( filename );
    if ( !f.open( IO_ReadOnly ) ) {
	git = groups.end();
	return;
    }

    QTextStream s( &f );
#ifdef CONFIG_MULTICODEC
    QTextCodec* codec = QTextCodec::codecForName(m_charset);
    if(codec == NULL){
    	codec = QTextCodec::codecForName("utf8");
    	qWarning("Config CharSet[utf8]");
    } else {
    	qWarning("Config CharSet[%s]", m_charset.latin1());
    }
    s.setCodec(codec);
#else /* CONFIG_MULTICODEC */
#if QT_VERSION <= 230 && defined(QT_NO_CODECS)
    // The below should work, but doesn't in Qt 2.3.0
    s.setCodec( QTextCodec::codecForMib( 106 ) );
#else
    s.setEncoding( QTextStream::UnicodeUTF8 );
#endif
#endif /* CONFIG_MULTICODEC */

    QStringList list = QStringList::split('\n', s.read() );

    f.close();

    for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
        if ( !parse( *it ) ) {
            git = groups.end();
            return;
        }
    }
}

QStringList ConfigEx::getKeys()
{
	QStringList keys;
	if(groups.end() != git){
		for(ConfigGroup::ConstIterator it=(*git).begin();
				it!=(*git).end(); ++it){
			if(it.key()[0] != '#'){
				keys.append(it.key());
			}
		}
	}
	return(keys);
}

QDateTime ConfigEx::lastModified()
{
	QFileInfo info(filename);
	return(info.lastModified());
}
