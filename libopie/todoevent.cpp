
#include <opie/todoevent.h>


#include <qpe/palmtopuidgen.h>
#include <qpe/stringutil.h>
#include <qpe/palmtoprecord.h>

#include <qpe/stringutil.h>
#include <qpe/categories.h>
#include <qpe/categoryselect.h>

#include <qobject.h>

using namespace Opie;

ToDoEvent::ToDoEvent(const ToDoEvent &event )
{
  *this = event;
}

ToDoEvent::ToDoEvent(bool completed, int priority,
                     const QStringList &category,
                     const QString& summary,
                     const QString &description,
                     ushort progress,
                     bool hasDate, QDate date, int uid )
{
    m_date = date;
    m_isCompleted = completed;
    m_hasDate = hasDate;
    m_priority = priority;
    m_category = category;
    m_sum = summary;
    m_prog = progress;
    m_desc = Qtopia::simplifyMultiLineSpace(description );
    if (uid == -1 ) {
	Qtopia::UidGen *uidgen = new Qtopia::UidGen();
	uid = uidgen->generate();
	delete uidgen;
    }// generate the ids
    m_uid = uid;
    m_hasAlarmDateTime = false;

}
QArray<int> ToDoEvent::categories()const
{
  QArray<int> array(m_category.count() ); // currently the datebook can be only in one category
    array = Qtopia::Record::idsFromString( m_category.join(";") );
  return array;
}
bool ToDoEvent::match( const QRegExp &regExp )const
{
  if( QString::number( m_priority ).find( regExp ) != -1 ){
    return true;
  }else if( m_hasDate && m_date.toString().find( regExp) != -1 ){
    return true;
  }else if(m_desc.find( regExp ) != -1 ){
    return true;
  }
  return false;
}
bool ToDoEvent::isCompleted() const
{
    return m_isCompleted;
}
bool ToDoEvent::hasDueDate() const
{
    return m_hasDate;
}
bool ToDoEvent::hasAlarmDateTime() const
{
    return m_hasAlarmDateTime;
}
int ToDoEvent::priority()const
{
    return m_priority;
}
QStringList ToDoEvent::allCategories()const
{
    return m_category;
}
QString ToDoEvent::extra(const QString& )const
{
    return QString::null;
}
QString ToDoEvent::summary() const
{
    return m_sum;
}
ushort ToDoEvent::progress() const
{
    return m_prog;
}
QStringList ToDoEvent::relatedApps() const
{
    QStringList list;
    QMap<QString, QArray<int> >::ConstIterator it;
    for ( it = m_relations.begin(); it != m_relations.end(); ++it ) {
        list << it.key();
    }
    return list;
}
QArray<int> ToDoEvent::relations( const QString& app)const
{
    QArray<int> tmp;
    QMap<QString, QArray<int> >::ConstIterator it;
    it = m_relations.find( app);
    if ( it != m_relations.end() )
        tmp = it.data();
    return tmp;
}
void ToDoEvent::insertCategory(const QString &str )
{
  m_category.append( str );
}
void ToDoEvent::clearCategories()
{
  m_category.clear();
}
void ToDoEvent::setCategories(const QStringList &list )
{
  m_category = list;
}
QDate ToDoEvent::dueDate()const
{
    return m_date;
}

QDateTime ToDoEvent::alarmDateTime() const
{
	return m_alarmDateTime;
}

QString ToDoEvent::description()const
{
    return m_desc;
}
void ToDoEvent::setCompleted( bool completed )
{
    m_isCompleted = completed;
}
void ToDoEvent::setHasDueDate( bool hasDate )
{
    m_hasDate = hasDate;
}
void ToDoEvent::setHasAlarmDateTime( bool hasAlarmDateTime )
{
    m_hasAlarmDateTime = hasAlarmDateTime;
}
void ToDoEvent::setDescription(const QString &desc )
{
    m_desc = Qtopia::simplifyMultiLineSpace(desc );
}
void ToDoEvent::setExtra( const QString&, const QString& )
{

}
void ToDoEvent::setSummary( const QString& sum )
{
    m_sum = sum;
}
void ToDoEvent::setCategory( const QString &cat )
{
  qWarning("setCategory %s", cat.latin1() );
  m_category.clear();
  m_category << cat;
}
void ToDoEvent::setPriority(int prio )
{
    m_priority = prio;
}
void ToDoEvent::setDueDate( QDate date )
{
    m_date = date;
}
void ToDoEvent::setAlarmDateTime( const QDateTime& alarm )
{
    m_alarmDateTime = alarm;
}
void ToDoEvent::addRelated( const QString &app,  int id )
{
    QMap<QString, QArray<int> >::Iterator  it;
    QArray<int> tmp;
    it = m_relations.find( app );
    if ( it == m_relations.end() ) {
        tmp.resize(1 );
        tmp[0] = id;
    }else{
        tmp = it.data();
        tmp.resize( tmp.size() + 1 );
        tmp[tmp.size() - 1] = id;
    }
    m_relations.replace( app,  tmp );
}
void ToDoEvent::addRelated(const QString& app,  QArray<int> ids )
{
    QMap<QString, QArray<int> >::Iterator it;
    QArray<int> tmp;
    it = m_relations.find( app);
    if ( it == m_relations.end() ) { // not there
        /** tmp.resize( ids.size() ); stupid??
         */
        tmp = ids;
    }else{
        tmp = it.data();
        int offset = tmp.size()-1;
        tmp.resize( tmp.size() + ids.size() );
        for (uint i = 0; i < ids.size(); i++ ) {
            tmp[offset+i] = ids[i];
        }

    }
    m_relations.replace( app,  tmp );
}
void ToDoEvent::clearRelated( const QString& app )
{
    m_relations.remove( app );
}
bool ToDoEvent::isOverdue( )
{
    if( m_hasDate )
	return QDate::currentDate() > m_date;
    return false;
}
void ToDoEvent::setProgress(ushort progress )
{
    m_prog = progress;
}
/*!
  Returns a richt text string
*/
QString ToDoEvent::richText() const
{
  QString text;
  QStringList catlist;

  // Description of the todo
  if ( !summary().isEmpty() ) {
    text += "<b>" + QObject::tr( "Summary:") + "</b><br>";
    text += Qtopia::escapeString(summary() ).replace(QRegExp( "[\n]"),  "<br>" ) + "<br>";
  }
  if( !description().isEmpty() ){
    text += "<b>" + QObject::tr( "Description:" ) + "</b><br>";
    text += Qtopia::escapeString(description() ).replace(QRegExp( "[\n]"), "<br>" ) ;
  }
  text += "<br><br><br>";

  text += "<b>" + QObject::tr( "Priority:") +" </b>"
    +  QString::number( priority() ) + " <br>";
  text += "<b>" + QObject::tr( "Progress:") + " </b>"
          + QString::number( progress() ) + " %<br>";
  if (hasDueDate() ){
    text += "<b>" + QObject::tr( "Deadline:") + " </b>";
    text += dueDate().toString();
    text += "<br>";
  }
  if (hasAlarmDateTime() ){
    text += "<b>" + QObject::tr( "Alarmed Notification:") + " </b>";
    text += alarmDateTime().toString();
    text += "<br>";
  }

  // Open database of all categories and get the list of
  // the categories this todoevent belongs to.
  // Then print them...
  // I am not sure whether there is no better way doing this !?
  Categories catdb;
  bool firstloop = true;
  catdb.load( categoryFileName() );
  catlist = allCategories();

  text += "<b>" + QObject::tr( "Category:") + "</b> ";
  for ( QStringList::Iterator it = catlist.begin(); it != catlist.end(); ++it ) {
    if (!firstloop){
      text += ", ";
    }
    firstloop = false;
    text += catdb.label ("todo", (*it).toInt());
  }
  text += "<br>";
  return text;
}

bool ToDoEvent::operator<( const ToDoEvent &toDoEvent )const{
    if( !hasDueDate() && !toDoEvent.hasDueDate() ) return true;
    if( !hasDueDate() && toDoEvent.hasDueDate() ) return false;
    if( hasDueDate() && toDoEvent.hasDueDate() ){
	if( dueDate() == toDoEvent.dueDate() ){ // let's the priority decide
	    return priority() < toDoEvent.priority();
	}else{
	    return dueDate() < toDoEvent.dueDate();
	}
    }
    return false;
}
bool ToDoEvent::operator<=(const ToDoEvent &toDoEvent )const
{
    if( !hasDueDate() && !toDoEvent.hasDueDate() ) return true;
    if( !hasDueDate() && toDoEvent.hasDueDate() ) return true;
    if( hasDueDate() && toDoEvent.hasDueDate() ){
	if( dueDate() == toDoEvent.dueDate() ){ // let's the priority decide
	    return priority() <= toDoEvent.priority();
	}else{
	    return dueDate() <= toDoEvent.dueDate();
	}
    }
    return true;
}
bool ToDoEvent::operator>(const ToDoEvent &toDoEvent )const
{
    if( !hasDueDate() && !toDoEvent.hasDueDate() ) return false;
    if( !hasDueDate() &&  toDoEvent.hasDueDate() ) return false;
    if( hasDueDate()  &&  toDoEvent.hasDueDate() ){
	if( dueDate() == toDoEvent.dueDate() ){ // let's the priority decide
	    return priority() > toDoEvent.priority();
	}else{
	    return dueDate() > toDoEvent.dueDate();
	}
    }
    return false;
}
bool ToDoEvent::operator>=(const ToDoEvent &toDoEvent )const
{
    if( !hasDueDate() && !toDoEvent.hasDueDate() ) return true;
    if( !hasDueDate() &&  toDoEvent.hasDueDate() ) return false;
    if( hasDueDate() && toDoEvent.hasDueDate() ){
	if( dueDate() == toDoEvent.dueDate() ){ // let's the priority decide
	    return priority() > toDoEvent.priority();
	}else{
	    return dueDate() > toDoEvent.dueDate();
	}
    }
    return true;
}
bool ToDoEvent::operator==(const ToDoEvent &toDoEvent )const
{
    if( m_priority == toDoEvent.m_priority &&
        m_priority == toDoEvent.m_prog &&
        m_isCompleted == toDoEvent.m_isCompleted &&
        m_hasDate == toDoEvent.m_hasDate &&
        m_date == toDoEvent.m_date &&
        m_category == toDoEvent.m_category &&
        m_sum == toDoEvent.m_sum &&
        m_desc == toDoEvent.m_desc &&
	m_hasAlarmDateTime == toDoEvent.m_hasAlarmDateTime &&
	m_alarmDateTime == toDoEvent.m_alarmDateTime )
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
    m_sum = item.m_sum;
    m_prog = item.m_prog;
    m_extra = item.m_extra;
    m_relations = item.m_relations;
    m_hasAlarmDateTime = item.m_hasAlarmDateTime;
    m_alarmDateTime = item.m_alarmDateTime;

    return *this;
}

QMap<int, QString> ToDoEvent::toMap() const {
    QMap<int, QString> map;

    map.insert( Uid, QString::number( m_uid ) );
    map.insert( Category,  m_category.join(";") );
    map.insert( HasDate, QString::number( m_hasDate ) );
    map.insert( Completed, QString::number( m_isCompleted ) );
    map.insert( Description, m_desc );
    map.insert( Summary, m_sum );
    map.insert( Priority, QString::number( m_priority ) );
    map.insert( DateDay,  QString::number( m_date.day() ) );
    map.insert( DateMonth, QString::number( m_date.month() ) );
    map.insert( DateYear, QString::number( m_date.year() ) );
    map.insert( Progress, QString::number( m_prog ) );
    map.insert( CrossReference, crossToString() );
    map.insert( HasAlarmDateTime,  QString::number( m_hasAlarmDateTime ) );
    map.insert( AlarmDateTime, m_alarmDateTime.toString() );

    return map;
}


QString ToDoEvent::crossToString()const {
    QString str;
    QMap<QString, QArray<int> >::ConstIterator it;
    for (it = m_relations.begin(); it != m_relations.end(); ++it ) {
        QArray<int> id = it.data();
        for ( uint i = 0; i < id.size(); ++i ) {
            str += it.key() + "," + QString::number( i ) + ";";
        }
    }
    str = str.remove( str.length()-1, 1); // strip the ;
    qWarning("IDS " + str );

    return str;
}




