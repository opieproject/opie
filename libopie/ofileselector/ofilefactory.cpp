#include "ofilefactory.h"

OFileFactory::OFileFactory() {
}
OFileFactory::~OFileFactory() {
}
QStringList OFileFactory::lister()const {
    QStringList list;
    QMap<QString, listerFact>::ConstIterator it;
    for ( it = m_lister.begin(); it != m_lister.end(); ++it ) {
        list << it.key();
    }
    return list;
}
QStringList OFileFactory::views()const {
    QStringList list;
    QMap<QString, viewFact>::ConstIterator it;
    for (it = m_view.begin(); it != m_view.end(); ++it ) {
        list << it.key();
    }

    return list;
}
OFileView* OFileFactory::view( const QString& name,
                               OFileSelector* sel, QWidget* par) {
    OFileView* vie= 0l;

    QMap<QString, viewFact>::Iterator it;
    it = m_view.find( name );

    if ( it != m_view.end() ) {
        vie = (*(it.data() ) )(sel, par);
    }
    return vie;
}
OLister* OFileFactory::lister(const QString& name,  OFileSelector* sel) {
    OLister* lis = 0l;

    QMap<QString, listerFact>::Iterator it;
    it = m_lister.find( name );
    if ( it != m_lister.end() ) {
        lis = (*(it.data() ) )(sel);
    }

    return lis;
}
void OFileFactory::addLister( const QString& name, listerFact fact ) {
    m_lister.insert( name, fact );
}
void OFileFactory::addView( const QString& name, viewFact fact ) {
    m_view.insert( name, fact );
}
void OFileFactory::removeLister( const QString& name) {
    m_lister.remove( name );
}
void OFileFactory::removeView( const QString& name) {
    m_view.remove( name );
}
