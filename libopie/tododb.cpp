
#include <qdir.h>
#include <opie/tododb.h>
#include <opie/xmltree.h>
#include <qpe/palmtoprecord.h>
#include <qpe/global.h>

ToDoDB::ToDoDB(const QString &fileName = QString::null ){
    m_fileName = fileName;
    if( fileName.isEmpty() ){
	m_fileName = Global::applicationFileName("todolist","todolist.xml");;
	qWarning("%s", m_fileName.latin1() );
    }
    
    load();
}
ToDoDB::~ToDoDB()
{

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
void ToDoDB::reload()
{
    load();
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
    qWarning("loading tododb" );
    m_todos.clear();
    XMLElement *root = XMLElement::load( m_fileName );
    if(root != 0l ){ // start parsing
	qWarning("ToDoDB::load tagName(): %s", root->tagName().latin1()  );
	//if( root->tagName() == QString::fromLatin1("Tasks" ) ){// Start
	    XMLElement *element = root->firstChild();
	    element = element->firstChild();
	    while( element ){
		qWarning("ToDoDB::load element tagName() : %s", element->tagName().latin1() );
		QString dummy;
		ToDoEvent event;
		bool ok;
		int dumInt;
                // completed
		dummy = element->attribute("Completed" );
		dumInt = dummy.toInt(&ok );
		if(ok ) event.setCompleted( dumInt == 0 ? false : true );
                // hasDate
		dummy = element->attribute("HasDate" );
		dumInt = dummy.toInt(&ok );
		if(ok ) event.setHasDate( dumInt == 0 ? false: true );
                // set the date
		bool hasDa = dumInt;
		if ( hasDa ) { //parse the date
		    int year, day, month = 0;
		    year = day = month;
                    // year
		    dummy = element->attribute("DateYear" );
		    dumInt = dummy.toInt(&ok );
		    if( ok ) year = dumInt;
                    // month
		    dummy = element->attribute("DateMonth" );
		    dumInt = dummy.toInt(&ok );
		    if(ok ) month = dumInt;
		    dummy = element->attribute("DateDay" );
		    dumInt = dummy.toInt(&ok );
		    if(ok ) day = dumInt;
                    // set the date
		    QDate date( year, month, day );
		    event.setDate( date);
		}
		dummy = element->attribute("Priority" );
		dumInt = dummy.toInt(&ok );
		if(!ok ) dumInt = ToDoEvent::NORMAL;
		event.setPriority( dumInt );
                //description
		dummy = element->attribute("Description" );
		event.setDescription( dummy );
                // category
		dummy = element->attribute("Categories" );
		dumInt = dummy.toInt(&ok );
		if(ok ) {
		    QArray<int> arrat(1);
		    arrat[0] = dumInt;
		    event.setCategory( Qtopia::Record::idsToString( arrat ) );
		}
                //uid
		dummy = element->attribute("Uid" );
		dumInt = dummy.toInt(&ok );
		if(ok ) event.m_uid = dumInt;
		m_todos.append( event );
                element = element->nextChild(); // next element
	    }
	    //}
    }else {
	qWarning("could not load" );
    }
    delete root;
}
bool ToDoDB::save()
{
// prepare the XML
    XMLElement *tasks = new XMLElement( );
    tasks->setTagName("Tasks" );
    for( QValueList<ToDoEvent>::Iterator it = m_todos.begin(); it != m_todos.end(); ++it ){
	XMLElement::AttributeMap map;
	XMLElement *task = new XMLElement();
	map.insert( "Completed", QString::number((int)(*it).isCompleted() ) );
	map.insert( "HasDate", QString::number((int)(*it).hasDate() ) );
	map.insert( "Priority", QString::number( (*it).priority() ) );
	if(!(*it).category().isEmpty() ){
	    QArray<int> arrat(1);
	    arrat = Qtopia::Record::idsFromString(  (*it).category() );
	    map.insert( "Categories", QString::number( arrat[0] ) );
	}else
	    map.insert( "Categories", QString::null );
	map.insert( "Description", (*it).description() );
	if( (*it).hasDate() ){
	    map.insert("DateYear",  QString::number( (*it).date().year()  ) );
	    map.insert("DateMonth", QString::number( (*it).date().month() ) );
	    map.insert("DateDay", QString::number( (*it).date().day()  ) );
	}
	map.insert("Uid", QString::number( (*it).uid() ) );
	task->setTagName("Task" );
	task->setAttributes( map );
	tasks->appendChild(task);
    }
    QFile file( m_fileName);
    if( file.open(IO_WriteOnly ) ){
	QTextStream stream(&file );
	stream << "<!DOCTYPE Tasks>" << endl;
	tasks->save(stream );
	delete tasks;
	file.close();
	return true;
    }
    return false;
}








