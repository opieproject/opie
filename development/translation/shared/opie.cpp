#include <stdlib.h>

#include <qdir.h>

#include "opie.h"

OPIE* OPIE::m_self = 0;


OPIE::OPIE() {
}
OPIE::~OPIE() {
}
OPIE* OPIE::self() {
    if (!m_self ) m_self = new OPIE;
    return m_self;
}
QStringList OPIE::languageList( const QString& _opieDir )const {
    QString opieDi = opieDir( _opieDir );

    QStringList langs;
    QDir dir( opieDi + "/i18n/");
    if (!dir.exists() ) return langs;
    langs = dir.entryList( QDir::Dirs );

    langs.remove("CVS"); // hey this no language
    langs.remove("unmaintained"); // remove this one too
    langs.remove(".");
    langs.remove("..");



    return langs;
}
QString OPIE::opieDir( const QString& _opieDir ) const{
    if (!_opieDir.isEmpty() ) return _opieDir;
    char* dir = ::getenv("OPIEDIR");
    if (!dir ) return QString::null;
    return QString::fromLatin1(dir);
}
