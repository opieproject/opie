/*
                             This file is part of the Opie Project
                             Copyright (C) Stefan Eilers (Eilers.Stefan@epost.de)
                             Copyright (C) Holger Freyther (zecke@handhelds.org)
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

#include <qdatetime.h>
#include <qmap.h>
#include <qstring.h>

#include <qpe/global.h>

#include <opie2/osqldriver.h>
#include <opie2/osqlresult.h>
#include <opie2/osqlmanager.h>
#include <opie2/osqlquery.h>

#include <opie2/otodoaccesssql.h>
#include <opie2/opimstate.h>
#include <opie2/opimnotifymanager.h>
#include <opie2/opimrecurrence.h>

using namespace Opie::DB;

using namespace Opie;
/*
 * first some query
 * CREATE query
 * LOAD query
 * INSERT
 * REMOVE
 * CLEAR
 */
namespace {
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
     * inserts/adds a OPimTodo to the table
     */
    class InsertQuery : public OSQLQuery {
    public:
        InsertQuery(const OPimTodo& );
        ~InsertQuery();
        QString query()const;
    private:
        OPimTodo m_todo;
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
     * Clears (delete) a Table
     */
    class ClearQuery : public OSQLQuery {
    public:
        ClearQuery();
        ~ClearQuery();
        QString query()const;

    };

    /**
     * a find query
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
     * overdue query
     */
    class OverDueQuery : public OSQLQuery {
    public:
        OverDueQuery();
        ~OverDueQuery();
        QString query()const;
    };
    class EffQuery : public OSQLQuery {
    public:
        EffQuery( const QDate&, const QDate&, bool inc );
        ~EffQuery();
        QString query()const;
    private:
        QString with()const;
        QString out()const;
        QDate m_start;
        QDate m_end;
        bool m_inc :1;
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



    CreateQuery::CreateQuery() : OSQLQuery() {}
    CreateQuery::~CreateQuery() {}
    QString CreateQuery::query()const {
        QString qu;
        qu += "create table todolist( uid PRIMARY KEY, categories, completed, ";
        qu += "description, summary, priority, DueDate, progress ,  state, ";
	// This is the recurrance-stuff .. Exceptions are currently not supported (see OPimRecurrence.cpp) ! (eilers)
	qu += "RType, RWeekdays, RPosition, RFreq, RHasEndDate, EndDate, Created, Exceptions, ";
	qu += "reminders, alarms, maintainer, startdate, completeddate);";
	qu += "create table custom_data( uid INTEGER, id INTEGER, type VARCHAR(10), priority INTEGER, value VARCHAR(10), PRIMARY KEY /* identifier */ (uid, id) );";
        return qu;
    }

    LoadQuery::LoadQuery() : OSQLQuery() {}
    LoadQuery::~LoadQuery() {}
    QString LoadQuery::query()const {
        QString qu;
	// We do not need "distinct" here. The primary key is always unique..
        //qu += "select distinct uid from todolist";
        qu += "select uid from todolist";

        return qu;
    }

    InsertQuery::InsertQuery( const OPimTodo& todo )
        : OSQLQuery(), m_todo( todo ) {
    }
    InsertQuery::~InsertQuery() {
    }
    /*
     * converts from a OPimTodo to a query
     * we leave out X-Ref + Maintainer
     * FIXME: Implement/Finish toMap()/fromMap() into OpimTodo to move the encoding
     *        decoding stuff there.. (eilers)
     */
    QString InsertQuery::query()const{

        int year, month, day;
        year = month = day = 0;
        if (m_todo.hasDueDate() ) {
            QDate date = m_todo.dueDate();
            year = date.year();
            month = date.month();
            day = date.day();
	}
	int sYear = 0, sMonth = 0, sDay = 0;
	if( m_todo.hasStartDate() ){
		QDate sDate = m_todo.startDate();
		sYear = sDate.year();
		sMonth= sDate.month();
		sDay  = sDate.day();
	}

	int eYear = 0, eMonth = 0, eDay = 0;
	if( m_todo.hasCompletedDate() ){
		QDate eDate = m_todo.completedDate();
		eYear = eDate.year();
		eMonth= eDate.month();
		eDay  = eDate.day();
	}
        QString qu;
	QMap<int, QString> recMap = m_todo.recurrence().toMap();
        qu  = "insert into todolist VALUES("
		+  QString::number( m_todo.uid() ) + ","
		+ "'" + m_todo.idsToString( m_todo.categories() ) + "'" + ","
		+       QString::number( m_todo.isCompleted() )         + ","
		+ "'" + m_todo.description()                      + "'" + ","
		+ "'" + m_todo.summary()                          + "'" + ","
		+       QString::number(m_todo.priority() )             + ","
		+ "'" + QString::number(year).rightJustify( 4, '0' ) + "-"
		+       QString::number(month).rightJustify( 2, '0' )
		+       "-" + QString::number( day ).rightJustify( 2, '0' )+ "'" + ","
		+       QString::number( m_todo.progress() )            + ","
 		+       QString::number( m_todo.state().state() )       + ","
		+ "'" + recMap[ OPimRecurrence::RType ]                   + "'" + ","
		+ "'" + recMap[ OPimRecurrence::RWeekdays ]               + "'" + ","
		+ "'" + recMap[ OPimRecurrence::RPosition ]               + "'" + ","
		+ "'" + recMap[ OPimRecurrence::RFreq ]                   + "'" + ","
		+ "'" + recMap[ OPimRecurrence::RHasEndDate ]             + "'" + ","
		+ "'" + recMap[ OPimRecurrence::EndDate ]                 + "'" + ","
		+ "'" + recMap[ OPimRecurrence::Created ]                 + "'" + ","
		+ "'" + recMap[ OPimRecurrence::Exceptions ]              + "'" + ",";

	if ( m_todo.hasNotifiers() ) {
		OPimNotifyManager manager = m_todo.notifiers();
		qu += "'" + manager.remindersToString()           + "'" + ","
			+ "'" + manager.alarmsToString()          + "'" + ",";
	}
	else{
		qu += QString( "''" )                                   + ","
			+ "''"                                          + ",";
	}

	qu +=   QString( "''" )                             + QString( "," ) // Maintainers (cur. not supported !)
		+ "'" + QString::number(sYear).rightJustify( 4, '0' ) + "-"
		+ QString::number(sMonth).rightJustify( 2, '0' )
		+ "-" + QString::number(sDay).rightJustify( 2, '0' )+ "'" + ","
		+ "'" + QString::number(eYear).rightJustify( 4, '0' ) + "-"
		+       QString::number(eMonth).rightJustify( 2, '0' )
		+       "-"+QString::number(eDay).rightJustify( 2, '0' ) + "'"
		+ "); ";

	// Save custom Entries:
	int id = 0;
	id = 0;
	QMap<QString, QString> customMap = m_todo.toExtraMap();
	for( QMap<QString, QString>::Iterator it = customMap.begin();
	     it != customMap.end(); ++it ){
		qu  += "insert into custom_data VALUES("
			+  QString::number( m_todo.uid() )
			+ ","
			+  QString::number( id++ )
			+ ",'"
			+ it.key()
			+ "',"
			+ "0" // Priority for future enhancements
			+ ",'"
			+ it.data()
			+ "');";
	}


        qDebug("add %s", qu.latin1() );
        return qu;
    }

    RemoveQuery::RemoveQuery(int uid )
        : OSQLQuery(), m_uid( uid ) {}
    RemoveQuery::~RemoveQuery() {}
    QString RemoveQuery::query()const {
        QString qu = "DELETE FROM todolist WHERE uid = " + QString::number(m_uid) + " ;";
	qu += "DELETE FROM custom_data WHERE uid = " + QString::number(m_uid);
        return qu;
    }


    ClearQuery::ClearQuery()
        : OSQLQuery() {}
    ClearQuery::~ClearQuery() {}
    QString ClearQuery::query()const {
        QString qu = "drop table todolist";
        return qu;
    }
    FindQuery::FindQuery(int uid)
        : OSQLQuery(), m_uid(uid ) {
    }
    FindQuery::FindQuery(const QArray<int>& ints)
        : OSQLQuery(), m_uids(ints){
    }
    FindQuery::~FindQuery() {
    }
    QString FindQuery::query()const{
        if (m_uids.count() == 0 )
            return single();
        else
            return multi();
    }
    QString FindQuery::single()const{
        QString qu = "select * from todolist where uid = " + QString::number(m_uid);
        return qu;
    }
    QString FindQuery::multi()const {
        QString qu = "select * from todolist where ";
        for (uint i = 0; i < m_uids.count(); i++ ) {
            qu += " UID = " + QString::number( m_uids[i] ) + " OR";
        }
        qu.remove( qu.length()-2, 2 );
        return qu;
    }

    OverDueQuery::OverDueQuery(): OSQLQuery() {}
    OverDueQuery::~OverDueQuery() {}
    QString OverDueQuery::query()const {
        QDate date = QDate::currentDate();
        QString str;
        str = QString("select uid from todolist where DueDate ='%1-%2-%3'")
		.arg( QString::number( date.year() ).rightJustify( 4, '0' ) )
		.arg( QString::number( date.month() ).rightJustify( 2, '0' ) )
		.arg( QString::number( date.day() ) .rightJustify( 2, '0' ) );

        return str;
    }


    EffQuery::EffQuery( const QDate& start, const QDate& end, bool inc )
        : OSQLQuery(), m_start( start ), m_end( end ),m_inc(inc) {}
    EffQuery::~EffQuery() {}
    QString EffQuery::query()const {
        return m_inc ? with() : out();
    }
    QString EffQuery::with()const {
        QString str;
        str = QString("select uid from todolist where ( DueDate >= '%1-%2-%3' AND DueDate <= '%4-%5-%6' ) OR DueDate = '0-0-0' ")
		.arg( QString::number( m_start.year() ).rightJustify( 4, '0' ) )
		.arg( QString::number( m_start.month() ).rightJustify( 2, '0' ) )
		.arg( QString::number( m_start.day() ).rightJustify( 2, '0' ) )
		.arg( QString::number( m_end.year() ).rightJustify( 4, '0' ) )
		.arg( QString::number( m_end.month() ).rightJustify( 2, '0' ) )
		.arg( QString::number( m_end.day() ).rightJustify( 2, '0' ) );
        return str;
    }
    QString EffQuery::out()const {
        QString str;
        str = QString("select uid from todolist where DueDate >= '%1-%2-%3' AND  DueDate <= '%4-%5-%6'")
		.arg( QString::number( m_start.year() ).rightJustify( 4, '0' ) )
		.arg( QString::number( m_start.month() ).rightJustify( 2, '0' ) )
		.arg( QString::number( m_start.day() ).rightJustify( 2, '0' ) )
		.arg( QString::number( m_end.year() ).rightJustify( 4, '0' ) )
		.arg( QString::number( m_end.month() ).rightJustify( 2, '0' ) )
		.arg( QString::number( m_end.day() ).rightJustify( 2, '0' ) );

        return str;
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
		return single(); // Multiple requests not supported !
	}
	QString FindCustomQuery::single()const{
		QString qu = "select uid, type, value from custom_data where uid = ";
		qu += QString::number(m_uid);
		return qu;
	}

};


namespace Opie {
OPimTodoAccessBackendSQL::OPimTodoAccessBackendSQL( const QString& file )
	: OPimTodoAccessBackend(),/* m_dict(15),*/ m_driver(NULL), m_dirty(true)
{
    QString fi = file;
    if ( fi.isEmpty() )
        fi = Global::applicationFileName( "todolist", "todolist.db" );
    OSQLManager man;
    m_driver = man.standard();
    m_driver->setUrl(fi);
    // fillDict();
}

OPimTodoAccessBackendSQL::~OPimTodoAccessBackendSQL(){
	if( m_driver )
		delete m_driver;
}

bool OPimTodoAccessBackendSQL::load(){
    if (!m_driver->open() )
        return false;

    CreateQuery creat;
    OSQLResult res = m_driver->query(&creat );

    m_dirty = true;
    return true;
}
bool OPimTodoAccessBackendSQL::reload(){
    return load();
}

bool OPimTodoAccessBackendSQL::save(){
    return m_driver->close();  // Shouldn't m_driver->sync be better than close ? (eilers)
}
QArray<int> OPimTodoAccessBackendSQL::allRecords()const {
    if (m_dirty )
        update();

    return m_uids;
}
QArray<int> OPimTodoAccessBackendSQL::queryByExample( const OPimTodo& , int, const QDateTime& ){
    QArray<int> ints(0);
    return ints;
}
OPimTodo OPimTodoAccessBackendSQL::find(int uid ) const{
    FindQuery query( uid );
    return todo( m_driver->query(&query) );

}
OPimTodo OPimTodoAccessBackendSQL::find( int uid, const QArray<int>& ints,
                                   uint cur, Frontend::CacheDirection dir ) const{
    uint CACHE = readAhead();
    qDebug("searching for %d", uid );
    QArray<int> search( CACHE );
    uint size =0;
    OPimTodo to;

    // we try to cache CACHE items
    switch( dir ) {
        /* forward */
    case 0: // FIXME: Not a good style to use magic numbers here (eilers)
        for (uint i = cur; i < ints.count() && size < CACHE; i++ ) {
            qDebug("size %d %d", size,  ints[i] );
            search[size] = ints[i];
            size++;
        }
        break;
        /* reverse */
    case 1: // FIXME: Not a good style to use magic numbers here (eilers)
        for (uint i = cur; i != 0 && size <  CACHE; i-- ) {
            search[size] = ints[i];
            size++;
        }
        break;
    }
    search.resize( size );
    FindQuery query( search );
    OSQLResult res = m_driver->query( &query  );
    if ( res.state() != OSQLResult::Success )
        return to;

    return todo( res );
}
void OPimTodoAccessBackendSQL::clear() {
    ClearQuery cle;
    OSQLResult res = m_driver->query( &cle );
    CreateQuery qu;
    res = m_driver->query(&qu);
}
bool OPimTodoAccessBackendSQL::add( const OPimTodo& t) {
    InsertQuery ins( t );
    OSQLResult res = m_driver->query( &ins );

    if ( res.state() == OSQLResult::Failure )
        return false;
    int c = m_uids.count();
    m_uids.resize( c+1 );
    m_uids[c] = t.uid();

    return true;
}
bool OPimTodoAccessBackendSQL::remove( int uid ) {
    RemoveQuery rem( uid );
    OSQLResult res = m_driver->query(&rem );

    if ( res.state() == OSQLResult::Failure )
        return false;

    m_dirty = true;
    return true;
}
/*
 * FIXME better set query
 * but we need the cache for that
 * now we remove
 */
bool OPimTodoAccessBackendSQL::replace( const OPimTodo& t) {
    remove( t.uid() );
    bool b= add(t);
    m_dirty = false; // we changed some stuff but the UID stayed the same
    return b;
}
QArray<int> OPimTodoAccessBackendSQL::overDue() {
    OverDueQuery qu;
    return uids( m_driver->query(&qu ) );
}
QArray<int> OPimTodoAccessBackendSQL::effectiveToDos( const QDate& s,
                                                   const QDate& t,
                                                   bool u) {
    EffQuery ef(s, t, u );
    return uids (m_driver->query(&ef) );
}
/*
 *
 */
QArray<int> OPimTodoAccessBackendSQL::sorted( bool asc, int sortOrder,
                                           int sortFilter, int cat ) {
    qDebug("sorted %d, %d", asc, sortOrder );
    QString query;
    query = "select uid from todolist WHERE ";

    /*
     * Sort Filter stuff
     * not that straight forward
     * FIXME: Replace magic numbers
     *
     */
    /* Category */
    if ( sortFilter & 1 ) {
        QString str;
        if (cat != 0 ) str = QString::number( cat );
        query += " categories like '%" +str+"%' AND";
    }
    /* Show only overdue */
    if ( sortFilter & 2 ) {
        QDate date = QDate::currentDate();
        QString due;
        QString base;
        base = QString("DueDate <= '%1-%2-%3' AND completed = 0")
		.arg( QString::number( date.year() ).rightJustify( 4, '0' ) )
		.arg( QString::number( date.month() ).rightJustify( 2, '0' ) )
		.arg( QString::number( date.day() ).rightJustify( 2, '0' ) );
        query += " " + base + " AND";
    }
    /* not show completed */
    if ( sortFilter & 4 ) {
        query += " completed = 0 AND";
    }else{
       query += " ( completed = 1 OR  completed = 0) AND";
    }
    /* strip the end */
    query = query.remove( query.length()-3, 3 );


    /*
     * sort order stuff
     * quite straight forward
     */
    query += "ORDER BY ";
    switch( sortOrder ) {
        /* completed */
    case 0:
        query += "completed";
        break;
    case 1:
        query += "priority";
        break;
    case 2:
        query += "summary";
        break;
    case 3:
        query += "DueDate";
        break;
    }

    if ( !asc ) {
        qDebug("not ascending!");
        query += " DESC";
    }

    qDebug( query );
    OSQLRawQuery raw(query );
    return uids( m_driver->query(&raw) );
}
bool OPimTodoAccessBackendSQL::date( QDate& da, const QString& str ) const{
    if ( str == "0-0-0" )
        return false;
    else{
        int day, year, month;
        QStringList list = QStringList::split("-", str );
        year = list[0].toInt();
        month = list[1].toInt();
        day = list[2].toInt();
        da.setYMD( year, month, day );
        return true;
    }
}
OPimTodo OPimTodoAccessBackendSQL::todo( const OSQLResult& res ) const{
    if ( res.state() == OSQLResult::Failure ) {
        OPimTodo to;
        return to;
    }

    OSQLResultItem::ValueList list = res.results();
    OSQLResultItem::ValueList::Iterator it = list.begin();
    qDebug("todo1");
    OPimTodo to = todo( (*it) );
    cache( to );
    ++it;

    for ( ; it != list.end(); ++it ) {
        qDebug("caching");
        cache( todo( (*it) ) );
    }
    return to;
}
OPimTodo OPimTodoAccessBackendSQL::todo( OSQLResultItem& item )const {
    qDebug("todo(ResultItem)");

    // Request information from addressbook table and create the OPimTodo-object.

    bool hasDueDate = false; QDate dueDate = QDate::currentDate();
    hasDueDate = date( dueDate, item.data("DueDate") );
    QStringList cats = QStringList::split(";", item.data("categories") );

    qDebug("Item is completed: %d", item.data("completed").toInt() );

    OPimTodo to( (bool)item.data("completed").toInt(), item.data("priority").toInt(),
              cats, item.data("summary"), item.data("description"),
              item.data("progress").toUShort(), hasDueDate, dueDate,
              item.data("uid").toInt() );

    bool isOk;
    int prioInt = QString( item.data("priority") ).toInt( &isOk );
    if ( isOk )
	    to.setPriority( prioInt );

    bool hasStartDate = false; QDate startDate = QDate::currentDate();
    hasStartDate = date( startDate, item.data("startdate") );
    bool hasCompletedDate = false; QDate completedDate = QDate::currentDate();
    hasCompletedDate = date( completedDate, item.data("completeddate") );

    if ( hasStartDate )
	    to.setStartDate( startDate );
    if ( hasCompletedDate )
	    to.setCompletedDate( completedDate );

    OPimNotifyManager& manager = to.notifiers();
    manager.alarmsFromString( item.data("alarms") );
    manager.remindersFromString( item.data("reminders") );

    OPimState pimState;
    pimState.setState( QString( item.data("state") ).toInt() );
    to.setState( pimState );

    QMap<int, QString> recMap;
    recMap.insert( OPimRecurrence::RType      , item.data("RType") );
    recMap.insert( OPimRecurrence::RWeekdays  , item.data("RWeekdays") );
    recMap.insert( OPimRecurrence::RPosition  , item.data("RPosition") );
    recMap.insert( OPimRecurrence::RFreq      , item.data("RFreq") );
    recMap.insert( OPimRecurrence::RHasEndDate, item.data("RHasEndDate") );
    recMap.insert( OPimRecurrence::EndDate    , item.data("EndDate") );
    recMap.insert( OPimRecurrence::Created    , item.data("Created") );
    recMap.insert( OPimRecurrence::Exceptions , item.data("Exceptions") );

    OPimRecurrence recur;
    recur.fromMap( recMap );
    to.setRecurrence( recur );

    // Finally load the custom-entries for this UID and put it into the created object
    to.setExtraMap( requestCustom( to.uid() ) );

    return to;
}
OPimTodo OPimTodoAccessBackendSQL::todo( int uid )const {
    FindQuery find( uid );
    return todo( m_driver->query(&find) );
}
/*
 * update the dict
 */
void OPimTodoAccessBackendSQL::fillDict() {

#if 0
    /* initialize dict */
    /*
     * UPDATE dict if you change anything!!!
     * FIXME: Isn't this dict obsolete ? (eilers)
     */
    m_dict.setAutoDelete( TRUE );
    m_dict.insert("Categories" ,     new int(OPimTodo::Category)         );
    m_dict.insert("Uid" ,            new int(OPimTodo::Uid)              );
    m_dict.insert("HasDate" ,        new int(OPimTodo::HasDate)          );
    m_dict.insert("Completed" ,      new int(OPimTodo::Completed)        );
    m_dict.insert("Description" ,    new int(OPimTodo::Description)      );
    m_dict.insert("Summary" ,        new int(OPimTodo::Summary)          );
    m_dict.insert("Priority" ,       new int(OPimTodo::Priority)         );
    m_dict.insert("DateDay" ,        new int(OPimTodo::DateDay)          );
    m_dict.insert("DateMonth" ,      new int(OPimTodo::DateMonth)        );
    m_dict.insert("DateYear" ,       new int(OPimTodo::DateYear)         );
    m_dict.insert("Progress" ,       new int(OPimTodo::Progress)         );
    m_dict.insert("Completed",       new int(OPimTodo::Completed)        ); // Why twice ? (eilers)
    m_dict.insert("CrossReference",  new int(OPimTodo::CrossReference)   );
//    m_dict.insert("HasAlarmDateTime",new int(OPimTodo::HasAlarmDateTime) ); // old stuff (eilers)
//    m_dict.insert("AlarmDateTime",   new int(OPimTodo::AlarmDateTime)    ); // old stuff (eilers)

#endif
}
/*
 * need to be const so let's fool the
 * compiler :(
 */
void OPimTodoAccessBackendSQL::update()const {
    ((OPimTodoAccessBackendSQL*)this)->m_dirty = false;
    LoadQuery lo;
    OSQLResult res = m_driver->query(&lo);
    if ( res.state() != OSQLResult::Success )
        return;

    ((OPimTodoAccessBackendSQL*)this)->m_uids = uids( res );
}
QArray<int> OPimTodoAccessBackendSQL::uids( const OSQLResult& res) const{

    OSQLResultItem::ValueList list = res.results();
    OSQLResultItem::ValueList::Iterator it;
    QArray<int> ints(list.count() );
    qDebug(" count = %d", list.count() );

    int i = 0;
    for (it = list.begin(); it != list.end(); ++it ) {
        ints[i] =  (*it).data("uid").toInt();
        i++;
    }
    return ints;
}

QArray<int> OPimTodoAccessBackendSQL::matchRegexp(  const QRegExp &r ) const
{

#if 0
	QArray<int> empty;
	return empty;

#else
	QString qu = "SELECT uid FROM todolist WHERE (";

	// Do it make sense to search other fields, too ?
	qu += " rlike(\""+ r.pattern() + "\",\"description\") OR";
	qu += " rlike(\""+ r.pattern() + "\",\"summary\")";

	qu += ")";

	qDebug( "query: %s", qu.latin1() );

	OSQLRawQuery raw( qu );
	OSQLResult res = m_driver->query( &raw );

	return uids( res );


#endif

}
QBitArray OPimTodoAccessBackendSQL::supports()const {

	return sup();
}

QBitArray OPimTodoAccessBackendSQL::sup() const{

	QBitArray ar( OPimTodo::CompletedDate + 1 );
	ar.fill( true );
	ar[OPimTodo::CrossReference] = false;
	ar[OPimTodo::State ] = false;
	ar[OPimTodo::Reminders] = false;
	ar[OPimTodo::Notifiers] = false;
	ar[OPimTodo::Maintainer] = false;

	return ar;
}

void OPimTodoAccessBackendSQL::removeAllCompleted(){
	// First we need the uids from all entries which are
	// completed. Then, we just have to remove them...

	QString qu = "SELECT uid FROM todolist WHERE completed = 1";

	OSQLRawQuery raw( qu );
	OSQLResult res = m_driver->query( &raw );

	QArray<int> completed_uids = uids( res );

	qDebug( "Number of completed: %d", completed_uids.size() );

	if ( completed_uids.size() == 0 )
		return;

	qu = "DELETE FROM todolist WHERE (";
	QString query;

	for ( int i = 0; i < completed_uids.size(); i++ ){
		if ( !query.isEmpty() )
			query += " OR ";
		query += QString( "uid = %1" ).arg( completed_uids[i] );
	}
	qu += query + " );";

	// Put remove of custom entries in this query to speed up..
 	qu += "DELETE FORM custom_data WHERE (";
 	query = "";

	for ( int i = 0; i < completed_uids.size(); i++ ){
		if ( !query.isEmpty() )
			query += " OR ";
		query += QString( "uid = %1" ).arg( completed_uids[i] );
	}
	qu += query + " );";

	qDebug( "query: %s", qu.latin1() );

	OSQLRawQuery raw2( qu );
	res = m_driver->query( &raw2 );
	if ( res.state() == OSQLResult::Failure ) {
		qWarning("OPimTodoAccessBackendSQL::removeAllCompleted():Failure in query: %s", qu.latin1() );
	}
}


QMap<QString, QString>  OPimTodoAccessBackendSQL::requestCustom( int uid ) const
{
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

	return customMap;
}




}
