#include "ofilefactory.h"

OFileFactory::OFileFactory() {
}
OFileFactory::~OFileFactory() {
}
QStringList OFileFactory::lister()const {
    QStringList list;
    QMap<QString, listerFact>::ConstIterator it;
    for ( it = m_lister.begin(); it != m_lister.end(); ++it ) {
        list << (*it);
    }
    return list;
}
QStringList OFileFactory::views()const {
    QStringList list;


    return list;
}
OFileView* OFileFactory::view( const QString& name,
                               OFileSelector*, QWidget* ) {

}
OLister* OFileFactory::lister(const QString&,  OFileSelector*) {

}
void OFileFactory::addLister( const QString&, listerFact fact ) {

}
void OFileFactory::addView( const QString&, viewFact fact ) {


}
void OFileFactory::removeLister( const QString& ) {

}
void OFileFactory::removeView( const QString& ) {

}
