
#include <qobject.h>
#include <qshared.h>



#include <qpe/palmtopuidgen.h>
#include <qpe/stringutil.h>
#include <qpe/palmtoprecord.h>
#include <qpe/stringutil.h>
#include <qpe/categories.h>
#include <qpe/categoryselect.h>


#include "opimstate.h"
#include "orecur.h"
#include "opimmaintainer.h"
#include "opimnotifymanager.h"
#include "opimresolver.h"

#include "otodo.h"


struct OTodo::OTodoData : public QShared {
    OTodoData() : QShared() {
        recur = 0;
        state = 0;
        maintainer = 0;
        notifiers = 0;
    };
    ~OTodoData() {
        delete recur;
        delete maintainer;
        delete notifiers;
    }

    QDate date;
    bool isCompleted:1;
    bool hasDate:1;
    int priority;
    QString desc;
    QString sum;
    QMap<QString, QString> extra;
    ushort prog;
    OPimState *state;
    ORecur *recur;
    OPimMaintainer *maintainer;
    QDate start;
    QDate completed;
    OPimNotifyManager *notifiers;
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
}
bool OTodo::match( const QRegExp &regExp )const
{
  if( QString::number( data->priority ).find( regExp ) != -1 ){
      setLastHitField( Priority );
      return true;
  }else if( data->hasDate && data->date.toString().find( regExp) != -1 ){
      setLastHitField( HasDate );
      return true;
  }else if(data->desc.find( regExp ) != -1 ){
        setLastHitField( Description );
        return true;
  }else if(data->sum.find( regExp ) != -1 ) {
        setLastHitField( Summary );
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
bool OTodo::hasStartDate()const {
    return data->start.isValid();
}
bool OTodo::hasCompletedDate()const {
    return data->completed.isValid();
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
QDate OTodo::startDate()const {
    return data->start;
}
QDate OTodo::completedDate()const {
    return data->completed;
}
QString OTodo::description()const
{
    return data->desc;
}
bool OTodo::hasState() const{
    if (!data->state ) return false;
    return ( data->state->state() != OPimState::Undefined );
}
OPimState OTodo::state()const {
    if (!data->state ) {
        OPimState state;
        return state;
    }

    return (*data->state);
}
bool OTodo::hasRecurrence()const {
    if (!data->recur) return false;
    return data->recur->doesRecur();
}
ORecur OTodo::recurrence()const {
    if (!data->recur) return ORecur();

    return (*data->recur);
}
bool OTodo::hasMaintainer()const {
    if (!data->maintainer) return false;

    return (data->maintainer->mode() != OPimMaintainer::Undefined );
}
OPimMaintainer OTodo::maintainer()const {
    if (!data->maintainer) return OPimMaintainer();

    return (*data->maintainer);
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
void OTodo::setDueDate( const QDate& date )
{
    changeOrModify();
    data->date = date;
}
void OTodo::setStartDate( const QDate& date ) {
    changeOrModify();
    data->start = date;
}
void OTodo::setCompletedDate( const QDate& date ) {
    changeOrModify();
    data->completed = date;
}
void OTodo::setState( const OPimState& state ) {
    changeOrModify();
    if (data->state )
        (*data->state) = state;
    else
        data->state = new OPimState( state );
}
void OTodo::setRecurrence( const ORecur& rec) {
    changeOrModify();
    if (data->recur )
        (*data->recur) = rec;
    else
        data->recur = new ORecur( rec );
}
void OTodo::setMaintainer( const OPimMaintainer& pim ) {
    changeOrModify();

    if (data->maintainer )
        (*data->maintainer) = pim;
    else
        data->maintainer = new OPimMaintainer( pim );
}
bool OTodo::isOverdue( )
{
    if( data->hasDate && !data->isCompleted)
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

  // summary
  text += "<b><h3><img src=\"todo/TodoList\"> ";
  if ( !summary().isEmpty() ) {
      text += Qtopia::escapeString(summary() ).replace(QRegExp( "[\n]"),  "" );
  }
  text += "</h3></b><br><hr><br>";

  // description
  if( !description().isEmpty() ){
    text += "<b>" + QObject::tr( "Description:" ) + "</b><br>";
    text += Qtopia::escapeString(description() ).replace(QRegExp( "[\n]"), "<br>" ) + "<br>";
  }

  // priority
  int priorityval = priority();
  text += "<b>" + QObject::tr( "Priority:") +" </b><img src=\"todo/priority" +
          QString::number( priorityval ) + "\"> ";
//  text += "<b>" + QObject::tr( "Priority:") +"</b><img src=\"todo/priority" +
//          QString::number( priority() ) + "\"><br>";
  switch ( priorityval )
  {
    case 1 : text += QObject::tr( "Very high" );
        break;
    case 2 : text += QObject::tr( "High" );
        break;
    case 3 : text += QObject::tr( "Normal" );
        break;
    case 4 : text += QObject::tr( "Low" );
        break;
    case 5 : text += QObject::tr( "Very low" );
        break;
  };
  text += "<br>";

  // progress
  text += "<b>" + QObject::tr( "Progress:") + " </b>"
          + QString::number( progress() ) + " %<br>";

  // due date
  if (hasDueDate() ){
    QDate dd = dueDate();
    int off = QDate::currentDate().daysTo( dd );

    text += "<b>" + QObject::tr( "Deadline:" ) + " </b><font color=\"";
    if ( off < 0 )
      text += "#FF0000";
    else if ( off == 0 )
      text += "#FFFF00";
    else if ( off > 0 )
      text += "#00FF00";

    text += "\">" + dd.toString() + "</font><br>";
  }

  // categories
  text += "<b>" + QObject::tr( "Category:") + "</b> ";
  text += categoryNames( "Todo List" ).join(", ");
  text += "<br>";

  return text;
}
bool OTodo::hasNotifiers()const {
    if (!data->notifiers) return false;
    return !data->notifiers->isEmpty();
}
OPimNotifyManager& OTodo::notifiers() {
    if (!data->notifiers )
        data->notifiers = new OPimNotifyManager;
    return (*data->notifiers);
}
const OPimNotifyManager& OTodo::notifiers()const{
    if (!data->notifiers )
        data->notifiers = new OPimNotifyManager;

    return (*data->notifiers);
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
    if ( data->maintainer    != toDoEvent.data->maintainer )
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
    if ( this == &item ) return *this;

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
//    map.insert( CrossReference, crossToString() );
    /* FIXME!!!   map.insert( State,  );
    map.insert( Recurrence, );
    map.insert( Reminders, );
    map.
    */
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
        qWarning("changeOrModify");
        data->deref();
        OTodoData* d2 = new OTodoData();
        copy(data, d2 );
        data = d2;
    }
}
// WATCHOUT
/*
 * if you add something to the Data struct
 * be sure to copy it here
 */
void OTodo::copy( OTodoData* src, OTodoData* dest ) {
    dest->date = src->date;
    dest->isCompleted = src->isCompleted;
    dest->hasDate = src->hasDate;
    dest->priority = src->priority;
    dest->desc = src->desc;
    dest->sum = src->sum;
    dest->extra = src->extra;
    dest->prog = src->prog;

    if (src->state )
        dest->state = new OPimState( *src->state );

    if (src->recur )
        dest->recur = new ORecur( *src->recur );

    if (src->maintainer )
        dest->maintainer = new OPimMaintainer( *src->maintainer )
                           ;
    dest->start = src->start;
    dest->completed = src->completed;

    if (src->notifiers )
        dest->notifiers = new OPimNotifyManager( *src->notifiers );
}
QString OTodo::type() const {
    return QString::fromLatin1("OTodo");
}
QString OTodo::recordField(int /*id*/ )const {
    return QString::null;
}

int OTodo::rtti(){
    return OPimResolver::TodoList;
}
