/****************************************************************************
** $Id: qrichtext.cpp,v 1.1 2002-07-14 21:21:35 leseb Exp $
**
** Implementation of the internal Qt classes dealing with rich text
**
** Created : 990101
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
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

#include "qrichtext_p.h"

#include "qstringlist.h"
#include "qfont.h"
#include "qtextstream.h"
#include "qfile.h"
#include "qregexp.h"
#include "qapplication.h"
#include "qclipboard.h"
#include "qmap.h"
#include "qfileinfo.h"
#include "qstylesheet.h"
#include "qmime.h"
#include "qregexp.h"
#include "qimage.h"
#include "qdragobject.h"
#include "qpaintdevicemetrics.h"
#include "qpainter.h"
#include "qdrawutil.h"
#include "qcursor.h"
#include "qstack.h"
#include "qstyle.h"
#include "qcomplextext_p.h"
#include "qcleanuphandler.h"

#include <stdlib.h>

using namespace Qt3;

//#define PARSER_DEBUG
//#define DEBUG_COLLECTION// ---> also in qrichtext_p.h
//#define DEBUG_TABLE_RENDERING

static QTextFormatCollection *qFormatCollection = 0;

const int QStyleSheetItem_WhiteSpaceNoCompression = 3; // ### belongs in QStyleSheetItem, fix 3.1
const int QStyleSheetItem_WhiteSpaceNormalWithNewlines = 4; // ### belongs in QStyleSheetItem, fix 3.1

const int border_tolerance = 2;

#if defined(PARSER_DEBUG)
static QString debug_indent;
#endif

#ifdef Q_WS_WIN
#include "qt_windows.h"
#endif

static inline bool is_printer( QPainter *p )
{
    if ( !p || !p->device() )
	return FALSE;
    return p->device()->devType() == QInternal::Printer;
}

static inline int scale( int value, QPainter *painter )
{
    if ( is_printer( painter ) ) {
	QPaintDeviceMetrics metrics( painter->device() );
#if defined(Q_WS_X11)
	value = value * metrics.logicalDpiY() / QPaintDevice::x11AppDpiY();
#elif defined (Q_WS_WIN)
	HDC hdc = GetDC( 0 );
	int gdc = GetDeviceCaps( hdc, LOGPIXELSY );
	if ( gdc )
	    value = value * metrics.logicalDpiY() / gdc;
	ReleaseDC( 0, hdc );
#elif defined (Q_WS_MAC)
	value = value * metrics.logicalDpiY() / 75; // ##### FIXME
#elif defined (Q_WS_QWS)
	value = value * metrics.logicalDpiY() / 75;
#endif
    }
    return value;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void QTextCommandHistory::addCommand( QTextCommand *cmd )
{
    if ( current < (int)history.count() - 1 ) {
	QPtrList<QTextCommand> commands;
	commands.setAutoDelete( FALSE );

	for( int i = 0; i <= current; ++i ) {
	    commands.insert( i, history.at( 0 ) );
	    history.take( 0 );
	}

	commands.append( cmd );
	history.clear();
	history = commands;
	history.setAutoDelete( TRUE );
    } else {
	history.append( cmd );
    }

    if ( (int)history.count() > steps )
	history.removeFirst();
    else
	++current;
}

QTextCursor *QTextCommandHistory::undo( QTextCursor *c )
{
    if ( current > -1 ) {
	QTextCursor *c2 = history.at( current )->unexecute( c );
	--current;
	return c2;
    }
    return 0;
}

QTextCursor *QTextCommandHistory::redo( QTextCursor *c )
{
    if ( current > -1 ) {
	if ( current < (int)history.count() - 1 ) {
	    ++current;
	    return history.at( current )->execute( c );
	}
    } else {
	if ( history.count() > 0 ) {
	    ++current;
	    return history.at( current )->execute( c );
	}
    }
    return 0;
}

bool QTextCommandHistory::isUndoAvailable()
{
    return current > -1;
}

bool QTextCommandHistory::isRedoAvailable()
{
   return current > -1 && current < (int)history.count() - 1 || current == -1 && history.count() > 0;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

QTextDeleteCommand::QTextDeleteCommand( QTextDocument *d, int i, int idx, const QMemArray<QTextStringChar> &str,
					const QValueList< QPtrVector<QStyleSheetItem> > &os,
					const QValueList<QStyleSheetItem::ListStyle> &ols,
					const QMemArray<int> &oas)
    : QTextCommand( d ), id( i ), index( idx ), parag( 0 ), text( str ), oldStyles( os ), oldListStyles( ols ), oldAligns( oas )
{
    for ( int j = 0; j < (int)text.size(); ++j ) {
	if ( text[ j ].format() )
	    text[ j ].format()->addRef();
    }
}

QTextDeleteCommand::QTextDeleteCommand( QTextParag *p, int idx, const QMemArray<QTextStringChar> &str )
    : QTextCommand( 0 ), id( -1 ), index( idx ), parag( p ), text( str )
{
    for ( int i = 0; i < (int)text.size(); ++i ) {
	if ( text[ i ].format() )
	    text[ i ].format()->addRef();
    }
}

QTextDeleteCommand::~QTextDeleteCommand()
{
    for ( int i = 0; i < (int)text.size(); ++i ) {
	if ( text[ i ].format() )
	    text[ i ].format()->removeRef();
    }
    text.resize( 0 );
}

QTextCursor *QTextDeleteCommand::execute( QTextCursor *c )
{
    QTextParag *s = doc ? doc->paragAt( id ) : parag;
    if ( !s ) {
	qWarning( "can't locate parag at %d, last parag: %d", id, doc->lastParag()->paragId() );
	return 0;
    }

    cursor.setParag( s );
    cursor.setIndex( index );
    int len = text.size();
    if ( c )
	*c = cursor;
    if ( doc ) {
	doc->setSelectionStart( QTextDocument::Temp, &cursor );
	for ( int i = 0; i < len; ++i )
	    cursor.gotoNextLetter();
	doc->setSelectionEnd( QTextDocument::Temp, &cursor );
	doc->removeSelectedText( QTextDocument::Temp, &cursor );
	if ( c )
	    *c = cursor;
    } else {
	s->remove( index, len );
    }

    return c;
}

QTextCursor *QTextDeleteCommand::unexecute( QTextCursor *c )
{
    QTextParag *s = doc ? doc->paragAt( id ) : parag;
    if ( !s ) {
	qWarning( "can't locate parag at %d, last parag: %d", id, doc->lastParag()->paragId() );
	return 0;
    }

    cursor.setParag( s );
    cursor.setIndex( index );
    QString str = QTextString::toString( text );
    cursor.insert( str, TRUE, &text );
    cursor.setParag( s );
    cursor.setIndex( index );
    if ( c ) {
	c->setParag( s );
	c->setIndex( index );
	for ( int i = 0; i < (int)text.size(); ++i )
	    c->gotoNextLetter();
    }

    QValueList< QPtrVector<QStyleSheetItem> >::Iterator it = oldStyles.begin();
    QValueList<QStyleSheetItem::ListStyle>::Iterator lit = oldListStyles.begin();
    int i = 0;
    QTextParag *p = s;
    bool end = FALSE;
    while ( p ) {
	if ( it != oldStyles.end() )
	    p->setStyleSheetItems( *it );
	else
	    end = TRUE;
	if ( lit != oldListStyles.end() )
	    p->setListStyle( *lit );
	else
	    end = TRUE;
	if ( i < (int)oldAligns.size() )
	    p->setAlignment( oldAligns.at( i ) );
	else
	    end = TRUE;
	if ( end )
	    break;
	p = p->next();
	++it;
	++lit;
	++i;
    }

    s = cursor.parag();
    while ( s ) {
	s->format();
	s->setChanged( TRUE );
	if ( s == c->parag() )
	    break;
	s = s->next();
    }

    return &cursor;
}

QTextFormatCommand::QTextFormatCommand( QTextDocument *d, int sid, int sidx, int eid, int eidx,
					const QMemArray<QTextStringChar> &old, QTextFormat *f, int fl )
    : QTextCommand( d ), startId( sid ), startIndex( sidx ), endId( eid ), endIndex( eidx ), format( f ), oldFormats( old ), flags( fl )
{
    format = d->formatCollection()->format( f );
    for ( int j = 0; j < (int)oldFormats.size(); ++j ) {
	if ( oldFormats[ j ].format() )
	    oldFormats[ j ].format()->addRef();
    }
}

QTextFormatCommand::~QTextFormatCommand()
{
    format->removeRef();
    for ( int j = 0; j < (int)oldFormats.size(); ++j ) {
	if ( oldFormats[ j ].format() )
	    oldFormats[ j ].format()->removeRef();
    }
}

QTextCursor *QTextFormatCommand::execute( QTextCursor *c )
{
    QTextParag *sp = doc->paragAt( startId );
    QTextParag *ep = doc->paragAt( endId );
    if ( !sp || !ep )
	return c;

    QTextCursor start( doc );
    start.setParag( sp );
    start.setIndex( startIndex );
    QTextCursor end( doc );
    end.setParag( ep );
    end.setIndex( endIndex );

    doc->setSelectionStart( QTextDocument::Temp, &start );
    doc->setSelectionEnd( QTextDocument::Temp, &end );
    doc->setFormat( QTextDocument::Temp, format, flags );
    doc->removeSelection( QTextDocument::Temp );
    if ( endIndex == ep->length() )
	end.gotoLeft();
    *c = end;
    return c;
}

QTextCursor *QTextFormatCommand::unexecute( QTextCursor *c )
{
    QTextParag *sp = doc->paragAt( startId );
    QTextParag *ep = doc->paragAt( endId );
    if ( !sp || !ep )
	return 0;

    int idx = startIndex;
    int fIndex = 0;
    for ( ;; ) {
	if ( oldFormats.at( fIndex ).c == '\n' ) {
	    if ( idx > 0 ) {
		if ( idx < sp->length() && fIndex > 0 )
		    sp->setFormat( idx, 1, oldFormats.at( fIndex - 1 ).format() );
		if ( sp == ep )
		    break;
		sp = sp->next();
		idx = 0;
	    }
	    fIndex++;
	}
	if ( oldFormats.at( fIndex ).format() )
	    sp->setFormat( idx, 1, oldFormats.at( fIndex ).format() );
	idx++;
	fIndex++;
	if ( fIndex >= (int)oldFormats.size() )
	    break;
	if ( idx >= sp->length() ) {
	    if ( sp == ep )
		break;
	    sp = sp->next();
	    idx = 0;
	}
    }

    QTextCursor end( doc );
    end.setParag( ep );
    end.setIndex( endIndex );
    if ( endIndex == ep->length() )
	end.gotoLeft();
    *c = end;
    return c;
}

QTextAlignmentCommand::QTextAlignmentCommand( QTextDocument *d, int fParag, int lParag, int na, const QMemArray<int> &oa )
    : QTextCommand( d ), firstParag( fParag ), lastParag( lParag ), newAlign( na ), oldAligns( oa )
{
}

QTextCursor *QTextAlignmentCommand::execute( QTextCursor *c )
{
    QTextParag *p = doc->paragAt( firstParag );
    if ( !p )
	return c;
    while ( p ) {
	p->setAlignment( newAlign );
	if ( p->paragId() == lastParag )
	    break;
	p = p->next();
    }
    return c;
}

QTextCursor *QTextAlignmentCommand::unexecute( QTextCursor *c )
{
    QTextParag *p = doc->paragAt( firstParag );
    if ( !p )
	return c;
    int i = 0;
    while ( p ) {
	if ( i < (int)oldAligns.size() )
	    p->setAlignment( oldAligns.at( i ) );
	if ( p->paragId() == lastParag )
	    break;
	p = p->next();
	++i;
    }
    return c;
}

QTextParagTypeCommand::QTextParagTypeCommand( QTextDocument *d, int fParag, int lParag, bool l,
					      QStyleSheetItem::ListStyle s, const QValueList< QPtrVector<QStyleSheetItem> > &os,
					      const QValueList<QStyleSheetItem::ListStyle> &ols )
    : QTextCommand( d ), firstParag( fParag ), lastParag( lParag ), list( l ), listStyle( s ), oldStyles( os ), oldListStyles( ols )
{
}

QTextCursor *QTextParagTypeCommand::execute( QTextCursor *c )
{
    QTextParag *p = doc->paragAt( firstParag );
    if ( !p )
	return c;
    while ( p ) {
	p->setList( list, (int)listStyle );
	if ( p->paragId() == lastParag )
	    break;
	p = p->next();
    }
    return c;
}

QTextCursor *QTextParagTypeCommand::unexecute( QTextCursor *c )
{
    QTextParag *p = doc->paragAt( firstParag );
    if ( !p )
	return c;
    QValueList< QPtrVector<QStyleSheetItem> >::Iterator it = oldStyles.begin();
    QValueList<QStyleSheetItem::ListStyle>::Iterator lit = oldListStyles.begin();
    while ( p ) {
	if ( it != oldStyles.end() )
	    p->setStyleSheetItems( *it );
	if ( lit != oldListStyles.end() )
	    p->setListStyle( *lit );
	if ( p->paragId() == lastParag )
	    break;
	p = p->next();
	++it;
	++lit;
    }
    return c;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

QTextCursor::QTextCursor( QTextDocument *d )
    : doc( d ), ox( 0 ), oy( 0 )
{
    nested = FALSE;
    idx = 0;
    string = doc ? doc->firstParag() : 0;
    tmpIndex = -1;
    valid = TRUE;
}

QTextCursor::QTextCursor()
{
}

QTextCursor::QTextCursor( const QTextCursor &c )
{
    doc = c.doc;
    ox = c.ox;
    oy = c.oy;
    nested = c.nested;
    idx = c.idx;
    string = c.string;
    tmpIndex = c.tmpIndex;
    indices = c.indices;
    parags = c.parags;
    xOffsets = c.xOffsets;
    yOffsets = c.yOffsets;
    valid = c.valid;
}

QTextCursor &QTextCursor::operator=( const QTextCursor &c )
{
    doc = c.doc;
    ox = c.ox;
    oy = c.oy;
    nested = c.nested;
    idx = c.idx;
    string = c.string;
    tmpIndex = c.tmpIndex;
    indices = c.indices;
    parags = c.parags;
    xOffsets = c.xOffsets;
    yOffsets = c.yOffsets;
    valid = c.valid;

    return *this;
}

bool QTextCursor::operator==( const QTextCursor &c ) const
{
    return doc == c.doc && string == c.string && idx == c.idx;
}

int QTextCursor::totalOffsetX() const
{
    if ( !nested )
	return 0;
    QValueStack<int>::ConstIterator xit = xOffsets.begin();
    int xoff = ox;
    for ( ; xit != xOffsets.end(); ++xit )
	xoff += *xit;
    return xoff;
}

int QTextCursor::totalOffsetY() const
{
    if ( !nested )
	return 0;
    QValueStack<int>::ConstIterator yit = yOffsets.begin();
    int yoff = oy;
    for ( ; yit != yOffsets.end(); ++yit )
	yoff += *yit;
    return yoff;
}

void QTextCursor::gotoIntoNested( const QPoint &globalPos )
{
    if ( !doc )
	return;
    push();
    ox = 0;
    int bl, y;
    string->lineHeightOfChar( idx, &bl, &y );
    oy = y + string->rect().y();
    nested = TRUE;
    QPoint p( globalPos.x() - offsetX(), globalPos.y() - offsetY() );
    Q_ASSERT( string->at( idx )->isCustom() );
    ox = string->at( idx )->x;
    string->at( idx )->customItem()->enterAt( this, doc, string, idx, ox, oy, p );
}

void QTextCursor::invalidateNested()
{
    if ( nested ) {
	QValueStack<QTextParag*>::Iterator it = parags.begin();
	QValueStack<int>::Iterator it2 = indices.begin();
	for ( ; it != parags.end(); ++it, ++it2 ) {
	    if ( *it == string )
		continue;
	    (*it)->invalidate( 0 );
	    if ( (*it)->at( *it2 )->isCustom() )
		(*it)->at( *it2 )->customItem()->invalidate();
	}
    }
}

void QTextCursor::insert( const QString &str, bool checkNewLine, QMemArray<QTextStringChar> *formatting )
{
    tmpIndex = -1;
    bool justInsert = TRUE;
    QString s( str );
#if defined(Q_WS_WIN)
    if ( checkNewLine )
	s = s.replace( QRegExp( "\\r" ), "" );
#endif
    if ( checkNewLine )
	justInsert = s.find( '\n' ) == -1;
    if ( justInsert ) {
	string->insert( idx, s );
	if ( formatting ) {
	    for ( int i = 0; i < (int)s.length(); ++i ) {
		if ( formatting->at( i ).format() ) {
		    formatting->at( i ).format()->addRef();
		    string->string()->setFormat( idx + i, formatting->at( i ).format(), TRUE );
		}
	    }
	}
	idx += s.length();
    } else {
	QStringList lst = QStringList::split( '\n', s, TRUE );
	QStringList::Iterator it = lst.begin();
	int y = string->rect().y() + string->rect().height();
	int lastIndex = 0;
	QTextFormat *lastFormat = 0;
	for ( ; it != lst.end(); ) {
	    if ( it != lst.begin() ) {
		splitAndInsertEmptyParag( FALSE, TRUE );
		string->setEndState( -1 );
		string->prev()->format( -1, FALSE );
		if ( lastFormat && formatting && string->prev() ) {
		    lastFormat->addRef();
		    string->prev()->string()->setFormat( string->prev()->length() - 1, lastFormat, TRUE );
		}
	    }
	    lastFormat = 0;
	    QString s = *it;
	    ++it;
	    if ( !s.isEmpty() )
		string->insert( idx, s );
	    else
		string->invalidate( 0 );
	    if ( formatting ) {
		int len = s.length();
		for ( int i = 0; i < len; ++i ) {
		    if ( formatting->at( i + lastIndex ).format() ) {
			formatting->at( i + lastIndex ).format()->addRef();
			string->string()->setFormat( i + idx, formatting->at( i + lastIndex ).format(), TRUE );
		    }
		}
		if ( it != lst.end() )
		    lastFormat = formatting->at( len + lastIndex ).format();
		++len;
		lastIndex += len;
	    }

	    idx += s.length();
	}
	string->format( -1, FALSE );
	int dy = string->rect().y() + string->rect().height() - y;
	QTextParag *p = string;
	p->setParagId( p->prev()->paragId() + 1 );
	p = p->next();
	while ( p ) {
	    p->setParagId( p->prev()->paragId() + 1 );
	    p->move( dy );
	    p->invalidate( 0 );
	    p->setEndState( -1 );
	    p = p->next();
	}
    }

    int h = string->rect().height();
    string->format( -1, TRUE );
    if ( h != string->rect().height() )
	invalidateNested();
    else if ( doc && doc->parent() )
	doc->nextDoubleBuffered = TRUE;
}

void QTextCursor::gotoLeft()
{
    if ( string->string()->isRightToLeft() )
	gotoNextLetter();
    else
	gotoPreviousLetter();
}

void QTextCursor::gotoPreviousLetter()
{
    tmpIndex = -1;

    if ( idx > 0 ) {
	idx--;
    } else if ( string->prev() ) {
	QTextParag *s = string->prev();
	while ( s && !s->isVisible() )
	    s = s->prev();
        if ( s ) {
	    string = s;
	    idx = string->length() - 1;
        }
    } else {
	if ( nested ) {
	    pop();
	    processNesting( Prev );
	    if ( idx == -1 ) {
		pop();
		if ( idx > 0 ) {
		    idx--;
		} else if ( string->prev() ) {
		    string = string->prev();
		    idx = string->length() - 1;
		}
	    }
	}
    }

    const QTextStringChar *tsc = string->at( idx );
    if ( tsc && tsc->isCustom() && tsc->customItem()->isNested() ) {
	processNesting( EnterEnd );
    }
}

void QTextCursor::push()
{
    indices.push( idx );
    parags.push( string );
    xOffsets.push( ox );
    yOffsets.push( oy );
    nestedStack.push( nested );
}

void QTextCursor::pop()
{
    if ( !doc )
	return;
    idx = indices.pop();
    string = parags.pop();
    ox = xOffsets.pop();
    oy = yOffsets.pop();
    if ( doc->parent() )
	doc = doc->parent();
    nested = nestedStack.pop();
}

void QTextCursor::restoreState()
{
    while ( !indices.isEmpty() )
	pop();
}

bool QTextCursor::place( const QPoint &p, QTextParag *s, bool link )
{
    QPoint pos( p );
    QRect r;
    QTextParag *str = s;
    if ( pos.y() < s->rect().y() )
	pos.setY( s->rect().y() );
    while ( s ) {
	r = s->rect();
	r.setWidth( doc ? doc->width() : QWIDGETSIZE_MAX );
	if ( s->isVisible() )
	    str = s;
	if ( pos.y() >= r.y() && pos.y() <= r.y() + r.height() || !s->next() )
	    break;
	s = s->next();
    }

    if ( !s || !str )
	return FALSE;

    s = str;

    setParag( s, FALSE );
    int y = s->rect().y();
    int lines = s->lines();
    QTextStringChar *chr = 0;
    int index = 0;
    int i = 0;
    int cy = 0;
    int ch = 0;
    for ( ; i < lines; ++i ) {
	chr = s->lineStartOfLine( i, &index );
	cy = s->lineY( i );
	ch = s->lineHeight( i );
	if ( !chr )
	    return FALSE;
	if ( pos.y() >= y + cy && pos.y() <= y + cy + ch )
	    break;
    }
    int nextLine;
    if ( i < lines - 1 )
	s->lineStartOfLine( i+1, &nextLine );
    else
	nextLine = s->length();
    i = index;
    int x = s->rect().x();
    if ( pos.x() < x )
	pos.setX( x + 1 );
    int cw;
    int curpos = s->length()-1;
    int dist = 10000000;
    bool inCustom = FALSE;
    while ( i < nextLine ) {
	chr = s->at(i);
	int cpos = x + chr->x;
	cw = s->string()->width( i );
	if ( chr->isCustom() && chr->customItem()->isNested() ) {
	    if ( pos.x() >= cpos && pos.x() <= cpos + cw &&
		 pos.y() >= y + cy && pos.y() <= y + cy + chr->height() ) {
		inCustom = TRUE;
		curpos = i;
		break;
	    }
	} else {
	    if( chr->rightToLeft )
		cpos += cw;
	    int d = cpos - pos.x();
	    bool dm = d < 0 ? !chr->rightToLeft : chr->rightToLeft;
	    if ( QABS( d ) < dist || (dist == d && dm == TRUE ) ) {
		dist = QABS( d );
		if ( !link || pos.x() >= x + chr->x )
		    curpos = i;
	    }
	}
	i++;
    }
    setIndex( curpos, FALSE );

    if ( inCustom && doc && parag()->at( curpos )->isCustom() && parag()->at( curpos )->customItem()->isNested() ) {
	QTextDocument *oldDoc = doc;
	gotoIntoNested( pos );
	if ( oldDoc == doc )
	    return TRUE;
	QPoint p( pos.x() - offsetX(), pos.y() - offsetY() );
	if ( !place( p, document()->firstParag(), link ) )
	    pop();
    }
    return TRUE;
}

void QTextCursor::processNesting( Operation op )
{
    if ( !doc )
	return;
    push();
    ox = string->at( idx )->x;
    int bl, y;
    string->lineHeightOfChar( idx, &bl, &y );
    oy = y + string->rect().y();
    nested = TRUE;
    bool ok = FALSE;

    switch ( op ) {
    case EnterBegin:
	ok = string->at( idx )->customItem()->enter( this, doc, string, idx, ox, oy );
	break;
    case EnterEnd:
	ok = string->at( idx )->customItem()->enter( this, doc, string, idx, ox, oy, TRUE );
	break;
    case Next:
	ok = string->at( idx )->customItem()->next( this, doc, string, idx, ox, oy );
	break;
    case Prev:
	ok = string->at( idx )->customItem()->prev( this, doc, string, idx, ox, oy );
	break;
    case Down:
	ok = string->at( idx )->customItem()->down( this, doc, string, idx, ox, oy );
	break;
    case Up:
	ok = string->at( idx )->customItem()->up( this, doc, string, idx, ox, oy );
	break;
    }
    if ( !ok )
	pop();
}

void QTextCursor::gotoRight()
{
    if ( string->string()->isRightToLeft() )
	gotoPreviousLetter();
    else
	gotoNextLetter();
}

void QTextCursor::gotoNextLetter()
{
    tmpIndex = -1;

    const QTextStringChar *tsc = string->at( idx );
    if ( tsc && tsc->isCustom() && tsc->customItem()->isNested() ) {
	processNesting( EnterBegin );
	return;
    }

    if ( idx < string->length() - 1 ) {
	idx++;
    } else if ( string->next() ) {
	QTextParag *s = string->next();
	while ( s && !s->isVisible() )
	    s = s->next();
        if ( s ) {
	    string = s;
	    idx = 0;
        }
    } else {
	if ( nested ) {
	    pop();
	    processNesting( Next );
	    if ( idx == -1 ) {
		pop();
		if ( idx < string->length() - 1 ) {
		    idx++;
		} else if ( string->next() ) {
		    string = string->next();
		    idx = 0;
		}
	    }
	}
    }
}

void QTextCursor::gotoUp()
{
    int indexOfLineStart;
    int line;
    QTextStringChar *c = string->lineStartOfChar( idx, &indexOfLineStart, &line );
    if ( !c )
	return;

    tmpIndex = QMAX( tmpIndex, idx - indexOfLineStart );
    if ( indexOfLineStart == 0 ) {
	if ( !string->prev() ) {
	    if ( !nested )
		return;
	    pop();
	    processNesting( Up );
	    if ( idx == -1 ) {
		pop();
		if ( !string->prev() )
		    return;
		idx = tmpIndex = 0;
	    } else {
		tmpIndex = -1;
		return;
	    }
	}
	QTextParag *s = string->prev();
	while ( s && !s->isVisible() )
	    s = s->prev();
        if ( s )
	    string = s;
	int lastLine = string->lines() - 1;
	if ( !string->lineStartOfLine( lastLine, &indexOfLineStart ) )
	    return;
	if ( indexOfLineStart + tmpIndex < string->length() )
	    idx = indexOfLineStart + tmpIndex;
	else
	    idx = string->length() - 1;
    } else {
	--line;
	int oldIndexOfLineStart = indexOfLineStart;
	if ( !string->lineStartOfLine( line, &indexOfLineStart ) )
	    return;
	if ( indexOfLineStart + tmpIndex < oldIndexOfLineStart )
	    idx = indexOfLineStart + tmpIndex;
	else
	    idx = oldIndexOfLineStart - 1;
    }
}

void QTextCursor::gotoDown()
{
    int indexOfLineStart;
    int line;
    QTextStringChar *c = string->lineStartOfChar( idx, &indexOfLineStart, &line );
    if ( !c )
	return;

    tmpIndex = QMAX( tmpIndex, idx - indexOfLineStart );
    if ( line == string->lines() - 1 ) {
	if ( !string->next() ) {
	    if ( !nested )
		return;
	    pop();
	    processNesting( Down );
	    if ( idx == -1 ) {
		pop();
		if ( !string->next() )
		    return;
		idx = tmpIndex = 0;
	    } else {
		tmpIndex = -1;
		return;
	    }
	}
	QTextParag *s = string->next();
	while ( s && !s->isVisible() )
	    s = s->next();
	if ( s )
	    string = s;
	if ( !string->lineStartOfLine( 0, &indexOfLineStart ) )
	    return;
	int end;
	if ( string->lines() == 1 )
	    end = string->length();
	else
	    string->lineStartOfLine( 1, &end );
	if ( indexOfLineStart + tmpIndex < end )
	    idx = indexOfLineStart + tmpIndex;
	else
	    idx = end - 1;
    } else {
	++line;
	int end;
	if ( line == string->lines() - 1 )
	    end = string->length();
	else
	    string->lineStartOfLine( line + 1, &end );
	if ( !string->lineStartOfLine( line, &indexOfLineStart ) )
	    return;
	if ( indexOfLineStart + tmpIndex < end )
	    idx = indexOfLineStart + tmpIndex;
	else
	    idx = end - 1;
    }
}

void QTextCursor::gotoLineEnd()
{
    tmpIndex = -1;
    int indexOfLineStart;
    int line;
    QTextStringChar *c = string->lineStartOfChar( idx, &indexOfLineStart, &line );
    if ( !c )
	return;

    if ( line == string->lines() - 1 ) {
	idx = string->length() - 1;
    } else {
	c = string->lineStartOfLine( ++line, &indexOfLineStart );
	indexOfLineStart--;
	idx = indexOfLineStart;
    }
}

void QTextCursor::gotoLineStart()
{
    tmpIndex = -1;
    int indexOfLineStart;
    int line;
    QTextStringChar *c = string->lineStartOfChar( idx, &indexOfLineStart, &line );
    if ( !c )
	return;

    idx = indexOfLineStart;
}

void QTextCursor::gotoHome()
{
    tmpIndex = -1;
    if ( doc )
	string = doc->firstParag();
    idx = 0;
}

void QTextCursor::gotoEnd()
{
    if ( doc && !doc->lastParag()->isValid() )
	return;

    tmpIndex = -1;
    if ( doc )
	string = doc->lastParag();
    idx = string->length() - 1;
}

void QTextCursor::gotoPageUp( int visibleHeight )
{
    tmpIndex = -1;
    QTextParag *s = string;
    int h = visibleHeight;
    int y = s->rect().y();
    while ( s ) {
	if ( y - s->rect().y() >= h )
	    break;
	s = s->prev();
    }

    if ( !s && doc )
	s = doc->firstParag();

    string = s;
    idx = 0;
}

void QTextCursor::gotoPageDown( int visibleHeight )
{
    tmpIndex = -1;
    QTextParag *s = string;
    int h = visibleHeight;
    int y = s->rect().y();
    while ( s ) {
	if ( s->rect().y() - y >= h )
	    break;
	s = s->next();
    }

    if ( !s && doc ) {
	s = doc->lastParag();
	string = s;
	idx = string->length() - 1;
	return;
    }

    if ( !s->isValid() )
	return;

    string = s;
    idx = 0;
}

void QTextCursor::gotoWordRight()
{
    if ( string->string()->isRightToLeft() )
	gotoPreviousWord();
    else
	gotoNextWord();
}

void QTextCursor::gotoWordLeft()
{
    if ( string->string()->isRightToLeft() )
	gotoNextWord();
    else
	gotoPreviousWord();
}

void QTextCursor::gotoPreviousWord()
{
    gotoPreviousLetter();
    tmpIndex = -1;
    QTextString *s = string->string();
    bool allowSame = FALSE;
    if ( idx == ((int)s->length()-1) )
	return;
    for ( int i = idx; i >= 0; --i ) {
	if ( s->at( i ).c.isSpace() || s->at( i ).c == '\t' || s->at( i ).c == '.' ||
	     s->at( i ).c == ',' || s->at( i ).c == ':' || s->at( i ).c == ';' ) {
	    if ( !allowSame )
		continue;
	    idx = i + 1;
	    return;
	}
	if ( !allowSame && !( s->at( i ).c.isSpace() || s->at( i ).c == '\t' || s->at( i ).c == '.' ||
			      s->at( i ).c == ',' || s->at( i ).c == ':' || s->at( i ).c == ';'  ) )
	    allowSame = TRUE;
    }
    idx = 0;
}

void QTextCursor::gotoNextWord()
{
    tmpIndex = -1;
    QTextString *s = string->string();
    bool allowSame = FALSE;
    for ( int i = idx; i < (int)s->length(); ++i ) {
	if ( ! (s->at( i ).c.isSpace() || s->at( i ).c == '\t' || s->at( i ).c == '.' ||
	     s->at( i ).c == ',' || s->at( i ).c == ':' || s->at( i ).c == ';') ) {
	    if ( !allowSame )
		continue;
	    idx = i;
	    return;
	}
	if ( !allowSame && ( s->at( i ).c.isSpace() || s->at( i ).c == '\t' || s->at( i ).c == '.' ||
	    s->at( i ).c == ',' || s->at( i ).c == ':' || s->at( i ).c == ';'  ) )
	    allowSame = TRUE;

    }

    if ( idx < ((int)s->length()-1) ) {
	gotoLineEnd();
    } else if ( string->next() ) {
	QTextParag *s = string->next();
	while ( s  && !s->isVisible() )
	    s = s->next();
	if ( s ) {
	    string = s;
	    idx = 0;
	}
    } else {
	gotoLineEnd();
    }
}

bool QTextCursor::atParagStart()
{
    return idx == 0;
}

bool QTextCursor::atParagEnd()
{
    return idx == string->length() - 1;
}

void QTextCursor::splitAndInsertEmptyParag( bool ind, bool updateIds )
{
    if ( !doc )
	return;
    tmpIndex = -1;
    QTextFormat *f = 0;
    if ( doc->useFormatCollection() ) {
	f = string->at( idx )->format();
	if ( idx == string->length() - 1 && idx > 0 )
	    f = string->at( idx - 1 )->format();
	if ( f->isMisspelled() ) {
	    f->removeRef();
	    f = doc->formatCollection()->format( f->font(), f->color() );
	}
    }

    if ( atParagEnd() ) {
	QTextParag *n = string->next();
	QTextParag *s = doc->createParag( doc, string, n, updateIds );
	if ( f )
	    s->setFormat( 0, 1, f, TRUE );
	s->copyParagData( string );
	if ( ind ) {
	    int oi, ni;
	    s->indent( &oi, &ni );
	    string = s;
	    idx = ni;
	} else {
	    string = s;
	    idx = 0;
	}
    } else if ( atParagStart() ) {
	QTextParag *p = string->prev();
	QTextParag *s = doc->createParag( doc, p, string, updateIds );
	if ( f )
	    s->setFormat( 0, 1, f, TRUE );
	s->copyParagData( string );
	if ( ind ) {
	    s->indent();
	    s->format();
	    indent();
	    string->format();
	}
    } else {
	QString str = string->string()->toString().mid( idx, 0xFFFFFF );
	QTextParag *n = string->next();
	QTextParag *s = doc->createParag( doc, string, n, updateIds );
	s->copyParagData( string );
	s->remove( 0, 1 );
	s->append( str, TRUE );
	for ( uint i = 0; i < str.length(); ++i ) {
	    s->setFormat( i, 1, string->at( idx + i )->format(), TRUE );
	    if ( string->at( idx + i )->isCustom() ) {
		QTextCustomItem * item = string->at( idx + i )->customItem();
		s->at( i )->setCustomItem( item );
		string->at( idx + i )->loseCustomItem();
	    }
	}
	string->truncate( idx );
	if ( ind ) {
	    int oi, ni;
	    s->indent( &oi, &ni );
	    string = s;
	    idx = ni;
	} else {
	    string = s;
	    idx = 0;
	}
    }

    invalidateNested();
}

bool QTextCursor::remove()
{
    tmpIndex = -1;
    if ( !atParagEnd() ) {
	string->remove( idx, 1 );
	int h = string->rect().height();
	string->format( -1, TRUE );
	if ( h != string->rect().height() )
	    invalidateNested();
	else if ( doc && doc->parent() )
	    doc->nextDoubleBuffered = TRUE;
	return FALSE;
    } else if ( string->next() ) {
	if ( string->length() == 1 ) {
	    string->next()->setPrev( string->prev() );
	    if ( string->prev() )
		string->prev()->setNext( string->next() );
	    QTextParag *p = string->next();
	    delete string;
	    string = p;
	    string->invalidate( 0 );
	    QTextParag *s = string;
	    while ( s ) {
		s->id = s->p ? s->p->id + 1 : 0;
		s->state = -1;
		s->needPreProcess = TRUE;
		s->changed = TRUE;
		s = s->n;
	    }
	    string->format();
	} else {
	    string->join( string->next() );
	}
	invalidateNested();
	return TRUE;
    }
    return FALSE;
}

void QTextCursor::killLine()
{
    if ( atParagEnd() )
	return;
    string->remove( idx, string->length() - idx - 1 );
    int h = string->rect().height();
    string->format( -1, TRUE );
    if ( h != string->rect().height() )
	invalidateNested();
    else if ( doc && doc->parent() )
	doc->nextDoubleBuffered = TRUE;
}

void QTextCursor::indent()
{
    int oi = 0, ni = 0;
    string->indent( &oi, &ni );
    if ( oi == ni )
	return;

    if ( idx >= oi )
	idx += ni - oi;
    else
	idx = ni;
}

void QTextCursor::setDocument( QTextDocument *d )
{
    doc = d;
    string = d->firstParag();
    idx = 0;
    nested = FALSE;
    restoreState();
    tmpIndex = -1;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

QTextDocument::QTextDocument( QTextDocument *p )
    : par( p ), parParag( 0 ), tc( 0 ), tArray( 0 ), tStopWidth( 0 )
{
    fCollection = new QTextFormatCollection;
    init();
}

QTextDocument::QTextDocument( QTextDocument *p, QTextFormatCollection *f )
    : par( p ), parParag( 0 ), tc( 0 ), tArray( 0 ), tStopWidth( 0 )
{
    fCollection = f;
    init();
}

void QTextDocument::init()
{
#if defined(PARSER_DEBUG)
    qDebug( debug_indent + "new QTextDocument (%p)", this );
#endif
    oTextValid = TRUE;
    mightHaveCustomItems = FALSE;
    if ( par )
	par->insertChild( this );
    pProcessor = 0;
    useFC = TRUE;
    pFormatter = 0;
    indenter = 0;
    fParag = 0;
    txtFormat = Qt::AutoText;
    preferRichText = FALSE;
    pages = FALSE;
    focusIndicator.parag = 0;
    minw = 0;
    wused = 0;
    minwParag = curParag = 0;
    align = AlignAuto;
    nSelections = 1;
    addMargs = FALSE;

    sheet_ = QStyleSheet::defaultSheet();
    factory_ = QMimeSourceFactory::defaultFactory();
    contxt = QString::null;
    fCollection->setStyleSheet( sheet_ );

    underlLinks = par ? par->underlLinks : TRUE;
    backBrush = 0;
    buf_pixmap = 0;
    nextDoubleBuffered = FALSE;

    if ( par )
	withoutDoubleBuffer = par->withoutDoubleBuffer;
    else
	withoutDoubleBuffer = FALSE;

    lParag = fParag = createParag( this, 0, 0 );
    tmpCursor = 0;

    cx = 0;
    cy = 2;
    if ( par )
	cx = cy = 0;
    cw = 600;
    vw = 0;
    flow_ = new QTextFlow;
    flow_->setWidth( cw );

    leftmargin = rightmargin = 4;

    selectionColors[ Standard ] = QApplication::palette().color( QPalette::Active, QColorGroup::Highlight );
    selectionText[ Standard ] = TRUE;
    commandHistory = new QTextCommandHistory( 100 );
    tStopWidth = formatCollection()->defaultFormat()->width( 'x' ) * 8;
}

QTextDocument::~QTextDocument()
{
    if ( par )
	par->removeChild( this );
    clear();
    delete commandHistory;
    delete flow_;
    if ( !par )
	delete pFormatter;
    delete fCollection;
    delete pProcessor;
    delete buf_pixmap;
    delete indenter;
    delete backBrush;
    if ( tArray )
	delete [] tArray;
}

void QTextDocument::clear( bool createEmptyParag )
{
    if ( flow_ )
	flow_->clear();
    while ( fParag ) {
	QTextParag *p = fParag->next();
	delete fParag;
	fParag = p;
    }
    fParag = lParag = 0;
    if ( createEmptyParag )
	fParag = lParag = createParag( this );
    selections.clear();
}

int QTextDocument::widthUsed() const
{
    return wused + border_tolerance;
}

int QTextDocument::height() const
{
    int h = 0;
    if ( lParag )
	h = lParag->rect().top() + lParag->rect().height() + 1;
    int fh = flow_->boundingRect().bottom();
    return QMAX( h, fh );
}



QTextParag *QTextDocument::createParag( QTextDocument *d, QTextParag *pr, QTextParag *nx, bool updateIds )
{
    return new QTextParag( d, pr, nx, updateIds );
}

bool QTextDocument::setMinimumWidth( int needed, int used, QTextParag *p )
{
    if ( needed == -1 ) {
	minw = 0;
	wused = 0;
	p = 0;
    }
    if ( p == minwParag ) {
	minw = needed;
	emit minimumWidthChanged( minw );
    } else if ( needed > minw ) {
	minw = needed;
	minwParag = p;
	emit minimumWidthChanged( minw );
    }
    wused = QMAX( wused, used );
    wused = QMAX( wused, minw );
    cw = QMAX( minw, cw );
    return TRUE;
}

void QTextDocument::setPlainText( const QString &text )
{
    clear();
    preferRichText = FALSE;
    oTextValid = TRUE;
    oText = text;

    int lastNl = 0;
    int nl = text.find( '\n' );
    if ( nl == -1 ) {
	lParag = createParag( this, lParag, 0 );
	if ( !fParag )
	    fParag = lParag;
	QString s = text;
	if ( !s.isEmpty() ) {
	    if ( s[ (int)s.length() - 1 ] == '\r' )
		s.remove( s.length() - 1, 1 );
	    lParag->append( s );
	}
    } else {
	for (;;) {
	    lParag = createParag( this, lParag, 0 );
	    if ( !fParag )
		fParag = lParag;
	    QString s = text.mid( lastNl, nl - lastNl );
	    if ( !s.isEmpty() ) {
		if ( s[ (int)s.length() - 1 ] == '\r' )
		    s.remove( s.length() - 1, 1 );
		lParag->append( s );
	    }
	    if ( nl == 0xffffff )
		break;
	    lastNl = nl + 1;
	    nl = text.find( '\n', nl + 1 );
	    if ( nl == -1 )
		nl = 0xffffff;
	}
    }
    if ( !lParag )
	lParag = fParag = createParag( this, 0, 0 );
}

struct Q_EXPORT QTextDocumentTag {
    QTextDocumentTag(){}
    QTextDocumentTag( const QString&n, const QStyleSheetItem* s, const QTextFormat& f )
	:name(n),style(s), format(f), alignment(Qt3::AlignAuto), direction(QChar::DirON),liststyle(QStyleSheetItem::ListDisc) {
	    wsm = QStyleSheetItem::WhiteSpaceNormal;
    }
    QString name;
    const QStyleSheetItem* style;
    QString anchorHref;
    QStyleSheetItem::WhiteSpaceMode wsm;
    QTextFormat format;
    int alignment : 16;
    int direction : 5;
    QStyleSheetItem::ListStyle liststyle;

    QTextDocumentTag(  const QTextDocumentTag& t ) {
	name = t.name;
	style = t.style;
	anchorHref = t.anchorHref;
	wsm = t.wsm;
	format = t.format;
	alignment = t.alignment;
	direction = t.direction;
	liststyle = t.liststyle;
    }
    QTextDocumentTag& operator=(const QTextDocumentTag& t) {
	name = t.name;
	style = t.style;
	anchorHref = t.anchorHref;
	wsm = t.wsm;
	format = t.format;
	alignment = t.alignment;
	direction = t.direction;
	liststyle = t.liststyle;
	return *this;
    }

#if defined(Q_FULL_TEMPLATE_INSTANTIATION)
    bool operator==( const QTextDocumentTag& ) const { return FALSE; }
#endif
};

#define NEWPAR       do{ if ( !hasNewPar ) curpar = createParag( this, curpar ); \
		    if ( curpar->isBr ) curpar->isBr = FALSE; \
		    hasNewPar = TRUE; \
		    curpar->setAlignment( curtag.alignment ); \
		    curpar->setDirection( (QChar::Direction)curtag.direction ); \
		    space = TRUE; \
		    QPtrVector<QStyleSheetItem> vec( (uint)tags.count() + 1); \
		    int i = 0; \
		    for ( QValueStack<QTextDocumentTag>::Iterator it = tags.begin(); it != tags.end(); ++it ) \
			vec.insert( i++, (*it).style ); \
		    vec.insert( i, curtag.style ); \
		    curpar->setStyleSheetItems( vec ); }while(FALSE)


void QTextDocument::setRichText( const QString &text, const QString &context )
{
    setTextFormat( Qt::RichText );
    if ( !context.isEmpty() )
	setContext( context );
    clear();
    fParag = lParag = createParag( this );
    setRichTextInternal( text );
}

static QStyleSheetItem::ListStyle chooseListStyle( const QStyleSheetItem *nstyle,
						   const QMap<QString, QString> &attr,
						   QStyleSheetItem::ListStyle curListStyle )
{
    if ( nstyle->name() == "ol" || nstyle->name() == "ul" ) {
	curListStyle = nstyle->listStyle();
	QMap<QString, QString>::ConstIterator it = attr.find( "type" );
	if ( it != attr.end() ) {
	    QString sl = *it;
	    if ( sl == "1" ) {
		curListStyle = QStyleSheetItem::ListDecimal;
	    } else if ( sl == "a" ) {
		curListStyle =  QStyleSheetItem::ListLowerAlpha;
	    } else if ( sl == "A" ) {
		curListStyle = QStyleSheetItem::ListUpperAlpha;
	    } else {
		sl = sl.lower();
		if ( sl == "square" )
		    curListStyle = QStyleSheetItem::ListSquare;
		else if ( sl == "disc" )
		    curListStyle = QStyleSheetItem::ListDisc;
		else if ( sl == "circle" )
		    curListStyle = QStyleSheetItem::ListCircle;
	    }
	}
    }
    return curListStyle;
}

void QTextDocument::setRichTextInternal( const QString &text )
{
    oTextValid = TRUE;
    oText = text;
    QTextParag* curpar = lParag;
    int pos = 0;
    QValueStack<QTextDocumentTag> tags;
    QTextDocumentTag initag( "", sheet_->item(""), *formatCollection()->defaultFormat() );
    QTextDocumentTag curtag = initag;
    bool space = TRUE;

    const QChar* doc = text.unicode();
    int length = text.length();
    bool hasNewPar = curpar->length() <= 1;
    QString lastClose;
    QString anchorName;
    while ( pos < length ) {
	if ( hasPrefix(doc, length, pos, '<' ) ){
	    if ( !hasPrefix( doc, length, pos+1, QChar('/') ) ) {
		// open tag
		QMap<QString, QString> attr;
		bool emptyTag = FALSE;
		QString tagname = parseOpenTag(doc, length, pos, attr, emptyTag);
		if ( tagname.isEmpty() )
		    continue; // nothing we could do with this, probably parse error

		if ( tagname == "title" ) {
		    QString title;
		    while ( pos < length ) {
			if ( hasPrefix( doc, length, pos, QChar('<') ) && hasPrefix( doc, length, pos+1, QChar('/') ) &&
			     parseCloseTag( doc, length, pos ) == "title" )
			    break;
			title += doc[ pos ];
			++pos;
		    }
		    attribs.replace( "title", title );
		}

		const QStyleSheetItem* nstyle = sheet_->item(tagname);

		if ( curtag.style->displayMode() == QStyleSheetItem::DisplayListItem ) {
// 		    if ( tagname == "br" ) {
// 			// our standard br emty-tag handling breaks
// 			// inside list items, we would get another
// 			// list item in this case. As workaround, fake
// 			// a new paragraph instead
// 			tagname = "p";
// 			nstyle = sheet_->item( tagname );
// 		    }
		    if ( nstyle )
			hasNewPar = FALSE; // we want empty paragraphs in this case
		}

		if ( nstyle ) {
		    // we might have to close some 'forgotten' tags
		    while ( !nstyle->allowedInContext( curtag.style ) ) {
			QString msg;
			msg.sprintf( "QText Warning: Document not valid ( '%s' not allowed in '%s' #%d)",
				     tagname.ascii(), curtag.style->name().ascii(), pos);
			sheet_->error( msg );
			if ( tags.isEmpty() )
			    break;
			curtag = tags.pop();
		    }

		    // special handling for p. We do not want to nest there for HTML compatibility
		    if ( nstyle->displayMode() == QStyleSheetItem::DisplayBlock ) {
			while ( curtag.style->name() == "p" ) {
			    if ( tags.isEmpty() )
				break;
			    curtag = tags.pop();
			}
		    }

		}

		QTextCustomItem* custom =  0;
		// some well-known empty tags
		if ( tagname == "br" ) {
		    emptyTag = TRUE;
		    hasNewPar = FALSE;
		    if ( curtag.style->displayMode() == QStyleSheetItem::DisplayListItem ) {
			// when linebreaking a list item, we do not
			// actually want a new list item but just a
			// new line. Fake this by pushing a paragraph
			// onto the stack
			tags.push( curtag );
 			curtag.name = tagname;
 			curtag.style = nstyle;
		    }
		    NEWPAR;
		    curpar->isBr = TRUE;
		    curpar->setAlignment( curtag.alignment );
		}  else if ( tagname == "hr" ) {
		    emptyTag = TRUE;
		    custom = sheet_->tag( tagname, attr, contxt, *factory_ , emptyTag, this );
		    NEWPAR;
		} else if ( tagname == "table" ) {
		    QTextFormat format = curtag.format.makeTextFormat(  nstyle, attr );
		    curpar->setAlignment( curtag.alignment );
		    custom = parseTable( attr, format, doc, length, pos, curpar );
		    (void)eatSpace( doc, length, pos );
		    emptyTag = TRUE;
		} else if ( tagname == "qt" ) {
		    for ( QMap<QString, QString>::Iterator it = attr.begin(); it != attr.end(); ++it ) {
			if ( it.key() == "bgcolor" ) {
			    QBrush *b = new QBrush( QColor( *it ) );
			    setPaper( b );
			} else if ( it.key() == "background" ) {
			    QImage img;
			    const QMimeSource* m = factory_->data( *it, contxt );
			    if ( !m ) {
				qWarning("QRichText: no mimesource for %s", (*it).latin1() );
			    } else {
				if ( !QImageDrag::decode( m, img ) ) {
				    qWarning("QTextImage: cannot decode %s", (*it).latin1() );
				}
			    }
			    if ( !img.isNull() ) {
				QPixmap pm;
				pm.convertFromImage( img );
				QBrush *b = new QBrush( QColor(), pm );
				setPaper( b );
			    }
			} else if ( it.key() == "text" ) {
			    QColor c( *it );
			    if ( formatCollection()->defaultFormat()->color() != c ) {
				QDict<QTextFormat> formats = formatCollection()->dict();
				QDictIterator<QTextFormat> it( formats );
				while ( it.current() ) {
				    if ( it.current() == formatCollection()->defaultFormat() ) {
					++it;
					continue;
				    }
				    it.current()->setColor( c );
				    ++it;
				}
				formatCollection()->defaultFormat()->setColor( c );
				curtag.format.setColor( c );
			    }
			} else if ( it.key() == "link" ) {
			    linkColor = QColor( *it );
			} else if ( it.key() == "title" ) {
			    attribs.replace( it.key(), *it );
			}
		    }
		} else {
		    custom = sheet_->tag( tagname, attr, contxt, *factory_ , emptyTag, this );
		}

		if ( !nstyle && !custom ) // we have no clue what this tag could be, ignore it
		    continue;

		if ( custom ) {
		    int index = curpar->length() - 1;
		    if ( index < 0 )
			index = 0;
		    QTextFormat format = curtag.format.makeTextFormat( nstyle, attr );
		    curpar->append( QChar('*') );
		    curpar->setFormat( index, 1, &format );
		    curpar->at( index )->setCustomItem( custom );
		    if ( !curtag.anchorHref.isEmpty() )
 			curpar->at(index)->setAnchor( QString::null, curtag.anchorHref );
 		    if ( !anchorName.isEmpty()  ) {
 			curpar->at(index)->setAnchor( anchorName, curpar->at(index)->anchorHref() );
 			anchorName = QString::null;
 		    }
		    registerCustomItem( custom, curpar );
		    hasNewPar = FALSE;
		} else if ( !emptyTag ) {
		    // ignore whitespace for inline elements if there was already one
		    if ( nstyle->whiteSpaceMode() == QStyleSheetItem::WhiteSpaceNormal
			 && ( space || nstyle->displayMode() != QStyleSheetItem::DisplayInline ) )
			eatSpace( doc, length, pos );

		    // if we do nesting, push curtag on the stack,
		    // otherwise reinint curag.
 		    if ( nstyle != curtag.style || nstyle->selfNesting() ) {
			tags.push( curtag );
		    } else {
			if ( !tags.isEmpty() )
			    curtag = tags.top();
			else
			    curtag = initag;
		    }

		    const QStyleSheetItem* ostyle = curtag.style;

		    curtag.name = tagname;
		    curtag.style = nstyle;
		    curtag.name = tagname;
		    curtag.style = nstyle;
		    if ( nstyle->whiteSpaceMode() != QStyleSheetItem::WhiteSpaceNormal )
			curtag.wsm = nstyle->whiteSpaceMode();
		    curtag.liststyle = chooseListStyle( nstyle, attr, curtag.liststyle );
		    curtag.format = curtag.format.makeTextFormat( nstyle, attr );
		    if ( nstyle->isAnchor() ) {
			if ( !anchorName.isEmpty() )
			    anchorName += "#" + attr["name"];
			else
			    anchorName = attr["name"];
			curtag.anchorHref = attr["href"];
		    }

		    if ( nstyle->alignment() != QStyleSheetItem::Undefined )
			curtag.alignment = nstyle->alignment();

		    if ( ostyle->displayMode() == QStyleSheetItem::DisplayListItem &&
			 curpar->length() <= 1
			 && nstyle->displayMode() == QStyleSheetItem::DisplayBlock  ) {
			// do not do anything, we reuse the paragraph we have
		    } else if ( nstyle->displayMode() != QStyleSheetItem::DisplayInline && nstyle->displayMode() != QStyleSheetItem::DisplayNone ) {
			NEWPAR;
		    }

		    if ( curtag.style->displayMode() == QStyleSheetItem::DisplayListItem ) {
			curpar->setListStyle( curtag.liststyle );
			if ( attr.find( "value" ) != attr.end() )
			    curpar->setListValue( (*attr.find( "value" )).toInt() );
		    }

		    if ( nstyle->displayMode() != QStyleSheetItem::DisplayInline )
			curpar->setFormat( &curtag.format );

		    if ( attr.contains( "align" ) &&
			 ( curtag.name == "p" ||
			   curtag.name == "div" ||
			   curtag.name == "li" ||
			   curtag.name[ 0 ] == 'h' ) ) {
			QString align = attr["align"];
			if ( align == "center" )
			    curtag.alignment = Qt::AlignCenter;
			else if ( align == "right" )
			    curtag.alignment = Qt::AlignRight;
			else if ( align == "justify" )
			    curtag.alignment = Qt3::AlignJustify;
		    }
		    if ( attr.contains( "dir" ) &&
			 ( curtag.name == "p" ||
			   curtag.name == "div" ||
			   curtag.name == "li" ||
			   curtag.name[ 0 ] == 'h' ) ) {
			QString dir = attr["dir"];
			if ( dir == "rtl" )
			    curtag.direction = QChar::DirR;
			else if ( dir == "ltr" )
			    curtag.direction = QChar::DirL;
		    }
		    if ( nstyle->displayMode() != QStyleSheetItem::DisplayInline ) {
			curpar->setAlignment( curtag.alignment );
			curpar->setDirection( (QChar::Direction)curtag.direction );
		    }
		}
	    } else {
		QString tagname = parseCloseTag( doc, length, pos );
		lastClose = tagname;
		if ( tagname.isEmpty() )
		    continue; // nothing we could do with this, probably parse error
		if ( !sheet_->item( tagname ) ) // ignore unknown tags
		    continue;


		// we close a block item. Since the text may continue, we need to have a new paragraph
		bool needNewPar = curtag.style->displayMode() == QStyleSheetItem::DisplayBlock;

		if ( curtag.style->displayMode() == QStyleSheetItem::DisplayListItem ) {
 		    needNewPar = TRUE;
		    hasNewPar = FALSE; // we want empty paragraphs in this case
		}

		// html slopiness: handle unbalanched tag closing
		while ( curtag.name != tagname ) {
		    QString msg;
		    msg.sprintf( "QText Warning: Document not valid ( '%s' not closed before '%s' #%d)",
				 curtag.name.ascii(), tagname.ascii(), pos);
		    sheet_->error( msg );
		    if ( tags.isEmpty() )
			break;
		    curtag = tags.pop();
		}


		// close the tag
		if ( !tags.isEmpty() )
		    curtag = tags.pop();
		else
		    curtag = initag;

 		if ( needNewPar ) {
		    if ( curtag.style->displayMode() == QStyleSheetItem::DisplayListItem ) {
			tags.push( curtag );
			curtag.name = "p";
			curtag.style = sheet_->item( curtag.name ); // a list item continues, use p for that
		    }
		    NEWPAR;
		}
	    }
	} else {
	    // normal contents
	    QString s;
	    QChar c;
	    while ( pos < length && !hasPrefix(doc, length, pos, QChar('<') ) ){
		QStyleSheetItem::WhiteSpaceMode wsm = curtag.wsm;
		if ( s.length() > 4096 )
		    wsm =  (QStyleSheetItem::WhiteSpaceMode)QStyleSheetItem_WhiteSpaceNormalWithNewlines;

		c = parseChar( doc, length, pos, wsm );

		if ( c == '\n' ) // happens only in whitespacepre-mode or with WhiteSpaceNormalWithNewlines.
 		    break;  // we want a new line in this case

		bool c_isSpace = c.isSpace() && c.unicode() != 0x00a0U &&
		   curtag.wsm != QStyleSheetItem_WhiteSpaceNoCompression;

		if ( curtag.wsm == QStyleSheetItem::WhiteSpaceNormal && c_isSpace && space )
		    continue;
		if ( c == '\r' )
		    continue;
		space = c_isSpace;
		s += c;
	    }
	    if ( !s.isEmpty() && curtag.style->displayMode() != QStyleSheetItem::DisplayNone ) {
		hasNewPar = FALSE;
		int index = curpar->length() - 1;
		if ( index < 0 )
		    index = 0;
		curpar->append( s );
		QTextFormat* f = formatCollection()->format( &curtag.format );
		curpar->setFormat( index, s.length(), f, FALSE ); // do not use collection because we have done that already
		f->ref += s.length() -1; // that what friends are for...
		if ( !curtag.anchorHref.isEmpty() ) {
		    for ( int i = 0; i < int(s.length()); i++ )
			curpar->at(index + i)->setAnchor( QString::null, curtag.anchorHref );
		}
		if ( !anchorName.isEmpty()  ) {
		    curpar->at(index)->setAnchor( anchorName, curpar->at(index)->anchorHref() );
		    anchorName = QString::null;
		}
	    }
	    if ( c == '\n' ) { // happens in WhiteSpacePre mode
		hasNewPar = FALSE;
		tags.push( curtag );
		NEWPAR;
		curtag = tags.pop();
	    }
	}
    }

    if ( hasNewPar && curpar != fParag ) {
	// cleanup unused last paragraphs
	curpar = curpar->p;
	delete curpar->n;
    }

    if ( !anchorName.isEmpty()  ) {
	curpar->at(curpar->length() - 1)->setAnchor( anchorName, curpar->at( curpar->length() - 1 )->anchorHref() );
	anchorName = QString::null;
    }
}

void QTextDocument::setText( const QString &text, const QString &context )
{
    focusIndicator.parag = 0;
    selections.clear();
    if ( txtFormat == Qt::AutoText && QStyleSheet::mightBeRichText( text ) ||
	 txtFormat == Qt::RichText )
	setRichText( text, context );
    else
	setPlainText( text );
}

QString QTextDocument::plainText( QTextParag *p ) const
{
    if ( !p ) {
	QString buffer;
	QString s;
	QTextParag *p = fParag;
	while ( p ) {
	    if ( !p->mightHaveCustomItems ) {
		s = p->string()->toString();
	    } else {
		for ( int i = 0; i < p->length() - 1; ++i ) {
		    if ( p->at( i )->isCustom() ) {
			if ( p->at( i )->customItem()->isNested() ) {
			    s += "\n";
			    QTextTable *t = (QTextTable*)p->at( i )->customItem();
			    QPtrList<QTextTableCell> cells = t->tableCells();
			    for ( QTextTableCell *c = cells.first(); c; c = cells.next() )
				s += c->richText()->plainText() + "\n";
			    s += "\n";
			}
		    } else {
			s += p->at( i )->c;
		    }
		}
	    }
	    s.remove( s.length() - 1, 1 );
	    if ( p->next() )
		s += "\n";
	    buffer += s;
	    p = p->next();
	}
	return buffer;
    } else {
	return p->string()->toString();
    }
}

static QString align_to_string( const QString &tag, int a )
{
    if ( tag == "p" || tag == "li" || ( tag[0] == 'h' && tag[1].isDigit() ) ) {
	if ( a & Qt::AlignRight )
	    return " align=\"right\"";
	if ( a & Qt::AlignCenter )
	    return " align=\"center\"";
	if ( a & Qt3::AlignJustify )
	    return " align=\"justify\"";
    }
    return "";
}

static QString direction_to_string( const QString &tag, int d )
{
    if ( d != QChar::DirON &&
	 ( tag == "p" || tag == "div" || tag == "li" || ( tag[0] == 'h' && tag[1].isDigit() ) ) )
	return ( d == QChar::DirL? " dir=\"ltr\"" : " dir=\"rtl\"" );
    return "";
}

QString QTextDocument::richText( QTextParag *p ) const
{
    QString s,n;
    if ( !p ) {
	p = fParag;
	QPtrVector<QStyleSheetItem> lastItems, items;
	while ( p ) {
	    items = p->styleSheetItems();
	    if ( items.size() ) {
		QStyleSheetItem *item = items[ items.size() - 1 ];
		items.resize( items.size() - 1 );
		if ( items.size() > lastItems.size() ) {
		    for ( int i = lastItems.size(); i < (int)items.size(); ++i ) {
			n = items[i]->name();
			if ( n.isEmpty() || n == "li" )
			    continue;
			s += "<" + n + align_to_string( n, p->alignment() ) + ">";
		    }
		} else {
		    QString end;
		    for ( int i = items.size(); i < (int)lastItems.size(); ++i ) {
			n = lastItems[i]->name();
			if ( n.isEmpty() || n == "li" || n == "br" )
			    continue;
			end.prepend( "</" + lastItems[ i ]->name() + ">" );
		    }
		    s += end;
		}
		lastItems = items;
		n = item->name();
		if ( n == "li" && p->listValue() != -1 ) {
		    s += "<li value=\"" + QString::number( p->listValue() ) + "\">";
		} else {
		    QString ps = p->richText();
		    if ( ps.isEmpty() )
			s += "<br>"; // empty paragraph
		    else if ( !n.isEmpty() ) {
			s += "<" + n + align_to_string( n, p->alignment() )
			     + direction_to_string( n, p->direction() )  + ">" + ps;
			if ( n != "li" && n != "br")
			    s += "</" + n + ">";
		    } else
			s += ps;
		}
	    } else {
		QString end;
		for ( int i = 0; i < (int)lastItems.size(); ++i ) {
		    QString n = lastItems[i]->name();
		    if ( n.isEmpty() || n == "li" || n == "br" )
			continue;
		    end.prepend( "</" + n + ">" );
		}
		s += end;
		QString ps = p->richText();
		if ( ps.isEmpty() )
		    s += "<br>"; // empty paragraph
		else
		    s += "<p" + align_to_string( "p", p->alignment() ) + direction_to_string( "p", p->direction() )
			 + ">" + ps + "</p>";
		lastItems = items;
	    }
	    if ( ( p = p->next() ) )
		  s += '\n';
	}
    } else {
	s = p->richText();
    }

    return s;
}

QString QTextDocument::text() const
{
    if ( plainText().simplifyWhiteSpace().isEmpty() )
	return QString("");
    if ( txtFormat == Qt::AutoText && preferRichText || txtFormat == Qt::RichText )
	return richText();
    return plainText( 0 );
}

QString QTextDocument::text( int parag ) const
{
    QTextParag *p = paragAt( parag );
    if ( !p )
	return QString::null;

    if ( txtFormat == Qt::AutoText && preferRichText || txtFormat == Qt::RichText )
	return richText( p );
    else
	return plainText( p );
}

void QTextDocument::invalidate()
{
    QTextParag *s = fParag;
    while ( s ) {
	s->invalidate( 0 );
	s = s->next();
    }
}

void QTextDocument::selectionStart( int id, int &paragId, int &index )
{
    QMap<int, QTextDocumentSelection>::Iterator it = selections.find( id );
    if ( it == selections.end() )
	return;
    QTextDocumentSelection &sel = *it;
    paragId = !sel.swapped ? sel.startCursor.parag()->paragId() : sel.endCursor.parag()->paragId();
    index = !sel.swapped ? sel.startCursor.index() : sel.endCursor.index();
}

QTextCursor QTextDocument::selectionStartCursor( int id)
{
    QMap<int, QTextDocumentSelection>::Iterator it = selections.find( id );
    if ( it == selections.end() )
	return QTextCursor( this );
    QTextDocumentSelection &sel = *it;
    if ( sel.swapped )
	return sel.endCursor;
    return sel.startCursor;
}

QTextCursor QTextDocument::selectionEndCursor( int id)
{
    QMap<int, QTextDocumentSelection>::Iterator it = selections.find( id );
    if ( it == selections.end() )
	return QTextCursor( this );
    QTextDocumentSelection &sel = *it;
    if ( !sel.swapped )
	return sel.endCursor;
    return sel.startCursor;
}

void QTextDocument::selectionEnd( int id, int &paragId, int &index )
{
    QMap<int, QTextDocumentSelection>::Iterator it = selections.find( id );
    if ( it == selections.end() )
	return;
    QTextDocumentSelection &sel = *it;
    paragId = sel.swapped ? sel.startCursor.parag()->paragId() : sel.endCursor.parag()->paragId();
    index = sel.swapped ? sel.startCursor.index() : sel.endCursor.index();
}

QTextParag *QTextDocument::selectionStart( int id )
{
    QMap<int, QTextDocumentSelection>::Iterator it = selections.find( id );
    if ( it == selections.end() )
	return 0;
    QTextDocumentSelection &sel = *it;
    if ( sel.startCursor.parag()->paragId() <  sel.endCursor.parag()->paragId() )
	return sel.startCursor.parag();
    return sel.endCursor.parag();
}

QTextParag *QTextDocument::selectionEnd( int id )
{
    QMap<int, QTextDocumentSelection>::Iterator it = selections.find( id );
    if ( it == selections.end() )
	return 0;
    QTextDocumentSelection &sel = *it;
    if ( sel.startCursor.parag()->paragId() >  sel.endCursor.parag()->paragId() )
	return sel.startCursor.parag();
    return sel.endCursor.parag();
}

void QTextDocument::addSelection( int id )
{
    nSelections = QMAX( nSelections, id + 1 );
}

static void setSelectionEndHelper( int id, QTextDocumentSelection &sel, QTextCursor &start, QTextCursor &end )
{
    QTextCursor c1 = start;
    QTextCursor c2 = end;
    if ( sel.swapped ) {
	c1 = end;
	c2 = start;
    }

    c1.parag()->removeSelection( id );
    c2.parag()->removeSelection( id );
    if ( c1.parag() != c2.parag() ) {
	c1.parag()->setSelection( id, c1.index(), c1.parag()->length() - 1 );
	c2.parag()->setSelection( id, 0, c2.index() );
    } else {
	c1.parag()->setSelection( id, QMIN( c1.index(), c2.index() ), QMAX( c1.index(), c2.index() ) );
    }

    sel.startCursor = start;
    sel.endCursor = end;
    if ( sel.startCursor.parag() == sel.endCursor.parag() )
	sel.swapped = sel.startCursor.index() > sel.endCursor.index();
}

bool QTextDocument::setSelectionEnd( int id, QTextCursor *cursor )
{
    QMap<int, QTextDocumentSelection>::Iterator it = selections.find( id );
    if ( it == selections.end() )
	return FALSE;
    QTextDocumentSelection &sel = *it;

    QTextCursor start = sel.startCursor;
    QTextCursor end = *cursor;

    if ( start == end ) {
	removeSelection( id );
	setSelectionStart( id, cursor );
	return TRUE;
    }

    if ( sel.endCursor.parag() == end.parag() ) {
	setSelectionEndHelper( id, sel, start, end );
	return TRUE;
    }

    bool inSelection = FALSE;
    QTextCursor c( this );
    QTextCursor tmp = sel.startCursor;
    if ( sel.swapped )
	tmp = sel.endCursor;
    tmp.restoreState();
    QTextCursor tmp2 = *cursor;
    tmp2.restoreState();
    c.setParag( tmp.parag()->paragId() < tmp2.parag()->paragId() ? tmp.parag() : tmp2.parag() );
    QTextCursor old;
    bool hadStart = FALSE;
    bool hadEnd = FALSE;
    bool hadStartParag = FALSE;
    bool hadEndParag = FALSE;
    bool hadOldStart = FALSE;
    bool hadOldEnd = FALSE;
    bool leftSelection = FALSE;
    sel.swapped = FALSE;
    for ( ;; ) {
	if ( c == start )
	    hadStart = TRUE;
	if ( c == end )
	    hadEnd = TRUE;
	if ( c.parag() == start.parag() )
	    hadStartParag = TRUE;
	if ( c.parag() == end.parag() )
	    hadEndParag = TRUE;
	if ( c == sel.startCursor )
	    hadOldStart = TRUE;
	if ( c == sel.endCursor )
	    hadOldEnd = TRUE;

	if ( !sel.swapped &&
	     ( hadEnd && !hadStart ||
	       hadEnd && hadStart && start.parag() == end.parag() && start.index() > end.index() ) )
	    sel.swapped = TRUE;

	if ( c == end && hadStartParag ||
	     c == start && hadEndParag ) {
	    QTextCursor tmp = c;
	    tmp.restoreState();
	    if ( tmp.parag() != c.parag() ) {
		int sstart = tmp.parag()->selectionStart( id );
		tmp.parag()->removeSelection( id );
		tmp.parag()->setSelection( id, sstart, tmp.index() );
	    }
	}

	if ( inSelection &&
	     ( c == end && hadStart || c == start && hadEnd ) )
	     leftSelection = TRUE;
	else if ( !leftSelection && !inSelection && ( hadStart || hadEnd ) )
	    inSelection = TRUE;

	bool noSelectionAnymore = hadOldStart && hadOldEnd && leftSelection && !inSelection && !c.parag()->hasSelection( id ) && c.atParagEnd();
	c.parag()->removeSelection( id );
	if ( inSelection ) {
	    if ( c.parag() == start.parag() && start.parag() == end.parag() ) {
		c.parag()->setSelection( id, QMIN( start.index(), end.index() ), QMAX( start.index(), end.index() ) );
	    } else if ( c.parag() == start.parag() && !hadEndParag ) {
		c.parag()->setSelection( id, start.index(), c.parag()->length() - 1 );
	    } else if ( c.parag() == end.parag() && !hadStartParag ) {
		c.parag()->setSelection( id, end.index(), c.parag()->length() - 1 );
	    } else if ( c.parag() == end.parag() && hadEndParag ) {
		c.parag()->setSelection( id, 0, end.index() );
	    } else if ( c.parag() == start.parag() && hadStartParag ) {
		c.parag()->setSelection( id, 0, start.index() );
	    } else {
		c.parag()->setSelection( id, 0, c.parag()->length() - 1 );
	    }
	}

	if ( leftSelection )
	    inSelection = FALSE;

	old = c;
	c.gotoNextLetter();
	if ( old == c || noSelectionAnymore )
	    break;
    }

    if ( !sel.swapped )
	sel.startCursor.parag()->setSelection( id, sel.startCursor.index(), sel.startCursor.parag()->length() - 1 );

    sel.startCursor = start;
    sel.endCursor = end;
    if ( sel.startCursor.parag() == sel.endCursor.parag() )
	sel.swapped = sel.startCursor.index() > sel.endCursor.index();

    setSelectionEndHelper( id, sel, start, end );

    return TRUE;
}

void QTextDocument::selectAll( int id )
{
    removeSelection( id );

    QTextDocumentSelection sel;
    sel.swapped = FALSE;
    QTextCursor c( this );

    c.setParag( fParag );
    c.setIndex( 0 );
    sel.startCursor = c;

    c.setParag( lParag );
    c.setIndex( lParag->length() - 1 );
    sel.endCursor = c;

    QTextParag *p = fParag;
    while ( p ) {
	p->setSelection( id, 0, p->length() - 1 );
	for ( int i = 0; i < (int)p->length(); ++i ) {
	    if ( p->at( i )->isCustom() && p->at( i )->customItem()->isNested() ) {
		QTextTable *t = (QTextTable*)p->at( i )->customItem();
		QPtrList<QTextTableCell> tableCells = t->tableCells();
		for ( QTextTableCell *c = tableCells.first(); c; c = tableCells.next() )
		    c->richText()->selectAll( id );
	    }
	}
	p = p->next();
    }

    selections.insert( id, sel );
}

bool QTextDocument::removeSelection( int id )
{
    QMap<int, QTextDocumentSelection>::Iterator it = selections.find( id );
    if ( it == selections.end() )
	return FALSE;

    QTextDocumentSelection &sel = *it;

    if ( sel.startCursor == sel.endCursor ) {
	selections.remove( id );
	return TRUE;
    }

    if ( !mightHaveCustomItems ) {
	QTextCursor start = sel.startCursor;
	QTextCursor end = sel.endCursor;
	if ( sel.swapped ) {
	    start = sel.endCursor;
	    end = sel.startCursor;
	}

	for ( QTextParag *p = start.parag(); p; p = p->next() ) {
	    p->removeSelection( id );
	    if ( p == end.parag() )
		break;
	}

	selections.remove( id );
	return TRUE;
    }

    QTextCursor c( this );
    QTextCursor tmp = sel.startCursor;
    if ( sel.swapped )
	tmp = sel.endCursor;
    tmp.restoreState();
    c.setParag( tmp.parag() );
    QTextCursor old;
    bool hadStart = FALSE;
    bool hadEnd = FALSE;
    QTextParag *lastParag = 0;
    bool leftSelection = FALSE;
    bool inSelection = FALSE;
    sel.swapped = FALSE;
    for ( ;; ) {
	if ( c.parag() == sel.startCursor.parag() )
	    hadStart = TRUE;
	if ( c.parag() == sel.endCursor.parag() )
	    hadEnd = TRUE;

	if ( inSelection &&
	     ( c == sel.endCursor && hadStart || c == sel.startCursor && hadEnd ) )
	     leftSelection = TRUE;
	else if ( !leftSelection && !inSelection && ( c.parag() == sel.startCursor.parag() || c.parag() == sel.endCursor.parag() ) )
	    inSelection = TRUE;

	bool noSelectionAnymore = leftSelection && !inSelection && !c.parag()->hasSelection( id ) && c.atParagEnd();

	if ( lastParag != c.parag() )
	    c.parag()->removeSelection( id );

	old = c;
	lastParag = c.parag();
	c.gotoNextLetter();
	if ( old == c || noSelectionAnymore )
	    break;
    }

    selections.remove( id );
    return TRUE;
}

QString QTextDocument::selectedText( int id, bool withCustom ) const
{
    // ######## TODO: look at textFormat() and return rich text or plain text (like the text() method!)
    QMap<int, QTextDocumentSelection>::ConstIterator it = selections.find( id );
    if ( it == selections.end() )
	return QString::null;

    QTextDocumentSelection sel = *it;


    QTextCursor c1 = sel.startCursor;
    QTextCursor c2 = sel.endCursor;
    if ( sel.swapped ) {
	c2 = sel.startCursor;
	c1 = sel.endCursor;
    }

    /* 3.0.3 improvement: Make it possible to get a reasonable
       selection inside a table.  This approach is very conservative:
       make sure that both cursors have the same depth level and point
       to paragraphs within the same text document.

       Meaning if you select text in two table cells, you will get the
       entire table. This is still far better than the 3.0.2, where
       you always got the entire table.

       ### Fix this properly for 3.0.4.
     */
    while ( c2.nestedDepth() > c1.nestedDepth() )
	c2.oneUp();
    while ( c1.nestedDepth() > c2.nestedDepth() )
	c1.oneUp();
    while ( c1.nestedDepth() && c2.nestedDepth() &&
	    c1.parag()->document() != c2.parag()->document() ) {
	c1.oneUp();
	c2.oneUp();
    }
    // do not trust sel_swapped with tables. Fix this properly for 3.0.4 as well
    if ( c1.parag()->paragId() > c2.parag()->paragId() ||
	 (c1.parag() == c2.parag() && c1.index() > c2.index() ) ) {
	QTextCursor tmp = c1;
	c2 = c1;
	c1 = tmp;
    }

    // end selection 3.0.3 improvement


    if ( c1.parag() == c2.parag() ) {
	QString s;
	QTextParag *p = c1.parag();
	int end = c2.index();
	if ( p->at( QMAX( 0, end - 1 ) )->isCustom() )
	    ++end;
	if ( !withCustom || !p->mightHaveCustomItems ) {
	    s += p->string()->toString().mid( c1.index(), end - c1.index() );
	} else {
	    for ( int i = c1.index(); i < end; ++i ) {
		if ( p->at( i )->isCustom() ) {
		    if ( p->at( i )->customItem()->isNested() ) {
			s += "\n";
			QTextTable *t = (QTextTable*)p->at( i )->customItem();
			QPtrList<QTextTableCell> cells = t->tableCells();
			for ( QTextTableCell *c = cells.first(); c; c = cells.next() )
			    s += c->richText()->plainText() + "\n";
			s += "\n";
		    }
		} else {
		    s += p->at( i )->c;
		}
	    }
	}
	return s;
    }

    QString s;
    QTextParag *p = c1.parag();
    int start = c1.index();
    while ( p ) {
	int end = p == c2.parag() ? c2.index() : p->length() - 1;
	if ( p == c2.parag() && p->at( QMAX( 0, end - 1 ) )->isCustom() )
	    ++end;
	if ( !withCustom || !p->mightHaveCustomItems ) {
	    s += p->string()->toString().mid( start, end - start );
	    if ( p != c2.parag() )
		s += "\n";
	} else {
	    for ( int i = start; i < end; ++i ) {
		if ( p->at( i )->isCustom() ) {
		    if ( p->at( i )->customItem()->isNested() ) {
			s += "\n";
			QTextTable *t = (QTextTable*)p->at( i )->customItem();
			QPtrList<QTextTableCell> cells = t->tableCells();
			for ( QTextTableCell *c = cells.first(); c; c = cells.next() )
			    s += c->richText()->plainText() + "\n";
			s += "\n";
		    }
		} else {
		    s += p->at( i )->c;
		}
	    }
	}
	start = 0;
	if ( p == c2.parag() )
	    break;
	p = p->next();
    }
    return s;
}

void QTextDocument::setFormat( int id, QTextFormat *f, int flags )
{
    QMap<int, QTextDocumentSelection>::ConstIterator it = selections.find( id );
    if ( it == selections.end() )
	return;

    QTextDocumentSelection sel = *it;

    QTextCursor c1 = sel.startCursor;
    QTextCursor c2 = sel.endCursor;
    if ( sel.swapped ) {
	c2 = sel.startCursor;
	c1 = sel.endCursor;
    }

    c2.restoreState();
    c1.restoreState();

    if ( c1.parag() == c2.parag() ) {
	c1.parag()->setFormat( c1.index(), c2.index() - c1.index(), f, TRUE, flags );
	return;
    }

    c1.parag()->setFormat( c1.index(), c1.parag()->length() - c1.index(), f, TRUE, flags );
    QTextParag *p = c1.parag()->next();
    while ( p && p != c2.parag() ) {
	p->setFormat( 0, p->length(), f, TRUE, flags );
	p = p->next();
    }
    c2.parag()->setFormat( 0, c2.index(), f, TRUE, flags );
}

void QTextDocument::copySelectedText( int id )
{
#ifndef QT_NO_CLIPBOARD
    if ( !hasSelection( id ) )
	return;

    QApplication::clipboard()->setText( selectedText( id ) );
#endif
}

void QTextDocument::removeSelectedText( int id, QTextCursor *cursor )
{
    QMap<int, QTextDocumentSelection>::Iterator it = selections.find( id );
    if ( it == selections.end() )
	return;

    QTextDocumentSelection sel = *it;

    QTextCursor c1 = sel.startCursor;
    QTextCursor c2 = sel.endCursor;
    if ( sel.swapped ) {
	c2 = sel.startCursor;
	c1 = sel.endCursor;
    }

    // ### no support for editing tables yet
    if ( c1.nestedDepth() || c2.nestedDepth() )
	return;

    c2.restoreState();
    c1.restoreState();

    *cursor = c1;
    removeSelection( id );

    if ( c1.parag() == c2.parag() ) {
	c1.parag()->remove( c1.index(), c2.index() - c1.index() );
	return;
    }

    if ( c1.parag() == fParag && c1.index() == 0 &&
	 c2.parag() == lParag && c2.index() == lParag->length() - 1 )
	cursor->setValid( FALSE );

    bool didGoLeft = FALSE;
    if (  c1.index() == 0 && c1.parag() != fParag ) {
	cursor->gotoPreviousLetter();
	if ( cursor->isValid() )
	    didGoLeft = TRUE;
    }

    c1.parag()->remove( c1.index(), c1.parag()->length() - 1 - c1.index() );
    QTextParag *p = c1.parag()->next();
    int dy = 0;
    QTextParag *tmp;
    while ( p && p != c2.parag() ) {
	tmp = p->next();
	dy -= p->rect().height();
	delete p;
	p = tmp;
    }
    c2.parag()->remove( 0, c2.index() );
    while ( p ) {
	p->move( dy );
	p->invalidate( 0 );
	p->setEndState( -1 );
	p = p->next();
    }

    c1.parag()->join( c2.parag() );

    if ( didGoLeft )
	cursor->gotoNextLetter();
}

void QTextDocument::indentSelection( int id )
{
    QMap<int, QTextDocumentSelection>::Iterator it = selections.find( id );
    if ( it == selections.end() )
	return;

    QTextDocumentSelection sel = *it;
    QTextParag *startParag = sel.startCursor.parag();
    QTextParag *endParag = sel.endCursor.parag();
    if ( sel.endCursor.parag()->paragId() < sel.startCursor.parag()->paragId() ) {
	endParag = sel.startCursor.parag();
	startParag = sel.endCursor.parag();
    }

    QTextParag *p = startParag;
    while ( p && p != endParag ) {
	p->indent();
	p = p->next();
    }
}

void QTextDocument::addCommand( QTextCommand *cmd )
{
    commandHistory->addCommand( cmd );
}

QTextCursor *QTextDocument::undo( QTextCursor *c )
{
    return commandHistory->undo( c );
}

QTextCursor *QTextDocument::redo( QTextCursor *c )
{
    return commandHistory->redo( c );
}

bool QTextDocument::find( const QString &expr, bool cs, bool wo, bool forward,
			  int *parag, int *index, QTextCursor *cursor )
{
    QTextParag *p = forward ? fParag : lParag;
    if ( parag )
	p = paragAt( *parag );
    else if ( cursor )
	p = cursor->parag();
    bool first = TRUE;

    while ( p ) {
	QString s = p->string()->toString();
	s.remove( s.length() - 1, 1 ); // get rid of trailing space
	int start = forward ? 0 : s.length() - 1;
	if ( first && index )
	    start = *index;
	else if ( first )
	    start = cursor->index();
	if ( !forward && first ) {
	    start -= expr.length() + 1;
	    if ( start < 0 ) {
		first = FALSE;
		p = p->prev();
		continue;
	    }
	}
	first = FALSE;

	for ( ;; ) {
	    int res = forward ? s.find( expr, start, cs ) : s.findRev( expr, start, cs );
	    if ( res == -1 )
		break;

	    bool ok = TRUE;
	    if ( wo ) {
		int end = res + expr.length();
		if ( ( res == 0 || s[ res - 1 ].isSpace() || s[ res - 1 ].isPunct() ) &&
		     ( end == (int)s.length() || s[ end ].isSpace() || s[ end ].isPunct() ) )
		    ok = TRUE;
		else
		    ok = FALSE;
	    }
	    if ( ok ) {
		cursor->setParag( p );
		cursor->setIndex( res );
		setSelectionStart( Standard, cursor );
		cursor->setIndex( res + expr.length() );
		setSelectionEnd( Standard, cursor );
		if ( parag )
		    *parag = p->paragId();
		if ( index )
		    *index = res;
		return TRUE;
	    }
	    if ( forward ) {
		start = res + 1;
	    } else {
		if ( res == 0 )
		    break;
		start = res - 1;
	    }
	}
	p = forward ? p->next() : p->prev();
    }

    return FALSE;
}

void QTextDocument::setTextFormat( Qt::TextFormat f )
{
    txtFormat = f;
    if ( txtFormat == Qt::RichText && fParag && fParag == lParag && fParag->length() <= 1 ) {
	QPtrVector<QStyleSheetItem> v = fParag->styleSheetItems();
	v.resize( v.size() + 1 );
	v.insert( v.size() - 1, styleSheet()->item( "p" ) );
	fParag->setStyleSheetItems( v );
    }

}

Qt::TextFormat QTextDocument::textFormat() const
{
    return txtFormat;
}

bool QTextDocument::inSelection( int selId, const QPoint &pos ) const
{
    QMap<int, QTextDocumentSelection>::ConstIterator it = selections.find( selId );
    if ( it == selections.end() )
	return FALSE;

    QTextDocumentSelection sel = *it;
    QTextParag *startParag = sel.startCursor.parag();
    QTextParag *endParag = sel.endCursor.parag();
    if ( sel.startCursor.parag() == sel.endCursor.parag() &&
	 sel.startCursor.parag()->selectionStart( selId ) == sel.endCursor.parag()->selectionEnd( selId ) )
	return FALSE;
    if ( sel.endCursor.parag()->paragId() < sel.startCursor.parag()->paragId() ) {
	endParag = sel.startCursor.parag();
	startParag = sel.endCursor.parag();
    }

    QTextParag *p = startParag;
    while ( p ) {
	if ( p->rect().contains( pos ) ) {
	    bool inSel = FALSE;
	    int selStart = p->selectionStart( selId );
	    int selEnd = p->selectionEnd( selId );
	    int y = 0;
	    int h = 0;
	    for ( int i = 0; i < p->length(); ++i ) {
		if ( i == selStart )
		    inSel = TRUE;
		if ( i == selEnd )
		    break;
		if ( p->at( i )->lineStart ) {
		    y = (*p->lineStarts.find( i ))->y;
		    h = (*p->lineStarts.find( i ))->h;
		}
		if ( pos.y() - p->rect().y() >= y && pos.y() - p->rect().y() <= y + h ) {
		    if ( inSel && pos.x() >= p->at( i )->x &&
			 pos.x() <= p->at( i )->x + p->at( i )->format()->width( p->at( i )->c ) )
			return TRUE;
		}
	    }
	}
	if ( pos.y() < p->rect().y() )
	    break;
	if ( p == endParag )
	    break;
	p = p->next();
    }

    return FALSE;
}

void QTextDocument::doLayout( QPainter *p, int w )
{
    minw = wused = 0;
    if ( !is_printer( p ) )
	p = 0;
    withoutDoubleBuffer = ( p != 0 );
    QPainter * oldPainter = QTextFormat::painter();
    QTextFormat::setPainter( p );
    flow_->setWidth( w );
    cw = w;
    vw = w;
    QTextParag *parag = fParag;
    while ( parag ) {
	parag->invalidate( 0 );
	if ( p )
	    parag->adjustToPainter( p );
	parag->format();
	parag = parag->next();
    }
    QTextFormat::setPainter( oldPainter );
}

QPixmap *QTextDocument::bufferPixmap( const QSize &s )
{
    if ( !buf_pixmap ) {
	int w = QABS( s.width() );
	int h = QABS( s.height() );
	buf_pixmap = new QPixmap( w, h );
    } else {
	if ( buf_pixmap->width() < s.width() ||
	     buf_pixmap->height() < s.height() ) {
	    buf_pixmap->resize( QMAX( s.width(), buf_pixmap->width() ),
				QMAX( s.height(), buf_pixmap->height() ) );
	}
    }

    return buf_pixmap;
}

void QTextDocument::draw( QPainter *p, const QRect &rect, const QColorGroup &cg, const QBrush *paper )
{
    if ( !firstParag() )
	return;

    if ( paper ) {
	p->setBrushOrigin( 0, 0 );

	p->fillRect( rect, *paper );
    }

    if ( formatCollection()->defaultFormat()->color() != cg.text() ) {
	QDict<QTextFormat> formats = formatCollection()->dict();
	QDictIterator<QTextFormat> it( formats );
	while ( it.current() ) {
	    if ( it.current() == formatCollection()->defaultFormat() ) {
		++it;
		continue;
	    }
	    it.current()->setColor( cg.text() );
	    ++it;
	}
	formatCollection()->defaultFormat()->setColor( cg.text() );
    }

    QTextParag *parag = firstParag();
    while ( parag ) {
	if ( !parag->isValid() )
	    parag->format();
	int y = parag->rect().y();
	QRect pr( parag->rect() );
	pr.setX( 0 );
	pr.setWidth( QWIDGETSIZE_MAX );
	if ( !rect.isNull() && !rect.intersects( pr ) ) {
	    parag = parag->next();
	    continue;
	}
	p->translate( 0, y );
	if ( rect.isValid() )
	    parag->paint( *p, cg, 0, FALSE, rect.x(), rect.y(), rect.width(), rect.height() );
	else
	    parag->paint( *p, cg, 0, FALSE );
	p->translate( 0, -y );
	parag = parag->next();
	if ( !flow()->isEmpty() )
	    flow()->drawFloatingItems( p, rect.x(), rect.y(), rect.width(), rect.height(), cg, FALSE );
    }
}

void QTextDocument::drawParag( QPainter *p, QTextParag *parag, int cx, int cy, int cw, int ch,
			       QPixmap *&doubleBuffer, const QColorGroup &cg,
			       bool drawCursor, QTextCursor *cursor, bool resetChanged )
{
    QPainter *painter = 0;
    if ( resetChanged )
	parag->setChanged( FALSE );
    QRect ir( parag->rect() );
    bool useDoubleBuffer = !parag->document()->parent();
    if ( !useDoubleBuffer && parag->document()->nextDoubleBuffered )
	useDoubleBuffer = TRUE;
    if ( is_printer( p ) )
	useDoubleBuffer = FALSE;

    if ( useDoubleBuffer  ) {
	painter = new QPainter;
	if ( cx >= 0 && cy >= 0 )
	    ir = ir.intersect( QRect( cx, cy, cw, ch ) );
	if ( !doubleBuffer ||
	     ir.width() > doubleBuffer->width() ||
	     ir.height() > doubleBuffer->height() ) {
	    doubleBuffer = bufferPixmap( ir.size() );
	    painter->begin( doubleBuffer );
	} else {
	    painter->begin( doubleBuffer );
	}
    } else {
	painter = p;
	painter->translate( ir.x(), ir.y() );
    }

    painter->setBrushOrigin( -ir.x(), -ir.y() );

    if ( useDoubleBuffer || is_printer( painter ) ) {
	if ( !parag->backgroundColor() )
	    painter->fillRect( QRect( 0, 0, ir.width(), ir.height() ),
			       cg.brush( QColorGroup::Base ) );
	else
	    painter->fillRect( QRect( 0, 0, ir.width(), ir.height() ),
			       *parag->backgroundColor() );
    } else {
	if ( cursor && cursor->parag() == parag ) {
	    if ( !parag->backgroundColor() )
		painter->fillRect( QRect( parag->at( cursor->index() )->x, 0, 2, ir.height() ),
				   cg.brush( QColorGroup::Base ) );
	    else
		painter->fillRect( QRect( parag->at( cursor->index() )->x, 0, 2, ir.height() ),
				   *parag->backgroundColor() );
	}
    }

    painter->translate( -( ir.x() - parag->rect().x() ),
			-( ir.y() - parag->rect().y() ) );
    parag->paint( *painter, cg, drawCursor ? cursor : 0, TRUE, cx, cy, cw, ch );

    if ( useDoubleBuffer ) {
	delete painter;
	painter = 0;
	p->drawPixmap( ir.topLeft(), *doubleBuffer, QRect( QPoint( 0, 0 ), ir.size() ) );
    } else {
	painter->translate( -ir.x(), -ir.y() );
    }

    if ( parag->rect().x() + parag->rect().width() < parag->document()->x() + parag->document()->width() ) {
	p->fillRect( parag->rect().x() + parag->rect().width(), parag->rect().y(),
		     ( parag->document()->x() + parag->document()->width() ) -
		     ( parag->rect().x() + parag->rect().width() ),
		     parag->rect().height(), cg.brush( QColorGroup::Base ) );
    }

    parag->document()->nextDoubleBuffered = FALSE;
}

QTextParag *QTextDocument::draw( QPainter *p, int cx, int cy, int cw, int ch, const QColorGroup &cg,
				 bool onlyChanged, bool drawCursor, QTextCursor *cursor, bool resetChanged )
{
    if ( withoutDoubleBuffer || par && par->withoutDoubleBuffer ) {
	withoutDoubleBuffer = TRUE;
	QRect r;
	draw( p, r, cg );
	return 0;
    }
    withoutDoubleBuffer = FALSE;

    if ( !firstParag() )
	return 0;

    if ( drawCursor && cursor )
	tmpCursor = cursor;
    if ( cx < 0 && cy < 0 ) {
	cx = 0;
	cy = 0;
	cw = width();
	ch = height();
    }

    QTextParag *lastFormatted = 0;
    QTextParag *parag = firstParag();

    QPixmap *doubleBuffer = 0;
    QPainter painter;

    while ( parag ) {
	lastFormatted = parag;
	if ( !parag->isValid() )
	    parag->format();

	if ( !parag->rect().intersects( QRect( cx, cy, cw, ch ) ) ) {
	    QRect pr( parag->rect() );
	    pr.setWidth( parag->document()->width() );
	    if ( pr.intersects( QRect( cx, cy, cw, ch ) ) )
		p->fillRect( pr.intersect( QRect( cx, cy, cw, ch ) ), cg.brush( QColorGroup::Base ) );
	    if ( parag->rect().y() > cy + ch ) {
		tmpCursor = 0;
		goto floating;
	    }
	    parag = parag->next();
	    continue;
	}

	if ( !parag->hasChanged() && onlyChanged ) {
	    parag = parag->next();
	    continue;
	}

	drawParag( p, parag, cx, cy, cw, ch, doubleBuffer, cg, drawCursor, cursor, resetChanged );
	parag = parag->next();
    }

    parag = lastParag();

 floating:
    if ( parag->rect().y() + parag->rect().height() < parag->document()->height() ) {
	if ( !parag->document()->parent() ) { // !useDoubleBuffer
	    p->fillRect( 0, parag->rect().y() + parag->rect().height(), parag->document()->width(),
			 parag->document()->height() - ( parag->rect().y() + parag->rect().height() ),
			 cg.brush( QColorGroup::Base ) );
	}
 	if ( !flow()->isEmpty() ) {
 	    QRect cr( cx, cy, cw, ch );
//   	    cr = cr.intersect( QRect( 0, parag->rect().y() + parag->rect().height(), parag->document()->width(),
//   				      parag->document()->height() - ( parag->rect().y() + parag->rect().height() ) ) );
 	    flow()->drawFloatingItems( p, cr.x(), cr.y(), cr.width(), cr.height(), cg, FALSE );
 	}
    }

    if ( buf_pixmap && buf_pixmap->height() > 300 ) {
	delete buf_pixmap;
	buf_pixmap = 0;
    }

    tmpCursor = 0;
    return lastFormatted;
}

void QTextDocument::setDefaultFont( const QFont &f )
{
    int s = f.pointSize();
    bool usePixels = FALSE;
    if ( s == -1 ) {
	s = f.pixelSize();
	usePixels = TRUE;
    }
    updateFontSizes( s, usePixels );
}

void QTextDocument::registerCustomItem( QTextCustomItem *i, QTextParag *p )
{
    if ( i && i->placement() != QTextCustomItem::PlaceInline ) {
	flow_->registerFloatingItem( i );
	p->registerFloatingItem( i );
	i->setParagraph( p );
    }
    p->mightHaveCustomItems = mightHaveCustomItems = TRUE;
}

void QTextDocument::unregisterCustomItem( QTextCustomItem *i, QTextParag *p )
{
    flow_->unregisterFloatingItem( i );
    p->unregisterFloatingItem( i );
    i->setParagraph( 0 );
}

bool QTextDocument::hasFocusParagraph() const
{
    return !!focusIndicator.parag;
}

QString QTextDocument::focusHref() const
{
    return focusIndicator.href;
}

bool QTextDocument::focusNextPrevChild( bool next )
{
    if ( !focusIndicator.parag ) {
	if ( next ) {
	    focusIndicator.parag = fParag;
	    focusIndicator.start = 0;
	    focusIndicator.len = 0;
	} else {
	    focusIndicator.parag = lParag;
	    focusIndicator.start = lParag->length();
	    focusIndicator.len = 0;
	}
    } else {
	focusIndicator.parag->setChanged( TRUE );
    }
    focusIndicator.href = QString::null;

    if ( next ) {
	QTextParag *p = focusIndicator.parag;
	int index = focusIndicator.start + focusIndicator.len;
	while ( p ) {
	    for ( int i = index; i < p->length(); ++i ) {
		if ( p->at( i )->isAnchor() ) {
		    p->setChanged( TRUE );
		    focusIndicator.parag = p;
		    focusIndicator.start = i;
		    focusIndicator.len = 0;
		    focusIndicator.href = p->at( i )->anchorHref();
		    while ( i < p->length() ) {
			if ( !p->at( i )->isAnchor() )
			    return TRUE;
			focusIndicator.len++;
			i++;
		    }
		} else if ( p->at( i )->isCustom() ) {
		    if ( p->at( i )->customItem()->isNested() ) {
			QTextTable *t = (QTextTable*)p->at( i )->customItem();
			QPtrList<QTextTableCell> cells = t->tableCells();
			// first try to continue
			QTextTableCell *c;
			bool resetCells = TRUE;
			for ( c = cells.first(); c; c = cells.next() ) {
			    if ( c->richText()->hasFocusParagraph() ) {
				if ( c->richText()->focusNextPrevChild( next ) ) {
				    p->setChanged( TRUE );
				    focusIndicator.parag = p;
				    focusIndicator.start = i;
				    focusIndicator.len = 0;
				    focusIndicator.href = c->richText()->focusHref();
				    return TRUE;
				} else {
				    resetCells = FALSE;
				    c = cells.next();
				    break;
				}
			    }
			}
			// now really try
			if ( resetCells )
			    c = cells.first();
			for ( ; c; c = cells.next() ) {
			    if ( c->richText()->focusNextPrevChild( next ) ) {
				p->setChanged( TRUE );
				focusIndicator.parag = p;
				focusIndicator.start = i;
				focusIndicator.len = 0;
				focusIndicator.href = c->richText()->focusHref();
				return TRUE;
			    }
			}
		    }
		}
	    }
	    index = 0;
	    p = p->next();
	}
    } else {
	QTextParag *p = focusIndicator.parag;
	int index = focusIndicator.start - 1;
	if ( focusIndicator.len == 0 && index < focusIndicator.parag->length() - 1 )
	    index++;
	while ( p ) {
	    for ( int i = index; i >= 0; --i ) {
		if ( p->at( i )->isAnchor() ) {
		    p->setChanged( TRUE );
		    focusIndicator.parag = p;
		    focusIndicator.start = i;
		    focusIndicator.len = 0;
		    focusIndicator.href = p->at( i )->anchorHref();
		    while ( i >= -1 ) {
			if ( i < 0 || !p->at( i )->isAnchor() ) {
			    focusIndicator.start++;
			    return TRUE;
			}
			if ( i < 0 )
			    break;
			focusIndicator.len++;
			focusIndicator.start--;
			i--;
		    }
		} else if ( p->at( i )->isCustom() ) {
		    if ( p->at( i )->customItem()->isNested() ) {
			QTextTable *t = (QTextTable*)p->at( i )->customItem();
			QPtrList<QTextTableCell> cells = t->tableCells();
			// first try to continue
			QTextTableCell *c;
			bool resetCells = TRUE;
			for ( c = cells.last(); c; c = cells.prev() ) {
			    if ( c->richText()->hasFocusParagraph() ) {
				if ( c->richText()->focusNextPrevChild( next ) ) {
				    p->setChanged( TRUE );
				    focusIndicator.parag = p;
				    focusIndicator.start = i;
				    focusIndicator.len = 0;
				    focusIndicator.href = c->richText()->focusHref();
				    return TRUE;
				} else {
				    resetCells = FALSE;
				    c = cells.prev();
				    break;
				}
			    }
			    if ( cells.at() == 0 )
				break;
			}
			// now really try
			if ( resetCells )
			    c = cells.last();
			for ( ; c; c = cells.prev() ) {
			    if ( c->richText()->focusNextPrevChild( next ) ) {
				p->setChanged( TRUE );
				focusIndicator.parag = p;
				focusIndicator.start = i;
				focusIndicator.len = 0;
				focusIndicator.href = c->richText()->focusHref();
				return TRUE;
			    }
			    if ( cells.at() == 0 )
				break;
			}
		    }
		}
	    }
	    p = p->prev();
	    if ( p )
		index = p->length() - 1;
	}
    }

    focusIndicator.parag = 0;

    return FALSE;
}

int QTextDocument::length() const
{
    int l = 0;
    QTextParag *p = fParag;
    while ( p ) {
	l += p->length() - 1; // don't count trailing space
	p = p->next();
    }
    return l;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int QTextFormat::width( const QChar &c ) const
{
    if ( c.unicode() == 0xad ) // soft hyphen
	return 0;
    if ( !pntr || !pntr->isActive() ) {
	if ( c == '\t' )
	    return fm.width( 'x' ) * 8;
	if ( ha == AlignNormal ) {
	    int w;
	    if ( c.row() )
		w = fm.width( c );
	    else
		w = widths[ c.unicode() ];
	    if ( w == 0 && !c.row() ) {
		w = fm.width( c );
		( (QTextFormat*)this )->widths[ c.unicode() ] = w;
	    }
	    return w;
	} else {
	    QFont f( fn );
	    if ( usePixelSizes )
		f.setPixelSize( ( f.pixelSize() * 2 ) / 3 );
	    else
		f.setPointSize( ( f.pointSize() * 2 ) / 3 );
	    QFontMetrics fm_( f );
	    return fm_.width( c );
	}
    }

    QFont f( fn );
    if ( ha != AlignNormal ) {
	if ( usePixelSizes )
	    f.setPixelSize( ( f.pixelSize() * 2 ) / 3 );
	else
	    f.setPointSize( ( f.pointSize() * 2 ) / 3 );
    }
    pntr->setFont( f );

    return pntr->fontMetrics().width( c );
}

int QTextFormat::width( const QString &str, int pos ) const
{
    int w = 0;
    if ( str[ pos ].unicode() == 0xad )
	return w;
    if ( !pntr || !pntr->isActive() ) {
	if ( ha == AlignNormal ) {
	    w = fm.width( str[ pos ] );
	} else {
	    QFont f( fn );
	    if ( usePixelSizes )
		f.setPixelSize( ( f.pixelSize() * 2 ) / 3 );
	    else
		f.setPointSize( ( f.pointSize() * 2 ) / 3 );
	    QFontMetrics fm_( f );
	    w = fm_.width( str[ pos ] );
	}
    } else {
	QFont f( fn );
	if ( ha != AlignNormal ) {
	    if ( usePixelSizes )
		f.setPixelSize( ( f.pixelSize() * 2 ) / 3 );
	    else
		f.setPointSize( ( f.pointSize() * 2 ) / 3 );
	}
	pntr->setFont( f );
	w = pntr->fontMetrics().width( str[ pos ] );
    }
    return w;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

QTextString::QTextString()
{
    bidiDirty = FALSE;
    bidi = FALSE;
    rightToLeft = FALSE;
    dir = QChar::DirON;
}

QTextString::QTextString( const QTextString &s )
{
    bidiDirty = s.bidiDirty;
    bidi = s.bidi;
    rightToLeft = s.rightToLeft;
    dir = s.dir;
    data = s.subString();
}

void QTextString::insert( int index, const QString &s, QTextFormat *f )
{
    int os = data.size();
    data.resize( data.size() + s.length() );
    if ( index < os ) {
	memmove( data.data() + index + s.length(), data.data() + index,
		 sizeof( QTextStringChar ) * ( os - index ) );
    }
    for ( int i = 0; i < (int)s.length(); ++i ) {
	data[ (int)index + i ].x = 0;
	data[ (int)index + i ].lineStart = 0;
	data[ (int)index + i ].d.format = 0;
	data[ (int)index + i ].type = QTextStringChar::Regular;
	data[ (int)index + i ].rightToLeft = 0;
	data[ (int)index + i ].startOfRun = 0;
	data[ (int)index + i ].c = s[ i ];
	data[ (int)index + i ].setFormat( f );
    }
    bidiDirty = TRUE;
}

QTextString::~QTextString()
{
    clear();
}

void QTextString::insert( int index, QTextStringChar *c )
{
    int os = data.size();
    data.resize( data.size() + 1 );
    if ( index < os ) {
	memmove( data.data() + index + 1, data.data() + index,
		 sizeof( QTextStringChar ) * ( os - index ) );
    }
    data[ (int)index ].c = c->c;
    data[ (int)index ].x = 0;
    data[ (int)index ].lineStart = 0;
    data[ (int)index ].rightToLeft = 0;
    data[ (int)index ].d.format = 0;
    data[ (int)index ].type = QTextStringChar::Regular;
    data[ (int)index ].setFormat( c->format() );
    bidiDirty = TRUE;
}

void QTextString::truncate( int index )
{
    index = QMAX( index, 0 );
    index = QMIN( index, (int)data.size() - 1 );
    if ( index < (int)data.size() ) {
	for ( int i = index + 1; i < (int)data.size(); ++i ) {
	    if ( !(data[ i ].type == QTextStringChar::Regular) ) {
		delete data[ i ].customItem();
		if ( data[ i ].d.custom->format )
		    data[ i ].d.custom->format->removeRef();
		delete data[ i ].d.custom;
		data[ i ].d.custom = 0;
	    } else if ( data[ i ].format() ) {
		data[ i ].format()->removeRef();
	    }
	}
    }
    data.truncate( index );
    bidiDirty = TRUE;
}

void QTextString::remove( int index, int len )
{
    for ( int i = index; i < (int)data.size() && i - index < len; ++i ) {
	if ( !(data[ i ].type == QTextStringChar::Regular) ) {
	    delete data[ i ].customItem();
	    if ( data[ i ].d.custom->format )
		data[ i ].d.custom->format->removeRef();
	    delete data[ i ].d.custom;
	    data[ i ].d.custom = 0;
	} else if ( data[ i ].format() ) {
	    data[ i ].format()->removeRef();
	}
    }
    memmove( data.data() + index, data.data() + index + len,
	     sizeof( QTextStringChar ) * ( data.size() - index - len ) );
    data.resize( data.size() - len );
    bidiDirty = TRUE;
}

void QTextString::clear()
{
    for ( int i = 0; i < (int)data.count(); ++i ) {
	if ( !(data[ i ].type == QTextStringChar::Regular) ) {
	    delete data[ i ].customItem();
	    if ( data[ i ].d.custom->format )
		data[ i ].d.custom->format->removeRef();
	    delete data[ i ].d.custom;
	    data[ i ].d.custom = 0;
	} else if ( data[ i ].format() ) {
	    data[ i ].format()->removeRef();
	}
    }
    data.resize( 0 );
}

void QTextString::setFormat( int index, QTextFormat *f, bool useCollection )
{
    if ( useCollection && data[ index ].format() )
	data[ index ].format()->removeRef();
    data[ index ].setFormat( f );
}

void QTextString::checkBidi() const
{
    bool rtlKnown = FALSE;
    if ( dir == QChar::DirR ) {
	((QTextString *)this)->bidi = TRUE;
	((QTextString *)this)->rightToLeft = TRUE;
	rtlKnown = TRUE;
	return;
    } else if ( dir == QChar::DirL ) {
	((QTextString *)this)->rightToLeft = FALSE;
	rtlKnown = TRUE;
    } else {
	((QTextString *)this)->rightToLeft = FALSE;
    }

    int len = data.size();
    const QTextStringChar *c = data.data();
    ((QTextString *)this)->bidi = FALSE;
    while( len ) {
	if ( !rtlKnown ) {
	    switch( c->c.direction() )
	    {
		case QChar::DirL:
		case QChar::DirLRO:
		case QChar::DirLRE:
		    ((QTextString *)this)->rightToLeft = FALSE;
		    rtlKnown = TRUE;
		    break;
		case QChar::DirR:
		case QChar::DirAL:
		case QChar::DirRLO:
		case QChar::DirRLE:
		    ((QTextString *)this)->rightToLeft = TRUE;
		    rtlKnown = TRUE;
		    break;
		default:
		    break;
	    }
	}
	uchar row = c->c.row();
	if( (row > 0x04 && row < 0x09) || (row > 0xfa && row < 0xff) ) {
	    ((QTextString *)this)->bidi = TRUE;
	    if ( rtlKnown )
		return;
	}
	len--;
	++c;
    }
}

void QTextDocument::setStyleSheet( QStyleSheet *s )
{
    if ( !s )
	return;
    sheet_ = s;
    fCollection->setStyleSheet( s );
    updateStyles();
}

void QTextDocument::updateStyles()
{
    invalidate();
    if ( par )
	underlLinks = par->underlLinks;
    fCollection->updateStyles();
    for ( QTextDocument *d = childList.first(); d; d = childList.next() )
	d->updateStyles();
}

void QTextDocument::updateFontSizes( int base, bool usePixels )
{
    for ( QTextDocument *d = childList.first(); d; d = childList.next() )
	d->updateFontSizes( base, usePixels );
    invalidate();
    fCollection->updateFontSizes( base, usePixels );
}

void QTextDocument::updateFontAttributes( const QFont &f, const QFont &old )
{
    for ( QTextDocument *d = childList.first(); d; d = childList.next() )
	d->updateFontAttributes( f, old );
    invalidate();
    fCollection->updateFontAttributes( f, old );
}

void QTextStringChar::setFormat( QTextFormat *f )
{
    if ( type == Regular ) {
	d.format = f;
    } else {
	if ( !d.custom ) {
	    d.custom = new CustomData;
	    d.custom->custom = 0;
	}
	d.custom->format = f;
    }
}

void QTextStringChar::setCustomItem( QTextCustomItem *i )
{
    if ( type == Regular ) {
	QTextFormat *f = format();
	d.custom = new CustomData;
	d.custom->format = f;
    } else {
	delete d.custom->custom;
    }
    d.custom->custom = i;
    type = (type == Anchor ? CustomAnchor : Custom);
}

void QTextStringChar::loseCustomItem()
{
    if ( type == Custom ) {
	QTextFormat *f = d.custom->format;
	d.custom->custom = 0;
	delete d.custom;
	type = Regular;
	d.format = f;
    } else if ( type == CustomAnchor ) {
	d.custom->custom = 0;
	type = Anchor;
    }
}

QString QTextStringChar::anchorName() const
{
    if ( type == Regular )
	return QString::null;
    else
	return d.custom->anchorName;
}

QString QTextStringChar::anchorHref() const
{
    if ( type == Regular )
	return QString::null;
    else
	return d.custom->anchorHref;
}

void QTextStringChar::setAnchor( const QString& name, const QString& href )
{
    if ( type == Regular ) {
	QTextFormat *f = format();
	d.custom = new CustomData;
	d.custom->custom = 0;
	d.custom->format = f;
	type = Anchor;
    } else if ( type == Custom ) {
	type = CustomAnchor;
    }
    d.custom->anchorName = name;
    d.custom->anchorHref = href;
}


int QTextString::width( int idx ) const
{
     int w = 0;
     QTextStringChar *c = &at( idx );
     if ( c->c.unicode() == 0xad )
	 return 0;
     if( c->isCustom() ) {
	 if( c->customItem()->placement() == QTextCustomItem::PlaceInline )
	     w = c->customItem()->width;
     } else {
	 int r = c->c.row();
	 if( r < 0x06 || r > 0x1f )
	     w = c->format()->width( c->c );
	 else {
	     // complex text. We need some hacks to get the right metric here
	     QString str;
	     int pos = 0;
	     if( idx > 4 )
		 pos = idx - 4;
	     int off = idx - pos;
	     int end = QMIN( length(), idx + 4 );
	     while ( pos < end ) {
		 str += at(pos).c;
		 pos++;
	     }
	     w = c->format()->width( str, off );
	 }
     }
     return w;
}

QMemArray<QTextStringChar> QTextString::subString( int start, int len ) const
{
    if ( len == 0xFFFFFF )
	len = data.size();
    QMemArray<QTextStringChar> a;
    a.resize( len );
    for ( int i = 0; i < len; ++i ) {
	QTextStringChar *c = &data[ i + start ];
	a[ i ].c = c->c;
	a[ i ].x = 0;
	a[ i ].lineStart = 0;
	a[ i ].rightToLeft = 0;
	a[ i ].d.format = 0;
	a[ i ].type = QTextStringChar::Regular;
	a[ i ].setFormat( c->format() );
	if ( c->format() )
	    c->format()->addRef();
    }
    return a;
}

QTextStringChar *QTextStringChar::clone() const
{
    QTextStringChar *chr = new QTextStringChar;
    chr->c = c;
    chr->x = 0;
    chr->lineStart = 0;
    chr->rightToLeft = 0;
    chr->d.format = 0;
    chr->type = QTextStringChar::Regular;
    chr->setFormat( format() );
    if ( chr->format() )
	chr->format()->addRef();
    return chr;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

QTextParag::QTextParag( QTextDocument *d, QTextParag *pr, QTextParag *nx, bool updateIds )
    : invalid( 0 ), p( pr ), n( nx ), docOrPseudo( d ),  align( 0 ),mSelections( 0 ),
      mStyleSheetItemsVec( 0 ), mFloatingItems( 0 ), listS( QStyleSheetItem::ListDisc ),
      numSubParag( -1 ), tm( -1 ), bm( -1 ), lm( -1 ), rm( -1 ), flm( -1 ),
      tArray(0), tabStopWidth(0), eData( 0 )
{
    listS = QStyleSheetItem::ListDisc;
    if ( ! (hasdoc = docOrPseudo != 0 ) )
	docOrPseudo = new QTextParagPseudoDocument;
    bgcol = 0;
    breakable = TRUE;
    isBr = FALSE;
    movedDown = FALSE;
    mightHaveCustomItems = FALSE;
    visible = TRUE;
    list_val = -1;
    newLinesAllowed = FALSE;
    lastInFrame = FALSE;
    defFormat = formatCollection()->defaultFormat();
    if ( !hasdoc ) {
	tabStopWidth = defFormat->width( 'x' ) * 8;
	pseudoDocument()->commandHistory = new QTextCommandHistory( 100 );
    }
#if defined(PARSER_DEBUG)
    qDebug( debug_indent + "new QTextParag" );
#endif
    fullWidth = TRUE;

    if ( p )
	p->n = this;
    if ( n )
	n->p = this;


    if ( !p && hasdoc )
	document()->setFirstParag( this );
    if ( !n && hasdoc )
	document()->setLastParag( this );

    changed = FALSE;
    firstFormat = TRUE;
    state = -1;
    needPreProcess = FALSE;

    if ( p )
	id = p->id + 1;
    else
	id = 0;
    if ( n && updateIds ) {
	QTextParag *s = n;
	while ( s ) {
	    s->id = s->p->id + 1;
	    s->numSubParag = -1;
	    s->lm = s->rm = s->tm = s->bm = -1, s->flm = -1;
	    s = s->n;
	}
    }
    firstPProcess = TRUE;

    str = new QTextString();
    str->insert( 0, " ", formatCollection()->defaultFormat() );
}

QTextParag::~QTextParag()
{
    delete str;
    if ( hasdoc ) {
	register QTextDocument *doc = document();
	if ( this == doc->minwParag ) {
	    doc->minwParag = 0;
	    doc->minw = 0;
	}
	if ( this == doc->curParag )
	    doc->curParag = 0;
    } else {
	delete pseudoDocument();
    }
    if ( tArray )
	delete [] tArray;
    delete eData;
    QMap<int, QTextParagLineStart*>::Iterator it = lineStarts.begin();
    for ( ; it != lineStarts.end(); ++it )
	delete *it;
    if ( mSelections )
	delete mSelections;
    if ( mFloatingItems )
	delete mFloatingItems;
    if ( mStyleSheetItemsVec )
	delete mStyleSheetItemsVec;
    if ( p )
	p->setNext( n );
    if ( n )
	n->setPrev( p );
}

void QTextParag::setNext( QTextParag *s )
{
    n = s;
    if ( !n && hasdoc )
	document()->setLastParag( this );
}

void QTextParag::setPrev( QTextParag *s )
{
    p = s;
    if ( !p && hasdoc )
	document()->setFirstParag( this );
}

void QTextParag::invalidate( int chr )
{
    if ( invalid < 0 )
	invalid = chr;
    else
	invalid = QMIN( invalid, chr );
    if ( mFloatingItems ) {
	for ( QTextCustomItem *i = mFloatingItems->first(); i; i = mFloatingItems->next() )
	    i->ypos = -1;
    }
    lm = rm = bm = tm = flm = -1;
}

void QTextParag::insert( int index, const QString &s )
{
    if ( hasdoc && !document()->useFormatCollection() && document()->preProcessor() )
	str->insert( index, s,
		     document()->preProcessor()->format( QTextPreProcessor::Standard ) );
    else
	str->insert( index, s, formatCollection()->defaultFormat() );
    invalidate( index );
    needPreProcess = TRUE;
}

void QTextParag::truncate( int index )
{
    str->truncate( index );
    insert( length(), " " );
    needPreProcess = TRUE;
}

void QTextParag::remove( int index, int len )
{
    if ( index + len - str->length() > 0 )
	return;
    for ( int i = index; i < index + len; ++i ) {
	QTextStringChar *c = at( i );
	if ( hasdoc && c->isCustom() ) {
	    document()->unregisterCustomItem( c->customItem(), this );
	}
    }
    str->remove( index, len );
    invalidate( 0 );
    needPreProcess = TRUE;
}

void QTextParag::join( QTextParag *s )
{
    int oh = r.height() + s->r.height();
    n = s->n;
    if ( n )
	n->p = this;
    else if ( hasdoc )
	document()->setLastParag( this );

    int start = str->length();
    if ( length() > 0 && at( length() - 1 )->c == ' ' ) {
	remove( length() - 1, 1 );
	--start;
    }
    append( s->str->toString(), TRUE );

    for ( int i = 0; i < s->length(); ++i ) {
	if ( !hasdoc || document()->useFormatCollection() ) {
	    s->str->at( i ).format()->addRef();
	    str->setFormat( i + start, s->str->at( i ).format(), TRUE );
	}
	if ( s->str->at( i ).isCustom() ) {
	    QTextCustomItem * item = s->str->at( i ).customItem();
	    str->at( i + start ).setCustomItem( item );
	    s->str->at( i ).loseCustomItem();
	}
	if ( s->str->at( i ).isAnchor() ) {
	    str->at( i + start ).setAnchor( s->str->at( i ).anchorName(),
			    s->str->at( i ).anchorHref() );
	}
    }

    if ( !extraData() && s->extraData() ) {
	setExtraData( s->extraData() );
	s->setExtraData( 0 );
    } else if ( extraData() && s->extraData() ) {
	extraData()->join( s->extraData() );
    }
    delete s;
    invalidate( 0 );
    r.setHeight( oh );
    needPreProcess = TRUE;
    if ( n ) {
	QTextParag *s = n;
	while ( s ) {
	    s->id = s->p->id + 1;
	    s->state = -1;
	    s->needPreProcess = TRUE;
	    s->changed = TRUE;
	    s = s->n;
	}
    }
    format();
    state = -1;
}

void QTextParag::move( int &dy )
{
    if ( dy == 0 )
	return;
    changed = TRUE;
    r.moveBy( 0, dy );
    if ( mFloatingItems ) {
	for ( QTextCustomItem *i = mFloatingItems->first(); i; i = mFloatingItems->next() )
	    i->ypos += dy;
    }
    if ( p )
	p->lastInFrame = TRUE;

    // do page breaks if required
    if ( hasdoc && document()->isPageBreakEnabled() ) {
	int shift;
	if ( ( shift = document()->formatter()->formatVertically(  document(), this ) ) ) {
	    if ( p )
		p->setChanged( TRUE );
	    dy += shift;
	}
    }
}

void QTextParag::format( int start, bool doMove )
{
    if ( !str || str->length() == 0 || !formatter() )
	return;

    if ( hasdoc &&
	 document()->preProcessor() &&
	 ( needPreProcess || state == -1 ) )
	document()->preProcessor()->process( document(), this, invalid <= 0 ? 0 : invalid );
    needPreProcess = FALSE;

    if ( invalid == -1 )
	return;

    r.moveTopLeft( QPoint( documentX(), p ? p->r.y() + p->r.height() : documentY() ) );
    r.setWidth( documentWidth() );
    if ( p )
	p->lastInFrame = FALSE;

    movedDown = FALSE;
    bool formattedAgain = FALSE;

 formatAgain:

    if ( hasdoc && mFloatingItems ) {
	for ( QTextCustomItem *i = mFloatingItems->first(); i; i = mFloatingItems->next() ) {
	    i->ypos = r.y();
	    if ( i->placement() == QTextCustomItem::PlaceRight ) {
		i->xpos = r.x() + r.width() - i->width;
	    }
	}
    }
    QMap<int, QTextParagLineStart*> oldLineStarts = lineStarts;
    lineStarts.clear();
    int y = formatter()->format( document(), this, start, oldLineStarts );


    r.setWidth( QMAX( r.width(), formatter()->minimumWidth() ) );


    QMap<int, QTextParagLineStart*>::Iterator it = oldLineStarts.begin();

    for ( ; it != oldLineStarts.end(); ++it )
	delete *it;

    QTextStringChar *c = 0;
 // do not do this on mac, as the paragraph
 // with has to be the full document width on mac as the selections
 // always extend completely to the right. This is a bit unefficient,
 // as this results in a bigger double buffer than needed but ok for
 // now.
    if ( lineStarts.count() == 1 ) { //&& ( !doc || document()->flow()->isEmpty() ) ) {
	if ( !string()->isBidi() ) {
	    c = &str->at( str->length() - 1 );
	    r.setWidth( c->x + str->width( str->length() - 1 ) );
	} else {
	    r.setWidth( lineStarts[0]->w );
	}
    }

    if ( newLinesAllowed ) {
	it = lineStarts.begin();
	int usedw = 0;
	for ( ; it != lineStarts.end(); ++it )
	    usedw = QMAX( usedw, (*it)->w );
	if ( r.width() <= 0 ) {
	    // if the user specifies an invalid rect, this means that the
	    // bounding box should grow to the width that the text actually
	    // needs
	    r.setWidth( usedw );
	} else {
	    r.setWidth( QMIN( usedw, r.width() ) );
	}
    }

    if ( y != r.height() )
	r.setHeight( y );

    if ( !visible ) {
	r.setHeight( 0 );
    } else {
	int minw = formatter()->minimumWidth();
	int wused = formatter()->widthUsed();
	wused = QMAX( minw, wused );
	if ( hasdoc ) {
	    document()->setMinimumWidth( minw, wused, this );
	}  else {
	    pseudoDocument()->minw = QMAX( pseudoDocument()->minw, minw );
	    pseudoDocument()->wused = QMAX( pseudoDocument()->wused, wused );
	}
    }

    // do page breaks if required
    if ( hasdoc && document()->isPageBreakEnabled() ) {
        int shift = document()->formatter()->formatVertically( document(), this );
        if ( shift && !formattedAgain ) {
            formattedAgain = TRUE;
            goto formatAgain;
        }
    }

    if ( n && doMove && n->invalid == -1 && r.y() + r.height() != n->r.y() ) {
	int dy = ( r.y() + r.height() ) - n->r.y();
	QTextParag *s = n;
	bool makeInvalid = p && p->lastInFrame;
	while ( s && dy ) {
	    if ( !s->isFullWidth() )
		makeInvalid = TRUE;
	    if ( makeInvalid )
		s->invalidate( 0 );
	    s->move( dy );
	    if ( s->lastInFrame )
		makeInvalid = TRUE;
  	    s = s->n;
	}
    }

    firstFormat = FALSE;
    changed = TRUE;
    invalid = -1;
    //#####   string()->setTextChanged( FALSE );
}

int QTextParag::lineHeightOfChar( int i, int *bl, int *y ) const
{
    if ( !isValid() )
	( (QTextParag*)this )->format();

    QMap<int, QTextParagLineStart*>::ConstIterator it = lineStarts.end();
    --it;
    for ( ;; ) {
	if ( i >= it.key() ) {
	    if ( bl )
		*bl = ( *it )->baseLine;
	    if ( y )
		*y = ( *it )->y;
	    return ( *it )->h;
	}
	if ( it == lineStarts.begin() )
	    break;
	--it;
    }

    qWarning( "QTextParag::lineHeightOfChar: couldn't find lh for %d", i );
    return 15;
}

QTextStringChar *QTextParag::lineStartOfChar( int i, int *index, int *line ) const
{
    if ( !isValid() )
	( (QTextParag*)this )->format();

    int l = (int)lineStarts.count() - 1;
    QMap<int, QTextParagLineStart*>::ConstIterator it = lineStarts.end();
    --it;
    for ( ;; ) {
	if ( i >= it.key() ) {
	    if ( index )
		*index = it.key();
	    if ( line )
		*line = l;
	    return &str->at( it.key() );
	}
	if ( it == lineStarts.begin() )
	    break;
	--it;
	--l;
    }

    qWarning( "QTextParag::lineStartOfChar: couldn't find %d", i );
    return 0;
}

int QTextParag::lines() const
{
    if ( !isValid() )
	( (QTextParag*)this )->format();

    return (int)lineStarts.count();
}

QTextStringChar *QTextParag::lineStartOfLine( int line, int *index ) const
{
    if ( !isValid() )
	( (QTextParag*)this )->format();

    if ( line >= 0 && line < (int)lineStarts.count() ) {
	QMap<int, QTextParagLineStart*>::ConstIterator it = lineStarts.begin();
	while ( line-- > 0 )
	    ++it;
	int i = it.key();
	if ( index )
	    *index = i;
	return &str->at( i );
    }

    qWarning( "QTextParag::lineStartOfLine: couldn't find %d", line );
    return 0;
}

int QTextParag::leftGap() const
{
    if ( !isValid() )
	( (QTextParag*)this )->format();

    int line = 0;
    int x = str->at(0).x;  /* set x to x of first char */
    if ( str->isBidi() ) {
	for ( int i = 1; i < str->length(); ++i )
	    x = QMIN(x, str->at(i).x);
	return x;
    }

    QMap<int, QTextParagLineStart*>::ConstIterator it = lineStarts.begin();
    while (line < (int)lineStarts.count()) {
	int i = it.key(); /* char index */
	x = QMIN(x, str->at(i).x);
	++it;
	++line;
    }
    return x;
}

void QTextParag::setFormat( int index, int len, QTextFormat *f, bool useCollection, int flags )
{
    if ( !f )
	return;
    if ( index < 0 )
	index = 0;
    if ( index > str->length() - 1 )
	index = str->length() - 1;
    if ( index + len >= str->length() )
	len = str->length() - index;

    QTextFormatCollection *fc = 0;
    if ( useCollection )
	fc = formatCollection();
    QTextFormat *of;
    for ( int i = 0; i < len; ++i ) {
	of = str->at( i + index ).format();
	if ( !changed && f->key() != of->key() )
	    changed = TRUE;
	if ( invalid == -1 &&
	     ( f->font().family() != of->font().family() ||
	       f->font().pointSize() != of->font().pointSize() ||
	       f->font().weight() != of->font().weight() ||
	       f->font().italic() != of->font().italic() ||
	       f->vAlign() != of->vAlign() ) ) {
	    invalidate( 0 );
	}
	if ( flags == -1 || flags == QTextFormat::Format || !fc ) {
	    if ( fc )
		f = fc->format( f );
	    str->setFormat( i + index, f, useCollection );
	} else {
	    QTextFormat *fm = fc->format( of, f, flags );
	    str->setFormat( i + index, fm, useCollection );
	}
    }
}

void QTextParag::indent( int *oldIndent, int *newIndent )
{
    if ( !hasdoc || !document()->indent() || style() && style()->displayMode() != QStyleSheetItem::DisplayBlock ) {
	if ( oldIndent )
	    *oldIndent = 0;
	if ( newIndent )
	    *newIndent = 0;
	if ( oldIndent && newIndent )
	    *newIndent = *oldIndent;
	return;
    }
    document()->indent()->indent( document(), this, oldIndent, newIndent );
}

void QTextParag::paint( QPainter &painter, const QColorGroup &cg, QTextCursor *cursor, bool drawSelections,
			int clipx, int clipy, int clipw, int cliph )
{
    if ( !visible )
	return;
    QTextStringChar *chr = at( 0 );
    int i = 0;
    int h = 0;
    int baseLine = 0, lastBaseLine = 0;
    QTextStringChar *formatChar = 0;
    int lastY = -1;
    int startX = 0;
    int bw = 0;
    int cy = 0;
    int curx = -1, cury = 0, curh = 0;
    bool lastDirection = chr->rightToLeft;
    const int full_sel_width = (hasdoc ? document()->width() : r.width());
#if 0 // seems we don't need that anymore
    int tw = 0;
#endif

    QString qstr = str->toString();
    // ### workaround so that \n are not drawn, actually this should be
    // fixed in QFont somewhere (under Windows you get ugly boxes
    // otherwise)
    QChar* uc = (QChar*) qstr.unicode();
    for ( uint ii = 0; ii < qstr.length(); ii++ ) {
	if ( uc[(int)ii]== '\n' )
	    uc[(int)ii] = 0x20;
    }


    const int nSels = hasdoc ? document()->numSelections() : 1;
    QMemArray<int> selectionStarts( nSels );
    QMemArray<int> selectionEnds( nSels );
    if ( drawSelections ) {
	bool hasASelection = FALSE;
	for ( i = 0; i < nSels; ++i ) {
	    if ( !hasSelection( i ) ) {
		selectionStarts[ i ] = -1;
		selectionEnds[ i ] = -1;
	    } else {
		hasASelection = TRUE;
		selectionStarts[ i ] = selectionStart( i );
		int end = selectionEnd( i );
		if ( end == length() - 1 && n && n->hasSelection( i ) )
		    end++;
		selectionEnds[ i ] = end;
	    }
	}
	if ( !hasASelection )
	    drawSelections = FALSE;
    }

    int line = -1;
    int cw;
    bool didListLabel = FALSE;
    int paintStart = 0;
    int paintEnd = -1;
    int lasth = 0;
    for ( i = 0; i < length(); i++ ) {
	chr = at( i );
#if 0 // seems we don't need that anymore
	if ( !str->isBidi() && is_printer( &painter ) ) { // ### fix our broken ps-printer
	    if ( !chr->lineStart )
		chr->x = QMAX( chr->x, tw );
	    else
		tw = 0;
	}
#endif
	cw = string()->width( i );
	if ( chr->c == '\t' && i < length() - 1 )
	    cw = at( i + 1 )->x - chr->x + 1;
	if ( chr->c.unicode() == 0xad && i < length() - 1 )
	    cw = 0;

	// init a new line
	if ( chr->lineStart ) {
#if 0 // seems we don't need that anymore
	    tw = 0;
#endif
	    ++line;
	    lineInfo( line, cy, h, baseLine );
	    lasth = h;
	    if ( clipy != -1 && cy > clipy - r.y() + cliph ) // outside clip area, leave
		break;
	    if ( lastBaseLine == 0 )
		lastBaseLine = baseLine;
	}

	// draw bullet list items
	if ( !didListLabel && line == 0 && style() && style()->displayMode() == QStyleSheetItem::DisplayListItem ) {
	    didListLabel = TRUE;
	    drawLabel( &painter, chr->x, cy, 0, 0, baseLine, cg );
	}

	// check for cursor mark
	if ( cursor && this == cursor->parag() && i == cursor->index() ) {
	    curx = cursor->x();
	    QTextStringChar *c = chr;
	    if ( i > 0 )
		--c;
	    curh = c->format()->height();
	    cury = cy + baseLine - c->format()->ascent();
	}

	// first time - start again...
	if ( !formatChar || lastY == -1 ) {
	    formatChar = chr;
	    lastY = cy;
	    startX = chr->x;
	    if ( !chr->isCustom() && chr->c != '\n' )
		paintEnd = i;
	    bw = cw;
	    if ( !chr->isCustom() )
		continue;
	}

	// check if selection state changed
	bool selectionChange = FALSE;
	if ( drawSelections ) {
	    for ( int j = 0; j < nSels; ++j ) {
		selectionChange = selectionStarts[ j ] == i || selectionEnds[ j ] == i;
		if ( selectionChange )
		    break;
	    }
	}

	//if something (format, etc.) changed, draw what we have so far
	if ( ( ( ( alignment() & Qt3::AlignJustify ) == Qt3::AlignJustify && at(paintEnd)->c.isSpace() ) ||
	       lastDirection != (bool)chr->rightToLeft ||
	       chr->startOfRun ||
	       lastY != cy || chr->format() != formatChar->format() || chr->isAnchor() != formatChar->isAnchor() ||
	       ( paintEnd != -1 && at( paintEnd )->c =='\t' ) || chr->c == '\t' ||
	       ( paintEnd != -1 && at( paintEnd )->c.unicode() == 0xad ) || chr->c.unicode() == 0xad ||
	       selectionChange || chr->isCustom() ) ) {
	    if ( paintStart <= paintEnd ) {
		// ### temporary hack until I get the new placement/shaping stuff working
		int x = startX;
		if ( ( alignment() & Qt3::AlignJustify ) == Qt3::AlignJustify && paintEnd != -1 &&
		     paintEnd > 1 && at( paintEnd )->c.isSpace() ) {
		    int add = str->at(paintEnd).x - str->at(paintEnd-1).x - str->width(paintEnd-1);
		    bw += ( lastDirection ? 0 : add );
		}
		drawParagString( painter, qstr, paintStart, paintEnd - paintStart + 1, x, lastY,
				 lastBaseLine, bw, lasth, drawSelections,
				 formatChar, i, selectionStarts, selectionEnds, cg, lastDirection );
	    }
#if 0 // seems we don't need that anymore
	    if ( !str->isBidi() && is_printer( &painter ) ) { // ### fix our broken ps-printer
		if ( !chr->lineStart ) {
		    // ### the next line doesn't look 100% correct for arabic
		    tw = startX + painter.fontMetrics().width( qstr.mid(paintStart, paintEnd - paintStart +1) );
		    chr->x = QMAX( chr->x, tw );
		} else {
		    tw = 0;
		}
	    }
#endif
	    if ( !chr->isCustom() ) {
		if ( chr->c != '\n' ) {
		    paintStart = i;
		    paintEnd = i;
		} else {
		    paintStart = i+1;
		    paintEnd = -1;
		}
		formatChar = chr;
		lastY = cy;
		startX = chr->x;
		bw = cw;
	    } else {
		if ( chr->customItem()->placement() == QTextCustomItem::PlaceInline ) {
		    chr->customItem()->draw( &painter, chr->x, cy, clipx - r.x(), clipy - r.y(), clipw, cliph, cg,
					     nSels && selectionStarts[ 0 ] <= i && selectionEnds[ 0 ] >= i );
		    paintStart = i+1;
		    paintEnd = -1;
		    formatChar = chr;
		    lastY = cy;
		    startX = chr->x + string()->width( i );
		    bw = 0;
		} else {
		    chr->customItem()->resize( chr->customItem()->width );
		    paintStart = i+1;
		    paintEnd = -1;
		    formatChar = chr;
		    lastY = cy;
		    startX = chr->x + string()->width( i );
		    bw = 0;
		}
	    }
	} else {
	    if ( chr->c != '\n' ) {
		if( chr->rightToLeft ) {
		    startX = chr->x;
		}
		paintEnd = i;
	    }
	    bw += cw;
	}
	lastBaseLine = baseLine;
	lasth = h;
	lastDirection = chr->rightToLeft;
    }

    // if we are through the parag, but still have some stuff left to draw, draw it now
    if ( paintStart <= paintEnd ) {
	bool selectionChange = FALSE;
	if ( drawSelections ) {
	    for ( int j = 0; j < nSels; ++j ) {
		selectionChange = selectionStarts[ j ] == i || selectionEnds[ j ] == i;
		if ( selectionChange )
		    break;
	    }
	}
	int x = startX;
	drawParagString( painter, qstr, paintStart, paintEnd-paintStart+1, x, lastY,
			 lastBaseLine, bw, h, drawSelections,
			 formatChar, i, selectionStarts, selectionEnds, cg, lastDirection );
    }

    // if we should draw a cursor, draw it now
    if ( curx != -1 && cursor ) {
	painter.fillRect( QRect( curx, cury, 1, curh - lineSpacing() ), cg.color( QColorGroup::Text ) );
	painter.save();
	if ( string()->isBidi() ) {
	    const int d = 4;
	    if ( at( cursor->index() )->rightToLeft ) {
		painter.setPen( Qt::black );
		painter.drawLine( curx, cury, curx - d / 2, cury + d / 2 );
		painter.drawLine( curx, cury + d, curx - d / 2, cury + d / 2 );
	    } else {
		painter.setPen( Qt::black );
		painter.drawLine( curx, cury, curx + d / 2, cury + d / 2 );
		painter.drawLine( curx, cury + d, curx + d / 2, cury + d / 2 );
	    }
	}
	painter.restore();
    }
}

//#define BIDI_DEBUG

void QTextParag::drawParagString( QPainter &painter, const QString &s, int start, int len, int startX,
				      int lastY, int baseLine, int bw, int h, bool drawSelections,
				      QTextStringChar *formatChar, int i, const QMemArray<int> &selectionStarts,
				      const QMemArray<int> &selectionEnds, const QColorGroup &cg, bool rightToLeft )
{
    bool plainText = hasdoc ? document()->textFormat() == Qt::PlainText : FALSE;
    QTextFormat* format = formatChar->format();
    QString str( s );
    if ( str[ (int)str.length() - 1 ].unicode() == 0xad )
	str.remove( str.length() - 1, 1 );
    if ( !plainText || hasdoc && format->color() != document()->formatCollection()->defaultFormat()->color() )
	painter.setPen( QPen( format->color() ) );
    else
	painter.setPen( cg.text() );
    painter.setFont( format->font() );

    if ( hasdoc && formatChar->isAnchor() && !formatChar->anchorHref().isEmpty() ) {
	if ( format->useLinkColor() ) {
	    if ( document()->linkColor.isValid() )
		painter.setPen( document()->linkColor );
	    else
		painter.setPen( QPen( Qt::blue ) );
	}
	if ( document()->underlineLinks() ) {
	    QFont fn = format->font();
	    fn.setUnderline( TRUE );
	    painter.setFont( fn );
	}
    }

    if ( drawSelections ) {
	const int nSels = hasdoc ? document()->numSelections() : 1;
	const int startSel = is_printer( 0 ) ? 1 : 0;
	for ( int j = startSel; j < nSels; ++j ) {
	    if ( i > selectionStarts[ j ] && i <= selectionEnds[ j ] ) {
		if ( !hasdoc || document()->invertSelectionText( j ) )
		    painter.setPen( QPen( cg.color( QColorGroup::HighlightedText ) ) );
		if ( j == QTextDocument::Standard )
		    painter.fillRect( startX, lastY, bw, h, cg.color( QColorGroup::Highlight ) );
		else
		    painter.fillRect( startX, lastY, bw, h, hasdoc ? document()->selectionColor( j ) : cg.color( QColorGroup::Highlight ) );
	    }
	}
    }

    if ( str[ start ] != '\t' && str[ start ].unicode() != 0xad ) {
	if ( format->vAlign() == QTextFormat::AlignNormal ) {
	    painter.drawText( startX, lastY + baseLine, str.mid( start ), len );
#ifdef BIDI_DEBUG
	    painter.save();
	    painter.setPen ( Qt::red );
	    painter.drawLine( startX, lastY, startX, lastY + baseLine );
	    painter.drawLine( startX, lastY + baseLine/2, startX + 10, lastY + baseLine/2 );
	    int w = 0;
	    int i = 0;
	    while( i < len )
		w += painter.fontMetrics().charWidth( str, start + i++ );
	    painter.setPen ( Qt::blue );
	    painter.drawLine( startX + w - 1, lastY, startX + w - 1, lastY + baseLine );
	    painter.drawLine( startX + w - 1, lastY + baseLine/2, startX + w - 1 - 10, lastY + baseLine/2 );
	    painter.restore();
#endif
	} else if ( format->vAlign() == QTextFormat::AlignSuperScript ) {
	    QFont f( painter.font() );
	    if ( format->fontSizesInPixels() )
		f.setPixelSize( ( f.pixelSize() * 2 ) / 3 );
	    else
		f.setPointSize( ( f.pointSize() * 2 ) / 3 );
	    painter.setFont( f );
	    painter.drawText( startX, lastY + baseLine - ( painter.fontMetrics().height() / 2 ),
			      str.mid( start ), len );
	} else if ( format->vAlign() == QTextFormat::AlignSubScript ) {
	    QFont f( painter.font() );
	    if ( format->fontSizesInPixels() )
		f.setPixelSize( ( f.pixelSize() * 2 ) / 3 );
	    else
		f.setPointSize( ( f.pointSize() * 2 ) / 3 );
	    painter.setFont( f );
	    painter.drawText( startX, lastY + baseLine + painter.fontMetrics().height() / 6, str.mid( start ), len );
	}
    }
    if ( i + 1 < length() && at( i + 1 )->lineStart && at( i )->c.unicode() == 0xad ) {
	painter.drawText( startX + bw, lastY + baseLine, "\xad" );
    }
    if ( format->isMisspelled() ) {
	painter.save();
	painter.setPen( QPen( Qt::red, 1, Qt::DotLine ) );
	painter.drawLine( startX, lastY + baseLine + 1, startX + bw, lastY + baseLine + 1 );
	painter.restore();
    }

    i -= len;

    if ( hasdoc && formatChar->isAnchor() && !formatChar->anchorHref().isEmpty() &&
	 document()->focusIndicator.parag == this &&
	 ( document()->focusIndicator.start >= i  &&
	   document()->focusIndicator.start + document()->focusIndicator.len <= i + len ||
	   document()->focusIndicator.start <= i &&
	   document()->focusIndicator.start + document()->focusIndicator.len >= i + len ) ) {
	painter.drawWinFocusRect( QRect( startX, lastY, bw, h ) );
    }

}

void QTextParag::drawLabel( QPainter* p, int x, int y, int w, int h, int base, const QColorGroup& cg )
{
    if ( !style() )
	return;
    QRect r ( x, y, w, h );
    QStyleSheetItem::ListStyle s = listStyle();

    p->save();
    p->setPen( defFormat->color() );

    QFont font2( defFormat->font() );
    if ( length() > 0 ) {
	QTextFormat *format = at( 0 )->format();
	if ( format ) {
	    if ( format->fontSizesInPixels() )
		font2.setPixelSize( at( 0 )->format()->font().pixelSize() );
	    else
		font2.setPointSize( at( 0 )->format()->font().pointSize() );
	}
    }
    p->setFont( font2 );
    QFontMetrics fm( p->fontMetrics() );
    int size = fm.lineSpacing() / 3;

    switch ( s ) {
    case QStyleSheetItem::ListDecimal:
    case QStyleSheetItem::ListLowerAlpha:
    case QStyleSheetItem::ListUpperAlpha:
	{
	    int n = numberOfSubParagraph();
	    QString l;
	    switch ( s ) {
	    case QStyleSheetItem::ListLowerAlpha:
		if ( n < 27 ) {
		    l = QChar( ('a' + (char) (n-1)));
		    break;
		}
	    case QStyleSheetItem::ListUpperAlpha:
		if ( n < 27 ) {
		    l = QChar( ('A' + (char) (n-1)));
		    break;
		}
		break;
	    default:  //QStyleSheetItem::ListDecimal:
		l.setNum( n );
		break;
	    }
	    l += QString::fromLatin1(". ");
	    p->drawText( r.right() - fm.width( l ), r.top() + base, l );
	}
	break;
    case QStyleSheetItem::ListSquare:
	{
	    QRect er( r.right() - size * 2, r.top() + fm.height() / 2 - size / 2, size, size );
	    p->fillRect( er , cg.brush( QColorGroup::Foreground ) );
	}
	break;
    case QStyleSheetItem::ListCircle:
	{
	    QRect er( r.right()-size*2, r.top() + fm.height() / 2 - size / 2, size, size);
	    p->drawEllipse( er );
	}
	break;
    case QStyleSheetItem::ListDisc:
    default:
	{
	    p->setBrush( cg.brush( QColorGroup::Foreground ));
	    QRect er( r.right()-size*2, r.top() + fm.height() / 2 - size / 2, size, size);
	    p->drawEllipse( er );
	    p->setBrush( Qt::NoBrush );
	}
	break;
    }

    p->restore();
}

void QTextParag::setStyleSheetItems( const QPtrVector<QStyleSheetItem> &vec )
{
    styleSheetItemsVec() = vec;
    invalidate( 0 );
    lm = rm = tm = bm = flm = -1;
    numSubParag = -1;
}

void QTextParag::setList( bool b, int listStyle )
{
    if ( !hasdoc )
	return;

    if ( !style() ) {
	styleSheetItemsVec().resize( 2 );
	mStyleSheetItemsVec->insert( 0, document()->styleSheet()->item( "html" ) );
	mStyleSheetItemsVec->insert( 1, document()->styleSheet()->item( "p" ) );
    }

    if ( b ) {
	if ( style()->displayMode() != QStyleSheetItem::DisplayListItem || this->listStyle() != listStyle ) {
	    styleSheetItemsVec().remove( styleSheetItemsVec().size() - 1 );
	    QStyleSheetItem *item = (*mStyleSheetItemsVec)[ mStyleSheetItemsVec->size() - 1 ];
	    if ( item )
		mStyleSheetItemsVec->remove( mStyleSheetItemsVec->size() - 1 );
	    mStyleSheetItemsVec->insert( mStyleSheetItemsVec->size() - 1,
				       listStyle == QStyleSheetItem::ListDisc || listStyle == QStyleSheetItem::ListCircle
				       || listStyle == QStyleSheetItem::ListSquare ?
				       document()->styleSheet()->item( "ul" ) : document()->styleSheet()->item( "ol" ) );
	    mStyleSheetItemsVec->insert( mStyleSheetItemsVec->size() - 1, document()->styleSheet()->item( "li" ) );
	    setListStyle( (QStyleSheetItem::ListStyle)listStyle );
	} else {
	    return;
	}
    } else {
	if ( style()->displayMode() != QStyleSheetItem::DisplayBlock ) {
	    styleSheetItemsVec().remove( styleSheetItemsVec().size() - 1 );
	    if ( mStyleSheetItemsVec->size() >= 2 ) {
		mStyleSheetItemsVec->remove( mStyleSheetItemsVec->size() - 2 );
		mStyleSheetItemsVec->resize( mStyleSheetItemsVec->size() - 2 );
	    } else {
		mStyleSheetItemsVec->resize( mStyleSheetItemsVec->size() - 1 );
	    }
	} else {
	    return;
	}
    }
    invalidate( 0 );
    lm = rm = tm = bm = flm = -1;
    numSubParag = -1;
    if ( next() ) {
	QTextParag *s = next();
	while ( s ) {
	    s->numSubParag = -1;
	    s->lm = s->rm = s->tm = s->bm = flm = -1;
	    s->numSubParag = -1;
	    s->invalidate( 0 );
	    s = s->next();
	}
    }
}

void QTextParag::incDepth()
{
    if ( !style() || !hasdoc )
	return;
    if ( style()->displayMode() != QStyleSheetItem::DisplayListItem )
	return;
    styleSheetItemsVec().resize( styleSheetItemsVec().size() + 1 );
    mStyleSheetItemsVec->insert( mStyleSheetItemsVec->size() - 1, (*mStyleSheetItemsVec)[ mStyleSheetItemsVec->size() - 2 ] );
    mStyleSheetItemsVec->insert( mStyleSheetItemsVec->size() - 2,
			       listStyle() == QStyleSheetItem::ListDisc || listStyle() == QStyleSheetItem::ListCircle ||
			       listStyle() == QStyleSheetItem::ListSquare ?
			       document()->styleSheet()->item( "ul" ) : document()->styleSheet()->item( "ol" ) );
    invalidate( 0 );
    lm = -1;
    flm = -1;
}

void QTextParag::decDepth()
{
    if ( !style() || !hasdoc )
	return;
    if ( style()->displayMode() != QStyleSheetItem::DisplayListItem )
	return;
    int numLists = 0;
    QStyleSheetItem *lastList = 0;
    int lastIndex = 0;
    int i;
    if ( mStyleSheetItemsVec ) {
	for ( i = 0; i < (int)mStyleSheetItemsVec->size(); ++i ) {
	    QStyleSheetItem *item = (*mStyleSheetItemsVec)[ i ];
	    if ( item->name() == "ol" || item->name() == "ul" ) {
		lastList = item;
		lastIndex = i;
		numLists++;
	    }
	}
    }

    if ( !lastList )
	return;
    styleSheetItemsVec().remove( lastIndex );
    for ( i = lastIndex; i < (int)mStyleSheetItemsVec->size() - 1; ++i )
	mStyleSheetItemsVec->insert( i, (*mStyleSheetItemsVec)[ i + 1 ] );
    mStyleSheetItemsVec->resize( mStyleSheetItemsVec->size() - 1 );
    if ( numLists == 1 )
	setList( FALSE, -1 );
    invalidate( 0 );
    lm = -1;
    flm = -1;
}

int QTextParag::listDepth() const
{
    int numLists = 0;
    int i;
    if ( mStyleSheetItemsVec ) {
	for ( i = 0; i < (int)mStyleSheetItemsVec->size(); ++i ) {
	    QStyleSheetItem *item = (*mStyleSheetItemsVec)[ i ];
	    if ( item->name() == "ol" || item->name() == "ul" )
		numLists++;
	}
    }
    return numLists - 1;
}

int *QTextParag::tabArray() const
{
    int *ta = tArray;
    if ( !ta && hasdoc )
	ta = document()->tabArray();
    return ta;
}

int QTextParag::nextTab( int, int x )
{
    int *ta = tArray;
    if ( hasdoc ) {
	if ( !ta )
	    ta = document()->tabArray();
	tabStopWidth = document()->tabStopWidth();
    }
    if ( ta ) {
	int i = 0;
	while ( ta[ i ] ) {
	    if ( ta[ i ] >= x )
		return tArray[ i ];
	    ++i;
	}
	return tArray[ 0 ];
    } else {
	int d;
	if ( tabStopWidth != 0 )
	    d = x / tabStopWidth;
	else
	    return x;
	return tabStopWidth * ( d + 1 );
    }
}

void QTextParag::adjustToPainter( QPainter *p )
{
    for ( int i = 0; i < length(); ++i ) {
	if ( at( i )->isCustom() )
	    at( i )->customItem()->adjustToPainter( p );
    }
}

QTextFormatCollection *QTextParag::formatCollection() const
{
    if ( hasdoc )
	return document()->formatCollection();
    if ( !qFormatCollection ) {
	qFormatCollection = new QTextFormatCollection;
	static QSingleCleanupHandler<QTextFormatCollection> qtfCleanup;
	qtfCleanup.set( &qFormatCollection );
    }
    return qFormatCollection;
}

QString QTextParag::richText() const
{
    QString s;
    QTextStringChar *formatChar = 0;
    QString spaces;
    bool lastCharWasSpace = FALSE;
    int firstcol = 0;
    for ( int i = 0; i < length()-1; ++i ) {
	QTextStringChar *c = &str->at( i );
	if ( c->isAnchor() && !c->anchorName().isEmpty() ) {
	    if ( c->anchorName().contains( '#' ) ) {
		QStringList l = QStringList::split( '#', c->anchorName() );
		for ( QStringList::ConstIterator it = l.begin(); it != l.end(); ++it )
		    s += "<a name=\"" + *it + "\"></a>";
	    } else {
		s += "<a name=\"" + c->anchorName() + "\"></a>";
	    }
	}
	if ( !formatChar ) {
	    s += c->format()->makeFormatChangeTags( 0, QString::null, c->anchorHref() );
	    formatChar = c;
	} else if ( ( formatChar->format()->key() != c->format()->key() ) ||
		  (formatChar->isAnchor() != c->isAnchor() &&
		   (!c->anchorHref().isEmpty() || !formatChar->anchorHref().isEmpty() ) ) )  {// lisp was here

	    if ( !spaces.isEmpty() ) {
		if (  spaces[0] == '\t' || lastCharWasSpace  )
		    s += "<wsp>" + spaces + "</wsp>";
		else if ( spaces.length() > 1 )
		    s += "<wsp>" + spaces.mid(1) + "</wsp> ";
		else
		    s += spaces;
		lastCharWasSpace = TRUE;
		spaces = QString::null;
	    }
	    s += c->format()->makeFormatChangeTags( formatChar->format() , formatChar->anchorHref(), c->anchorHref() );
	    formatChar = c;
	}

	if ( c->c == ' ' || c->c == '\t' ) {
	    spaces += c->c;
	    continue;
	} else if ( !spaces.isEmpty() ) {
	    if (  spaces[0] == '\t' || lastCharWasSpace  )
		s += "<wsp>" + spaces + "</wsp>";
	    else if ( spaces.length() > 1 )
		s += "<wsp>" + spaces.mid(1) + "</wsp> ";
	    else
		s += spaces;
	    spaces = QString::null;
	    if ( s.length() - firstcol > 60 ) {
		s += '\n';
		firstcol = s.length();
	    }
	}

	lastCharWasSpace = FALSE;
	if ( c->c == '<' ) {
	    s += "&lt;";
	} else if ( c->c == '>' ) {
	    s += "&gt;";
	} else if ( c->isCustom() ) {
	    s += c->customItem()->richText();
	} else {
	    s += c->c;
	}
    }
    if ( !spaces.isEmpty() ) {
	if ( spaces.length() > 1 || spaces[0] == '\t' || lastCharWasSpace )
		s += "<wsp>" + spaces + "</wsp>";
	else
	    s += spaces;
    }

    if ( formatChar )
	s += formatChar->format()->makeFormatEndTags( formatChar->anchorHref() );
    return s;
}

void QTextParag::addCommand( QTextCommand *cmd )
{
    if ( !hasdoc )
	pseudoDocument()->commandHistory->addCommand( cmd );
    else
	document()->commands()->addCommand( cmd );
}

QTextCursor *QTextParag::undo( QTextCursor *c )
{
    if ( !hasdoc )
	return pseudoDocument()->commandHistory->undo( c );
    return document()->commands()->undo( c );
}

QTextCursor *QTextParag::redo( QTextCursor *c )
{
    if ( !hasdoc )
	return pseudoDocument()->commandHistory->redo( c );
    return document()->commands()->redo( c );
}

int QTextParag::topMargin() const
{
    if ( !p && ( !hasdoc || !document()->addMargins() ) )
	return 0;
    if ( tm != -1 )
	return tm;
    QStyleSheetItem *item = style();
    if ( !item ) {
	( (QTextParag*)this )->tm = 0;
	return 0;
    }

    int m = 0;
    if ( item->margin( QStyleSheetItem::MarginTop ) != QStyleSheetItem::Undefined )
	m = item->margin( QStyleSheetItem::MarginTop );
    if ( mStyleSheetItemsVec ) {
	QStyleSheetItem *it = 0;
	QStyleSheetItem *p = prev() ? prev()->style() : 0;
	for ( int i = (int)mStyleSheetItemsVec->size() - 2 ; i >= 0; --i ) {
	    it = (*mStyleSheetItemsVec)[ i ];
	    if ( it != p )
		break;
	    int mar = it->margin( QStyleSheetItem::MarginTop );
	    m += (mar != QStyleSheetItem::Undefined) ? mar : 0;
	    if ( it->displayMode() != QStyleSheetItem::DisplayInline )
		break;
	}
    }
    m = scale( m, QTextFormat::painter() );

    ( (QTextParag*)this )->tm = m;
    return tm;
}

int QTextParag::bottomMargin() const
{
    if ( bm != -1 )
	return bm;
    QStyleSheetItem *item = style();
    if ( !item || !next() ) {
	( (QTextParag*)this )->bm = 0;
	return 0;
    }

    int m = 0;
    if ( item->margin( QStyleSheetItem::MarginBottom ) != QStyleSheetItem::Undefined )
	m = item->margin( QStyleSheetItem::MarginBottom );
    if ( mStyleSheetItemsVec ) {
	QStyleSheetItem *it = 0;
	QStyleSheetItem *n = next() ? next()->style() : 0;
	for ( int i =(int)mStyleSheetItemsVec->size() - 2 ; i >= 0; --i ) {
	    it = (*mStyleSheetItemsVec)[ i ];
	    if ( it != n )
		break;
	    int mar = it->margin( QStyleSheetItem::MarginBottom );
	    m += mar != QStyleSheetItem::Undefined ? mar : 0;
	    if ( it->displayMode() != QStyleSheetItem::DisplayInline )
		break;
	}
    }
    m = scale ( m, QTextFormat::painter() );

    ( (QTextParag*)this )->bm = m;
    return bm;
}

int QTextParag::leftMargin() const
{
    if ( lm != -1 )
	return lm;
    QStyleSheetItem *item = style();
    if ( !item ) {
	( (QTextParag*)this )->lm = 0;
	return 0;
    }
    int m = 0;
    if ( mStyleSheetItemsVec ) {
	for ( int i = 0; i < (int)mStyleSheetItemsVec->size(); ++i ) {
	    item = (*mStyleSheetItemsVec)[ i ];
	    int mar = item->margin( QStyleSheetItem::MarginLeft );
	    m += mar != QStyleSheetItem::Undefined ? mar : 0;
	    if ( item->name() == "ol" || item->name() == "ul" ) {
		QPainter* oldPainter = QTextFormat::painter();
		QTextFormat::setPainter( 0 );
		m += defFormat->width( '1' ) +
		     defFormat->width( '2' ) +
		     defFormat->width( '3' ) +
		     defFormat->width( '.' );
		QTextFormat::setPainter( oldPainter );
	    }
	}
    }

    m = scale ( m, QTextFormat::painter() );

    ( (QTextParag*)this )->lm = m;
    return lm;
}

int QTextParag::firstLineMargin() const
{
    if ( flm != -1 )
	return lm;
    QStyleSheetItem *item = style();
    if ( !item ) {
	( (QTextParag*)this )->flm = 0;
	return 0;
    }
    int m = 0;
    if ( mStyleSheetItemsVec ) {
	for ( int i = 0; i < (int)mStyleSheetItemsVec->size(); ++i ) {
	    item = (*mStyleSheetItemsVec)[ i ];
	    int mar = item->margin( QStyleSheetItem::MarginFirstLine );
	    m += mar != QStyleSheetItem::Undefined ? mar : 0;
	}
    }

    m = scale( m, QTextFormat::painter() );

    ( (QTextParag*)this )->flm = m;
    return flm;
}

int QTextParag::rightMargin() const
{
    if ( rm != -1 )
	return rm;
    QStyleSheetItem *item = style();
    if ( !item ) {
	( (QTextParag*)this )->rm = 0;
	return 0;
    }
    int m = 0;
    if ( mStyleSheetItemsVec ) {
	for ( int i = 0; i < (int)mStyleSheetItemsVec->size(); ++i ) {
	    item = (*mStyleSheetItemsVec)[ i ];
	    int mar = item->margin( QStyleSheetItem::MarginRight );
	    m += mar != QStyleSheetItem::Undefined ? mar : 0;
	}
    }
    m = scale( m, QTextFormat::painter() );

    ( (QTextParag*)this )->rm = m;
    return rm;
}

int QTextParag::lineSpacing() const
{
    QStyleSheetItem *item = style();
    if ( !item )
	return 0;

    int ls = item->lineSpacing();
    if ( ls == QStyleSheetItem::Undefined )
	return 0;
    ls = scale( ls, QTextFormat::painter() );

    return ls;
}

void QTextParag::copyParagData( QTextParag *parag )
{
    setStyleSheetItems( parag->styleSheetItems() );
    setListStyle( parag->listStyle() );
    setAlignment( parag->alignment() );
    QColor *c = parag->backgroundColor();
    if ( c )
	setBackgroundColor( *c );
}

void QTextParag::show()
{
    if ( visible || !hasdoc )
	return;
    visible = TRUE;
}

void QTextParag::hide()
{
    if ( !visible || !hasdoc )
	return;
    visible = FALSE;
}

void QTextParag::setDirection( QChar::Direction d )
{
    if ( str && str->direction() != d ) {
	str->setDirection( d );
	invalidate( 0 );
    }
}

QChar::Direction QTextParag::direction() const
{
    return (str ? str->direction() : QChar::DirON );
}

void QTextParag::setChanged( bool b, bool recursive )
{
    changed = b;
    if ( recursive ) {
	if ( document() && document()->parentParag() )
	    document()->parentParag()->setChanged( b, recursive );
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


QTextPreProcessor::QTextPreProcessor()
{
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

QTextFormatter::QTextFormatter()
    : thisminw(0), thiswused(0), wrapEnabled( TRUE ), wrapColumn( -1 ), biw( FALSE )
{
}

/* only used for bidi or complex text reordering
 */
QTextParagLineStart *QTextFormatter::formatLine( QTextParag *parag, QTextString *string, QTextParagLineStart *line,
						   QTextStringChar *startChar, QTextStringChar *lastChar, int align, int space )
{
#ifndef QT_NO_COMPLEXTEXT
    if( string->isBidi() )
	return bidiReorderLine( parag, string, line, startChar, lastChar, align, space );
#endif
    space = QMAX( space, 0 ); // #### with nested tables this gets negative because of a bug I didn't find yet, so workaround for now. This also means non-left aligned nested tables do not work at the moment
    int start = (startChar - &string->at(0));
    int last = (lastChar - &string->at(0) );
    // do alignment Auto == Left in this case
    if ( align & Qt::AlignHCenter || align & Qt::AlignRight ) {
	if ( align & Qt::AlignHCenter )
	    space /= 2;
	for ( int j = start; j <= last; ++j )
	    string->at( j ).x += space;
    } else if ( align & Qt3::AlignJustify ) {
	int numSpaces = 0;
	for ( int j = start; j < last; ++j ) {
	    if( isBreakable( string, j ) ) {
		numSpaces++;
	    }
	}
	int toAdd = 0;
	for ( int k = start + 1; k <= last; ++k ) {
	    if( isBreakable( string, k ) && numSpaces ) {
		int s = space / numSpaces;
		toAdd += s;
		space -= s;
		numSpaces--;
	    }
	    string->at( k ).x += toAdd;
	}
    }

    if ( last >= 0 && last < string->length() )
	line->w = string->at( last ).x + string->width( last );
    else
	line->w = 0;

    return new QTextParagLineStart();
}

#ifndef QT_NO_COMPLEXTEXT

#ifdef BIDI_DEBUG
#include <iostream>
#endif

// collects one line of the paragraph and transforms it to visual order
QTextParagLineStart *QTextFormatter::bidiReorderLine( QTextParag * /*parag*/, QTextString *text, QTextParagLineStart *line,
							QTextStringChar *startChar, QTextStringChar *lastChar, int align, int space )
{
    int start = (startChar - &text->at(0));
    int last = (lastChar - &text->at(0) );
    //qDebug("doing BiDi reordering from %d to %d!", start, last);

    QBidiControl *control = new QBidiControl( line->context(), line->status );
    QString str;
    str.setUnicode( 0, last - start + 1 );
    // fill string with logically ordered chars.
    QTextStringChar *ch = startChar;
    QChar *qch = (QChar *)str.unicode();
    while ( ch <= lastChar ) {
	*qch = ch->c;
	qch++;
	ch++;
    }
    int x = startChar->x;

    QPtrList<QTextRun> *runs;
    runs = QComplexText::bidiReorderLine(control, str, 0, last - start + 1,
					 (text->isRightToLeft() ? QChar::DirR : QChar::DirL) );

    // now construct the reordered string out of the runs...

    int numSpaces = 0;
    // set the correct alignment. This is a bit messy....
    if( align == Qt3::AlignAuto ) {
	// align according to directionality of the paragraph...
	if ( text->isRightToLeft() )
	    align = Qt::AlignRight;
    }

    if ( align & Qt::AlignHCenter )
	x += space/2;
    else if ( align & Qt::AlignRight )
	x += space;
    else if ( align & Qt3::AlignJustify ) {
	for ( int j = start; j < last; ++j ) {
	    if( isBreakable( text, j ) ) {
		numSpaces++;
	    }
	}
    }
    int toAdd = 0;
    bool first = TRUE;
    QTextRun *r = runs->first();
    int xmax = -0xffffff;
    while ( r ) {
	if(r->level %2) {
	    // odd level, need to reverse the string
	    int pos = r->stop + start;
	    while(pos >= r->start + start) {
		QTextStringChar *c = &text->at(pos);
		if( numSpaces && !first && isBreakable( text, pos ) ) {
		    int s = space / numSpaces;
		    toAdd += s;
		    space -= s;
		    numSpaces--;
		} else if ( first ) {
		    first = FALSE;
		    if ( c->c == ' ' )
			x -= c->format()->width( ' ' );
		}
		c->x = x + toAdd;
		c->rightToLeft = TRUE;
		c->startOfRun = FALSE;
		int ww = 0;
		if ( c->c.unicode() >= 32 || c->c == '\t' || c->c == '\n' || c->isCustom() ) {
		    ww = text->width( pos );
		} else {
		    ww = c->format()->width( ' ' );
		}
		if ( xmax < x + toAdd + ww ) xmax = x + toAdd + ww;
		x += ww;
		pos--;
	    }
	} else {
	    int pos = r->start + start;
	    while(pos <= r->stop + start) {
		QTextStringChar* c = &text->at(pos);
		if( numSpaces && !first && isBreakable( text, pos ) ) {
		    int s = space / numSpaces;
		    toAdd += s;
		    space -= s;
		    numSpaces--;
		} else if ( first ) {
		    first = FALSE;
		    if ( c->c == ' ' )
			x -= c->format()->width( ' ' );
		}
		c->x = x + toAdd;
		c->rightToLeft = FALSE;
		c->startOfRun = FALSE;
		int ww = 0;
		if ( c->c.unicode() >= 32 || c->c == '\t' || c->isCustom() ) {
		    ww = text->width( pos );
		} else {
		    ww = c->format()->width( ' ' );
		}
		//qDebug("setting char %d at pos %d", pos, x);
		if ( xmax < x + toAdd + ww ) xmax = x + toAdd + ww;
		x += ww;
		pos++;
	    }
	}
	text->at( r->start + start ).startOfRun = TRUE;
	r = runs->next();
    }

    line->w = xmax + 10;
    QTextParagLineStart *ls = new QTextParagLineStart( control->context, control->status );
    delete control;
    delete runs;
    return ls;
}
#endif

bool QTextFormatter::isBreakable( QTextString *string, int pos ) const
{
    const QChar &c = string->at( pos ).c;
    char ch = c.latin1();
    if ( c.isSpace() && ch != '\n' && c.unicode() != 0x00a0U )
	return TRUE;
    if ( c.unicode() == 0xad ) // soft hyphen
	return TRUE;
    if ( !ch ) {
	// not latin1, need to do more sophisticated checks for other scripts
	uchar row = c.row();
	if ( row == 0x0e ) {
	    // 0e00 - 0e7f == Thai
	    if ( c.cell() < 0x80 ) {
#ifdef HAVE_THAI_BREAKS
		// check for thai
		if( string != cachedString ) {
		    // build up string of thai chars
		    QTextCodec *thaiCodec = QTextCodec::codecForMib(2259);
		    if ( !thaiCache )
			thaiCache = new QCString;
		    if ( !thaiIt )
			thaiIt = ThBreakIterator::createWordInstance();
		    *thaiCache = thaiCodec->fromUnicode( s->string() );
		}
		thaiIt->setText(thaiCache->data());
		for(int i = thaiIt->first(); i != thaiIt->DONE; i = thaiIt->next() ) {
		    if( i == pos )
			return TRUE;
		    if( i > pos )
			return FALSE;
		}
		return FALSE;
#else
		// if we don't have a thai line breaking lib, allow
		// breaks everywhere except directly before punctuation.
		return TRUE;
#endif
	    } else
		return FALSE;
	}
	if ( row < 0x11 ) // no asian font
	    return FALSE;
	if ( row > 0x2d && row < 0xfb || row == 0x11 )
	    // asian line breaking. Everywhere allowed except directly
	    // in front of a punctuation character.
	    return TRUE;
    }
    return FALSE;
}

void QTextFormatter::insertLineStart( QTextParag *parag, int index, QTextParagLineStart *ls )
{
    if ( index > 0 ) { // we can assume that only first line starts are insrted multiple times
	parag->lineStartList().insert( index, ls );
	return;
    }
    QMap<int, QTextParagLineStart*>::Iterator it;
    if ( ( it = parag->lineStartList().find( index ) ) == parag->lineStartList().end() ) {
	parag->lineStartList().insert( index, ls );
    } else {
	delete *it;
	parag->lineStartList().remove( it );
	parag->lineStartList().insert( index, ls );
    }
}


/* Standard pagebreak algorithm using QTextFlow::adjustFlow. Returns
 the shift of the paragraphs bottom line.
 */
int QTextFormatter::formatVertically( QTextDocument* doc, QTextParag* parag )
{
    int oldHeight = parag->rect().height();
    QMap<int, QTextParagLineStart*>& lineStarts = parag->lineStartList();
    QMap<int, QTextParagLineStart*>::Iterator it = lineStarts.begin();
    int h = doc->addMargins() ? parag->topMargin() : 0;
    for ( ; it != lineStarts.end() ; ++it  ) {
	QTextParagLineStart * ls = it.data();
	ls->y = h;
	QTextStringChar *c = &parag->string()->at(it.key());
	if ( c && c->customItem() && c->customItem()->ownLine() ) {
	    int h = c->customItem()->height;
	    c->customItem()->pageBreak( parag->rect().y() + ls->y + ls->baseLine - h, doc->flow() );
	    int delta = c->customItem()->height - h;
	    ls->h += delta;
	    if ( delta )
		parag->setMovedDown( TRUE );
	} else {
	    int shift = doc->flow()->adjustFlow( parag->rect().y() + ls->y, ls->w, ls->h );
	    ls->y += shift;
	    if ( shift )
		parag->setMovedDown( TRUE );
	}
	h = ls->y + ls->h;
    }
    int m = parag->bottomMargin();
    if ( parag->next() && doc && !doc->addMargins() )
	m = QMAX( m, parag->next()->topMargin() );
    if ( parag->next() && parag->next()->isLineBreak() )
	m = 0;
    h += m;
    parag->setHeight( h );
    return h - oldHeight;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

QTextFormatterBreakInWords::QTextFormatterBreakInWords()
{
}

int QTextFormatterBreakInWords::format( QTextDocument *doc,QTextParag *parag,
					int start, const QMap<int, QTextParagLineStart*> & )
{
    QTextStringChar *c = 0;
    QTextStringChar *firstChar = 0;
    int left = doc ? parag->leftMargin() + doc->leftMargin() : 4;
    int x = left + ( doc ? parag->firstLineMargin() : 0 );
    int dw = parag->documentVisibleWidth() - ( doc ? doc->rightMargin() : 0 );
    int y = doc && doc->addMargins() ? parag->topMargin() : 0;
    int h = y;
    int len = parag->length();
    if ( doc )
	x = doc->flow()->adjustLMargin( y + parag->rect().y(), parag->rect().height(), x, 4 );
    int rm = parag->rightMargin();
    int w = dw - ( doc ? doc->flow()->adjustRMargin( y + parag->rect().y(), parag->rect().height(), rm, 4 ) : 0 );
    bool fullWidth = TRUE;
    int minw = 0;
    int wused = 0;
    bool wrapEnabled = isWrapEnabled( parag );

    start = 0;    //######### what is the point with start?! (Matthias)
    if ( start == 0 )
	c = &parag->string()->at( 0 );

    int i = start;
    QTextParagLineStart *lineStart = new QTextParagLineStart( y, y, 0 );
    insertLineStart( parag, 0, lineStart );

    QPainter *painter = QTextFormat::painter();

    int col = 0;
    int ww = 0;
    QChar lastChr;
    for ( ; i < len; ++i, ++col ) {
	if ( c )
	    lastChr = c->c;
	c = &parag->string()->at( i );
	c->rightToLeft = FALSE;
	// ### the lines below should not be needed
	if ( painter )
	    c->format()->setPainter( painter );
	if ( i > 0 ) {
	    c->lineStart = 0;
	} else {
	    c->lineStart = 1;
	    firstChar = c;
	}
	if ( c->c.unicode() >= 32 || c->isCustom() ) {
	    ww = parag->string()->width( i );
	} else if ( c->c == '\t' ) {
	    int nx = parag->nextTab( i, x - left ) + left;
	    if ( nx < x )
		ww = w - x;
	    else
		ww = nx - x;
	} else {
	    ww = c->format()->width( ' ' );
	}

	if ( c->isCustom() && c->customItem()->ownLine() ) {
	    x = doc ? doc->flow()->adjustLMargin( y + parag->rect().y(), parag->rect().height(), left, 4 ) : left;
	    w = dw - ( doc ? doc->flow()->adjustRMargin( y + parag->rect().y(), parag->rect().height(), rm, 4 ) : 0 );
	    c->customItem()->resize( w - x );
	    w = dw;
	    y += h;
	    h = c->height();
	    lineStart = new QTextParagLineStart( y, h, h );
	    insertLineStart( parag, i, lineStart );
	    c->lineStart = 1;
	    firstChar = c;
	    x = 0xffffff;
	    continue;
	}

	if ( wrapEnabled &&
	     ( wrapAtColumn() == -1 && x + ww > w ||
	       wrapAtColumn() != -1 && col >= wrapAtColumn() ) ||
	       parag->isNewLinesAllowed() && lastChr == '\n' ) {
	    x = doc ? parag->document()->flow()->adjustLMargin( y + parag->rect().y(), parag->rect().height(), left, 4 ) : left;
	    w = dw;
	    y += h;
	    h = c->height();
	    lineStart = formatLine( parag, parag->string(), lineStart, firstChar, c-1 );
	    lineStart->y = y;
	    insertLineStart( parag, i, lineStart );
	    lineStart->baseLine = c->ascent();
	    lineStart->h = c->height();
	    c->lineStart = 1;
	    firstChar = c;
	    col = 0;
	    if ( wrapAtColumn() != -1 )
		minw = QMAX( minw, w );
	} else if ( lineStart ) {
	    lineStart->baseLine = QMAX( lineStart->baseLine, c->ascent() );
	    h = QMAX( h, c->height() );
	    lineStart->h = h;
	}

	c->x = x;
	x += ww;
	wused = QMAX( wused, x );
    }

    int m = parag->bottomMargin();
    if ( parag->next() && doc && !doc->addMargins() )
	m = QMAX( m, parag->next()->topMargin() );
    parag->setFullWidth( fullWidth );
    if ( parag->next() && parag->next()->isLineBreak() )
	m = 0;
    y += h + m;
    if ( !wrapEnabled )
	minw = QMAX(minw, wused);

    thisminw = minw;
    thiswused = wused;
    return y;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

QTextFormatterBreakWords::QTextFormatterBreakWords()
{
}

#define DO_FLOW( lineStart ) do{ if ( doc && doc->isPageBreakEnabled() ) { \
		    int yflow = lineStart->y + parag->rect().y();\
		    int shift = doc->flow()->adjustFlow( yflow, dw, lineStart->h ); \
		    lineStart->y += shift;\
		    y += shift;\
		}}while(FALSE)

int QTextFormatterBreakWords::format( QTextDocument *doc, QTextParag *parag,
				      int start, const QMap<int, QTextParagLineStart*> & )
{
    QTextStringChar *c = 0;
    QTextStringChar *firstChar = 0;
    QTextString *string = parag->string();
    int left = doc ? parag->leftMargin() + doc->leftMargin() : 0;
    int x = left + ( doc ? parag->firstLineMargin() : 0 );
    int y = doc && doc->addMargins() ? parag->topMargin() : 0;
    int h = y;
    int len = parag->length();
    if ( doc )
	x = doc->flow()->adjustLMargin( y + parag->rect().y(), parag->rect().height(), x, 0 );
    int dw = parag->documentVisibleWidth() - ( doc ? ( left != x ? 0 : doc->rightMargin() ) : 0 );

    int curLeft = x;
    int rm = parag->rightMargin();
    int rdiff = doc ? doc->flow()->adjustRMargin( y + parag->rect().y(), parag->rect().height(), rm, 0 ) : 0;
    int w = dw - rdiff;
    bool fullWidth = TRUE;
    int marg = left + rdiff;
    int minw = 0;
    int wused = 0;
    int tminw = marg;
    int linespace = doc ? parag->lineSpacing() : 0;
    bool wrapEnabled = isWrapEnabled( parag );

    start = 0;
    if ( start == 0 )
	c = &parag->string()->at( 0 );

    int i = start;
    QTextParagLineStart *lineStart = new QTextParagLineStart( y, y, 0 );
    insertLineStart( parag, 0, lineStart );
    int lastBreak = -1;
    int tmpBaseLine = 0, tmph = 0;
    bool lastWasNonInlineCustom = FALSE;

    int align = parag->alignment();
    if ( align == Qt3::AlignAuto && doc && doc->alignment() != Qt3::AlignAuto )
	align = doc->alignment();

    align &= Qt3::AlignHorizontal_Mask;

    QPainter *painter = QTextFormat::painter();
    int col = 0;
    int ww = 0;
    QChar lastChr;
    for ( ; i < len; ++i, ++col ) {
	if ( c )
	    lastChr = c->c;
	// ### next line should not be needed
	if ( painter )
	    c->format()->setPainter( painter );
	c = &string->at( i );
	c->rightToLeft = FALSE;
	if ( i > 0 && (x > curLeft || ww == 0) || lastWasNonInlineCustom ) {
	    c->lineStart = 0;
	} else {
	    c->lineStart = 1;
	    firstChar = c;
	}

	if ( c->isCustom() && c->customItem()->placement() != QTextCustomItem::PlaceInline )
	    lastWasNonInlineCustom = TRUE;
	else
	    lastWasNonInlineCustom = FALSE;

	if ( c->c.unicode() >= 32 || c->isCustom() ) {
	    ww = string->width( i );
	} else if ( c->c == '\t' ) {
	    int nx = parag->nextTab( i, x - left ) + left;
	    if ( nx < x )
		ww = w - x;
	    else
		ww = nx - x;
	} else {
	    ww = c->format()->width( ' ' );
	}

	// last character ("invisible" space) has no width
	if ( i == len - 1 )
	    ww = 0;

	QTextCustomItem* ci = c->customItem();
	if ( c->isCustom() && ci->ownLine() ) {
	    x = doc ? doc->flow()->adjustLMargin( y + parag->rect().y(), parag->rect().height(), left, 4 ) : left;
	    w = dw - ( doc ? doc->flow()->adjustRMargin( y + parag->rect().y(), parag->rect().height(), rm, 4 ) : 0 );
	    QTextParagLineStart *lineStart2 = formatLine( parag, string, lineStart, firstChar, c-1, align, w - x );
	    ci->resize( w - x);
	    if ( ci->width < w - x ) {
		if ( align & Qt::AlignHCenter )
		    x = ( w - ci->width ) / 2;
		else if ( align & Qt::AlignRight ) {
		    x = w - ci->width;
		}
	    }
	    c->x = x;
	    curLeft = x;
	    if ( i == 0 || !isBreakable( string, i - 1 ) || string->at( i - 1 ).lineStart == 0 ) {
		y += QMAX( h, tmph );
		tmph = c->height() + linespace;
		h = tmph;
		lineStart = lineStart2;
		lineStart->y = y;
		insertLineStart( parag, i, lineStart );
		c->lineStart = 1;
		firstChar = c;
	    } else {
		tmph = c->height() + linespace;
		h = tmph;
		delete lineStart2;
	    }
	    lineStart->h = h;
	    lineStart->baseLine = h;
	    tmpBaseLine = lineStart->baseLine;
	    lastBreak = -2;
	    x = 0xffffff;
	    minw = QMAX( minw, tminw );

	    int tw = ci->minimumWidth();
	    if ( tw < QWIDGETSIZE_MAX )
		tminw = tw;
	    else
		tminw = marg;
 	    wused = QMAX( wused, ci->width );
	    continue;
	} else if ( c->isCustom() && ci->placement() != QTextCustomItem::PlaceInline ) {
	    int tw = ci->minimumWidth();
	    if ( tw < QWIDGETSIZE_MAX )
		minw = QMAX( minw, tw );
	}

	if ( wrapEnabled && ( !c->c.isSpace() || lastBreak == -2 )
	     && ( lastBreak != -1 || allowBreakInWords() ) &&
	     ( wrapAtColumn() == -1 && x + ww > w && lastBreak != -1 ||
	       wrapAtColumn() == -1 && x + ww > w - 4 && lastBreak == -1 && allowBreakInWords() ||
	       wrapAtColumn() != -1 && col >= wrapAtColumn() ) ||
	       parag->isNewLinesAllowed() && lastChr == '\n' && firstChar < c ) {
	    if ( wrapAtColumn() != -1 )
		minw = QMAX( minw, x + ww );
	    if ( lastBreak < 0 ) {
		if ( lineStart ) {
		    lineStart->baseLine = QMAX( lineStart->baseLine, tmpBaseLine );
		    h = QMAX( h, tmph );
		    lineStart->h = h;
  		    DO_FLOW( lineStart );
		}
		lineStart = formatLine( parag, string, lineStart, firstChar, c-1, align, w - x );
		x = doc ? doc->flow()->adjustLMargin( y + parag->rect().y(), parag->rect().height(), left, 4 ) : left;
		w = dw - ( doc ? doc->flow()->adjustRMargin( y + parag->rect().y(), parag->rect().height(), rm, 4 ) : 0 );
		if ( parag->isNewLinesAllowed() && c->c == '\t' ) {
		    int nx = parag->nextTab( i, x - left ) + left;
		    if ( nx < x )
			ww = w - x;
		    else
			ww = nx - x;
		}
		curLeft = x;
		y += h;
		tmph = c->height() + linespace;
		h = 0;
		lineStart->y = y;
		insertLineStart( parag, i, lineStart );
		lineStart->baseLine = c->ascent();
		lineStart->h = c->height();
		c->lineStart = 1;
		firstChar = c;
		tmpBaseLine = lineStart->baseLine;
		lastBreak = -1;
		col = 0;
	    } else {
  		DO_FLOW( lineStart );
		i = lastBreak;
		lineStart = formatLine( parag, string, lineStart, firstChar, parag->at( lastBreak ), align, w - string->at( i ).x );
		x = doc ? doc->flow()->adjustLMargin( y + parag->rect().y(), parag->rect().height(), left, 4 ) : left;
		w = dw - ( doc ? doc->flow()->adjustRMargin( y + parag->rect().y(), parag->rect().height(), rm, 4 ) : 0 );
		if ( parag->isNewLinesAllowed() && c->c == '\t' ) {
		    int nx = parag->nextTab( i, x - left ) + left;
		    if ( nx < x )
			ww = w - x;
		    else
			ww = nx - x;
		}
		curLeft = x;
		y += h;
		tmph = c->height() + linespace;
		h = tmph;
		lineStart->y = y;
		insertLineStart( parag, i + 1, lineStart );
		lineStart->baseLine = c->ascent();
		lineStart->h = c->height();
		c->lineStart = 1;
		firstChar = c;
		tmpBaseLine = lineStart->baseLine;
		lastBreak = -1;
		col = 0;
		tminw = marg;
		continue;
	    }
	} else if ( lineStart && ( isBreakable( string, i ) || parag->isNewLinesAllowed() && c->c == '\n' ) ) {
	    if ( len <= 2 || i < len - 1 ) {
		tmpBaseLine = QMAX( tmpBaseLine, c->ascent() );
		tmph = QMAX( tmph, c->height() + linespace );
	    }
	    minw = QMAX( minw, tminw );
	    tminw = marg + ww;
	    lineStart->baseLine = QMAX( lineStart->baseLine, tmpBaseLine );
	    h = QMAX( h, tmph );
	    lineStart->h = h;
	    if ( i < len - 2 || c->c != ' ' )
		lastBreak = i;
	} else {
	    tminw += ww;
	    int belowBaseLine = QMAX( tmph - tmpBaseLine, c->height() + linespace - c->ascent() );
	    tmpBaseLine = QMAX( tmpBaseLine, c->ascent() );
	    tmph = tmpBaseLine + belowBaseLine;
	}

	c->x = x;
	x += ww;
	wused = QMAX( wused, x );
    }

    // ### hack. The last char in the paragraph is always invisible, and somehow sometimes has a wrong format. It changes between
    // layouting and printing. This corrects some layouting errors in BiDi mode due to this.
    if ( len > 1 && !c->isAnchor() ) {
	c->format()->removeRef();
	c->setFormat( string->at( len - 2 ).format() );
	c->format()->addRef();
    }

    if ( lineStart ) {
	lineStart->baseLine = QMAX( lineStart->baseLine, tmpBaseLine );
	h = QMAX( h, tmph );
	lineStart->h = h;
	// last line in a paragraph is not justified
	if ( align == Qt3::AlignJustify )
	    align = Qt3::AlignAuto;
 	DO_FLOW( lineStart );
	lineStart = formatLine( parag, string, lineStart, firstChar, c, align, w - x );
	delete lineStart;
    }

    minw = QMAX( minw, tminw );

    int m = parag->bottomMargin();
    if ( parag->next() && doc && !doc->addMargins() )
	m = QMAX( m, parag->next()->topMargin() );
    parag->setFullWidth( fullWidth );
    if ( parag->next() && parag->next()->isLineBreak() )
	m = 0;
    y += h + m;

    wused += rm;
    if ( !wrapEnabled || wrapAtColumn() != -1 )
	minw = QMAX(minw, wused);
    thisminw = minw;
    thiswused = wused;
    return y;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

QTextIndent::QTextIndent()
{
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

QTextFormatCollection::QTextFormatCollection()
    : cKey( 307 ), sheet( 0 )
{
    defFormat = new QTextFormat( QApplication::font(),
				 QApplication::palette().color( QPalette::Active, QColorGroup::Text ) );
    lastFormat = cres = 0;
    cflags = -1;
    cKey.setAutoDelete( TRUE );
    cachedFormat = 0;
}

QTextFormatCollection::~QTextFormatCollection()
{
    delete defFormat;
}

QTextFormat *QTextFormatCollection::format( QTextFormat *f )
{
    if ( f->parent() == this || f == defFormat ) {
#ifdef DEBUG_COLLECTION
	qDebug( "need '%s', best case!", f->key().latin1() );
#endif
	lastFormat = f;
	lastFormat->addRef();
	return lastFormat;
    }

    if ( f == lastFormat || ( lastFormat && f->key() == lastFormat->key() ) ) {
#ifdef DEBUG_COLLECTION
	qDebug( "need '%s', good case!", f->key().latin1() );
#endif
	lastFormat->addRef();
	return lastFormat;
    }

    QTextFormat *fm = cKey.find( f->key() );
    if ( fm ) {
#ifdef DEBUG_COLLECTION
	qDebug( "need '%s', normal case!", f->key().latin1() );
#endif
	lastFormat = fm;
	lastFormat->addRef();
	return lastFormat;
    }

    if ( f->key() == defFormat->key() )
	return defFormat;

#ifdef DEBUG_COLLECTION
    qDebug( "need '%s', worst case!", f->key().latin1() );
#endif
    lastFormat = createFormat( *f );
    lastFormat->collection = this;
    cKey.insert( lastFormat->key(), lastFormat );
    return lastFormat;
}

QTextFormat *QTextFormatCollection::format( QTextFormat *of, QTextFormat *nf, int flags )
{
    if ( cres && kof == of->key() && knf == nf->key() && cflags == flags ) {
#ifdef DEBUG_COLLECTION
	qDebug( "mix of '%s' and '%s, best case!", of->key().latin1(), nf->key().latin1() );
#endif
	cres->addRef();
	return cres;
    }

    cres = createFormat( *of );
    kof = of->key();
    knf = nf->key();
    cflags = flags;
    if ( flags & QTextFormat::Bold )
	cres->fn.setBold( nf->fn.bold() );
    if ( flags & QTextFormat::Italic )
	cres->fn.setItalic( nf->fn.italic() );
    if ( flags & QTextFormat::Underline )
	cres->fn.setUnderline( nf->fn.underline() );
    if ( flags & QTextFormat::Family )
	cres->fn.setFamily( nf->fn.family() );
    if ( flags & QTextFormat::Size ) {
	if ( of->usePixelSizes )
	    cres->fn.setPixelSize( nf->fn.pixelSize() );
	else
	    cres->fn.setPointSize( nf->fn.pointSize() );
    }
    if ( flags & QTextFormat::Color )
	cres->col = nf->col;
    if ( flags & QTextFormat::Misspelled )
	cres->missp = nf->missp;
    if ( flags & QTextFormat::VAlign )
	cres->ha = nf->ha;
    cres->update();

    QTextFormat *fm = cKey.find( cres->key() );
    if ( !fm ) {
#ifdef DEBUG_COLLECTION
	qDebug( "mix of '%s' and '%s, worst case!", of->key().latin1(), nf->key().latin1() );
#endif
	cres->collection = this;
	cKey.insert( cres->key(), cres );
    } else {
#ifdef DEBUG_COLLECTION
	qDebug( "mix of '%s' and '%s, good case!", of->key().latin1(), nf->key().latin1() );
#endif
	delete cres;
	cres = fm;
	cres->addRef();
    }

    return cres;
}

QTextFormat *QTextFormatCollection::format( const QFont &f, const QColor &c )
{
    if ( cachedFormat && cfont == f && ccol == c ) {
#ifdef DEBUG_COLLECTION
	qDebug( "format of font and col '%s' - best case", cachedFormat->key().latin1() );
#endif
	cachedFormat->addRef();
	return cachedFormat;
    }

    QString key = QTextFormat::getKey( f, c, FALSE,  QTextFormat::AlignNormal );
    cachedFormat = cKey.find( key );
    cfont = f;
    ccol = c;

    if ( cachedFormat ) {
#ifdef DEBUG_COLLECTION
	qDebug( "format of font and col '%s' - good case", cachedFormat->key().latin1() );
#endif
	cachedFormat->addRef();
	return cachedFormat;
    }

    if ( key == defFormat->key() )
	return defFormat;

    cachedFormat = createFormat( f, c );
    cachedFormat->collection = this;
    cKey.insert( cachedFormat->key(), cachedFormat );
    if ( cachedFormat->key() != key )
	qWarning("ASSERT: keys for format not identical: '%s '%s'", cachedFormat->key().latin1(), key.latin1() );
#ifdef DEBUG_COLLECTION
    qDebug( "format of font and col '%s' - worst case", cachedFormat->key().latin1() );
#endif
    return cachedFormat;
}

void QTextFormatCollection::remove( QTextFormat *f )
{
    if ( lastFormat == f )
	lastFormat = 0;
    if ( cres == f )
	cres = 0;
    if ( cachedFormat == f )
	cachedFormat = 0;
    cKey.remove( f->key() );
}

void QTextFormatCollection::debug()
{
#ifdef DEBUG_COLLECTION
    qDebug( "------------ QTextFormatCollection: debug --------------- BEGIN" );
    QDictIterator<QTextFormat> it( cKey );
    for ( ; it.current(); ++it ) {
	qDebug( "format '%s' (%p): refcount: %d", it.current()->key().latin1(),
		it.current(), it.current()->ref );
    }
    qDebug( "------------ QTextFormatCollection: debug --------------- END" );
#endif
}

void QTextFormatCollection::updateStyles()
{
    QDictIterator<QTextFormat> it( cKey );
    QTextFormat *f;
    while ( ( f = it.current() ) ) {
	++it;
	f->updateStyle();
    }
    updateKeys();
}

void QTextFormatCollection::updateFontSizes( int base, bool usePixels )
{
    QDictIterator<QTextFormat> it( cKey );
    QTextFormat *f;
    while ( ( f = it.current() ) ) {
	++it;
	f->stdSize = base;
	f->usePixelSizes = usePixels;
	if ( usePixels )
	    f->fn.setPixelSize( f->stdSize );
	else
	    f->fn.setPointSize( f->stdSize );
	styleSheet()->scaleFont( f->fn, f->logicalFontSize );
	f->update();
    }
    f = defFormat;
    f->stdSize = base;
    f->usePixelSizes = usePixels;
    if ( usePixels )
	f->fn.setPixelSize( f->stdSize );
    else
	f->fn.setPointSize( f->stdSize );
    styleSheet()->scaleFont( f->fn, f->logicalFontSize );
    f->update();
    updateKeys();
}

void QTextFormatCollection::updateFontAttributes( const QFont &f, const QFont &old )
{
    QDictIterator<QTextFormat> it( cKey );
    QTextFormat *fm;
    while ( ( fm = it.current() ) ) {
	++it;
	if ( fm->fn.family() == old.family() &&
	     fm->fn.weight() == old.weight() &&
	     fm->fn.italic() == old.italic() &&
	     fm->fn.underline() == old.underline() ) {
	    fm->fn.setFamily( f.family() );
	    fm->fn.setWeight( f.weight() );
	    fm->fn.setItalic( f.italic() );
	    fm->fn.setUnderline( f.underline() );
	    fm->update();
	}
    }
    fm = defFormat;
    if ( fm->fn.family() == old.family() &&
	 fm->fn.weight() == old.weight() &&
	 fm->fn.italic() == old.italic() &&
	 fm->fn.underline() == old.underline() ) {
	fm->fn.setFamily( f.family() );
	fm->fn.setWeight( f.weight() );
	fm->fn.setItalic( f.italic() );
	fm->fn.setUnderline( f.underline() );
	fm->update();
    }
    updateKeys();
}


// the keys in cKey have changed, rebuild the hashtable
void QTextFormatCollection::updateKeys()
{
    if ( cKey.isEmpty() )
	return;
    cKey.setAutoDelete( FALSE );
    QTextFormat** formats = new QTextFormat*[  cKey.count() + 1];
    QTextFormat **f = formats;
    QDictIterator<QTextFormat> it( cKey );
    while ( ( *f = it.current() ) ) {
       ++it;
       ++f;
    }
    cKey.clear();
    for ( f = formats; *f; f++ )
       cKey.insert( (*f)->key(), *f );
    cKey.setAutoDelete( TRUE );
}



// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void QTextFormat::setBold( bool b )
{
    if ( b == fn.bold() )
	return;
    fn.setBold( b );
    update();
}

void QTextFormat::setMisspelled( bool b )
{
    if ( b == (bool)missp )
	return;
    missp = b;
    update();
}

void QTextFormat::setVAlign( VerticalAlignment a )
{
    if ( a == ha )
	return;
    ha = a;
    update();
}

void QTextFormat::setItalic( bool b )
{
    if ( b == fn.italic() )
	return;
    fn.setItalic( b );
    update();
}

void QTextFormat::setUnderline( bool b )
{
    if ( b == fn.underline() )
	return;
    fn.setUnderline( b );
    update();
}

void QTextFormat::setFamily( const QString &f )
{
    if ( f == fn.family() )
	return;
    fn.setFamily( f );
    update();
}

void QTextFormat::setPointSize( int s )
{
    if ( s == fn.pointSize() )
	return;
    fn.setPointSize( s );
    usePixelSizes = FALSE;
    update();
}

void QTextFormat::setFont( const QFont &f )
{
    if ( f == fn && !k.isEmpty() )
	return;
    fn = f;
    update();
}

void QTextFormat::setColor( const QColor &c )
{
    if ( c == col )
	return;
    col = c;
    update();
}

static int makeLogicFontSize( int s )
{
    int defSize = QApplication::font().pointSize();
    if ( s < defSize - 4 )
	return 1;
    if ( s < defSize )
	return 2;
    if ( s < defSize + 4 )
	return 3;
    if ( s < defSize + 8 )
	return 4;
    if ( s < defSize + 12 )
	return 5;
    if (s < defSize + 16 )
	return 6;
    return 7;
}

static QTextFormat *defaultFormat = 0;

QString QTextFormat::makeFormatChangeTags( QTextFormat *f, const QString& oldAnchorHref, const QString& anchorHref  ) const
{
    if ( !defaultFormat ) // #### wrong, use the document's default format instead
	defaultFormat = new QTextFormat( QApplication::font(),
					     QApplication::palette().color( QPalette::Active, QColorGroup::Text ) );

    QString tag;
    if ( f ) {
	if ( f->font() != defaultFormat->font() ) {
	    if ( f->font().family() != defaultFormat->font().family()
		 || f->font().pointSize() != defaultFormat->font().pointSize()
		 || f->color().rgb() != defaultFormat->color().rgb() )
		tag += "</font>";
	    if ( f->font().underline() && f->font().underline() != defaultFormat->font().underline() )
		tag += "</u>";
	    if ( f->font().italic() && f->font().italic() != defaultFormat->font().italic() )
		tag += "</i>";
	    if ( f->font().bold() && f->font().bold() != defaultFormat->font().bold() )
		tag += "</b>";
	}
	if ( !oldAnchorHref.isEmpty() )
	    tag += "</a>";
    }

    if ( !anchorHref.isEmpty() )
	tag += "<a href=\"" + anchorHref + "\">";

    if ( font() != defaultFormat->font() ) {
	if ( font().bold() && font().bold() != defaultFormat->font().bold() )
	    tag += "<b>";
	if ( font().italic() && font().italic() != defaultFormat->font().italic() )
	    tag += "<i>";
	if ( font().underline() && font().underline() != defaultFormat->font().underline() )
	    tag += "<u>";
    }
    if ( font() != defaultFormat->font()
	 || color().rgb() != defaultFormat->color().rgb() ) {
	QString f;
	if ( font().family() != defaultFormat->font().family() )
	    f +=" face=\"" + fn.family() + "\"";
	if ( font().pointSize() != defaultFormat->font().pointSize() ) {
	    f +=" size=\"" + QString::number( makeLogicFontSize( fn.pointSize() ) ) + "\"";
	    f +=" style=\"font-size:" + QString::number( fn.pointSize() ) + "pt\"";
	}
	if ( color().rgb() != defaultFormat->color().rgb() )
	    f +=" color=\"" + col.name() + "\"";
	if ( !f.isEmpty() )
	    tag += "<font" + f + ">";
    }

    return tag;
}

QString QTextFormat::makeFormatEndTags( const QString& anchorHref ) const
{
    if ( !defaultFormat )
	defaultFormat = new QTextFormat( QApplication::font(),
					     QApplication::palette().color( QPalette::Active, QColorGroup::Text ) );

    QString tag;
    if ( font() != defaultFormat->font() ) {
	if ( font().family() != defaultFormat->font().family()
	     || font().pointSize() != defaultFormat->font().pointSize()
	     || color().rgb() != defaultFormat->color().rgb() )
	    tag += "</font>";
	if ( font().underline() && font().underline() != defaultFormat->font().underline() )
	    tag += "</u>";
	if ( font().italic() && font().italic() != defaultFormat->font().italic() )
	    tag += "</i>";
	if ( font().bold() && font().bold() != defaultFormat->font().bold() )
	    tag += "</b>";
    }
    if ( !anchorHref.isEmpty() )
	tag += "</a>";
    return tag;
}

QTextFormat QTextFormat::makeTextFormat( const QStyleSheetItem *style, const QMap<QString,QString>& attr ) const
{
    QTextFormat format(*this);
    if ( style ) {
	format.style = style->name();
	if ( style->name() == "font") {
	    if ( attr.contains("color") ) {
		QString s = attr["color"];
		if ( !s.isEmpty() ) {
		    format.col.setNamedColor( s );
		    format.linkColor = FALSE;
		}
	    }
	    if ( attr.contains("size") ) {
		QString a = attr["size"];
		int n = a.toInt();
		if ( a[0] == '+' || a[0] == '-' )
		    n += format.logicalFontSize;
		format.logicalFontSize = n;
		if ( format.usePixelSizes )
		    format.fn.setPixelSize( format.stdSize );
		else
		    format.fn.setPointSize( format.stdSize );
		style->styleSheet()->scaleFont( format.fn, format.logicalFontSize );
	    }
	    if ( attr.contains("style" ) ) {
		QString a = attr["style"];
		if ( a.startsWith( "font-size:" ) ) {
		    QString s = a.mid( a.find( ':' ) + 1 );
		    int n = s.left( s.length() - 2 ).toInt();
		    format.logicalFontSize = 0;
		    if ( format.usePixelSizes )
			format.fn.setPixelSize( n );
		    else
			format.fn.setPointSize( n );
		}
	    }
	    if ( attr.contains("face") ) {
		QString a = attr["face"];
		if ( a.contains(',') )
		    a = a.left( a.find(',') );
		format.fn.setFamily( a );
	    }
	} else {
	    if ( !style->isAnchor() && style->color().isValid() ) {
		// the style is not an anchor and defines a color.
		// It might be used inside an anchor and it should
		// override the link color.
		format.linkColor = FALSE;
	    }
	    switch ( style->verticalAlignment() ) {
	    case QStyleSheetItem::VAlignBaseline:
		format.setVAlign( QTextFormat::AlignNormal );
		break;
	    case QStyleSheetItem::VAlignSuper:
		format.setVAlign( QTextFormat::AlignSuperScript );
		break;
	    case QStyleSheetItem::VAlignSub:
		format.setVAlign( QTextFormat::AlignSubScript );
		break;
	    }

	    if ( style->fontWeight() != QStyleSheetItem::Undefined )
		format.fn.setWeight( style->fontWeight() );
	    if ( style->fontSize() != QStyleSheetItem::Undefined ) {
		format.fn.setPointSize( style->fontSize() );
	    } else if ( style->logicalFontSize() != QStyleSheetItem::Undefined ) {
		format.logicalFontSize = style->logicalFontSize();
		if ( format.usePixelSizes )
		    format.fn.setPixelSize( format.stdSize );
		else
		    format.fn.setPointSize( format.stdSize );
		style->styleSheet()->scaleFont( format.fn, format.logicalFontSize );
	    } else if ( style->logicalFontSizeStep() ) {
		format.logicalFontSize += style->logicalFontSizeStep();
		if ( format.usePixelSizes )
		    format.fn.setPixelSize( format.stdSize );
		else
		    format.fn.setPointSize( format.stdSize );
		style->styleSheet()->scaleFont( format.fn, format.logicalFontSize );
	    }
	    if ( !style->fontFamily().isEmpty() )
		format.fn.setFamily( style->fontFamily() );
	    if ( style->color().isValid() )
		format.col = style->color();
	    if ( style->definesFontItalic() )
		format.fn.setItalic( style->fontItalic() );
	    if ( style->definesFontUnderline() )
		format.fn.setUnderline( style->fontUnderline() );
	}
    }

    format.update();
    return format;
}

struct QPixmapInt
{
    QPixmapInt() : ref( 0 ) {}
    QPixmap pm;
    int	    ref;
};

static QMap<QString, QPixmapInt> *pixmap_map = 0;

QTextImage::QTextImage( QTextDocument *p, const QMap<QString, QString> &attr, const QString& context,
			QMimeSourceFactory &factory )
    : QTextCustomItem( p )
{
#if defined(PARSER_DEBUG)
    qDebug( debug_indent + "new QTextImage (pappi: %p)", p );
#endif

    width = height = 0;
    if ( attr.contains("width") )
	width = attr["width"].toInt();
    if ( attr.contains("height") )
	height = attr["height"].toInt();

    reg = 0;
    QString imageName = attr["src"];

    if (!imageName)
	imageName = attr["source"];

#if defined(PARSER_DEBUG)
    qDebug( debug_indent + "    .." + imageName );
#endif

    if ( !imageName.isEmpty() ) {
	imgId = QString( "%1,%2,%3,%4" ).arg( imageName ).arg( width ).arg( height ).arg( (ulong)&factory );
	if ( !pixmap_map )
	    pixmap_map = new QMap<QString, QPixmapInt>;
	if ( pixmap_map->contains( imgId ) ) {
	    QPixmapInt& pmi = pixmap_map->operator[](imgId);
	    pm = pmi.pm;
	    pmi.ref++;
	    width = pm.width();
	    height = pm.height();
	} else {
	    QImage img;
	    const QMimeSource* m =
		factory.data( imageName, context );
	    if ( !m ) {
		qWarning("QTextImage: no mimesource for %s", imageName.latin1() );
	    }
	    else {
		if ( !QImageDrag::decode( m, img ) ) {
		    qWarning("QTextImage: cannot decode %s", imageName.latin1() );
		}
	    }

	    if ( !img.isNull() ) {
		if ( width == 0 ) {
		    width = img.width();
		    if ( height != 0 ) {
			width = img.width() * height / img.height();
		    }
		}
		if ( height == 0 ) {
		    height = img.height();
		    if ( width != img.width() ) {
			height = img.height() * width / img.width();
		    }
		}
		if ( img.width() != width || img.height() != height ){
#ifndef QT_NO_IMAGE_SMOOTHSCALE
		    img = img.smoothScale(width, height);
#endif
		    width = img.width();
		    height = img.height();
		}
		pm.convertFromImage( img );
	    }
	    if ( !pm.isNull() ) {
		QPixmapInt& pmi = pixmap_map->operator[](imgId);
		pmi.pm = pm;
		pmi.ref++;
	    }
	}
	if ( pm.mask() ) {
	    QRegion mask( *pm.mask() );
	    QRegion all( 0, 0, pm.width(), pm.height() );
	    reg = new QRegion( all.subtract( mask ) );
	}
    }

    if ( pm.isNull() && (width*height)==0 )
	width = height = 50;

    place = PlaceInline;
    if ( attr["align"] == "left" )
	place = PlaceLeft;
    else if ( attr["align"] == "right" )
	place = PlaceRight;

    tmpwidth = width;
    tmpheight = height;

    attributes = attr;
}

QTextImage::~QTextImage()
{
    if ( pixmap_map && pixmap_map->contains( imgId ) ) {
	QPixmapInt& pmi = pixmap_map->operator[](imgId);
	pmi.ref--;
	if ( !pmi.ref ) {
	    pixmap_map->remove( imgId );
	    if ( pixmap_map->isEmpty() ) {
		delete pixmap_map;
		pixmap_map = 0;
	    }
	}
    }
}

QString QTextImage::richText() const
{
    QString s;
    s += "<img ";
    QMap<QString, QString>::ConstIterator it = attributes.begin();
    for ( ; it != attributes.end(); ++it )
	s += it.key() + "=" + *it + " ";
    s += ">";
    return s;
}

void QTextImage::adjustToPainter( QPainter* p )
{
    width = scale( tmpwidth, p );
    height = scale( tmpheight, p );
}

#if !defined(Q_WS_X11)
#include <qbitmap.h>
#include "qcleanuphandler.h"
static QPixmap *qrt_selection = 0;
static QSingleCleanupHandler<QPixmap> qrt_cleanup_pixmap;
static void qrt_createSelectionPixmap( const QColorGroup &cg )
{
    qrt_selection = new QPixmap( 2, 2 );
    qrt_cleanup_pixmap.set( &qrt_selection );
    qrt_selection->fill( Qt::color0 );
    QBitmap m( 2, 2 );
    m.fill( Qt::color1 );
    QPainter p( &m );
    p.setPen( Qt::color0 );
    for ( int j = 0; j < 2; ++j ) {
	p.drawPoint( j % 2, j );
    }
    p.end();
    qrt_selection->setMask( m );
    qrt_selection->fill( cg.highlight() );
}
#endif

void QTextImage::draw( QPainter* p, int x, int y, int cx, int cy, int cw, int ch, const QColorGroup& cg, bool selected )
{
    if ( placement() != PlaceInline ) {
	x = xpos;
	y = ypos;
    }

    if ( pm.isNull() ) {
	p->fillRect( x , y, width, height,  cg.dark() );
	return;
    }

    if ( is_printer( p ) ) {
	p->drawPixmap( x, y, pm );
	return;
    }

    if ( placement() != PlaceInline && !QRect( xpos, ypos, width, height ).intersects( QRect( cx, cy, cw, ch ) ) )
	return;

    if ( placement() == PlaceInline )
	p->drawPixmap( x , y, pm );
    else
	p->drawPixmap( cx , cy, pm, cx - x, cy - y, cw, ch );

    if ( selected && placement() == PlaceInline && is_printer( p ) ) {
#if defined(Q_WS_X11)
	p->fillRect( QRect( QPoint( x, y ), pm.size() ), QBrush( cg.highlight(), QBrush::Dense4Pattern) );
#else // in WIN32 Dense4Pattern doesn't work correctly (transparency problem), so work around it
	if ( !qrt_selection )
	    qrt_createSelectionPixmap( cg );
	p->drawTiledPixmap( x, y, pm.width(), pm.height(), *qrt_selection );
#endif
    }
}

void QTextHorizontalLine::adjustToPainter( QPainter* p )
{
    height = scale( tmpheight, p );
}


QTextHorizontalLine::QTextHorizontalLine( QTextDocument *p, const QMap<QString, QString> &attr,
					  const QString &,
					  QMimeSourceFactory & )
    : QTextCustomItem( p )
{
    height = tmpheight = 8;
    if ( attr.find( "color" ) != attr.end() )
	color = QColor( *attr.find( "color" ) );
}

QTextHorizontalLine::~QTextHorizontalLine()
{
}

QString QTextHorizontalLine::richText() const
{
    return "<hr>";
}

void QTextHorizontalLine::draw( QPainter* p, int x, int y, int , int , int , int , const QColorGroup& cg, bool selected )
{
    QRect r( x, y, width, height);
    if ( is_printer( p ) ) {
	QPen oldPen = p->pen();
	if ( !color.isValid() )
	    p->setPen( QPen( cg.text(), height/8 ) );
	else
	    p->setPen( QPen( color, height/8 ) );
	p->drawLine( r.left()-1, y + height / 2, r.right() + 1, y + height / 2 );
	p->setPen( oldPen );
    } else {
	QColorGroup g( cg );
	if ( color.isValid() )
	    g.setColor( QColorGroup::Dark, color );
	if ( selected )
	    p->fillRect( r.left(), y, r.right(), y + height, g.highlight() );
	qDrawShadeLine( p, r.left() - 1, y + height / 2, r.right() + 1, y + height / 2, g, TRUE, height / 8 );
    }
}


/*****************************************************************/
// Small set of utility functions to make the parser a bit simpler
//

bool QTextDocument::hasPrefix(const QChar* doc, int length, int pos, QChar c)
{
    if ( pos >= length )
	return FALSE;
    return doc[ pos ].lower() == c.lower();
}

bool QTextDocument::hasPrefix( const QChar* doc, int length, int pos, const QString& s )
{
    if ( pos + (int) s.length() >= length )
	return FALSE;
    for ( int i = 0; i < (int)s.length(); i++ ) {
	if ( doc[ pos + i ].lower() != s[ i ].lower() )
	    return FALSE;
    }
    return TRUE;
}

static bool qt_is_cell_in_use( QPtrList<QTextTableCell>& cells, int row, int col )
{
    for ( QTextTableCell* c = cells.first(); c; c = cells.next() ) {
	if ( row >= c->row() && row < c->row() + c->rowspan()
	     && col >= c->column() && col < c->column() + c->colspan() )
	    return TRUE;
    }
    return FALSE;
}

QTextCustomItem* QTextDocument::parseTable( const QMap<QString, QString> &attr, const QTextFormat &fmt,
					    const QChar* doc, int length, int& pos, QTextParag *curpar )
{

    QTextTable* table = new QTextTable( this, attr );
    int row = -1;
    int col = -1;

    QString rowbgcolor;
    QString rowalign;
    QString tablebgcolor = attr["bgcolor"];

    QPtrList<QTextTableCell> multicells;

    QString tagname;
    (void) eatSpace(doc, length, pos);
    while ( pos < length) {
	if (hasPrefix(doc, length, pos, QChar('<')) ){
	    if (hasPrefix(doc, length, pos+1, QChar('/'))) {
		tagname = parseCloseTag( doc, length, pos );
		if ( tagname == "table" ) {
#if defined(PARSER_DEBUG)
		    debug_indent.remove( debug_indent.length() - 3, 2 );
#endif
		    return table;
		}
	    } else {
		QMap<QString, QString> attr2;
		bool emptyTag = FALSE;
		tagname = parseOpenTag( doc, length, pos, attr2, emptyTag );
		if ( tagname == "tr" ) {
		    rowbgcolor = attr2["bgcolor"];
		    rowalign = attr2["align"];
		    row++;
		    col = -1;
		}
		else if ( tagname == "td" || tagname == "th" ) {
		    col++;
		    while ( qt_is_cell_in_use( multicells, row, col ) ) {
			col++;
		    }

		    if ( row >= 0 && col >= 0 ) {
			const QStyleSheetItem* s = sheet_->item(tagname);
			if ( !attr2.contains("bgcolor") ) {
			    if (!rowbgcolor.isEmpty() )
				attr2["bgcolor"] = rowbgcolor;
			    else if (!tablebgcolor.isEmpty() )
				attr2["bgcolor"] = tablebgcolor;
			}
			if ( !attr2.contains("align") ) {
			    if (!rowalign.isEmpty() )
				attr2["align"] = rowalign;
			}

			// extract the cell contents
			int end = pos;
			while ( end < length
				&& !hasPrefix( doc, length, end, "</td")
				&& !hasPrefix( doc, length, end, "<td")
				&& !hasPrefix( doc, length, end, "</th")
				&& !hasPrefix( doc, length, end, "<th")
				&& !hasPrefix( doc, length, end, "<td")
				&& !hasPrefix( doc, length, end, "</tr")
				&& !hasPrefix( doc, length, end, "<tr")
				&& !hasPrefix( doc, length, end, "</table") ) {
			    if ( hasPrefix( doc, length, end, "<table" ) ) { // nested table
				int nested = 1;
				++end;
				while ( end < length && nested != 0 ) {
				    if ( hasPrefix( doc, length, end, "</table" ) )
					nested--;
				    if ( hasPrefix( doc, length, end, "<table" ) )
					nested++;
				    end++;
				}
			    }
			    end++;
			}
			QTextTableCell* cell  = new QTextTableCell( table, row, col,
								    attr2, s, fmt.makeTextFormat( s, attr2 ),
								    contxt, *factory_, sheet_,
								    QString( doc, length).mid( pos, end - pos ) );
			cell->richText()->parParag = curpar;
			if ( cell->colspan() > 1 || cell->rowspan() > 1 )
			    multicells.append( cell );
			col += cell->colspan()-1;
			pos = end;
		    }
		}
	    }

	} else {
	    ++pos;
	}
    }
#if defined(PARSER_DEBUG)
    debug_indent.remove( debug_indent.length() - 3, 2 );
#endif
    return table;
}

bool QTextDocument::eatSpace(const QChar* doc, int length, int& pos, bool includeNbsp )
{
    int old_pos = pos;
    while (pos < length && doc[pos].isSpace() && ( includeNbsp || (doc[pos] != QChar::nbsp ) ) )
	pos++;
    return old_pos < pos;
}

bool QTextDocument::eat(const QChar* doc, int length, int& pos, QChar c)
{
    bool ok = pos < length && doc[pos] == c;
    if ( ok )
	pos++;
    return ok;
}
/*****************************************************************/

struct Entity {
    const char * name;
    Q_UINT16 code;
};

static const Entity entitylist [] = {
    { "AElig", 0x00c6 },
    { "Aacute", 0x00c1 },
    { "Acirc", 0x00c2 },
    { "Agrave", 0x00c0 },
    { "Alpha", 0x0391 },
    { "AMP", 38 },
    { "Aring", 0x00c5 },
    { "Atilde", 0x00c3 },
    { "Auml", 0x00c4 },
    { "Beta", 0x0392 },
    { "Ccedil", 0x00c7 },
    { "Chi", 0x03a7 },
    { "Dagger", 0x2021 },
    { "Delta", 0x0394 },
    { "ETH", 0x00d0 },
    { "Eacute", 0x00c9 },
    { "Ecirc", 0x00ca },
    { "Egrave", 0x00c8 },
    { "Epsilon", 0x0395 },
    { "Eta", 0x0397 },
    { "Euml", 0x00cb },
    { "Gamma", 0x0393 },
    { "GT", 62 },
    { "Iacute", 0x00cd },
    { "Icirc", 0x00ce },
    { "Igrave", 0x00cc },
    { "Iota", 0x0399 },
    { "Iuml", 0x00cf },
    { "Kappa", 0x039a },
    { "Lambda", 0x039b },
    { "LT", 60 },
    { "Mu", 0x039c },
    { "Ntilde", 0x00d1 },
    { "Nu", 0x039d },
    { "OElig", 0x0152 },
    { "Oacute", 0x00d3 },
    { "Ocirc", 0x00d4 },
    { "Ograve", 0x00d2 },
    { "Omega", 0x03a9 },
    { "Omicron", 0x039f },
    { "Oslash", 0x00d8 },
    { "Otilde", 0x00d5 },
    { "Ouml", 0x00d6 },
    { "Phi", 0x03a6 },
    { "Pi", 0x03a0 },
    { "Prime", 0x2033 },
    { "Psi", 0x03a8 },
    { "QUOT", 34 },
    { "Rho", 0x03a1 },
    { "Scaron", 0x0160 },
    { "Sigma", 0x03a3 },
    { "THORN", 0x00de },
    { "Tau", 0x03a4 },
    { "Theta", 0x0398 },
    { "Uacute", 0x00da },
    { "Ucirc", 0x00db },
    { "Ugrave", 0x00d9 },
    { "Upsilon", 0x03a5 },
    { "Uuml", 0x00dc },
    { "Xi", 0x039e },
    { "Yacute", 0x00dd },
    { "Yuml", 0x0178 },
    { "Zeta", 0x0396 },
    { "aacute", 0x00e1 },
    { "acirc", 0x00e2 },
    { "acute", 0x00b4 },
    { "aelig", 0x00e6 },
    { "agrave", 0x00e0 },
    { "alefsym", 0x2135 },
    { "alpha", 0x03b1 },
    { "amp", 38 },
    { "and", 0x22a5 },
    { "ang", 0x2220 },
    { "apos", 0x0027 },
    { "aring", 0x00e5 },
    { "asymp", 0x2248 },
    { "atilde", 0x00e3 },
    { "auml", 0x00e4 },
    { "bdquo", 0x201e },
    { "beta", 0x03b2 },
    { "brvbar", 0x00a6 },
    { "bull", 0x2022 },
    { "cap", 0x2229 },
    { "ccedil", 0x00e7 },
    { "cedil", 0x00b8 },
    { "cent", 0x00a2 },
    { "chi", 0x03c7 },
    { "circ", 0x02c6 },
    { "clubs", 0x2663 },
    { "cong", 0x2245 },
    { "copy", 0x00a9 },
    { "crarr", 0x21b5 },
    { "cup", 0x222a },
    { "curren", 0x00a4 },
    { "dArr", 0x21d3 },
    { "dagger", 0x2020 },
    { "darr", 0x2193 },
    { "deg", 0x00b0 },
    { "delta", 0x03b4 },
    { "diams", 0x2666 },
    { "divide", 0x00f7 },
    { "eacute", 0x00e9 },
    { "ecirc", 0x00ea },
    { "egrave", 0x00e8 },
    { "empty", 0x2205 },
    { "emsp", 0x2003 },
    { "ensp", 0x2002 },
    { "epsilon", 0x03b5 },
    { "equiv", 0x2261 },
    { "eta", 0x03b7 },
    { "eth", 0x00f0 },
    { "euml", 0x00eb },
    { "euro", 0x20ac },
    { "exist", 0x2203 },
    { "fnof", 0x0192 },
    { "forall", 0x2200 },
    { "frac12", 0x00bd },
    { "frac14", 0x00bc },
    { "frac34", 0x00be },
    { "frasl", 0x2044 },
    { "gamma", 0x03b3 },
    { "ge", 0x2265 },
    { "gt", 62 },
    { "hArr", 0x21d4 },
    { "harr", 0x2194 },
    { "hearts", 0x2665 },
    { "hellip", 0x2026 },
    { "iacute", 0x00ed },
    { "icirc", 0x00ee },
    { "iexcl", 0x00a1 },
    { "igrave", 0x00ec },
    { "image", 0x2111 },
    { "infin", 0x221e },
    { "int", 0x222b },
    { "iota", 0x03b9 },
    { "iquest", 0x00bf },
    { "isin", 0x2208 },
    { "iuml", 0x00ef },
    { "kappa", 0x03ba },
    { "lArr", 0x21d0 },
    { "lambda", 0x03bb },
    { "lang", 0x2329 },
    { "laquo", 0x00ab },
    { "larr", 0x2190 },
    { "lceil", 0x2308 },
    { "ldquo", 0x201c },
    { "le", 0x2264 },
    { "lfloor", 0x230a },
    { "lowast", 0x2217 },
    { "loz", 0x25ca },
    { "lrm", 0x200e },
    { "lsaquo", 0x2039 },
    { "lsquo", 0x2018 },
    { "lt", 60 },
    { "macr", 0x00af },
    { "mdash", 0x2014 },
    { "micro", 0x00b5 },
    { "middot", 0x00b7 },
    { "minus", 0x2212 },
    { "mu", 0x03bc },
    { "nabla", 0x2207 },
    { "nbsp", 0x00a0 },
    { "ndash", 0x2013 },
    { "ne", 0x2260 },
    { "ni", 0x220b },
    { "not", 0x00ac },
    { "notin", 0x2209 },
    { "nsub", 0x2284 },
    { "ntilde", 0x00f1 },
    { "nu", 0x03bd },
    { "oacute", 0x00f3 },
    { "ocirc", 0x00f4 },
    { "oelig", 0x0153 },
    { "ograve", 0x00f2 },
    { "oline", 0x203e },
    { "omega", 0x03c9 },
    { "omicron", 0x03bf },
    { "oplus", 0x2295 },
    { "or", 0x22a6 },
    { "ordf", 0x00aa },
    { "ordm", 0x00ba },
    { "oslash", 0x00f8 },
    { "otilde", 0x00f5 },
    { "otimes", 0x2297 },
    { "ouml", 0x00f6 },
    { "para", 0x00b6 },
    { "part", 0x2202 },
    { "percnt", 0x0025 },
    { "permil", 0x2030 },
    { "perp", 0x22a5 },
    { "phi", 0x03c6 },
    { "pi", 0x03c0 },
    { "piv", 0x03d6 },
    { "plusmn", 0x00b1 },
    { "pound", 0x00a3 },
    { "prime", 0x2032 },
    { "prod", 0x220f },
    { "prop", 0x221d },
    { "psi", 0x03c8 },
    { "quot", 34 },
    { "rArr", 0x21d2 },
    { "radic", 0x221a },
    { "rang", 0x232a },
    { "raquo", 0x00bb },
    { "rarr", 0x2192 },
    { "rceil", 0x2309 },
    { "rdquo", 0x201d },
    { "real", 0x211c },
    { "reg", 0x00ae },
    { "rfloor", 0x230b },
    { "rho", 0x03c1 },
    { "rlm", 0x200f },
    { "rsaquo", 0x203a },
    { "rsquo", 0x2019 },
    { "sbquo", 0x201a },
    { "scaron", 0x0161 },
    { "sdot", 0x22c5 },
    { "sect", 0x00a7 },
    { "shy", 0x00ad },
    { "sigma", 0x03c3 },
    { "sigmaf", 0x03c2 },
    { "sim", 0x223c },
    { "spades", 0x2660 },
    { "sub", 0x2282 },
    { "sube", 0x2286 },
    { "sum", 0x2211 },
    { "sup1", 0x00b9 },
    { "sup2", 0x00b2 },
    { "sup3", 0x00b3 },
    { "sup", 0x2283 },
    { "supe", 0x2287 },
    { "szlig", 0x00df },
    { "tau", 0x03c4 },
    { "there4", 0x2234 },
    { "theta", 0x03b8 },
    { "thetasym", 0x03d1 },
    { "thinsp", 0x2009 },
    { "thorn", 0x00fe },
    { "tilde", 0x02dc },
    { "times", 0x00d7 },
    { "trade", 0x2122 },
    { "uArr", 0x21d1 },
    { "uacute", 0x00fa },
    { "uarr", 0x2191 },
    { "ucirc", 0x00fb },
    { "ugrave", 0x00f9 },
    { "uml", 0x00a8 },
    { "upsih", 0x03d2 },
    { "upsilon", 0x03c5 },
    { "uuml", 0x00fc },
    { "weierp", 0x2118 },
    { "xi", 0x03be },
    { "yacute", 0x00fd },
    { "yen", 0x00a5 },
    { "yuml", 0x00ff },
    { "zeta", 0x03b6 },
    { "zwj", 0x200d },
    { "zwnj", 0x200c },
    { "", 0x0000 }
};





static QMap<QCString, QChar> *html_map = 0;
static void qt_cleanup_html_map()
{
    delete html_map;
    html_map = 0;
}

static QMap<QCString, QChar> *htmlMap()
{
    if ( !html_map ) {
	html_map = new QMap<QCString, QChar>;
	qAddPostRoutine( qt_cleanup_html_map );

	const Entity *ent = entitylist;
	while( ent->code ) {
	    html_map->insert( ent->name, QChar(ent->code) );
	    ent++;
	}
    }
    return html_map;
}

QChar QTextDocument::parseHTMLSpecialChar(const QChar* doc, int length, int& pos)
{
    QCString s;
    pos++;
    int recoverpos = pos;
    while ( pos < length && doc[pos] != ';' && !doc[pos].isSpace() && pos < recoverpos + 6) {
	s += doc[pos];
	pos++;
    }
    if (doc[pos] != ';' && !doc[pos].isSpace() ) {
	pos = recoverpos;
	return '&';
    }
    pos++;

    if ( s.length() > 1 && s[0] == '#') {
	int num = s.mid(1).toInt();
	if ( num == 151 ) // ### hack for designer manual
	    return '-';
	return num;
    }

    QMap<QCString, QChar>::Iterator it = htmlMap()->find(s);
    if ( it != htmlMap()->end() ) {
	return *it;
    }

    pos = recoverpos;
    return '&';
}

QString QTextDocument::parseWord(const QChar* doc, int length, int& pos, bool lower)
{
    QString s;

    if (doc[pos] == '"') {
	pos++;
	while ( pos < length  && doc[pos] != '"' ) {
	    s += doc[pos];
	    pos++;
	}
	eat(doc, length, pos, '"');
    } else {
	static QString term = QString::fromLatin1("/>");
	while( pos < length &&
	       (doc[pos] != '>' && !hasPrefix( doc, length, pos, term))
	       && doc[pos] != '<'
	       && doc[pos] != '='
	       && !doc[pos].isSpace())
	{
	    if ( doc[pos] == '&')
		s += parseHTMLSpecialChar( doc, length, pos );
	    else {
		s += doc[pos];
		pos++;
	    }
	}
	if (lower)
	    s = s.lower();
    }
    return s;
}

QChar QTextDocument::parseChar(const QChar* doc, int length, int& pos, QStyleSheetItem::WhiteSpaceMode wsm )
{
    if ( pos >=  length )
	return QChar::null;

    QChar c = doc[pos++];

    if (c == '<' )
	return QChar::null;

    if ( c.isSpace() && c != QChar::nbsp ) {
	if ( wsm == QStyleSheetItem::WhiteSpacePre ) {
	    if ( c == ' ' )
		return QChar::nbsp;
	    else
		return c;
	} else if ( wsm ==  QStyleSheetItem_WhiteSpaceNoCompression ) {
	    return c;
	} else if ( wsm == QStyleSheetItem_WhiteSpaceNormalWithNewlines ) {
	    if ( c == '\n' )
		return c;
	    while ( pos< length &&
		    doc[pos].isSpace()  && doc[pos] != QChar::nbsp && doc[pos] != '\n' )
		pos++;
	    return ' ';
	} else { // non-pre mode: collapse whitespace except nbsp
	    while ( pos< length &&
		    doc[pos].isSpace()  && doc[pos] != QChar::nbsp )
		pos++;
	    if ( wsm == QStyleSheetItem::WhiteSpaceNoWrap )
		return QChar::nbsp;
	    else
		return ' ';
	}
    }
    else if ( c == '&' )
	return parseHTMLSpecialChar( doc, length, --pos );
    else
	return c;
}

QString QTextDocument::parseOpenTag(const QChar* doc, int length, int& pos,
				  QMap<QString, QString> &attr, bool& emptyTag)
{
    emptyTag = FALSE;
    pos++;
    if ( hasPrefix(doc, length, pos, '!') ) {
	if ( hasPrefix( doc, length, pos+1, "--")) {
	    pos += 3;
	    // eat comments
	    QString pref = QString::fromLatin1("-->");
	    while ( !hasPrefix(doc, length, pos, pref ) && pos < length )
		pos++;
	    if ( hasPrefix(doc, length, pos, pref ) ) {
		pos += 3;
		eatSpace(doc, length, pos, TRUE);
	    }
	    emptyTag = TRUE;
	    return QString::null;
	}
	else {
	    // eat strange internal tags
	    while ( !hasPrefix(doc, length, pos, '>') && pos < length )
		pos++;
	    if ( hasPrefix(doc, length, pos, '>') ) {
		pos++;
		eatSpace(doc, length, pos, TRUE);
	    }
	    return QString::null;
	}
    }

    QString tag = parseWord(doc, length, pos );
    eatSpace(doc, length, pos, TRUE);
    static QString term = QString::fromLatin1("/>");
    static QString s_TRUE = QString::fromLatin1("TRUE");

    while (doc[pos] != '>' && ! (emptyTag = hasPrefix(doc, length, pos, term) )) {
	QString key = parseWord(doc, length, pos );
	eatSpace(doc, length, pos, TRUE);
	if ( key.isEmpty()) {
	    // error recovery
	    while ( pos < length && doc[pos] != '>' )
		pos++;
	    break;
	}
	QString value;
	if (hasPrefix(doc, length, pos, '=') ){
	    pos++;
	    eatSpace(doc, length, pos);
	    value = parseWord(doc, length, pos, FALSE);
	}
	else
	    value = s_TRUE;
	attr.insert(key.lower(), value );
	eatSpace(doc, length, pos, TRUE);
    }

    if (emptyTag) {
	eat(doc, length, pos, '/');
	eat(doc, length, pos, '>');
    }
    else
	eat(doc, length, pos, '>');

    return tag;
}

QString QTextDocument::parseCloseTag( const QChar* doc, int length, int& pos )
{
    pos++;
    pos++;
    QString tag = parseWord(doc, length, pos );
    eatSpace(doc, length, pos, TRUE);
    eat(doc, length, pos, '>');
    return tag;
}

QTextFlow::QTextFlow()
{
    w = pagesize = 0;
    leftItems.setAutoDelete( FALSE );
    rightItems.setAutoDelete( FALSE );
}

QTextFlow::~QTextFlow()
{
}

void QTextFlow::clear()
{
    leftItems.clear();
    rightItems.clear();
}

void QTextFlow::setWidth( int width )
{
    w = width;
}

int QTextFlow::adjustLMargin( int yp, int, int margin, int space )
{
    for ( QTextCustomItem* item = leftItems.first(); item; item = leftItems.next() ) {
	if ( item->ypos == -1 )
	    continue;
	if ( yp >= item->ypos && yp < item->ypos + item->height )
	    margin = QMAX( margin, item->xpos + item->width + space );
    }
    return margin;
}

int QTextFlow::adjustRMargin( int yp, int, int margin, int space )
{
    for ( QTextCustomItem* item = rightItems.first(); item; item = rightItems.next() ) {
	if ( item->ypos == -1 )
	    continue;
	if ( yp >= item->ypos && yp < item->ypos + item->height )
	    margin = QMAX( margin, w - item->xpos - space );
    }
    return margin;
}


int QTextFlow::adjustFlow( int y, int /*w*/, int h )
{
    if ( pagesize > 0 ) { // check pages
	int yinpage = y % pagesize;
	if ( yinpage <= border_tolerance )
	    return border_tolerance - yinpage;
	else
	    if ( yinpage + h > pagesize - border_tolerance )
		return ( pagesize - yinpage ) + border_tolerance;
    }
    return 0;
}

void QTextFlow::unregisterFloatingItem( QTextCustomItem* item )
{
    leftItems.removeRef( item );
    rightItems.removeRef( item );
}

void QTextFlow::registerFloatingItem( QTextCustomItem* item )
{
    if ( item->placement() == QTextCustomItem::PlaceRight ) {
	if ( !rightItems.contains( item ) )
	    rightItems.append( item );
    } else if ( item->placement() == QTextCustomItem::PlaceLeft &&
		!leftItems.contains( item ) ) {
	leftItems.append( item );
    }
}

QRect QTextFlow::boundingRect() const
{
    QRect br;
    QPtrListIterator<QTextCustomItem> l( leftItems );
    while( l.current() ) {
	br = br.unite( l.current()->geometry() );
	++l;
    }
    QPtrListIterator<QTextCustomItem> r( rightItems );
    while( r.current() ) {
	br = br.unite( r.current()->geometry() );
	++r;
    }
    return br;
}


void QTextFlow::drawFloatingItems( QPainter* p, int cx, int cy, int cw, int ch, const QColorGroup& cg, bool selected )
{
    QTextCustomItem *item;
    for ( item = leftItems.first(); item; item = leftItems.next() ) {
	if ( item->xpos == -1 || item->ypos == -1 )
	    continue;
	item->draw( p, item->xpos, item->ypos, cx, cy, cw, ch, cg, selected );
    }

    for ( item = rightItems.first(); item; item = rightItems.next() ) {
	if ( item->xpos == -1 || item->ypos == -1 )
	    continue;
	item->draw( p, item->xpos, item->ypos, cx, cy, cw, ch, cg, selected );
    }
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void QTextCustomItem::pageBreak( int /*y*/ , QTextFlow* /*flow*/ )
{
}

QTextTable::QTextTable( QTextDocument *p, const QMap<QString, QString> & attr  )
    : QTextCustomItem( p )
{
    cells.setAutoDelete( FALSE );
#if defined(PARSER_DEBUG)
    debug_indent += "\t";
    qDebug( debug_indent + "new QTextTable (%p)", this );
    debug_indent += "\t";
#endif
    cellspacing = 2;
    if ( attr.contains("cellspacing") )
	cellspacing = attr["cellspacing"].toInt();
    cellpadding = 1;
    if ( attr.contains("cellpadding") )
	cellpadding = attr["cellpadding"].toInt();
    border = innerborder = 0;
    if ( attr.contains("border" ) ) {
	QString s( attr["border"] );
	if ( s == "TRUE" )
	    border = 1;
	else
	    border = attr["border"].toInt();
    }
    us_b = border;

    innerborder = us_ib = border ? 1 : 0;

    if ( border )
	cellspacing += 2;

    us_ib = innerborder;
    us_cs = cellspacing;
    us_cp = cellpadding;
    outerborder = cellspacing + border;
    us_ob = outerborder;
    layout = new QGridLayout( 1, 1, cellspacing );

    fixwidth = 0;
    stretch = 0;
    if ( attr.contains("width") ) {
	bool b;
	QString s( attr["width"] );
	int w = s.toInt( &b );
	if ( b ) {
	    fixwidth = w;
	} else {
	    s = s.stripWhiteSpace();
	    if ( s.length() > 1 && s[ (int)s.length()-1 ] == '%' )
		stretch = s.left( s.length()-1).toInt();
	}
    }

    place = PlaceInline;
    if ( attr["align"] == "left" )
	place = PlaceLeft;
    else if ( attr["align"] == "right" )
	place = PlaceRight;
    cachewidth = 0;
    attributes = attr;
    pageBreakFor = -1;
}

QTextTable::~QTextTable()
{
    delete layout;
}

QString QTextTable::richText() const
{
    QString s;
    s = "<table ";
    QMap<QString, QString>::ConstIterator it = attributes.begin();
    for ( ; it != attributes.end(); ++it )
	s += it.key() + "=" + *it + " ";
    s += ">\n";

    int lastRow = -1;
    bool needEnd = FALSE;
    QPtrListIterator<QTextTableCell> it2( cells );
    while ( it2.current() ) {
	QTextTableCell *cell = it2.current();
	++it2;
	if ( lastRow != cell->row() ) {
	    if ( lastRow != -1 )
		s += "</tr>\n";
	    s += "<tr>";
	    lastRow = cell->row();
	    needEnd = TRUE;
	}
	s += "<td ";
	it = cell->attributes.begin();
	for ( ; it != cell->attributes.end(); ++it )
	    s += it.key() + "=" + *it + " ";
	s += ">";
	s += cell->richText()->richText();
	s += "</td>";
    }
    if ( needEnd )
	s += "</tr>\n";
    s += "</table>\n";
    return s;
}

void QTextTable::adjustToPainter( QPainter* p )
{
    cellspacing = scale( us_cs, p );
    cellpadding = scale( us_cp, p );
    border = scale( us_b , p );
    innerborder = scale( us_ib, p );
    outerborder = scale( us_ob ,p );
    width = 0;
    cachewidth = 0;
    for ( QTextTableCell* cell = cells.first(); cell; cell = cells.next() )
	cell->adjustToPainter( p );
}

void QTextTable::adjustCells( int y , int shift )
{
    QPtrListIterator<QTextTableCell> it( cells );
    QTextTableCell* cell;
    bool enlarge = FALSE;
    while ( ( cell = it.current() ) ) {
	++it;
	QRect r = cell->geometry();
	if ( y <= r.top() ) {
	    r.moveBy(0, shift );
	    cell->setGeometry( r );
	    enlarge = TRUE;
	} else if ( y <= r.bottom() ) {
	    r.rBottom() += shift;
	    cell->setGeometry( r );
	    enlarge = TRUE;
	}
    }
    if ( enlarge )
	height += shift;
}

void QTextTable::pageBreak( int  yt, QTextFlow* flow )
{
    if ( flow->pageSize() <= 0 )
        return;
    if ( layout && pageBreakFor > 0 && pageBreakFor != yt ) {
	layout->invalidate();
	int h = layout->heightForWidth( width-2*outerborder );
	layout->setGeometry( QRect(0, 0, width-2*outerborder, h)  );
	height = layout->geometry().height()+2*outerborder;
    }
    pageBreakFor = yt;
    QPtrListIterator<QTextTableCell> it( cells );
    QTextTableCell* cell;
    while ( ( cell = it.current() ) ) {
	++it;
	int y = yt + outerborder + cell->geometry().y();
	int shift = flow->adjustFlow( y - cellspacing, width, cell->richText()->height() + 2*cellspacing );
	adjustCells( y - outerborder - yt, shift );
    }
}


void QTextTable::draw(QPainter* p, int x, int y, int cx, int cy, int cw, int ch, const QColorGroup& cg, bool selected )
{
    if ( placement() != PlaceInline ) {
	x = xpos;
	y = ypos;
    }

    for (QTextTableCell* cell = cells.first(); cell; cell = cells.next() ) {
	if ( cx < 0 && cy < 0 ||
	     QRect( cx, cy, cw, ch ).intersects( QRect( x + outerborder + cell->geometry().x(),
							y + outerborder + cell->geometry().y(),
							cell->geometry().width(), cell->geometry().height() ) ) ) {
	    cell->draw( p, x+outerborder, y+outerborder, cx, cy, cw, ch, cg, selected );
	    if ( border ) {
		QRect r( x+outerborder+cell->geometry().x() - innerborder,
			 y+outerborder+cell->geometry().y() - innerborder,
			 cell->geometry().width() + 2 * innerborder,
			 cell->geometry().height() + 2 * innerborder );
		if ( is_printer( p ) ) {
		    QPen oldPen = p->pen();
		    QRect r2 = r;
		    r2.setLeft( r2.left() + innerborder/2 );
		    r2.setTop( r2.top() + innerborder/2 );
		    r2.setRight( r2.right() - innerborder/2 );
		    r2.setBottom( r2.bottom() - innerborder/2 );
		    p->setPen( QPen( cg.text(), innerborder ) );
		    p->drawRect( r2 );
		    p->setPen( oldPen );
		} else {
		    int s =  QMAX( cellspacing-2*innerborder, 0);
		    if ( s ) {
			p->fillRect( r.left()-s, r.top(), s+1, r.height(), cg.button() );
			p->fillRect( r.right(), r.top(), s+1, r.height(), cg.button() );
			p->fillRect( r.left()-s, r.top()-s, r.width()+2*s, s, cg.button() );
			p->fillRect( r.left()-s, r.bottom(), r.width()+2*s, s, cg.button() );
		    }
		    qDrawShadePanel( p, r, cg, TRUE, innerborder );
		}
	    }
	}
    }
    if ( border ) {
	QRect r ( x, y, width, height );
	if ( is_printer( p ) ) {
 	    QRect r2 = r;
	    r2.setLeft( r2.left() + border/2 );
	    r2.setTop( r2.top() + border/2 );
	    r2.setRight( r2.right() - border/2 );
	    r2.setBottom( r2.bottom() - border/2 );
	    QPen oldPen = p->pen();
	    p->setPen( QPen( cg.text(), border ) );
	    p->drawRect( r2 );
	    p->setPen( oldPen );
	} else {
	    int s = border+QMAX( cellspacing-2*innerborder, 0);
	    if ( s ) {
		p->fillRect( r.left(), r.top(), s, r.height(), cg.button() );
		p->fillRect( r.right()-s, r.top(), s, r.height(), cg.button() );
		p->fillRect( r.left(), r.top(), r.width(), s, cg.button() );
		p->fillRect( r.left(), r.bottom()-s, r.width(), s, cg.button() );
	    }
	    qDrawShadePanel( p, r, cg, FALSE, border );
	}
    }

#if defined(DEBUG_TABLE_RENDERING)
    p->save();
    p->setPen( Qt::red );
    p->drawRect( x, y, width, height );
    p->restore();
#endif
}

int QTextTable::minimumWidth() const
{
    return (layout ? layout->minimumSize().width() : 0) + 2 * outerborder;
}

void QTextTable::resize( int nwidth )
{
    if ( fixwidth && cachewidth != 0 )
	return;
    if ( nwidth == cachewidth )
	return;


    cachewidth = nwidth;
    int w = nwidth;

    format( w );

    if ( stretch )
	nwidth = nwidth * stretch / 100;

    width = nwidth;
    layout->invalidate();
    int shw = layout->sizeHint().width() + 2*outerborder;
    int mw = layout->minimumSize().width() + 2*outerborder;
    if ( stretch )
	width = QMAX( mw, nwidth );
    else
	width = QMAX( mw, QMIN( nwidth, shw ) );

    if ( fixwidth )
	width = fixwidth;

    layout->invalidate();
    mw = layout->minimumSize().width() + 2*outerborder;
    width = QMAX( width, mw );

    int h = layout->heightForWidth( width-2*outerborder );
    layout->setGeometry( QRect(0, 0, width-2*outerborder, h)  );
    height = layout->geometry().height()+2*outerborder;
}

void QTextTable::format( int w )
{
    for ( int i = 0; i < (int)cells.count(); ++i ) {
	QTextTableCell *cell = cells.at( i );
	QRect r = cell->geometry();
	r.setWidth( w - 2*outerborder );
	cell->setGeometry( r );
    }
}

void QTextTable::addCell( QTextTableCell* cell )
{
    cells.append( cell );
    layout->addMultiCell( cell, cell->row(), cell->row() + cell->rowspan()-1,
			  cell->column(), cell->column() + cell->colspan()-1 );
}

bool QTextTable::enter( QTextCursor *c, QTextDocument *&doc, QTextParag *&parag, int &idx, int &ox, int &oy, bool atEnd )
{
    currCell.remove( c );
    if ( !atEnd )
	return next( c, doc, parag, idx, ox, oy );
    currCell.insert( c, cells.count() );
    return prev( c, doc, parag, idx, ox, oy );
}

bool QTextTable::enterAt( QTextCursor *c, QTextDocument *&doc, QTextParag *&parag, int &idx, int &ox, int &oy, const QPoint &pos )
{
    currCell.remove( c );
    int lastCell = -1;
    int lastY = -1;
    int i;
    for ( i = 0; i < (int)cells.count(); ++i ) {
	QTextTableCell *cell = cells.at( i );
	if ( !cell )
	    continue;
	QRect r( cell->geometry().x(),
		 cell->geometry().y(),
		 cell->geometry().width() + 2 * innerborder + 2 * outerborder,
		 cell->geometry().height() + 2 * innerborder + 2 * outerborder );

	if ( r.left() <= pos.x() && r.right() >= pos.x() ) {
	    if ( cell->geometry().y() > lastY ) {
		lastCell = i;
		lastY = cell->geometry().y();
	    }
	    if ( r.top() <= pos.y() && r.bottom() >= pos.y() ) {
		currCell.insert( c, i );
		break;
	    }
	}
    }
    if ( i == (int) cells.count() )
 	return FALSE; // no cell found

    if ( currCell.find( c ) == currCell.end() ) {
	if ( lastY != -1 )
	    currCell.insert( c, lastCell );
	else
	    return FALSE;
    }

    QTextTableCell *cell = cells.at( *currCell.find( c ) );
    if ( !cell )
	return FALSE;
    doc = cell->richText();
    parag = doc->firstParag();
    idx = 0;
    ox += cell->geometry().x() + cell->horizontalAlignmentOffset() + outerborder + parent->x();
    oy += cell->geometry().y() + cell->verticalAlignmentOffset() + outerborder;
    return TRUE;
}

bool QTextTable::next( QTextCursor *c, QTextDocument *&doc, QTextParag *&parag, int &idx, int &ox, int &oy )
{
    int cc = -1;
    if ( currCell.find( c ) != currCell.end() )
	cc = *currCell.find( c );
    if ( cc > (int)cells.count() - 1 || cc < 0 )
	cc = -1;
    currCell.remove( c );
    currCell.insert( c, ++cc );
    if ( cc >= (int)cells.count() ) {
	currCell.insert( c, 0 );
	QTextCustomItem::next( c, doc, parag, idx, ox, oy );
	QTextTableCell *cell = cells.first();
	if ( !cell )
	    return FALSE;
	doc = cell->richText();
	idx = -1;
	return TRUE;
    }

    if ( currCell.find( c ) == currCell.end() )
	return FALSE;
    QTextTableCell *cell = cells.at( *currCell.find( c ) );
    if ( !cell )
	return FALSE;
    doc = cell->richText();
    parag = doc->firstParag();
    idx = 0;
    ox += cell->geometry().x() + cell->horizontalAlignmentOffset() + outerborder + parent->x();
    oy += cell->geometry().y() + cell->verticalAlignmentOffset() + outerborder;
    return TRUE;
}

bool QTextTable::prev( QTextCursor *c, QTextDocument *&doc, QTextParag *&parag, int &idx, int &ox, int &oy )
{
    int cc = -1;
    if ( currCell.find( c ) != currCell.end() )
	cc = *currCell.find( c );
    if ( cc > (int)cells.count() - 1 || cc < 0 )
	cc = cells.count();
    currCell.remove( c );
    currCell.insert( c, --cc );
    if ( cc < 0 ) {
	currCell.insert( c, 0 );
	QTextCustomItem::prev( c, doc, parag, idx, ox, oy );
	QTextTableCell *cell = cells.first();
	if ( !cell )
	    return FALSE;
	doc = cell->richText();
	idx = -1;
	return TRUE;
    }

    if ( currCell.find( c ) == currCell.end() )
	return FALSE;
    QTextTableCell *cell = cells.at( *currCell.find( c ) );
    if ( !cell )
	return FALSE;
    doc = cell->richText();
    parag = doc->firstParag();
    idx = parag->length() - 1;
    ox += cell->geometry().x() + cell->horizontalAlignmentOffset() + outerborder + parent->x();
    oy += cell->geometry().y()  + cell->verticalAlignmentOffset() + outerborder;
    return TRUE;
}

bool QTextTable::down( QTextCursor *c, QTextDocument *&doc, QTextParag *&parag, int &idx, int &ox, int &oy )
{
    if ( currCell.find( c ) == currCell.end() )
	return FALSE;
    QTextTableCell *cell = cells.at( *currCell.find( c ) );
    if ( cell->row_ == layout->numRows() - 1 ) {
	currCell.insert( c, 0 );
	QTextCustomItem::down( c, doc, parag, idx, ox, oy );
	QTextTableCell *cell = cells.first();
	if ( !cell )
	    return FALSE;
	doc = cell->richText();
	idx = -1;
	return TRUE;
    }

    int oldRow = cell->row_;
    int oldCol = cell->col_;
    if ( currCell.find( c ) == currCell.end() )
	return FALSE;
    int cc = *currCell.find( c );
    for ( int i = cc; i < (int)cells.count(); ++i ) {
	cell = cells.at( i );
	if ( cell->row_ > oldRow && cell->col_ == oldCol ) {
	    currCell.insert( c, i );
	    break;
	}
    }
    doc = cell->richText();
    if ( !cell )
	return FALSE;
    parag = doc->firstParag();
    idx = 0;
    ox += cell->geometry().x() + cell->horizontalAlignmentOffset() + outerborder + parent->x();
    oy += cell->geometry().y()  + cell->verticalAlignmentOffset() + outerborder;
    return TRUE;
}

bool QTextTable::up( QTextCursor *c, QTextDocument *&doc, QTextParag *&parag, int &idx, int &ox, int &oy )
{
    if ( currCell.find( c ) == currCell.end() )
	return FALSE;
    QTextTableCell *cell = cells.at( *currCell.find( c ) );
    if ( cell->row_ == 0 ) {
	currCell.insert( c, 0 );
	QTextCustomItem::up( c, doc, parag, idx, ox, oy );
	QTextTableCell *cell = cells.first();
	if ( !cell )
	    return FALSE;
	doc = cell->richText();
	idx = -1;
	return TRUE;
    }

    int oldRow = cell->row_;
    int oldCol = cell->col_;
    if ( currCell.find( c ) == currCell.end() )
	return FALSE;
    int cc = *currCell.find( c );
    for ( int i = cc; i >= 0; --i ) {
	cell = cells.at( i );
	if ( cell->row_ < oldRow && cell->col_ == oldCol ) {
	    currCell.insert( c, i );
	    break;
	}
    }
    doc = cell->richText();
    if ( !cell )
	return FALSE;
    parag = doc->lastParag();
    idx = parag->length() - 1;
    ox += cell->geometry().x() + cell->horizontalAlignmentOffset() + outerborder + parent->x();
    oy += cell->geometry().y()  + cell->verticalAlignmentOffset() + outerborder;
    return TRUE;
}

QTextTableCell::QTextTableCell( QTextTable* table,
				int row, int column,
				const QMap<QString, QString> &attr,
				const QStyleSheetItem* /*style*/, // ### use them
				const QTextFormat& /*fmt*/, const QString& context,
				QMimeSourceFactory &factory, QStyleSheet *sheet,
				const QString& doc)
{
#if defined(PARSER_DEBUG)
    qDebug( debug_indent + "new QTextTableCell1 (pappi: %p)", table );
    qDebug( debug_indent + doc );
#endif
    cached_width = -1;
    cached_sizehint = -1;

    maxw = QWIDGETSIZE_MAX;
    minw = 0;

    parent = table;
    row_ = row;
    col_ = column;
    stretch_ = 0;
    richtext = new QTextDocument( table->parent );
    richtext->setTableCell( this );
    QString a = *attr.find( "align" );
    if ( !a.isEmpty() ) {
	a = a.lower();
	if ( a == "left" )
	    richtext->setAlignment( Qt::AlignLeft );
	else if ( a == "center" )
	    richtext->setAlignment( Qt::AlignHCenter );
	else if ( a == "right" )
	    richtext->setAlignment( Qt::AlignRight );
    }
    align = 0;
    QString va = *attr.find( "valign" );
    if ( !va.isEmpty() ) {
	va = va.lower();
	if ( va == "center" )
	    align |= Qt::AlignVCenter;
	else if ( va == "bottom" )
	    align |= Qt::AlignBottom;
    }
    richtext->setFormatter( table->parent->formatter() );
    richtext->setUseFormatCollection( table->parent->useFormatCollection() );
    richtext->setMimeSourceFactory( &factory );
    richtext->setStyleSheet( sheet );
    richtext->setDefaultFont( table->parent->formatCollection()->defaultFormat()->font() );
    richtext->setRichText( doc, context );
    rowspan_ = 1;
    colspan_ = 1;
    if ( attr.contains("colspan") )
	colspan_ = attr["colspan"].toInt();
    if ( attr.contains("rowspan") )
	rowspan_ = attr["rowspan"].toInt();

    background = 0;
    if ( attr.contains("bgcolor") ) {
	background = new QBrush(QColor( attr["bgcolor"] ));
    }


    hasFixedWidth = FALSE;
    if ( attr.contains("width") ) {
	bool b;
	QString s( attr["width"] );
	int w = s.toInt( &b );
	if ( b ) {
	    maxw = w;
	    minw = maxw;
	    hasFixedWidth = TRUE;
	} else {
	    s = s.stripWhiteSpace();
	    if ( s.length() > 1 && s[ (int)s.length()-1 ] == '%' )
		stretch_ = s.left( s.length()-1).toInt();
	}
    }

    attributes = attr;

    parent->addCell( this );
}

QTextTableCell::QTextTableCell( QTextTable* table, int row, int column )
{
#if defined(PARSER_DEBUG)
    qDebug( debug_indent + "new QTextTableCell2( pappi: %p", table );
#endif
    maxw = QWIDGETSIZE_MAX;
    minw = 0;
    cached_width = -1;
    cached_sizehint = -1;

    parent = table;
    row_ = row;
    col_ = column;
    stretch_ = 0;
    richtext = new QTextDocument( table->parent );
    richtext->setTableCell( this );
    richtext->setFormatter( table->parent->formatter() );
    richtext->setUseFormatCollection( table->parent->useFormatCollection() );
    richtext->setDefaultFont( table->parent->formatCollection()->defaultFormat()->font() );
    richtext->setRichText( "<html></html>", QString::null );
    rowspan_ = 1;
    colspan_ = 1;
    background = 0;
    hasFixedWidth = FALSE;
    parent->addCell( this );
}


QTextTableCell::~QTextTableCell()
{
    delete background;
    background = 0;
    delete richtext;
    richtext = 0;
}

QSize QTextTableCell::sizeHint() const
{
    int extra = 2 * ( parent->innerborder + parent->cellpadding + border_tolerance);
    int used = richtext->widthUsed() + extra;

    if  (stretch_ ) {
	int w = parent->width * stretch_ / 100 - 2*parent->cellspacing - 2*parent->cellpadding;
	return QSize( QMIN( w, maxw ), 0 ).expandedTo( minimumSize() );
    }

    return QSize( used, 0 ).expandedTo( minimumSize() );
}

QSize QTextTableCell::minimumSize() const
{
    int extra = 2 * ( parent->innerborder + parent->cellpadding + border_tolerance);
    return QSize( QMAX( richtext->minimumWidth() + extra, minw), 0 );
}

QSize QTextTableCell::maximumSize() const
{
    return QSize( QWIDGETSIZE_MAX, QWIDGETSIZE_MAX );
}

QSizePolicy::ExpandData QTextTableCell::expanding() const
{
    return QSizePolicy::BothDirections;
}

bool QTextTableCell::isEmpty() const
{
    return FALSE;
}
void QTextTableCell::setGeometry( const QRect& r )
{
    int extra = 2 * ( parent->innerborder + parent->cellpadding );
    if ( r.width() != cached_width )
	richtext->doLayout( QTextFormat::painter(), r.width() - extra );
    cached_width = r.width();
    geom = r;
}

QRect QTextTableCell::geometry() const
{
    return geom;
}

bool QTextTableCell::hasHeightForWidth() const
{
    return TRUE;
}

int QTextTableCell::heightForWidth( int w ) const
{
    int extra = 2 * ( parent->innerborder + parent->cellpadding );
    w = QMAX( minw, w );

    if ( cached_width != w ) {
	QTextTableCell* that = (QTextTableCell*) this;
	that->richtext->doLayout( QTextFormat::painter(), w - extra );
	that->cached_width = w;
    }
    return richtext->height() + extra;
}

void QTextTableCell::adjustToPainter( QPainter* p )
{
    QTextParag *parag = richtext->firstParag();
    while ( parag ) {
	parag->adjustToPainter( p );
	parag = parag->next();
    }
}

int QTextTableCell::horizontalAlignmentOffset() const
{
    return parent->cellpadding;
}

int QTextTableCell::verticalAlignmentOffset() const
{
    if ( (align & Qt::AlignVCenter ) == Qt::AlignVCenter )
	return ( geom.height() - richtext->height() ) / 2;
    else if ( ( align & Qt::AlignBottom ) == Qt::AlignBottom )
	return geom.height() - parent->cellpadding - richtext->height()  ;
    return parent->cellpadding;
}

void QTextTableCell::draw( QPainter* p, int x, int y, int cx, int cy, int cw, int ch, const QColorGroup& cg, bool )
{
    if ( cached_width != geom.width() ) {
	int extra = 2 * ( parent->innerborder + parent->cellpadding );
	richtext->doLayout( p, geom.width() - extra );
	cached_width = geom.width();
    }
    QColorGroup g( cg );
    if ( background )
	g.setBrush( QColorGroup::Base, *background );
    else if ( richtext->paper() )
	g.setBrush( QColorGroup::Base, *richtext->paper() );

    p->save();
    p->translate( x + geom.x(), y + geom.y() );
    if ( background )
	p->fillRect( 0, 0, geom.width(), geom.height(), *background );
    else if ( richtext->paper() )
	p->fillRect( 0, 0, geom.width(), geom.height(), *richtext->paper() );

    p->translate( horizontalAlignmentOffset(), verticalAlignmentOffset() );

    QRegion r;
    QTextCursor *c = 0;
    if ( richtext->parent()->tmpCursor )
	c = richtext->parent()->tmpCursor;
    if ( cx >= 0 && cy >= 0 )
	richtext->draw( p, cx - ( x + horizontalAlignmentOffset() + geom.x() ),
			cy - ( y + geom.y() + verticalAlignmentOffset() ),
			cw, ch, g, FALSE, (c != 0), c );
    else
	richtext->draw( p, -1, -1, -1, -1, g, FALSE, (c != 0), c );

    p->restore();
}
