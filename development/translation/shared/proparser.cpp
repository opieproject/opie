/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Linguist.
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

#include "proparser.h"

#include <qregexp.h>
#include <qstringlist.h>

QMap<QString, QString> proFileTagMap( const QString& text )
{
    QString t = text;

    /*
      Strip comments, merge lines ending with backslash, add
      spaces around '=' and '+=', replace '\n' with ';', and
      simplify white spaces.
    */
    t.replace( QRegExp(QString("#[^\n]$")), QString(" ") );
    t.replace( QRegExp(QString("\\\\\\s*\n")), QString(" ") );
    t.replace( "=", QString(" = ") );
    t.replace( "+ =", QString(" += ") );
    t.replace( "\n", QString(";") );
    t = t.simplifyWhiteSpace();

    QMap<QString, QString> tagMap;

    QStringList lines = QStringList::split( QChar(';'), t );
    QStringList::Iterator line;
    for ( line = lines.begin(); line != lines.end(); ++line ) {
	QStringList toks = QStringList::split( QChar(' '), *line );

	if ( toks.count() >= 3 && 
	     (toks[1] == QString("=") || toks[1] == QString("+=")) ) {
	    QString tag = toks.first();
	    int k = tag.findRev( QChar(':') ); // as in 'unix:'
	    if ( k != -1 )
		tag = tag.mid( k + 1 );
	    toks.remove( toks.begin() );

	    QString action = toks.first();
	    toks.remove( toks.begin() );

	    if ( tagMap.contains(tag) ) {
		if ( action == QString("=") )
		    tagMap.replace( tag, toks.join(QChar(' ')) );
		else
		    tagMap[tag] += QChar( ' ' ) + toks.join( QChar(' ') );
	    } else {
		tagMap[tag] = toks.join( QChar(' ') );
	    }
	}
    }

    QRegExp var( "\\$\\$[a-zA-Z0-9_]+" );
    QMap<QString, QString>::Iterator it;
    for ( it = tagMap.begin(); it != tagMap.end(); ++it ) {
	int i = 0;

	while ( (i = var.search(it.data(), i)) != -1 ) {
	    int len = var.matchedLength();
	    QString invocation = (*it).mid( i + 2, len - 2 );
	    QString after;
	    if ( tagMap.contains(invocation) )
		after = tagMap[invocation];
	    (*it).replace( i, len, after );
	    i += after.length();
	}
    }
    return tagMap;
}
