#include "opimxrefmanager.h"


OPimXRefManager::OPimXRefManager() {
}
OPimXRefManager::OPimXRefManager( const OPimXRefManager& ref) {
    m_list = ref.m_list;
}
OPimXRefManager::~OPimXRefManager() {
}
OPimXRefManager &OPimXRefManager::operator=( const OPimXRefManager& ref) {
    m_list = ref.m_list;
    return *this;
}
bool OPimXRefManager::operator==( const OPimXRefManager& /*ref*/) {
    //   if ( m_list == ref.m_list ) return true;

    return false;
}
void OPimXRefManager::add( const OPimXRef& ref) {
    m_list.append( ref );
}
void OPimXRefManager::remove( const OPimXRef& ref) {
    m_list.remove( ref );
}
void OPimXRefManager::replace( const OPimXRef& ref) {
    m_list.remove( ref );
    m_list.append( ref );
}
void OPimXRefManager::clear() {
    m_list.clear();
}
QStringList OPimXRefManager::apps()const {
    OPimXRef::ValueList::ConstIterator it;
    QStringList list;

    QString str;
    for ( it = m_list.begin(); it != m_list.end(); ++it ) {
        str = (*it).partner( OPimXRef::One ).appName();
        if ( !list.contains( str ) ) list << str;

        str = (*it).partner( OPimXRef::Two ).appName();
        if ( !list.contains( str ) ) list << str;
    }
    return list;
}
OPimXRef::ValueList OPimXRefManager::list()const {
    return m_list;
}
OPimXRef::ValueList OPimXRefManager::list( const QString& appName )const{
    OPimXRef::ValueList list;
    OPimXRef::ValueList::ConstIterator it;

    for ( it = m_list.begin(); it != m_list.end(); ++it ) {
        if ( (*it).containsString( appName ) )
            list.append( (*it) );
    }

    return list;
}
OPimXRef::ValueList OPimXRefManager::list( int uid )const {
    OPimXRef::ValueList list;
    OPimXRef::ValueList::ConstIterator it;

    for ( it = m_list.begin(); it != m_list.end(); ++it ) {
        if ( (*it).containsUid( uid ) )
            list.append( (*it) );
    }

    return list;
}
