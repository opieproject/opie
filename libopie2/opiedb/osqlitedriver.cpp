/*
                             This file is part of the Opie Project

              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "osqlquery.h"
#include "osqlitedriver.h"

#include <opie2/odebug.h>

#include <stdlib.h>

// fromLocal8Bit() does not work as expected. Thus it
// is replaced by fromLatin1() (eilers)
#define __BUGGY_LOCAL8BIT_


using namespace Opie::DB;
using namespace Opie::DB::Private;

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
    qDebug("OSQLiteDriver::open: about to open");
    m_sqlite = sqlite_open(m_url.local8Bit(),
                           0,
                           &error );

    /* failed to open */
    if (m_sqlite == 0l ) {
        // FIXME set the last error
        qWarning("OSQLiteDriver::open: %s", error );
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
        qWarning("OSQLiteDriver::query: Error while executing");
        free(err );
        // FixMe Errors
    }

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
    Query* qu = (Query*)voi;

    //copy them over to a OSQLResultItem
    QMap<QString, QString> tableString;
    QMap<int, QString> tableInt;
    for (int i = 0; i < argc; i++ ) {

#ifdef __BUGGY_LOCAL8BIT_
        tableInt.insert( i, QString::fromLatin1( argv[i] ) );
        tableString.insert( QString::fromLatin1( columns[i] ),
                            QString::fromLatin1( argv[i] ) );
#else
        tableInt.insert( i, QString::fromLocal8Bit( argv[i] ) );
        tableString.insert( QString::fromLocal8Bit( columns[i] ),
                            QString::fromLocal8Bit( argv[i] ) );
#endif
    }
    OSQLResultItem item( tableString, tableInt );
    qu->items.append( item );

    return ((Query*)voi)->driver->handleCallBack( argc,
                                                  argv,
                                                  columns );


}
