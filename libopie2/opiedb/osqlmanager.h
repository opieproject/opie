
#ifndef OSQL_MANAGER_H
#define OSQL_MANAGER_H

#include <qobject.h>
#include <qstringlist.h>

#include "osqlbackend.h"


namespace Opie {
namespace DB {
/**
 * OSQLManager is responsible for loading
 * and unloading, querying different OSQL
 * services
 * Load a OSQLDriver and delete it yourself
 *
 */
class OSQLDriver;
class OSQLManager : public QObject {
    Q_OBJECT
public:
    /**
     * Empty c'tor
     */
    OSQLManager();

    /**
     * Query the Manager for different backends
     */
    OSQLBackEnd::ValueList queryBackEnd();

    /**
     * Load a backend with it's name from param name
     */
    OSQLDriver* load( const QString& name );

    /**
     * Load a OSQLDevice from const reference of OSQLBackEnd
     */
    OSQLDriver* load( const OSQLBackEnd& );

    /**
     * loads the Opie standard backend
     */
    OSQLDriver *standard();

    /**
     * register path to the search path list
     * When querying for services we scan all the
     * registered path for backends
     */
    void registerPath( const QString& path );

    /**
     * unregisterPath from the search path list
     */
    bool unregisterPath( const QString& path );
private:
    OSQLBackEnd::ValueList builtIn()const;
    OSQLBackEnd::ValueList m_list;
    QStringList m_path;
    class Private;
    Private *d;
};
}
}

#endif
