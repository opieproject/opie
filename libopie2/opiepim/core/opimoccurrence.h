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

#ifndef OPIE_PIM_OCCURRENCE_H
#define OPIE_PIM_OCCURRENCE_H

#include <opie2/osharedpointer.h>
#include <opie2/opimrecord.h>
#include <opie2/opimevent.h>
#include <opie2/opimtodo.h>
#include <opie2/opimcontact.h>

#include <qdatetime.h>
#include <qstringlist.h>

namespace Opie {

template<class T> class OPimAccessTemplate;
/**
 * \brief An OPimOccurrence represents a occurence for one day of a OPimRecord
 *
 * An OPimOccurrence represents the occurrence of one OPimRecord
 * for a period of Time for one day. An occurrence can spawn
 * more then one day and then is splitted into multiple OPimOccurrence.
 * By attributes you can find if a OPimOccurrence is the beginning and
 * end, begin, end or is midway of  a multiday occurrence.
 *
 */
class OPimOccurrence {
    friend class OPimBase;
public:
    typedef QValueList<OPimOccurrence> List;
    /**
     * The position of the OPimOccurrence in a possible
     * MultiDay Occurrence.
     */
    enum Position {
        MidWay,  /* This OPimOccurrence is somewhere in between Start and End  */
        Start,   /* This OPimOccurrence is the Start of a multi day Occurrence */
        End,     /* This OPimOccurrence is the End of a multi day Occurrence   */
        StartEnd /* This OPimOccurrence only spans one day                     */
    };

    //@{
    OPimOccurrence();
    OPimOccurrence( const OPimOccurrence& );
    ~OPimOccurrence();
    //@}

    //@{
    void setPeriod( const QDate& from );
    void setPeriod( const QDateTime& from, const QDateTime& to );
    void setPeriod( const QDate& from,  const QTime& start, const QTime& end );
    //@}

    //@{
    bool isAllDay()const;
    QDate date()const;
    QTime startTime()const;
    QTime endTime()const;
    QDateTime startDateTime()const;
    QDateTime endDateTime()const;
    //@}

    //@{
    QString summary()const;
    QString location()const;
    QString note()const;
    //@}

    //@{
    int length()const;
    Position position()const;
    void setPosition( enum Position& );
    //@}

    //@{
    Opie::Core::OSharedPointer<OPimRecord> record()const;
    OPimEvent   toEvent()const;
    OPimTodo    toTodo()const;
    OPimContact toContact()const;
    //@}


    //@{
    bool operator< ( const OPimOccurrence& )const;
    bool operator<=( const OPimOccurrence& )const;
    bool operator==( const OPimOccurrence& )const;
    bool operator!=( const OPimOccurrence& )const;
    bool operator> ( const OPimOccurrence& )const;
    bool operator>=( const OPimOccurrence& )const;
    OPimOccurrence &operator=( const OPimOccurrence& );
    //@}

private:
    QDate m_occurrence;
    QTime m_start, m_end;
    bool m_isAllDay : 1;
    enum Position m_pos;

    void deref();
    inline void changeOrModify();

    struct Private;
    struct Data;

    Data *data;
    Private *d;

private: // ctor
    OPimOccurrence( OPimOccurrence::Data *, enum Position );
    template<class T> T internalToRecord()const;
};
}

#endif
