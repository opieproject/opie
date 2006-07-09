/**********************************************************************
** Copyright (C) 2000-2006 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This program is free software; you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by the
** Free Software Foundation; either version 2 of the License, or (at your
** option) any later version.
**
** A copy of the GNU GPL license version 2 is included in this package as
** LICENSE.GPL.
**
** This program is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** In addition, as a special exception Trolltech gives permission to link
** the code of this program with Qtopia applications copyrighted, developed
** and distributed by Trolltech under the terms of the Qtopia Personal Use
** License Agreement. You must comply with the GNU General Public License
** in all respects for all of the code used other than the applications
** licensed under the Qtopia Personal Use License Agreement. If you modify
** this file, you may extend this exception to your version of the file,
** but you are not obligated to do so. If you do not wish to do so, delete
** this exception statement from your version.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "timestring.h"
#include <qobject.h>
#include <qdatetime.h>
#include <qapplication.h>
#include "config.h"

#include <time.h>


class TimeStringFormat : public QObject
{
    Q_OBJECT
public:
    static DateFormat currentFormat()
    {
	if ( !self )
	    self  = new TimeStringFormat;
	return self->format;
    }
private slots:
    void formatChanged( DateFormat f )
    {
	format = f;
    }
private:
    static TimeStringFormat *self;
    DateFormat format;

    TimeStringFormat()
	: QObject( qApp )
    {
	Config config("qpe");
	config.setGroup( "Date" );
	format = ::DateFormat(QChar(config.readEntry("Separator", "/")[0]),
		(::DateFormat::Order)config.readNumEntry("ShortOrder", ::DateFormat::DayMonthYear),
		(::DateFormat::Order)config.readNumEntry("LongOrder", ::DateFormat::DayMonthYear));

	connect( qApp, SIGNAL( dateFormatChanged(DateFormat) ),
		 this, SLOT( formatChanged(DateFormat) ) );
    }
};

TimeStringFormat *TimeStringFormat::self = 0;

QString DateFormat::toNumberString() const
{
    QString buf = "";
    // for each part of the order
    for (int i = 0; i < 3; i++) {
	// switch on the relavent 3 bits.
	switch((_shortOrder >> (i * 3)) & 0x0007) {
	    case 0x0001:
		buf += TimeStringFormat::tr( "D", "first letter of the word 'Day'" );
		break;
	    case 0x0002:
		buf += TimeStringFormat::tr( "M" , "first letter of the word 'Month'" );
		break;
	    case 0x0004:
		buf += TimeStringFormat::tr( "Y" , "first letter of the word 'Year'" );
		break;
	}
	if (i < 2)
	    buf += _shortSeparator;
    }
    return buf;
}

QString DateFormat::toWordString() const
{
    QString buf = "";
    // for each part of the order
    for (int i = 0; i < 3; i++) {
	// switch on the relavent 3 bits.
	switch((_longOrder >> (i * 3)) & 0x0007) {
	    case 0x0001:
		buf += TimeStringFormat::tr( "day", "in month" );
		if (i < 2) {
		    if ((_shortOrder << ((i+1) * 3)) & 0x0007)
			buf += ", ";
		    else
			buf += " ";
		}
		break;
	    case 0x0002:
		buf += TimeStringFormat::tr( "month" );
		if (i < 2)
		    buf += " ";
		break;
	    case 0x0004:
		buf += TimeStringFormat::tr( "year" );
		if (i < 2)
		    buf += ", ";
		break;
	}
    }
    return buf;
}

QString DateFormat::numberDate(const QDate &d, int v) const
{
    QString buf = "";

    int pad = 2;

    // for each part of the order
    for (int i = 0; i < 3; i++) {
	// switch on the relavent 3 bits.
	switch((_shortOrder >> (i * 3)) & 0x0007) {
	    case 0x0001:
	      if (pad==2) buf += QString().sprintf("%02d",d.day());
	      else buf += QString().sprintf("%d",d.day());
		break;
	    case 0x0002:
	      if (i==0) { // no padding with only MM/DD/YY format
		pad=0;
	      }
	      if (pad==2) buf += QString().sprintf("%02d",d.month());
	      else buf += QString().sprintf("%d",d.month());
		break;
	    case 0x0004:
		{
		    int year = d.year();
		    if (!(v & longNumber))
			year = year % 100;
		    buf += QString().sprintf("%02d",year);
		}
		break;
	}
	if (i < 2)
	    buf += _shortSeparator;
    }
    return buf;
}

static const char* unTranslatedFullMonthNames[] = {
    QT_TRANSLATE_NOOP( "QDate", "January" ),
    QT_TRANSLATE_NOOP( "QDate", "February" ),
    QT_TRANSLATE_NOOP( "QDate", "March" ),
    QT_TRANSLATE_NOOP( "QDate", "April" ),
    QT_TRANSLATE_NOOP( "QDate", "May" ),
    QT_TRANSLATE_NOOP( "QDate", "June" ),
    QT_TRANSLATE_NOOP( "QDate", "July" ),
    QT_TRANSLATE_NOOP( "QDate", "August" ),
    QT_TRANSLATE_NOOP( "QDate", "September" ),
    QT_TRANSLATE_NOOP( "QDate", "October" ),
    QT_TRANSLATE_NOOP( "QDate", "November" ),
    QT_TRANSLATE_NOOP( "QDate", "December" )
};

static const char* unTranslatedFullDayNames[] = {
    QT_TRANSLATE_NOOP( "QDate", "Monday" ),
    QT_TRANSLATE_NOOP( "QDate", "Tuesday" ),
    QT_TRANSLATE_NOOP( "QDate", "Wednesday" ),
    QT_TRANSLATE_NOOP( "QDate", "Thursday" ),
    QT_TRANSLATE_NOOP( "QDate", "Friday" ),
    QT_TRANSLATE_NOOP( "QDate", "Saturday" ),
    QT_TRANSLATE_NOOP( "QDate", "Sunday" )
};

#ifdef QTOPIA_DESKTOP
//translations in qt.qm
static const char* unTranslatedMediumDayNames[] = {
    "Mon" , "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};

static const char* unTranslatedMediumMonthNames[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
#endif

static QString dayname(const QDate& d, bool lng)
{
    if (lng && qApp)
        return qApp->translate("QDate", unTranslatedFullDayNames[d.dayOfWeek()-1]);
    else {
#ifdef QTOPIA_DESKTOP
        if (qApp)
            return qApp->translate("QDate", unTranslatedMediumDayNames[ d.dayOfWeek()-1]);
#endif
        return d.dayName(d.dayOfWeek());
    }
}

QString DateFormat::wordDate(const QDate &d, int v) const
{
    // for each part of the order
    QString weekDay;
    if (v & showWeekDay)
	weekDay = ::dayname(d,(v & longWord));

    QString date="";
    QString sep="";
    for (int i = 0; i < 3; i++) {
	// switch on the relavent 3 bits.
	int field = (_longOrder >> (i * 3)) & 0x0007;
        if ( field && !date.isEmpty() )
	    date += sep;
	switch (field) {
	    case 0x0001: // Day
                {
                    QString daysuffix = TimeStringFormat::tr("@day", "day suffix - applies to some asian languages (e.g. Japanese and Trad. Chinese). If it doesn't apply to your language it has to be translated to an '@day' " );
                    if (i==1) {
                        date += QString().sprintf("%02d",d.day());
                        if (daysuffix != "@day")
                            date+=daysuffix;
                        sep = TimeStringFormat::tr(",","day-date separator") + " ";
                    } else {
                        date += QString().sprintf("%2d",d.day());
                        if (daysuffix == "@day") {
                            if (separator()=='.') // 2002/1/11
                                sep = ". ";
                            else
                                sep = " ";
                        } else {
                            date += daysuffix+" ";
                            sep = " ";
                        }
                    }
                }
		break;
	    case 0x0002: // Month
		{
		    QString monthName;

                    if (v & longWord)
                        monthName = qApp->translate("QDate", unTranslatedFullMonthNames[d.month()-1] );
                    else {
#ifdef QTOPIA_DESKTOP
                        monthName = qApp->translate("QDate", unTranslatedMediumMonthNames[d.month()-1] );
#else
                        monthName = d.monthName( d.month() );
#endif
                    }
		    date += monthName;
		}
		sep = " ";//TimeStringFormat::tr(" ","month-date separator");
		break;
	    case 0x0004: // Year
		{
		    int year = d.year();
		    if (!(v & longNumber))
			year = year % 100;

		    if (year < 10)
			date += "0";

		    date += QString::number(year);
                    QString yearsuffix = TimeStringFormat::tr("@year", "year suffix - applies to some asian languages (e.g. Japanese and Trad. Chinese). If it doesn't apply to your language it has to be translated to an '@year' " );
                    if (yearsuffix != "@year")
                        date += yearsuffix;

		}
		sep = TimeStringFormat::tr(",","year-date seperator") + " ";
		break;
	}
    }

    QString r = "";
    if ( weekDay.isEmpty() )
	r = date;
    else if ((_longOrder & 0x0007) == 0x0002)
	r = TimeStringFormat::tr("%1 %2","1=Monday 2=January 12").arg(weekDay).arg(date);
    else if ( _longOrder )
	r = TimeStringFormat::tr("%1, %2","1=Monday 2=12 January").arg(weekDay).arg(date);
    else
	r = weekDay;
    return r;
}

#ifndef QT_NO_DATASTREAM
void DateFormat::save(QDataStream &d) const
{
    d << _shortSeparator.unicode();
    uint v= _shortOrder;
    d << v;
    v = _longOrder;
    d << v;
}

void DateFormat::load(QDataStream &d)
{
    ushort value;
    d >> value;
    _shortSeparator = QChar(value);
    uint v = 0;
    d >> v;
    _shortOrder = (Order)v;
    v = 0;
    d >> v;
    _longOrder = (Order)v;
}

QDataStream &operator<<(QDataStream &s, const DateFormat&df)
{
        df.save(s);
	    return s;
}
QDataStream &operator>>(QDataStream &s, DateFormat&df)
{
        df.load(s);
	    return s;
}
#endif

QString TimeString::shortDate( const QDate &d, DateFormat dtf )
{
    return dtf.wordDate(d);
}

QString TimeString::dateString( const QDate &d, DateFormat dtf )
{
    return dtf.wordDate(d, DateFormat::longNumber);
}


QString TimeString::longDateString( const QDate &d, DateFormat dtf )
{
    return dtf.wordDate(d, DateFormat::showWeekDay | DateFormat::longNumber
	    | DateFormat::longWord);
}

DateFormat TimeString::currentDateFormat()
{
    return TimeStringFormat::currentFormat();
}


QString TimeString::dateString( const QDateTime &dt, bool ampm, bool seconds, DateFormat dtf )
{
    const QDate& d = dt.date();
    const QTime& t = dt.time();

    // based on QDateTime::toString()
    QString buf = timeString(t,ampm,seconds);
    buf += " ";
    buf += longDateString( d, dtf );

    return buf;
}

QString TimeString::timeString( const QTime &t, bool ampm, bool seconds )
{
    if ( !ampm ) {
	if ( seconds )
	    return t.toString();
	QString r = QString::number(t.hour());
	if ( t.hour() < 10 ) r.prepend( "0" );
	r.append( ":" );
	if ( t.minute() < 10 ) r.append( "0" );
	r.append(QString::number(t.minute()));
	return r;
    }
    // ### else the hard case that should disappear in Qt 3.0
    QString argString = seconds ? "%4:%5:%6 %7" : "%4:%5 %7";
    int hour = t.hour();
    QString strMin = QString::number( t.minute() );
    QString strSec = QString::number( t.second() );
    if ( hour > 12 )
	argString = argString.arg( hour - 12, 2 );
    else {
        if ( hour == 0 )
	    argString = argString.arg( 12 );
        else
	    argString = argString.arg( hour, 2 );
    }
    if ( t.minute() < 10 )
	strMin.prepend( "0" );
    if ( t.second() < 10 )
	strSec.prepend( "0" );
    argString = argString.arg( strMin );
    if ( seconds )
	argString = argString.arg( strSec );
    if ( hour >= 12 )
	argString = argString.arg( TimeStringFormat::tr("PM") );
    else
	argString = argString.arg( TimeStringFormat::tr("AM") );
    return argString;
}

QString TimeString::shortTime( bool ampm, bool seconds )
{
    // just create a shorter time String
    QDateTime dtTmp = QDateTime::currentDateTime();
    QString strTime = TimeStringFormat::tr( "%1 %2", "1=Monday 2=12:45" )
	.arg(::dayname(dtTmp.date(),FALSE))
	.arg(timeString( dtTmp.time(), ampm, seconds ));
    return strTime;
}

QString TimeString::dateString( const QDateTime &t, bool ampm )
{
    return dateString(t,ampm,FALSE);
}

QString TimeString::timeString( const QTime &t, bool ampm)
{
    return timeString(t,ampm,FALSE);
}

QString TimeString::shortTime( bool ampm )
{
    return shortTime(ampm,FALSE);
}

QString TimeString::numberDateString( const QDate &d, DateFormat dtf )
{
  return dtf.numberDate(d);
}
QString TimeString::longNumberDateString( const QDate &d, DateFormat dtf )
{
  return dtf.numberDate(d,DateFormat::longNumber);
}
/*!
    Returns date/time \a dt as a string,
      showing year, month, date, hours, minutes, and seconds.
        \a len determines the length of the resulting string.
        
          The format, including order depends on the user's settings.
          
            First availability: Qtopia 1.6
*/
//QString TimeString::localYMDHMS( const QDateTime &dt, Length len )
//{
//    const QDate& d = dt.date();
//    const QTime& t = dt.time();
//    return LocalTimeFormat::tr("%1 %2","date,time").arg(localYMD(d,len)).arg(localHMS(t));
//}


#include "timestring.moc"
