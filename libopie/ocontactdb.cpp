/*
 * Class to manage the Contacts.
 *
 * Copyright (c) 2002 by Stefan Eilers (Eilers.Stefan@epost.de)
 * Copyright (C) 2000 Trolltech AS.  All rights reserved.
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
 * Version: $Id: ocontactdb.cpp,v 1.1.2.4 2002-07-06 16:06:03 eilers Exp $
 * =====================================================================
 * History:
 * $Log: ocontactdb.cpp,v $
 * Revision 1.1.2.4  2002-07-06 16:06:03  eilers
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
		bool save( const QValueList<Contact>& list ) {
			QString strNewFile = m_filename + ".new";
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
			for ( it = list.begin(); it != list.end(); ++it ) {
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
			if ( total_written != int(cstr.length()) ) {
				f.close();
				QFile::remove( strNewFile );
				return false;
			}
			f.close();
			
			// move the file over, I'm just going to use the system call
			// because, I don't feel like using QDir.
			if ( ::rename( strNewFile.latin1(), m_filename.latin1() ) < 0 ) {
				qWarning( "problem renaming file %s to %s, errno: %d",
					  strNewFile.latin1(), m_journal.latin1(), errno );
				// remove the tmp file...
				QFile::remove( strNewFile );
			}
			
			/* The journalfile should be removed now... */
			QFile::remove (m_journal);
			return true;
		}
		
		bool load( ) {
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
			
			qWarning( "OContactDefaultBackEnd::loading %s", m_filename.latin1() );
			
			/* Store the date and time when we initialized the database */
			QFileInfo fi( m_filename );
			//m_readtime = fi.lastModified ();
			
			XMLElement *root = XMLElement::load( m_filename );
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
						qWarning ("OContactDefBack::Searching for Tag \"Contacts\"! Found: %s",
							  element->tagName().latin1());
						element = element->nextChild();
					} else {
						element = element->firstChild();
						break;
					}
				}
				/* Parse all Contacts and ignore unknown tags */
				while( element ){
					if( element->tagName() != QString::fromLatin1("Contact") ){
						qWarning ("OContactDefBack::Searching for Tag \"Contact\"! Found: %s",
							  element->tagName().latin1());
						element = element->nextChild();
						continue;
					}
					/* Found alement with tagname "contact", now parse and store all
					 * attributes contained
					 */
					qWarning("OContactDefBack::load element tagName() : %s", element->tagName().latin1() );
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
							parent()->addContact (contact);
							break;
						case Contact::ACTION_REMOVE:
							parent()->removeContact (contact.uid(), contact);
							break;
						case Contact::ACTION_REPLACE:
							parent()->replaceContact (contact.uid(), contact);
							break;
						default:
							qWarning ("Unknown action: ignored !");
							break;
						}
					}else{
						/* Add contact to list */
						parent()->addContact (contact);
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
			QFile f( m_journal );
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
			QFile f ( m_journal );
			if ( !f.exists() )
				f.remove();
		}
	};
}


OContactDB::OContactDB (const QString appname,
                        const QString &fileName,
                        OContactBackend* end):
	m_loc_fileName(fileName)
{
	if ( fileName.isEmpty() ){
		m_loc_fileName = Global::applicationFileName("addressbook","addressbook.xml");
	}
	
        /* take care of the backend */
        if( end == 0 ) {
		end = new DefaultBackend();
        }
        end->m_parent = this;
        m_backEnd = end;

	/* Set journalfile name ... */
	m_loc_journalName = getenv("HOME");
	m_loc_journalName +="/.abjournal" + appname;
	m_backEnd->m_journal = m_loc_journalName;
	
	/* Load XML-File and journal if it exists */
	load (m_loc_fileName, false);
	load (m_loc_journalName, true);
}
OContactDB::~OContactDB ()
{
	delete m_backEnd;
}

/* This function does not set the local filename "m_loc_filename" due to the fact
 * that it is just used internally and therefore the filename was 
 * already set by the constructor ! (se)
 */
void OContactDB::load (const QString& filename, bool isJournal)
{
	m_loading = true;
	m_backEnd->m_filename = filename;

	/* We use the time of the last read to check if the file was
	 * changed externally.
	 */
	if ( !isJournal ){
		QFileInfo fi( filename );
		m_readtime = fi.lastModified ();
	}
	m_backEnd->load();
	m_loading = false;
}

bool OContactDB::save (bool autoreload)
{
	/* If the database was changed externally, we could not save the
	 * Data. This will remove added items which is unacceptable !
	 * Therefore: Reload database and merge the data...
	 */
	if ( isChangedExternally() && !autoreload )
		return false;
	else
		reload();
	
        bool status = m_backEnd->save(m_contactList);
	
        if ( !status ) return false;
	
	/* The journalfile is now useless */
	removeJournal ();
	
	/* Now tell everyone that new data is available */
	QCopEnvelope e( "QPE/PIM", "addressbookUpdated(QString)" );
	e << m_loc_fileName;
	
	return true;
}


bool OContactDB::isChangedExternally()
{
	QFileInfo fi( m_loc_fileName );
	
	QDateTime lastmod = fi.lastModified ();
	
	return (lastmod != m_readtime);
}


QValueList<Contact> OContactDB::allContacts()const
{
	return m_contactList;
}

bool OContactDB::addContact (const Contact &newcontact)
{
	updateJournal (newcontact, Contact::ACTION_ADD);
        m_contactList.append (newcontact);
	return true;
}

bool OContactDB::replaceContact (int uid, const Contact &contact)
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

bool OContactDB::removeContact (int uid, const Contact &contact)
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

void OContactDB::reload()
{
	m_contactList.clear();
	load (m_loc_fileName, false);
	load (m_loc_journalName, true);
}


/* Every change in the local copy of the database is
 * written into the journalfile.
 */
void OContactDB::updateJournal( const Contact &cnt,
				Contact::journal_action action)
{
	if (m_loading )
		return;
	m_backEnd->m_journal = m_loc_journalName;
	m_backEnd->updateJournal( cnt, action );
}

/* Removes the current journal-file */
void OContactDB::removeJournal()
{
	m_backEnd->removeJournal();
}
