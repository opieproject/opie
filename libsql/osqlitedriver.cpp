#include <stdlib.h>

#include "osqlresult.h"
#include "osqlquery.h"
#include "osqlitedriver.h"


namespace {
    struct Query {
        OSQLError::ValueList errors;
        OSQLResultItem::ValueList items;
        OSQLiteDriver *driver;
    };
}

OSQLiteDriver::OSQLiteDriver( QLibrary *lib )
    : OSQLDriver( lib )
{
    m_sqlite = 0l;
}
OSQLiteDriver::~OSQLiteDriver() {
    close();
}

QString OSQLiteDriver::id()const {
    return QString::fromLatin1("SQLite");
}

void OSQLiteDriver::setUserName( const QString& ) {}
void OSQLiteDriver::setPassword( const QString& ) {}

void OSQLiteDriver::setUrl( const QString& url ) {
    m_url = url;
}
void OSQLiteDriver::setOptions( const QStringList& ) {
}
/*
 * try to open a db specified via setUrl
 * and options
 */
bool OSQLiteDriver::open() {
    char *error;
    qWarning("about to open");
    m_sqlite = sqlite_open(m_url.local8Bit(),
                           0,
                           &error );

    /* failed to open */
    if (m_sqlite == 0l ) {
        // FIXME set the last error
        qWarning("error:%s",  error );
        free( error );
        return false;
    }
    return true;
}
/* close the db
 * sqlite closes them without
 * telling failure or success
 */
bool OSQLiteDriver::close() {
    if (m_sqlite )
        sqlite_close( m_sqlite ), m_sqlite=0l;

    return true;
}
/* Query */
OSQLResult OSQLiteDriver::query( OSQLQuery* qu) {
    if ( !m_sqlite ) {
        // FIXME set error code
        OSQLResult result( OSQLResult::Failure );
        return result;
    }
    Query query;
    query.driver = this;
    char *err;
    /* SQLITE_OK 0 if return code > 0 == failure */
    if ( sqlite_exec(m_sqlite, qu->query(),&call_back, &query, &err)  > 0 ) {
        qWarning("Error while executing");
        free(err );
        // FixMe Errors
    }
    qWarning("Item count is %d", query.items.count() );
    OSQLResult result(OSQLResult::Success,
                      query.items,
                      query.errors );
    return result;
}
OSQLTable::ValueList OSQLiteDriver::tables() const {

}
OSQLError OSQLiteDriver::lastError() {
    OSQLError error;
    return error;
};
/* handle a callback add the row to the global
 * OSQLResultItem
 */
int OSQLiteDriver::handleCallBack( int, char**, char** ) {
    return 0;
}
/* callback_handler add the values to the list*/
int OSQLiteDriver::call_back( void* voi, int argc,
                              char** argv, char** columns) {
    qWarning("Callback with %d items", argc );
    Query* qu = (Query*)voi;

    //copy them over to a OSQLResultItem
    QMap<QString, QString> tableString;
    QMap<int, QString> tableInt;
    for (int i = 0; i < argc; i++ ) {
        qWarning("%s|%s", columns[i], argv[i] );
        tableInt.insert( i, QString::fromLocal8Bit(argv[i] ) );
        tableString.insert( QString::fromLocal8Bit( columns[i]),
                            QString::fromLocal8Bit( argv[i] ) );

    }
    OSQLResultItem item( tableString, tableInt );
    qu->items.append( item );

    return ((Query*)voi)->driver->handleCallBack( argc,
                                                  argv,
                                                  columns );


}
