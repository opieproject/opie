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
 * ToDo: XML-Backend: Automatic reload if something was changed...
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

#include <opie2/ocontactaccessbackend_xml.h>
#include <opie2/opimresolver.h>

namespace Opie {

OPimContactAccess::OPimContactAccess ( const QString appname, const QString ,
			 OPimContactAccessBackend* end, bool autosync ):
	OPimAccessTemplate<OPimContact>( end )
{
        /* take care of the backend. If there is no one defined, we
	 * will use the XML-Backend as default (until we have a cute SQL-Backend..).
	 */
        if( end == 0 ) {
		qWarning ("Using BackendFactory !");
		end = OBackendFactory<OPimContactAccessBackend>::Default( "contact", appname );
        }
	// Set backend locally and in template
        m_backEnd = end;
	OPimAccessTemplate<OPimContact>::setBackEnd (end);


	/* Connect signal of external db change to function */
	QCopChannel *dbchannel = new QCopChannel( "QPE/PIM", this );
	connect( dbchannel, SIGNAL(received(const QCString&,const QByteArray&)),
               this, SLOT(copMessage(const QCString&,const QByteArray&)) );
	if ( autosync ){
		QCopChannel *syncchannel = new QCopChannel( "QPE/Sync", this );
		connect( syncchannel, SIGNAL(received(const QCString&,const QByteArray&)),
			 this, SLOT(copMessage(const QCString&,const QByteArray&)) );
	}


}
OPimContactAccess::~OPimContactAccess ()
{
	/* The user may forget to save the changed database, therefore try to
	 * do it for him..
	 */
	save();
	// delete m_backEnd; is done by template..
}


bool OPimContactAccess::save ()
{
	/* If the database was changed externally, we could not save the
	 * Data. This will remove added items which is unacceptable !
	 * Therefore: Reload database and merge the data...
	 */
	if ( OPimAccessTemplate<OPimContact>::wasChangedExternally() )
		reload();

	bool status = OPimAccessTemplate<OPimContact>::save();
	if ( !status ) return false;

	/* Now tell everyone that new data is available.
	 */
	QCopEnvelope e( "QPE/PIM", "addressbookUpdated()" );

	return true;
}

const uint OPimContactAccess::querySettings()
{
	return ( m_backEnd->querySettings() );
}

bool OPimContactAccess::hasQuerySettings ( int querySettings ) const
{
	return ( m_backEnd->hasQuerySettings ( querySettings ) );
}
OPimRecordList<OPimContact> OPimContactAccess::sorted( bool ascending, int sortOrder, int sortFilter, int cat ) const
{
	QArray<int> matchingContacts = m_backEnd -> sorted( ascending, sortOrder, sortFilter, cat );
	return ( OPimRecordList<OPimContact>(matchingContacts, this) );
}


bool OPimContactAccess::wasChangedExternally()const
{
	return ( m_backEnd->wasChangedExternally() );
}


void OPimContactAccess::copMessage( const QCString &msg, const QByteArray & )
{
	if ( msg == "addressbookUpdated()" ){
		qWarning ("OPimContactAccess: Received addressbokUpdated()");
		emit signalChanged ( this );
	} else if ( msg == "flush()" ) {
		qWarning ("OPimContactAccess: Received flush()");
		save ();
	} else if ( msg == "reload()" ) {
		qWarning ("OPimContactAccess: Received reload()");
		reload ();
		emit signalChanged ( this );
	}
}

int OPimContactAccess::rtti() const
{
	return OPimResolver::AddressBook;
}

}
