
#include <qdatetime.h>

#include <qpe/global.h>

#include <opie/osqldriver.h>
#include <opie/osqlresult.h>
#include <opie/osqlmanager.h>
#include <opie/osqlquery.h>

#include "otodoaccesssql.h"

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
     * inserts/adds a OTodo to the table
     */
    class InsertQuery : public OSQLQuery {
    public:
        InsertQuery(const OTodo& );
        ~InsertQuery();
        QString query()const;
    private:
        OTodo m_todo;
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


    CreateQuery::CreateQuery() : OSQLQuery() {}
    CreateQuery::~CreateQuery() {}
    QString CreateQuery::query()const {
        QString qu;
        qu += "create table todolist( uid, categories, completed, progress, ";
        qu += "summary, DueDate, priority, description )";
        return qu;
    }

    LoadQuery::LoadQuery() : OSQLQuery() {}
    LoadQuery::~LoadQuery() {}
    QString LoadQuery::query()const {
        QString qu;
        qu += "select distinct uid from todolist";

        return qu;
    }

    InsertQuery::InsertQuery( const OTodo& todo )
        : OSQLQuery(), m_todo( todo ) {
    }
    InsertQuery::~InsertQuery() {
    }
    /*
     * converts from a OTodo to a query
     * we leave out X-Ref + Alarms
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
        QString qu;
        qu  = "insert into todolist VALUES(" +  QString::number( m_todo.uid() ) + ",'" + m_todo.idsToString( m_todo.categories() ) + "',";
        qu += QString::number( m_todo.isCompleted() ) + "," + QString::number( m_todo.progress() ) + ",";
        qu += "'"+m_todo.summary()+"','"+QString::number(year)+"-"+QString::number(month)+"-"+QString::number(day)+"',";
        qu += QString::number(m_todo.priority() ) +",'" + m_todo.description() + "')";

        qWarning("add %s", qu.latin1() );
        return qu;
    }

    RemoveQuery::RemoveQuery(int uid )
        : OSQLQuery(), m_uid( uid ) {}
    RemoveQuery::~RemoveQuery() {}
    QString RemoveQuery::query()const {
        QString qu = "DELETE from todolist where uid = " + QString::number(m_uid);
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
        QString qu = "select uid, categories, completed, progress, summary, ";
        qu += "DueDate, priority, description from todolist where uid = " + QString::number(m_uid);
        return qu;
    }
    QString FindQuery::multi()const {
        QString qu = "select uid, categories, completed, progress, summary, ";
        qu += "DueDate, priority, description from todolist where ";
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
        str = QString("select uid from todolist where DueDate ='%1-%2-%3'").arg(date.year() ).arg(date.month() ).arg(date.day() );

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
            .arg( m_start.year() ).arg( m_start.month() ).arg( m_start.day() )
            .arg( m_end  .year() ).arg( m_end  .month() ).arg( m_end  .day() );
        return str;
    }
    QString EffQuery::out()const {
        QString str;
        str = QString("select uid from todolist where DueDate >= '%1-%2-%3' AND  DueDate <= '%4-%5-%6'")
            .arg(m_start.year() ).arg(m_start.month() ).arg( m_start.day() )
            .arg(m_end.  year() ).arg(m_end.  month() ).arg(m_end.day() );

        return str;
    }
};

OTodoAccessBackendSQL::OTodoAccessBackendSQL( const QString& file )
    : OTodoAccessBackend(), m_dict(15), m_dirty(true)
{
    QString fi = file;
    if ( fi.isEmpty() )
        fi = Global::applicationFileName( "todolist", "todolist.db" );
    OSQLManager man;
    m_driver = man.standard();
    m_driver->setUrl(fi);
    fillDict();
}

OTodoAccessBackendSQL::~OTodoAccessBackendSQL(){
}
bool OTodoAccessBackendSQL::load(){
    if (!m_driver->open() )
        return false;

    CreateQuery creat;
    OSQLResult res = m_driver->query(&creat );

    m_dirty = true;
    return true;
}
bool OTodoAccessBackendSQL::reload(){
    return load();
}

bool OTodoAccessBackendSQL::save(){
    return m_driver->close();
}
QArray<int> OTodoAccessBackendSQL::allRecords()const {
    if (m_dirty )
        update();

    return m_uids;
}
QArray<int> OTodoAccessBackendSQL::queryByExample( const OTodo& , int  ){
    QArray<int> ints(0);
    return ints;
}
OTodo OTodoAccessBackendSQL::find(int uid ) const{
    FindQuery query( uid );
    return todo( m_driver->query(&query) );

}
OTodo OTodoAccessBackendSQL::find( int uid, const QArray<int>& ints,
                                   uint cur, Frontend::CacheDirection dir ) const{
    int CACHE = readAhead();
    qWarning("searching for %d", uid );
    QArray<int> search( CACHE );
    uint size =0;
    OTodo to;

    // we try to cache CACHE items
    switch( dir ) {
        /* forward */
    case 0:
        for (uint i = cur; i < ints.count() && size < CACHE; i++ ) {
            qWarning("size %d %d", size,  ints[i] );
            search[size] = ints[i];
            size++;
        }
        break;
        /* reverse */
    case 1:
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
void OTodoAccessBackendSQL::clear() {
    ClearQuery cle;
    OSQLResult res = m_driver->query( &cle );
    CreateQuery qu;
    res = m_driver->query(&qu);
}
bool OTodoAccessBackendSQL::add( const OTodo& t) {
    InsertQuery ins( t );
    OSQLResult res = m_driver->query( &ins );

    if ( res.state() == OSQLResult::Failure )
        return false;
    int c = m_uids.count();
    m_uids.resize( c+1 );
    m_uids[c] = t.uid();

    return true;
}
bool OTodoAccessBackendSQL::remove( int uid ) {
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
bool OTodoAccessBackendSQL::replace( const OTodo& t) {
    remove( t.uid() );
    bool b= add(t);
    m_dirty = false; // we changed some stuff but the UID stayed the same
    return b;
}
QArray<int> OTodoAccessBackendSQL::overDue() {
    OverDueQuery qu;
    return uids( m_driver->query(&qu ) );
}
QArray<int> OTodoAccessBackendSQL::effectiveToDos( const QDate& s,
                                                   const QDate& t,
                                                   bool u) {
    EffQuery ef(s, t, u );
    return uids (m_driver->query(&ef) );
}
/*
 *
 */
QArray<int> OTodoAccessBackendSQL::sorted( bool asc, int sortOrder,
                                           int sortFilter, int cat ) {
    qWarning("sorted %d, %d", asc, sortOrder );
    QString query;
    query = "select uid from todolist WHERE ";

    /*
     * Sort Filter stuff
     * not that straight forward
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
        base = QString("DueDate <= '%1-%2-%3' AND completed = 0").arg( date.year() ).arg( date.month() ).arg( date.day() );
        query += " " + base + " AND";
    }
    /* not show completed */
    if ( sortFilter & 4 ) {
        query += " completed = 0 AND";
    }else{
       query += " ( completed = 1 OR  completed = 0) AND";
    }
    /* srtip the end */
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
        query += "description";
        break;
    case 3:
        query += "DueDate";
        break;
    }

    if ( !asc ) {
        qWarning("not ascending!");
        query += " DESC";
    }

    qWarning( query );
    OSQLRawQuery raw(query );
    return uids( m_driver->query(&raw) );
}
bool OTodoAccessBackendSQL::date( QDate& da, const QString& str ) const{
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
OTodo OTodoAccessBackendSQL::todo( const OSQLResult& res) const{
    if ( res.state() == OSQLResult::Failure ) {
        OTodo to;
        return to;
    }

    OSQLResultItem::ValueList list = res.results();
    OSQLResultItem::ValueList::Iterator it = list.begin();
    qWarning("todo1");
    OTodo to = todo( (*it) );
    cache( to );
    ++it;

    for ( ; it != list.end(); ++it ) {
        qWarning("caching");
        cache( todo( (*it) ) );
    }
    return to;
}
OTodo OTodoAccessBackendSQL::todo( OSQLResultItem& item )const {
    qWarning("todo");
    bool has = false; QDate da = QDate::currentDate();
    has = date( da, item.data("DueDate") );
    QStringList cats = QStringList::split(";", item.data("categories") );

    OTodo to( (bool)item.data("completed").toInt(), item.data("priority").toInt(),
              cats, item.data("summary"), item.data("description"),
              item.data("progress").toUShort(), has, da,
              item.data("uid").toInt() );
    return to;
}
OTodo OTodoAccessBackendSQL::todo( int uid )const {
    FindQuery find( uid );
    return todo( m_driver->query(&find) );
}
/*
 * update the dict
 */
void OTodoAccessBackendSQL::fillDict() {
    /* initialize dict */
    /*
     * UPDATE dict if you change anything!!!
     */
    m_dict.setAutoDelete( TRUE );
    m_dict.insert("Categories" ,     new int(OTodo::Category)         );
    m_dict.insert("Uid" ,            new int(OTodo::Uid)              );
    m_dict.insert("HasDate" ,        new int(OTodo::HasDate)          );
    m_dict.insert("Completed" ,      new int(OTodo::Completed)        );
    m_dict.insert("Description" ,    new int(OTodo::Description)      );
    m_dict.insert("Summary" ,        new int(OTodo::Summary)          );
    m_dict.insert("Priority" ,       new int(OTodo::Priority)         );
    m_dict.insert("DateDay" ,        new int(OTodo::DateDay)          );
    m_dict.insert("DateMonth" ,      new int(OTodo::DateMonth)        );
    m_dict.insert("DateYear" ,       new int(OTodo::DateYear)         );
    m_dict.insert("Progress" ,       new int(OTodo::Progress)         );
    m_dict.insert("Completed",       new int(OTodo::Completed)        );
    m_dict.insert("CrossReference",  new int(OTodo::CrossReference)   );
    m_dict.insert("HasAlarmDateTime",new int(OTodo::HasAlarmDateTime) );
    m_dict.insert("AlarmDateTime",   new int(OTodo::AlarmDateTime)    );
}
/*
 * need to be const so let's fool the
 * compiler :(
 */
void OTodoAccessBackendSQL::update()const {
    ((OTodoAccessBackendSQL*)this)->m_dirty = false;
    LoadQuery lo;
    OSQLResult res = m_driver->query(&lo);
    if ( res.state() != OSQLResult::Success )
        return;

    ((OTodoAccessBackendSQL*)this)->m_uids = uids( res );
}
QArray<int> OTodoAccessBackendSQL::uids( const OSQLResult& res) const{

    OSQLResultItem::ValueList list = res.results();
    OSQLResultItem::ValueList::Iterator it;
    QArray<int> ints(list.count() );
    qWarning(" count = %d", list.count() );

    int i = 0;
    for (it = list.begin(); it != list.end(); ++it ) {
        ints[i] =  (*it).data("uid").toInt();
        i++;
    }
    return ints;
}

