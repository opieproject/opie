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
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef _TIMESTRING_H_
#define _TIMESTRING_H_
#include <qdatetime.h>
#include <qstring.h>
#include <qarray.h>

#if (QT_VERSION-0 >= 0x030000)
#define DateFormat QPEDateFormat
#endif

#include <qtopia/qpeglobal.h>

class QObject;

// return a string with the time based on whether or not you want
// you want it in 12 hour form.   if ampm is true, then return
// it in 12 hour (am/pm) form otherwise return it in 24 hour form
// in theory Qt 3,0 handles this better (hopefully obsoleteing this)
class DateFormat
{
public:
    // date format type 1,2,4 = day,month,year
    enum Order {
	DayMonthYear = 0421, // right-to-left
	MonthDayYear = 0412,
	YearMonthDay = 0124
    };

    DateFormat(QChar s = '/', Order so = MonthDayYear) : _shortOrder(so),
        _longOrder(so), _shortSeparator(s) { }
    DateFormat(QChar s, Order so, Order lo) :  _shortOrder(so),
        _longOrder(lo), _shortSeparator(s) { }
    DateFormat(const DateFormat &o) : _shortOrder(o._shortOrder),
        _longOrder(o._longOrder), _shortSeparator(o._shortSeparator) { }

    bool operator==(const DateFormat &o)
    {
	if (o._shortOrder == _shortOrder && o._longOrder == _longOrder &&
		o._shortSeparator == _shortSeparator)
	    return TRUE;
	return FALSE;
    }

    // verbosity specifiers
    enum Verbosity {
	shortNumber = 0x01, // default
	longNumber  = 0x02,

	padNumber = 0x04,

	shortWord = 0x08, // default
	longWord = 0x10,

	showWeekDay = 0x20
    };

    QString toNumberString() const; // the M/D/Y string.
    QString toWordString() const; // the Month day, year string.

    QString numberDate(const QDate &d, int v = 0) const;
    QString wordDate(const QDate &d, int v = 0) const;

#ifndef QT_NO_DATASTREAM
    void load(QDataStream&);
    void save(QDataStream&) const;
#endif

    QChar separator() const { return _shortSeparator; };
    Order shortOrder() const { return _shortOrder; };
    Order longOrder() const { return _longOrder; };

private:
    Order _shortOrder;
    Order _longOrder;
    QChar _shortSeparator;
};

#ifndef QT_NO_DATASTREAM
QDataStream &operator<<(QDataStream &s, const DateFormat&df);
QDataStream &operator>>(QDataStream &s, DateFormat&df);
#endif

class TimeString
{
public:

    //enum DateFormat { MonthDayYear, DayMonthYear, ISO8601,
		      //YearMonthDay = ISO8601 };


//private:
    static QString shortDate( const QDate &d )
    { return shortDate( d, currentDateFormat() ); }
    static QString dateString( const QDate &d )
    { return dateString( d, currentDateFormat() ); }
    static QString longDateString( const QDate &d )
    { return longDateString( d, currentDateFormat() ); }
    static QString dateString( const QDateTime &dt, bool ampm, bool seconds )
    { return dateString( dt, ampm, seconds, currentDateFormat() ); }

public:
    enum Length { Short, Medium, Long };
    static QString localH( int hour );
    static QString localHM( const QTime & );
    static QString localHM( const QTime &, Length ); // qtopia 2.1.0
    static QString localHMS( const QTime & );
    static QString localHMDayOfWeek( const QDateTime &t );
    static QString localHMSDayOfWeek( const QDateTime &t );
    static QString localMD( const QDate &, Length=Medium );
    static QString localYMD( const QDate &, Length=Medium );
    static QString localYMDHMS( const QDateTime &, Length=Medium );
    static QString localDayOfWeek( const QDate&, Length=Medium );
    static QString localDayOfWeek( int day1to7, Length=Medium );

    static QString hourString( int hour, bool ampm );
    static bool currentAMPM();
    static DateFormat currentDateFormat();
    static QArray<DateFormat> formatOptions(); // qtopia 1.6.0

    static void connectChange(QObject*,const char* member);
    static void disconnectChange(QObject*,const char* member);

    // Not recommended to call these (they don't honor system ampm)
    static QString dateString( const QDateTime &t, bool ampm = false);
    static QString timeString( const QTime &t, bool ampm, bool seconds );
    static QString timeString( const QTime &t, bool ampm = false);
    static QString shortTime( bool ampm, bool seconds );
    static QString shortTime( bool ampm = false);

    static QString numberDateString( const QDate &d, DateFormat );
    static QString numberDateString( const QDate &d )
    { return numberDateString( d, currentDateFormat() ); }
    static QString longNumberDateString( const QDate &d, DateFormat );
    static QString longNumberDateString( const QDate &d )
    { return longNumberDateString( d, currentDateFormat() ); }

    static QString shortDate( const QDate &, DateFormat );
    static QString dateString( const QDate &, DateFormat  );
    static QString longDateString( const QDate &, DateFormat );

private:
    static QString dateString( const QDateTime &t, bool ampm, bool seconds, DateFormat );


};

#endif
