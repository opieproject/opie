#include "opimnotifymanager.h"

OPimNotifyManager::OPimNotifyManager( const Reminders& rem,  const Alarms& al)
    : m_rem( rem ), m_al( al )
{}
OPimNotifyManager::~OPimNotifyManager() {
}
/* use static_cast and type instead of dynamic... */
void OPimNotifyManager::add( const OPimNotify& noti) {
    if ( noti.type() == QString::fromLatin1("OPimReminder") ) {
        const OPimReminder& rem = static_cast<const OPimReminder&>(noti);
        m_rem.append( rem );
    }else if ( noti.type() == QString::fromLatin1("OPimAlarm") ) {
        const OPimAlarm& al = static_cast<const OPimAlarm&>(noti);
        m_al.append( al );
    }
}
void OPimNotifyManager::remove( const OPimNotify& noti) {
    if ( noti.type() == QString::fromLatin1("OPimReminder") ) {
        const OPimReminder& rem = static_cast<const OPimReminder&>(noti);
        m_rem.remove( rem );
    }else if ( noti.type() == QString::fromLatin1("OPimAlarm") ) {
        const OPimAlarm& al = static_cast<const OPimAlarm&>(noti);
        m_al.remove( al );
    }
}
void OPimNotifyManager::replace( const OPimNotify& noti) {
    if ( noti.type() == QString::fromLatin1("OPimReminder") ) {
        const OPimReminder& rem = static_cast<const OPimReminder&>(noti);
        m_rem.remove( rem );
        m_rem.append( rem );
    }else if ( noti.type() == QString::fromLatin1("OPimAlarm") ) {
        const OPimAlarm& al = static_cast<const OPimAlarm&>(noti);
        m_al.remove( al );
        m_al.append( al );
    }
}
OPimNotifyManager::Reminders OPimNotifyManager::reminders()const {
    return m_rem;
}
OPimNotifyManager::Alarms    OPimNotifyManager::alarms()const {
    return m_al;
}
void OPimNotifyManager::setAlarms( const Alarms& al) {
    m_al = al;
}
void OPimNotifyManager::setReminders( const Reminders& rem) {
    m_rem = rem;
}
/* FIXME!!! */
/**
 * The idea is to check if the provider for our service
 * is online
 * if it is we will use QCOP
 * if not the Factory to get the backend...
 * Qtopia1.6 services would be kewl to have here....
 */
void OPimNotifyManager::registerNotify( const OPimNotify& ) {

}
/* FIXME!!! */
/**
 * same as above...
 * Also implement Url model
 * have a MainWindow....
 */
void OPimNotifyManager::deregister( const OPimNotify& ) {

}
