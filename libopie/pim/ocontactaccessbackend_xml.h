/*
 * XML Backend for the OPIE-Contact Database.
 *
 * Copyright (c) 2002 by Stefan Eilers (Eilers.Stefan@epost.de)
 *
 * =====================================================================
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU Library General Public
 *      License as published by the Free Software Foundation; either
 *      version 2 of the License, or (at your option) any later version.
 * =====================================================================
 * ToDo: XML-Backend: Automatic reload if something was changed...
 *
 *
 * =====================================================================
 * Version: $Id: ocontactaccessbackend_xml.h,v 1.6 2002-11-13 14:14:51 eilers Exp $
 * =====================================================================
 * History:
 * $Log: ocontactaccessbackend_xml.h,v $
 * Revision 1.6  2002-11-13 14:14:51  eilers
 * Added sorted for Contacts..
 *
 * Revision 1.5  2002/11/01 15:10:42  eilers
 * Added regExp-search in database for all fields in a contact.
 *
 * Revision 1.4  2002/10/16 10:52:40  eilers
 * Added some docu to the interface and now using the cache infrastucture by zecke.. :)
 *
 * Revision 1.3  2002/10/14 16:21:54  eilers
 * Some minor interface updates
 *
 * Revision 1.2  2002/10/07 17:34:24  eilers
 * added OBackendFactory for advanced backend access
 *
 * Revision 1.1  2002/09/27 17:11:44  eilers
 * Added API for accessing the Contact-Database ! It is compiling, but
 * please do not expect that anything is working !
 * I will debug that stuff in the next time ..
 * Please read README_COMPILE for compiling !
 *
 *
 */

#ifndef _OContactAccessBackend_XML_
#define _OContactAccessBackend_XML_

#include <qasciidict.h>
#include <qdatetime.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qregexp.h>
#include <qarray.h>
#include <qmap.h>

#include <qpe/global.h>

#include <opie/xmltree.h>
#include "ocontactaccessbackend.h"
#include "ocontactaccess.h"

#include <stdlib.h>
#include <errno.h>

using namespace Opie;

/* the default xml implementation */
class OContactAccessBackend_XML : public OContactAccessBackend {
 public:
	OContactAccessBackend_XML ( QString appname, QString filename = 0l ):
		m_changed( false )
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

		if ( !m_changed )
			return true;

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
		QValueListConstIterator<OContact> it;
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

		m_changed = false;
		return true;
	}
	
	bool load () {
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

	void clear () {
		m_contactList.clear();
		m_changed = false;

	}
	
	bool wasChangedExternally()
		{
			QFileInfo fi( m_fileName );
			
			QDateTime lastmod = fi.lastModified ();
			
			return (lastmod != m_readtime);
		}
	
	QArray<int> allRecords() const {
		QArray<int> uid_list( m_contactList.count() );
		
		uint counter = 0; 
		QValueListConstIterator<OContact> it;
		for( it = m_contactList.begin(); it != m_contactList.end(); ++it ){
			uid_list[counter++] = (*it).uid();
		}
		
		return ( uid_list );
	}
	
	OContact find ( int uid ) const
		{
			bool found = false;
			OContact foundContact; //Create empty contact

			QValueListConstIterator<OContact> it;
			for( it = m_contactList.begin(); it != m_contactList.end(); ++it ){
				if ((*it).uid() == uid){
					found = true;
					break;
				}
			}
			if ( found ){
				foundContact = *it;
			}
			
			return ( foundContact );
		}
	
	QArray<int> queryByExample ( const OContact &query, int settings ){
		
		QArray<int> m_currentQuery( m_contactList.count() );
		QValueListConstIterator<OContact> it;
		uint arraycounter = 0;
		
		for( it = m_contactList.begin(); it != m_contactList.end(); ++it ){
			/* Search all fields and compare them with query object. Store them into list
			 * if all fields matches.
			 */
			bool allcorrect = true;
			for ( int i = 0; i < Qtopia::rid; i++ ) {
				/* Just compare fields which are not empty in the query object */
				if ( !query.field(i).isEmpty() ){
					switch ( settings & ~OContactAccess::IgnoreCase ){
					case OContactAccess::RegExp:{
						QRegExp expr ( query.field(i),
							       !(settings & OContactAccess::IgnoreCase),
							       false );
						if ( expr.find ( (*it).field(i), 0 ) == -1 )
							allcorrect = false;
					}
						break;
					case OContactAccess::WildCards:{
						QRegExp expr ( query.field(i),
							       !(settings & OContactAccess::IgnoreCase),
							       true );
						if ( expr.find ( (*it).field(i), 0 ) == -1 )
							allcorrect = false;
					}
						break;
					case OContactAccess::ExactMatch:{
						if (settings & OContactAccess::IgnoreCase){
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
			if ( allcorrect ){
				m_currentQuery[arraycounter++] = (*it).uid();
			}
		}
			
		// Shrink to fit..
		m_currentQuery.resize(arraycounter);
	
		return m_currentQuery;
	}

	QArray<int> matchRegexp(  const QRegExp &r ) const{
		QArray<int> m_currentQuery( m_contactList.count() );
		QValueListConstIterator<OContact> it;
		uint arraycounter = 0;
		
		for( it = m_contactList.begin(); it != m_contactList.end(); ++it ){
			if ( (*it).match( r ) ){
				m_currentQuery[arraycounter++] = (*it).uid();
			}
			
		}
		// Shrink to fit..
		m_currentQuery.resize(arraycounter);

		return m_currentQuery;
	}
	
	const uint querySettings()
		{
			return ( OContactAccess::WildCards
				 & OContactAccess::IgnoreCase
				 & OContactAccess::RegExp
				 & OContactAccess::ExactMatch );
		}
	
	bool hasQuerySettings (uint querySettings) const
		{
			/* OContactAccess::IgnoreCase may be added with one
			 * of the other settings, but never used alone.
			 * The other settings are just valid alone...
			 */
			switch ( querySettings & ~OContactAccess::IgnoreCase ){
			case OContactAccess::RegExp:
				return ( true );
			case OContactAccess::WildCards:
				return ( true );
			case OContactAccess::ExactMatch:
				return ( true );
			default:
				return ( false );
			}
		}
	
	// Currently only asc implemented.. 
	QArray<int> sorted( bool asc,  int , int ,  int ) 
		{
			QMap<QString, int> nameToUid;
			QStringList names;
			QArray<int> m_currentQuery( m_contactList.count() );

			// First fill map and StringList with all Names ( better LastNames ? )
			// Afterwards sort namelist and use map to fill array to return..
			QValueListConstIterator<OContact> it;
			for( it = m_contactList.begin(); it != m_contactList.end(); ++it ){
				names.append( (*it).lastName() );
				nameToUid.insert( (*it).lastName(), (*it).uid() );
			}
			names.sort();

			int i = 0;
			if ( asc ){
				for ( QStringList::Iterator it = names.begin(); it != names.end(); ++it )
					m_currentQuery[i++] = nameToUid[ (*it) ];
			}else{
				for ( QStringList::Iterator it = names.end(); it != names.begin(); --it )
					m_currentQuery[i++] = nameToUid[ (*it) ];
			}
			
			return m_currentQuery;
			
		}
	bool add ( const OContact &newcontact )
		{
			//qWarning("odefaultbackend: ACTION::ADD");
			updateJournal (newcontact, OContact::ACTION_ADD);
			addContact_p( newcontact );
			
			m_changed = true;

			return true;
		}
	
	bool replace ( const OContact &contact )
		{
			m_changed = true;

			bool found = false;
			
			QValueListIterator<OContact> it;
			for( it = m_contactList.begin(); it != m_contactList.end(); ++it ){
				if ( (*it).uid() == contact.uid() ){
					found = true;
					break;
				}
			}
			if (found) {
				updateJournal (contact, OContact::ACTION_REPLACE);
				m_contactList.remove (it);
				m_contactList.append (contact);
				return true;
			} else
				return false;
		}
	
	bool remove ( int uid )
		{
			m_changed = true;

			bool found = false;
			QValueListIterator<OContact> it;
			for( it = m_contactList.begin(); it != m_contactList.end(); ++it ){
				if ((*it).uid() == uid){
					found = true;
					break;
				}
			}
			if (found) {
				updateJournal ( *it, OContact::ACTION_REMOVE);
				m_contactList.remove (it);
				return true;
			} else
				return false;
		}
	
	bool reload(){
		/* Reload is the same as load in this implementation */
		return ( load() );
	}
	
 private:
	void addContact_p( const OContact &newcontact ){
		m_contactList.append (newcontact);
	}
	
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
		OContact::journal_action action = OContact::ACTION_ADD;
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
		
		//qWarning( "OContactDefaultBackEnd::loading %s", filename.latin1() );
		
		XMLElement *root = XMLElement::load( filename );
		if(root != 0l ){ // start parsing
			/* Parse all XML-Elements and put the data into the
			 * Contact-Class
			 */
			XMLElement *element = root->firstChild();
			//qWarning("OContactAccess::load tagName(): %s", root->tagName().latin1() );
			element = element->firstChild();
			
			/* Search Tag "Contacts" which is the parent of all Contacts */
			while( element && !isJournal ){
				if( element->tagName() != QString::fromLatin1("Contacts") ){
					//qWarning ("OContactDefBack::Searching for Tag \"Contacts\"! Found: %s",
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
						action = OContact::journal_action(it.data().toInt());
						foundAction = true;
						qWarning ("ODefBack(journal)::ACTION found: %d", action);
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
				OContact contact( contactMap );
				
				for (customIt = customMap.begin(); customIt != customMap.end(); ++customIt ) {
					contact.setCustomField( customIt.key(),  customIt.data() );
				}
				
				if (foundAction){
					foundAction = false;
					switch ( action ) {
					case OContact::ACTION_ADD:
						addContact_p (contact);
						break;
					case OContact::ACTION_REMOVE:
						if ( !remove (contact.uid()) )
							qWarning ("ODefBack(journal)::Unable to remove uid: %d",
								  contact.uid() );
						break;
					case OContact::ACTION_REPLACE:
						if ( !replace ( contact ) )
							qWarning ("ODefBack(journal)::Unable to replace uid: %d",
								  contact.uid() );
						break;
					default:
						qWarning ("Unknown action: ignored !");
						break;
					}
				}else{
					/* Add contact to list */
					addContact_p (contact);
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
	
	
	void updateJournal( const OContact& cnt,
			    OContact::journal_action action ) {
		QFile f( m_journalName );
		bool created = !f.exists();
		if ( !f.open(IO_WriteOnly|IO_Append) )
			return;
		
		QString buf;
		QCString str;
		
		// if the file was created, we have to set the Tag "<CONTACTS>" to
		// get a XML-File which is readable by our parser.
		// This is just a cheat, but better than rewrite the parser.
		if ( created ){
			buf = "<Contacts>";
			QCString cstr = buf.utf8();
			f.writeBlock( cstr.data(), cstr.length() );
		}
		
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
			if ( f.exists() )
				f.remove();
		}
	
 protected:
	bool m_changed;
	QString m_journalName;
	QString m_fileName;
	QString m_appName;
	QValueList<OContact> m_contactList;
	QDateTime m_readtime;
};

#endif
