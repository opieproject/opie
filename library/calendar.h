/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#ifndef CALENDAR_H
#define CALENDAR_H

#include <qstring.h>
#include <qvaluelist.h>

class Calendar
{
public:
    struct Day
    {
	enum Type { PrevMonth, ThisMonth, NextMonth };
	
	Day() : date( 0 ), type( ThisMonth ), holiday( FALSE ) {}
	Day( int d, Type t, bool h ) : date( d ), type( t ), holiday( h ) {}
	
	int date;
	Type type;
	bool holiday;
    };

    static QString nameOfMonth( int m );
    static QString nameOfDay( int d );
    static QValueList<Day> daysOfMonth( int year, int month, bool startWithMonday = FALSE );

};

#endif
