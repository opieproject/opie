/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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


#ifndef QTPALMTOP_stringutil_h__
#define QTPALMTOP_stringutil_h__

#include <qstring.h>
#include "qpcglobal.h"

namespace Qtopia
{

// Simplifies white space within each line but keeps the new line characters
QString QPC_EXPORT simplifyMultiLineSpace( const QString &multiLine );

// Creates a QString which doesn't contain any "dangerous"
// characters (i.e. <, >, &, ")
QString QPC_EXPORT escapeString( const QString& plain );

// Takes a UTF-8!! string and removes all the XML thingies (entities?)
// from the string and also calls fromUtf8() on it... so make sure
// to pass a QCString/const char* with UTF-8 data only
QString QPC_EXPORT plainString( const char* escaped, unsigned int length );
QString QPC_EXPORT plainString( const QCString& string );

QString QPC_EXPORT plainString( const QString& string );


// collation functions
int compare( const QString & s1, const QString & s2 );
QString buildSortKey( const QString & s );
QString buildSortKey( const QString & s1, const QString & s2 );
QString buildSortKey( const QString & s1, const QString & s2, 
		      const QString & s3 );

}

#endif
