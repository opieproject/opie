
#include "metafactory.h"

MetaFactory::MetaFactory() {
}
MetaFactory::~MetaFactory() {

}
void MetaFactory::addConfigWidgetFactory( const QString& str,
                                          configWidget wid) {
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
