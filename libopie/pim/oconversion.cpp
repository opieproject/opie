/**********************************************************************
** Copyright (C) 2003 by Stefan Eilers (eilers.stefan@epost.de)
**
** This file may be distributed and/or modified under the terms of the
** GNU Lesser General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#include "oconversion.h"
#include <qpe/timeconversion.h>


QString OConversion::dateToString( const QDate &d )
{
	if ( d.isNull() || !d.isValid() )
		return QString::null;

	// ISO format in year, month, day (YYYYMMDD); e.g. 20021231
	QString year = QString::number( d.year() );
	QString month = QString::number( d.month() );
	month = month.rightJustify( 2, '0' );
	QString day = QString::number( d.day() );
	day = day.rightJustify( 2, '0' );

	QString str = year + month + day;
	//qDebug( "\tPimContact dateToStr = %s", str.latin1() );

	return str;
}

QDate OConversion::dateFromString( const QString& s )
{
	QDate date;

	if ( s.isEmpty() )
		return date;

	// Be backward compatible to old Opie format:
	// Try to load old format. If it fails, try new ISO-Format!
	date = TimeConversion::fromString ( s );
	if ( date.isValid() )
		return date;

	// Read ISO-Format (YYYYMMDD)
	int year = s.mid(0, 4).toInt();
	int month = s.mid(4,2).toInt();
	int day = s.mid(6,2).toInt();

	// do some quick sanity checking -eilers
        // but we isValid() again? -zecke
	if ( year < 1900 || year > 3000 ) {
		qWarning( "PimContact year is not in range");
		return date;
	}
	if ( month < 0 || month > 12 ) {
		qWarning( "PimContact month is not in range");
		return date;
	}
	if ( day < 0 || day > 31 ) {
		qWarning( "PimContact day is not in range");
		return date;
	}

	date.setYMD( year, month, day );
	if ( !date.isValid() ) {
		qWarning( "PimContact date is not valid");
		return date;
	}

	return date;
}
QString OConversion::dateTimeToString( const QDateTime& dt ) {
    if (!dt.isValid() || dt.isNull() ) return QString::null;

    QString year  = QString::number( dt.date().year()   );
    QString month = QString::number( dt.date().month()  );
    QString day   = QString::number( dt.date().day()    );

    QString hour  = QString::number( dt.time().hour()   );
    QString min   = QString::number( dt.time().minute() );
    QString sec   = QString::number( dt.time().second() );

    month = month.rightJustify( 2, '0' );
    day   = day.  rightJustify( 2, '0' );
    hour  = hour. rightJustify( 2, '0' );
    min   = min.  rightJustify( 2, '0' );
    sec   = sec.  rightJustify( 2, '0' );

    QString str = day + month + year + hour + min + sec;

    return str;
}
QDateTime OConversion::dateTimeFromString( const QString& str) {

    if ( str.isEmpty() ) return QDateTime();
    int day   = str.mid(0, 2).toInt();
    int month = str.mid(2, 2).toInt();
    int year  = str.mid(4, 4).toInt();
    int hour  = str.mid(8, 2).toInt();
    int min   = str.mid(10, 2).toInt();
    int sec   = str.mid(12, 2).toInt();

    QDate date( year, month, day );
    QTime time( hour, min, sec );
    QDateTime dt( date, time );
    return dt;
}

