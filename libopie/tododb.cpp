
#include <qdir.h>

#include <qpe/palmtoprecord.h>
#include <qpe/global.h>
#include <qpe/alarmserver.h>

//#include <opie/tododb.h>
#include <opie/xmltree.h>
#include <opie/todoresource.h>

using namespace Opie;

// Implement the iterator
/*
 * end with a special boolean
 */

/** Iterator Private data */
struct ToDoDB::Iterator::IteratorPrivate {
    QArray<int> m_uids;
    uint m_current; // the current UID
    bool m_end : 1;
    ToDoDB *m_db;
    ToDoEvent m_ev;
};

ToDoDB::Iterator::Iterator() {
    d = new IteratorPrivate;
    d->m_end = false; // not at end
    d->m_current = 0; // but not initialised
    d->m_db = 0;
}
ToDoDB::Iterator::Iterator( const ToDoDB::Iterator& it ) {
    d = new IteratorPrivate;
    d->m_end = it.d->m_end;
    d->m_uids = it.d->m_uids;
    d->m_current = it.d->m_current;
    d->m_db = it.d->m_db;
    d->m_ev = it.d->m_ev;

}
ToDoDB::Iterator &ToDoDB::Iterator::operator=( const ToDoDB::Iterator& it ) {
    // I'm not sure about this one though
    d = new IteratorPrivate;
    d->m_end = it.d->m_end;
    d->m_uids = it.d->m_uids;
    d->m_current = it.d->m_current;
    d->m_db = it.d->m_db;
    d->m_ev = it.d->m_ev;

    return *this;
}
ToDoDB::Iterator::~Iterator() {
    delete d;
}
ToDoEvent &ToDoDB::Iterator::operator*() {
    ToDoEvent ev;
    d->m_ev = ev;
    /* either current not initalised or current is past
     * the size of the array return
     */
    if ( d->m_end ||
        ( d->m_current + 1) > d->m_uids.size() )
        return d->m_ev;

    int uid = d->m_uids[d->m_current];

    bool found = false;
    while (!found ) {
        found = false;
        d->m_ev = d->m_db->findEvent(uid, &found );
    }

    return d->m_ev;
}
ToDoDB::Iterator &ToDoDB::Iterator::operator++() {
    /* at the end */
    if ( d->m_end ||
         (d->m_current + 1) > d->m_uids.size() ) {
        d->m_end = true;
        d->m_current = 0;
        return *this;
    }

    d->m_current++;
    return *this;
}
/*
 * If at end == true
 * if current uid != current id false
 */
bool ToDoDB::Iterator::operator==( const ToDoDB::Iterator& it ) {
    /* either both are at the end -> return true
     * either one of them is at the end the other
     * not -> return false
     * both are not at the end do the usual computing
     */
    if ( d->m_end && it.d->m_end ) return true;
    else if ( d->m_end || it.d->m_end ) return false;

    if ( d->m_uids[d->m_current] !=
         it.d->m_uids[it.d->m_current] ) return false;

    return true;
}
bool ToDoDB::Iterator::operator!=( const ToDoDB::Iterator& it ) {
    return !( *this == it );
}
// off the Iterator

ToDoDB::ToDoDB( ToDoResource *res ){
    /* if no resource was set
     * take the default one
     */
    if( res == 0 ) {
        QString file = Global::applicationFileName("todolist","todolist.xml");
	//res = new FileToDoResource(file, QString::fromLatin1("Default");
    }
    m_res = res;
}
ToDoResource* ToDoDB::resource(){
    return m_res;
};
void ToDoDB::setResource( ToDoResource *res )
{
    delete m_res;
    m_res = res;
}
ToDoDB::~ToDoDB()
{
  delete m_res;
}
ToDoDB::Iterator ToDoDB::effectiveToDos(const QDate &from, const QDate &to,
                                bool all )
{
    Iterator it;
    it.d->m_db = this;
    QArray<int> items = m_res->effectiveToDos( from,
                                               to, all );
    it.d->m_uids = items;

    /* check if we found something */
    if ( items.size() == 0 ) {
        it.d->m_end = true;
    }else {
        it.d->m_current = 0;
        it.d->m_end = false;
    }
    return it;
}
ToDoDB::Iterator ToDoDB::effectiveToDos(const QDate &from,
					     bool all)
{
    return effectiveToDos( from, QDate::currentDate(), all );
}
ToDoDB::Iterator ToDoDB::overDue()
{
    Iterator it;
    it.d->m_db = this;
    it.d->m_uids = m_res->overDue();

    /* same check for the iterator */
    if (it.d->m_uids.size() == 0 ) {
        it.d->m_end = true;
    }else {
        it.d->m_current = 0;
        it.d->m_end = false;
    }
    return it;
}
ToDoDB::Iterator ToDoDB::rawToDos()
{
    Iterator it;
    it.d->m_db = this;
    it.d->m_uids = m_res->rawToDos();

    /* same check but Iterator should
     * only be known to this class */
    if ( it.d->m_uids.size() == 0 ) {
        it.d->m_end = true;
    }else {
        it.d->m_current = 0;
        it.d->m_end= false;
    };
    return it;
}
void ToDoDB::addEvent( const ToDoEvent &event )
{
    addEventAlarm( event );

    m_res->addEvent( event );
}
void ToDoDB::removeEvent( const ToDoEvent &event )
{
    delEventAlarm( event );
    m_res->removeEvent( event );
}
ToDoDB::Iterator ToDoDB::remove( const Iterator& it2) {
    Iterator it = it2;
    m_res->removeEvent( (*it) );
    delEventAlarm( (*it ) );

    /* now update m_current and the size of the array */
    /* actually we won't change the size of the array
     * because we don't have -- implemented
     */
    it.d->m_current++;
    if ( (it.d->m_current + 1) > it.d->m_uids.size() )
        it.d->m_end = true;

    return it;
}
void ToDoDB::replaceEvent(const ToDoEvent &event )
{
    delEventAlarm( event );
    addEventAlarm( event );
    m_res->replaceEvent( event );
}
bool ToDoDB::reload()
{
    return m_res->reload();
}
void ToDoDB::mergeWith(const QValueList<ToDoEvent>& events )
{
    QValueList<ToDoEvent>::ConstIterator it;
    for( it = events.begin(); it != events.end(); ++it ){
        replaceEvent( (*it) );
    }
}
bool ToDoDB::load()
{
    return m_res->load( );
}
bool ToDoDB::save()
{
    return m_res->save(  );
}
ToDoEvent ToDoDB::findEvent(int uid, bool *ok ) {
    return m_res->findEvent( uid, ok );
}
void ToDoDB::addEventAlarm( const ToDoEvent& event )
{
    if (!event.hasAlarmDateTime() )
        return;

    QDateTime now      = QDateTime::currentDateTime();
    QDateTime schedule = event.alarmDateTime();
    if ( schedule > now ){
        AlarmServer::addAlarm( schedule,
                               "QPE/Application/todolist",
                               "alarm(QDateTime,int)", event.uid() );

    }
}

void ToDoDB::delEventAlarm( const ToDoEvent& event )
{
    if (!event.hasAlarmDateTime() )
        return;

    QDateTime schedule; // Create null DateTime

    // I hope this will remove all scheduled alarms
    // with the given uid !?
    // If not: I have to rethink how to remove already
    // scheduled events... (se)
    qWarning("Removing alarm for event with uid %d", event.uid());
    AlarmServer::deleteAlarm( schedule ,
                              "QPE/Application/todolist",
                              "alarm(QDateTime,int)", event.uid() );
}






