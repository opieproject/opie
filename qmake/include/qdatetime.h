/*************************************************************************
** $Id: qdatetime.h,v 1.1 2002-11-01 00:10:43 kergoth Exp $
**
** Definition of date and time classes
**
** Created : 940124
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the tools module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QDATETIME_H
#define QDATETIME_H

#ifndef QT_H
#include "qstring.h"
#include "qnamespace.h"
#endif // QT_H


/*****************************************************************************
  QDate class
 *****************************************************************************/

class Q_EXPORT QDate
{
public:
    QDate() { jd = 0; }
    QDate( int y, int m, int d );

    bool isNull() const { return jd == 0; }
    bool isValid() const;

    int year() const;
    int month() const;
    int day() const;
    int dayOfWeek() const;
    int dayOfYear() const;
    int daysInMonth() const;
    int daysInYear() const;
    int weekNumber( int *yearNum = 0 ) const;

#ifndef QT_NO_TEXTDATE
#ifndef QT_NO_COMPAT
    static QString monthName( int month ) { return shortMonthName( month ); }
    static QString dayName( int weekday ) { return shortDayName( weekday ); }
#endif
    static QString shortMonthName( int month );
    static QString shortDayName( int weekday );
    static QString longMonthName( int month );
    static QString longDayName( int weekday );
#endif //QT_NO_TEXTDATE
#ifndef QT_NO_TEXTSTRING
#if !defined(QT_NO_SPRINTF)
    QString toString( Qt::DateFormat f = Qt::TextDate ) const;
#endif
    QString toString( const QString& format ) const;
#endif
    bool   setYMD( int y, int m, int d );

    QDate  addDays( int days )		const;
    QDate  addMonths( int months )      const;
    QDate  addYears( int years )        const;
    int	   daysTo( const QDate & )	const;

    bool   operator==( const QDate &d ) const { return jd == d.jd; }
    bool   operator!=( const QDate &d ) const { return jd != d.jd; }
    bool   operator<( const QDate &d )	const { return jd < d.jd; }
    bool   operator<=( const QDate &d ) const { return jd <= d.jd; }
    bool   operator>( const QDate &d )	const { return jd > d.jd; }
    bool   operator>=( const QDate &d ) const { return jd >= d.jd; }

    static QDate currentDate();
    static QDate currentDate( Qt::TimeSpec );
#ifndef QT_NO_DATESTRING
    static QDate fromString( const QString& s, Qt::DateFormat f = Qt::TextDate );
#endif
    static bool	 isValid( int y, int m, int d );
    static bool	 leapYear( int year );

    static uint	 gregorianToJulian( int y, int m, int d );
    static void	 julianToGregorian( uint jd, int &y, int &m, int &d );
private:
    uint	 jd;
    friend class QDateTime;
#ifndef QT_NO_DATASTREAM
    friend Q_EXPORT QDataStream &operator<<( QDataStream &, const QDate & );
    friend Q_EXPORT QDataStream &operator>>( QDataStream &, QDate & );
#endif
};


/*****************************************************************************
  QTime class
 *****************************************************************************/

class Q_EXPORT QTime
{
public:
    QTime() { ds=0; }				// set null time
    QTime( int h, int m, int s=0, int ms=0 );	// set time

    bool   isNull()	 const { return ds == 0; }
    bool   isValid()	 const;			// valid time

    int	   hour()	 const;			// 0..23
    int	   minute()	 const;			// 0..59
    int	   second()	 const;			// 0..59
    int	   msec()	 const;			// 0..999
#ifndef QT_NO_DATESTRING
#ifndef QT_NO_SPRINTF
    QString toString( Qt::DateFormat f = Qt::TextDate )	 const;
#endif
    QString toString( const QString& format ) const;
#endif
    bool   setHMS( int h, int m, int s, int ms=0 );

    QTime  addSecs( int secs )		const;
    int	   secsTo( const QTime & )	const;
    QTime  addMSecs( int ms )		const;
    int	   msecsTo( const QTime & )	const;

    bool   operator==( const QTime &d ) const { return ds == d.ds; }
    bool   operator!=( const QTime &d ) const { return ds != d.ds; }
    bool   operator<( const QTime &d )	const { return ds < d.ds; }
    bool   operator<=( const QTime &d ) const { return ds <= d.ds; }
    bool   operator>( const QTime &d )	const { return ds > d.ds; }
    bool   operator>=( const QTime &d ) const { return ds >= d.ds; }

    static QTime currentTime();
    static QTime currentTime( Qt::TimeSpec );
#ifndef QT_NO_DATESTRING
    static QTime fromString( const QString& s, Qt::DateFormat f = Qt::TextDate );
#endif
    static bool	 isValid( int h, int m, int s, int ms=0 );

    void   start();
    int	   restart();
    int	   elapsed() const;

private:
    static bool currentTime( QTime * );
    static bool currentTime( QTime *, Qt::TimeSpec );

    uint   ds;
    friend class QDateTime;
#ifndef QT_NO_DATASTREAM
    friend Q_EXPORT QDataStream &operator<<( QDataStream &, const QTime & );
    friend Q_EXPORT QDataStream &operator>>( QDataStream &, QTime & );
#endif
};


/*****************************************************************************
  QDateTime class
 *****************************************************************************/

class Q_EXPORT QDateTime
{
public:
    QDateTime() {}				// set null date and null time
    QDateTime( const QDate & );
    QDateTime( const QDate &, const QTime & );

    bool   isNull()	const		{ return d.isNull() && t.isNull(); }
    bool   isValid()	const		{ return d.isValid() && t.isValid(); }

    QDate  date()	const		{ return d; }
    QTime  time()	const		{ return t; }
    uint   toTime_t()	const;
    void   setDate( const QDate &date ) { d = date; }
    void   setTime( const QTime &time ) { t = time; }
    void   setTime_t( uint secsSince1Jan1970UTC );
    void   setTime_t( uint secsSince1Jan1970UTC, Qt::TimeSpec );
#ifndef QT_NO_DATESTRING
#ifndef QT_NO_SPRINTF
    QString toString( Qt::DateFormat f = Qt::TextDate )	const;
#endif
    QString toString( const QString& format ) const;
#endif
    QDateTime addDays( int days )	const;
    QDateTime addMonths( int months )   const;
    QDateTime addYears( int years )     const;
    QDateTime addSecs( int secs )	const;
    int	   daysTo( const QDateTime & )	const;
    int	   secsTo( const QDateTime & )	const;

    bool   operator==( const QDateTime &dt ) const;
    bool   operator!=( const QDateTime &dt ) const;
    bool   operator<( const QDateTime &dt )  const;
    bool   operator<=( const QDateTime &dt ) const;
    bool   operator>( const QDateTime &dt )  const;
    bool   operator>=( const QDateTime &dt ) const;

    static QDateTime currentDateTime();
    static QDateTime currentDateTime( Qt::TimeSpec );
#ifndef QT_NO_DATESTRING
    static QDateTime fromString( const QString& s, Qt::DateFormat f = Qt::TextDate );
#endif
private:
    QDate  d;
    QTime  t;
#ifndef QT_NO_DATASTREAM
    friend Q_EXPORT QDataStream &operator<<( QDataStream &, const QDateTime &);
    friend Q_EXPORT QDataStream &operator>>( QDataStream &, QDateTime & );
#endif
};


/*****************************************************************************
  Date and time stream functions
 *****************************************************************************/

#ifndef QT_NO_DATASTREAM
Q_EXPORT QDataStream &operator<<( QDataStream &, const QDate & );
Q_EXPORT QDataStream &operator>>( QDataStream &, QDate & );
Q_EXPORT QDataStream &operator<<( QDataStream &, const QTime & );
Q_EXPORT QDataStream &operator>>( QDataStream &, QTime & );
Q_EXPORT QDataStream &operator<<( QDataStream &, const QDateTime & );
Q_EXPORT QDataStream &operator>>( QDataStream &, QDateTime & );
#endif // QT_NO_DATASTREAM

#endif // QDATETIME_H

