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

#ifndef OPIE_PIM_BACKEND_OCCURRENCE_H
#define OPIE_PIM_BACKEND_OCCURRENCE_H

#include <opie2/opimglobal.h>

#include <qarray.h>
#include <qdatetime.h>
#include <qvaluelist.h>

namespace Opie {

/**
 * \brief Internal representation of an Occurence
 *
 * This class is used by the Backends to express
 * Occurences for the Period Based Query to
 * the by the Backend represanted Database.
 * In the Frontend this single representation is splitted
 * into per day \sa OPimOccurrence 's.
 * OPimBackendOccurrence can be understand as a hint to
 * the Frontend and must contain the \sa UID, the Start Date
 * and End Date of the Occurence. If you have no time associated
 * to the datetime use the QDate constructors.
 * If OPimRecord::summary() does not describe the Occurrence
 * right you can call setSummary() and then the supplied
 * summary will be used.
 * All Dates and Times are in the local time.
 *
 * @version 1.0
 * @author Holger Hans Peter Freyther zecke@handhelds.org
 */
class OPimBackendOccurrence {
public:
    typedef QValueList<OPimBackendOccurrence> List;

    //@{
    OPimBackendOccurrence();
    OPimBackendOccurrence( const QDate& date,
                           const UID& , const QString& = QString::null );
    OPimBackendOccurrence( const QDate& date, const QDate& end,
			   const UID& );
    OPimBackendOccurrence( const QDateTime& start,
			   const QDateTime& end,
			   const UID& uid );
    //@}

    //@{
    QDateTime startDateTime()const;
    QDateTime endDateTime()const;
    UID       uid()const;
    bool      isAllDay()const;
    QString   summary()const;
    QString   location()const;
    QString   note()const;
    //@}

    //@{
    void setStartDate( const QDate& );
    void setStartDateTime( const QDateTime& dt );
    void setEndDate( const QDate& );
    void setEndDateTime( const QDateTime& dt );
    void setUid( const UID& );
    void setSummary( const QString& );
    void setLocation( const QString& );
    void setNote( const QString& );
    //@}

private:
    QDateTime m_start, m_end;
    UID       m_uid;
    bool      m_haveTime : 1;
    QString   m_summary, m_note, m_location;

    struct Private;
    Private *d;
};
}

#endif
