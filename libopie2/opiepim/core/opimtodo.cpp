/*
                             This file is part of the Opie Project
                             Copyright (C) The Main Author <main-author@whereever.org>
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

#include "opimtodo.h"

/* OPIE */
#include <opie2/opimstate.h>
#include <opie2/opimrecurrence.h>
#include <opie2/opimmaintainer.h>
#include <opie2/opimnotifymanager.h>
#include <opie2/opimresolver.h>
#include <opie2/opimdateconversion.h>
#include <opie2/odebug.h>

#include <qpe/palmtopuidgen.h>
#include <qpe/palmtoprecord.h>
#include <qpe/categories.h>
#include <qpe/categoryselect.h>
#include <qpe/stringutil.h>

/* QT */
#include <qobject.h>
#include <qshared.h>

namespace Opie
{

struct OPimTodo::OPimTodoData : public QShared
{
    OPimTodoData() : QShared()
    {
        recur = 0;
        state = 0;
        maintainer = 0;
        notifiers = 0;
    };
    ~OPimTodoData()
    {
        delete recur;
        delete maintainer;
        delete notifiers;
        delete state;
    }

    QDate date;
    bool isCompleted: 1;
    bool hasDate: 1;
    int priority;
    QString desc;
    QString sum;
    QMap<QString, QString> extra;
    ushort prog;
    OPimState *state;
    OPimRecurrence *recur;
    OPimMaintainer *maintainer;
    QDate start;
    QDate completed;
    OPimNotifyManager *notifiers;
    ChangeAction action;
};


OPimTodo::OPimTodo( const OPimTodo &event )
        : OPimRecord( event ), data( event.data )
{
    data->ref();
}


OPimTodo::~OPimTodo()
{

    if ( data->deref() )
    {
        delete data;
        data = 0l;
    }
}


OPimTodo::OPimTodo( bool completed, int priority,
              const QArray<int> &category,
              const QString& summary,
              const QString &description,
              ushort progress,
              bool hasDate, QDate date, int uid )
        : OPimRecord( uid )
{
    setCategories( category );

    data = new OPimTodoData;

    data->date = date;
    data->isCompleted = completed;
    data->hasDate = hasDate;
    if ( priority < 1 )
        priority = 1;
    else if ( priority > 5 )
        priority = 5;
    data->priority = priority;
    data->sum = summary;
    data->prog = progress;
    data->desc = Qtopia::simplifyMultiLineSpace( description );
    data->action = ACTION_ADD;
}


OPimTodo::OPimTodo( bool completed, int priority,
              const QStringList &category,
              const QString& summary,
              const QString &description,
              ushort progress,
              bool hasDate, QDate date, int uid )
        : OPimRecord( uid )
{
    setCategories( idsFromString( category.join( ";" ) ) );

    data = new OPimTodoData;

    data->date = date;
    data->isCompleted = completed;
    data->hasDate = hasDate;
    if ( priority < 1 )
        priority = 1;
    else if ( priority > 5 )
        priority = 5;
    data->priority = priority;
    data->sum = summary;
    data->prog = progress;
    data->desc = Qtopia::simplifyMultiLineSpace( description );
    data->action = ACTION_ADD;
}


bool OPimTodo::match( const QRegExp &regExp ) const
{
    if ( QString::number( data->priority ).find( regExp ) != -1 )
    {
        setLastHitField( Priority );
        return true;
    }
    else if ( data->hasDate && data->date.toString().find( regExp ) != -1 )
    {
        setLastHitField( HasDate );
        return true;
    }
    else if ( data->desc.find( regExp ) != -1 )
    {
        setLastHitField( Description );
        return true;
    }
    else if ( data->sum.find( regExp ) != -1 )
    {
        setLastHitField( Summary );
        return true;
    }
    return false;
}


bool OPimTodo::isCompleted() const
{
    return data->isCompleted;
}


bool OPimTodo::hasDueDate() const
{
    return data->hasDate;
}

/**
 * \brief Does this Todo have a start date
 *
 * Does this Todo have a start date. The decision
 * is based on if the internal startDate isValid
 * in the sense of QDate::isValid.
 *
 * @return True if the startDate isValid
 * @see startDate
 * @see setStartDate
 * @see QDate::isValid()
 */
bool OPimTodo::hasStartDate() const
{
    return data->start.isValid();
}

/**
 * \brief Does this Todo have a Date when it was completed
 *
 * As in \sa hasStartDate() it is determined if there
 * is a completed date by looking if the internal date
 * isValid \sa QDate::isValid.
 *
 * @see hasStartDate
 * @return True if the completedDate is set and valid.
 */
bool OPimTodo::hasCompletedDate() const
{
    return data->completed.isValid();
}


int OPimTodo::priority() const
{
    return data->priority;
}


QString OPimTodo::summary() const
{
    return data->sum;
}


ushort OPimTodo::progress() const
{
    return data->prog;
}


QDate OPimTodo::dueDate() const
{
    return data->date;
}


QDate OPimTodo::startDate() const
{
    return data->start;
}


QDate OPimTodo::completedDate() const
{
    return data->completed;
}


QString OPimTodo::description() const
{
    return data->desc;
}


bool OPimTodo::hasState() const
{
    if ( !data->state ) return false;
    return ( data->state->state() != OPimState::Undefined );
}


OPimState OPimTodo::state() const
{
    if ( !data->state )
    {
        OPimState state;
        return state;
    }

    return ( *data->state );
}


bool OPimTodo::hasRecurrence() const
{
    if ( !data->recur ) return false;
    return data->recur->doesRecur();
}


OPimRecurrence OPimTodo::recurrence() const
{
    if ( !data->recur ) return OPimRecurrence();

    return ( *data->recur );
}


bool OPimTodo::hasMaintainer() const
{
    if ( !data->maintainer ) return false;

    return ( data->maintainer->mode() != OPimMaintainer::Undefined );
}


OPimMaintainer OPimTodo::maintainer() const
{
    if ( !data->maintainer ) return OPimMaintainer();

    return ( *data->maintainer );
}


void OPimTodo::setCompleted( bool completed )
{
    changeOrModify();
    data->isCompleted = completed;
}


void OPimTodo::setHasDueDate( bool hasDate )
{
    changeOrModify();
    data->hasDate = hasDate;
}


void OPimTodo::setDescription( const QString &desc )
{
    changeOrModify();
    data->desc = Qtopia::simplifyMultiLineSpace( desc );
}


void OPimTodo::setSummary( const QString& sum )
{
    changeOrModify();
    data->sum = sum;
}


void OPimTodo::setPriority( int prio )
{
    changeOrModify();
    if ( prio < 1 )
        prio = 1;
    else if ( prio > 5 )
        prio = 5;
    data->priority = prio;
}


void OPimTodo::setDueDate( const QDate& date )
{
    changeOrModify();
    data->date = date;
}


void OPimTodo::setStartDate( const QDate& date )
{
    changeOrModify();
    data->start = date;
}


void OPimTodo::setCompletedDate( const QDate& date )
{
    changeOrModify();
    data->completed = date;
}


void OPimTodo::setState( const OPimState& state )
{
    changeOrModify();
    if ( data->state )
        ( *data->state ) = state;
    else
        data->state = new OPimState( state );
}


void OPimTodo::setRecurrence( const OPimRecurrence& rec )
{
    changeOrModify();
    if ( data->recur )
        ( *data->recur ) = rec;
    else
        data->recur = new OPimRecurrence( rec );
}


void OPimTodo::setMaintainer( const OPimMaintainer& pim )
{
    changeOrModify();

    if ( data->maintainer )
        ( *data->maintainer ) = pim;
    else
        data->maintainer = new OPimMaintainer( pim );
}


bool OPimTodo::isOverdue( )const
{
    if ( data->hasDate && !data->isCompleted )
        return QDate::currentDate() > data->date;
    return false;
}


void OPimTodo::setProgress( ushort progress )
{
    changeOrModify();
    data->prog = progress;
}


QString OPimTodo::toShortText() const
{
    return summary();
}


/*!
  Returns a richt text string
*/
QString OPimTodo::toRichText() const
{
    QString text;
    QStringList catlist;

    // summary
    text += "<b><h3><img src=\"todo/TodoList\"> ";
    if ( !summary().isEmpty() )
    {
        text += Qtopia::escapeString( summary() ).replace( QRegExp( "[\n]" ), "" );
    }
    text += "</h3></b><br><hr><br>";

    // description
    if ( !description().isEmpty() )
    {
        text += "<b>" + QObject::tr( "Description:" ) + "</b><br>";
        text += Qtopia::escapeString( description() ).replace( QRegExp( "[\n]" ), "<br>" ) + "<br>";
    }

    // priority
    int priorityval = priority();
    text += "<b>" + QObject::tr( "Priority:" ) + " </b><img src=\"todo/priority" +
            QString::number( priorityval ) + "\"> ";

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
    text += "<b>" + QObject::tr( "Progress:" ) + " </b>"
            + QString::number( progress() ) + " %<br>";

    // due date
    if ( hasDueDate() )
    {
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
    text += "<b>" + QObject::tr( "Category:" ) + "</b> ";
    text += categoryNames( "Todo List" ).join( ", " );
    text += "<br>";

    return text;
}


bool OPimTodo::hasNotifiers() const
{
    if ( !data->notifiers ) return false;
    return !data->notifiers->isEmpty();
}


OPimNotifyManager& OPimTodo::notifiers()
{
    if ( !data->notifiers )
        data->notifiers = new OPimNotifyManager;
    return ( *data->notifiers );
}


const OPimNotifyManager& OPimTodo::notifiers() const
{
    if ( !data->notifiers )
        data->notifiers = new OPimNotifyManager;

    return ( *data->notifiers );
}


bool OPimTodo::operator<( const OPimTodo &toDoEvent ) const
{
    if ( !hasDueDate() && !toDoEvent.hasDueDate() ) return true;
    if ( !hasDueDate() && toDoEvent.hasDueDate() ) return false;
    if ( hasDueDate() && toDoEvent.hasDueDate() )
    {
        if ( dueDate() == toDoEvent.dueDate() )
        { // let's the priority decide
            return priority() < toDoEvent.priority();
        }
        else
        {
            return dueDate() < toDoEvent.dueDate();
        }
    }
    return false;
}


bool OPimTodo::operator<=( const OPimTodo &toDoEvent ) const
{
    if ( !hasDueDate() && !toDoEvent.hasDueDate() ) return true;
    if ( !hasDueDate() && toDoEvent.hasDueDate() ) return true;
    if ( hasDueDate() && toDoEvent.hasDueDate() )
    {
        if ( dueDate() == toDoEvent.dueDate() )
        { // let's the priority decide
            return priority() <= toDoEvent.priority();
        }
        else
        {
            return dueDate() <= toDoEvent.dueDate();
        }
    }
    return true;
}


bool OPimTodo::operator>( const OPimTodo &toDoEvent ) const
{
    if ( !hasDueDate() && !toDoEvent.hasDueDate() ) return false;
    if ( !hasDueDate() && toDoEvent.hasDueDate() ) return false;
    if ( hasDueDate() && toDoEvent.hasDueDate() )
    {
        if ( dueDate() == toDoEvent.dueDate() )
        { // let's the priority decide
            return priority() > toDoEvent.priority();
        }
        else
        {
            return dueDate() > toDoEvent.dueDate();
        }
    }
    return false;
}


bool OPimTodo::operator>=( const OPimTodo &toDoEvent ) const
{
    if ( !hasDueDate() && !toDoEvent.hasDueDate() ) return true;
    if ( !hasDueDate() && toDoEvent.hasDueDate() ) return false;
    if ( hasDueDate() && toDoEvent.hasDueDate() )
    {
        if ( dueDate() == toDoEvent.dueDate() )
        { // let's the priority decide
            return priority() > toDoEvent.priority();
        }
        else
        {
            return dueDate() > toDoEvent.dueDate();
        }
    }
    return true;
}


bool OPimTodo::operator==( const OPimTodo &toDoEvent ) const
{
    if ( data->priority != toDoEvent.data->priority ) return false;
    if ( data->priority != toDoEvent.data->prog ) return false;
    if ( data->isCompleted != toDoEvent.data->isCompleted ) return false;
    if ( data->hasDate != toDoEvent.data->hasDate ) return false;
    if ( data->date != toDoEvent.data->date ) return false;
    if ( data->sum != toDoEvent.data->sum ) return false;
    if ( data->desc != toDoEvent.data->desc ) return false;
    if ( data->maintainer != toDoEvent.data->maintainer )
        return false;

    return OPimRecord::operator==( toDoEvent );
}


void OPimTodo::deref()
{

    if ( data->deref() )
    {
        delete data;
        data = 0;
    }
}


OPimTodo &OPimTodo::operator=( const OPimTodo &item )
{
    if ( this == &item ) return * this;

    OPimRecord::operator=( item );
    item.data->ref();
    deref();
    data = item.data;

    return *this;
}


QMap<int, QString> OPimTodo::toMap() const
{
    QMap<int, QString> map;

    map.insert( Uid, QString::number( uid() ) );
    map.insert( Category, idsToString( categories() ) );
    map.insert( HasDate, QString::number( data->hasDate ) );
    map.insert( Completed, QString::number( data->isCompleted ) );
    map.insert( Description, data->desc );
    map.insert( Summary, data->sum );
    map.insert( Priority, QString::number( data->priority ) );
    if( hasDueDate() ) {
        map.insert( DateDay, QString::number( data->date.day() ) );
        map.insert( DateMonth, QString::number( data->date.month() ) );
        map.insert( DateYear, QString::number( data->date.year() ) );
    }
    map.insert( Progress, QString::number( data->prog ) );
    if( hasStartDate() )
        map.insert( StartDate, OPimDateConversion::dateToString( data->start ) );
    if( hasCompletedDate() )
        map.insert( CompletedDate, OPimDateConversion::dateToString( data->completed ) );
    if( hasState() )
        map.insert( State, QString::number( state().state() ) );

    // Add recurrence stuff
    if ( hasRecurrence() )
    {
        OPimRecurrence recur = recurrence();
        QMap<int, QString> recFields = recur.toMap();
        map.insert( FRType, recFields[ OPimRecurrence::RType ] );
        map.insert( FRWeekdays, recFields[ OPimRecurrence::RWeekdays ] );
        map.insert( FRPosition, recFields[ OPimRecurrence::RPosition ] );
        map.insert( FRFreq, recFields[ OPimRecurrence::RFreq ] );
        map.insert( FRHasEndDate, recFields[ OPimRecurrence::RHasEndDate ] );
        map.insert( FREndDate, recFields[ OPimRecurrence::EndDate ] );
    }

    // save reminders and notifiers!
    if ( hasNotifiers() ) {
        // Alarm format:
        // DATE_TIME:DURATION:SOUND:NOT_USED_YET;OTHER_DATE_TIME:OTHER_DURATION:SOUND:....
        OPimNotifyManager manager = notifiers();
        OPimNotifyManager::Alarms alarms = manager.alarms();
        if (!alarms.isEmpty() ) {
            QStringList als;
            OPimNotifyManager::Alarms::Iterator it = alarms.begin();
            for ( ; it != alarms.end(); ++it ) {
                /* only if time is valid */
                if ( (*it).dateTime().isValid() ) {
                    als << OPimDateConversion::dateTimeToString( (*it).dateTime() )
                        + ":" + QString::number( (*it).duration() )
                        + ":" + QString::number( (*it).sound() )
                        + ":";
                }
            }
            map.insert( Alarms, als.join(";") );
        }

        // now the same for reminders but easier. We just save the uid of the OPimEvent.
        OPimNotifyManager::Reminders reminders = manager.reminders();
        if (!reminders.isEmpty() ) {
            OPimNotifyManager::Reminders::Iterator it = reminders.begin();
            QStringList records;
            for ( ; it != reminders.end(); ++it ) {
                records << QString::number( (*it).recordUid() );
            }
            map.insert( Reminders, records.join(";") );
        }
    }

    ChangeAction action = this->action();
    if( action != ACTION_ADD )
        map.insert( FIELDID_ACTION, actionToStr( action ) );

    // FIXME CrossReference, Maintainer

    return map;
}

void OPimTodo::fromMap( const QMap<int, QString>& map )
{
    // We just want to set the UID if it is really stored.
    if ( !map[ Uid ].isEmpty() )
        setUid( map[ Uid ].toInt() );

    setCategories( idsFromString( map[ Category ] ) );

    setHasDueDate( map[ HasDate ].toInt() );
    setCompleted( map[ Completed ].toInt() );
    setDescription( map[ Description ] );
    setSummary( map[ Summary ] );

    setPriority( map[ Priority ].toInt() );

    int dateday = map[ DateDay ].toInt();
    int datemonth = map[ DateMonth ].toInt();
    int dateyear = map[ DateYear ].toInt();
    setDueDate( QDate( dateyear, datemonth, dateday ) );

    setProgress( map[ Progress ].toInt() );

    setCompletedDate( OPimDateConversion::dateFromString( map[ CompletedDate ] ) );
    setStartDate( OPimDateConversion::dateFromString( map[ StartDate ] ) );

    if( !map[ State ].isEmpty() )
        setState( map[ State ].toInt() );

    QString alarmstr = map[ Alarms ];
    if( !alarmstr.isEmpty() ) {
        OPimNotifyManager &manager = notifiers();
        QStringList als = QStringList::split(";", alarmstr );
        for (QStringList::Iterator it = als.begin(); it != als.end(); ++it ) {
            QStringList alarm = QStringList::split(":", (*it), TRUE ); // allow empty
            OPimAlarm al( alarm[2].toInt(), OPimDateConversion::dateTimeFromString( alarm[0] ), alarm[1].toInt() );
            manager.add( al );
        }
    }

    QString reminderstr = map[ Reminders ];
    if( !reminderstr.isEmpty() ) {
        OPimNotifyManager &manager = notifiers();
        QStringList rems = QStringList::split(";", reminderstr );
        for (QStringList::Iterator it = rems.begin(); it != rems.end(); ++it ) {
            OPimReminder rem( (*it).toInt() );
            manager.add( rem );
        }
    }

    // Fill recurrence stuff and put it directly into the OPimRecurrence-Object using fromMap..
    if ( !map[ OPimTodo::FRType ].isEmpty() ) {
        QMap<int, QString> recFields;
        recFields.insert( OPimRecurrence::RType, map[ OPimTodo::FRType ] );
        recFields.insert( OPimRecurrence::RWeekdays, map[ OPimTodo::FRWeekdays ] );
        recFields.insert( OPimRecurrence::RPosition, map[ OPimTodo::FRPosition ] );
        recFields.insert( OPimRecurrence::RFreq, map[ OPimTodo::FRFreq ] );
        recFields.insert( OPimRecurrence::RHasEndDate, map[ OPimTodo::FRHasEndDate ] );
        recFields.insert( OPimRecurrence::EndDate, map[ OPimTodo::FREndDate ] );
        OPimRecurrence recur( recFields );
        setRecurrence( recur );
    }

    if ( !map[ OPimTodo::CrossReference ].isEmpty() ) {
        // FIXME xref is not fully implemented
        /*
         * A cross refernce looks like
         * appname,id;appname,id
         * we need to split it up
         */
        QStringList refs = QStringList::split(';', map[ OPimTodo::CrossReference ] );
        QStringList::Iterator strIt;
        for (strIt = refs.begin(); strIt != refs.end(); ++strIt ) {
            int pos = (*strIt).find(',');
            if ( pos > -1 ) {
                // ev.addRelation( (*strIt).left(pos),  (*strIt).mid(pos+1).toInt() );
            }
        }
    }

    setAction( strToAction( map[ FIELDID_ACTION ] ) );

    // FIXME Maintainer
}


/**
 *  change or modify looks at the ref count and either
 *  creates a new QShared Object or it can modify it
 * right in place
 */
void OPimTodo::changeOrModify()
{
    if ( data->count != 1 )
    {
        data->deref();
        OPimTodoData* d2 = new OPimTodoData();
        copy( data, d2 );
        data = d2;
    }
}


// WATCHOUT
/*
 * if you add something to the Data struct
 * be sure to copy it here
 */
void OPimTodo::copy( OPimTodoData* src, OPimTodoData* dest )
{
    dest->date = src->date;
    dest->isCompleted = src->isCompleted;
    dest->hasDate = src->hasDate;
    dest->priority = src->priority;
    dest->desc = src->desc;
    dest->sum = src->sum;
    dest->extra = src->extra;
    dest->prog = src->prog;

    if ( src->state )
        dest->state = new OPimState( *src->state );

    if ( src->recur )
        dest->recur = new OPimRecurrence( *src->recur );

    if ( src->maintainer )
        dest->maintainer = new OPimMaintainer( *src->maintainer )
                           ;
    dest->start = src->start;
    dest->completed = src->completed;

    if ( src->notifiers )
        dest->notifiers = new OPimNotifyManager( *src->notifiers );

    dest->action = src->action;
}


QString OPimTodo::type() const
{
    return QString::fromLatin1( "OPimTodo" );
}


QString OPimTodo::recordField( int  id) const
{
    QString res;
    Q_UNUSED( id )
#if 0
    switch( id ) {
    case HasDate:
        res = (hasDueDate() ?
              QObject::tr( "Has a due-date" )
              : QObject::tr( "No due-date" ));
        break;
    case Completed:
        res = ( isCompleted() ?
                QObject::tr( "Completed" ) :
                QObject::tr( "Not completed" ));
        break;
    case Description:
        res = description();
        break;
    case Summary:
        res = summary();
        break;
    case Priority:
        res = QString::number( priority() );
        break;
    case DateDay:
        res = QString::number( dueDate().day() );
        break;
    case DateMonth:
        res = QString::number( dueDate().month() );
        break;
    case DateYear:
        res = QString::number( dueDate().year() );
        break;
    case Progress:
        res = QString::number( progress() );
        break;
    case State:
        res = QString::number( state().state() );
        break;
    case Recurrence:
        res = ( hasRecurrence() ?
                QString::null /*recurrence().summary()*/ :
                QObject::tr("No reccurrence"));
        break;
    case Alarms:
        break;
    case Reminders:
        break;
    case Maintainer:
        break;
    case StartDate:
        res = ( hasStartDate() ?
                /*TimeString::()*/ QString::null :
                QObject::tr( "No start-date" ) );
        break;
    case CompletedDate:
        res = ( hasCompletedDate() ?
                /*TimeString::()*/ QString::null :
                QObject::tr( "No completed-date" ) );
        break;
    case DueDate:
        res = ( hasDueDate() ?
                /*TimeString::()*/ QString::null :
                QObject::tr( "No due-date" );
                break;
    default:
        res = OPimRecord::recordField( id );
    }

#endif
    return res;
}

OPimRecord::ChangeAction OPimTodo::action() const
{
    return data->action;
}

void OPimTodo::setAction( ChangeAction action )
{
    changeOrModify();
    data->action = action;
}

int OPimTodo::rtti() const
{
    return OPimResolver::TodoList;
}

/**
 * \brief Provide a SafeCast to OPimTodo from a OPimRecord
 *
 * Provide a safe cast that will return 0 if the record
 * type is not OPimTodo. In the other case it will
 * be casted to OPimTodo and returned
 *
 * @param rec The OPimRecord to be casted
 *
 * @return a pointer to OPimTodo or 0l
 */
OPimTodo* OPimTodo::safeCast( const OPimRecord* rec ) {
    return (rec && rec->rtti() == OPimResolver::TodoList ) ?
        static_cast<OPimTodo*>( const_cast<OPimRecord*>(rec) ) :
        0l;
}

}
