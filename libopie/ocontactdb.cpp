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
 * Version: $Id: ocontactdb.cpp,v 1.1.2.21 2002-09-13 11:32:59 eilers Exp $
 * =====================================================================
 * History:
 * $Log: ocontactdb.cpp,v $
 * Revision 1.1.2.21  2002-09-13 11:32:59  eilers
 * Sorry for that..!
 *
 * Revision 1.1.2.20  2002/09/13 11:16:21  eilers
 * added prefix increment/decrement
 *
 * Revision 1.1.2.19  2002/09/12 17:01:52  eilers
 * First attempt of iterator in ocontactdb
 *
 * Revision 1.1.2.18  2002/08/31 20:22:59  zecke
 * libopie renamed to libopie-two for now
 * toMap added to ToDoEvent
 * renamed date -> dueDate
 * hasDueDate()
 * setDueDate.....
 * (Re)Implement the XML Todo
 * Iterator fixes
 *
 * Revision 1.1.2.17  2002/08/27 20:11:30  zecke
 * So nice to be able to commit again..
 *
 * Revision 1.1.2.16  2002/08/10 15:00:30  eilers
 * Improved search interface. Thanks to Zecke
 *
 * Revision 1.1.2.15  2002/08/04 12:24:30  eilers
 * It is now possible to ask the backend which kind of queries he support
 * or if a query is correct...
 *
 */

#include "ocontactdb.h"

#include <qasciidict.h>
#include <qdatetime.h>
#include <qfile.h>
#include <qregexp.h>
#include <qlist.h>
#include <qcopchannel_qws.h>

//#include <qpe/qcopenvelope_qws.h>
#include <qpe/global.h>
#include <opie/xmltree.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "ocontactdb_defbackend.h"

OContactDB::Iterator::Iterator():
	m_cur_position( 0 ),
	m_end_reached( false ),
	m_db ( NULL )
{}

OContactDB::Iterator::Iterator( const OContactDB::Iterator& copy )
{ 
	m_cur_position = copy.m_cur_position;
	m_uids = copy.m_uids;
	m_end_reached = copy.m_end_reached;
	m_db = copy.m_db;
}

OContactDB::Iterator& OContactDB::Iterator::operator= ( const OContactDB::Iterator& it )
{
	m_cur_position = it.m_cur_position;
	m_uids = it.m_uids;
	m_end_reached = it.m_end_reached;
	m_db = it.m_db;

	return *this;
}
bool OContactDB::Iterator::operator== ( const OContactDB::Iterator& it )
{
	// Hey Zecke: Ich verstehe Deine Gleichoperation nicht ! Für mich sind
	// Zwei iteratoren gleich, wenn sie die gleiche UID-Liste beinhalten,
	// auf den gleichen Eintrag zeigen und das Gleiche Backend benutzen.. (se)
	
	return ( ( m_uids == it.m_uids ) 
		 && ( m_cur_position == it.m_cur_position ) 
		 && ( m_db == it.m_db ) );
	
}

bool OContactDB::Iterator::operator!= ( const OContactDB::Iterator& it )
{
	return !( *this == it );
}

uint OContactDB::Iterator::count() const
{
	return m_uids.count();
}

OContactDB::Iterator OContactDB::Iterator::begin()
{
	Iterator begin_it;

	begin_it.m_cur_position = 0;
	begin_it.m_uids = m_uids;
	begin_it.m_end_reached = false;
	begin_it.m_db = m_db;

	return begin_it;
}

OContactDB::Iterator OContactDB::Iterator::end()
{
	Iterator end_it;

	end_it.m_cur_position = m_uids.count();
	end_it.m_uids = m_uids;
	end_it.m_end_reached = true;
	end_it.m_db = m_db;

	return end_it;
}

OContactDB::Iterator& OContactDB::Iterator::operator++ ()
{    // prefix
	if ( m_cur_position < (int) m_uids.count() ){
		m_end_reached = false;
		++m_cur_position;
	} else {
		m_end_reached = true;
	}

	return *this;
}

OContactDB::Iterator OContactDB::Iterator::operator++ ( int )
{  // postfix
	Iterator temp = *this;

	++*this;

	return temp;
}

OContactDB::Iterator& OContactDB::Iterator::operator-- ()
{    // prefix
	if ( m_cur_position >= 0 ){
		m_end_reached = false;
		--m_cur_position;
	} else {
		m_end_reached = true;
	}

	return *this;
}

OContactDB::Iterator OContactDB::Iterator::operator-- ( int )
{  // postfix
	Iterator temp = *this;

	--*this;

	return temp;
}

Contact OContactDB::Iterator::operator*() const
{
	Contact contact;

	// Returns uninitialized (empty) contact if 
	// we are out of bounds.
	if ( m_end_reached )
		return contact;

	// sets contact or let it unchanged if uid is invalid
	m_db->findContact ( contact, m_uids[m_cur_position] ); 

	return contact;
}

Contact OContactDB::Iterator::operator->() const
{
	return *(*this);
}



OContactDB::OContactDB ( const QString appname, const QString filename,
			 OContactBackend* end, bool autosync ):
	m_changed (false)
{
        /* take care of the backend */
        if( end == 0 ) {
		end = new DefaultBackend( appname, filename );
        }
        m_backEnd = end;

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
OContactDB::~OContactDB ()
{
	/* The user may forget to save the changed database, therefore try to
	 * do it for him..
	 */
	save();
	delete m_backEnd;
}

bool OContactDB::save ()
{
	/* If the database was changed externally, we could not save the
	 * Data. This will remove added items which is unacceptable !
	 * Therefore: Reload database and merge the data...
	 */
	if ( m_backEnd->isChangedExternally() )
		reload();

	/* We just want to store data if we have something to store */
	if ( m_changed ) {
		bool status = m_backEnd->save();
		if ( !status ) return false;

		/* Now tell everyone that new data is available.
		 */
		QCopEnvelope e( "QPE/PIM", "addressbookUpdated()" );

		m_changed = false;
	}

	return true;
}

bool OContactDB::findContact(Contact &foundContact, int uid )
{
	return ( m_backEnd->findContact(foundContact, uid) );
}

OContactDB::Iterator OContactDB::allContacts()
{
	Iterator it;
	it.m_db = this;
	it.m_uids = m_backEnd->allContacts();

	return it;
}

OContactDB::Iterator OContactDB::queryByExample ( const Contact &query, const Query setting )
{
	Iterator it;
	it.m_db = this;
	if ( m_backEnd->queryByExample ( query, setting ) ){
		it.m_uids = m_backEnd->allFound();
	}

	return it;
}

const uint OContactDB::querySettings()
{
	return ( m_backEnd->querySettings() );
}

bool OContactDB::hasQuerySettings ( Query querySettings ) const
{
	return ( m_backEnd->hasQuerySettings ( querySettings ) );
}

bool OContactDB::addContact ( const Contact& newcontact )
{
	m_changed = true;
	return ( m_backEnd->addContact ( newcontact ) );
}

bool OContactDB::replaceContact (int uid, const Contact &contact)
{
	m_changed = true;
	return ( m_backEnd->replaceContact ( uid, contact ) );
}

bool OContactDB::removeContact (int uid, const Contact &contact)
{
	m_changed = true;
	return ( m_backEnd->removeContact ( uid, contact ) );
}

void OContactDB::reload()
{
	m_backEnd->reload();
}

void OContactDB::copMessage( const QCString &msg, const QByteArray & )
{
	if ( msg == "addressbookUpdated()" ){
		qWarning ("OContactDB: Received addressbokUpdated()");
		emit signalChanged ( this );
	} else if ( msg == "flush()" ) {
		qWarning ("OContactDB: Received flush()");
		save ();
	} else if ( msg == "reload()" ) {
		qWarning ("OContactDB: Received reload()");
		reload ();
		emit signalChanged ( this );
	}
}
