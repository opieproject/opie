/*
 * Default Backend (XML) for the OPIE-Contact Database.
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
 * Version: $Id: ocontactdb_defbackend.h,v 1.1.2.1 2002-09-12 19:06:02 eilers Exp $
 * =====================================================================
 * History:
 * $Log: ocontactdb_defbackend.h,v $
 * Revision 1.1.2.1  2002-09-12 19:06:02  eilers
 * default backend now in own file
 *
 *
 */

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

		QValueList<int> allContacts() const {
			QValueList<int> uid_list;
			
			QValueListConstIterator<Contact> it;
			for( it = m_contactList.begin(); it != m_contactList.end(); ++it ){
				uid_list.append( (*it).uid() );
			}
			
			return ( uid_list );
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

		bool queryByExample ( const Contact &query, const uint settings ){

			m_currentQuery.clear();
			QValueListConstIterator<Contact> it;
			bool found_one = false;

			for( it = m_contactList.begin(); it != m_contactList.end(); ++it ){
				/* Search all fields and compare them with query object. Store them into list
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
				if ( allcorrect ){
					found_one = true;
					m_currentQuery.append( (*it).uid() );
				}
			}

			/* Move to the top of the list and set this query valid */
			m_queryIndex = 0;

			if ( found_one )
				m_queryValid = true;

			return found_one;
		}

		const QValueList<int> allFound ()
		{
			return m_currentQuery;
		}

		const uint querySettings()
		{
			return ( OContactDB::query_WildCards
				 & OContactDB::query_IgnoreCase
				 & OContactDB::query_RegExp
				 & OContactDB::query_ExactMatch );
		}

		bool hasQuerySettings (uint querySettings) const
		{
			/* OContactDB::query_IgnoreCase may be added with one
			 * of the other settings, but never used alone.
			 * The other settings are just valid alone...
			 */
			switch ( querySettings & ~OContactDB::query_IgnoreCase ){
			case OContactDB::query_RegExp:
				return ( true );
			case OContactDB::query_WildCards:
				return ( true );
			case OContactDB::query_ExactMatch:
				return ( true );
			default:
				return ( false );
			}
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
		QValueList<int> m_currentQuery;
		uint m_queryIndex;
		bool m_queryValid;
		QDateTime m_readtime;
	};
}
