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

#include "opimrecurrence.h"

/* OPIE */
#include <opie2/opimtimezone.h>
#include <qpe/timeconversion.h>

/* QT */
#include <qshared.h>

/* STD */
#include <time.h>

namespace Opie {

struct OPimRecurrence::Data : public QShared {
    Data() : QShared() {
        type = OPimRecurrence::NoRepeat;
        freq = -1;
        days = 0;
        pos = 0;
        create = QDateTime::currentDateTime();
        hasEnd = FALSE;
        end = QDate::currentDate();
    }
    char days; // Q_UINT8 for 8 seven days;)
    OPimRecurrence::RepeatType type;
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


OPimRecurrence::OPimRecurrence() {
    data = new Data;
}

OPimRecurrence::OPimRecurrence( const QMap<int, QString>& map )
{
    OPimRecurrence();
    fromMap( map );
}


OPimRecurrence::OPimRecurrence( const OPimRecurrence& rec)
    : data( rec.data )
{
    data->ref();
}


OPimRecurrence::~OPimRecurrence() {
    if ( data->deref() ) {
        delete data;
        data = 0l;
    }
}


void OPimRecurrence::deref() {
    if ( data->deref() ) {
        delete data;
        data = 0l;
    }
}


bool OPimRecurrence::operator==( const OPimRecurrence& )const {
    return false;
}


OPimRecurrence &OPimRecurrence::operator=( const OPimRecurrence& re) {
    if ( *this == re ) return *this;

    re.data->ref();
    deref();
    data = re.data;

    return *this;
}


bool OPimRecurrence::doesRecur()const {
    return !( type() == NoRepeat );
}


/*
 * we try to be smart here
 *
 */
bool OPimRecurrence::doesRecur( const QDate& date ) {
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
bool OPimRecurrence::nextOcurrence( const QDate& from, QDate& next ) {
    bool stillLooking;
    stillLooking  = p_nextOccurrence( from, next );
    while ( stillLooking && data->list.contains(next) )
        stillLooking = p_nextOccurrence( next.addDays(1), next );

    return stillLooking;
}


bool OPimRecurrence::p_nextOccurrence( const QDate& from, QDate& next ) {

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


OPimRecurrence::RepeatType OPimRecurrence::type()const{
    return data->type;
}


int OPimRecurrence::frequency()const {
    return data->freq;
}


int OPimRecurrence::position()const {
    return data->pos;
}


char OPimRecurrence::days() const{
    return data->days;
}


bool OPimRecurrence::hasEndDate()const {
    return data->hasEnd;
}


QDate OPimRecurrence::endDate()const {
    return data->end;
}


QDate OPimRecurrence::start()const{
    return data->start;
}


QDateTime OPimRecurrence::createdDateTime()const {
    return data->create;
}


int OPimRecurrence::repetition()const {
    return data->rep;
}


QString OPimRecurrence::service()const {
    return data->app;
}


OPimRecurrence::ExceptionList& OPimRecurrence::exceptions() {
    return data->list;
}


void OPimRecurrence::setType( const RepeatType& z) {
    checkOrModify();
    data->type = z;
}


void OPimRecurrence::setFrequency( int freq ) {
    checkOrModify();
    data->freq = freq;
}


void OPimRecurrence::setPosition( int pos ) {
    checkOrModify();
    data->pos = pos;
}


void OPimRecurrence::setDays( char c ) {
    checkOrModify();
    data->days = c;
}


void OPimRecurrence::setEndDate( const QDate& dt) {
    checkOrModify();
    data->end = dt;
}


void OPimRecurrence::setCreatedDateTime( const QDateTime& t) {
    checkOrModify();
    data->create = t;
}


void OPimRecurrence::setHasEndDate( bool b) {
    checkOrModify();
    data->hasEnd = b;
}


void OPimRecurrence::setRepitition( int rep ) {
    checkOrModify();
    data->rep = rep;
}


void OPimRecurrence::setService( const QString& app ) {
    checkOrModify();
    data->app = app;
}


void OPimRecurrence::setStart( const QDate& dt ) {
    checkOrModify();
    data->start = dt;
}


void OPimRecurrence::checkOrModify() {
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


QString OPimRecurrence::toString()const {
    QString buf;
    QMap<int, QString> recMap = toMap();

    buf += " rtype=\"";
    buf += recMap[OPimRecurrence::RType];
    buf += "\"";
    if (data->days > 0 )
        buf += " rweekdays=\"" + recMap[OPimRecurrence::RWeekdays] + "\"";
    if ( data->pos != 0 )
        buf += " rposition=\"" + recMap[OPimRecurrence::RPosition] + "\"";

    buf += " rfreq=\"" + recMap[OPimRecurrence::RFreq] + "\"";
    buf += " rhasenddate=\"" + recMap[OPimRecurrence::RHasEndDate]+ "\"";
    if ( data->hasEnd )
        buf += " enddt=\""
            + recMap[OPimRecurrence::EndDate]
            + "\"";
    buf += " created=\"" + recMap[OPimRecurrence::Created] + "\"";

    if ( data->list.isEmpty() ) return buf;
    buf += " exceptions=\"";
    buf += recMap[OPimRecurrence::Exceptions];
    buf += "\" ";

    return buf;
}

QString OPimRecurrence::rTypeString() const
{
    QString retString;
    switch ( data->type ) {
    case OPimRecurrence::Daily:
            retString = "Daily";
            break;
    case OPimRecurrence::Weekly:
            retString = "Weekly";
            break;
    case OPimRecurrence::MonthlyDay:
            retString = "MonthlyDay";
            break;
    case OPimRecurrence::MonthlyDate:
            retString = "MonthlyDate";
            break;
    case OPimRecurrence::Yearly:
            retString = "Yearly";
            break;
    default:
            retString = "NoRepeat";
            break;

    }

    return retString;
}

QMap<QString, OPimRecurrence::RepeatType> OPimRecurrence::rTypeValueConvertMap() const
{
        QMap<QString, RepeatType> convertMap;

        convertMap.insert( QString( "Daily" ), OPimRecurrence::Daily );
        convertMap.insert( QString( "Weekly" ), OPimRecurrence::Weekly );
        convertMap.insert( QString( "MonthlyDay" ), OPimRecurrence::MonthlyDay );
        convertMap.insert( QString( "MonthlyDate" ), OPimRecurrence::MonthlyDate );
        convertMap.insert( QString( "Yearly" ), OPimRecurrence::Yearly );
        convertMap.insert( QString( "NoRepeat" ), OPimRecurrence::NoRepeat );

        return convertMap;
}


QMap<int, QString> OPimRecurrence::toMap() const
{
    QMap<int, QString> retMap;
    
    retMap.insert( OPimRecurrence::RType, rTypeString() );
    retMap.insert( OPimRecurrence::RWeekdays, QString::number( static_cast<int>( data->days ) ) );
    retMap.insert( OPimRecurrence::RPosition, QString::number(data->pos ) );
    retMap.insert( OPimRecurrence::RFreq, QString::number( data->freq ) );
    retMap.insert( OPimRecurrence::RHasEndDate, QString::number( static_cast<int>( data->hasEnd ) ) );
    if( data -> hasEnd )
            retMap.insert( OPimRecurrence::EndDate, QString::number( OPimTimeZone::utc().fromUTCDateTime( QDateTime( data->end, QTime(12,0,0) ) ) ) );
    retMap.insert( OPimRecurrence::Created, QString::number( OPimTimeZone::utc().fromUTCDateTime( data->create ) ) );
    
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

    retMap.insert( OPimRecurrence::Exceptions, exceptBuf );

    return retMap;
}

void OPimRecurrence::fromMap( const QMap<int, QString>& map )
{
    QMap<QString, RepeatType> repTypeMap = rTypeValueConvertMap(); 

    data -> type  = repTypeMap[ map [OPimRecurrence::RType] ];
    data -> days  = (char) map[ OPimRecurrence::RWeekdays ].toInt();
    data -> pos   = map[ OPimRecurrence::RPosition ].toInt();
    data -> freq = map[ OPimRecurrence::RFreq ].toInt();
    data -> hasEnd= map[ OPimRecurrence::RHasEndDate ].toInt() ? true : false;
    OPimTimeZone utc = OPimTimeZone::utc();
    if ( data -> hasEnd ){
            data -> end = utc.fromUTCDateTime( (time_t) map[ OPimRecurrence::EndDate ].toLong() ).date();
    }
    data -> create = utc.fromUTCDateTime( (time_t) map[ OPimRecurrence::Created ].toLong() ).date();

#if 0
    // FIXME: Exceptions currently not supported...
    // Convert the list of exceptions from QString into ExceptionList
    data -> list.clear();
    QString exceptStr = map[ OPimRecurrence::Exceptions ];
    QStringList exceptList = QStringList::split( " ", exceptStr );
    ...
#endif
        
        
}

}
