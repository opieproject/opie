
#include <stdlib.h>

#include "osqlbackend.h"
#include "osqldriver.h"
#include "osqlmanager.h"
#include "osqlbackendmanager.h"
#include "osqlitedriver.h"

OSQLManager::OSQLManager() {
}
OSQLBackEnd::ValueList OSQLManager::queryBackEnd() {
    m_list.clear();
    QString opie = QString::fromLatin1( getenv("OPIEDIR") );
    QString qpe = QString::fromLatin1( getenv("QPEDIR") );

    if ( !m_path.contains(opie) && !opie.isEmpty() )
        m_path << opie;
    if ( !m_path.contains(qpe) && !qpe.isEmpty() )
        m_path << qpe;

    OSQLBackEndManager mng( m_path );
    m_list = mng.scan();
    m_list += builtIn();

    return m_list;
}
/*
 * loading dso's is currently not enabled due problems with QLibrary
 * beeing in libqpe and not libqte
 */
OSQLDriver* OSQLManager::load( const QString& name ) {
    OSQLDriver* driver = 0l;

    if ( name == "SQLite" ) {
        driver = new OSQLiteDriver();
    }
    return driver;
}
/*
 * same as above
 */
OSQLDriver* OSQLManager::load( const OSQLBackEnd& end) {
    OSQLDriver *driver = 0l;
    if ( end.library() == "builtin" &&
         end.name() == "SQLite" )
        driver = new OSQLiteDriver();

    return driver;
}
/*
 * let's find the a default with the highes preference
 */
OSQLDriver* OSQLManager::standard() {
    OSQLDriver* driver =0l;
    if ( m_list.isEmpty() ) queryBackEnd();
    OSQLBackEnd::ValueList::Iterator it;
    OSQLBackEnd back;
    for ( it = m_list.begin(); it != m_list.end(); ++it ) {
        if ( (*it).isDefault() &&
             back.preference() < (*it).preference() ) {
            back = (*it);
        }
    }
    driver = load( back );
    return driver;
}
void OSQLManager::registerPath( const QString& path ) {
    m_path << path;
}
bool OSQLManager::unregisterPath( const QString& path ) {
    m_path.remove( path );
    return true;
}
OSQLBackEnd::ValueList OSQLManager::builtIn()const {
    OSQLBackEnd::ValueList list;
    // create the OSQLiteBackend
    OSQLBackEnd back("SQLite","Opie e.V.","GPL", "builtin" );
    back.setDefault( true );
    back.setPreference( 50 );
    list.append( back );
    return list;
}
