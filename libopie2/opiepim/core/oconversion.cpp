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

#include <opie2/oconversion.h>
#include <qpe/timeconversion.h>


namespace Opie {

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

}