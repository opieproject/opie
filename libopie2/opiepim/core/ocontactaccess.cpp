/*
 * Class to manage the Contacts.
 *
 * Copyright (c) 2002 by Stefan Eilers (Eilers.Stefan@epost.de)
 *
 * =====================================================================
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU Library General Public
 *      License as published by the Free Software Foundation; either
 *      version 2 of the License, or (at your option) any later version.
 * =====================================================================
 * Info: This class could just work with a change in the header-file
 *       of the Contact class ! Therefore our libopie only compiles
 *       with our version of libqpe
 * =====================================================================
 * ToDo: XML-Backend: Automatic reload if something was changed...
 *
 *
 * =====================================================================
 * Version: $Id: ocontactaccess.cpp,v 1.6 2002-11-01 15:10:42 eilers Exp $
 * =====================================================================
 * History:
 * $Log: ocontactaccess.cpp,v $
 * Revision 1.6  2002-11-01 15:10:42  eilers
 * Added regExp-search in database for all fields in a contact.
 *
 * Revision 1.5  2002/10/16 10:52:40  eilers
 * Added some docu to the interface and now using the cache infrastucture by zecke.. :)
 *
 * Revision 1.4  2002/10/14 16:21:54  eilers
 * Some minor interface updates
 *
 * Revision 1.3  2002/10/07 17:34:24  eilers
 * added OBackendFactory for advanced backend access
 *
 * Revision 1.2  2002/10/02 16:18:11  eilers
 * debugged and seems to work almost perfectly ..
 *
 * Revision 1.1  2002/09/27 17:11:44  eilers
 * Added API for accessing the Contact-Database ! It is compiling, but
 * please do not expect that anything is working !
 * I will debug that stuff in the next time ..
 * Please read README_COMPILE for compiling !
 *
 *
 */

#include "ocontactaccess.h"
#include "obackendfactory.h"

#include <qasciidict.h>
#include <qdatetime.h>
#include <qfile.h>
#include <qregexp.h>
#include <qlist.h>
#include <qcopchannel_qws.h>

//#include <qpe/qcopenvelope_qws.h>
#include <qpe/global.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "ocontactaccessbackend_xml.h"


OContactAccess::OContactAccess ( const QString appname, const QString ,
			 OContactAccessBackend* end, bool autosync ):
	OPimAccessTemplate<OContact>( end )
{
        /* take care of the backend. If there is no one defined, we 
	 * will use the XML-Backend as default (until we have a cute SQL-Backend..).
	 */
        if( end == 0 ) {
		qWarning ("Using BackendFactory !");
		end = OBackendFactory<OContactAccessBackend>::Default( "contact", appname );
        }
	// Set backend locally and in template
        m_backEnd = end;
	OPimAccessTemplate<OContact>::setBackEnd (end);
	

	/* Connect signal of external db change to function */
	QCopChannel *dbchannel = new QCopChannel( "QPE/PIM", this );
	connect( dbchannel, SIGNAL(received(const QCString &, const QByteArray &)),
               this, SLOT(copMessage( const QCString &, const QByteArray &)) );
	if ( autosync ){
		QCopChannel *syncchannel = new QCopChannel( "QPE/Sync", this );
		connect( syncchannel, SIGNAL(received(const QCString &, const QByteArray &)),
			 this, SLOT(copMessage( const QCString &, const QByteArray &)) );
	}


}
OContactAccess::~OContactAccess ()
{
	/* The user may forget to save the changed database, therefore try to
	 * do it for him..
	 */
	save();
	// delete m_backEnd; is done by template..
}


bool OContactAccess::save ()
{
	/* If the database was changed externally, we could not save the
	 * Data. This will remove added items which is unacceptable !
	 * Therefore: Reload database and merge the data...
	 */
	if ( OPimAccessTemplate<OContact>::wasChangedExternally() )
		reload();

	bool status = OPimAccessTemplate<OContact>::save();
	if ( !status ) return false;

	/* Now tell everyone that new data is available.
	 */
	QCopEnvelope e( "QPE/PIM", "addressbookUpdated()" );

	return true;
}

ORecordList<OContact> OContactAccess::matchRegexp(  const QRegExp &r ) const{
	QArray<int> matchingContacts = m_backEnd -> matchRegexp( r );
	return ( ORecordList<OContact>(matchingContacts, this) );
}

const uint OContactAccess::querySettings()
{
	return ( m_backEnd->querySettings() );
}

bool OContactAccess::hasQuerySettings ( int querySettings ) const
{
	return ( m_backEnd->hasQuerySettings ( querySettings ) );
}


bool OContactAccess::wasChangedExternally()const
{
	return ( m_backEnd->wasChangedExternally() );
}


void OContactAccess::copMessage( const QCString &msg, const QByteArray & )
{
	if ( msg == "addressbookUpdated()" ){
		qWarning ("OContactAccess: Received addressbokUpdated()");
		emit signalChanged ( this );
	} else if ( msg == "flush()" ) {
		qWarning ("OContactAccess: Received flush()");
		save ();
	} else if ( msg == "reload()" ) {
		qWarning ("OContactAccess: Received reload()");
		reload ();
		emit signalChanged ( this );
	}
}
