#include "bookmanager.h"

using namespace Datebook;

BookManager::BookManager() {
    m_db = 0;
}
BookManager::~BookManager() {
    delete m_db;
}
void BookManager::add( const OPimEvent& ev) {
    if (!m_db ) return;

    m_db->add( ev );
}
void BookManager::add( const OPimRecord& rec ) {
    if (!m_db) load(); // we might where called by setDocument... before we even fully initialized
    m_db->add( rec );
}
void BookManager::update( const OPimEvent& up) {
    if ( !m_db ) return;
    m_db->replace( up );
}
void BookManager::remove( int uid ) {
    if ( !m_db ) return;
    m_db->remove( uid );
}
void BookManager::remove( const QArray<int>& ar) {
    uint count = ar.count();
    for (uint i = 0; i < count; i++ )
        remove( ar[i] );
}
QList<OPimRecord> BookManager::records( const QDate& , const QDate& ) {
    return QList<OPimRecord>();
}
bool BookManager::isLoaded() const{
    return ( m_db != 0 );
}
bool BookManager::load() {
    m_db = new ODateBookAccess;
    return m_db->load();
}
void BookManager::reload() {
    if (!m_db ) return;

    m_db->reload();
}
OPimEvent BookManager::event( int uid ) {
    if (!m_db ) return OPimEvent();

    return m_db->find( uid );
}
ODateBookAccess::List BookManager::allRecords()const {
    if (!m_db) return ODateBookAccess::List();

    return m_db->allRecords();
}
OPimOccurrence::List BookManager::list( const QDate& from,
                                              const QDate& to ) {
    if (!m_db) return OPimOccurrence::List();

    return m_db->occurrences( from, to );
}
bool BookManager::save() {
    if (!m_db) return false;
    return m_db->save();
}
