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
 *
 * =====================================================================
 * Version: $Id: ocontactdb.cpp,v 1.1.2.14 2002-07-28 17:41:52 eilers Exp $
 * =====================================================================
 * History:
 * $Log: ocontactdb.cpp,v $
 * Revision 1.1.2.14  2002-07-28 17:41:52  eilers
 * oops..
 *
 * Revision 1.1.2.13  2002/07/28 15:35:22  eilers
 * Example-By-Query Search interface debugged. It is working now.. :)
 *
 * Revision 1.1.2.12  2002/07/24 07:01:58  eilers
 * Some minor changes. Fixed search query bug
 *
 * Revision 1.1.2.11  2002/07/21 15:21:26  eilers
 * Some interface changes and minor bugfixes...
 * The search interface is able to use wildcards, regular expressions and
 * ignore cases... I love the Trolltech cClasslibrary ! :)
 *
 * Revision 1.1.2.10  2002/07/18 13:37:53  eilers
 * Uniinitialized variable: fixed..
 *
 * Revision 1.1.2.9  2002/07/14 13:50:08  eilers
 * Interface change... REMEMBER: The search function is currently totally
 * untested !!
 *
 * Revision 1.1.2.8  2002/07/14 13:41:30  eilers
 * Some bugfixes,
 * Added interface for searching contacts in database, using "query by example"
 * like system
 *
 * Revision 1.1.2.7  2002/07/13 17:19:20  eilers
 * Added signal handling:
 * The database will be informed if it is changed externally and if flush() or
 * reload() signals sent. The application which is using the database may
 * reload manually if this happens...
 *
 * Revision 1.1.2.6  2002/07/07 16:24:47  eilers
 * All active parts moved into the backend. It should be easily possible to
 * use a database as backend
 *
 * Revision 1.1.2.5  2002/07/07 12:40:35  zecke
 * Why was this there in the first place?
 *
 * Revision 1.1.2.4  2002/07/06 16:06:03  eilers
 * Some bugfixes and cleanup of inconsistencies
 *
 * Revision 1.1.2.3  2002/07/05 13:03:30  zecke
 * Move the stuff responsible for loading and unloading
 * out of the ContactDB. This will make the switch to a real
 * database more easy.
 * Contact::insert is private so I had to workaround this 'feature'
 * CVS
 *
 * Revision 1.1.2.2  2002/07/05 11:17:19  zecke
 * Some API updates by me
 *
 * Revision 1.1.2.1  2002/07/01 16:49:46  eilers
 * First attempt for cross reference
 *
 */

#include "ocontactdb.h"

#include <qasciidict.h>
#include <qdatetime.h>
#include <qfile.h>
#include <qregexp.h>
#include <qlist.h> 

#include <qpe/qcopenvelope_qws.h>
#include <qpe/global.h>
#include <opie/xmltree.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>


using namespace Opie;

namespace {
	/* the default xml implementation */
	class DefaultBackend : public OContactBackend {
	public:
		DefaultBackend ( QString appname, QString filename = 0l ):
			m_queryValid (false)
		{
			m_appName = appname;

			/* Set journalfile name ... */
			m_journalName = getenv("HOME");
			m_journalName +="/.abjournal" + appname;

			/* Expecting to access the default filename if nothing else is set */
			if ( filename.isEmpty() ){
				m_fileName = Global::applicationFileName( "addressbook","addressbook.xml" );
			} else
				m_fileName = filename;

			/* Load Database now */
			load ();
		}

		bool save() {
			QString strNewFile = m_fileName + ".new";
			QFile f( strNewFile );
			if ( !f.open( IO_WriteOnly|IO_Raw ) )
				return false;
			
			int total_written;
			QString out;
			out = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE Addressbook ><AddressBook>\n"
				" <Groups>\n"
				" </Groups>\n"
				" <Contacts>\n";
			//QValueList<Contact>::iterator it;
			QValueListConstIterator<Contact> it;
			for ( it = m_contactList.begin(); it != m_contactList.end(); ++it ) {
				out += "<Contact ";
				(*it).save( out );
				out += "/>\n";
				QCString cstr = out.utf8();
				total_written = f.writeBlock( cstr.data(), cstr.length() );
				if ( total_written != int(cstr.length()) ) {
					f.close();
					QFile::remove( strNewFile );
					return false;
				}
				out = "";
			}
			out += " </Contacts>\n</AddressBook>\n";
			
			QCString cstr = out.utf8();
			total_written = f.writeBlock( cstr.data(), cstr.length() );
			if ( total_written != int( cstr.length() ) ) {
				f.close();
				QFile::remove( strNewFile );
				return false;
			}
			f.close();
			
			// move the file over, I'm just going to use the system call
			// because, I don't feel like using QDir.
			if ( ::rename( strNewFile.latin1(), m_fileName.latin1() ) < 0 ) {
				qWarning( "problem renaming file %s to %s, errno: %d",
					  strNewFile.latin1(), m_journalName.latin1(), errno );
				// remove the tmp file...
				QFile::remove( strNewFile );
			}
			
			/* The journalfile should be removed now... */
			removeJournal();
			return true;
		}
		
		bool load () {
			m_queryValid = false;
			m_contactList.clear();	

			/* Load XML-File and journal if it exists */
			if ( !load ( m_fileName, false ) )
				return false;
			/* The returncode of the journalfile is ignored due to the
			 * fact that it does not exist when this class is instantiated !
			 * But there may such a file exist, if the application crashed.
			 * Therefore we try to load it to get the changes before the #
			 * crash happened...
			 */
			load (m_journalName, true);

			return true;
		}

		bool isChangedExternally()
		{
			QFileInfo fi( m_fileName );
	
			QDateTime lastmod = fi.lastModified ();
	
			return (lastmod != m_readtime);
		}

		QValueList<Contact> allContacts() const {
			return (m_contactList);
		}

		bool findContact(Contact &foundContact, int uid )
		{
			bool found = false;
			QValueListConstIterator<Contact> it;
			for( it = m_contactList.begin(); it != m_contactList.end(); ++it ){
				if ((*it).uid() == uid){
					found = true;
					break;
				}
			}
			if ( found ){
				foundContact = *it;
				return ( true );
			}

			return ( false );
		}
		
		const Contact *queryByExample ( const Contact &query, const uint settings ){
			m_currentQuery.setAutoDelete( false );
			m_currentQuery.clear();

			QValueListConstIterator<Contact> it;
			for( it = m_contactList.begin(); it != m_contactList.end(); ++it ){
				/* Search all fields and compare them with query object. Store pointer into list
				 * if all fields matches. 
				 */
				bool allcorrect = true;
				for ( int i = 0; i < Qtopia::rid; i++ ) {
					/* Just compare fields which are not empty in the query object */
					if ( !query.field(i).isEmpty() ){
						switch ( settings & ~OContactDB::query_IgnoreCase ){
						case OContactDB::query_RegExp:{
							QRegExp expr ( query.field(i), 
								       !(settings & OContactDB::query_IgnoreCase), 
								       false );
							if ( expr.find ( (*it).field(i), 0 ) == -1 )
								allcorrect = false;
						}
							break;
						case OContactDB::query_WildCards:{
							QRegExp expr ( query.field(i), 
								       !(settings & OContactDB::query_IgnoreCase), 
								       true );
							if ( expr.find ( (*it).field(i), 0 ) == -1 )
								allcorrect = false;
						}
							break;
						case OContactDB::query_ExactMatch:{
							if (settings & OContactDB::query_IgnoreCase){
								if ( query.field(i).upper() != 
								     (*it).field(i).upper() )
									allcorrect = false;
							}else{
								if ( query.field(i) != (*it).field(i) )
									allcorrect = false;
							}
						}
							break;
						}
					}
				}
				if ( allcorrect )
					m_currentQuery.append( &(*it) );
			}
			
			/* Move to the top of the list and set this query valid */
			
			m_queryValid = true;
			return m_currentQuery.first();
		}

		const Contact *nextFound ()
		{
			if ( m_queryValid )
				return ( m_currentQuery.next() );
			else
				return NULL;
		}

		bool addContact ( const Contact &newcontact )
		{
			updateJournal (newcontact, Contact::ACTION_ADD);
			m_contactList.append (newcontact);
			return true;
		}

		bool replaceContact (int uid, const Contact &contact)
		{
			bool found = false;
			QValueListIterator<Contact> it;
			for( it = m_contactList.begin(); it != m_contactList.end(); ++it ){
				if ((*it).uid() == uid){
					found = true;
					break;
				}
			}
			if (found) {
				updateJournal (contact, Contact::ACTION_REPLACE);
				m_contactList.remove (it);
				m_contactList.append (contact);
				return true;
			} else
				return false;
		}

		bool removeContact (int uid, const Contact &contact)
		{
			bool found = false;
			QValueListIterator<Contact> it;
			for( it = m_contactList.begin(); it != m_contactList.end(); ++it ){
				if ((*it).uid() == uid){
					found = true;
					break;
				}
			}
			if (found) {
				updateJournal (contact, Contact::ACTION_REMOVE);
				m_contactList.remove (it);
				return true;
			} else
				return false;
		}
		
		void reload(){
			/* Reload is the same as load in this implementation */
			m_queryValid = false;
			load();
		}

	private:
		/* This function loads the xml-database and the journalfile */
		bool load( const QString filename, bool isJournal ) {

			/* We use the time of the last read to check if the file was
			 * changed externally.
			 */
			if ( !isJournal ){
				QFileInfo fi( filename );
				m_readtime = fi.lastModified ();
			}

			const int JOURNALACTION = Qtopia::Notes + 1;
			const int JOURNALROW = JOURNALACTION + 1;
			
			bool foundAction = false;
			Contact::journal_action action = Contact::ACTION_ADD;
			int journalKey = 0;
			QMap<int, QString> contactMap;
			QMap<QString, QString> customMap;
			QMap<QString, QString>::Iterator customIt;
			QAsciiDict<int> dict( 47 );
			
			dict.setAutoDelete( TRUE );
			dict.insert( "Uid", new int(Qtopia::AddressUid) );
			dict.insert( "Title", new int(Qtopia::Title) );
			dict.insert( "FirstName", new int(Qtopia::FirstName) );
			dict.insert( "MiddleName", new int(Qtopia::MiddleName) );
			dict.insert( "LastName", new int(Qtopia::LastName) );
			dict.insert( "Suffix", new int(Qtopia::Suffix) );
			dict.insert( "FileAs", new int(Qtopia::FileAs) );
			dict.insert( "Categories", new int(Qtopia::AddressCategory) );
			dict.insert( "DefaultEmail", new int(Qtopia::DefaultEmail) );
			dict.insert( "Emails", new int(Qtopia::Emails) );
			dict.insert( "HomeStreet", new int(Qtopia::HomeStreet) );
			dict.insert( "HomeCity", new int(Qtopia::HomeCity) );
			dict.insert( "HomeState", new int(Qtopia::HomeState) );
			dict.insert( "HomeZip", new int(Qtopia::HomeZip) );
			dict.insert( "HomeCountry", new int(Qtopia::HomeCountry) );
			dict.insert( "HomePhone", new int(Qtopia::HomePhone) );
			dict.insert( "HomeFax", new int(Qtopia::HomeFax) );
			dict.insert( "HomeMobile", new int(Qtopia::HomeMobile) );
			dict.insert( "HomeWebPage", new int(Qtopia::HomeWebPage) );
			dict.insert( "Company", new int(Qtopia::Company) );
			dict.insert( "BusinessStreet", new int(Qtopia::BusinessStreet) );
			dict.insert( "BusinessCity", new int(Qtopia::BusinessCity) );
			dict.insert( "BusinessState", new int(Qtopia::BusinessState) );
			dict.insert( "BusinessZip", new int(Qtopia::BusinessZip) );
			dict.insert( "BusinessCountry", new int(Qtopia::BusinessCountry) );
			dict.insert( "BusinessWebPage", new int(Qtopia::BusinessWebPage) );
			dict.insert( "JobTitle", new int(Qtopia::JobTitle) );
			dict.insert( "Department", new int(Qtopia::Department) );
			dict.insert( "Office", new int(Qtopia::Office) );
			dict.insert( "BusinessPhone", new int(Qtopia::BusinessPhone) );
			dict.insert( "BusinessFax", new int(Qtopia::BusinessFax) );
			dict.insert( "BusinessMobile", new int(Qtopia::BusinessMobile) );
			dict.insert( "BusinessPager", new int(Qtopia::BusinessPager) );
			dict.insert( "Profession", new int(Qtopia::Profession) );
			dict.insert( "Assistant", new int(Qtopia::Assistant) );
			dict.insert( "Manager", new int(Qtopia::Manager) );
			dict.insert( "Spouse", new int(Qtopia::Spouse) );
			dict.insert( "Children", new int(Qtopia::Children) );
			dict.insert( "Gender", new int(Qtopia::Gender) );
			dict.insert( "Birthday", new int(Qtopia::Birthday) );
			dict.insert( "Anniversary", new int(Qtopia::Anniversary) );
			dict.insert( "Nickname", new int(Qtopia::Nickname) );
			dict.insert( "Notes", new int(Qtopia::Notes) );
			dict.insert( "action", new int(JOURNALACTION) );
			dict.insert( "actionrow", new int(JOURNALROW) );
			
			qWarning( "OContactDefaultBackEnd::loading %s", m_fileName.latin1() );
						
			XMLElement *root = XMLElement::load( filename );
			if(root != 0l ){ // start parsing
				/* Parse all XML-Elements and put the data into the
				 * Contact-Class
				 */
				XMLElement *element = root->firstChild();
				// qWarning("OContactDB::load tagName(): %s", root->tagName().latin1() );
				element = element->firstChild();
				
				/* Search Tag "Contacts" which is the parent of all Contacts */
				while( element ){
					if( element->tagName() != QString::fromLatin1("Contacts") ){
						// qWarning ("OContactDefBack::Searching for Tag \"Contacts\"! Found: %s",
						//	  element->tagName().latin1());
						element = element->nextChild();
					} else {
						element = element->firstChild();
						break;
					}
				}
				/* Parse all Contacts and ignore unknown tags */
				while( element ){
					if( element->tagName() != QString::fromLatin1("Contact") ){
						//qWarning ("OContactDefBack::Searching for Tag \"Contact\"! Found: %s",
						//	  element->tagName().latin1());
						element = element->nextChild();
						continue;
					}
					/* Found alement with tagname "contact", now parse and store all
					 * attributes contained
					 */
					//qWarning("OContactDefBack::load element tagName() : %s", 
					//	 element->tagName().latin1() );
					QString dummy;
					foundAction = false;
					
					XMLElement::AttributeMap aMap = element->attributes();
					XMLElement::AttributeMap::Iterator it;
					contactMap.clear();
					customMap.clear();
					for( it = aMap.begin(); it != aMap.end(); ++it ){
						// qWarning ("Read Attribute: %s=%s", it.key().latin1(),it.data().latin1());
						
						int *find = dict[ it.key() ];
						/* Unknown attributes will be stored as "Custom" elements */
						if ( !find ) {
							qWarning("Attribute %s not known.", it.key().latin1());
							//contact.setCustomField(it.key(), it.data());
							customMap.insert( it.key(),  it.data() );
							continue;
						}
						
						/* Check if special conversion is needed and add attribute
						 * into Contact class
						 */
						switch( *find ) {
							/*
							  case Qtopia::AddressUid:
							  contact.setUid( it.data().toInt() );
							  break;
							  case Qtopia::AddressCategory:
							  contact.setCategories( Qtopia::Record::idsFromString( it.data( )));
							  break;
							*/
						case JOURNALACTION:
							action = Contact::journal_action(it.data().toInt());
							foundAction = true;
							break;
						case JOURNALROW:
							journalKey = it.data().toInt();
							break;
						default: // no conversion needed add them to the map
							contactMap.insert( *find, it.data() );
							break;
						}
					}
					/* now generate the Contact contact */
					Contact contact( contactMap );
					
					for (customIt = customMap.begin(); customIt != customMap.end(); ++customIt ) {
						contact.setCustomField( customIt.key(),  customIt.data() );
					}
					
					if (foundAction){
						foundAction = false;
						switch ( action ) {
						case Contact::ACTION_ADD:
							addContact (contact);
							break;
						case Contact::ACTION_REMOVE:
							removeContact (contact.uid(), contact);
							break;
						case Contact::ACTION_REPLACE:
							replaceContact (contact.uid(), contact);
							break;
						default:
							qWarning ("Unknown action: ignored !");
							break;
						}
					}else{
						/* Add contact to list */
						addContact (contact);
					}
					
					/* Move to next element */
					element = element->nextChild();
				}
			}else {
				qWarning("ODefBack::could not load");
			}
			delete root;
			qWarning("returning from loading" );
			return true;
		}
		

		void updateJournal( const Contact& cnt,
				    Contact::journal_action action ) {
			QFile f( m_journalName );
			if ( !f.open(IO_WriteOnly|IO_Append) )
				return;
			QString buf;
			QCString str;
			buf = "<Contact ";
			cnt.save( buf );
			buf += " action=\"" + QString::number( (int)action ) + "\" ";
			buf += "/>\n";
			QCString cstr = buf.utf8();
			f.writeBlock( cstr.data(), cstr.length() );
		}
		
		void removeJournal()
		{
			QFile f ( m_journalName );
			if ( !f.exists() )
				f.remove();
		}

	protected:
		QString m_journalName;
		QString m_fileName;
		QString m_appName;
		QValueList<Contact> m_contactList;
		QList<Contact> m_currentQuery;
		bool m_queryValid;
		QDateTime m_readtime;
	};
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
	
QValueList<Contact> OContactDB::allContacts() const
{
	return ( m_backEnd->allContacts() );
}

const Contact *OContactDB::queryByExample ( const Contact &query, const uint setting )
{
	return ( m_backEnd->queryByExample ( query, setting ) );
}

const Contact *OContactDB::nextFound ()
{
	return ( m_backEnd->nextFound () ); 
}

bool OContactDB::addContact ( const Contact &newcontact )
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
