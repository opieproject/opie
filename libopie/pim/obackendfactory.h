/*
 * Class to manage Backends.
 *
 * Copyright (c) 2002 by Stefan Eilers (Eilers.Stefan@epost.de)
 *
 * =====================================================================
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU Library General Public
 *      License as published by the Free Software Foundation;
 *      either version 2 of the License, or (at your option) any later
 *      version.
 * =====================================================================
 * ToDo: Use plugins
 * =====================================================================
 * Version: $Id: obackendfactory.h,v 1.9 2003-12-22 10:19:26 eilers Exp $
 * =====================================================================
 * History:
 * $Log: obackendfactory.h,v $
 * Revision 1.9  2003-12-22 10:19:26  eilers
 * Finishing implementation of sql-backend for datebook. But I have to
 * port the PIM datebook application to use it, before I could debug the
 * whole stuff.
 * Thus, PIM-Database backend is finished, but highly experimental. And some
 * parts are still generic. For instance, the "queryByExample()" methods are
 * not (or not fully) implemented. Todo: custom-entries not stored.
 * The big show stopper: matchRegExp() (needed by OpieSearch) needs regular
 * expression search in the database, which is not supported by sqlite !
 * Therefore we need either an extended sqlite or a workaround which would
 * be very slow and memory consuming..
 *
 * Revision 1.8  2003/09/22 14:31:16  eilers
 * Added first experimental incarnation of sql-backend for addressbook.
 * Some modifications to be able to compile the todo sql-backend.
 * A lot of changes fill follow...
 *
 * Revision 1.7  2003/08/01 12:30:16  eilers
 * Merging changes from BRANCH_1_0 to HEAD
 *
 * Revision 1.6.4.1  2003/06/30 14:34:19  eilers
 * Patches from Zecke:
 * Fixing and cleaning up extraMap handling
 * Adding d_ptr for binary compatibility in the future
 *
 * Revision 1.6  2003/04/13 18:07:10  zecke
 * More API doc
 * QString -> const QString&
 * QString = 0l -> QString::null
 *
 * Revision 1.5  2003/02/21 23:31:52  zecke
 * Add XML datebookresource
 * -clean up todoaccessxml header
 * -implement some more stuff in the oeven tester
 * -extend DefaultFactory to not crash and to use datebook
 *
 * -reading of OEvents is working nicely.. saving will be added
 *  tomorrow
 *  -fix spelling in ODateBookAcces
 *
 * Revision 1.4  2002/10/14 15:55:18  eilers
 * Redeactivate SQL.. ;)
 *
 * Revision 1.3  2002/10/10 17:08:58  zecke
 * The Cache is finally in place
 * I tested it with my todolist and it 'works' for 10.000 todos the hits are awesome ;)
 * The read ahead functionality does not make sense for XMLs backends because most of the stuff is already in memory. While using readahead on SQL makes things a lot faster....
 * I still have to fully implement read ahead
 * This change is bic but sc
 *
 * Revision 1.2  2002/10/08 09:27:36  eilers
 * Fixed libopie.pro to include the new pim-API.
 * The SQL-Stuff is currently deactivated. Otherwise everyone who wants to
 * compile itself would need to install libsqlite, libopiesql...
 * Therefore, the backend currently uses XML only..
 *
 * Revision 1.1  2002/10/07 17:35:01  eilers
 * added OBackendFactory for advanced backend access
 *
 *
 * =====================================================================
 */
#ifndef OPIE_BACKENDFACTORY_H_
#define OPIE_BACKENDFACTORY_H_

#include <qstring.h>
#include <qasciidict.h>
#include <qpe/config.h>

#include "otodoaccessxml.h"
#include "ocontactaccessbackend_xml.h"
#include "odatebookaccessbackend_xml.h"

#ifdef __USE_SQL
#include "otodoaccesssql.h"
#include "ocontactaccessbackend_sql.h"
#include "odatebookaccessbackend_sql.h"
#endif

class OBackendPrivate;

/**
 * This class is our factory. It will give us the default implementations
 * of at least Todolist, Contacts and Datebook. In the future this class will
 * allow users to switch the backend with ( XML->SQLite ) without the need
 * to recompile.#
 * This class as the whole PIM Api is making use of templates
 *
 * <pre>
 *   OTodoAccessBackend* backend = OBackEndFactory<OTodoAccessBackend>::Default("todo", QString::null );
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
				return (T*) new OTodoAccessBackendSQL("");
#else
			if ( backend == "sql" )
				qWarning ("OBackendFactory:: sql Backend for TODO not implemented! Using XML instead!");
#endif

			return (T*) new OTodoAccessXML( appName );
		case CONTACT:
#ifdef __USE_SQL
			if ( backend == "sql" )
				return (T*) new OContactAccessBackend_SQL("");
#else
			if ( backend == "sql" )
				qWarning ("OBackendFactory:: sql Backend for CONTACT not implemented! Using XML instead!");
#endif

			return (T*) new OContactAccessBackend_XML( appName );
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


#endif
