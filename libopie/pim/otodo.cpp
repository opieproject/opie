
#include <qobject.h>
#include <qshared.h>



#include <qpe/palmtopuidgen.h>
#include <qpe/stringutil.h>
#include <qpe/palmtoprecord.h>
#include <qpe/stringutil.h>
#include <qpe/categories.h>
#include <qpe/categoryselect.h>



#include "otodo.h"


struct OTodo::OTodoData : public QShared {
    OTodoData() : QShared() {
    };

    QDate date;
    bool isCompleted:1;
    bool hasDate:1;
    int priority;
    QString desc;
    QString sum;
    QMap<QString, QString> extra;
    ushort prog;
    bool hasAlarmDateTime :1;
    QDateTime alarmDateTime;
};

OTodo::OTodo(const OTodo &event )
    : OPimRecord( event ),  data( event.data )
{
    data->ref();
//    qWarning("ref up");
}
OTodo::~OTodo() {

//    qWarning("~OTodo "  );
    if ( data->deref() ) {
//        qWarning("OTodo::dereffing");
        delete data;
        data = 0l;
    }
}
OTodo::OTodo(bool completed, int priority,
                     const QArray<int> &category,
                     const QString& summary,
                     const QString &description,
                     ushort progress,
                     bool hasDate, QDate date, int uid )
    : OPimRecord(  uid )
{
//    qWarning("OTodoData " + summary);
    setCategories( category );

    data = new OTodoData;

    data->date = date;
    data->isCompleted = completed;
    data->hasDate = hasDate;
    data->priority = priority;
    data->sum = summary;
    data->prog = progress;
    data->desc = Qtopia::simplifyMultiLineSpace(description );
    data->hasAlarmDateTime = false;

}
OTodo::OTodo(bool completed, int priority,
                     const QStringList &category,
                     const QString& summary,
                     const QString &description,
                     ushort progress,
                     bool hasDate, QDate date, int uid )
    : OPimRecord(  uid )
{
//    qWarning("OTodoData" + summary);
    setCategories( idsFromString( category.join(";") ) );

    data = new OTodoData;

    data->date = date;
    data->isCompleted = completed;
    data->hasDate = hasDate;
    data->priority = priority;
    data->sum = summary;
    data->prog = progress;
    data->desc = Qtopia::simplifyMultiLineSpace(description );
    data->hasAlarmDateTime = false;

}
bool OTodo::match( const QRegExp &regExp )const
{
  if( QString::number( data->priority ).find( regExp ) != -1 ){
    return true;
  }else if( data->hasDate && data->date.toString().find( regExp) != -1 ){
    return true;
  }else if(data->desc.find( regExp ) != -1 ){
    return true;
  }else if(data->sum.find( regExp ) != -1 ) {
    return true;
  }
  return false;
}
bool OTodo::isCompleted() const
{
    return data->isCompleted;
}
bool OTodo::hasDueDate() const
{
    return data->hasDate;
}
bool OTodo::hasAlarmDateTime() const
{
    return data->hasAlarmDateTime;
}
int OTodo::priority()const
{
    return data->priority;
}
QString OTodo::summary() const
{
    return data->sum;
}
ushort OTodo::progress() const
{
    return data->prog;
}
QDate OTodo::dueDate()const
{
    return data->date;
}

QDateTime OTodo::alarmDateTime() const
{
    return data->alarmDateTime;
}

QString OTodo::description()const
{
    return data->desc;
}
void OTodo::setCompleted( bool completed )
{
    changeOrModify();
    data->isCompleted = completed;
}
void OTodo::setHasDueDate( bool hasDate )
{
    changeOrModify();
    data->hasDate = hasDate;
}
void OTodo::setHasAlarmDateTime( bool hasAlarmDateTime )
{
    changeOrModify();
    data->hasAlarmDateTime = hasAlarmDateTime;
}
void OTodo::setDescription(const QString &desc )
{
//    qWarning( "desc " + desc );
    changeOrModify();
    data->desc = Qtopia::simplifyMultiLineSpace(desc );
}
void OTodo::setSummary( const QString& sum )
{
    changeOrModify();
    data->sum = sum;
}
void OTodo::setPriority(int prio )
{
    changeOrModify();
    data->priority = prio;
}
void OTodo::setDueDate( QDate date )
{
    changeOrModify();
    data->date = date;
}
void OTodo::setAlarmDateTime( const QDateTime& alarm )
{
    changeOrModify();
    data->alarmDateTime = alarm;
}
bool OTodo::isOverdue( )
{
    if( data->hasDate )
	return QDate::currentDate() > data->date;
    return false;
}
void OTodo::setProgress(ushort progress )
{
    changeOrModify();
    data->prog = progress;
}
QString OTodo::toShortText() const {
 return summary();
}
/*!
  Returns a richt text string
*/
QString OTodo::toRichText() const
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

  text += "<b>" + QObject::tr( "Category:") + "</b> ";
  text += categoryNames().join(", ");
  text += "<br>";

  return text;
}

bool OTodo::operator<( const OTodo &toDoEvent )const{
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
bool OTodo::operator<=(const OTodo &toDoEvent )const
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
bool OTodo::operator>(const OTodo &toDoEvent )const
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
bool OTodo::operator>=(const OTodo &toDoEvent )const
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
bool OTodo::operator==(const OTodo &toDoEvent )const
{
    if ( data->priority != toDoEvent.data->priority ) return false;
    if ( data->priority != toDoEvent.data->prog ) return false;
    if ( data->isCompleted != toDoEvent.data->isCompleted ) return false;
    if ( data->hasDate != toDoEvent.data->hasDate ) return false;
    if ( data->date != toDoEvent.data->date ) return false;
    if ( data->sum != toDoEvent.data->sum ) return false;
    if ( data->desc != toDoEvent.data->desc ) return false;
    if ( data->hasAlarmDateTime != toDoEvent.data->hasAlarmDateTime )
        return false;
    if ( data->alarmDateTime != toDoEvent.data->alarmDateTime )
	return false;

    return OPimRecord::operator==( toDoEvent );
}
void OTodo::deref() {

//    qWarning("deref in ToDoEvent");
    if ( data->deref() ) {
//        qWarning("deleting");
        delete data;
        data= 0;
    }
}
OTodo &OTodo::operator=(const OTodo &item )
{
    OPimRecord::operator=( item );
    //qWarning("operator= ref ");
    item.data->ref();
    deref();
    data = item.data;

    return *this;
}

QMap<int, QString> OTodo::toMap() const {
    QMap<int, QString> map;

    map.insert( Uid, QString::number( uid() ) );
    map.insert( Category,  idsToString( categories() ) );
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

QMap<QString, QString> OTodo::toExtraMap()const {
    return data->extra;
}
/**
 *  change or modify looks at the ref count and either
 *  creates a new QShared Object or it can modify it
 * right in place
 */
void OTodo::changeOrModify() {
    if ( data->count != 1 ) {
//        qWarning("changeOrModify");
        data->deref();
        OTodoData* d2 = new OTodoData();
        copy(data, d2 );
        data = d2;
    }
}
void OTodo::copy( OTodoData* src, OTodoData* dest ) {
    dest->date = src->date;
    dest->isCompleted = src->isCompleted;
    dest->hasDate = src->hasDate;
    dest->priority = src->priority;
    dest->desc = src->desc;
    dest->sum = src->sum;
    dest->extra = src->extra;
    dest->prog = src->prog;
    dest->hasAlarmDateTime = src->hasAlarmDateTime;
    dest->alarmDateTime = src->alarmDateTime;
}
QString OTodo::type() const {
    return QString::fromLatin1("OTodo");
}
QString OTodo::recordField(int id )const {
    return QString::null;
}

