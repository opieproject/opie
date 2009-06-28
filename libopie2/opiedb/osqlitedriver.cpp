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
#include <stdio.h>

namespace Opie {
namespace DB {
namespace Internal {

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
 * Functions to patch a regex search into sqlite
 */
int sqliteRlikeCompare(const char *zPattern, const char *zString, sqregex *reg){
    int res;
    if (reg->regex_raw == NULL || (strcmp (zPattern, reg->regex_raw) != 0)){
        if (reg->regex_raw != NULL) {
            free(reg->regex_raw);
            regfree(&reg->regex_c);
        }
        reg->regex_raw = (char *)malloc(strlen(zPattern)+1);
        strncpy(reg->regex_raw, zPattern, strlen(zPattern)+1);
        res = regcomp(&reg->regex_c, zPattern, REG_EXTENDED);
        if ( res != 0 ) {
            printf("Regcomp failed with code %u on string %s\n",res,zPattern);
            free(reg->regex_raw);
            reg->regex_raw=NULL;
        return 0;
        }
    }
    res = (regexec(&reg->regex_c, zString, 0, NULL, 0)==0);
    return res;
}

void rlikeFunc( sqlite3_context* context, int count, sqlite3_value** values ){
    const unsigned char* argv0 = sqlite3_value_text( values[0] );
    const unsigned char* argv1 = sqlite3_value_text( values[1] ); 
    if( count < 2 || argv0 == 0 || argv1 == 0 ){
	    qWarning( "One of arguments Null!!\n" );
	    return;
    }
    sqlite3_result_int(context, sqliteRlikeCompare((const char*)argv0,
						      (const char*)argv1, 
						      (sqregex *) sqlite3_user_data( context ) ));
}

/*
 * try to open a db specified via setUrl
 * and options
 */
bool OSQLiteDriver::open() {
    odebug << "OSQLiteDriver::open: about to open" << oendl;

    int error = sqlite3_open( m_url.utf8(),
                           &m_sqlite );

    /* failed to open */
    if ( error != SQLITE_OK ) {
        // FIXME set the last error
        owarn << "OSQLiteDriver::open: " << error << "" << oendl;
        sqlite3_close( m_sqlite );
        return false;
    }
    if ( sqlite3_create_function( m_sqlite, "rlike", 2, SQLITE_UTF8, &sqreg, rlikeFunc, NULL, NULL ) != SQLITE_OK ){
	    odebug << "Unable to create user defined function!" << oendl;
	    return false;
    }

    sqreg.regex_raw = NULL;

    return true;
}


/* close the db
 * sqlite closes them without
 * telling failure or success
 */
bool OSQLiteDriver::close() {
	if ( m_sqlite ){
		sqlite3_close( m_sqlite );
		m_sqlite=0l;
	}
	if ( sqreg.regex_raw != NULL){
		odebug << "Freeing regex on close" << oendl;
		free( sqreg.regex_raw );
		sqreg.regex_raw = NULL;
		regfree( &sqreg.regex_c );
	}
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
	if ( sqlite3_exec( m_sqlite, qu->query().utf8(), &call_back, &query, &err )  > SQLITE_OK ) {
		owarn << "OSQLiteDriver::query: error while executing: " << err << oendl;
		sqlite3_free( err );
		// FixMe Errors
	}

	OSQLResult result(OSQLResult::Success,
			  query.items,
			  query.errors );
	return result;
}


OSQLTable::ValueList OSQLiteDriver::tables() const {
    return OSQLTable::ValueList();
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
                              char** argv, char** columns ) {
    Query* qu = (Query*)voi;

    //copy them over to a OSQLResultItem
    QMap<QString, QString> tableString;
    QMap<int, QString> tableInt;
    for (int i = 0; i < argc; i++ ) {

        tableInt.insert( i, QString::fromUtf8( argv[i] ) );
        tableString.insert( QString::fromUtf8( columns[i] ),
                            QString::fromUtf8( argv[i] ) );
    }
    OSQLResultItem item( tableString, tableInt );
    qu->items.append( item );

    return ((Query*)voi)->driver->handleCallBack( argc,
                                                  argv,
                                                  columns );


}

}}} // namespace OPIE::DB::Internal
