/*
                             This file is part of the Opie Project
                             Copyright (C) The Main Author <main-author@whereever.org>
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

#include <opie2/otodoaccessxml.h>
#include <opie2/ocontactaccessbackend_xml.h>
#include <opie2/odatebookaccessbackend_xml.h>

#ifdef __USE_SQL
#include <opie2/otodoaccesssql.h>
#include <opie2/ocontactaccessbackend_sql.h>
#include <opie2/odatebookaccessbackend_sql.h>
#endif

namespace Opie {

class OBackendPrivate;

/**
 * This class is our factory. It will give us the default implementations
 * of at least Todolist, Contacts and Datebook. In the future this class will
 * allow users to switch the backend with ( XML->SQLite ) without the need
 * to recompile.#
 * This class as the whole PIM Api is making use of templates
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

	enum BACKENDS {
		TODO,
		CONTACT,
		DATE
	};

        /**
         * Returns a backend implementation for backendName
         * @param backendName the type of the backend
         * @param appName will be passed on to the backend
         */
	static T* Default( const QString backendName, const QString& appName ){

		// __asm__("int3");

		Config config( "pimaccess" );
		config.setGroup ( backendName );
		QString backend = config.readEntry( "usebackend" );

		qWarning("Selected backend for %s is: %s", backendName.latin1(), backend.latin1() ); 

		QAsciiDict<int> dict ( 3 );
		dict.setAutoDelete ( TRUE );

		dict.insert( "todo", new int (TODO) );
		dict.insert( "contact", new int (CONTACT) );
                dict.insert( "datebook", new int(DATE) );

                int *find = dict[ backendName ];
                if (!find ) return 0;

		switch ( *find ){
		case TODO:
#ifdef __USE_SQL
			if ( backend == "sql" )
				return (T*) new OPimTodoAccessBackendSQL("");
#else
			if ( backend == "sql" )
				qWarning ("OBackendFactory:: sql Backend for TODO not implemented! Using XML instead!");
#endif

			return (T*) new OPimTodoAccessXML( appName );
		case CONTACT:
#ifdef __USE_SQL
			if ( backend == "sql" )
				return (T*) new OPimContactAccessBackend_SQL("");
#else
			if ( backend == "sql" )
				qWarning ("OBackendFactory:: sql Backend for CONTACT not implemented! Using XML instead!");
#endif

			return (T*) new OPimContactAccessBackend_XML( appName );
		case DATE:
#ifdef __USE_SQL
			if ( backend == "sql" )
				return (T*) new ODateBookAccessBackend_SQL("");
#else
			if ( backend == "sql" )
                            qWarning("OBackendFactory:: sql Backend for DATEBOOK not implemented! Using XML instead!");
#endif

			return (T*) new ODateBookAccessBackend_XML( appName );
		default:
			return NULL;
		}


	}
    private:
	OBackendPrivate* d;
};

}

#endif
