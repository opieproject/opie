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

#include <opie2/opimaccessbackend.h>
#include <opie2/opimglobal.h>
#include <opie2/otodoaccessxml.h>
#include <opie2/otodoaccessvcal.h>
#include <opie2/ocontactaccessbackend_xml.h>
#include <opie2/ocontactaccessbackend_vcard.h>
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
         * Returns a selected backend implementation
         * @param type the type of the backend
	 * @param database the type of the used database
         * @param appName The name of your application. It will be passed on to the backend.
	 * @param filename Filename of the database file if you don't want to access the default 
         */
	static T* create( OPimGlobal::PimType type, OPimGlobal::DatabaseStyle database,
			  const QString& appName, const QString& filename = QString::null ){
		qWarning("Selected backend for %d is: %d", type, database );
		// If we should use the dafult database style, we have to request it
		OPimGlobal::DatabaseStyle use_database = database;
		if ( use_database == OPimGlobal::DEFAULT ){
			use_database = defaultDB( type );
		}

		switch ( type ){
		case OPimGlobal::TODOLIST:

			switch ( use_database ){
			default: // Use SQL if something weird is given.
				// Fall through !!
			case OPimGlobal::SQL:
#ifdef __USE_SQL
				return (T*) new OPimTodoAccessBackendSQL( filename );
				break;
#else
				qWarning ("OBackendFactory:: sql Backend for TODO not implemented! Using XML instead!");
				// Fall through !!
#endif
			case OPimGlobal::XML:
				return (T*) new OPimTodoAccessXML( appName, filename );
				break;
			case OPimGlobal::VCARD:
				return (T*) new OPimTodoAccessVCal( filename );
				break;
			}
		case OPimGlobal::CONTACTLIST:
			switch ( use_database ){
			default: // Use SQL if something weird is given.
				// Fall through !!
			case OPimGlobal::SQL:
#ifdef __USE_SQL
				return (T*) new OPimContactAccessBackend_SQL( appName, filename );
				break;
#else
				qWarning ("OBackendFactory:: sql Backend for CONTACT not implemented! Using XML instead!");
				// Fall through !!
#endif
			case OPimGlobal::XML:
				return (T*) new OPimContactAccessBackend_XML( appName, filename );
				break;
			case OPimGlobal::VCARD:
				return (T*) new OPimContactAccessBackend_VCard( appName, filename );
				break;
			}
		case OPimGlobal::DATEBOOK:
			switch ( use_database ){
			default: // Use SQL if something weird is given.
				// Fall through !!
			case OPimGlobal::SQL:
#ifdef __USE_SQL
				return (T*) new ODateBookAccessBackend_SQL( appName, filename );
				break;
#else
				qWarning("OBackendFactory:: sql Backend for DATEBOOK not implemented! Using XML instead!");
				// Fall through !!
#endif
			case OPimGlobal::XML:
				return (T*) new ODateBookAccessBackend_XML( appName, filename );
				break;
			case OPimGlobal::VCARD:
				qWarning("OBackendFactory:: VCal Backend for DATEBOOK not implemented!");
				return (T*) NULL;
				break;
			}
		default:
			return (T*) NULL;
		}

	}

	/**
	 * Returns the style of the default database which is used to contact PIM data.
         * @param type the type of the backend
	 * @see OPimGlobal
	 */
	static OPimGlobal::DatabaseStyle defaultDB( OPimGlobal::PimType type ){
		QString group_name;
		switch ( type ){
		case OPimGlobal::TODOLIST:
			group_name = "todo";
			break;
		case OPimGlobal::CONTACTLIST:
			group_name = "contact";
			break;
		case OPimGlobal::DATEBOOK:
			group_name = "datebook";
			break;
		default:
			group_name = "unknown";
		}

		Config config( "pimaccess" );
		config.setGroup ( group_name );
		QString db_String = config.readEntry( "usebackend", "xml" );

		QAsciiDict<int> dictDbTypes( OPimGlobal::_END_DatabaseStyle );
		dictDbTypes.setAutoDelete( TRUE );

		dictDbTypes.insert( "xml", new int (OPimGlobal::XML) );
		dictDbTypes.insert( "sql", new int (OPimGlobal::SQL) );
		dictDbTypes.insert( "vcard", new int (OPimGlobal::VCARD) );

	        int* db_find = dictDbTypes[ db_String ];

		if ( !db_find )
			return OPimGlobal::UNKNOWN;

		return (OPimGlobal::DatabaseStyle) *db_find;
	}


        /**
         * Returns the default backend implementation for backendName. Which one is used, is defined
	 * by the configfile "pimaccess.conf".
         * @param type The type of the backend (@see OPimGlobal())
         * @param appName The name of your application. It will be passed on to the backend
         */
	static T* defaultBackend( OPimGlobal::PimType type, const QString& appName ){
		return create( type, OPimGlobal::DEFAULT, appName );
	}
    private:
	OBackendPrivate* d;

};

}

#endif
