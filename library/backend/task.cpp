/**********************************************************************
** Copyright (C) 2001 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include <qpe/task.h>
#include <qregexp.h>
#include <qstring.h>
#include <qpe/recordfields.h>
#include "vobject_p.h"
#include "timeconversion.h"
#include "qfiledirect_p.h"

#include <stdio.h>

using namespace Qtopia;
UidGen Task::sUidGen( UidGen::Qtopia );

Task::Task() : Record(), mDue( FALSE ),
mDueDate( QDate::currentDate() ),
mCompleted( FALSE ), mPriority( 3 ), mDesc()
{
}

Task::Task( const QMap<int, QString> &m ) : Record(), mDue( FALSE ),
mDueDate( QDate::currentDate() ), mCompleted( FALSE ), mPriority( 3 ), mDesc()
{
    //qDebug("Task::Task fromMap");
    //dump( m );
    for ( QMap<int,QString>::ConstIterator it = m.begin(); it != m.end();++it )
	switch ( (TaskFields) it.key() ) {
	case HasDate: if ( *it == "1" ) mDue = TRUE; break;
	case Completed: setCompleted( *it == "1" ); break;
	case TaskCategory: setCategories( idsFromString( *it ) ); break;
	case TaskDescription: setDescription( *it ); break;
	case Priority: setPriority( (*it).toInt() ); break;
	case Date: mDueDate = TimeConversion::fromString( (*it) ); break;
	case TaskUid: setUid( (*it).toInt() ); break;
	default: break;
	}
}

Task::~Task()
{
}

QMap<int, QString> Task::toMap() const
{
    QMap<int, QString> m;
    m.insert( HasDate, hasDueDate() ? "1" : "0" );
    m.insert( Completed, isCompleted() ? "1" : "0" );
    m.insert( TaskCategory, idsToString( categories() ) );
    m.insert( TaskDescription, description() );
    m.insert( Priority, QString::number( priority() ) );
    m.insert( Date, TimeConversion::toString( dueDate() ) );
    m.insert( TaskUid, QString::number(uid()) );

    //qDebug("Task::toMap");
    //dump( m );
    return m;
}

void Task::save( QString& buf ) const
{
    buf += " Completed=\"";
    //    qDebug( "writing %d", complete );
    buf += QString::number( (int)mCompleted );
    buf += "\"";
    buf += " HasDate=\"";
    //    qDebug( "writing %d",  );
    buf += QString::number( (int)mDue );
    buf += "\"";
    buf += " Priority=\"";
    //    qDebug ("writing %d", prior );
    buf += QString::number( mPriority );
    buf += "\"";
    buf += " Categories=\"";
    buf += Qtopia::Record::idsToString( categories() );
    buf += "\"";
    buf += " Description=\"";
    //    qDebug( "writing note %s", note.latin1() );
    buf += Qtopia::escapeString( mDesc );
    buf += "\"";
    if ( mDue ) {
	// qDebug("saving ymd %d %d %d", mDueDate.year(), mDueDate.month(),
	// mDueDate.day() );
	buf += " DateYear=\"";
	buf += QString::number( mDueDate.year() );
	buf += "\"";
	buf += " DateMonth=\"";
	buf += QString::number( mDueDate.month() );
	buf += "\"";
	buf += " DateDay=\"";
	buf += QString::number( mDueDate.day() );
	buf += "\"";
    }
    buf += customToXml();
    //    qDebug ("writing uid %d", uid() );
    buf += " Uid=\"";
    buf += QString::number( uid() );
    // terminate it in the application...
    buf += "\"";
}

bool Task::match ( const QRegExp &r ) const
{
    // match on priority, description on due date...
    bool match;
    match = false;
    if ( QString::number( mPriority ).find( r ) > -1 )
	match = true;
    else if ( mDue && mDueDate.toString().find( r ) > -1 )
	match = true;
    else if ( mDesc.find( r ) > -1 )
	match = true;
    return match;
}

static inline VObject *safeAddPropValue( VObject *o, const char *prop, const QString &value )
{
    VObject *ret = 0;
    if ( o && !value.isEmpty() )
	ret = addPropValue( o, prop, value.latin1() );
    return ret;
}

static inline VObject *safeAddProp( VObject *o, const char *prop)
{
    VObject *ret = 0;
    if ( o )
	ret = addProp( o, prop );
    return ret;
}


static VObject *createVObject( const Task &t )
{
    VObject *vcal = newVObject( VCCalProp );
    safeAddPropValue( vcal, VCVersionProp, "1.0" );
    VObject *task = safeAddProp( vcal, VCTodoProp );

    if ( t.hasDueDate() )
	safeAddPropValue( task, VCDueProp, TimeConversion::toISO8601( t.dueDate() ) );
    safeAddPropValue( task, VCDescriptionProp, t.description() );
    if ( t.isCompleted() )
	safeAddPropValue( task, VCStatusProp, "COMPLETED" );
    safeAddPropValue( task, VCPriorityProp, QString::number( t.priority() ) );

    return vcal;
}


static Task parseVObject( VObject *obj )
{
    Task t;

    VObjectIterator it;
    initPropIterator( &it, obj );
    while( moreIteration( &it ) ) {
	VObject *o = nextVObject( &it );
	QCString name = vObjectName( o );
	QCString value = vObjectStringZValue( o );
	if ( name == VCDueProp ) {
	    t.setDueDate( TimeConversion::fromISO8601( value ).date(), TRUE );
	}
	else if ( name == VCDescriptionProp ) {
	    t.setDescription( value );
	}
	else if ( name == VCStatusProp ) {
	    if ( value == "COMPLETED" )
		t.setCompleted( TRUE );
	}
	else if ( name == VCPriorityProp ) {
	    t.setPriority( value.toInt() );
	}
#if 0
	else {
	    printf("Name: %s, value=%s\n", name.data(), vObjectStringZValue( o ) );
	    VObjectIterator nit;
	    initPropIterator( &nit, o );
	    while( moreIteration( &nit ) ) {
		VObject *o = nextVObject( &nit );
		QCString name = vObjectName( o );
		QString value = vObjectStringZValue( o );
		printf(" subprop: %s = %s\n", name.data(), value.latin1() );
	    }
	}
#endif
    }

    return t;
}



void Task::writeVCalendar( const QString &filename, const QValueList<Task> &tasks)
{
  QFileDirect f( filename.utf8().data() );
  if ( !f.open( IO_WriteOnly ) ) {
    qWarning("Unable to open vcard write");
    return;
  }

    QValueList<Task>::ConstIterator it;
    for( it = tasks.begin(); it != tasks.end(); ++it ) {
	VObject *obj = createVObject( *it );
	writeVObject(f.directHandle() , obj );
	cleanVObject( obj );
    }

    cleanStrTbl();
}

void Task::writeVCalendar( const QString &filename, const Task &task)
{
  QFileDirect f( filename.utf8().data() );
  if ( !f.open( IO_WriteOnly ) ) {
    qWarning("Unable to open vcard write");
    return;
  }

  VObject *obj = createVObject( task );
  writeVObject(f.directHandle() , obj );
  cleanVObject( obj );

  cleanStrTbl();
}


QValueList<Task> Task::readVCalendar( const QString &filename )
{
    VObject *obj = Parse_MIME_FromFileName( (char *)filename.utf8().data() );

    QValueList<Task> tasks;

    while ( obj ) {
	QCString name = vObjectName( obj );
	if ( name == VCCalProp ) {
	    VObjectIterator nit;
	    initPropIterator( &nit, obj );
	    while( moreIteration( &nit ) ) {
		VObject *o = nextVObject( &nit );
		QCString name = vObjectName( o );
		if ( name == VCTodoProp )
		    tasks.append( parseVObject( o ) );
	    }
	} else if ( name == VCTodoProp ) {
	    // shouldn't happen, but just to be sure
	    tasks.append( parseVObject( obj ) );
	}
	VObject *t = obj;
	obj = nextVObjectInList(obj);
	cleanVObject( t );
    }

    return tasks;
}
