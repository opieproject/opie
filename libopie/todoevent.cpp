
#include <opie/todoevent.h>
#include <qpe/palmtopuidgen.h>
#include <qpe/stringutil.h>
//#include <qpe/palmtoprecord.h>

ToDoEvent::ToDoEvent(const ToDoEvent &event )
{
  *this = event;
}

ToDoEvent::ToDoEvent(bool completed, int priority, const QString &category, 
	       const QString &description, bool hasDate, QDate date, int uid )
{
    qWarning("todoEvent c'tor" );
    m_date = date;
    m_isCompleted = completed;
    m_hasDate = hasDate;
    m_priority = priority;
    m_category = category;
    m_desc = Qtopia::simplifyMultiLineSpace(description );
    if (uid == -1 ) {
	Qtopia::UidGen *uidgen = new Qtopia::UidGen();
	uid = uidgen->generate();
	delete uidgen;
    }// generate the ids
    m_uid = uid; 
}
bool ToDoEvent::isCompleted() const
{
    return m_isCompleted;
}
bool ToDoEvent::hasDate() const
{
    return m_hasDate;
}
int ToDoEvent::priority()const
{
    return m_priority;
}
QString ToDoEvent::category()const
{
    return m_category;
}
QDate ToDoEvent::date()const
{
    return m_date;
}
QString ToDoEvent::description()const
{
    return m_desc;
}
void ToDoEvent::setCompleted( bool completed )
{
    m_isCompleted = completed;
}
void ToDoEvent::setHasDate( bool hasDate )
{
    m_hasDate = hasDate;
}
void ToDoEvent::setDescription(const QString &desc )
{
    m_desc = Qtopia::simplifyMultiLineSpace(desc );
}
void ToDoEvent::setCategory( const QString &cat )
{
    m_category = cat;
}
void ToDoEvent::setPriority(int prio )
{
    m_priority = prio;
}
void ToDoEvent::setDate( QDate date )
{
    m_date = date;
}
bool ToDoEvent::isOverdue( )
{
    if( m_hasDate )
	return QDate::currentDate() > m_date;
    return false;
}
bool ToDoEvent::operator<( const ToDoEvent &toDoEvent )const{
    if( !hasDate() && !toDoEvent.hasDate() ) return true;
    if( !hasDate() && toDoEvent.hasDate() ) return true;
    if( hasDate() && toDoEvent.hasDate() ){
	if( date() == toDoEvent.date() ){ // let's the priority decide
	    return priority() < toDoEvent.priority();
	}else{
	    return date() < toDoEvent.date();
	}
    }
    return false;
}
bool ToDoEvent::operator<=(const ToDoEvent &toDoEvent )const
{
    if( !hasDate() && !toDoEvent.hasDate() ) return true;
    if( !hasDate() && toDoEvent.hasDate() ) return true;
    if( hasDate() && toDoEvent.hasDate() ){
	if( date() == toDoEvent.date() ){ // let's the priority decide
	    return priority() <= toDoEvent.priority();
	}else{
	    return date() <= toDoEvent.date();
	}
    }
    return true;
}
bool ToDoEvent::operator>(const ToDoEvent &toDoEvent )const
{
    if( !hasDate() && !toDoEvent.hasDate() ) return false;
    if( !hasDate() && toDoEvent.hasDate() ) return false;
    if( hasDate() && toDoEvent.hasDate() ){
	if( date() == toDoEvent.date() ){ // let's the priority decide
	    return priority() > toDoEvent.priority();
	}else{
	    return date() > toDoEvent.date();
	}
    }
    return false;
}
bool ToDoEvent::operator>=(const ToDoEvent &toDoEvent )const
{
    if( !hasDate() && !toDoEvent.hasDate() ) return true;
    if( !hasDate() && toDoEvent.hasDate() ) return false;
    if( hasDate() && toDoEvent.hasDate() ){
	if( date() == toDoEvent.date() ){ // let's the priority decide
	    return priority() > toDoEvent.priority();
	}else{
	    return date() > toDoEvent.date();
	}
    }
    return true;
}
bool ToDoEvent::operator==(const ToDoEvent &toDoEvent )const
{
    if( m_date == toDoEvent.m_date && m_isCompleted == toDoEvent.m_isCompleted && m_hasDate == toDoEvent.m_hasDate && m_priority == toDoEvent.m_priority && m_category == toDoEvent.m_category && m_desc == toDoEvent.m_category )
	return true;
    return false;
}
ToDoEvent &ToDoEvent::operator=(const ToDoEvent &item )
{
    m_date = item.m_date;
    m_isCompleted = item.m_isCompleted;
    m_hasDate = item.m_hasDate;
    m_priority = item.m_priority;
    m_category = item.m_category;
    m_desc = item.m_desc;
    m_uid = item.m_uid;
    return *this;
}








