#include <qpe/config.h>

#include "stringmanager.h"

using namespace Datebook;

StringManager::StringManager( const QString& str )
    : m_base( str ) {
}
StringManager::~StringManager() {

}
void StringManager::add( const QString& str ) {
    ManagerTemplate<QString>::add(str, str);
}
bool StringManager::doLoad() {
    Config qpe( "datebook-"+m_base );
    qpe.setGroup(m_base );
    QStringList list =  qpe.readListEntry( "Names",  0x1f );
    for (QStringList::Iterator it = list.begin(); it != list.end(); ++it )
        add( (*it) );

    return true;
}
bool StringManager::doSave() {
    Config qpe( "datebook"+m_base );
    qpe.setGroup(m_base);
    qpe.writeEntry( "Names", names(), 0x1f );

    return false;
}


StringManagerDialog::StringManagerDialog(const StringManager& )
    : QDialog(0, 0, true ) {
}
StringManagerDialog::~StringManagerDialog() {

}
StringManager StringManagerDialog::manager()const {
    return StringManager();
}
