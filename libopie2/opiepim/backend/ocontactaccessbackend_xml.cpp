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
 * Version: $Id: ocontactaccessbackend_xml.cpp,v 1.8 2003-08-30 15:28:26 eilers Exp $
 * =====================================================================
 * History:
 * $Log: ocontactaccessbackend_xml.cpp,v $
 * Revision 1.8  2003-08-30 15:28:26  eilers
 * Removed some unimportant debug output which causes slow down..
 *
 * Revision 1.7  2003/08/01 12:30:16  eilers
 * Merging changes from BRANCH_1_0 to HEAD
 *
 * Revision 1.6  2003/07/07 16:19:47  eilers
 * Fixing serious bug in hasQuerySettings()
 *
 * Revision 1.5  2003/04/13 18:07:10  zecke
 * More API doc
 * QString -> const QString&
 * QString = 0l -> QString::null
 *
 * Revision 1.4  2003/03/21 14:32:54  mickeyl
 * g++ compliance fix: default arguments belong into the declaration, but not the definition
 *
 * Revision 1.3  2003/03/21 12:26:28  eilers
 * Fixing small bug: If we search a birthday from today to today, it returned
 * every contact ..
 *
 * Revision 1.2  2003/03/21 10:33:09  eilers
 * Merged speed optimized xml backend for contacts to main.
 * Added QDateTime to querybyexample. For instance, it is now possible to get
 * all Birthdays/Anniversaries between two dates. This should be used
 * to show all birthdays in the datebook..
 * This change is sourcecode backward compatible but you have to upgrade
 * the binaries for today-addressbook.
 *
 * Revision 1.1.2.2  2003/02/11 12:17:28  eilers
 * Speed optimization. Removed the sequential search loops.
 *
 * Revision 1.1.2.1  2003/02/10 15:31:38  eilers
 * Writing offsets to debug output..
 *
 * Revision 1.1  2003/02/09 15:05:01  eilers
 * Nothing happened.. Just some cleanup before I will start..
 *
 * Revision 1.12  2003/01/03 16:58:03  eilers
 * Reenable debug output
 *
 * Revision 1.11  2003/01/03 12:31:28  eilers
 * Bugfix for calculating data diffs..
 *
 * Revision 1.10  2003/01/02 14:27:12  eilers
 * Improved query by example: Search by date is possible.. First step
 * for a today plugin for birthdays..
 *
 * Revision 1.9  2002/12/08 12:48:57  eilers
 * Moved journal-enum from ocontact into i the xml-backend..
 *
 * Revision 1.8  2002/11/14 17:04:24  eilers
 * Sorting will now work if fullname is identical on some entries
 *
 * Revision 1.7  2002/11/13 15:02:46  eilers
 * Small Bug in sorted fixed
 *
 * Revision 1.6  2002/11/13 14:14:51  eilers
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

#include "ocontactaccessbackend_xml.h"

#include <qasciidict.h>
#include <qdatetime.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qregexp.h>
#include <qarray.h>
#include <qmap.h>
#include <qdatetime.h>

#include <qpe/global.h>

#include <opie/xmltree.h>
#include "ocontactaccessbackend.h"
#include "ocontactaccess.h"

#include <stdlib.h>
#include <errno.h>

using namespace Opie;


OContactAccessBackend_XML::OContactAccessBackend_XML ( const QString& appname, const QString& filename ):
	m_changed( false )
{
	// Just m_contactlist should call delete if an entry
	// is removed.
	m_contactList.setAutoDelete( true );
	m_uidToContact.setAutoDelete( false );

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

bool OContactAccessBackend_XML::save()
{

	if ( !m_changed )
		return true;

	QString strNewFile = m_fileName + ".new";
	QFile f( strNewFile );
	if ( !f.open( IO_WriteOnly|IO_Raw ) )
		return false;

	int total_written;
	int idx_offset = 0;
	QString out;

	// Write Header
	out = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE Addressbook ><AddressBook>\n"
		" <Groups>\n"
		" </Groups>\n"
		" <Contacts>\n";
	QCString cstr = out.utf8();
	f.writeBlock( cstr.data(), cstr.length() );
	idx_offset += cstr.length();
	out = "";

	// Write all contacts
	QListIterator<OContact> it( m_contactList );
	for ( ; it.current(); ++it ) {
		// qWarning(" Uid %d at Offset: %x", (*it)->uid(), idx_offset );
		out += "<Contact ";
		(*it)->save( out );
		out += "/>\n";
		cstr = out.utf8();
		total_written = f.writeBlock( cstr.data(), cstr.length() );
		idx_offset += cstr.length();
		if ( total_written != int(cstr.length()) ) {
			f.close();
			QFile::remove( strNewFile );
			return false;
		}
		out = "";
	}
	out += " </Contacts>\n</AddressBook>\n";

	// Write Footer
	cstr = out.utf8();
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

bool OContactAccessBackend_XML::load ()
{
	m_contactList.clear();
	m_uidToContact.clear();

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

void OContactAccessBackend_XML::clear ()
{
	m_contactList.clear();
	m_uidToContact.clear();

	m_changed = false;
}

bool OContactAccessBackend_XML::wasChangedExternally()
{
	QFileInfo fi( m_fileName );

	QDateTime lastmod = fi.lastModified ();

	return (lastmod != m_readtime);
}

QArray<int> OContactAccessBackend_XML::allRecords() const
{
	QArray<int> uid_list( m_contactList.count() );

	uint counter = 0;
	QListIterator<OContact> it( m_contactList );
	for( ; it.current(); ++it ){
		uid_list[counter++] = (*it)->uid();
	}

	return ( uid_list );
}

OContact OContactAccessBackend_XML::find ( int uid ) const
{
	OContact foundContact; //Create empty contact

	OContact* found = m_uidToContact.find( QString().setNum( uid ) );

	if ( found ){
		foundContact = *found;
	}

	return ( foundContact );
}

QArray<int> OContactAccessBackend_XML::queryByExample ( const OContact &query, int settings,
							const QDateTime& d )
{

	QArray<int> m_currentQuery( m_contactList.count() );
	QListIterator<OContact> it( m_contactList );
	uint arraycounter = 0;

	for( ; it.current(); ++it ){
		/* Search all fields and compare them with query object. Store them into list
		 * if all fields matches.
		 */
		QDate* queryDate = 0l;
		QDate* checkDate = 0l;
		bool allcorrect = true;
		for ( int i = 0; i < Qtopia::Groups; i++ ) {
			// Birthday and anniversary are special nonstring fields and should
			// be handled specially
			switch ( i ){
			case Qtopia::Birthday:
				queryDate = new QDate( query.birthday() );
				checkDate = new QDate( (*it)->birthday() );
			case Qtopia::Anniversary:
				if ( queryDate == 0l ){
					queryDate = new QDate( query.anniversary() );
					checkDate = new QDate( (*it)->anniversary() );
				}

				if ( queryDate->isValid() ){
					if(  checkDate->isValid() ){
						if ( settings & OContactAccess::DateYear ){
							if ( queryDate->year() != checkDate->year() )
								allcorrect = false;
						}
						if ( settings & OContactAccess::DateMonth ){
							if ( queryDate->month() != checkDate->month() )
								allcorrect = false;
						}
						if ( settings & OContactAccess::DateDay ){
							if ( queryDate->day() != checkDate->day() )
								allcorrect = false;
						}
						if ( settings & OContactAccess::DateDiff ) {
							QDate current;
							// If we get an additional date, we
							// will take this date instead of
							// the current one..
							if ( !d.date().isValid() )
								current = QDate::currentDate();
							else
								current = d.date();

							// We have to equalize the year, otherwise
							// the search will fail..
							checkDate->setYMD( current.year(),
									   checkDate->month(),
									   checkDate->day() );
							if ( *checkDate < current )
								checkDate->setYMD( current.year()+1,
										   checkDate->month(),
										   checkDate->day() );

							// Check whether the birthday/anniversary date is between
							// the current/given date and the maximum date
							// ( maximum time range ) !
							qWarning("Checking if %s is between %s and %s ! ",
								 checkDate->toString().latin1(),
								 current.toString().latin1(),
								 queryDate->toString().latin1() );
							if ( current.daysTo( *queryDate ) >= 0 ){
								if ( !( ( *checkDate >= current ) &&
									( *checkDate <= *queryDate ) ) ){
									allcorrect = false;
									qWarning (" Nope!..");
								}
							}
						}
					} else{
						// checkDate is invalid. Therefore this entry is always rejected
						allcorrect = false;
					}
				}

				delete queryDate;
				queryDate = 0l;
				delete checkDate;
				checkDate = 0l;
				break;
			default:
				/* Just compare fields which are not empty in the query object */
				if ( !query.field(i).isEmpty() ){
					switch ( settings & ~( OContactAccess::IgnoreCase
							       | OContactAccess::DateDiff
							       | OContactAccess::DateYear
							       | OContactAccess::DateMonth
							       | OContactAccess::DateDay
							       | OContactAccess::MatchOne
							       ) ){

					case OContactAccess::RegExp:{
						QRegExp expr ( query.field(i),
							       !(settings & OContactAccess::IgnoreCase),
							       false );
						if ( expr.find ( (*it)->field(i), 0 ) == -1 )
							allcorrect = false;
					}
						break;
					case OContactAccess::WildCards:{
						QRegExp expr ( query.field(i),
							       !(settings & OContactAccess::IgnoreCase),
							       true );
						if ( expr.find ( (*it)->field(i), 0 ) == -1 )
							allcorrect = false;
					}
						break;
					case OContactAccess::ExactMatch:{
						if (settings & OContactAccess::IgnoreCase){
							if ( query.field(i).upper() !=
							     (*it)->field(i).upper() )
								allcorrect = false;
						}else{
							if ( query.field(i) != (*it)->field(i) )
								allcorrect = false;
						}
					}
						break;
					}
				}
			}
		}
		if ( allcorrect ){
			m_currentQuery[arraycounter++] = (*it)->uid();
		}
	}

	// Shrink to fit..
	m_currentQuery.resize(arraycounter);

	return m_currentQuery;
}

QArray<int> OContactAccessBackend_XML::matchRegexp(  const QRegExp &r ) const
{
	QArray<int> m_currentQuery( m_contactList.count() );
	QListIterator<OContact> it( m_contactList );
	uint arraycounter = 0;

	for( ; it.current(); ++it ){
		if ( (*it)->match( r ) ){
			m_currentQuery[arraycounter++] = (*it)->uid();
		}

	}
	// Shrink to fit..
	m_currentQuery.resize(arraycounter);

	return m_currentQuery;
}

const uint OContactAccessBackend_XML::querySettings()
{
	return ( OContactAccess::WildCards
		 | OContactAccess::IgnoreCase
		 | OContactAccess::RegExp
		 | OContactAccess::ExactMatch
		 | OContactAccess::DateDiff
		 | OContactAccess::DateYear
		 | OContactAccess::DateMonth
		 | OContactAccess::DateDay
		 );
}

bool OContactAccessBackend_XML::hasQuerySettings (uint querySettings) const
{
	/* OContactAccess::IgnoreCase, DateDiff, DateYear, DateMonth, DateDay
	 * may be added with any of the other settings. IgnoreCase should never used alone.
	 * Wildcards, RegExp, ExactMatch should never used at the same time...
	 */

	// Step 1: Check whether the given settings are supported by this backend
	if ( ( querySettings & ( 
				OContactAccess::IgnoreCase
				| OContactAccess::WildCards
				| OContactAccess::DateDiff
				| OContactAccess::DateYear
				| OContactAccess::DateMonth
				| OContactAccess::DateDay
				| OContactAccess::RegExp
				| OContactAccess::ExactMatch
			       ) ) != querySettings )
		return false;

	// Step 2: Check whether the given combinations are ok..

	// IngoreCase alone is invalid
	if ( querySettings == OContactAccess::IgnoreCase )
		return false;

	// WildCards, RegExp and ExactMatch should never used at the same time 
	switch ( querySettings & ~( OContactAccess::IgnoreCase
				    | OContactAccess::DateDiff
				    | OContactAccess::DateYear
				    | OContactAccess::DateMonth
				    | OContactAccess::DateDay
				    )
		 ){
	case OContactAccess::RegExp:
		return ( true );
	case OContactAccess::WildCards:
		return ( true );
	case OContactAccess::ExactMatch:
		return ( true );
	case 0: // one of the upper removed bits were set..
		return ( true );
	default:
		return ( false );
	}
}

// Currently only asc implemented..
QArray<int> OContactAccessBackend_XML::sorted( bool asc,  int , int ,  int )
{
	QMap<QString, int> nameToUid;
	QStringList names;
	QArray<int> m_currentQuery( m_contactList.count() );

	// First fill map and StringList with all Names
	// Afterwards sort namelist and use map to fill array to return..
	QListIterator<OContact> it( m_contactList );
	for( ; it.current(); ++it ){
		names.append( (*it)->fileAs() + QString::number( (*it)->uid() ) );
		nameToUid.insert( (*it)->fileAs() + QString::number( (*it)->uid() ), (*it)->uid() );
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

bool OContactAccessBackend_XML::add ( const OContact &newcontact )
{
	//qWarning("odefaultbackend: ACTION::ADD");
	updateJournal (newcontact, ACTION_ADD);
	addContact_p( newcontact );

	m_changed = true;

	return true;
}

bool OContactAccessBackend_XML::replace ( const OContact &contact )
{
	m_changed = true;

	OContact* found = m_uidToContact.find ( QString().setNum( contact.uid() ) );

	if ( found ) {
		OContact* newCont = new OContact( contact );

		updateJournal ( *newCont, ACTION_REPLACE);
		m_contactList.removeRef ( found );
		m_contactList.append ( newCont );
		m_uidToContact.remove( QString().setNum( contact.uid() ) );
		m_uidToContact.insert( QString().setNum( newCont->uid() ), newCont );

		qWarning("Nur zur Sicherheit: %d == %d ?",contact.uid(), newCont->uid());

		return true;
	} else
		return false;
}

bool OContactAccessBackend_XML::remove ( int uid )
{
	m_changed = true;

	OContact* found = m_uidToContact.find ( QString().setNum( uid ) );

	if ( found ) {
		updateJournal ( *found, ACTION_REMOVE);
		m_contactList.removeRef ( found );
		m_uidToContact.remove( QString().setNum( uid ) );

		return true;
	} else
		return false;
}

bool OContactAccessBackend_XML::reload(){
	/* Reload is the same as load in this implementation */
	return ( load() );
}

void OContactAccessBackend_XML::addContact_p( const OContact &newcontact )
{
	OContact* contRef = new OContact( newcontact );

	m_contactList.append ( contRef );
	m_uidToContact.insert( QString().setNum( newcontact.uid() ), contRef );
}

/* This function loads the xml-database and the journalfile */
bool OContactAccessBackend_XML::load( const QString filename, bool isJournal )
{

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
	journal_action action = ACTION_ADD;
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
					// qWarning("Attribute %s not known.", it.key().latin1());
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
					action = journal_action(it.data().toInt());
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
				case ACTION_ADD:
					addContact_p (contact);
					break;
				case ACTION_REMOVE:
					if ( !remove (contact.uid()) )
						qWarning ("ODefBack(journal)::Unable to remove uid: %d",
							  contact.uid() );
					break;
				case ACTION_REPLACE:
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


void OContactAccessBackend_XML::updateJournal( const OContact& cnt,
					       journal_action action )
{
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

void OContactAccessBackend_XML::removeJournal()
{
	QFile f ( m_journalName );
	if ( f.exists() )
		f.remove();
}

