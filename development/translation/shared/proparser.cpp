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
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "proparser.h"

#include <qfile.h>
#include <qregexp.h>
#include <qstringlist.h>
#include <qtextstream.h>

#ifdef Q_OS_UNIX
#include <unistd.h>
#endif

#ifdef Q_OS_WIN32
#define QT_POPEN _popen
#else
#define QT_POPEN popen
#endif

QString loadFile( const QString &fileName )
{
    QFile file( fileName );
    if ( !file.open(IO_ReadOnly) ) {
	fprintf( stderr, "error: Cannot load '%s': %s\n",
		 file.name().latin1(),
		 file.errorString().latin1() );
	return QString();
    }

    QTextStream in( &file );
    return in.read();
}

QMap<QString, QString> proFileTagMap( const QString& text, const QString& opieDir )
{
    QString t = text;

    /*
      Process include() commands.
    */
    QRegExp callToInclude("include\\s*\\(\\s*([^()\\s]+)\\s*\\)");
    int i = 0;
    while ( (i = callToInclude.search(t, i)) != -1 ) {
	QString after = loadFile( callToInclude.cap(1) );
	t.replace( i, callToInclude.matchedLength(), after );
	i += after.length();
    }

    /*
      Strip comments, merge lines ending with backslash, add
      spaces around '=' and '+=', replace '\n' with ';', and
      simplify white spaces.
    */
    t.replace( QRegExp(QString("#[^\n]*\n")), QString(" ") );
    t.replace( QRegExp(QString("\\\\[^\n\\S]*\n")), QString(" ") );
    t.replace( "=", QString(" = ") );
    t.replace( "+ =", QString(" += ") );
    t.replace( "\n", QString(";") );
    t = t.simplifyWhiteSpace();

    /*
      Populate tagMap with 'key = value' entries.
    */
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

    /*
      Expand $$variables within the 'value' part of a 'key = value'
      pair.
    */
    QRegExp var( "\\$\\$[({]?([a-zA-Z0-9_]+)[)}]?" );
    QMap<QString, QString>::Iterator it;
    for ( it = tagMap.begin(); it != tagMap.end(); ++it ) {
	int i = 0;
	while ( (i = var.search((*it), i)) != -1 ) {
	    int len = var.matchedLength();
	    QString invocation = var.cap(1);
	    QString after;

	    if ( invocation == "system" ) {
		// skip system(); it will be handled in the next pass
		++i;
	    } else if ( invocation == "OPIEDIR") {
                (*it).replace( i, len, opieDir );
            }else {
		if ( tagMap.contains(invocation) )
		    after = tagMap[invocation];
		(*it).replace( i, len, after );
	    }
	}
    }

    /*
      Execute system() calls.
    */
    QRegExp callToSystem( "\\$\\$system\\s*\\(([^()]*)\\)" );
    for ( it = tagMap.begin(); it != tagMap.end(); ++it ) {
	int i = 0;
	while ( (i = callToSystem.search((*it), i)) != -1 ) {
	    /*
	      This code is stolen from qmake's project.cpp file.
	      Ideally we would use the same parser, so we wouldn't
	      have this code duplication.
	    */
	    QString after;
	    char buff[256];
	    FILE *proc = QT_POPEN( callToSystem.cap(1).latin1(), "r" );
	    while ( proc && !feof(proc) ) {
		int read_in = fread( buff, 1, 255, proc );
		if ( !read_in )
		    break;
		for ( int i = 0; i < read_in; i++ ) {
		    if ( buff[i] == '\n' || buff[i] == '\t' )
			buff[i] = ' ';
		}
		buff[read_in] = '\0';
		after += buff;
	    }
	    (*it).replace( i, callToSystem.matchedLength(), after );
	    i += after.length();
	}
    }

    return tagMap;
}
