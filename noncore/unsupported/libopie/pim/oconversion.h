/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
** Copyright (C) 2002-2003 by Stefan Eilers (eilers.stefan@epost.de)
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
**********************************************************************/

#ifndef __oconversion_h__
#define __oconversion_h__

/* #include <time.h> */
/* #include <sys/types.h> */
#include <qdatetime.h>

/* FIXME namespace? -zecke */
class OConversion
{
public:
    static QString dateToString( const QDate &d );
    static QDate dateFromString( const QString &datestr );

    /**
     * simple function to store DateTime as string and read from string
     * no timezone changing is done
     * DDMMYYYYHHMMSS is the simple format
     */
    static QString dateTimeToString( const QDateTime& );
    static QDateTime dateTimeFromString( const QString& );
    
private:
    class Private;
    Private* d;    
    
};

#endif // __oconversion_h__

