
#include <qdir.h>

#include <qpe/palmtoprecord.h>
#include <qpe/global.h>


#include <opie/tododb.h>
#include <opie/xmltree.h>
#include <opie/todoresource.h>
#include <opie/todoxmlresource.h>

using namespace Opie;


ToDoDB::ToDoDB(const QString &fileName, ToDoResource *res ){
    m_fileName = fileName;
    if( fileName.isEmpty() && res == 0 ){
	m_fileName = Global::applicationFileName("todolist","todolist.xml");
	res = new FileToDoResource();
	//qWarning("%s", m_fileName.latin1() );
    }else if(res == 0 ){ // let's create a ToDoResource for xml
      res = new FileToDoResource();
    }
    m_res = res;
    load();
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
QValueList<ToDoEvent> ToDoDB::effectiveToDos(const QDate &from, const QDate &to,
					     bool all )
{
    QValueList<ToDoEvent> events;
    for( QValueList<ToDoEvent>::Iterator it = m_todos.begin(); it!= m_todos.end(); ++it ){
	if( (*it).hasDate() ){
	    if( (*it).date() >= from && (*it).date() <= to )
		events.append( (*it) );
	}else if( all ){
	    events.append( (*it) );
	}
    }
    return events;
}
QValueList<ToDoEvent> ToDoDB::effectiveToDos(const QDate &from,
					     bool all)
{
    return effectiveToDos( from, QDate::currentDate(), all );
}
QValueList<ToDoEvent> ToDoDB::overDue()
{
    QValueList<ToDoEvent> events;
    for( QValueList<ToDoEvent>::Iterator it = m_todos.begin(); it!= m_todos.end(); ++it ){
	if( (*it).isOverdue() )
	    events.append((*it) );
    }
    return events;
}
QValueList<ToDoEvent> ToDoDB::rawToDos()
{
    return m_todos;
}
void ToDoDB::addEvent( const ToDoEvent &event )
{
    m_todos.append( event );
}
void ToDoDB::editEvent( const ToDoEvent &event )
{
    m_todos.remove( event );
    m_todos.append( event );
}
void ToDoDB::removeEvent( const ToDoEvent &event )
{
    m_todos.remove( event );
}
void ToDoDB::replaceEvent(const ToDoEvent &event )
{
  QValueList<ToDoEvent>::Iterator it;
  int uid = event.uid();
  // == is not overloaded as we would like :( so let's search for the uid
  for(it = m_todos.begin(); it != m_todos.end(); ++it ){
    if( (*it).uid() == uid ){
      m_todos.remove( (*it) );
      break; // should save us the iterate is now borked
    }
  }
  m_todos.append(event);
}
void ToDoDB::reload()
{
    load();
}
void ToDoDB::mergeWith(const QValueList<ToDoEvent>& events )
{
  QValueList<ToDoEvent>::ConstIterator it;
  for( it = events.begin(); it != events.end(); ++it ){
    replaceEvent( (*it) );
  }
}
void ToDoDB::setFileName(const QString &file )
{
    m_fileName =file;
}
QString ToDoDB::fileName()const
{
    return m_fileName;
}
void ToDoDB::load()
{
  m_todos = m_res->load( m_fileName );
}
bool ToDoDB::save()
{
  return m_res->save( m_fileName, m_todos );
}








