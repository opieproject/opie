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
 * SQL Backend for the OPIE-Contact Database.
 */

#include "ocontactaccessbackend_sql.h"

#include <qarray.h>
#include <qdatetime.h>
#include <qstringlist.h>

#include <qpe/global.h>
#include <qpe/recordfields.h>

#include <opie2/opimcontactfields.h>
#include <opie2/opimdateconversion.h>
#include <opie2/osqldriver.h>
#include <opie2/osqlresult.h>
#include <opie2/osqlmanager.h>
#include <opie2/osqlquery.h>

using namespace Opie::DB;


// If defined, we use a horizontal table ( uid, attr1, attr2, attr3, ..., attrn ) instead
// vertical like "uid, type, value".
// DON'T DEACTIVATE THIS DEFINE IN PRODUCTIVE ENVIRONMENTS !!
#define __STORE_HORIZONTAL_

// Distinct loading is not very fast. If I expect that every person has just
// one (and always one) 'Last Name', I can request all uid's for existing lastnames, 
// which is faster..
// But this may not be true for all entries, like company contacts..
// The current AddressBook application handles this problem, but other may not.. (eilers)
#define __USE_SUPERFAST_LOADQUERY


/*
 * Implementation of used query types
 * CREATE query
 * LOAD query
 * INSERT
 * REMOVE
 * CLEAR
 */
namespace Opie {
	/**
	 * CreateQuery for the Todolist Table
	 */
	class CreateQuery : public OSQLQuery {
	public:
		CreateQuery();
		~CreateQuery();
		QString query()const;
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
	 * inserts/adds a OPimContact to the table
	 */
	class InsertQuery : public OSQLQuery {
	public:
		InsertQuery(const OPimContact& );
		~InsertQuery();
		QString query()const;
	private:
		OPimContact m_contact;
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
	


	// We using three tables to store the information:
	// 1. addressbook  : It contains General information about the contact (non custom)
	// 2. custom_data  : Not official supported entries
	// All tables are connected by the uid of the contact.
	// Maybe I should add a table for meta-information ? 
	CreateQuery::CreateQuery() : OSQLQuery() {}
	CreateQuery::~CreateQuery() {}
	QString CreateQuery::query()const {
		QString qu;
#ifdef __STORE_HORIZONTAL_

		qu += "create table addressbook( uid PRIMARY KEY ";

		QStringList fieldList = OPimContactFields::untrfields( false );
		for ( QStringList::Iterator it = ++fieldList.begin(); it != fieldList.end(); ++it ){
			qu += QString( ",\"%1\" VARCHAR(10)" ).arg( *it );
		}
		qu += " );";
		
		qu += "create table custom_data( uid INTEGER, id INTEGER, type VARCHAR, priority INTEGER, value VARCHAR, PRIMARY KEY /* identifier */ (uid, id) );";

#else

		qu += "create table addressbook( uid INTEGER, id INTEGER, type VARCHAR, priority INTEGER, value VARCHAR, PRIMARY KEY /* identifier */ (uid, id));";
		qu += "create table custom_data( uid INTEGER, id INTEGER, type VARCHAR, priority INTEGER, value VARCHAR, PRIMARY KEY /* identifier */ (uid, id) );";
// 		qu += "create table dates( uid PRIMARY KEY, type, day, month, year, hour, minute, second );";

#endif // __STORE_HORIZONTAL_
		return qu;
	}
	
	ClearQuery::ClearQuery()
		: OSQLQuery() {}
	ClearQuery::~ClearQuery() {}
	QString ClearQuery::query()const {
		QString qu = "drop table addressbook;";
		qu += "drop table custom_data;";
// 		qu += "drop table dates;";
		return qu;
	}


	LoadQuery::LoadQuery() : OSQLQuery() {}
	LoadQuery::~LoadQuery() {}
	QString LoadQuery::query()const {
		QString qu;
#ifdef __STORE_HORIZONTAL_
		qu += "select uid from addressbook";
#else
#  ifndef __USE_SUPERFAST_LOADQUERY 
		qu += "select distinct uid from addressbook";
#  else
		qu += "select uid from addressbook where type = 'Last Name'";
#  endif // __USE_SUPERFAST_LOADQUERY 
#endif // __STORE_HORIZONTAL_
		
		return qu;
	}
	

	InsertQuery::InsertQuery( const OPimContact& contact )
		: OSQLQuery(), m_contact( contact ) {
	}

	InsertQuery::~InsertQuery() {
	}

	/*
	 * converts from a OPimContact to a query
	 */
	QString InsertQuery::query()const{

#ifdef __STORE_HORIZONTAL_
		QString qu;
		qu  += "insert into addressbook VALUES( " + 
			QString::number( m_contact.uid() );

		// Get all information out of the contact-class
		// Remember: The category is stored in contactMap, too ! 
		QMap<int, QString> contactMap = m_contact.toMap();
		
		QStringList fieldList = OPimContactFields::untrfields( false );
		QMap<QString, int> translate = OPimContactFields::untrFieldsToId();
		for ( QStringList::Iterator it = ++fieldList.begin(); it != fieldList.end(); ++it ){
			// Convert Column-String to Id and get value for this id..
			// Hmmm.. Maybe not very cute solution..
			int id = translate[*it];
			switch ( id ){
			case Qtopia::Birthday:{
				// These entries should stored in a special format
				// year-month-day
				QDate day = m_contact.birthday();
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
				QDate day = m_contact.anniversary();
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

			
#else
		// Get all information out of the contact-class
		// Remember: The category is stored in contactMap, too ! 
		QMap<int, QString> contactMap = m_contact.toMap();
		
		QMap<QString, QString> addressbook_db;
		
		// Get the translation from the ID to the String
		QMap<int, QString> transMap = OPimContactFields::idToUntrFields();
		
		for( QMap<int, QString>::Iterator it = contactMap.begin(); 
		     it != contactMap.end(); ++it ){
			switch ( it.key() ){
			case Qtopia::Birthday:{
				// These entries should stored in a special format
				// year-month-day
				QDate day = m_contact.birthday();
				addressbook_db.insert( transMap[it.key()], 
						       QString("%1-%2-%3")
						       .arg( day.year() )
						       .arg( day.month() )
						       .arg( day.day() ) );
			}
				break;
			case Qtopia::Anniversary:{
				// These entries should stored in a special format
				// year-month-day
				QDate day = m_contact.anniversary();
				addressbook_db.insert( transMap[it.key()], 
						       QString("%1-%2-%3")
						       .arg( day.year() )
						       .arg( day.month() )
						       .arg( day.day() ) );
			}
				break;
			case Qtopia::AddressUid: // Ignore UID
				break;
			default: // Translate id to String
				addressbook_db.insert( transMap[it.key()], it.data() );
				break;
			}
		
		}
		
		// Now convert this whole stuff into a SQL String, beginning with
		// the addressbook table..
		QString qu;
		// qu  += "begin transaction;";
		int id = 0;
		for( QMap<QString, QString>::Iterator it = addressbook_db.begin(); 
		     it != addressbook_db.end(); ++it ){
			qu  += "insert into addressbook VALUES(" 
				+  QString::number( m_contact.uid() )
				+ ","
				+  QString::number( id++ ) 
				+ ",'" 
				+ it.key() //.latin1()
				+ "',"
				+ "0"  // Priority for future enhancements
				+ ",'" 
				+ it.data()  //.latin1()
				+ "');";
		}

#endif 	//__STORE_HORIZONTAL_	
		// Now add custom data..
#ifdef __STORE_HORIZONTAL_
		int id = 0;
#endif
		id = 0;
		QMap<QString, QString> customMap = m_contact.toExtraMap();
		for( QMap<QString, QString>::Iterator it = customMap.begin(); 
		     it != customMap.end(); ++it ){
			qu  += "insert into custom_data VALUES(" 
				+  QString::number( m_contact.uid() )
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
		// qu  += "commit;";
		qWarning("add %s", qu.latin1() );
		return qu;
	}
	

	RemoveQuery::RemoveQuery(int uid )
		: OSQLQuery(), m_uid( uid ) {}
	RemoveQuery::~RemoveQuery() {}
	QString RemoveQuery::query()const {
		QString qu = "DELETE from addressbook where uid = " 
			+ QString::number(m_uid) + ";";
		qu += "DELETE from custom_data where uid = " 
			+ QString::number(m_uid) + ";";
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
#ifdef __STORE_HORIZONTAL_
	QString FindQuery::single()const{
		QString qu = "select *";
		qu += " from addressbook where uid = " + QString::number(m_uid);

		// qWarning("find query: %s", qu.latin1() );
		return qu;
	}
#else
	QString FindQuery::single()const{
		QString qu = "select uid, type, value from addressbook where uid = ";
		qu += QString::number(m_uid);
		return qu;
	}
#endif


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

namespace Opie {

OPimContactAccessBackend_SQL::OPimContactAccessBackend_SQL ( const QString& /* appname */, 
						       const QString& filename ): 
	OPimContactAccessBackend(), m_changed(false), m_driver( NULL )
{
	qWarning("C'tor OPimContactAccessBackend_SQL starts");
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

	load();

	qWarning("C'tor OPimContactAccessBackend_SQL ends: %d ms", t.elapsed() );
}

OPimContactAccessBackend_SQL::~OPimContactAccessBackend_SQL ()
{
	if( m_driver )
		delete m_driver;
}

bool OPimContactAccessBackend_SQL::load ()
{
	if (!m_driver->open() )
		return false;

	// Don't expect that the database exists.
	// It is save here to create the table, even if it
	// do exist. ( Is that correct for all databases ?? )
	CreateQuery creat;
	OSQLResult res = m_driver->query( &creat );

	update();

	return true;

}

bool OPimContactAccessBackend_SQL::reload()
{
	return load();
}

bool OPimContactAccessBackend_SQL::save()
{
	return m_driver->close();  // Shouldn't m_driver->sync be better than close ? (eilers)
}


void OPimContactAccessBackend_SQL::clear ()
{
	ClearQuery cle;
	OSQLResult res = m_driver->query( &cle );

	reload();
}

bool OPimContactAccessBackend_SQL::wasChangedExternally()
{
	return false;
}

QArray<int> OPimContactAccessBackend_SQL::allRecords() const
{

	// FIXME: Think about cute handling of changed tables..
	// Thus, we don't have to call update here...
 	if ( m_changed )  
		((OPimContactAccessBackend_SQL*)this)->update();
	
	return m_uids;
}

bool OPimContactAccessBackend_SQL::add ( const OPimContact &newcontact )
{
	InsertQuery ins( newcontact );
	OSQLResult res = m_driver->query( &ins );

	if ( res.state() == OSQLResult::Failure )
		return false;

	int c = m_uids.count();
	m_uids.resize( c+1 );
	m_uids[c] = newcontact.uid();
	
	return true;
}


bool OPimContactAccessBackend_SQL::remove ( int uid )
{
	RemoveQuery rem( uid );
	OSQLResult res = m_driver->query(&rem );

	if ( res.state() == OSQLResult::Failure )
		return false;

	m_changed = true;

	return true;
}

bool OPimContactAccessBackend_SQL::replace ( const OPimContact &contact )
{
	if ( !remove( contact.uid() ) )
		return false;
	
	return add( contact );
}


OPimContact OPimContactAccessBackend_SQL::find ( int uid ) const
{
	qWarning("OPimContactAccessBackend_SQL::find()");
	QTime t;
	t.start();

	OPimContact retContact( requestNonCustom( uid ) );
	retContact.setExtraMap( requestCustom( uid ) );

	qWarning("OPimContactAccessBackend_SQL::find() needed: %d ms", t.elapsed() );
	return retContact;
}



QArray<int> OPimContactAccessBackend_SQL::queryByExample ( const OPimContact &query, int settings, const QDateTime& d = QDateTime() )
{
	QString qu = "SELECT uid FROM addressbook WHERE";

	QMap<int, QString> queryFields = query.toMap();
	QStringList fieldList = OPimContactFields::untrfields( false );
	QMap<QString, int> translate = OPimContactFields::untrFieldsToId();

	// Convert every filled field to a SQL-Query
	bool isAnyFieldSelected = false;
	for ( QStringList::Iterator it = ++fieldList.begin(); it != fieldList.end(); ++it ){
		int id = translate[*it];
		QString queryStr = queryFields[id];
		if ( !queryStr.isEmpty() ){
			isAnyFieldSelected = true;
			switch( id ){
			default:
				// Switching between case sensitive and insensitive...
				// LIKE is not case sensitive, GLOB is case sensitive
				// Do exist a better solution to switch this ?
				if ( settings & OPimContactAccess::IgnoreCase )
					qu += "(\"" + *it + "\"" + " LIKE " + "'" 
						+ queryStr.replace(QRegExp("\\*"),"%") + "'" + ") AND "; 
				else
					qu += "(\"" + *it + "\"" + " GLOB " + "'" 
						+ queryStr + "'" + ") AND "; 
					
			}
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

	return list;		
}

QArray<int> OPimContactAccessBackend_SQL::matchRegexp(  const QRegExp &r ) const
{
	QArray<int> nix(0);
	return nix;
}

const uint OPimContactAccessBackend_SQL::querySettings()
{
	return OPimContactAccess::IgnoreCase 
		|| OPimContactAccess::WildCards;
}

bool OPimContactAccessBackend_SQL::hasQuerySettings (uint querySettings) const
{
	/* OPimContactAccess::IgnoreCase, DateDiff, DateYear, DateMonth, DateDay
	 * may be added with any of the other settings. IgnoreCase should never used alone.
	 * Wildcards, RegExp, ExactMatch should never used at the same time...
	 */

	// Step 1: Check whether the given settings are supported by this backend
	if ( ( querySettings & ( 
				OPimContactAccess::IgnoreCase
				| OPimContactAccess::WildCards
// 				| OPimContactAccess::DateDiff
// 				| OPimContactAccess::DateYear
// 				| OPimContactAccess::DateMonth
// 				| OPimContactAccess::DateDay
// 				| OPimContactAccess::RegExp
// 				| OPimContactAccess::ExactMatch
			       ) ) != querySettings )
		return false;

	// Step 2: Check whether the given combinations are ok..

	// IngoreCase alone is invalid
	if ( querySettings == OPimContactAccess::IgnoreCase )
		return false;

	// WildCards, RegExp and ExactMatch should never used at the same time 
	switch ( querySettings & ~( OPimContactAccess::IgnoreCase
				    | OPimContactAccess::DateDiff
				    | OPimContactAccess::DateYear
				    | OPimContactAccess::DateMonth
				    | OPimContactAccess::DateDay
				    )
		 ){
	case OPimContactAccess::RegExp:
		return ( true );
	case OPimContactAccess::WildCards:
		return ( true );
	case OPimContactAccess::ExactMatch:
		return ( true );
	case 0: // one of the upper removed bits were set..
		return ( true );
	default:
		return ( false );
	}

}

QArray<int> OPimContactAccessBackend_SQL::sorted( bool asc,  int , int ,  int )
{
	QTime t;
	t.start();

#ifdef __STORE_HORIZONTAL_
	QString query = "SELECT uid FROM addressbook ";
	query += "ORDER BY \"Last Name\" ";
#else
	QString query = "SELECT uid FROM addressbook WHERE type = 'Last Name' ";
	query += "ORDER BY upper( value )";
#endif

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


void OPimContactAccessBackend_SQL::update()
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

QArray<int> OPimContactAccessBackend_SQL::extractUids( OSQLResult& res ) const
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
		ints[i] =  (*it).data("uid").toInt();
		i++;
	}
	qWarning("extractUids ready: count2 = %d needs %d ms", i, t.elapsed() );

	return ints;

}

#ifdef __STORE_HORIZONTAL_
QMap<int, QString>  OPimContactAccessBackend_SQL::requestNonCustom( int uid ) const
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
	QStringList fieldList = OPimContactFields::untrfields( false );
	QMap<QString, int> translate = OPimContactFields::untrFieldsToId();
	for ( QStringList::Iterator it = ++fieldList.begin(); it != fieldList.end(); ++it ){
		// Get data for the selected column and store it with the
		// corresponding id into the map..

		int id =  translate[*it];
		QString value = resItem.data( (*it) );
		
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
			     nonCustomMap.insert( id, OPimDateConversion::dateToString( date ) );
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
	qWarning("RequestNonCustom needed: insg.:%d ms, query: %d ms, mapping: %d ms", 
		 t.elapsed(), t2needed, t3needed  );

	return nonCustomMap;
}
#else

QMap<int, QString>  OPimContactAccessBackend_SQL::requestNonCustom( int uid ) const
{
	QTime t;
	t.start();

	QMap<int, QString> nonCustomMap;
	
	int t2needed = 0;
	QTime t2;
	t2.start();
	FindQuery query( uid );
	OSQLResult res_noncustom = m_driver->query( &query );
	t2needed = t2.elapsed();

	if ( res_noncustom.state() == OSQLResult::Failure ) {
		qWarning("OSQLResult::Failure in find query !!");
		QMap<int, QString> empty;
		return empty;
	}	

	int t3needed = 0;
	QTime t3;
	t3.start();
	QMap<QString, int> translateMap = OPimContactFields::untrFieldsToId();

	OSQLResultItem::ValueList list = res_noncustom.results();
	OSQLResultItem::ValueList::Iterator it = list.begin();
	for ( ; it != list.end(); ++it ) {
		if ( (*it).data("type") != "" ){
			int typeId = translateMap[(*it).data( "type" )];
			switch( typeId ){
			case Qtopia::Birthday:
			case Qtopia::Anniversary:{
				// Birthday and Anniversary are encoded special ( yyyy-mm-dd )
				QStringList list = QStringList::split( '-', (*it).data( "value" ) );
				QStringList::Iterator lit = list.begin();
				int year  = (*lit).toInt();
				qWarning("1. %s", (*lit).latin1());
				int month = (*(++lit)).toInt();
				qWarning("2. %s", (*lit).latin1());
				int day   = (*(++lit)).toInt();
				qWarning("3. %s", (*lit).latin1());
				qWarning( "RequestNonCustom->Converting:%s to Year: %d, Month: %d, Day: %d ", (*it).data( "value" ).latin1(), year, month, day );
				QDate date( year, month, day );
				nonCustomMap.insert( typeId, OPimDateConversion::dateToString( date ) );
			}
				break;
			default:
				nonCustomMap.insert( typeId, 
						     (*it).data( "value" ) );
			}
		}
	}
	// Add UID to Map..
	nonCustomMap.insert( Qtopia::AddressUid, QString::number( uid ) );
	t3needed = t3.elapsed();

	qWarning("RequestNonCustom needed: insg.:%d ms, query: %d ms, mapping: %d ms", t.elapsed(), t2needed, t3needed  );
	return nonCustomMap;
}

#endif // __STORE_HORIZONTAL_

QMap<QString, QString>  OPimContactAccessBackend_SQL::requestCustom( int uid ) const
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

}
