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

#ifndef ORECUR_H
#define ORECUR_H

/* QT */
#include <qdatetime.h>
#include <qvaluelist.h>
#include <qmap.h>

/* STD */
#include <sys/types.h>

namespace Opie {
/**
 * Class to handle Recurrencies..
 */

class OPimRecurrence {

  public:
    typedef QValueList<QDate> ExceptionList;
    enum RepeatType{ NoRepeat = -1, Daily, Weekly, MonthlyDay,
                     MonthlyDate, Yearly };
    enum Days { MON = 0x01, TUE = 0x02, WED = 0x04, THU = 0x08,
                FRI = 0x10, SAT = 0x20, SUN = 0x40 };
    enum Fields{ RType = 0, RWeekdays, RPosition, RFreq, RHasEndDate, 
                EndDate, Created, Exceptions };

    OPimRecurrence();
    OPimRecurrence( const QMap<int, QString>& map );
    OPimRecurrence( const OPimRecurrence& );
    ~OPimRecurrence();

    OPimRecurrence &operator=( const OPimRecurrence& );
    bool operator==(const OPimRecurrence& )const;

    bool doesRecur()const;
    /* if it recurrs on that day */
    bool doesRecur( const QDate& );
    RepeatType type()const;
    int frequency()const;
    int position()const;
    char days()const;
    bool hasEndDate()const;
    QDate start()const;
    QDate endDate()const;
    QDateTime createdDateTime()const;
    /**
     * starting on monday=0, sunday=6
     * for convience
     */
    bool repeatOnWeekDay( int day )const;

    /**
     * FromWhereToStart is not included!!!
     */
    bool nextOcurrence( const QDate& FromWhereToStart, QDate &recurDate );

    /**
     * The module this OPimRecurrence belongs to
     */
    QString service()const;

    /*
     * reference to the exception list
     */
    ExceptionList &exceptions();

    /**
     * the current repetition
     */
    int repetition()const;

    void setType( const RepeatType& );
    void setFrequency( int freq );
    void setPosition( int pos );
    void setDays( char c);
    void setEndDate( const QDate& dt );
    void setStart( const QDate& dt );
    void setCreatedDateTime( const QDateTime& );
    void setHasEndDate( bool b );
    void setRepitition(int );

    void setService( const QString& ser );
    
    QMap<int, QString> toMap() const;
    void fromMap( const QMap<int, QString>& map );

    /* almost internal */
    QString toString()const;
    
  private:
    bool p_nextOccurrence( const QDate& from, QDate& next );
    void deref();
    inline void checkOrModify();

    /* Converts rType to String */
    QString rTypeString() const;
    /* Returns a map to convert Stringname for RType to RepeatType */
    QMap<QString, RepeatType> rTypeValueConvertMap() const;

    class Data;
    Data* data;
    class OPimRecurrencePrivate;
    OPimRecurrencePrivate *d;
};

}
#endif
