/*
 * SQL Backend for the OPIE-Contact Database.
 *
 * Copyright (c) 2002 by Stefan Eilers (Eilers.Stefan@epost.de)
 *
 * =====================================================================
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU Library General Public
 *      License as published by the Free Software Foundation; either
 *      version 2 of the License, or (at your option) any later version.
 * =====================================================================
 * =====================================================================
 * Version: $Id: ocontactaccessbackend_sql.cpp,v 1.2.2.1 2003-10-20 15:52:23 eilers Exp $
 * =====================================================================
 * History:
 * $Log: ocontactaccessbackend_sql.cpp,v $
 * Revision 1.2.2.1  2003-10-20 15:52:23  eilers
 * This sqlite-stuff drives me crazy !! This is a much better database
 * structure, but much too slow.. I think I use the stupid, but fast structure..
 *
 * Revision 1.2  2003/09/29 07:44:26  eilers
 * Improvement of PIM-SQL Databases, but search queries are still limited.
 * Addressbook: Changed table layout. Now, we just need 1/3 of disk-space.
 * Todo: Started to add new attributes. Some type conversions missing.
 *
 * Revision 1.1  2003/09/22 14:31:16  eilers
 * Added first experimental incarnation of sql-backend for addressbook.
 * Some modifications to be able to compile the todo sql-backend.
 * A lot of changes fill follow...
 *
 */

#include "ocontactaccessbackend_sql.h"

#include <qarray.h>
#include <qdatetime.h>
#include <qstringlist.h>

#include <qpe/global.h>
#include <qpe/recordfields.h>

#include <opie/ocontactfields.h>
#include <opie/oconversion.h>
#include <opie2/osqldriver.h>
#include <opie2/osqlresult.h>
#include <opie2/osqlmanager.h>
#include <opie2/osqlquery.h>


/*
 * Implementation of used query types
 * CREATE query
 * LOAD query
 * INSERT
 * REMOVE
 * CLEAR
 */
namespace {
	// Maybe this should be added to OContactFields ?
	// These are the fields containing the information, how
	// to contact a person
	enum contactFields{
		DefaultEmail = 0,
		Emails,
		WebPage,

		Phone,
		Fax,
		Mobile,
		Pager,
		
		Street,
		City,
		State,
		Zip,
		Country,

		lastContactField
	};

	/**
	 * Clears (delete) a Table
	 */
	class ClearQuery : public OSQLQuery {
	public:
		ClearQuery();
		~ClearQuery();
		QString query()const;
		
	};
	

	/**
	 * LoadQuery
	 * this one queries for all uids
	 */
	class LoadQuery : public OSQLQuery {
	public:
		LoadQuery();
		~LoadQuery();
		QString query()const;
	};
	

	/**
	 * removes one from the table
	 */
	class RemoveQuery : public OSQLQuery {
	public:
		RemoveQuery(int uid );
		~RemoveQuery();
		QString query()const;
	private:
		int m_uid;
	};
	
	/**
	 * a find query for noncustom elements
	 */
	class FindQuery : public OSQLQuery {
	public:
		FindQuery(int uid);
		FindQuery(const QArray<int>& );
		~FindQuery();
		QString query()const;
	private:
		QString single()const;
		QString multi()const;
		QArray<int> m_uids;
		int m_uid;
	};

	/**
	 * a find query for custom elements
	 */
	class FindCustomQuery : public OSQLQuery {
	public:
		FindCustomQuery(int uid);
		FindCustomQuery(const QArray<int>& );
		~FindCustomQuery();
		QString query()const;
	private:
		QString single()const;
		QString multi()const;
		QArray<int> m_uids;
		int m_uid;
	};
	

	
	ClearQuery::ClearQuery()
		: OSQLQuery() {}
	ClearQuery::~ClearQuery() {}
	QString ClearQuery::query()const {
		QString qu = "drop table personal;";
		qu += "drop table custom_data;";
		qu += "drop table contact;";
		qu += "drop table hasContact;";
		return qu;
	}


	LoadQuery::LoadQuery() : OSQLQuery() {}
	LoadQuery::~LoadQuery() {}
	QString LoadQuery::query()const {
		QString qu;
		qu += "select personal.uid from personal";
		
		return qu;
	}
		

	RemoveQuery::RemoveQuery(int uid )
		: OSQLQuery(), m_uid( uid ) {}
	RemoveQuery::~RemoveQuery() {}
	QString RemoveQuery::query()const {
		QString qu = "DELETE from personal where uid = " 
			+ QString::number(m_uid) + ";";
		qu += "DELETE from hasContact where uid = "
			+ QString::number(m_uid) + ";";

		// Removes all entries from the contact table, which are not referenced
		// by the hasContact-table. 
		// This isn't the fastest solution, but protects us against lost
		// entries which just consumes memory..
		qu += "delete from contact where contact.id not in (select id from hasContact);";
		return qu;
	}
	

	

	FindQuery::FindQuery(int uid)
		: OSQLQuery(), m_uid( uid ) {
	}
	FindQuery::FindQuery(const QArray<int>& ints)
		: OSQLQuery(), m_uids( ints ){
	}
	FindQuery::~FindQuery() {
	}
	QString FindQuery::query()const{
// 		if ( m_uids.count() == 0 )
		return single();
	}
	/*
	  else
			return multi();
			}
        QString FindQuery::multi()const {
		QString qu = "select uid, type, value from addressbook where";
		for (uint i = 0; i < m_uids.count(); i++ ) {
			qu += " UID = " + QString::number( m_uids[i] ) + " OR";
		}
		qu.remove( qu.length()-2, 2 ); // Hmmmm.. 
		return qu;
	}
	*/
	QString FindQuery::single()const{
		QString qu = "select *";
		qu += " from personal where uid = " + QString::number(m_uid);

		// qWarning("find query: %s", qu.latin1() );
		return qu;
	}


	FindCustomQuery::FindCustomQuery(int uid)
		: OSQLQuery(), m_uid( uid ) {
	}
	FindCustomQuery::FindCustomQuery(const QArray<int>& ints)
		: OSQLQuery(), m_uids( ints ){
	}
	FindCustomQuery::~FindCustomQuery() {
	}
	QString FindCustomQuery::query()const{
//		if ( m_uids.count() == 0 )
			return single();
	}
	QString FindCustomQuery::single()const{
		QString qu = "select uid, type, value from custom_data where uid = ";
		qu += QString::number(m_uid);
		return qu;
	}



};


/* --------------------------------------------------------------------------- */

OContactAccessBackend_SQL::OContactAccessBackend_SQL ( const QString& /* appname */, 
						       const QString& filename ): m_changed(false)
{
	qWarning("C'tor OContactAccessBackend_SQL starts");
	QTime t;
	t.start();

	/* Expecting to access the default filename if nothing else is set */
	if ( filename.isEmpty() ){
		m_fileName = Global::applicationFileName( "addressbook","addressbook.db" );
	} else
		m_fileName = filename;

	// Get the standart sql-driver from the OSQLManager..
	OSQLManager man;
	m_driver = man.standard();
	m_driver->setUrl( m_fileName );	

	initFields();

	load();

	qWarning("C'tor OContactAccessBackend_SQL ends: %d ms", t.elapsed() );
}

void OContactAccessBackend_SQL::initFields()
{
	idToContactFieldName.insert( DefaultEmail, QString("DefaultEmail") );
	idToContactFieldName.insert( Emails, QString("Emails") );
	idToContactFieldName.insert( WebPage, QString("BusinessWebPage") );
	idToContactFieldName.insert( Phone, QString("Phone") );
	idToContactFieldName.insert( Fax, QString("Fax") );
	idToContactFieldName.insert( Mobile, QString("Mobile") );
	idToContactFieldName.insert( Pager, QString("Pager") );
	idToContactFieldName.insert( Street, QString("Street") );
	idToContactFieldName.insert( City, QString("City") );
	idToContactFieldName.insert( State, QString("State") );
	idToContactFieldName.insert( Zip, QString("Zip") );
	idToContactFieldName.insert( Country, QString("Country") );

	contactFieldNameToId.insert( QString("DefaultEmail"), DefaultEmail );
	contactFieldNameToId.insert( QString("Emails"), Emails);
	contactFieldNameToId.insert( QString("BusinessWebPage"), WebPage);
	contactFieldNameToId.insert( QString("Phone"), Phone);
	contactFieldNameToId.insert( QString("Fax"), Fax);
	contactFieldNameToId.insert( QString("Mobile"), Mobile);
	contactFieldNameToId.insert( QString("Pager"), Pager);
	contactFieldNameToId.insert( QString("Street"), Street);
	contactFieldNameToId.insert( QString("City"), City);
	contactFieldNameToId.insert( QString("State"), State);
	contactFieldNameToId.insert( QString("Zip"), Zip);
	contactFieldNameToId.insert( QString("Country"), Country);

}

bool OContactAccessBackend_SQL::load ()
{
	if (!m_driver->open() )
		return false;

	// Don't expect that the database exists.
	// It is save here to create the table, even if it
	// do exist. ( Is that correct for all databases ?? )
	if ( !createTable() )
		return false;

	update();

	return true;

}

bool OContactAccessBackend_SQL::reload()
{
	return load();
}

bool OContactAccessBackend_SQL::save()
{
	return m_driver->close();
}


void OContactAccessBackend_SQL::clear ()
{
	ClearQuery cle;
	OSQLResult res = m_driver->query( &cle );
	
	createTable();
}

bool OContactAccessBackend_SQL::wasChangedExternally()
{
	return false;
}

QArray<int> OContactAccessBackend_SQL::allRecords() const
{

	// FIXME: Think about cute handling of changed tables..
	// Thus, we don't have to call update here...
	// Or we should update always to receive changed from
	// other clients..
 	if ( m_changed )  
		((OContactAccessBackend_SQL*)this)->update();
	
	return m_uids;
}

bool OContactAccessBackend_SQL::add ( const OContact& newcontact )
{
	QString qu;

	qu = createPersonalString( newcontact );
	qu += createCustomString( newcontact );

	// qu  += "commit;";
	qWarning("add %s", qu.latin1() );

	// We need a unique id from the contact-table. Thus we take
	// the largest used number and increment it once..
	QString idQuery = "Select max(id) FROM contact";
	OSQLRawQuery rawQuery( idQuery );
	OSQLResult res = m_driver->query( &rawQuery );
	
	bool ok;
	int contactId = res.first().data("max(id)").toInt( &ok );
	if ( !ok )
		contactId = 0;
	else
		contactId++;

	// Create insert strings for contact table and connect it with the 
	// personal table via hasContact
	QString insertString = createContactString( contactId, Home, newcontact );
	if ( !insertString.isNull() ){
		qu += insertString;
		qu += createHasContactString( newcontact.uid(), contactId, Home );
		contactId++;
	}
	insertString = createContactString( contactId, Business, newcontact );
	if ( !insertString.isNull() ){
		qu += insertString;
		qu += createHasContactString( newcontact.uid(), contactId, Business );
	}


	qWarning("Insert into Tabel:\n%s", qu.latin1() );

	OSQLRawQuery rawInsertQuery( qu );
	OSQLResult resInsertQuery = m_driver->query( &rawInsertQuery );
	if ( resInsertQuery.state() != OSQLResult::Success ){
		return false;
	}

	// If everything went ok, we add the new uid to our internal list
	int c = m_uids.count();
	m_uids.resize( c+1 );
	m_uids[c] = newcontact.uid();


	
	return true;
}

bool OContactAccessBackend_SQL::remove ( int uid )
{
	RemoveQuery rem( uid );
	OSQLResult res = m_driver->query(&rem );

	if ( res.state() == OSQLResult::Failure )
		return false;

	m_changed = true;

	return true;
}

bool OContactAccessBackend_SQL::replace ( const OContact &contact )
{
	if ( !remove( contact.uid() ) )
		return false;
	
	return add( contact );
}


OContact OContactAccessBackend_SQL::find ( int uid ) const
{
	qWarning("OContactAccessBackend_SQL::find()");
	QTime t;
	t.start();
	int tneeded;

 	QMap< int, QString > contactDataMap = requestPersonal( uid );
 	requestContact( Home, uid, &contactDataMap );
  	requestContact( Business, uid, &contactDataMap );

	OContact retContact( contactDataMap );
       	retContact.setExtraMap( requestCustom( uid ) );

	tneeded = t.elapsed();

	qWarning("OContactAccessBackend_SQL::find() needed: %d ms", tneeded );

	return retContact;
}



QArray<int> OContactAccessBackend_SQL::queryByExample ( const OContact &query, int settings, const QDateTime& d = QDateTime() )
{
	QTime t;
	t.start();

	QString qu = "SELECT DISTINCT personal.uid FROM personal, contact, hasContact WHERE hasContact.uid = personal.uid AND hasContact.id = contact.id AND ";
	// 	QString qu = "SELECT personal.uid FROM personal WHERE ";

	QMap<int, QString> queryFields = query.toMap();
	QStringList fieldList = OContactFields::untrpersonalfields( false );
	fieldList += OContactFields::untrdetailsfields( false ); 
	QMap<int, QString> translate = OContactFields::idToUntrFields();

	// Convert every filled field to a SQL-Query which belongs to the 
	// personal table
	bool isAnyFieldSelected = false;
	for ( QMap<int, QString>::Iterator it = queryFields.begin(); it != queryFields.end(); ++it ){
		int fieldId = it.key(); 
		QString fieldName = translate[ fieldId ];

		// This "if" is highly dangerous due to the fact that it is
		// dependent to the enum in "recordfields.h"
		if ( (fieldId >= Qtopia::AddressCategory) && (fieldId <= Qtopia::Company) 
		     || ( (fieldId >= Qtopia::DefaultEmail) && (fieldId <= Qtopia::Emails) )
		     || ( (fieldId >= Qtopia::Office) && (fieldId <= Qtopia::Manager) )
		     || ( (fieldId >= Qtopia::Spouse) && (fieldId <= Qtopia::Notes) ) )
		{
			// Fields in personal table
		isAnyFieldSelected = true;
		switch( fieldId ){
		default:
			QString queryStr = it.data();
			// Switching between case sensitive and insensitive...
			// LIKE is not case sensitive, GLOB is case sensitive
			// Do exist a better solution to switch this ?
			if ( settings & OContactAccess::IgnoreCase )
				qu += "(\"" + fieldName + "\"" + " LIKE " + "'" 
					+ queryStr.replace(QRegExp("\\*"),"%") + "'" + ") AND "; 
			else
				qu += "(\"" + fieldName + "\"" + " GLOB " + "'" 
					+ queryStr + "'" + ") AND "; 
			
			
		}
		} else {
			// Fields in contact table
		}


	}
	// Skip trailing "AND"
	if ( isAnyFieldSelected )
		qu = qu.left( qu.length() - 4 );

	qWarning( "queryByExample query: %s", qu.latin1() );

	// Execute query and return the received uid's
	OSQLRawQuery raw( qu );
	OSQLResult res = m_driver->query( &raw );
	if ( res.state() != OSQLResult::Success ){
		QArray<int> empty;
		return empty;
	}

	QArray<int> list = extractUids( res );

	qWarning("QueryByExample needs: %d ms", t.elapsed() );

	return list;		
}

QArray<int> OContactAccessBackend_SQL::matchRegexp(  const QRegExp &r ) const
{
	QArray<int> nix(0);
	return nix;
}

const uint OContactAccessBackend_SQL::querySettings()
{
	return OContactAccess::IgnoreCase 
		|| OContactAccess::WildCards;
}

bool OContactAccessBackend_SQL::hasQuerySettings (uint querySettings) const
{
	/* OContactAccess::IgnoreCase, DateDiff, DateYear, DateMonth, DateDay
	 * may be added with any of the other settings. IgnoreCase should never used alone.
	 * Wildcards, RegExp, ExactMatch should never used at the same time...
	 */

	// Step 1: Check whether the given settings are supported by this backend
	if ( ( querySettings & ( 
				OContactAccess::IgnoreCase
				| OContactAccess::WildCards
// 				| OContactAccess::DateDiff
// 				| OContactAccess::DateYear
// 				| OContactAccess::DateMonth
// 				| OContactAccess::DateDay
// 				| OContactAccess::RegExp
// 				| OContactAccess::ExactMatch
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

QArray<int> OContactAccessBackend_SQL::sorted( bool asc,  int , int ,  int )
{
	QTime t;
	t.start();

	QString query = "SELECT personal.uid FROM personal ";
	query += "ORDER BY \"Last Name\" ";

	if ( !asc )
		query += "DESC";

	// qWarning("sorted query is: %s", query.latin1() ); 

	OSQLRawQuery raw( query );
	OSQLResult res = m_driver->query( &raw );
	if ( res.state() != OSQLResult::Success ){
		QArray<int> empty;
		return empty;
	}

	QArray<int> list = extractUids( res );

	qWarning("sorted needed %d ms!", t.elapsed() );
	return list;
}


void OContactAccessBackend_SQL::update()
{
	qWarning("Update starts");
	QTime t;
	t.start();

	// Now load the database set and extract the uid's
	// which will be held locally

	LoadQuery lo;
	OSQLResult res = m_driver->query(&lo);
	if ( res.state() != OSQLResult::Success )
		return;

	m_uids = extractUids( res );

	m_changed = false;

	qWarning("Update ends %d ms", t.elapsed() );
}

/* --- All private stuff below this line ---------------------------------------- */

QString OContactAccessBackend_SQL::createPersonalString( const OContact& contact )
{
	QString qu;
	qu  += "insert into personal VALUES( " + 
		QString::number( contact.uid() );

	// Get personal information out of the contact-class
	// Remember: The category is stored in contactMap, too ! 
	QMap<int, QString> contactMap = contact.toMap();
	
	QStringList fieldList = OContactFields::untrpersonalfields( false );
	fieldList += OContactFields::untrdetailsfields( false );
	QMap<QString, int> translate = OContactFields::untrFieldsToId();
	for ( QStringList::Iterator it = ++fieldList.begin(); it != fieldList.end(); ++it ){
		// Convert Column-String to Id and get value for this id..
		// Hmmm.. Maybe not very cute solution..
		int id = translate[*it];
		switch ( id ){
		case Qtopia::Birthday:{
			// These entries should stored in a special format
			// year-month-day
			QDate day = contact.birthday();
			if ( day.isValid() ){
				qu += QString(",\"%1-%2-%3\"")
					.arg( day.year() )
					.arg( day.month() )
					.arg( day.day() );
			} else {
				qu += ",\"\"";
			}
		}
			break;
		case Qtopia::Anniversary:{
			// These entries should stored in a special format
			// year-month-day
			QDate day = contact.anniversary();
			if ( day.isValid() ){
				qu += QString(",\"%1-%2-%3\"")
					.arg( day.year() )
					.arg( day.month() )
					.arg( day.day() );
			} else {
				qu += ",\"\"";
			}
		}
			break;
			
		default:
			qu += QString( ",\"%1\"" ).arg( contactMap[id] );
		}
	}
	qu += " );";

	return qu;
}

QString OContactAccessBackend_SQL::createCustomString( const OContact& contact )
{
	// Now add custom data..
	QString qu;
	int id = 0;
	id = 0;
	QMap<QString, QString> customMap = contact.toExtraMap();
	if ( customMap.count() ){
		for( QMap<QString, QString>::Iterator it = customMap.begin(); 
		     it != customMap.end(); ++it ){
			qu  += "insert into custom_data VALUES(" 
				+  QString::number( contact.uid() )
				+ ","
				+  QString::number( id++ ) 
				+ ",'" 
				+ it.key() //.latin1()
				+ "',"
				+ "0" // Priority for future enhancements
				+ ",'" 
				+ it.data() //.latin1()
				+ "');";
		}		
	}
	return qu;
}

QString OContactAccessBackend_SQL::createContactString( int id, int type, const OContact& contact )
{
	QString qu;
	QMap<int, QString> contactMap = contact.toMap();
	bool hasContent = false;

	qu = "INSERT INTO contact VALUES( " + QString::number( id );
	for( int field = DefaultEmail; field < lastContactField; ++field ){
		QString insertString = "";
		switch( field ){
			// FIXME: We should seperate between business and home
			//        emails !!
		case DefaultEmail:
			switch( type ){
			case Home:
				insertString = contactMap[Qtopia::DefaultEmail];
				break;
			case Business:
				break;
			}
			break;
		case Emails:
			switch( type ){
			case Home:
				// insertString = contactMap[Qtopia::HomePager];
				insertString = contactMap[Qtopia::Emails];
				break;
			case Business:
				break;
			}
			break;
		case WebPage:
			switch( type ){
			case Home:
				insertString = contactMap[Qtopia::HomeWebPage];
				break;
			case Business:
				insertString = contactMap[Qtopia::BusinessWebPage];
				break;
			}
			break;
		case Phone:
			switch( type ){
			case Home:
				insertString = contactMap[Qtopia::HomePhone];
				break;
			case Business:
				insertString = contactMap[Qtopia::BusinessPhone];
				break;
			}
			break;
		case Fax:
			switch( type ){
			case Home:
				insertString = contactMap[Qtopia::HomeFax];
				break;
			case Business:
				insertString = contactMap[Qtopia::BusinessFax];
				break;
			}
			break;
		case Mobile:
			switch( type ){
			case Home:
				insertString = contactMap[Qtopia::HomeMobile];
				break;
			case Business:
				insertString = contactMap[Qtopia::BusinessMobile];
				break;
			}
			break;
		case Pager:
			switch( type ){
			case Home:
				// insertString = contactMap[Qtopia::HomePager];
				qWarning("Homepager is currently not supported !");
				break;
			case Business:
				insertString = contactMap[Qtopia::BusinessPager];
				break;
			}
			break;
		case Street:
			switch( type ){
			case Home:
				insertString = contactMap[Qtopia::HomeStreet];
				break;
			case Business:
				insertString = contactMap[Qtopia::BusinessStreet];
				break;
			}
			break;
		case City:
			switch( type ){
			case Home:
				insertString = contactMap[Qtopia::HomeCity];
				break;
			case Business:
				insertString = contactMap[Qtopia::BusinessCity];
				break;
			}
			break;
		case State:
			switch( type ){
			case Home:
				insertString = contactMap[Qtopia::HomeState];
				break;
			case Business:
				insertString = contactMap[Qtopia::BusinessState];
				break;
			}
			break;
		case Zip:
			switch( type ){
			case Home:
				insertString = contactMap[Qtopia::HomeZip];
				break;
			case Business:
				insertString = contactMap[Qtopia::BusinessZip];
				break;
			}
			break;
		case Country:
			switch( type ){
			case Home:
				insertString = contactMap[Qtopia::HomeCountry];
				break;
			case Business:
				insertString = contactMap[Qtopia::BusinessCountry];
				break;
			}
			break;
		default:
			qWarning("OContactAccessBackend_SQL::createContactString():Unknown Field selected");
		}
		if ( !insertString.isEmpty() ){
			qu += ",'" + insertString + "'";
			hasContent = true;
		}else 
			qu += ",''";
	}
	qu += ");";

	// Only if we really have to store something, we will return a nonempty string
	if ( hasContent )
		return qu;
	else
		return QString::null;
}

QString OContactAccessBackend_SQL::createHasContactString( int uid, int id, contactTypes type )
{
	QString qu;
	
	qu = "INSERT INTO hasContact VALUES( " 
		+ QString::number( uid ) + ", " 
		+ QString::number( id )  + ", "
		+ QString::number( type )+ " );";

	return qu;
}

bool OContactAccessBackend_SQL::createTable()
{
	QString qu = createCreateTableString ( idToContactFieldName );
	OSQLRawQuery rawCreateQuery( qu );
	OSQLResult resCreateQuery = m_driver->query( &rawCreateQuery );
	if ( resCreateQuery.state() != OSQLResult::Success ){
		return false;
	}

	return true;
}

QString OContactAccessBackend_SQL::createCreateTableString( const QMap<int, QString>& idToContactFieldName )
{
		QString qu;

		qu += "create table personal( uid INTEGER PRIMARY KEY ";

		QStringList personalList = OContactFields::untrpersonalfields( false );
		personalList += OContactFields::untrdetailsfields( false );
		for ( QStringList::Iterator it = ++personalList.begin(); it != personalList.end(); ++it ){
			qu += QString( ",\"%1\" VARCHAR(10)" ).arg( *it );
		}
		qu += " );";

		qu += "create table contact( id INTEGER PRIMARY KEY ";
		for ( int i = DefaultEmail; 
		      i < lastContactField; ++i ){
			qu += QString( ",\"%1\" VARCHAR(10)" ).arg( idToContactFieldName[i] );
		}
		qu += " );";
		
		qu += "create table custom_data( uid INTEGER, id INTEGER, type VARCHAR(10), priority INTEGER, value VARCHAR, PRIMARY KEY /* identifier */ (uid, id) );";

		qu += "create table hasContact( uid INTEGER, id INTEGER, type INTEGER, PRIMARY KEY /* bez */ (uid, id) );";
		qWarning("create ->: %s",qu.latin1());

		return qu;
}


QArray<int> OContactAccessBackend_SQL::extractUids( OSQLResult& res ) const
{
	qWarning("extractUids");
	QTime t;
	t.start();
	OSQLResultItem::ValueList list = res.results();
	OSQLResultItem::ValueList::Iterator it;
	QArray<int> ints(list.count() );
	qWarning(" count = %d", list.count() );

	int i = 0;
	for (it = list.begin(); it != list.end(); ++it ) {
		ints[i] =  (*it).data("personal.uid").toInt();
		i++;
	}
	qWarning("extractUids ready: count2 = %d needs %d ms", i, t.elapsed() );

	return ints;

}

QMap<int, QString>  OContactAccessBackend_SQL::requestPersonal( int uid ) const
{
	QTime t;
	t.start();

	QMap<int, QString> nonCustomMap;
	
	int t2needed = 0;
	int t3needed = 0;
	QTime t2;
	t2.start();
	FindQuery query( uid );
	OSQLResult res_noncustom = m_driver->query( &query );
	t2needed = t2.elapsed();

	OSQLResultItem resItem = res_noncustom.first();

	QTime t3;
	t3.start();
	// Now loop through all columns
	QStringList fieldList = OContactFields::untrpersonalfields( false );
	fieldList += OContactFields::untrdetailsfields( false );
	QMap<QString, int> translate = OContactFields::untrFieldsToId();
	for ( QStringList::Iterator it = ++fieldList.begin(); it != fieldList.end(); ++it ){
		// Get data for the selected column and store it with the
		// corresponding id into the map..

		int id =  translate[*it];
		QString value = resItem.data( (*it) );

		if ( value.isEmpty() )
			continue;
		
		// qWarning("Reading %s... found: %s", (*it).latin1(), value.latin1() );

		switch( id ){
		case Qtopia::Birthday:
		case Qtopia::Anniversary:{
			// Birthday and Anniversary are encoded special ( yyyy-mm-dd )
			QStringList list = QStringList::split( '-', value );
			QStringList::Iterator lit = list.begin();
			int year  = (*lit).toInt();
			int month = (*(++lit)).toInt();
			int day   = (*(++lit)).toInt();
			if ( ( day != 0 ) && ( month != 0 ) && ( year != 0 ) ){
			     QDate date( year, month, day );
			     nonCustomMap.insert( id, OConversion::dateToString( date ) );
			}
		}
			break;
		case Qtopia::AddressCategory:
			qWarning("Category is: %s", value.latin1() );
		default:
			nonCustomMap.insert( id, value );
		}
	}

	// First insert uid
	nonCustomMap.insert( Qtopia::AddressUid, resItem.data( "uid" ) ); 
	t3needed = t3.elapsed();

	// qWarning("Adding UID: %s", resItem.data( "uid" ).latin1() );
	qWarning("RequestPersonal needed: insg.:%d ms ( query: %d ms, mapping: %d ms)", 
		 t.elapsed(), t2needed, t3needed  );

	return nonCustomMap;
}

void OContactAccessBackend_SQL::requestContact( contactTypes type, int uid, 
						QMap<int,QString>* contactMap ) const
{
	QTime t;
	t.start();

	QTime t2;
	t2.start();
	// Step 1: Request all information from table contact
	QString qu = QString( "SELECT contact.* FROM contact, hasContact WHERE " )
		+ QString( "hasContact.id = contact.id" )
		+ QString( " AND hasContact.uid = " )
		+ QString::number( uid )
		+ QString( " AND hasContact.type = " ) 
		+ QString::number( type );

	OSQLRawQuery raw( qu );
	OSQLResult res = m_driver->query( &raw );
	if ( ( res.state() != OSQLResult::Success ) && ( res.results().count() == 0 ) ){
		return;
	}

	int querytime = t2.elapsed();

	QTime t3;
	t3.start();
	// Step 2: Now put all information into map
	for( int field = DefaultEmail; field < lastContactField; ++field ){

		// Req. content of field. Skip insertion if field was empty
		QString insertString = res.first().data( "contact." + idToContactFieldName[field] ) ;
		if ( insertString.isEmpty() )
			continue;

		// Insert data into map
		switch( field ){
			// FIXME: We should seperate between business and home
			//        emails !!
		case DefaultEmail:
			switch( type ){
			case Home:
				contactMap->insert( Qtopia::DefaultEmail, insertString );
				break;
			case Business:
				break;
			}
			break;
		case Emails:
			switch( type ){
			case Home:
				// insertString = contactMap[Qtopia::HomePager];
				contactMap->insert( Qtopia::Emails, insertString );
				break;
			case Business:
				break;
			}
			break;
		case WebPage:
			switch( type ){
			case Home:
				contactMap->insert( Qtopia::HomeWebPage, insertString );
				break;
			case Business:
				contactMap->insert( Qtopia::BusinessWebPage, insertString );
				break;
			}
			break;
		case Phone:
			switch( type ){
			case Home:
				contactMap->insert( Qtopia::HomePhone, insertString );
				break;
			case Business:
				contactMap->insert( Qtopia::BusinessPhone, insertString );
				break;
			}
			break;
		case Fax:
			switch( type ){
			case Home:
				contactMap->insert( Qtopia::HomeFax, insertString );
				break;
			case Business:
				contactMap->insert( Qtopia::BusinessFax, insertString );
				break;
			}
			break;
		case Mobile:
			switch( type ){
			case Home:
				contactMap->insert( Qtopia::HomeMobile, insertString );
				break;
			case Business:
				contactMap->insert( Qtopia::BusinessMobile, insertString );
				break;
			}
			break;
		case Pager:
			switch( type ){
			case Home:
				// insertString = contactMap[Qtopia::HomePager];
				qWarning("Homepager is currently not supported !");
				break;
			case Business:
				contactMap->insert( Qtopia::BusinessPager, insertString );
				break;
			}
			break;
		case Street:
			switch( type ){
			case Home:
				contactMap->insert( Qtopia::HomeStreet, insertString );
				break;
			case Business:
				contactMap->insert( Qtopia::BusinessStreet, insertString );
				break;
			}
			break;
		case City:
			switch( type ){
			case Home:
				contactMap->insert( Qtopia::HomeCity, insertString );
				break;
			case Business:
				contactMap->insert( Qtopia::BusinessCity, insertString );
				break;
			}
			break;
		case State:
			switch( type ){
			case Home:
				contactMap->insert( Qtopia::HomeState, insertString );
				break;
			case Business:
				contactMap->insert( Qtopia::BusinessState, insertString );
				break;
			}
			break;
		case Zip:
			switch( type ){
			case Home:
				contactMap->insert( Qtopia::HomeZip, insertString );
				break;
			case Business:
				contactMap->insert( Qtopia::BusinessZip, insertString );
				break;
			}
			break;
		case Country:
			switch( type ){
			case Home:
				contactMap->insert( Qtopia::HomeCountry, insertString );
				break;
			case Business:
				contactMap->insert( Qtopia::BusinessCountry, insertString );
				break;
			}
			break;
		default:
			qWarning("OContactAccessBackend_SQL::createContactString():Unknown Field selected");

		}
	}
	int mappingtime = t3.elapsed();
	qWarning("requestContact needs %d ms (query: %d ms, mapping: %d ms)", t.elapsed(), querytime, mappingtime );
}


QMap<QString, QString>  OContactAccessBackend_SQL::requestCustom( int uid ) const
{
	QTime t;
	t.start();

	QMap<QString, QString> customMap;
	
	FindCustomQuery query( uid );
	OSQLResult res_custom = m_driver->query( &query );

	if ( res_custom.state() == OSQLResult::Failure ) {
		qWarning("OSQLResult::Failure in find query !!");
		QMap<QString, QString> empty;
		return empty;
	}

	OSQLResultItem::ValueList list = res_custom.results();
	OSQLResultItem::ValueList::Iterator it = list.begin();
	for ( ; it != list.end(); ++it ) {
		customMap.insert( (*it).data( "type" ), (*it).data( "value" ) );
	}

	qWarning("RequestCustom needed: %d ms", t.elapsed() );
	return customMap;
}
