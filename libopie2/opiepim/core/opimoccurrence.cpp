/*
                             This file is part of the Opie Project
                             Copyright (C) 2003, 2004 Holger Freyther <zecke@handhelds.org>
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

#include "opimoccurrence.h"
#include <opie2/opimtemplatebase.h>
#include <opie2/private/opimoccurrence_p.h>


/* QT */
#include <qshared.h>


namespace Opie {

OPimOccurrence::OPimOccurrence( OPimOccurrence::Data* _data,
                                enum OPimOccurrence::Position pos )
    : m_isAllDay( false ), m_pos( pos ), data( _data ), d( 0 )
{
    data->ref();
}

/**
 * \brief Copy constructor
 */
OPimOccurrence::OPimOccurrence( const OPimOccurrence& oc )
    : data( oc.data ), d( 0 )
{
    /*
     * Increment the reference count
     */
    data->ref();

    /*
     * copy the other information
     */
    m_start = oc.m_start;
    m_end   = oc.m_end;
    m_occurrence = oc.m_occurrence;
    m_isAllDay = oc.m_isAllDay;
    m_pos = oc.m_pos;
}

OPimOccurrence::OPimOccurrence()
    : m_isAllDay( false )
    , m_pos( StartEnd )
    , data( new OPimOccurrence::Data() )
    , d( 0 )
{}

OPimOccurrence::~OPimOccurrence() {
    deref();
}

/**
 * \brief Copy Operator
 */
OPimOccurrence& OPimOccurrence::operator=( const OPimOccurrence& oc ) {
    /* guard against self assignment */
    if ( this == &oc ) return *this;

    oc.data->ref();
    deref();
    data = oc.data;


    /*
     * copy the other information
     */
    m_start = oc.m_start;
    m_end   = oc.m_end;
    m_occurrence = oc.m_occurrence;
    m_isAllDay = oc.m_isAllDay;
    m_pos = oc.m_pos;

    return *this;
}


/**
 * @internal
 */
void OPimOccurrence::deref() {
    if ( data->deref() ) {
        delete data;
        data = 0;
    }
}

/**
 * \brief Set the Occurrence to be All Day on a specified QDate
 *
 * If no QTime is associated to a OPimOccurrence use this Method
 * to set the Period of this Occurrence. When using this Method
 * later calls to \sa isAllDay() will return true.
 * The Occurrence will be set to occurr on \par from.
 *
 * @param from The Day this OPimOccurrence occurs
 *
 */
void OPimOccurrence::setPeriod( const QDate& from ) {
    m_occurrence = from;
    m_start = m_end = QTime(); // assign invalid value just in case
    m_isAllDay = true;
}

/**
 * \brief Set the period of this Occurrence with a QTime associated (overloaded)
 *
 * Set the period of time for this Occurrence. Each Ocurrence is limited
 * to one day. Using this Method will make \sa isAllDay() return false.
 * If \par from and \par to are on two different days the QDate of the
 * \par from QDateTime will be used.
 *
 * @param from The Start Date Time of the Occurrence
 * @param to   The End Date Time of the Occurrence
 */
void OPimOccurrence::setPeriod( const QDateTime& from,  const QDateTime& to ) {
    m_occurrence = from.date();
    m_start = from.time();
    m_end = to.time();
    m_isAllDay = false;
}

/**
 * \brief Set the period of this Occurrence with a QTime associated
 *
 * @param from  The QDate of the Occurrence
 * @param start The Start QTime of the Occurrence
 * @param end   The End QTime of the Occurrence
 */
void OPimOccurrence::setPeriod( const QDate& from, const QTime& start,
                                const QTime& end ) {
    m_occurrence = from;
    m_start = start;
    m_end = end;
    m_isAllDay = false;
}


/**
 * \brief Is a QTime associated to the OPimOccurrence
 *
 * @return Return true if no QTime is associated
 */
bool OPimOccurrence::isAllDay()const {
    return m_isAllDay;
}


/**
 * \brief Return the QDate where this OPimOccurrence takes place
 * @return the QDate where this OPimOccurrence occurrs.
 */
QDate OPimOccurrence::date()const {
    return m_occurrence;
}


/**
 * \brief Return the start time of the OPimOccurrence
 *
 * @return Return the Start Time of the OPimOccurrence. It is
 * invalid if \sa isAllDay() returns true.
 */
QTime OPimOccurrence::startTime()const {
    return m_start;
}

QTime OPimOccurrence::endTime()const {
    return m_end;
}

QDateTime OPimOccurrence::startDateTime()const {
    return QDateTime( m_occurrence, m_start );
}

QDateTime OPimOccurrence::endDateTime()const {
    return QDateTime( m_occurrence, m_end );
}


QString OPimOccurrence::summary()const {
    return data->summary;
}

QString OPimOccurrence::location()const {
    return data->location;
}

QString OPimOccurrence::note()const {
    return data->note;
}


/**
 * -1 if no time is associated
 * otherwise the length of the occurrence in hours
 */
int OPimOccurrence::length()const {
    if ( m_isAllDay )
        return -1;
    else
        return ( m_end.hour() * 60 - m_start.hour() * 60 )
            + QABS( m_start.minute() - m_end.minute() );
}

enum OPimOccurrence::Position OPimOccurrence::position()const {
    return m_pos;
}

void OPimOccurrence::setPosition( enum OPimOccurrence::Position& pos ) {
    m_pos = pos;
}


Opie::Core::OSharedPointer<OPimRecord> OPimOccurrence::record()const {
    if ( !data->record && data->backend )
        data->record = data->backend->record( data->uid );
    return data->record;
}

template<class Record> Record OPimOccurrence::internalToRecord()const {
    Record target;

    /* If it is not loaded, try to load it using OPimBase */
    if ( !data->record && data->backend )
        data->record = data->backend->record( data->uid );

    Record *ta = Record::safeCast( data->record );
    if ( ta )
        target = *ta;


    return target;
}

OPimEvent OPimOccurrence::toEvent()const {
    return internalToRecord<OPimEvent>();
}

OPimTodo OPimOccurrence::toTodo()const {
    return internalToRecord<OPimTodo>();
}

OPimContact OPimOccurrence::toContact()const {
    return internalToRecord<OPimContact>();
}

bool OPimOccurrence::operator<( const OPimOccurrence& oc )const {
    if ( m_occurrence < oc.m_occurrence )
        return true;
    if ( m_occurrence == oc.m_occurrence )
        return m_start < oc.m_start;
    else
        return false;
}

bool OPimOccurrence::operator<=( const OPimOccurrence& oc )const {
    return ( m_occurrence <= oc.m_occurrence );
}

bool OPimOccurrence::operator==( const OPimOccurrence& oc )const {
    if ( data->uid != oc.data->uid )
        return false;
    if ( m_occurrence != oc.m_occurrence )
        return false;
    if ( m_isAllDay != oc.m_isAllDay )
        return false;
    if ( m_isAllDay && oc.m_isAllDay )
        if ( m_start != oc.m_start ||
             m_end   != oc.m_end )
            return false;
    if ( data->summary != oc.data->summary )
        return false;
    if ( data->note != oc.data->note )
        return false;
    if ( data->location != oc.data->location )
        return false;

    return true;
}

bool OPimOccurrence::operator!=( const OPimOccurrence& oc )const {
    return !( *this == oc );
}

bool OPimOccurrence::operator>( const OPimOccurrence& oc )const {
    return !( *this <= oc );
}

bool OPimOccurrence::operator>=( const OPimOccurrence& oc )const {
    return !( *this < oc );
}

}
