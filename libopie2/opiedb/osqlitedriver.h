#ifndef OSQL_LITE_DRIVER_H
#define OSQL_LITE_DRIVER_H

#include <sqlite.h>
#if defined (__GNUC__) && (__GNUC__ < 3)
#include <sys/types.h>
#endif
#ifdef Q_OS_MACX 
#include <sys/types.h>
#endif
#include <regex.h>

#include "osqldriver.h"
#include "osqlerror.h"
#include "osqlresult.h"

namespace Opie {
namespace DB {
namespace Internal {

struct sqregex {
	char *regex_raw;
	regex_t regex_c;
};

class OSQLiteDriver : public OSQLDriver {
    Q_OBJECT
public:
    OSQLiteDriver( QLibrary *lib = 0l );
    ~OSQLiteDriver();
    QString id()const;
    void setUserName( const QString& );
    void setPassword( const QString& );
    void setUrl( const QString& url );
    void setOptions( const QStringList& );
    bool open();
    bool close();
    OSQLError lastError();
    OSQLResult query( OSQLQuery* );
    OSQLTable::ValueList tables()const;

private:
    OSQLError m_lastE;
    OSQLResult m_result;
    OSQLResultItem m_items;
    int handleCallBack( int, char**, char** );
    static int call_back( void*, int, char**, char** );
    QString m_url;
    sqlite *m_sqlite;
    sqregex sqreg;
};
}
}
}

#endif
