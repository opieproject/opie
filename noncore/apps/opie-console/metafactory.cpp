#include <qpe/config.h>
#include "metafactory.h"

MetaFactory::MetaFactory() {
}
MetaFactory::~MetaFactory() {

}
void MetaFactory::addConfigWidgetFactory( const QString& str,
                                          const QString& name,
                                          configWidget wid) {
    m_namemap.insert ( str, name );
    m_confFact.insert( str, wid );
}
void MetaFactory::addIOLayerFactory( const QString& str,
                                     iolayer lay) {
    m_layerFact.insert( str, lay );
}
void MetaFactory::addFileTransferLayer( const QString& str,
                                        filelayer lay) {
    m_fileFact.insert( str, lay );
}
QStringList MetaFactory::ioLayers()const {
    QStringList list;
    QMap<QString, iolayer>::ConstIterator it;
    for (it = m_layerFact.begin(); it != m_layerFact.end(); ++it ) {
        list << it.key();
    }
    return list;
}
QStringList MetaFactory::configWidgets()const {
    QStringList list;
    QMap<QString,  configWidget>::ConstIterator it;
    for ( it = m_confFact.begin(); it != m_confFact.end(); ++it ) {
        list << it.key();
    }
    return list;
}
QStringList MetaFactory::fileTransferLayers()const {
    QStringList list;
    QMap<QString, filelayer>::ConstIterator it;
    for ( it = m_fileFact.begin(); it != m_fileFact.end(); ++it ) {
        list << it.key();
    }
    return list;
}

IOLayer* MetaFactory::newIOLayer( const QString& str,const Profile& prof ) {
    IOLayer* lay = 0l;

    QMap<QString, iolayer>::Iterator it;
    it = m_layerFact.find( str );
    if ( it != m_layerFact.end() ) {
        lay = (*(it.data()))(prof);
        /*
        iolayer laye = it.data();
        lay = (*laye )(conf);*/
    }

    return lay;
}

ProfileEditorPlugin *MetaFactory::newConfigPlugin ( const QString& str, QWidget *parent, const Profile& prof) {
    ProfileEditorPlugin *p = NULL;
    configWidget c;

    c = m_confFact[str];
    if(c) p = c(parent, prof);

    return p;
}

QString MetaFactory::name( const QString& str ) {
    return m_namemap[str];
}

