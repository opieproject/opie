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

#include <qfile.h>
#include <qtl.h>
#include <math.h>
#include <limits.h>
#include <errno.h>
#include <qdatastream.h>
#include "qimpencombining.h"
#include "qimpenchar.h"

#define QIMPEN_MATCH_THRESHOLD	    200000

const QIMPenSpecialKeys qimpen_specialKeys[] = {
    { Qt::Key_Escape,		"[Esc]" },
    { Qt::Key_Tab,		"[Tab]" },
    { Qt::Key_Backspace,	"[BackSpace]" },
    { Qt::Key_Return,		"[Return]" },
    { QIMPenChar::Caps,		"[Uppercase]" },
    { QIMPenChar::CapsLock,	"[Caps Lock]" },
    { QIMPenChar::Shortcut,	"[Shortcut]" },
    { QIMPenChar::Punctuation,  "[Punctuation]" },
    { QIMPenChar::Symbol,	"[Symbol]" },
    { QIMPenChar::Extended,	"[Extended]" },
    { Qt::Key_unknown,	        0 } };


/*!
  \class QIMPenChar qimpenchar.h

  Handles a single character.  Can calculate closeness of match to
  another character.
*/

QIMPenChar::QIMPenChar()
{
    flags = 0;
    strokes.setAutoDelete( TRUE );
}

QIMPenChar::QIMPenChar( const QIMPenChar &chr )
{
    strokes.setAutoDelete( TRUE );
    ch = chr.ch;
    flags = chr.flags;
    d = chr.d;
    QIMPenStrokeIterator it( chr.strokes );
    while ( it.current() ) {
        strokes.append( new QIMPenStroke( *it.current() ) );
        ++it;
    }
}

QIMPenChar &QIMPenChar::operator=( const QIMPenChar &chr )
{
    strokes.clear();
    ch = chr.ch;
    flags = chr.flags;
    d = chr.d;
    QIMPenStrokeIterator it( chr.strokes );
    while ( it.current() ) {
        strokes.append( new QIMPenStroke( *it.current() ) );
        ++it;
    }

    return *this;
}

QString QIMPenChar::name() const
{
    QString n;

    if ( (ch & 0x0000FFFF) == 0 ) {
	int code = ch >> 16;
	for ( int i = 0; qimpen_specialKeys[i].code != Qt::Key_unknown; i++ ) {
	    if ( qimpen_specialKeys[i].code == code ) {
		n = qimpen_specialKeys[i].name;
		break;
	    }
	}
    } else {
	n = QChar( ch & 0x0000FFFF );
    }

    return n;
}

void QIMPenChar::clear()
{
    ch = 0;
    flags = 0;
    d = QString::null;
    strokes.clear();
}

unsigned int QIMPenChar::strokeLength( int s ) const
{
    QIMPenStrokeIterator it( strokes );
    while ( it.current() && s ) {
	++it;
	--s;
    }

    if ( it.current() )
	return it.current()->length();

    return 0;
}

/*!
  Add a stroke to the character
*/
void QIMPenChar::addStroke( QIMPenStroke *st )
{
    QIMPenStroke *stroke = new QIMPenStroke( *st );
    strokes.append( stroke );
}

/*!
  Return an indicator of the closeness of this character to \a pen.
  Lower value is better.
*/
int QIMPenChar::match( QIMPenChar *pen )
{
/*
    if ( strokes.count() > pen->strokes.count() )
        return INT_MAX;
*/
    int err = 0;
    int maxErr = 0;
    int diff = 0;
    QIMPenStrokeIterator it1( strokes );
    QIMPenStrokeIterator it2( pen->strokes );
    err = it1.current()->match( it2.current() );
    if ( err > maxErr )
	maxErr = err;
    ++it1;
    ++it2;
    while ( err < 400000 && it1.current() && it2.current() ) {
        QPoint p1 = it1.current()->boundingRect().center() -
		    strokes.getFirst()->boundingRect().center();
        QPoint p2 = it2.current()->boundingRect().center() -
		    pen->strokes.getFirst()->boundingRect().center();
        int xdiff = QABS( p1.x() - p2.x() ) - 6;
        int ydiff = QABS( p1.y() - p2.y() ) - 5;
	if ( xdiff < 0 )
	    xdiff = 0;
	if ( ydiff < 0 )
	    ydiff = 0;
	if ( xdiff > 10 || ydiff > 10 ) { // not a chance
#ifdef DEBUG_QIMPEN
	    qDebug( "char %c, stroke starting pt diff excessive", pen->ch );
#endif
	    return INT_MAX;
	}
        diff += xdiff*xdiff + ydiff*ydiff;
	err = it1.current()->match( it2.current() );
	if ( err > maxErr )
	    maxErr = err;
        ++it1;
        ++it2;
    }

    maxErr += diff * diff * 6; // magic weighting :)

#ifdef DEBUG_QIMPEN
    qDebug( "char: %c, maxErr %d, diff %d, (%d)", pen->ch, maxErr, diff, strokes.count() );
#endif
    return maxErr;
}

/*!
  Return the bounding rect of this character.  It may have sides with
  negative coords since its origin is where the user started drawing
  the character.
*/
QRect QIMPenChar::boundingRect()
{
    QRect br;
    QIMPenStroke *st = strokes.first();
    while ( st ) {
        br |= st->boundingRect();
        st = strokes.next();
    }

    return br;
}


/*!
  Write the character's data to the stream.
*/
QDataStream &operator<< (QDataStream &s, const QIMPenChar &ws)
{
    s << ws.ch;
    s << ws.flags;
    if ( ws.flags & QIMPenChar::Data )
	s << ws.d;
    s << ws.strokes.count();
    QIMPenStrokeIterator it( ws.strokes );
    while ( it.current() ) {
        s << *it.current();
        ++it;
    }

    return s;
}

/*!
  Read the character's data from the stream.
*/
QDataStream &operator>> (QDataStream &s, QIMPenChar &ws)
{
    s >> ws.ch;
    s >> ws.flags;
    if ( ws.flags & QIMPenChar::Data )
	s >> ws.d;
    unsigned size;
    s >> size;
    for ( unsigned i = 0; i < size; i++ ) {
        QIMPenStroke *st = new QIMPenStroke();
        s >> *st;
        ws.strokes.append( st );
    }

    return s;
}

//===========================================================================

bool QIMPenCharMatch::operator>( const QIMPenCharMatch &m )
{
    return error > m.error;
}

bool QIMPenCharMatch::operator<( const QIMPenCharMatch &m )
{
    return error < m.error;
}

bool QIMPenCharMatch::operator<=( const QIMPenCharMatch &m )
{
    return error <= m.error;
}

//===========================================================================

/*!
  \class QIMPenCharSet qimpenchar.h

  Maintains a set of related characters.
*/

QIMPenCharSet::QIMPenCharSet()
{
    chars.setAutoDelete( TRUE );
    desc = "Unnamed";
    csTitle = "abc";
    csType = Unknown;
    maxStrokes = 0;
}

/*!
  Construct and load a characters set from file \a fn.
*/
QIMPenCharSet::QIMPenCharSet( const QString &fn )
{
    chars.setAutoDelete( TRUE );
    desc = "Unnamed";
    csTitle = "abc";
    csType = Unknown;
    maxStrokes = 0;
    load( fn, System );
}

const QString &QIMPenCharSet::filename( Domain d ) const
{
    if ( d == System )
	return sysFilename;
    else
	return userFilename;
}

void QIMPenCharSet::setFilename( const QString &fn, Domain d )
{
    if ( d == System )
	sysFilename = fn;
    else if ( d == User )
	userFilename = fn;
}

/*!
  Load a character set from file \a fn.
*/
bool QIMPenCharSet::load( const QString &fn, Domain d )
{
    setFilename( fn, d );

    bool ok = FALSE;
    QFile file( fn );
    if ( file.open( IO_ReadOnly ) ) {
        QDataStream ds( &file );
	QString version;
	ds >> version;
        ds >> csTitle;
        ds >> desc;
	int major = version.mid( 4, 1 ).toInt();
	int minor = version.mid( 6 ).toInt();
	if ( major >= 1 && minor > 0 ) {
	    ds >> (Q_INT8 &)csType;
	} else {
	    if ( csTitle == "abc" )
		csType = Lower;
	    else if ( csTitle == "ABC" )
		csType = Upper;
	    else if ( csTitle == "123" )
		csType = Numeric;
	    else if ( fn == "Combining" )
		csType = Combining;
	}
        while ( !ds.atEnd() ) {
            QIMPenChar *pc = new QIMPenChar;
            ds >> *pc;
	    if ( d == User )
		markDeleted( pc->character() ); // override system
            addChar( pc );
        }
	if ( file.status() == IO_Ok )
	    ok = TRUE;
    }

    return ok;
}

/*!
  Save this character set.
*/
bool QIMPenCharSet::save( Domain d )
{
    if ( filename( d ).isEmpty() )
        return FALSE;

    bool ok = FALSE;

    QString fn = filename( d );
    QString tmpFn = fn + ".new";
    QFile file( tmpFn );
    if ( file.open( IO_WriteOnly|IO_Raw ) ) {
        QDataStream ds( &file );
	ds << QString( "QPT 1.1" );
        ds << csTitle;
        ds << desc;
	ds << (Q_INT8)csType;
        QIMPenCharIterator ci( chars );
        for ( ; ci.current(); ++ci ) {
	    QIMPenChar *pc = ci.current();
	    if ( ( (d == System) && pc->testFlag( QIMPenChar::System ) ) ||
		 ( (d == User) && !pc->testFlag( QIMPenChar::System ) ) ) {
		ds << *pc;
	    }
	    if ( file.status() != IO_Ok )
		break;
        }
	if ( file.status() == IO_Ok )
	    ok = TRUE;
    }

    if ( ok ) {
	if ( ::rename( tmpFn.latin1(), fn.latin1() ) < 0 ) {
	    qWarning( "problem renaming file %s to %s, errno: %d",
		    tmpFn.latin1(), fn.latin1(), errno );
	    // remove the tmp file, otherwise, it will just lay around...
	    QFile::remove( tmpFn.latin1() );
	    ok = FALSE;
	}
    }

    return ok;
}

QIMPenChar *QIMPenCharSet::at( int i )
{
    return chars.at(i);
}

void QIMPenCharSet::markDeleted( uint ch )
{
    QIMPenCharIterator ci( chars );
    for ( ; ci.current(); ++ci ) {
	QIMPenChar *pc = ci.current();
	if ( pc->character() == ch && pc->testFlag( QIMPenChar::System ) )
	    pc->setFlag( QIMPenChar::Deleted );
    }
}

/*!
  Find the best matches for \a ch in this character set.
*/
QIMPenCharMatchList QIMPenCharSet::match( QIMPenChar *ch )
{
    QIMPenCharMatchList matches;

    QIMPenCharIterator ci( chars );
    for ( ; ci.current(); ++ci ) {
	QIMPenChar *tmplChar = ci.current();
        if ( tmplChar->testFlag( QIMPenChar::Deleted ) ) {
            continue;
        }
	int err;
	if ( ch->penStrokes().count() <= tmplChar->penStrokes().count() ) {
	    err = ch->match( tmplChar );
	    if ( err <= QIMPEN_MATCH_THRESHOLD ) {
		if (tmplChar->penStrokes().count() != ch->penStrokes().count())
		    err = QIMPEN_MATCH_THRESHOLD;
		QIMPenCharMatchList::Iterator it;
		for ( it = matches.begin(); it != matches.end(); ++it ) {
		    if ( (*it).penChar->character() == tmplChar->character() &&
			 (*it).penChar->penStrokes().count() == tmplChar->penStrokes().count() ) {
			if ( (*it).error > err )
			    (*it).error = err;
			break;
		    }
		}
		if ( it == matches.end() ) {
		    QIMPenCharMatch m;
		    m.error = err;
		    m.penChar = tmplChar;
		    matches.append( m );
		}
	    }
	}
    }
    qHeapSort( matches );
/*
    QIMPenCharMatchList::Iterator it;
    for ( it = matches.begin(); it != matches.end(); ++it ) {
	qDebug( "Match: \'%c\', error %d, strokes %d", (*it).penChar->character(),
				(*it).error, (*it).penChar->penStrokes().count() );
    }
*/
    return matches;
}

/*!
  Add a character \a ch to this set.
  QIMPenCharSet will delete this character when it is no longer needed.
*/
void QIMPenCharSet::addChar( QIMPenChar *ch )
{
    if ( ch->penStrokes().count() > maxStrokes )
        maxStrokes = ch->penStrokes().count();
    chars.append( ch );
}

/*!
  Remove a character by reference \a ch from this set.
  QIMPenCharSet will delete this character.
*/
void QIMPenCharSet::removeChar( QIMPenChar *ch )
{
    chars.remove( ch );
}

/*!
  Move the character up the list of characters.
*/
void QIMPenCharSet::up( QIMPenChar *ch )
{
    int idx = chars.findRef( ch );
    if ( idx > 0 ) {
        chars.take();
        chars.insert( idx - 1, ch );
    }
}

/*!
  Move the character down the list of characters.
*/
void QIMPenCharSet::down( QIMPenChar *ch )
{
    int idx = chars.findRef( ch );
    if ( idx >= 0 && idx < (int)chars.count() - 1 ) {
        chars.take();
        chars.insert( idx + 1, ch );
    }
}

