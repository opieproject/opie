
#include <qobject.h>
#include <qshared.h>



#include <qpe/palmtopuidgen.h>
#include <qpe/stringutil.h>
#include <qpe/palmtoprecord.h>
#include <qpe/stringutil.h>
#include <qpe/categories.h>
#include <qpe/categoryselect.h>

#include "todoevent.h"

using namespace Opie;

Qtopia::UidGen ToDoEvent::m_gen;

struct ToDoEvent::ToDoEventData : public QShared {
    ToDoEventData() : QShared() {
    };

    QDate date;
    bool isCompleted:1;
    bool hasDate:1;
    int priority;
    QStringList category;
    QString desc;
    QString sum;
    QMap<QString, QString> extra;
    QMap<QString, QArray<int> > relations;
    int uid;
    ushort prog;
    bool hasAlarmDateTime :1;
    QDateTime alarmDateTime;
};

ToDoEvent::ToDoEvent(const ToDoEvent &event )
    : data( event.data )
{
    data->ref();
    qWarning("ref up");
}
ToDoEvent::~ToDoEvent() {
    if ( data->deref() ) {
        qWarning("ToDoEvent::dereffing");
        delete data;
        data = 0l;
    }
}

ToDoEvent::ToDoEvent(bool completed, int priority,
                     const QStringList &category,
                     const QString& summary,
                     const QString &description,
                     ushort progress,
                     bool hasDate, QDate date, int uid )
{
    qWarning("ToDoEventData");
    data = new ToDoEventData;
    data->date = date;
    data->isCompleted = completed;
    data->hasDate = hasDate;
    data->priority = priority;
    data->category = category;
    data->sum = summary;
    data->prog = progress;
    data->desc = Qtopia::simplifyMultiLineSpace(description );
    if (uid == -1 ) {
	uid = m_gen.generate();

    }// generated the ids
    m_gen.store( uid );

    data->uid = uid;
    data->hasAlarmDateTime = false;

}
QArray<int> ToDoEvent::categories()const
{
  QArray<int> array(data->category.count() ); // currently the datebook can be only in one category
    array = Qtopia::Record::idsFromString( data->category.join(";") );
  return array;
}
bool ToDoEvent::match( const QRegExp &regExp )const
{
  if( QString::number( data->priority ).find( regExp ) != -1 ){
    return true;
  }else if( data->hasDate && data->date.toString().find( regExp) != -1 ){
    return true;
  }else if(data->desc.find( regExp ) != -1 ){
    return true;
  }
  return false;
}
bool ToDoEvent::isCompleted() const
{
    return data->isCompleted;
}
bool ToDoEvent::hasDueDate() const
{
    return data->hasDate;
}
bool ToDoEvent::hasAlarmDateTime() const
{
    return data->hasAlarmDateTime;
}
int ToDoEvent::priority()const
{
    return data->priority;
}
QStringList ToDoEvent::allCategories()const
{
    return data->category;
}
QString ToDoEvent::extra(const QString& )const
{
    return QString::null;
}
QString ToDoEvent::summary() const
{
    return data->sum;
}
ushort ToDoEvent::progress() const
{
    return data->prog;
}
QStringList ToDoEvent::relatedApps() const
{
    QStringList list;
    QMap<QString, QArray<int> >::ConstIterator it;
    for ( it = data->relations.begin(); it != data->relations.end(); ++it ) {
        list << it.key();
    }
    return list;
}
QArray<int> ToDoEvent::relations( const QString& app)const
{
    QArray<int> tmp;
    QMap<QString, QArray<int> >::ConstIterator it;
    it = data->relations.find( app);
    if ( it != data->relations.end() )
        tmp = it.data();
    return tmp;
}
void ToDoEvent::insertCategory(const QString &str )
{
    changeOrModify();
    data->category.append( str );
}
void ToDoEvent::clearCategories()
{
    changeOrModify();
    data->category.clear();
}
void ToDoEvent::setCategories(const QStringList &list )
{
    changeOrModify();
    data->category = list;
}
QDate ToDoEvent::dueDate()const
{
    return data->date;
}

QDateTime ToDoEvent::alarmDateTime() const
{
    return data->alarmDateTime;
}

QString ToDoEvent::description()const
{
    return data->desc;
}
void ToDoEvent::setCompleted( bool completed )
{
    changeOrModify();
    data->isCompleted = completed;
}
void ToDoEvent::setHasDueDate( bool hasDate )
{
    changeOrModify();
    data->hasDate = hasDate;
}
void ToDoEvent::setHasAlarmDateTime( bool hasAlarmDateTime )
{
    changeOrModify();
    data->hasAlarmDateTime = hasAlarmDateTime;
}
void ToDoEvent::setDescription(const QString &desc )
{
    changeOrModify();
    data->desc = Qtopia::simplifyMultiLineSpace(desc );
}
void ToDoEvent::setExtra( const QString&, const QString& )
{

}
void ToDoEvent::setSummary( const QString& sum )
{
    changeOrModify();
    data->sum = sum;
}
void ToDoEvent::setCategory( const QString &cat )
{
    changeOrModify();
    qWarning("setCategory %s", cat.latin1() );
    data->category.clear();
    data->category << cat;
}
void ToDoEvent::setPriority(int prio )
{
    changeOrModify();
    data->priority = prio;
}
void ToDoEvent::setDueDate( QDate date )
{
    changeOrModify();
    data->date = date;
}
void ToDoEvent::setAlarmDateTime( const QDateTime& alarm )
{
    changeOrModify();
    data->alarmDateTime = alarm;
}
void ToDoEvent::addRelated( const QString &app,  int id )
{
    changeOrModify();

    QMap<QString, QArray<int> >::Iterator  it;
    QArray<int> tmp;
    it = data->relations.find( app );
    if ( it == data->relations.end() ) {
        tmp.resize(1 );
        tmp[0] = id;
    }else{
        tmp = it.data();
        tmp.resize( tmp.size() + 1 );
        tmp[tmp.size() - 1] = id;
    }
    data->relations.replace( app,  tmp );
}
void ToDoEvent::addRelated(const QString& app,  QArray<int> ids )
{
    changeOrModify();

    QMap<QString, QArray<int> >::Iterator it;
    QArray<int> tmp;
    it = data->relations.find( app);
    if ( it == data->relations.end() ) { // not there
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
    data->relations.replace( app,  tmp );
}
void ToDoEvent::clearRelated( const QString& app )
{
    changeOrModify();
    data->relations.remove( app );
}
bool ToDoEvent::isOverdue( )
{
    if( data->hasDate )
	return QDate::currentDate() > data->date;
    return false;
}
void ToDoEvent::setProgress(ushort progress )
{
    changeOrModify();
    data->prog = progress;
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
    if( data->priority == toDoEvent.data->priority &&
        data->priority == toDoEvent.data->prog &&
        data->isCompleted == toDoEvent.data->isCompleted &&
        data->hasDate == toDoEvent.data->hasDate &&
        data->date == toDoEvent.data->date &&
        data->category == toDoEvent.data->category &&
        data->sum == toDoEvent.data->sum &&
        data->desc == toDoEvent.data->desc &&
	data->hasAlarmDateTime == toDoEvent.data->hasAlarmDateTime &&
	data->alarmDateTime == toDoEvent.data->alarmDateTime )
	return true;

    return false;
}
void ToDoEvent::deref() {

    qWarning("deref in ToDoEvent");
    if ( data->deref() ) {
        qWarning("deleting");
        delete data;
        d= 0;
    }
}
ToDoEvent &ToDoEvent::operator=(const ToDoEvent &item )
{
    qWarning("operator= ref ");
    item.data->ref();
    deref();

    data = item.data;


    return *this;
}

QMap<int, QString> ToDoEvent::toMap() const {
    QMap<int, QString> map;

    map.insert( Uid, QString::number( data->uid ) );
    map.insert( Category,  data->category.join(";") );
    map.insert( HasDate, QString::number( data->hasDate ) );
    map.insert( Completed, QString::number( data->isCompleted ) );
    map.insert( Description, data->desc );
    map.insert( Summary, data->sum );
    map.insert( Priority, QString::number( data->priority ) );
    map.insert( DateDay,  QString::number( data->date.day() ) );
    map.insert( DateMonth, QString::number( data->date.month() ) );
    map.insert( DateYear, QString::number( data->date.year() ) );
    map.insert( Progress, QString::number( data->prog ) );
    map.insert( CrossReference, crossToString() );
    map.insert( HasAlarmDateTime,  QString::number( data->hasAlarmDateTime ) );
    map.insert( AlarmDateTime, data->alarmDateTime.toString() );

    return map;
}


QString ToDoEvent::crossToString()const {
    QString str;
    QMap<QString, QArray<int> >::ConstIterator it;
    for (it = data->relations.begin(); it != data->relations.end(); ++it ) {
        QArray<int> id = it.data();
        for ( uint i = 0; i < id.size(); ++i ) {
            str += it.key() + "," + QString::number( i ) + ";";
        }
    }
    str = str.remove( str.length()-1, 1); // strip the ;
    qWarning("IDS " + str );

    return str;
}
int ToDoEvent::uid()const {
    return data->uid;
}
void ToDoEvent::setUid( int id ) {
    m_gen.store(id );
    changeOrModify();
    data->uid = id;
}
QMap<QString, QString> ToDoEvent::extras()const {
    return data->extra;
}
/**
 *  change or modify looks at the ref count and either
 *  creates a new QShared Object or it can modify it
 * right in place
 */
void ToDoEvent::changeOrModify() {
    if ( data->count != 1 ) {
        qWarning("changeOrModify");
        data->deref();
        ToDoEventData* d2 = new ToDoEventData();
        copy(data, d2 );
        data = d2;
    }/*for testing */ else
        qWarning("not changed");
}
void ToDoEvent::copy( ToDoEventData* src, ToDoEventData* dest ) {
    dest->date = src->date;
    dest->isCompleted = src->isCompleted;
    dest->hasDate = src->hasDate;
    dest->priority = src->priority;
    dest->category = src->category;
    dest->desc = src->desc;
    dest->sum = src->sum;
    dest->extra = src->extra;
    dest->relations  = src->relations;
    dest->uid = src->uid;
    dest->prog = src->prog;
    dest->hasAlarmDateTime = src->hasAlarmDateTime;
    dest->alarmDateTime = src->alarmDateTime;
}


