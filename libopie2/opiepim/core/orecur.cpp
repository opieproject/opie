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

#include "orecur.h"

/* OPIE */
#include <opie2/otimezone.h>
#include <qpe/timeconversion.h>

/* QT */
#include <qshared.h>

/* STD */
#include <time.h>

namespace Opie {

struct ORecur::Data : public QShared {
    Data() : QShared() {
        type = ORecur::NoRepeat;
        freq = -1;
        days = 0;
        pos = 0;
        create = QDateTime::currentDateTime();
        hasEnd = FALSE;
        end = QDate::currentDate();
    }
    char days; // Q_UINT8 for 8 seven days;)
    ORecur::RepeatType type;
    int freq;
    int pos;
    bool hasEnd : 1;
    QDate end;
    QDateTime create;
    int rep;
    QString app;
    ExceptionList list;
    QDate start;
};


ORecur::ORecur() {
    data = new Data;
}

ORecur::ORecur( const QMap<int, QString>& map )
{
    ORecur();
    fromMap( map );
}


ORecur::ORecur( const ORecur& rec)
    : data( rec.data )
{
    data->ref();
}


ORecur::~ORecur() {
    if ( data->deref() ) {
        delete data;
        data = 0l;
    }
}


void ORecur::deref() {
    if ( data->deref() ) {
        delete data;
        data = 0l;
    }
}


bool ORecur::operator==( const ORecur& )const {
    return false;
}


ORecur &ORecur::operator=( const ORecur& re) {
    if ( *this == re ) return *this;

    re.data->ref();
    deref();
    data = re.data;

    return *this;
}


bool ORecur::doesRecur()const {
    return !( type() == NoRepeat );
}


/*
 * we try to be smart here
 *
 */
bool ORecur::doesRecur( const QDate& date ) {
    /* the day before the recurrance */
    QDate da = date.addDays(-1);

    QDate recur;
    if (!nextOcurrence( da, recur ) )
        return false;

    return (recur == date);
}


// FIXME unuglify!
// GPL from Datebookdb.cpp
// FIXME exception list!
bool ORecur::nextOcurrence( const QDate& from, QDate& next ) {
    bool stillLooking;
    stillLooking  = p_nextOccurrence( from, next );
    while ( stillLooking && data->list.contains(next) )
        stillLooking = p_nextOccurrence( next.addDays(1), next );

    return stillLooking;
}


bool ORecur::p_nextOccurrence( const QDate& from, QDate& next ) {

   // easy checks, first are we too far in the future or too far in the past?
    QDate tmpDate;
    int freq = frequency();
    int diff, diff2, a;
    int iday, imonth, iyear;
    int dayOfWeek = 0;
    int firstOfWeek = 0;
    int weekOfMonth;


    if (hasEndDate() && endDate() < from)
        return FALSE;

    if (start() >= from  ) {
        next = start();
        return TRUE;
    }

    switch ( type() ) {
        case Weekly:
            /* weekly is just daily by 7 */
            /* first convert the repeatPattern.Days() mask to the next
            day of week valid after from */
            dayOfWeek = from.dayOfWeek();
            dayOfWeek--; /* we want 0-6, doco for above specs 1-7 */

            /* this is done in case freq > 1 and from in week not
            for this round */
            // firstOfWeek = 0; this is already done at decl.
            while(!((1 << firstOfWeek) & days() ))
                firstOfWeek++;

            /* there is at least one 'day', or there would be no event */
            while(!((1 << (dayOfWeek % 7)) & days() ))
                dayOfWeek++;

            dayOfWeek = dayOfWeek % 7; /* the actual day of week */
            dayOfWeek -= start().dayOfWeek() -1;

            firstOfWeek = firstOfWeek % 7; /* the actual first of week */
            firstOfWeek -= start().dayOfWeek() -1;

            // dayOfWeek may be negitive now
            // day of week is number of days to add to start day

            freq *= 7;
            // FALL-THROUGH !!!!!
        case Daily:
            // the add is for the possible fall through from weekly */
            if(start().addDays(dayOfWeek) > from) {
                /* first week exception */
                next = QDate(start().addDays(dayOfWeek) );
                if ((next > endDate())
                    && hasEndDate() )
                    return FALSE;
                return TRUE;
            }
            /* if from is middle of a non-week */

            diff = start().addDays(dayOfWeek).daysTo(from) % freq;
            diff2 = start().addDays(firstOfWeek).daysTo(from) % freq;

            if(diff != 0)
                diff = freq - diff;
            if(diff2 != 0)
                diff2 = freq - diff2;
            diff = QMIN(diff, diff2);

            next = QDate(from.addDays(diff));
            if ( (next > endDate())
                && hasEndDate() )
                return FALSE;
            return TRUE;
        case MonthlyDay:
            iday = from.day();
            iyear = from.year();
            imonth = from.month();
            /* find equivelent day of month for this month */
            dayOfWeek = start().dayOfWeek();
            weekOfMonth = (start().day() - 1) / 7;

            /* work out when the next valid month is */
            a = from.year() - start().year();
            a *= 12;
            a = a + (imonth - start().month());
            /* a is e.start()monthsFrom(from); */
            if(a % freq) {
                a = freq - (a % freq);
                imonth = from.month() + a;
                if (imonth > 12) {
                    imonth--;
                    iyear += imonth / 12;
                    imonth = imonth % 12;
                    imonth++;
                }
            }
            /* imonth is now the first month after or on
            from that matches the frequency given */

            /* find for this month */
            tmpDate = QDate( iyear, imonth, 1 );

            iday = 1;
            iday += (7 + dayOfWeek - tmpDate.dayOfWeek()) % 7;
            iday += 7 * weekOfMonth;
            while (iday > tmpDate.daysInMonth()) {
                imonth += freq;
                if (imonth > 12) {
                    imonth--;
                    iyear += imonth / 12;
                    imonth = imonth % 12;
                    imonth++;
                }
                tmpDate = QDate( iyear, imonth, 1 );
                /* these loops could go for a while, check end case now */
                if ((tmpDate > endDate()) && hasEndDate() )
                    return FALSE;
                iday = 1;
                iday += (7 + dayOfWeek - tmpDate.dayOfWeek()) % 7;
                iday += 7 * weekOfMonth;
            }
            tmpDate = QDate(iyear, imonth, iday);

            if (tmpDate >= from) {
                next = tmpDate;
                if ((next > endDate() ) && hasEndDate() )
                    return FALSE;
                return TRUE;
            }

            /* need to find the next iteration */
            do {
                imonth += freq;
                if (imonth > 12) {
                    imonth--;
                    iyear += imonth / 12;
                    imonth = imonth % 12;
                    imonth++;
                }
                tmpDate = QDate( iyear, imonth, 1 );
                /* these loops could go for a while, check end case now */
                if ((tmpDate > endDate()) && hasEndDate() )
                    return FALSE;
                iday = 1;
                iday += (7 + dayOfWeek - tmpDate.dayOfWeek()) % 7;
                iday += 7 * weekOfMonth;
            } while (iday > tmpDate.daysInMonth());
            tmpDate = QDate(iyear, imonth, iday);

            next = tmpDate;
            if ((next > endDate()) && hasEndDate() )
                return FALSE;
            return TRUE;
        case MonthlyDate:
            iday = start().day();
            iyear = from.year();
            imonth = from.month();

            a = from.year() - start().year();
            a *= 12;
            a = a + (imonth - start().month());
            /* a is e.start()monthsFrom(from); */
            if(a % freq) {
                a = freq - (a % freq);
                imonth = from.month() + a;
                if (imonth > 12) {
                    imonth--;
                    iyear += imonth / 12;
                    imonth = imonth % 12;
                    imonth++;
                }
            }
            /* imonth is now the first month after or on
            from that matches the frequencey given */

            /* this could go for a while, worse case, 4*12 iterations, probably */
            while(!QDate::isValid(iyear, imonth, iday) ) {
                imonth += freq;
                if (imonth > 12) {
                    imonth--;
                    iyear += imonth / 12;
                    imonth = imonth % 12;
                    imonth++;
                }
                /* these loops could go for a while, check end case now */
                if ((QDate(iyear, imonth, 1) > endDate()) && hasEndDate() )
                    return FALSE;
            }

            if(QDate(iyear, imonth, iday) >= from) {
                /* done */
                next = QDate(iyear, imonth, iday);
                if ((next > endDate()) && hasEndDate() )
                    return FALSE;
                return TRUE;
            }

            /* ok, need to cycle */
            imonth += freq;
            imonth--;
            iyear += imonth / 12;
            imonth = imonth % 12;
            imonth++;

            while(!QDate::isValid(iyear, imonth, iday) ) {
                imonth += freq;
                imonth--;
                iyear += imonth / 12;
                imonth = imonth % 12;
                imonth++;
                if ((QDate(iyear, imonth, 1) > endDate()) && hasEndDate() )
                    return FALSE;
            }

            next = QDate(iyear, imonth, iday);
            if ((next > endDate()) && hasEndDate() )
                return FALSE;
            return TRUE;
        case Yearly:
            iday = start().day();
            imonth = start().month();
            iyear = from.year(); // after all, we want to start in this year

            diff = 1;
            if(imonth == 2 && iday > 28) {
                /* leap year, and it counts, calculate actual frequency */
                if(freq % 4)
                    if (freq % 2)
                        freq = freq * 4;
                    else
                        freq = freq * 2;
                /* else divides by 4 already, leave freq alone */
                diff = 4;
            }

            a = from.year() - start().year();
            if(a % freq) {
                a = freq - (a % freq);
                iyear = iyear + a;
            }

            /* under the assumption we won't hit one of the special not-leap years twice */
            if(!QDate::isValid(iyear, imonth, iday)) {
                /* must have been skipping by leap years and hit one that wasn't, (e.g. 2100) */
                iyear += freq;
            }

            if(QDate(iyear, imonth, iday) >= from) {
                next = QDate(iyear, imonth, iday);

                if ((next > endDate()) && hasEndDate() )
                    return FALSE;
                return TRUE;
            }
            /* iyear == from.year(), need to advance again */
            iyear += freq;
            /* under the assumption we won't hit one of the special not-leap years twice */
            if(!QDate::isValid(iyear, imonth, iday)) {
                /* must have been skipping by leap years and hit one that wasn't, (e.g. 2100) */
                iyear += freq;
            }

            next = QDate(iyear, imonth, iday);
            if ((next > endDate()) && hasEndDate() )
                return FALSE;
            return TRUE;
        default:
            return FALSE;
    }
}


ORecur::RepeatType ORecur::type()const{
    return data->type;
}


int ORecur::frequency()const {
    return data->freq;
}


int ORecur::position()const {
    return data->pos;
}


char ORecur::days() const{
    return data->days;
}


bool ORecur::hasEndDate()const {
    return data->hasEnd;
}


QDate ORecur::endDate()const {
    return data->end;
}


QDate ORecur::start()const{
    return data->start;
}


QDateTime ORecur::createdDateTime()const {
    return data->create;
}


int ORecur::repetition()const {
    return data->rep;
}


QString ORecur::service()const {
    return data->app;
}


ORecur::ExceptionList& ORecur::exceptions() {
    return data->list;
}


void ORecur::setType( const RepeatType& z) {
    checkOrModify();
    data->type = z;
}


void ORecur::setFrequency( int freq ) {
    checkOrModify();
    data->freq = freq;
}


void ORecur::setPosition( int pos ) {
    checkOrModify();
    data->pos = pos;
}


void ORecur::setDays( char c ) {
    checkOrModify();
    data->days = c;
}


void ORecur::setEndDate( const QDate& dt) {
    checkOrModify();
    data->end = dt;
}


void ORecur::setCreatedDateTime( const QDateTime& t) {
    checkOrModify();
    data->create = t;
}


void ORecur::setHasEndDate( bool b) {
    checkOrModify();
    data->hasEnd = b;
}


void ORecur::setRepitition( int rep ) {
    checkOrModify();
    data->rep = rep;
}


void ORecur::setService( const QString& app ) {
    checkOrModify();
    data->app = app;
}


void ORecur::setStart( const QDate& dt ) {
    checkOrModify();
    data->start = dt;
}


void ORecur::checkOrModify() {
    if ( data->count !=  1 ) {
        data->deref();
        Data* d2 = new Data;
        d2->days = data->days;
        d2->type = data->type;
        d2->freq = data->freq;
        d2->pos  = data->pos;
        d2->hasEnd = data->hasEnd;
        d2->end  = data->end;
        d2->create = data->create;
        d2->rep = data->rep;
        d2->app = data->app;
        d2->list = data->list;
        d2->start = data->start;
        data = d2;
    }
}


QString ORecur::toString()const {
    QString buf;
    QMap<int, QString> recMap = toMap();

    buf += " rtype=\"";
    buf += recMap[ORecur::RType];
    buf += "\"";
    if (data->days > 0 )
        buf += " rweekdays=\"" + recMap[ORecur::RWeekdays] + "\"";
    if ( data->pos != 0 )
        buf += " rposition=\"" + recMap[ORecur::RPosition] + "\"";

    buf += " rfreq=\"" + recMap[ORecur::RFreq] + "\"";
    buf += " rhasenddate=\"" + recMap[ORecur::RHasEndDate]+ "\"";
    if ( data->hasEnd )
        buf += " enddt=\""
            + recMap[ORecur::EndDate]
            + "\"";
    buf += " created=\"" + recMap[ORecur::Created] + "\"";

    if ( data->list.isEmpty() ) return buf;
    buf += " exceptions=\"";
    buf += recMap[ORecur::Exceptions];
    buf += "\" ";

    return buf;
}

QString ORecur::rTypeString() const
{
    QString retString;
    switch ( data->type ) {
    case ORecur::Daily:
            retString = "Daily";
            break;
    case ORecur::Weekly:
            retString = "Weekly";
            break;
    case ORecur::MonthlyDay:
            retString = "MonthlyDay";
            break;
    case ORecur::MonthlyDate:
            retString = "MonthlyDate";
            break;
    case ORecur::Yearly:
            retString = "Yearly";
            break;
    default:
            retString = "NoRepeat";
            break;

    }

    return retString;
}

QMap<QString, ORecur::RepeatType> ORecur::rTypeValueConvertMap() const
{
        QMap<QString, RepeatType> convertMap;

        convertMap.insert( QString( "Daily" ), ORecur::Daily );
        convertMap.insert( QString( "Weekly" ), ORecur::Weekly );
        convertMap.insert( QString( "MonthlyDay" ), ORecur::MonthlyDay );
        convertMap.insert( QString( "MonthlyDate" ), ORecur::MonthlyDate );
        convertMap.insert( QString( "Yearly" ), ORecur::Yearly );
        convertMap.insert( QString( "NoRepeat" ), ORecur::NoRepeat );

        return convertMap;
}


QMap<int, QString> ORecur::toMap() const
{
    QMap<int, QString> retMap;
    
    retMap.insert( ORecur::RType, rTypeString() );
    retMap.insert( ORecur::RWeekdays, QString::number( static_cast<int>( data->days ) ) );
    retMap.insert( ORecur::RPosition, QString::number(data->pos ) );
    retMap.insert( ORecur::RFreq, QString::number( data->freq ) );
    retMap.insert( ORecur::RHasEndDate, QString::number( static_cast<int>( data->hasEnd ) ) );
    if( data -> hasEnd )
            retMap.insert( ORecur::EndDate, QString::number( OTimeZone::utc().fromUTCDateTime( QDateTime( data->end, QTime(12,0,0) ) ) ) );
    retMap.insert( ORecur::Created, QString::number( OTimeZone::utc().fromUTCDateTime( data->create ) ) );
    
    if ( data->list.isEmpty() ) return retMap;

    // save exceptions list here!!
    ExceptionList::ConstIterator it;
    ExceptionList list = data->list;
    QString exceptBuf;
    QDate date;
    for ( it = list.begin(); it != list.end(); ++it ) {
            date = (*it);
            if ( it != list.begin() ) exceptBuf += " ";
            
            exceptBuf += QCString().sprintf("%04d%02d%02d", date.year(), date.month(), date.day() );
    }

    retMap.insert( ORecur::Exceptions, exceptBuf );

    return retMap;
}

void ORecur::fromMap( const QMap<int, QString>& map )
{
    QMap<QString, RepeatType> repTypeMap = rTypeValueConvertMap(); 

    data -> type  = repTypeMap[ map [ORecur::RType] ];
    data -> days  = (char) map[ ORecur::RWeekdays ].toInt();
    data -> pos   = map[ ORecur::RPosition ].toInt();
    data -> freq = map[ ORecur::RFreq ].toInt();
    data -> hasEnd= map[ ORecur::RHasEndDate ].toInt() ? true : false;
    OTimeZone utc = OTimeZone::utc();
    if ( data -> hasEnd ){
            data -> end = utc.fromUTCDateTime( (time_t) map[ ORecur::EndDate ].toLong() ).date();
    }
    data -> create = utc.fromUTCDateTime( (time_t) map[ ORecur::Created ].toLong() ).date();

#if 0
    // FIXME: Exceptions currently not supported...
    // Convert the list of exceptions from QString into ExceptionList
    data -> list.clear();
    QString exceptStr = map[ ORecur::Exceptions ];
    QStringList exceptList = QStringList::split( " ", exceptStr );
    ...
#endif
        
        
}

}
