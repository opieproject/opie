/*
                             This file is part of the Opie Project
                             Copyright (C) Stefan Eilers <eilers.stefan@epost.de>
              =.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/
/*
 * =====================================================================
 * ToDo: Use plugins
 * =====================================================================
 */
#ifndef OPIE_BACKENDFACTORY_H_
#define OPIE_BACKENDFACTORY_H_

#include <qstring.h>
#include <qasciidict.h>
#include <qpe/config.h>

#include <opie2/opimglobal.h>
#include <opie2/otodoaccessxml.h>
#include <opie2/ocontactaccessbackend_xml.h>
#include <opie2/odatebookaccessbackend_xml.h>

#ifdef __USE_SQL
#include <opie2/otodoaccesssql.h>
#include <opie2/ocontactaccessbackend_sql.h>
#include <opie2/odatebookaccessbackend_sql.h>
#endif

using namespace Opie;
using namespace Opie::Pim;

namespace Opie {

class OBackendPrivate;

/**
 * This class is our factory. It will give us the default implementations
 * of at least Todolist, Contacts and Datebook. In the future this class will
 * allow users to switch the backend with ( XML->SQLite ) without the need
 * to recompile.#
 * This class - as the whole PIM Api - is making use of templates
 *
 * <pre>
 *   OPimTodoAccessBackend* backend = OBackEndFactory<OPimTodoAccessBackend>::Default("todo", QString::null );
 *   backend->load();
 * </pre>
 *
 * @author Stefan Eilers
 * @version 0.1
 */
template<class T>
class OBackendFactory
{
 public:
	OBackendFactory() {};

        /**
         * Returns a backend implementation
         * @param type the type of the backend
	 * @param database the type of the used database
         * @param appName The name of your application. It will be passed on to the backend.
         */
	static T* create( OPimGlobal::PimType type, OPimGlobal::DatabaseStyle database, 
			  const QString& appName ){
		qWarning("Selected backend for %d is: %d", type, database ); 

		switch ( type ){
		case OPimGlobal::TODOLIST:
#ifdef __USE_SQL
			if ( database == OPimGlobal::SQL )
				return (T*) new OPimTodoAccessBackendSQL("");
#else
			if ( database == OPimGlobal::SQL )
				qWarning ("OBackendFactory:: sql Backend for TODO not implemented! Using XML instead!");
#endif

			return (T*) new OPimTodoAccessXML( appName );
		case OPimGlobal::CONTACTLIST:
#ifdef __USE_SQL
			if ( database == OPimGlobal::SQL )
				return (T*) new OPimContactAccessBackend_SQL("");
#else
			if ( database == OPimGlobal::SQL )
				qWarning ("OBackendFactory:: sql Backend for CONTACT not implemented! Using XML instead!");
#endif

			return (T*) new OPimContactAccessBackend_XML( appName );
		case OPimGlobal::DATEBOOK:
#ifdef __USE_SQL
			if ( database == OPimGlobal::SQL )
				return (T*) new ODateBookAccessBackend_SQL("");
#else
			if ( database == OPimGlobal::SQL )
                            qWarning("OBackendFactory:: sql Backend for DATEBOOK not implemented! Using XML instead!");
#endif

			return (T*) new ODateBookAccessBackend_XML( appName );
		default:
			return (T*) NULL;
		}


	}

        /**
         * Returns the default backend implementation for backendName. Which one is used, is defined
	 * by the configfile "pimaccess.conf".
         * @param backendName the type of the backend (use "todo", "contact" or "datebook" )
         * @param appName The name of your application. It will be passed on to the backend
         */
	static T* Default( const QString backendName, const QString& appName ){

		QAsciiDict<int> dictBackends( OPimGlobal::_END_PimType );
 		dictBackends.setAutoDelete ( TRUE );

		dictBackends.insert( "todo", new int (OPimGlobal::TODOLIST) );
		dictBackends.insert( "contact", new int (OPimGlobal::CONTACTLIST) );
                dictBackends.insert( "datebook", new int(OPimGlobal::DATEBOOK) );

		QAsciiDict<int> dictDbTypes( OPimGlobal::_END_DatabaseStyle );
		dictDbTypes.setAutoDelete( TRUE );

		dictDbTypes.insert( "xml", new int (OPimGlobal::XML) );
		dictDbTypes.insert( "sql", new int (OPimGlobal::SQL) );
		dictDbTypes.insert( "vcard", new int (OPimGlobal::VCARD) );

		Config config( "pimaccess" );
		config.setGroup ( backendName );
		QString db_String = config.readEntry( "usebackend" );

		int* db_find = dictDbTypes[ db_String ];
                int* backend_find = dictBackends[ backendName ];
                if ( !backend_find || !db_find ) return NULL;

		qDebug( "OBackendFactory::Default -> Backend is %s, Database is %s", backendName.latin1(), 
			db_String.latin1() );
		
		return create( (OPimGlobal::PimType) *backend_find, (OPimGlobal::DatabaseStyle) *db_find, appName );

	}
    private:
	OBackendPrivate* d;
	
};

}

#endif
