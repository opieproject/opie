#ifndef OSQL_BACKEND_MANAGER_H
#define OSQL_BACKEND_MANAGER_H

#include <qstringlist.h>

#include "osqlbackend.h"

class OSQLBackEndManager {
public:
    OSQLBackEndManager(const QStringList& path );
    ~OSQLBackEndManager();
    OSQLBackEnd::ValueList scan();
private:
    OSQLBackEnd::ValueList scanDir( const QString& dir );
    OSQLBackEnd file2backend( const QString& file );
    class OSQLBackEndManagerPrivate;
    OSQLBackEndManagerPrivate* d;
    QStringList m_path;
};
#endif
