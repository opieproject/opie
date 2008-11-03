#include "bookmanager.h"

#include <qpe/alarmserver.h>
#include <opie2/opimnotifymanager.h>

using namespace Opie;
using namespace Opie::Datebook;

BookManager::BookManager() {
    m_db = 0;
}

BookManager::~BookManager() {
    delete m_db;
}

void BookManager::add( const OPimEvent& ev) {
    if (!m_db ) return;

    addAlarms( ev );
    m_db->add( ev );
}

void BookManager::add( const OPimRecord& rec ) {
    if (!m_db) load(); // we might where called by setDocument... before we even fully initialized

    // FIXME alarms
    m_db->add( rec );
}

void BookManager::update( const OPimEvent& up) {
    if ( !m_db ) return;

    removeAlarms( event(up.uid()) ); // need to remove old alarms
    addAlarms( up );
    m_db->replace( up );
}

void BookManager::remove( int uid ) {
    if ( !m_db ) return;

    removeAlarms( event(uid) );
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
    if(!m_db) {
        m_db = new ODateBookAccess;
        return m_db->load();
    }
    else
        return true;
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

void BookManager::addAlarms( const OPimEvent &ev ) {
    const OPimNotifyManager::Alarms &als = ev.notifiers().alarms();
    OPimNotifyManager::Alarms::ConstIterator it;
    for ( it = als.begin(); it != als.end(); ++it ) {
        AlarmServer::addAlarm( (*it).dateTime(), "QPE/Application/datebook2", "alarm(QDateTime,int)", ev.uid() );
    }
}

void BookManager::removeAlarms( const OPimEvent &ev ) {
    const OPimNotifyManager::Alarms &als = ev.notifiers().alarms();
    OPimNotifyManager::Alarms::ConstIterator it;
    for ( it = als.begin(); it != als.end(); ++it ) {
        AlarmServer::deleteAlarm( (*it).dateTime(), "QPE/Application/datebook2", "alarm(QDateTime,int)", ev.uid() );
    }
}
