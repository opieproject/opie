/*
                             This file is part of the Opie Project
                             Copyright (C) 2004 Holger Hans Peter Freyther <zecke@handhelds.org>
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

#include "opimbackendoccurrence.h"

namespace Opie {

OPimBackendOccurrence::OPimBackendOccurrence() {}
/**
 * \brief The occurence is only on the specefic Day
 *
 * If an occurrence is only a day without any time associated
 * use this Constructor.
 * \sa timeAssociated() will return false.
 *
 * @param date The Date this Occurence takes place
 * @param  uid The \sa UID of the associated OPimRecord
 * @param  sum The optional summary
 *
 */
OPimBackendOccurrence::OPimBackendOccurrence( const QDate& date,
                                              const UID& uid,
                                              const QString& sum )
    : m_start( date ), m_end( date ), m_uid( uid ),
      m_haveTime(false ), m_summary( sum )
{}

/**
 * \brief An Occurrence with a start day and end day without time
 *
 * Overloaded Constructor. Use this if you've a start date and
 * end date but no time. If you need to overwrite the summary
 * use setSummary.
 * \sa timeAssociated() will return false.
 *
 * @param date The Start Date
 * @param  end Tne End Date
 * @param  uid The UID of the associated record
 *
 * @see setSummary
 */
OPimBackendOccurrence::OPimBackendOccurrence( const QDate& date,
                                              const QDate& end,
                                              const UID& uid)
    : m_start( date ), m_end( end ), m_uid( uid ), m_haveTime( false )
{}


/**
 * \brief Use Start and End Date with Time
 *
 * Overloaded Constructor to use Dates with Time time associated
 * to it. \sa timeAssociated() will return true.
 *
 * @param date The Start Date and Time of the occurrence
 * @param  end The End   Date and Time of the occurrence
 * @param  uid The UID of the \sa OPimRecord.
 */
OPimBackendOccurrence::OPimBackendOccurrence( const QDateTime& date,
                                              const QDateTime& end,
                                              const UID& uid )
    : m_start( date ), m_end( end ), m_uid( uid ), m_haveTime( true )
{}

/**
 * \brief Return the Start Date and Time
 *
 * @return This method will return the start
 *  Date and Time. Time is only valid if
 *  \sa timeAssociated() is true.
 *
 */
QDateTime OPimBackendOccurrence::startDateTime()const {
    return m_start;
}

/**
 * \brief Return the Start Date and Time
 *
 * @return This will return the end Date and Time. The
 * limitation for Time is the same as in startDateTime
 *
 * @see startDateTime()
 */
QDateTime OPimBackendOccurrence::endDateTime()const {
    return m_end;
}

/**
 * \brief Return the UID of the Associated OPimRecord
 *
 * @return the associated OPimRecord
 */
UID OPimBackendOccurrence::uid()const {
    return m_uid;
}

/**
 * \brief Return if there is a time associated or not
 *
 * If a time is present with start and end date this method
 * will return true. There is no direct way to manipulate
 * that attribute. But \sa setStartDate and \sa setStartDateTime
 * will change it.
 *
 * @return Return true if a time is available with the date
 *
 */
bool OPimBackendOccurrence::isAllDay()const {
    return m_haveTime;
}

/**
 * @return The special summary that will overwrite OPimRecord::summary
 */
QString OPimBackendOccurrence::summary()const {
    return m_summary;
}

QString OPimBackendOccurrence::location()const {
    return m_location;
}

QString OPimBackendOccurrence::note()const {
    return m_note;
}

/**
 * \brief Set the Start Date
 *
 * This method will set the start date and internally will mark
 * this occurrence to have no time associated to both start
 * and end date.
 * A call to timeAssociated will return false after using this
 * method.
 *
 * @param start The Start Date
 *
 */
void OPimBackendOccurrence::setStartDate( const QDate& start) {
    m_start = start;
    m_haveTime = false;
}

/**
 * \brief Set the Start Date and Time
 *
 * Set the Start Date and Time. After this call
 * \sa timeAssociated will return true.
 *
 * @param  dt The Start Date and Time to be set
 */
void OPimBackendOccurrence::setStartDateTime( const QDateTime& dt ) {
    m_start = dt;
    m_haveTime = true;
}

/**
 * \brief This will set the End Date.
 *
 * This method will set the End Date. The timeAssociated attribute
 * will not be changed.
 *
 * @param  end The End Date to be set
 */
void OPimBackendOccurrence::setEndDate( const QDate& end ) {
    m_end = end;
}

/**
 * \brief Set the End  Date and Time of the occurrence
 *
 * This will set the End Date and Time but will not change
 * the timeAssociated attribute.
 *
 * @param dt The End Date and Time to be set.
 */
void OPimBackendOccurrence::setEndDateTime( const QDateTime& dt ) {
    m_end = dt;
}

/**
 * \brief This method will set the UID of the Record
 *
 * Set the UID of the OPimRecord to be associated with
 * this OPimRecurrence.
 *
 * @param uid The UID of the associated OPimRecord to be set
 */
void OPimBackendOccurrence::setUid( const UID& uid ) {
    m_uid = uid;
}


/**
 * \brief Set a special summary instead of \sa OPimRecord::summary()
 *
 * If \sa OPimRecord::summary() doesn't describe the occurrence
 * reason you can set a custom summary for the Occurrence.
 *
 * @param str The to be set Summary
 */
void OPimBackendOccurrence::setSummary( const QString& str ) {
    m_summary = str;
}

void OPimBackendOccurrence::setLocation( const QString& str ) {
    m_location = str;
}

void OPimBackendOccurrence::setNote( const QString& str ) {
    m_note = str;
}

}
