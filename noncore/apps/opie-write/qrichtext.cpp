/****************************************************************************
** $Id: qrichtext.cpp,v 1.2 2002-07-15 23:22:50 leseb Exp $
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
#include "qapplication.h"
#include "qmap.h"
#include "qfileinfo.h"
#include "qstylesheet.h"
#include "qmime.h"
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

static QTextCursor* richTextExportStart = 0;
static QTextCursor* richTextExportEnd = 0;

static QTextFormatCollection *qFormatCollection = 0;

const int border_tolerance = 2;

#ifdef Q_WS_WIN
#include "qt_windows.h"
#endif

#define QChar_linesep QChar(0x2028U)

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
					const QByteArray& oldStyleInfo )
    : QTextCommand( d ), id( i ), index( idx ), parag( 0 ), text( str ), styleInformation( oldStyleInfo )
{
    for ( int j = 0; j < (int)text.size(); ++j ) {
	if ( text[ j ].format() )
	    text[ j ].format()->addRef();
    }
}

QTextDeleteCommand::QTextDeleteCommand( QTextParagraph *p, int idx, const QMemArray<QTextStringChar> &str )
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
    QTextParagraph *s = doc ? doc->paragAt( id ) : parag;
    if ( !s ) {
	qWarning( "can't locate parag at %d, last parag: %d", id, doc->lastParagraph()->paragId() );
	return 0;
    }

    cursor.setParagraph( s );
    cursor.setIndex( index );
    int len = text.size();
    if ( c )
	*c = cursor;
    if ( doc ) {
	doc->setSelectionStart( QTextDocument::Temp, cursor );
	for ( int i = 0; i < len; ++i )
	    cursor.gotoNextLetter();
	doc->setSelectionEnd( QTextDocument::Temp, cursor );
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
    QTextParagraph *s = doc ? doc->paragAt( id ) : parag;
    if ( !s ) {
	qWarning( "can't locate parag at %d, last parag: %d", id, doc->lastParagraph()->paragId() );
	return 0;
    }

    cursor.setParagraph( s );
    cursor.setIndex( index );
    QString str = QTextString::toString( text );
    cursor.insert( str, TRUE, &text );
    cursor.setParagraph( s );
    cursor.setIndex( index );
    if ( c ) {
	c->setParagraph( s );
	c->setIndex( index );
	for ( int i = 0; i < (int)text.size(); ++i )
	    c->gotoNextLetter();
    }

    if ( !styleInformation.isEmpty() ) {
	QDataStream styleStream( styleInformation, IO_ReadOnly );
	int num;
	styleStream >> num;
	QTextParagraph *p = s;
	while ( num-- && p ) {
	    p->readStyleInformation( styleStream );
	    p = p->next();
	}
    }
    s = cursor.paragraph();
    while ( s ) {
	s->format();
	s->setChanged( TRUE );
	if ( s == c->paragraph() )
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
    QTextParagraph *sp = doc->paragAt( startId );
    QTextParagraph *ep = doc->paragAt( endId );
    if ( !sp || !ep )
	return c;

    QTextCursor start( doc );
    start.setParagraph( sp );
    start.setIndex( startIndex );
    QTextCursor end( doc );
    end.setParagraph( ep );
    end.setIndex( endIndex );

    doc->setSelectionStart( QTextDocument::Temp, start );
    doc->setSelectionEnd( QTextDocument::Temp, end );
    doc->setFormat( QTextDocument::Temp, format, flags );
    doc->removeSelection( QTextDocument::Temp );
    if ( endIndex == ep->length() )
	end.gotoLeft();
    *c = end;
    return c;
}

QTextCursor *QTextFormatCommand::unexecute( QTextCursor *c )
{
    QTextParagraph *sp = doc->paragAt( startId );
    QTextParagraph *ep = doc->paragAt( endId );
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
    end.setParagraph( ep );
    end.setIndex( endIndex );
    if ( endIndex == ep->length() )
	end.gotoLeft();
    *c = end;
    return c;
}

QTextStyleCommand::QTextStyleCommand( QTextDocument *d, int fParag, int lParag, const QByteArray& beforeChange )
    : QTextCommand( d ), firstParag( fParag ), lastParag( lParag ), before( beforeChange )
{
    after = readStyleInformation(  d, fParag, lParag );
}


QByteArray QTextStyleCommand::readStyleInformation(  QTextDocument* doc, int fParag, int lParag )
{
    QByteArray style;
    QTextParagraph *p = doc->paragAt( fParag );
    if ( !p )
	return style;
    QDataStream styleStream( style, IO_WriteOnly );
    int num = lParag - fParag + 1;
    styleStream << num;
    while ( num -- && p ) {
	p->writeStyleInformation( styleStream );
	p = p->next();
    }
    return style;
}

void QTextStyleCommand::writeStyleInformation(  QTextDocument* doc, int fParag, const QByteArray& style )
{
    QTextParagraph *p = doc->paragAt( fParag );
    if ( !p )
	return;
    QDataStream styleStream( style, IO_ReadOnly );
    int num;
    styleStream >> num;
    while ( num-- && p ) {
	p->readStyleInformation( styleStream );
	p = p->next();
    }
}

QTextCursor *QTextStyleCommand::execute( QTextCursor *c )
{
    writeStyleInformation( doc, firstParag, after );
    return c;
}

QTextCursor *QTextStyleCommand::unexecute( QTextCursor *c )
{
    writeStyleInformation( doc, firstParag, before );
    return c;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

QTextCursor::QTextCursor( QTextDocument *d )
    : idx( 0 ), tmpIndex( -1 ), ox( 0 ), oy( 0 ),
      valid( TRUE )
{
    para = d ? d->firstParagraph() : 0;
}

QTextCursor::QTextCursor( const QTextCursor &c )
{
    ox = c.ox;
    oy = c.oy;
    idx = c.idx;
    para = c.para;
    tmpIndex = c.tmpIndex;
    indices = c.indices;
    paras = c.paras;
    xOffsets = c.xOffsets;
    yOffsets = c.yOffsets;
    valid = c.valid;
}

QTextCursor &QTextCursor::operator=( const QTextCursor &c )
{
    ox = c.ox;
    oy = c.oy;
    idx = c.idx;
    para = c.para;
    tmpIndex = c.tmpIndex;
    indices = c.indices;
    paras = c.paras;
    xOffsets = c.xOffsets;
    yOffsets = c.yOffsets;
    valid = c.valid;

    return *this;
}

bool QTextCursor::operator==( const QTextCursor &c ) const
{
    return para == c.para && idx == c.idx;
}

int QTextCursor::totalOffsetX() const
{
    int xoff = ox;
    for ( QValueStack<int>::ConstIterator xit = xOffsets.begin(); xit != xOffsets.end(); ++xit )
	xoff += *xit;
    return xoff;
}

int QTextCursor::totalOffsetY() const
{
    int yoff = oy;
    for ( QValueStack<int>::ConstIterator yit = yOffsets.begin(); yit != yOffsets.end(); ++yit )
	yoff += *yit;
    return yoff;
}

void QTextCursor::gotoIntoNested( const QPoint &globalPos )
{
    if ( !para )
	return;
    push();
    ox = 0;
    int bl, y;
    para->lineHeightOfChar( idx, &bl, &y );
    oy = y + para->rect().y();
    QPoint p( globalPos.x() - offsetX(), globalPos.y() - offsetY() );
    Q_ASSERT( para->at( idx )->isCustom() );
    ox = para->at( idx )->x;

    QTextDocument* doc = document();
    para->at( idx )->customItem()->enterAt( this, doc, para, idx, ox, oy, p );
}

void QTextCursor::invalidateNested()
{
    QValueStack<QTextParagraph*>::Iterator it = paras.begin();
    QValueStack<int>::Iterator it2 = indices.begin();
    for ( ; it != paras.end(); ++it, ++it2 ) {
	if ( *it == para )
	    continue;
	(*it)->invalidate( 0 );
	if ( (*it)->at( *it2 )->isCustom() )
	    (*it)->at( *it2 )->customItem()->invalidate();
    }
}

void QTextCursor::insert( const QString &str, bool checkNewLine, QMemArray<QTextStringChar> *formatting )
{
    tmpIndex = -1;
    bool justInsert = TRUE;
    QString s( str );
#if defined(Q_WS_WIN)
    if ( checkNewLine ) {
	int i = 0;
	while ( ( i = s.find( '\r', i ) ) != -1 )
	    s.remove( i ,1 );
    }
#endif
    if ( checkNewLine )
	justInsert = s.find( '\n' ) == -1;
    if ( justInsert ) { // we ignore new lines and insert all in the current para at the current index
	para->insert( idx, s.unicode(), s.length() );
	if ( formatting ) {
	    for ( int i = 0; i < (int)s.length(); ++i ) {
		if ( formatting->at( i ).format() ) {
		    formatting->at( i ).format()->addRef();
		    para->string()->setFormat( idx + i, formatting->at( i ).format(), TRUE );
		}
	    }
	}
	idx += s.length();
    } else { // we split at new lines
	int start = -1;
	int end;
	int y = para->rect().y() + para->rect().height();
	int lastIndex = 0;
	do {
	    end = s.find( '\n', start + 1 ); // find line break
	    if ( end == -1 ) // didn't find one, so end of line is end of string
		end = s.length();
	    int len = (start == -1 ? end : end - start - 1);
	    if ( len > 0 ) // insert the line
		para->insert( idx, s.unicode() + start + 1, len );
	    else
		para->invalidate( 0 );
	    if ( formatting ) { // set formats to the chars of the line
		for ( int i = 0; i < len; ++i ) {
		    if ( formatting->at( i + lastIndex ).format() ) {
			formatting->at( i + lastIndex ).format()->addRef();
			para->string()->setFormat( i + idx, formatting->at( i + lastIndex ).format(), TRUE );
		    }
		}
		lastIndex += len;
	    }
	    start = end; // next start is at the end of this line
	    idx += len; // increase the index of the cursor to the end of the inserted text
	    if ( s[end] == '\n' ) { // if at the end was a line break, break the line
		splitAndInsertEmptyParagraph( FALSE, TRUE );
		para->setEndState( -1 );
		para->prev()->format( -1, FALSE );
		lastIndex++;
	    }

	} while ( end < (int)s.length() );

	para->format( -1, FALSE );
	int dy = para->rect().y() + para->rect().height() - y;
	QTextParagraph *p = para;
	p->setParagId( p->prev() ? p->prev()->paragId() + 1 : 0 );
	p = p->next();
	while ( p ) {
	    p->setParagId( p->prev()->paragId() + 1 );
	    p->move( dy );
	    p->invalidate( 0 );
	    p->setEndState( -1 );
	    p = p->next();
	}
    }

    int h = para->rect().height();
    para->format( -1, TRUE );
    if ( h != para->rect().height() )
	invalidateNested();
    else if ( para->document() && para->document()->parent() )
	para->document()->nextDoubleBuffered = TRUE;
}

void QTextCursor::gotoLeft()
{
    if ( para->string()->isRightToLeft() )
	gotoNextLetter();
    else
	gotoPreviousLetter();
}

void QTextCursor::gotoPreviousLetter()
{
    tmpIndex = -1;

    if ( idx > 0 ) {
	idx--;
	const QTextStringChar *tsc = para->at( idx );
	if ( tsc && tsc->isCustom() && tsc->customItem()->isNested() )
	    processNesting( EnterEnd );
    } else if ( para->prev() ) {
	para = para->prev();
	while ( !para->isVisible() && para->prev() )
	    para = para->prev();
	idx = para->length() - 1;
    } else if ( nestedDepth() ) {
	pop();
	processNesting( Prev );
	if ( idx == -1 ) {
	    pop();
	    if ( idx > 0 ) {
		idx--;
	    } else if ( para->prev() ) {
		para = para->prev();
		idx = para->length() - 1;
	    }
	}
    }
}

void QTextCursor::push()
{
    indices.push( idx );
    paras.push( para );
    xOffsets.push( ox );
    yOffsets.push( oy );
}

void QTextCursor::pop()
{
    if ( indices.isEmpty() )
	return;
    idx = indices.pop();
    para = paras.pop();
    ox = xOffsets.pop();
    oy = yOffsets.pop();
}

void QTextCursor::restoreState()
{
    while ( !indices.isEmpty() )
	pop();
}

bool QTextCursor::place( const QPoint &p, QTextParagraph *s, bool link )
{
    QPoint pos( p );
    QRect r;
    QTextParagraph *str = s;
    if ( pos.y() < s->rect().y() )
	pos.setY( s->rect().y() );
    while ( s ) {
	r = s->rect();
	r.setWidth( document() ? document()->width() : QWIDGETSIZE_MAX );
	if ( s->isVisible() )
	    str = s;
	if ( pos.y() >= r.y() && pos.y() <= r.y() + r.height() || !s->next() )
	    break;
	s = s->next();
    }

    if ( !s || !str )
	return FALSE;

    s = str;

    setParagraph( s );
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
	if ( pos.y() <= y + cy + ch )
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
    setIndex( curpos );

    if ( inCustom && para->document() && para->at( curpos )->isCustom() && para->at( curpos )->customItem()->isNested() ) {
	QTextDocument *oldDoc = para->document();
	gotoIntoNested( pos );
	if ( oldDoc == para->document() )
	    return TRUE;
	QPoint p( pos.x() - offsetX(), pos.y() - offsetY() );
	if ( !place( p, document()->firstParagraph(), link ) )
	    pop();
    }
    return TRUE;
}

void QTextCursor::processNesting( Operation op )
{
    if ( !para->document() )
	return;
    QTextDocument* doc = para->document();
    push();
    ox = para->at( idx )->x;
    int bl, y;
    para->lineHeightOfChar( idx, &bl, &y );
    oy = y + para->rect().y();
    bool ok = FALSE;

    switch ( op ) {
    case EnterBegin:
	ok = para->at( idx )->customItem()->enter( this, doc, para, idx, ox, oy );
	break;
    case EnterEnd:
	ok = para->at( idx )->customItem()->enter( this, doc, para, idx, ox, oy, TRUE );
	break;
    case Next:
	ok = para->at( idx )->customItem()->next( this, doc, para, idx, ox, oy );
	break;
    case Prev:
	ok = para->at( idx )->customItem()->prev( this, doc, para, idx, ox, oy );
	break;
    case Down:
	ok = para->at( idx )->customItem()->down( this, doc, para, idx, ox, oy );
	break;
    case Up:
	ok = para->at( idx )->customItem()->up( this, doc, para, idx, ox, oy );
	break;
    }
    if ( !ok )
	pop();
}

void QTextCursor::gotoRight()
{
    if ( para->string()->isRightToLeft() )
	gotoPreviousLetter();
    else
	gotoNextLetter();
}

void QTextCursor::gotoNextLetter()
{
    tmpIndex = -1;

    const QTextStringChar *tsc = para->at( idx );
    if ( tsc && tsc->isCustom() && tsc->customItem()->isNested() ) {
	processNesting( EnterBegin );
	return;
    }

    if ( idx < para->length() - 1 ) {
	idx++;
    } else if ( para->next() ) {
	para = para->next();
	while ( !para->isVisible() && para->next() )
	    para = para->next();
	idx = 0;
    } else if ( nestedDepth() ) {
	pop();
	processNesting( Next );
	if ( idx == -1 ) {
	    pop();
	    if ( idx < para->length() - 1 ) {
		idx++;
	    } else if ( para->next() ) {
		para = para->next();
		idx = 0;
	    }
	}
    }
}

void QTextCursor::gotoUp()
{
    int indexOfLineStart;
    int line;
    QTextStringChar *c = para->lineStartOfChar( idx, &indexOfLineStart, &line );
    if ( !c )
	return;

    tmpIndex = QMAX( tmpIndex, idx - indexOfLineStart );
    if ( indexOfLineStart == 0 ) {
	if ( !para->prev() ) {
	    if ( !nestedDepth() )
		return;
	    pop();
	    processNesting( Up );
	    if ( idx == -1 ) {
		pop();
		if ( !para->prev() )
		    return;
		idx = tmpIndex = 0;
	    } else {
		tmpIndex = -1;
		return;
	    }
	}
	QTextParagraph *p = para->prev();
	while ( p && !p->isVisible() )
	    p = p->prev();
	if ( p )
	    para = p;
	int lastLine = para->lines() - 1;
	if ( !para->lineStartOfLine( lastLine, &indexOfLineStart ) )
	    return;
	if ( indexOfLineStart + tmpIndex < para->length() )
	    idx = indexOfLineStart + tmpIndex;
	else
	    idx = para->length() - 1;
    } else {
	--line;
	int oldIndexOfLineStart = indexOfLineStart;
	if ( !para->lineStartOfLine( line, &indexOfLineStart ) )
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
    QTextStringChar *c = para->lineStartOfChar( idx, &indexOfLineStart, &line );
    if ( !c )
	return;

    tmpIndex = QMAX( tmpIndex, idx - indexOfLineStart );
    if ( line == para->lines() - 1 ) {
	if ( !para->next() ) {
	    if ( !nestedDepth() )
		return;
	    pop();
	    processNesting( Down );
	    if ( idx == -1 ) {
		pop();
		if ( !para->next() )
		    return;
		idx = tmpIndex = 0;
	    } else {
		tmpIndex = -1;
		return;
	    }
	}
	QTextParagraph *s = para->next();
	while ( s && !s->isVisible() )
	    s = s->next();
	if ( s )
	    para = s;
	if ( !para->lineStartOfLine( 0, &indexOfLineStart ) )
	    return;
	int end;
	if ( para->lines() == 1 )
	    end = para->length();
	else
	    para->lineStartOfLine( 1, &end );
	if ( indexOfLineStart + tmpIndex < end )
	    idx = indexOfLineStart + tmpIndex;
	else
	    idx = end - 1;
    } else {
	++line;
	int end;
	if ( line == para->lines() - 1 )
	    end = para->length();
	else
	    para->lineStartOfLine( line + 1, &end );
	if ( !para->lineStartOfLine( line, &indexOfLineStart ) )
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
    QTextStringChar *c = para->lineStartOfChar( idx, &indexOfLineStart, &line );
    if ( !c )
	return;

    if ( line == para->lines() - 1 ) {
	idx = para->length() - 1;
    } else {
	c = para->lineStartOfLine( ++line, &indexOfLineStart );
	indexOfLineStart--;
	idx = indexOfLineStart;
    }
}

void QTextCursor::gotoLineStart()
{
    tmpIndex = -1;
    int indexOfLineStart;
    int line;
    QTextStringChar *c = para->lineStartOfChar( idx, &indexOfLineStart, &line );
    if ( !c )
	return;

    idx = indexOfLineStart;
}

void QTextCursor::gotoHome()
{
    if ( topParagraph()->document() )
	gotoPosition( topParagraph()->document()->firstParagraph() );
    else
	gotoLineStart();
}

void QTextCursor::gotoEnd()
{
    if ( topParagraph()->document() && topParagraph()->document()->lastParagraph()->isValid() )
	gotoPosition( topParagraph()->document()->lastParagraph(),
		      topParagraph()->document()->lastParagraph()->length() - 1);
    else
	gotoLineEnd();
}

void QTextCursor::gotoPageUp( int visibleHeight )
{
    int targetY  = globalY() - visibleHeight;
    QTextParagraph* old; int index;
    do {
	old = para; index = idx;
	gotoUp();
    } while ( (old != para || index != idx)  && globalY() > targetY );
}

void QTextCursor::gotoPageDown( int visibleHeight )
{
    int targetY  = globalY() + visibleHeight;
    QTextParagraph* old; int index;
    do {
	old = para; index = idx;
	gotoDown();
    } while ( (old != para || index != idx) && globalY() < targetY );
}

void QTextCursor::gotoWordRight()
{
    if ( para->string()->isRightToLeft() )
	gotoPreviousWord();
    else
	gotoNextWord();
}

void QTextCursor::gotoWordLeft()
{
    if ( para->string()->isRightToLeft() )
	gotoNextWord();
    else
	gotoPreviousWord();
}

void QTextCursor::gotoPreviousWord()
{
    gotoPreviousLetter();
    tmpIndex = -1;
    QTextString *s = para->string();
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
    QTextString *s = para->string();
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
    } else if ( para->next() ) {
	QTextParagraph *p = para->next();
	while ( p  && !p->isVisible() )
	    p = p->next();
	if ( s ) {
	    para = p;
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
    return idx == para->length() - 1;
}

void QTextCursor::splitAndInsertEmptyParagraph( bool ind, bool updateIds )
{
    if ( !para->document() )
	return;
    tmpIndex = -1;
    QTextFormat *f = 0;
    if ( para->document()->useFormatCollection() ) {
	f = para->at( idx )->format();
	if ( idx == para->length() - 1 && idx > 0 )
	    f = para->at( idx - 1 )->format();
	if ( f->isMisspelled() ) {
	    f->removeRef();
	    f = para->document()->formatCollection()->format( f->font(), f->color() );
	}
    }

    if ( atParagEnd() ) {
	QTextParagraph *n = para->next();
	QTextParagraph *s = para->document()->createParagraph( para->document(), para, n, updateIds );
	if ( f )
	    s->setFormat( 0, 1, f, TRUE );
	s->copyParagData( para );
	if ( ind ) {
	    int oi, ni;
	    s->indent( &oi, &ni );
	    para = s;
	    idx = ni;
	} else {
	    para = s;
	    idx = 0;
	}
    } else if ( atParagStart() ) {
	QTextParagraph *p = para->prev();
	QTextParagraph *s = para->document()->createParagraph( para->document(), p, para, updateIds );
	if ( f )
	    s->setFormat( 0, 1, f, TRUE );
	s->copyParagData( para );
	if ( ind ) {
	    s->indent();
	    s->format();
	    indent();
	    para->format();
	}
    } else {
	QString str = para->string()->toString().mid( idx, 0xFFFFFF );
	QTextParagraph *n = para->next();
	QTextParagraph *s = para->document()->createParagraph( para->document(), para, n, updateIds );
	s->copyParagData( para );
	s->remove( 0, 1 );
	s->append( str, TRUE );
	for ( uint i = 0; i < str.length(); ++i ) {
	    QTextStringChar* tsc = para->at( idx + i );
	    s->setFormat( i, 1, tsc->format(), TRUE );
	    if ( tsc->isCustom() ) {
		QTextCustomItem * item = tsc->customItem();
		s->at( i )->setCustomItem( item );
		tsc->loseCustomItem();
	    }
	    if ( tsc->isAnchor() )
		s->at( i )->setAnchor( tsc->anchorName(),
				       tsc->anchorHref() );
	}
	para->truncate( idx );
	if ( ind ) {
	    int oi, ni;
	    s->indent( &oi, &ni );
	    para = s;
	    idx = ni;
	} else {
	    para = s;
	    idx = 0;
	}
    }

    invalidateNested();
}

bool QTextCursor::remove()
{
    tmpIndex = -1;
    if ( !atParagEnd() ) {
	para->remove( idx, 1 );
	int h = para->rect().height();
	para->format( -1, TRUE );
	if ( h != para->rect().height() )
	    invalidateNested();
	else if ( para->document() && para->document()->parent() )
	    para->document()->nextDoubleBuffered = TRUE;
	return FALSE;
    } else if ( para->next() ) {
	para->join( para->next() );
	invalidateNested();
	return TRUE;
    }
    return FALSE;
}

void QTextCursor::indent()
{
    int oi = 0, ni = 0;
    para->indent( &oi, &ni );
    if ( oi == ni )
	return;

    if ( idx >= oi )
	idx += ni - oi;
    else
	idx = ni;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

QTextDocument::QTextDocument( QTextDocument *p )
    : par( p ), parentPar( 0 ), tc( 0 ), tArray( 0 ), tStopWidth( 0 )
{
    fCollection = new QTextFormatCollection;
    init();
}

QTextDocument::QTextDocument( QTextDocument *p, QTextFormatCollection *f )
    : par( p ), parentPar( 0 ), tc( 0 ), tArray( 0 ), tStopWidth( 0 )
{
    fCollection = f;
    init();
}

void QTextDocument::init()
{
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

    setStyleSheet( QStyleSheet::defaultSheet() );
    factory_ = QMimeSourceFactory::defaultFactory();
    contxt = QString::null;

    underlLinks = par ? par->underlLinks : TRUE;
    backBrush = 0;
    buf_pixmap = 0;
    nextDoubleBuffered = FALSE;

    if ( par )
	withoutDoubleBuffer = par->withoutDoubleBuffer;
    else
	withoutDoubleBuffer = FALSE;

    lParag = fParag = createParagraph( this, 0, 0 );

    cx = 0;
    cy = 2;
    if ( par )
	cx = cy = 0;
    cw = 600;
    vw = 0;
    flow_ = new QTextFlow;
    flow_->setWidth( cw );

    leftmargin = rightmargin = 4;
    scaleFontsFactor = 1;


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
	QTextParagraph *p = fParag->next();
	delete fParag;
	fParag = p;
    }
    fParag = lParag = 0;
    if ( createEmptyParag )
	fParag = lParag = createParagraph( this );
    selections.clear();
    oText = QString::null;
    oTextValid = TRUE;
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



QTextParagraph *QTextDocument::createParagraph( QTextDocument *d, QTextParagraph *pr, QTextParagraph *nx, bool updateIds )
{
    return new QTextParagraph( d, pr, nx, updateIds );
}

bool QTextDocument::setMinimumWidth( int needed, int used, QTextParagraph *p )
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
	lParag = createParagraph( this, lParag, 0 );
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
	    lParag = createParagraph( this, lParag, 0 );
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
	lParag = fParag = createParagraph( this, 0, 0 );
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


#define NEWPAR       do{ if ( !hasNewPar) { \
		    if ( !textEditMode && curpar && curpar->length()>1 && curpar->at( curpar->length()-2)->c == QChar_linesep ) \
			curpar->remove( curpar->length()-2, 1 ); \
		    curpar = createParagraph( this, curpar, curpar->next() ); styles.append( vec ); vec = 0;} \
		    hasNewPar = TRUE; \
		    curpar->rtext = TRUE;  \
		    curpar->align = curtag.alignment; \
		    curpar->lstyle = curtag.liststyle; \
		    curpar->litem = ( curtag.style->displayMode() == QStyleSheetItem::DisplayListItem ); \
		    curpar->str->setDirection( (QChar::Direction)curtag.direction ); \
		    space = TRUE; \
		    delete vec; vec = new QPtrVector<QStyleSheetItem>( (uint)tags.count() + 1); \
		    int i = 0; \
		    for ( QValueStack<QTextDocumentTag>::Iterator it = tags.begin(); it != tags.end(); ++it ) \
			vec->insert( i++, (*it).style ); \
		    vec->insert( i, curtag.style ); \
		    }while(FALSE)


void QTextDocument::setRichText( const QString &text, const QString &context )
{
    if ( !context.isEmpty() )
	setContext( context );
    clear();
    fParag = lParag = createParagraph( this );
    oTextValid = TRUE;
    oText = text;
    setRichTextInternal( text );
    fParag->rtext = TRUE;
}

void QTextDocument::setRichTextInternal( const QString &text, QTextCursor* cursor )
{
    QTextParagraph* curpar = lParag;
    int pos = 0;
    QValueStack<QTextDocumentTag> tags;
    QTextDocumentTag initag( "", sheet_->item(""), *formatCollection()->defaultFormat() );
    QTextDocumentTag curtag = initag;
    bool space = TRUE;
    bool canMergeLi = FALSE;

    bool textEditMode = FALSE;

    const QChar* doc = text.unicode();
    int length = text.length();
    bool hasNewPar = curpar->length() <= 1;
    QString anchorName;

    // style sheet handling for margin and line spacing calculation below
    QTextParagraph* stylesPar = curpar;
    QPtrVector<QStyleSheetItem>* vec = 0;
    QPtrList< QPtrVector<QStyleSheetItem> > styles;
    styles.setAutoDelete( TRUE );

    if ( cursor ) {
	cursor->splitAndInsertEmptyParagraph();
	QTextCursor tmp = *cursor;
	tmp.gotoPreviousLetter();
	stylesPar = curpar = tmp.paragraph();
	hasNewPar = TRUE;
	textEditMode = TRUE;
    } else {
	NEWPAR;
    }

    // set rtext spacing to FALSE for the initial paragraph.
    curpar->rtext = FALSE;

    QString wellKnownTags = "br hr wsp table qt body meta title";

    while ( pos < length ) {
	if ( hasPrefix(doc, length, pos, '<' ) ){
	    if ( !hasPrefix( doc, length, pos+1, QChar('/') ) ) {
		// open tag
		QMap<QString, QString> attr;
		bool emptyTag = FALSE;
		QString tagname = parseOpenTag(doc, length, pos, attr, emptyTag);
		if ( tagname.isEmpty() )
		    continue; // nothing we could do with this, probably parse error

		const QStyleSheetItem* nstyle = sheet_->item(tagname);

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

		    /* special handling for p and li for HTML
		       compatibility. We do not want to embed blocks in
		       p, and we do not want new blocks inside non-empty
		       lis. Plus we want to merge empty lis sometimes. */
		    if( nstyle->displayMode() == QStyleSheetItem::DisplayListItem ) {
			canMergeLi = TRUE;
		    } else if ( nstyle->displayMode() == QStyleSheetItem::DisplayBlock ) {
			while ( curtag.style->name() == "p" ) {
			    if ( tags.isEmpty() )
				break;
			    curtag = tags.pop();
			}

	   		if ( curtag.style->displayMode() == QStyleSheetItem::DisplayListItem ) {
			    // we are in a li and a new block comes along
			    if ( nstyle->name() == "ul" || nstyle->name() == "ol" )
				hasNewPar = FALSE; // we want an empty li (like most browsers)
			    if ( !hasNewPar ) {
				/* do not add new blocks inside
				   non-empty lis */
				while ( curtag.style->displayMode() == QStyleSheetItem::DisplayListItem ) {
				    if ( tags.isEmpty() )
					break;
				    curtag = tags.pop();
				}
			    } else if ( canMergeLi ) {
				/* we have an empty li and a block
				   comes along, merge them */
				nstyle = curtag.style;
			    }
			    canMergeLi = FALSE;
			}
		    }
		}

		QTextCustomItem* custom =  0;

		// some well-known tags, some have a nstyle, some not
		if ( wellKnownTags.find( tagname ) != -1 ) {
		    if ( tagname == "br" ) {
			emptyTag = space = TRUE;
			int index = QMAX( curpar->length(),1) - 1;
			QTextFormat format = curtag.format.makeTextFormat( nstyle, attr, scaleFontsFactor );
			curpar->append( QChar_linesep );
			curpar->setFormat( index, 1, &format );
		    }  else if ( tagname == "hr" ) {
			emptyTag = space = TRUE;
			custom = sheet_->tag( tagname, attr, contxt, *factory_ , emptyTag, this );
			NEWPAR;
		    } else if ( tagname == "table" ) {
			emptyTag = space = TRUE;
			QTextFormat format = curtag.format.makeTextFormat(  nstyle, attr, scaleFontsFactor );
			curpar->setAlignment( curtag.alignment );
			custom = parseTable( attr, format, doc, length, pos, curpar );
		    } else if ( tagname == "qt" || tagname == "body" ) {
			if ( attr.contains( "bgcolor" ) ) {
			    QBrush *b = new QBrush( QColor( attr["bgcolor"] ) );
			    setPaper( b );
			}
			if ( attr.contains( "background" ) ) {
			    QImage img;
			    QString bg = attr["background"];
			    const QMimeSource* m = factory_->data( bg, contxt );
			    if ( !m ) {
				qWarning("QRichText: no mimesource for %s", bg.latin1() );
			    } else {
				if ( !QImageDrag::decode( m, img ) ) {
				    qWarning("QTextImage: cannot decode %s", bg.latin1() );
				}
			    }
			    if ( !img.isNull() ) {
				QPixmap pm;
				pm.convertFromImage( img );
				QBrush *b = new QBrush( QColor(), pm );
				setPaper( b );
			    }
			}
			if ( attr.contains( "text" ) ) {
			    QColor c( attr["text"] );
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
			}
			if ( attr.contains( "link" ) )
			    linkColor = QColor( attr["link"] );
			if ( attr.contains( "title" ) )
			    attribs.replace( "title", attr["title"] );

			if ( textEditMode ) {
			    if ( attr.contains("style" ) ) {
				QString a = attr["style"];
				for ( int s = 0; s < a.contains(';')+1; s++ ) {
				    QString style = QTextDocument::section( a, ";", s, s );
				    if ( style.startsWith("font-size:" ) && QTextDocument::endsWith(style, "pt") ) {
					scaleFontsFactor = double( formatCollection()->defaultFormat()->fn.pointSize() ) /
							   style.mid( 10, style.length() - 12 ).toInt();
				    }
				}
			    }
			    nstyle = 0; // ignore body in textEditMode
			}
			// end qt- and body-tag handling
		    } else if ( tagname == "meta" ) {
			if ( attr["name"] == "qrichtext" && attr["content"] == "1" )
			    textEditMode = TRUE;
		    } else if ( tagname == "title" ) {
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
		} // end of well-known tag handling

		if ( !custom ) // try generic custom item
		    custom = sheet_->tag( tagname, attr, contxt, *factory_ , emptyTag, this );

		if ( !nstyle && !custom ) // we have no clue what this tag could be, ignore it
		    continue;

		if ( custom ) {
		    int index = QMAX( curpar->length(),1) - 1;
		    QTextFormat format = curtag.format.makeTextFormat( nstyle, attr, scaleFontsFactor );
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
		    /* if we do nesting, push curtag on the stack,
		       otherwise reinint curag. */
 		    if ( curtag.style->name() != tagname || nstyle->selfNesting() ) {
			tags.push( curtag );
		    } else {
			if ( !tags.isEmpty() )
			    curtag = tags.top();
			else
			    curtag = initag;
		    }

		    curtag.name = tagname;
		    curtag.style = nstyle;
		    curtag.name = tagname;
		    curtag.style = nstyle;
		    if ( int(nstyle->whiteSpaceMode())  != QStyleSheetItem::Undefined )
			curtag.wsm = nstyle->whiteSpaceMode();

		    /* ignore whitespace for inline elements if there
		       was already one*/
		    if ( !textEditMode && curtag.wsm == QStyleSheetItem::WhiteSpaceNormal
			 && ( space || nstyle->displayMode() != QStyleSheetItem::DisplayInline ) )
			eatSpace( doc, length, pos );

		    curtag.format = curtag.format.makeTextFormat( nstyle, attr, scaleFontsFactor );
		    if ( nstyle->isAnchor() ) {
			if ( !anchorName.isEmpty() )
			    anchorName += "#" + attr["name"];
			else
			    anchorName = attr["name"];
			curtag.anchorHref = attr["href"];
		    }

		    if ( nstyle->alignment() != QStyleSheetItem::Undefined )
			curtag.alignment = nstyle->alignment();

		    if ( (int) nstyle->listStyle() != QStyleSheetItem::Undefined )
			curtag.liststyle = nstyle->listStyle();

		    if ( nstyle->displayMode() == QStyleSheetItem::DisplayBlock
			 || nstyle->displayMode() == QStyleSheetItem::DisplayListItem ) {

			if ( nstyle->name() == "ol" || nstyle->name() == "ul" || nstyle->name() == "li") {
			    QString type = attr["type"];
			    if ( !type.isEmpty() ) {
				if ( type == "1" ) {
				    curtag.liststyle = QStyleSheetItem::ListDecimal;
				} else if ( type == "a" ) {
				    curtag.liststyle = QStyleSheetItem::ListLowerAlpha;
				} else if ( type == "A" ) {
				    curtag.liststyle = QStyleSheetItem::ListUpperAlpha;
				} else {
				    type = type.lower();
				    if ( type == "square" )
					curtag.liststyle = QStyleSheetItem::ListSquare;
				    else if ( type == "disc" )
					curtag.liststyle = QStyleSheetItem::ListDisc;
				    else if ( type == "circle" )
					curtag.liststyle = QStyleSheetItem::ListCircle;
				}
			    }
			}


			/* Internally we treat ordered and bullet
			  lists the same for margin calculations. In
			  order to have fast pointer compares in the
			  xMargin() functions we restrict ourselves to
			  <ol>. Once we calculate the margins in the
			  parser rathern than later, the unelegance of
			  this approach goes awy
			 */
			if ( nstyle->name() == "ul" )
			    curtag.style = sheet_->item( "ol" );

			if ( attr.contains( "align" ) ) {
			    QString align = attr["align"];
			    if ( align == "center" )
				curtag.alignment = Qt::AlignCenter;
			    else if ( align == "right" )
				curtag.alignment = Qt::AlignRight;
			    else if ( align == "justify" )
				curtag.alignment = Qt3::AlignJustify;
			}
			if ( attr.contains( "dir" ) ) {
			    QString dir = attr["dir"];
			    if ( dir == "rtl" )
				curtag.direction = QChar::DirR;
			    else if ( dir == "ltr" )
				curtag.direction = QChar::DirL;
			}

			NEWPAR;

			if ( curtag.style->displayMode() == QStyleSheetItem::DisplayListItem ) {
			    if ( attr.contains( "value " ) )
				curpar->setListValue( attr["value"].toInt() );
			}

			if ( attr.contains( "style" ) ) {
			    QString a = attr["style"];
			    bool ok = TRUE;
			    for ( int s = 0; ok && s < a.contains(';')+1; s++ ) {
				QString style = QTextDocument::section( a, ";", s, s );
				if ( style.startsWith("margin-top:" ) && QTextDocument::endsWith(style, "px") )
				    curpar->utm = 1+style.mid(11, style.length() - 13).toInt(&ok);
				else if ( style.startsWith("margin-bottom:" ) && QTextDocument::endsWith(style, "px") )
				    curpar->ubm = 1+style.mid(14, style.length() - 16).toInt(&ok);
				else if ( style.startsWith("margin-left:" ) && QTextDocument::endsWith(style, "px") )
				    curpar->ulm = 1+style.mid(12, style.length() - 14).toInt(&ok);
				else if ( style.startsWith("margin-right:" ) && QTextDocument::endsWith(style, "px") )
				    curpar->urm = 1+style.mid(13, style.length() - 15).toInt(&ok);
				else if ( style.startsWith("text-indent:" ) && QTextDocument::endsWith(style, "px") )
				    curpar->uflm = 1+style.mid(12, style.length() - 14).toInt(&ok);
			    }
			    if ( !ok ) // be pressmistic
				curpar->utm = curpar->ubm = curpar->urm = curpar->ulm = 0;
			}
		    }
		}
	    } else {
		QString tagname = parseCloseTag( doc, length, pos );
		if ( tagname.isEmpty() )
		    continue; // nothing we could do with this, probably parse error
		if ( !sheet_->item( tagname ) ) // ignore unknown tags
		    continue;

		// we close a block item. Since the text may continue, we need to have a new paragraph
		bool needNewPar = curtag.style->displayMode() == QStyleSheetItem::DisplayBlock
				 || curtag.style->displayMode() == QStyleSheetItem::DisplayListItem;


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
		    if ( textEditMode && tagname == "p" ) // preserve empty paragraphs
			hasNewPar = FALSE;
		    NEWPAR;
		}
	    }
	} else {
	    // normal contents
	    QString s;
	    QChar c;
	    while ( pos < length && !hasPrefix(doc, length, pos, QChar('<') ) ){
		if ( textEditMode ) {
		    // text edit mode: we handle all white space but ignore newlines
		    c = parseChar( doc, length, pos, QStyleSheetItem::WhiteSpacePre );
		    if ( c == QChar_linesep )
			break;
		} else {
		    int l = pos;
		    c = parseChar( doc, length, pos, curtag.wsm );

		    // in white space pre mode: treat any space as non  breakable
		    if ( c == ' ' && curtag.wsm == QStyleSheetItem::WhiteSpacePre )
			c = QChar::nbsp;

		    if ( c == ' ' || c == QChar_linesep ) {
			/* avoid overlong paragraphs by forcing a new
			       paragraph after 4096 characters. This case can
			       occur when loading undiscovered plain text
			       documents in rich text mode. Instead of hanging
			       forever, we do the trick.
			    */
			if ( curtag.wsm == QStyleSheetItem::WhiteSpaceNormal && s.length() > 4096 ) do {
			    if ( doc[l] == '\n' ) {
				hasNewPar = FALSE; // for a new paragraph ...
				NEWPAR;
				hasNewPar = FALSE; // ... and make it non-reusable
				c = '\n';  // make sure we break below
				break;
			    }
			} while ( ++l < pos );
		    }
		}

		if ( c == '\n' )
		    break;  // break on  newlines, pre delievers a QChar_linesep

		bool c_isSpace = c.isSpace() && c.unicode() != 0x00a0U && !textEditMode;

		if ( curtag.wsm == QStyleSheetItem::WhiteSpaceNormal && c_isSpace && space )
		    continue;
		if ( c == '\r' )
		    continue;
		space = c_isSpace;
		s += c;
	    }
	    if ( !s.isEmpty() && curtag.style->displayMode() != QStyleSheetItem::DisplayNone ) {
		hasNewPar = FALSE;
		int index = QMAX( curpar->length(),1) - 1;
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
	}
    }
    if ( hasNewPar && curpar != fParag && !cursor ) {
	// cleanup unused last paragraphs
	curpar = curpar->p;
	delete curpar->n;
    }
    if ( !anchorName.isEmpty()  ) {
	curpar->at(curpar->length() - 1)->setAnchor( anchorName, curpar->at( curpar->length() - 1 )->anchorHref() );
	anchorName = QString::null;
    }


    setRichTextMarginsInternal( styles, stylesPar );

    if ( cursor ) {
 	cursor->gotoPreviousLetter();
  	cursor->remove();
     }

}

void QTextDocument::setRichTextMarginsInternal( QPtrList< QPtrVector<QStyleSheetItem> >& styles, QTextParagraph* stylesPar )
{
    // margin and line spacing calculation
    QPtrVector<QStyleSheetItem>* prevStyle = 0;
    QPtrVector<QStyleSheetItem>* curStyle = styles.first();
    QPtrVector<QStyleSheetItem>* nextStyle = styles.next();
    while ( stylesPar ) {
	if ( !curStyle ) {
	    stylesPar = stylesPar->next();
	    prevStyle = curStyle;
	    curStyle = nextStyle;
	    nextStyle = styles.next();
	    continue;
	}

	int i, mar;
	QStyleSheetItem* mainStyle = curStyle->size() ? (*curStyle)[curStyle->size()-1] : 0;
	if ( mainStyle && mainStyle->displayMode() == QStyleSheetItem::DisplayListItem )
	    stylesPar->setListItem( TRUE );
	int numLists = 0;
	for ( i = 0; i < (int)curStyle->size(); ++i ) {
	    if ( (*curStyle)[ i ]->displayMode() == QStyleSheetItem::DisplayBlock
		 && int((*curStyle)[ i ]->listStyle()) != QStyleSheetItem::Undefined )
		numLists++;
	}
	stylesPar->ldepth = numLists;
	if ( stylesPar->next() && nextStyle ) {
	    // also set the depth of the next paragraph, required for the margin calculation
	    numLists = 0;
	    for ( i = 0; i < (int)nextStyle->size(); ++i ) {
		if ( (*nextStyle)[ i ]->displayMode() == QStyleSheetItem::DisplayBlock
		     && int((*nextStyle)[ i ]->listStyle()) != QStyleSheetItem::Undefined )
		    numLists++;
	    }
	    stylesPar->next()->ldepth = numLists;
	}

	// do the top margin
	QStyleSheetItem* item = mainStyle;
	int m;
	if (stylesPar->utm > 0 ) {
	    m = stylesPar->utm-1;
	    stylesPar->utm = 0;
	} else {
	    m = QMAX(0, item->margin( QStyleSheetItem::MarginTop ) );
	    if ( item->displayMode() == QStyleSheetItem::DisplayListItem
		 && stylesPar->ldepth )
	      m /= stylesPar->ldepth;
	}
	for ( i = (int)curStyle->size() - 2 ; i >= 0; --i ) {
	    item = (*curStyle)[ i ];
	    if ( prevStyle && i < (int) prevStyle->size() &&
		 (  item->displayMode() == QStyleSheetItem::DisplayBlock &&
		    (*prevStyle)[ i ] == item ) )
		break;
	    // emulate CSS2' standard 0 vertical margin for multiple ul or ol tags
 	    if ( int(item->listStyle()) != QStyleSheetItem::Undefined  &&
		 ( (  i> 0 && (*curStyle)[ i-1 ] == item ) || (*curStyle)[i+1] == item ) )
		continue;
	    mar = QMAX( 0, item->margin( QStyleSheetItem::MarginTop ) );
	    m = QMAX( m, mar );
	}
	stylesPar->utm = m - stylesPar->topMargin();

	// do the bottom margin
	item = mainStyle;
	if (stylesPar->ubm > 0 ) {
	    m = stylesPar->ubm-1;
	    stylesPar->ubm = 0;
	} else {
	    m = QMAX(0, item->margin( QStyleSheetItem::MarginBottom ) );
	    if ( item->displayMode() == QStyleSheetItem::DisplayListItem
		 && stylesPar->ldepth )
	      m /= stylesPar->ldepth;
	}
	for ( i = (int)curStyle->size() - 2 ; i >= 0; --i ) {
	    item = (*curStyle)[ i ];
	    if ( nextStyle && i < (int) nextStyle->size() &&
		 (  item->displayMode() == QStyleSheetItem::DisplayBlock &&
		    (*nextStyle)[ i ] == item ) )
		break;
	    // emulate CSS2' standard 0 vertical margin for multiple ul or ol tags
 	    if ( int(item->listStyle()) != QStyleSheetItem::Undefined  &&
		 ( (  i> 0 && (*curStyle)[ i-1 ] == item ) || (*curStyle)[i+1] == item ) )
		continue;
	    mar = QMAX(0, item->margin( QStyleSheetItem::MarginBottom ) );
	    m = QMAX( m, mar );
	}
	stylesPar->ubm = m - stylesPar->bottomMargin();

	// do the left margin, simplyfied
	item = mainStyle;
	if (stylesPar->ulm > 0 ) {
	    m = stylesPar->ulm-1;
	    stylesPar->ulm = 0;
	} else {
	    m = QMAX( 0, item->margin( QStyleSheetItem::MarginLeft ) );
	}
	for ( i = (int)curStyle->size() - 2 ; i >= 0; --i ) {
	    item = (*curStyle)[ i ];
	    m += QMAX( 0, item->margin( QStyleSheetItem::MarginLeft ) );
	}
	stylesPar->ulm = m - stylesPar->leftMargin();

	// do the right margin, simplyfied
	item = mainStyle;
	if (stylesPar->urm > 0 ) {
	    m = stylesPar->urm-1;
	    stylesPar->urm = 0;
	} else {
	    m = QMAX( 0, item->margin( QStyleSheetItem::MarginRight ) );
	}
	for ( i = (int)curStyle->size() - 2 ; i >= 0; --i ) {
	    item = (*curStyle)[ i ];
	    m += QMAX( 0, item->margin( QStyleSheetItem::MarginRight ) );
	}
	stylesPar->urm = m - stylesPar->rightMargin();

	// do the first line margin, which really should be called text-indent
	item = mainStyle;
	if (stylesPar->uflm > 0 ) {
	    m = stylesPar->uflm-1;
	    stylesPar->uflm = 0;
	} else {
	    m = QMAX( 0, item->margin( QStyleSheetItem::MarginFirstLine ) );
	}
	for ( i = (int)curStyle->size() - 2 ; i >= 0; --i ) {
	    item = (*curStyle)[ i ];
	    mar = QMAX( 0, item->margin( QStyleSheetItem::MarginFirstLine ) );
	    m = QMAX( m, mar );
	}
	stylesPar->uflm =m - stylesPar->firstLineMargin();

	// do the bogus line "spacing", which really is just an extra margin
	item = mainStyle;
	for ( i = (int)curStyle->size() - 1 ; i >= 0; --i ) {
	    item = (*curStyle)[ i ];
	    if ( item->lineSpacing() != QStyleSheetItem::Undefined ) {
		stylesPar->ulinespacing = item->lineSpacing();
		if ( formatCollection() &&
		     stylesPar->ulinespacing < formatCollection()->defaultFormat()->height() )
		    stylesPar->ulinespacing += formatCollection()->defaultFormat()->height();
		break;
	    }
	}

	stylesPar = stylesPar->next();
	prevStyle = curStyle;
	curStyle = nextStyle;
	nextStyle = styles.next();
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

QString QTextDocument::plainText() const
{
    QString buffer;
    QString s;
    QTextParagraph *p = fParag;
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
}

static QString align_to_string( int a )
{
    if ( a & Qt::AlignRight )
	return " align=\"right\"";
    if ( a & Qt::AlignHCenter )
	return " align=\"center\"";
    if ( a & Qt3::AlignJustify )
	return " align=\"justify\"";
    return QString::null;
}

static QString direction_to_string( int d )
{
    if ( d != QChar::DirON )
	return ( d == QChar::DirL? " dir=\"ltr\"" : " dir=\"rtl\"" );
    return QString::null;
}

static QString list_value_to_string( int v )
{
    if ( v != -1 )
	return " listvalue=\"" + QString::number( v ) + "\"";
    return QString::null;
}

static QString list_style_to_string( int v )
{
    switch( v ) {
    case QStyleSheetItem::ListDecimal: return "\"1\"";
    case QStyleSheetItem::ListLowerAlpha: return "\"a\"";
    case QStyleSheetItem::ListUpperAlpha: return "\"A\"";
    case QStyleSheetItem::ListDisc: return "\"disc\"";
    case QStyleSheetItem::ListSquare: return "\"square\"";
    case QStyleSheetItem::ListCircle: return "\"circle\"";
    default:
	return QString::null;
    }
}

static inline bool list_is_ordered( int v )
{
    return v == QStyleSheetItem::ListDecimal ||
	   v == QStyleSheetItem::ListLowerAlpha ||
	   v == QStyleSheetItem::ListUpperAlpha;
}


static QString margin_to_string( QStyleSheetItem* style, int t, int b, int l, int r, int fl )
{
    QString s;
    if ( l > 0 )
	s += QString(!!s?";":"") + "margin-left:" + QString::number(l+QMAX(0,style->margin(QStyleSheetItem::MarginLeft))) + "px";
    if ( r > 0 )
	s += QString(!!s?";":"") + "margin-right:" + QString::number(r+QMAX(0,style->margin(QStyleSheetItem::MarginRight))) + "px";
    if ( t > 0 )
	s += QString(!!s?";":"") + "margin-top:" + QString::number(t+QMAX(0,style->margin(QStyleSheetItem::MarginTop))) + "px";
    if ( b > 0 )
	s += QString(!!s?";":"") + "margin-bottom:" + QString::number(b+QMAX(0,style->margin(QStyleSheetItem::MarginBottom))) + "px";
    if ( fl > 0 )
	s += QString(!!s?";":"") + "text-indent:" + QString::number(fl+QMAX(0,style->margin(QStyleSheetItem::MarginFirstLine))) + "px";
    if ( !!s )
	return " style=\"" + s + "\"";
    return QString::null;
}

QString QTextDocument::richText() const
{
    QString s = "";
    if ( !par ) {
	s += "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\"font-size:" ;
	s += QString::number( formatCollection()->defaultFormat()->font().pointSize() );
	s += "pt;font-family:";
	s += formatCollection()->defaultFormat()->font().family();
	s +="\">";
    }
    QTextParagraph* p = fParag;

    QStyleSheetItem* item_p = styleSheet()->item("p");
    QStyleSheetItem* item_ul = styleSheet()->item("ul");
    QStyleSheetItem* item_ol = styleSheet()->item("ol");
    QStyleSheetItem* item_li = styleSheet()->item("li");
    if ( !item_p || !item_ul || !item_ol || !item_li ) {
	qWarning( "QTextEdit: cannot export HTML due to insufficient stylesheet (lack of p, ul, ol, or li)" );
	return QString::null;
    }
    int pastListDepth = 0;
    int listDepth = 0;
    int futureListDepth = 0;
    QMemArray<int> listStyles(10);

    while ( p ) {
	listDepth = p->listDepth();
	if ( listDepth < pastListDepth )  {
	    for ( int i = listDepth+1; i <= pastListDepth; i++ )
		s += list_is_ordered( listStyles[i] ) ? "</ol>" : "</ul>";
	    s += '\n';
	} else if ( listDepth > pastListDepth ) {
	    s += '\n';
	    listStyles.resize( QMAX( (int)listStyles.size(), listDepth+1 ) );
	    QString list_type;
	    listStyles[listDepth] = p->listStyle();
	    if ( !list_is_ordered( p->listStyle() ) || item_ol->listStyle() != p->listStyle() )
		list_type = " type=" + list_style_to_string( p->listStyle() );
	    for ( int i = pastListDepth; i < listDepth; i++ ) {
		s += list_is_ordered( p->listStyle() ) ? "<ol" : "<ul" ;
		s += list_type + ">";
	    }
	} else {
	    s += '\n';
	}

	QString ps = p->richText();

	  // for the bottom margin we need to know whether we are at the end of a list
	futureListDepth = 0;
	if ( listDepth > 0 && p->next() )
	    futureListDepth = p->next()->listDepth();

	if ( richTextExportStart && richTextExportStart->paragraph() ==p &&
	     richTextExportStart->index() == 0 )
	    s += "<selstart/>";

	if ( p->isListItem() ) {
	    s += "<li";
	    if ( p->listStyle() != listStyles[listDepth] )
		s += " type=" + list_style_to_string( p->listStyle() );
	    s +=align_to_string( p->alignment() );
	    s += margin_to_string( item_li, p->utm, p->ubm, p->ulm, p->urm, p->uflm );
	    s +=  list_value_to_string( p->listValue() );
	    s += direction_to_string( p->direction() );
	    s +=">";
	    s += ps;
	    s += "</li>";
	} else {
	    // normal paragraph item
	    s += "<p";
	    s += align_to_string( p->alignment() );
	    s += margin_to_string( item_p, p->utm, p->ubm, p->ulm, p->urm, p->uflm );
	    s +=direction_to_string( p->direction() );
	    s += ">";
	    s += ps;
	    s += "</p>";
	}
	pastListDepth = listDepth;
	p = p->next();
    }
    while ( listDepth > 0 ) {
	s += list_is_ordered( listStyles[listDepth] ) ? "</ol>" : "</ul>";
	listDepth--;
    }

    if ( !par )
	s += "\n</body></html>\n";

    return s;
}

QString QTextDocument::text() const
{
    if ( txtFormat == Qt::AutoText && preferRichText || txtFormat == Qt::RichText )
	return richText();
    return plainText();
}

QString QTextDocument::text( int parag ) const
{
    QTextParagraph *p = paragAt( parag );
    if ( !p )
	return QString::null;

    if ( txtFormat == Qt::AutoText && preferRichText || txtFormat == Qt::RichText )
	return p->richText();
    else
	return p->string()->toString();
}

void QTextDocument::invalidate()
{
    QTextParagraph *s = fParag;
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
    paragId = !sel.swapped ? sel.startCursor.paragraph()->paragId() : sel.endCursor.paragraph()->paragId();
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
    paragId = sel.swapped ? sel.startCursor.paragraph()->paragId() : sel.endCursor.paragraph()->paragId();
    index = sel.swapped ? sel.startCursor.index() : sel.endCursor.index();
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

    c1.paragraph()->removeSelection( id );
    c2.paragraph()->removeSelection( id );
    if ( c1.paragraph() != c2.paragraph() ) {
	c1.paragraph()->setSelection( id, c1.index(), c1.paragraph()->length() - 1 );
	c2.paragraph()->setSelection( id, 0, c2.index() );
    } else {
	c1.paragraph()->setSelection( id, QMIN( c1.index(), c2.index() ), QMAX( c1.index(), c2.index() ) );
    }

    sel.startCursor = start;
    sel.endCursor = end;
    if ( sel.startCursor.paragraph() == sel.endCursor.paragraph() )
	sel.swapped = sel.startCursor.index() > sel.endCursor.index();
}

bool QTextDocument::setSelectionEnd( int id, const QTextCursor &cursor )
{
    QMap<int, QTextDocumentSelection>::Iterator it = selections.find( id );
    if ( it == selections.end() )
	return FALSE;
    QTextDocumentSelection &sel = *it;

    QTextCursor start = sel.startCursor;
    QTextCursor end = cursor;

    if ( start == end ) {
	removeSelection( id );
	setSelectionStart( id, cursor );
	return TRUE;
    }

    if ( sel.endCursor.paragraph() == end.paragraph() ) {
	setSelectionEndHelper( id, sel, start, end );
	return TRUE;
    }

    bool inSelection = FALSE;
    QTextCursor c( this );
    QTextCursor tmp = sel.startCursor;
    if ( sel.swapped )
	tmp = sel.endCursor;
    tmp.restoreState();
    QTextCursor tmp2 = cursor;
    tmp2.restoreState();
    c.setParagraph( tmp.paragraph()->paragId() < tmp2.paragraph()->paragId() ? tmp.paragraph() : tmp2.paragraph() );
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
	if ( c.paragraph() == start.paragraph() )
	    hadStartParag = TRUE;
	if ( c.paragraph() == end.paragraph() )
	    hadEndParag = TRUE;
	if ( c == sel.startCursor )
	    hadOldStart = TRUE;
	if ( c == sel.endCursor )
	    hadOldEnd = TRUE;

	if ( !sel.swapped &&
	     ( hadEnd && !hadStart ||
	       hadEnd && hadStart && start.paragraph() == end.paragraph() && start.index() > end.index() ) )
	    sel.swapped = TRUE;

	if ( c == end && hadStartParag ||
	     c == start && hadEndParag ) {
	    QTextCursor tmp = c;
	    tmp.restoreState();
	    if ( tmp.paragraph() != c.paragraph() ) {
		int sstart = tmp.paragraph()->selectionStart( id );
		tmp.paragraph()->removeSelection( id );
		tmp.paragraph()->setSelection( id, sstart, tmp.index() );
	    }
	}

	if ( inSelection &&
	     ( c == end && hadStart || c == start && hadEnd ) )
	     leftSelection = TRUE;
	else if ( !leftSelection && !inSelection && ( hadStart || hadEnd ) )
	    inSelection = TRUE;

	bool noSelectionAnymore = hadOldStart && hadOldEnd && leftSelection && !inSelection && !c.paragraph()->hasSelection( id ) && c.atParagEnd();
	c.paragraph()->removeSelection( id );
	if ( inSelection ) {
	    if ( c.paragraph() == start.paragraph() && start.paragraph() == end.paragraph() ) {
		c.paragraph()->setSelection( id, QMIN( start.index(), end.index() ), QMAX( start.index(), end.index() ) );
	    } else if ( c.paragraph() == start.paragraph() && !hadEndParag ) {
		c.paragraph()->setSelection( id, start.index(), c.paragraph()->length() - 1 );
	    } else if ( c.paragraph() == end.paragraph() && !hadStartParag ) {
		c.paragraph()->setSelection( id, end.index(), c.paragraph()->length() - 1 );
	    } else if ( c.paragraph() == end.paragraph() && hadEndParag ) {
		c.paragraph()->setSelection( id, 0, end.index() );
	    } else if ( c.paragraph() == start.paragraph() && hadStartParag ) {
		c.paragraph()->setSelection( id, 0, start.index() );
	    } else {
		c.paragraph()->setSelection( id, 0, c.paragraph()->length() - 1 );
	    }
	}

	if ( leftSelection )
	    inSelection = FALSE;

	if ( noSelectionAnymore )
	    break;
	// *ugle*hack optimization
	QTextParagraph *p = c.paragraph();
	if (  p->mightHaveCustomItems || p == start.paragraph() || p == end.paragraph() || p == lastParagraph() ) {
	    c.gotoNextLetter();
	    if ( p == lastParagraph() && c.atParagEnd() )
		break;
	} else {
	    if ( p->document()->parent() )
		do {
		    c.gotoNextLetter();
		} while ( c.paragraph() == p );
	    else
		c.setParagraph( p->next() );
	}
    }

    if ( !sel.swapped )
	sel.startCursor.paragraph()->setSelection( id, sel.startCursor.index(), sel.startCursor.paragraph()->length() - 1 );

    sel.startCursor = start;
    sel.endCursor = end;
    if ( sel.startCursor.paragraph() == sel.endCursor.paragraph() )
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

    c.setParagraph( fParag );
    c.setIndex( 0 );
    sel.startCursor = c;

    c.setParagraph( lParag );
    c.setIndex( lParag->length() - 1 );
    sel.endCursor = c;

    selections.insert( id, sel );

    QTextParagraph *p = fParag;
    while ( p ) {
	p->setSelection( id, 0, p->length() - 1 );
	p = p->next();
    }

    for ( QTextDocument *d = childList.first(); d; d = childList.next() )
	d->selectAll( id );
}

bool QTextDocument::removeSelection( int id )
{
    if ( !selections.contains( id ) )
	return FALSE;

    QTextDocumentSelection &sel = selections[ id ];

    QTextCursor start = sel.swapped ? sel.endCursor : sel.startCursor;
    QTextCursor end = sel.swapped ? sel.startCursor : sel.endCursor;
    QTextParagraph* p = 0;
    while ( start != end ) {
	if ( p != start.paragraph() ) {
	    p = start.paragraph();
	    p->removeSelection( id );
	}
	start.gotoNextLetter();
    }
    selections.remove( id );
    return TRUE;
}

QString QTextDocument::selectedText( int id, bool asRichText ) const
{
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

       ### Fix this properly when refactoring
     */
    while ( c2.nestedDepth() > c1.nestedDepth() )
	c2.oneUp();
    while ( c1.nestedDepth() > c2.nestedDepth() )
	c1.oneUp();
    while ( c1.nestedDepth() && c2.nestedDepth() &&
	    c1.paragraph()->document() != c2.paragraph()->document() ) {
	c1.oneUp();
	c2.oneUp();
    }
    // do not trust sel_swapped with tables. Fix this properly when refactoring as well
    if ( c1.paragraph()->paragId() > c2.paragraph()->paragId() ||
	 (c1.paragraph() == c2.paragraph() && c1.index() > c2.index() ) ) {
	QTextCursor tmp = c1;
	c2 = c1;
	c1 = tmp;
    }

    // end selection 3.0.3 improvement

    if ( asRichText && !parent() ) {
	richTextExportStart = &c1;
	richTextExportEnd = &c2;

	QString sel = richText();
	int from = sel.find( "<selstart/>" );
	int to = sel.findRev( "<selend/>" );
	if ( from >= 0 && from <= to )
	    sel = sel.mid( from, to - from );
	richTextExportStart = richTextExportEnd = 0;
	return sel;
    }

    QString s;
    if ( c1.paragraph() == c2.paragraph() ) {
	QTextParagraph *p = c1.paragraph();
	int end = c2.index();
	if ( p->at( QMAX( 0, end - 1 ) )->isCustom() )
	    ++end;
	if ( !p->mightHaveCustomItems ) {
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
    } else {
	QTextParagraph *p = c1.paragraph();
	int start = c1.index();
	while ( p ) {
	    int end = p == c2.paragraph() ? c2.index() : p->length() - 1;
	    if ( p == c2.paragraph() && p->at( QMAX( 0, end - 1 ) )->isCustom() )
		++end;
	    if ( !p->mightHaveCustomItems ) {
		s += p->string()->toString().mid( start, end - start );
		if ( p != c2.paragraph() )
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
	    if ( p == c2.paragraph() )
		break;
	    p = p->next();
	}
    }
    // ### workaround for plain text export until we get proper
    // mime types: turn unicode line seperators into the more
    // widely understood \n. Makes copy and pasting code snipplets
    // from within Assistent possible
    QChar* uc = (QChar*) s.unicode();
    for ( uint ii = 0; ii < s.length(); ii++ )
	if ( uc[(int)ii] == QChar_linesep )
	    uc[(int)ii] = QChar('\n');
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

    if ( c1.paragraph() == c2.paragraph() ) {
	c1.paragraph()->setFormat( c1.index(), c2.index() - c1.index(), f, TRUE, flags );
	return;
    }

    c1.paragraph()->setFormat( c1.index(), c1.paragraph()->length() - c1.index(), f, TRUE, flags );
    QTextParagraph *p = c1.paragraph()->next();
    while ( p && p != c2.paragraph() ) {
	p->setFormat( 0, p->length(), f, TRUE, flags );
	p = p->next();
    }
    c2.paragraph()->setFormat( 0, c2.index(), f, TRUE, flags );
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

    if ( c1.paragraph() == c2.paragraph() ) {
	c1.paragraph()->remove( c1.index(), c2.index() - c1.index() );
	return;
    }

    if ( c1.paragraph() == fParag && c1.index() == 0 &&
	 c2.paragraph() == lParag && c2.index() == lParag->length() - 1 )
	cursor->setValid( FALSE );

    bool didGoLeft = FALSE;
    if (  c1.index() == 0 && c1.paragraph() != fParag ) {
	cursor->gotoPreviousLetter();
	if ( cursor->isValid() )
	    didGoLeft = TRUE;
    }

    c1.paragraph()->remove( c1.index(), c1.paragraph()->length() - 1 - c1.index() );
    QTextParagraph *p = c1.paragraph()->next();
    int dy = 0;
    QTextParagraph *tmp;
    while ( p && p != c2.paragraph() ) {
	tmp = p->next();
	dy -= p->rect().height();
	delete p;
	p = tmp;
    }
    c2.paragraph()->remove( 0, c2.index() );
    while ( p ) {
	p->move( dy );
	p->invalidate( 0 );
	p->setEndState( -1 );
	p = p->next();
    }

    c1.paragraph()->join( c2.paragraph() );

    if ( didGoLeft )
	cursor->gotoNextLetter();
}

void QTextDocument::indentSelection( int id )
{
    QMap<int, QTextDocumentSelection>::Iterator it = selections.find( id );
    if ( it == selections.end() )
	return;

    QTextDocumentSelection sel = *it;
    QTextParagraph *startParag = sel.startCursor.paragraph();
    QTextParagraph *endParag = sel.endCursor.paragraph();
    if ( sel.endCursor.paragraph()->paragId() < sel.startCursor.paragraph()->paragId() ) {
	endParag = sel.startCursor.paragraph();
	startParag = sel.endCursor.paragraph();
    }

    QTextParagraph *p = startParag;
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

bool QTextDocument::find( QTextCursor& cursor, const QString &e, bool cs, bool wo, bool forward )
{
    removeSelection( Standard );
    QTextParagraph *p = 0;
    QString expr = e;
    // if we search for 'word only' than we have to be sure that
    // the expression contains no space or punct character at the
    // beginning or in the end. Otherwise we would run into a
    // endlessloop.
    if ( wo ) {
	for ( ;; ) {
	    if ( expr[ 0 ].isSpace() || expr[ 0 ].isPunct() )
		expr = expr.right( expr.length() - 1 );
	    else
		break;
	}
	for ( ;; ) {
	    if ( expr.at( expr.length() - 1 ).isSpace() || expr.at( expr.length() - 1 ).isPunct() )
		expr = expr.left( expr.length() - 1 );
	    else
		break;
	}
    }
    for (;;) {
	if ( p != cursor.paragraph() ) {
	    p = cursor.paragraph();
	    QString s = cursor.paragraph()->string()->toString();
	    int start = cursor.index();
	    for ( ;; ) {
		int res = forward ? s.find( expr, start, cs ) : s.findRev( expr, start, cs );
		int end = res + expr.length();
		if ( res == -1 || ( !forward && start < end ) )
		    break;
		if ( !wo || ( ( res == 0 || s[ res - 1 ].isSpace() || s[ res - 1 ].isPunct() ) &&
			      ( end == (int)s.length() || s[ end ].isSpace() || s[ end ].isPunct() ) ) ) {
		    removeSelection( Standard );
		    cursor.setIndex( forward ? end : res );
		    setSelectionStart( Standard, cursor );
		    cursor.setIndex( forward ? res : end );
		    setSelectionEnd( Standard, cursor );
		    return TRUE;
		}
		start = res +  (forward ? 1 : -1);
	    }
	}
	if ( forward ) {
	    if ( cursor.paragraph() == lastParagraph() && cursor.atParagEnd () )
		 break;
	    cursor.gotoNextLetter();
	} else {
	    if ( cursor.paragraph() == firstParagraph() && cursor.atParagStart() )
		 break;
	    cursor.gotoPreviousLetter();
	}
    }
    return FALSE;
}

void QTextDocument::setTextFormat( Qt::TextFormat f )
{
    txtFormat = f;
    if ( fParag == lParag && fParag->length() <= 1 )
	fParag->rtext = ( f == Qt::RichText );
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
    QTextParagraph *startParag = sel.startCursor.paragraph();
    QTextParagraph *endParag = sel.endCursor.paragraph();
    if ( sel.startCursor.paragraph() == sel.endCursor.paragraph() &&
	 sel.startCursor.paragraph()->selectionStart( selId ) == sel.endCursor.paragraph()->selectionEnd( selId ) )
	return FALSE;
    if ( sel.endCursor.paragraph()->paragId() < sel.startCursor.paragraph()->paragId() ) {
	endParag = sel.startCursor.paragraph();
	startParag = sel.endCursor.paragraph();
    }

    QTextParagraph *p = startParag;
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
    QTextParagraph *parag = fParag;
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
    if ( !buf_pixmap )
	buf_pixmap = new QPixmap( s.expandedTo( QSize(1,1) ) );
    else if ( buf_pixmap->size() != s )
	buf_pixmap->resize( s.expandedTo( buf_pixmap->size() ) );
    return buf_pixmap;
}

void QTextDocument::draw( QPainter *p, const QRect &rect, const QColorGroup &cg, const QBrush *paper )
{
    if ( !firstParagraph() )
	return;

    if ( paper ) {
	p->setBrushOrigin( 0, 0 );

	p->fillRect( rect, *paper );
    }

    if ( formatCollection()->defaultFormat()->color() != cg.text() )
	setDefaultFormat( formatCollection()->defaultFormat()->font(), cg.text() );

    QTextParagraph *parag = firstParagraph();
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

void QTextDocument::drawParagraph( QPainter *p, QTextParagraph *parag, int cx, int cy, int cw, int ch,
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

    if ( useDoubleBuffer || is_printer( painter ) )
	painter->fillRect( QRect( 0, 0, ir.width(), ir.height() ), parag->backgroundBrush( cg ) );
    else if ( cursor && cursor->paragraph() == parag )
	painter->fillRect( QRect( parag->at( cursor->index() )->x, 0, 2, ir.height() ),
			   parag->backgroundBrush( cg ) );

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

    if ( useDoubleBuffer ) {
	if ( parag->rect().x() + parag->rect().width() < parag->document()->x() + parag->document()->width() ) {
	    p->fillRect( parag->rect().x() + parag->rect().width(), parag->rect().y(),
			 ( parag->document()->x() + parag->document()->width() ) -
			 ( parag->rect().x() + parag->rect().width() ),
			 parag->rect().height(), cg.brush( QColorGroup::Base ) );
	}
    }

    parag->document()->nextDoubleBuffered = FALSE;
}

QTextParagraph *QTextDocument::draw( QPainter *p, int cx, int cy, int cw, int ch, const QColorGroup &cg,
				 bool onlyChanged, bool drawCursor, QTextCursor *cursor, bool resetChanged )
{
    if ( withoutDoubleBuffer || par && par->withoutDoubleBuffer ) {
	withoutDoubleBuffer = TRUE;
	QRect r;
	draw( p, r, cg );
	return 0;
    }
    withoutDoubleBuffer = FALSE;

    if ( !firstParagraph() )
	return 0;

    if ( cx < 0 && cy < 0 ) {
	cx = 0;
	cy = 0;
	cw = width();
	ch = height();
    }

    QTextParagraph *lastFormatted = 0;
    QTextParagraph *parag = firstParagraph();

    QPixmap *doubleBuffer = 0;
    QPainter painter;

    bool fullWidthSelection = FALSE;
    while ( parag ) {
	lastFormatted = parag;
	if ( !parag->isValid() )
	    parag->format();

	QRect pr = parag->rect();
	if ( fullWidthSelection )
	    pr.setWidth( parag->document()->width() );
	if ( pr.y() > cy + ch )
	    goto floating;
	if ( !pr.intersects( QRect( cx, cy, cw, ch ) ) || ( onlyChanged && !parag->hasChanged() ) ) {
	    parag = parag->next();
	    continue;
	}

	drawParagraph( p, parag, cx, cy, cw, ch, doubleBuffer, cg, drawCursor, cursor, resetChanged );
	parag = parag->next();
    }

    parag = lastParagraph();

 floating:
    if ( parag->rect().y() + parag->rect().height() < parag->document()->height() ) {
	if ( !parag->document()->parent() ) {
	    p->fillRect( 0, parag->rect().y() + parag->rect().height(), parag->document()->width(),
			 parag->document()->height() - ( parag->rect().y() + parag->rect().height() ),
			 cg.brush( QColorGroup::Base ) );
	}
 	if ( !flow()->isEmpty() ) {
 	    QRect cr( cx, cy, cw, ch );
 	    flow()->drawFloatingItems( p, cr.x(), cr.y(), cr.width(), cr.height(), cg, FALSE );
 	}
    }

    if ( buf_pixmap && buf_pixmap->height() > 300 ) {
	delete buf_pixmap;
	buf_pixmap = 0;
    }

    return lastFormatted;
}

/*
  #### this function only sets the default font size in the format collection
 */
void QTextDocument::setDefaultFormat( const QFont &font, const QColor &color )
{
    bool reformat = font != fCollection->defaultFormat()->font();
    for ( QTextDocument *d = childList.first(); d; d = childList.next() )
	d->setDefaultFormat( font, color );
    fCollection->updateDefaultFormat( font, color, sheet_ );

    if ( !reformat )
	return;
    tStopWidth = formatCollection()->defaultFormat()->width( 'x' ) * 8;

    // invalidate paragraphs and custom items
    QTextParagraph *p = fParag;
    while ( p ) {
	p->invalidate( 0 );
	for ( int i = 0; i < p->length() - 1; ++i )
	    if ( p->at( i )->isCustom() )
		p->at( i )->customItem()->invalidate();
	p = p->next();
    }
}

void QTextDocument::registerCustomItem( QTextCustomItem *i, QTextParagraph *p )
{
    if ( i && i->placement() != QTextCustomItem::PlaceInline ) {
	flow_->registerFloatingItem( i );
	p->registerFloatingItem( i );
	i->setParagraph( p );
    }
    p->mightHaveCustomItems = mightHaveCustomItems = TRUE;
}

void QTextDocument::unregisterCustomItem( QTextCustomItem *i, QTextParagraph *p )
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
	QTextParagraph *p = focusIndicator.parag;
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
	QTextParagraph *p = focusIndicator.parag;
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
    QTextParagraph *p = fParag;
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
    insert( index, s.unicode(), s.length(), f );
}

void QTextString::insert( int index, const QChar *unicode, int len, QTextFormat *f )
{
    int os = data.size();
    data.resize( data.size() + len );
    if ( index < os ) {
	memmove( data.data() + index + len, data.data() + index,
		 sizeof( QTextStringChar ) * ( os - index ) );
    }
    for ( int i = 0; i < len; ++i ) {
	data[ (int)index + i ].x = 0;
	data[ (int)index + i ].lineStart = 0;
	data[ (int)index + i ].d.format = 0;
	data[ (int)index + i ].type = QTextStringChar::Regular;
	data[ (int)index + i ].rightToLeft = 0;
	data[ (int)index + i ].startOfRun = 0;
	data[ (int)index + i ].c = unicode[i];
	data[ (int)index + i ].setFormat( f );
    }
    bidiDirty = TRUE;
}

QTextString::~QTextString()
{
    clear();
}

void QTextString::insert( int index, QTextStringChar *c, bool doAddRefFormat  )
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
    if ( doAddRefFormat && c->format() )
	c->format()->addRef();
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
    list_tm = list_bm = par_tm = par_bm = 12;
    list_lm = 40;
    li_tm = li_bm = 0;
    QStyleSheetItem* item = s->item( "ol" );
    if ( item ) {
	list_tm = QMAX(0,item->margin( QStyleSheetItem::MarginTop ));
	list_bm = QMAX(0,item->margin( QStyleSheetItem::MarginBottom ));
	list_lm = QMAX(0,item->margin( QStyleSheetItem::MarginLeft ));
    }
    if ( (item = s->item( "li" ) ) ) {
	li_tm = QMAX(0,item->margin( QStyleSheetItem::MarginTop ));
	li_bm = QMAX(0,item->margin( QStyleSheetItem::MarginBottom ));
    }
    if ( (item = s->item( "p" ) ) ) {
	par_tm = QMAX(0,item->margin( QStyleSheetItem::MarginTop ));
	par_bm = QMAX(0,item->margin( QStyleSheetItem::MarginBottom ));
    }
}

void QTextDocument::setUnderlineLinks( bool b ) {
    underlLinks = b;
    for ( QTextDocument *d = childList.first(); d; d = childList.next() )
	d->setUnderlineLinks( b );
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
     if ( c->c.unicode() == 0xad || c->c.unicode() == 0x2028 )
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

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

QTextParagraph::QTextParagraph( QTextDocument *d, QTextParagraph *pr, QTextParagraph *nx, bool updateIds )
    : invalid( 0 ), p( pr ), n( nx ), docOrPseudo( d ),
      changed(FALSE), firstFormat(TRUE), firstPProcess(TRUE), needPreProcess(FALSE), fullWidth(TRUE),
      lastInFrame(FALSE), visible(TRUE), breakable(TRUE), movedDown(FALSE),
      mightHaveCustomItems(FALSE), hasdoc( d != 0 ), litem(FALSE), rtext(FALSE),
      align( 0 ),mSelections( 0 ),
      mFloatingItems( 0 ), lstyle( QStyleSheetItem::ListDisc ),
      utm( 0 ), ubm( 0 ), ulm( 0 ), urm( 0 ), uflm( 0 ), ulinespacing( 0 ),
      tArray(0), tabStopWidth(0), eData( 0 ), ldepth( 0 )
{
    lstyle = QStyleSheetItem::ListDisc;
    if ( !hasdoc )
	docOrPseudo = new QTextParagraphPseudoDocument;
    bgcol = 0;
    list_val = -1;
    QTextFormat* defFormat = formatCollection()->defaultFormat();
    if ( !hasdoc ) {
	tabStopWidth = defFormat->width( 'x' ) * 8;
	pseudoDocument()->commandHistory = new QTextCommandHistory( 100 );
    }

    if ( p )
	p->n = this;
    if ( n )
	n->p = this;


    if ( !p && hasdoc )
	document()->setFirstParagraph( this );
    if ( !n && hasdoc )
	document()->setLastParagraph( this );

    state = -1;

    if ( p )
	id = p->id + 1;
    else
	id = 0;
    if ( n && updateIds ) {
	QTextParagraph *s = n;
	while ( s ) {
	    s->id = s->p->id + 1;
	    s->invalidateStyleCache();
	    s = s->n;
	}
    }

    str = new QTextString();
    str->insert( 0, " ", formatCollection()->defaultFormat() );
}

QTextParagraph::~QTextParagraph()
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
    QMap<int, QTextLineStart*>::Iterator it = lineStarts.begin();
    for ( ; it != lineStarts.end(); ++it )
	delete *it;
    if ( mSelections )
	delete mSelections;
    if ( mFloatingItems )
	delete mFloatingItems;
    if ( p )
	p->setNext( n );
    if ( n )
	n->setPrev( p );
}

void QTextParagraph::setNext( QTextParagraph *s )
{
    n = s;
    if ( !n && hasdoc )
	document()->setLastParagraph( this );
}

void QTextParagraph::setPrev( QTextParagraph *s )
{
    p = s;
    if ( !p && hasdoc )
	document()->setFirstParagraph( this );
}

void QTextParagraph::invalidate( int chr )
{
    if ( invalid < 0 )
	invalid = chr;
    else
	invalid = QMIN( invalid, chr );
    if ( mFloatingItems ) {
	for ( QTextCustomItem *i = mFloatingItems->first(); i; i = mFloatingItems->next() )
	    i->ypos = -1;
    }
    invalidateStyleCache();
}

void QTextParagraph::invalidateStyleCache()
{
    if ( list_val < 0 )
	list_val = -1;
}


void QTextParagraph::insert( int index, const QString &s )
{
    insert( index, s.unicode(), s.length() );
}

void QTextParagraph::insert( int index, const QChar *unicode, int len )
{
    if ( hasdoc && !document()->useFormatCollection() && document()->preProcessor() )
	str->insert( index, unicode, len,
		     document()->preProcessor()->format( QTextPreProcessor::Standard ) );
    else
	str->insert( index, unicode, len, formatCollection()->defaultFormat() );
    invalidate( index );
    needPreProcess = TRUE;
}

void QTextParagraph::truncate( int index )
{
    str->truncate( index );
    insert( length(), " " );
    needPreProcess = TRUE;
}

void QTextParagraph::remove( int index, int len )
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

void QTextParagraph::join( QTextParagraph *s )
{
    int oh = r.height() + s->r.height();
    n = s->n;
    if ( n )
	n->p = this;
    else if ( hasdoc )
	document()->setLastParagraph( this );

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
	    if ( hasdoc ) {
		document()->unregisterCustomItem( item, s );
		document()->registerCustomItem( item, this );
	    }
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
	QTextParagraph *s = n;
	s->invalidate( 0 );
	while ( s ) {
	    s->id = s->p->id + 1;
	    s->state = -1;
	    s->needPreProcess = TRUE;
	    s->changed = TRUE;
	    s->invalidateStyleCache();
	    s = s->n;
	}
    }
    format();
    state = -1;
}

void QTextParagraph::move( int &dy )
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

void QTextParagraph::format( int start, bool doMove )
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
    if ( p )
	p->lastInFrame = FALSE;

    movedDown = FALSE;
    bool formattedAgain = FALSE;

 formatAgain:

    r.setWidth( documentWidth() );
    if ( hasdoc && mFloatingItems ) {
	for ( QTextCustomItem *i = mFloatingItems->first(); i; i = mFloatingItems->next() ) {
	    i->ypos = r.y();
	    if ( i->placement() == QTextCustomItem::PlaceRight ) {
		i->xpos = r.x() + r.width() - i->width;
	    }
	}
    }
    QMap<int, QTextLineStart*> oldLineStarts = lineStarts;
    lineStarts.clear();
    int y = formatter()->format( document(), this, start, oldLineStarts );


    r.setWidth( QMAX( r.width(), formatter()->minimumWidth() ) );


    QMap<int, QTextLineStart*>::Iterator it = oldLineStarts.begin();

    for ( ; it != oldLineStarts.end(); ++it )
	delete *it;

    QTextStringChar *c = 0;
    // do not do this on mac, as the paragraph
    // with has to be the full document width on mac as the selections
    // always extend completely to the right. This is a bit unefficient,
    // as this results in a bigger double buffer than needed but ok for
    // now.
    if ( lineStarts.count() == 1 ) {
	if ( !string()->isBidi() ) {
	    c = &str->at( str->length() - 1 );
	    r.setWidth( c->x + str->width( str->length() - 1 ) );
	} else {
	    r.setWidth( lineStarts[0]->w );
	}
    }

    if ( !hasdoc ) { // qt_format_text bounding rect handling
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
	QTextParagraph *s = n;
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

int QTextParagraph::lineHeightOfChar( int i, int *bl, int *y ) const
{
    if ( !isValid() )
	( (QTextParagraph*)this )->format();

    QMap<int, QTextLineStart*>::ConstIterator it = lineStarts.end();
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

    qWarning( "QTextParagraph::lineHeightOfChar: couldn't find lh for %d", i );
    return 15;
}

QTextStringChar *QTextParagraph::lineStartOfChar( int i, int *index, int *line ) const
{
    if ( !isValid() )
	( (QTextParagraph*)this )->format();

    int l = (int)lineStarts.count() - 1;
    QMap<int, QTextLineStart*>::ConstIterator it = lineStarts.end();
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

    qWarning( "QTextParagraph::lineStartOfChar: couldn't find %d", i );
    return 0;
}

int QTextParagraph::lines() const
{
    if ( !isValid() )
	( (QTextParagraph*)this )->format();

    return (int)lineStarts.count();
}

QTextStringChar *QTextParagraph::lineStartOfLine( int line, int *index ) const
{
    if ( !isValid() )
	( (QTextParagraph*)this )->format();

    if ( line >= 0 && line < (int)lineStarts.count() ) {
	QMap<int, QTextLineStart*>::ConstIterator it = lineStarts.begin();
	while ( line-- > 0 )
	    ++it;
	int i = it.key();
	if ( index )
	    *index = i;
	return &str->at( i );
    }

    qWarning( "QTextParagraph::lineStartOfLine: couldn't find %d", line );
    return 0;
}

int QTextParagraph::leftGap() const
{
    if ( !isValid() )
	( (QTextParagraph*)this )->format();

    int line = 0;
    int x = str->at(0).x;  /* set x to x of first char */
    if ( str->isBidi() ) {
	for ( int i = 1; i < str->length()-1; ++i )
	    x = QMIN(x, str->at(i).x);
	return x;
    }

    QMap<int, QTextLineStart*>::ConstIterator it = lineStarts.begin();
    while (line < (int)lineStarts.count()) {
	int i = it.key(); /* char index */
	x = QMIN(x, str->at(i).x);
	++it;
	++line;
    }
    return x;
}

void QTextParagraph::setFormat( int index, int len, QTextFormat *f, bool useCollection, int flags )
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

void QTextParagraph::indent( int *oldIndent, int *newIndent )
{
    if ( !hasdoc || !document()->indent() || isListItem() ) {
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

void QTextParagraph::paint( QPainter &painter, const QColorGroup &cg, QTextCursor *cursor, bool drawSelections,
			int clipx, int clipy, int clipw, int cliph )
{
    if ( !visible )
	return;
    QTextStringChar *chr = 0;
    int i, y, h, baseLine, xstart, xend;
    i = y =h = baseLine = 0;
    QRect cursorRect;
    drawSelections &= ( mSelections != 0 );
    // macintosh full-width selection style
    bool fullWidthStyle = FALSE;
    int fullSelectionWidth = 0;
    if ( drawSelections && fullWidthStyle )
	fullSelectionWidth = (hasdoc ? document()->width() : r.width());
	
    QString qstr = str->toString();
    // ### workaround so that \n are not drawn, actually this should
    // be fixed in QFont somewhere (under Windows you get ugly boxes
    // otherwise)
    QChar* uc = (QChar*) qstr.unicode();
    for ( uint ii = 0; ii < qstr.length(); ii++ )
	if ( uc[(int)ii]== '\n' || uc[(int)ii] == QChar_linesep || uc[(int)ii] == '\t' )
	    uc[(int)ii] = 0x20;

    int line = -1;
    int paintStart = 0;
    int selection = -1;
    for ( i = 0; i < length(); i++ ) {
	chr = at( i );

	// we flush at end of document
	bool flush = i== length()-1;
	bool selectionStateChanged = FALSE;
	if ( !flush ) {
	    QTextStringChar *nextchr = at( i+1 );
	    // we flush at end of line
	    flush |= nextchr->lineStart;
	    // we flush on format changes
	    flush |= ( nextchr->format() != chr->format() );
	    // we flush on anchor changes
	    flush |= ( nextchr->isAnchor() != chr->isAnchor() );
	    // we flush on start of run
	    flush |= nextchr->startOfRun;
	    // we flush on bidi changes
	    flush |= ( nextchr->rightToLeft != chr->rightToLeft );
	    // we flush on tab
	    flush |= ( chr->c == '\t' );
	    // we flush on soft hypens
	    flush |= ( chr->c.unicode() == 0xad );
	    // we flush on custom items
	    flush |= chr->isCustom();
	    // we flush before custom items
	    flush |= nextchr->isCustom();
	    // when painting justified, we flush on spaces
	    if ((alignment() & Qt3::AlignJustify) == Qt3::AlignJustify )
		flush |= QTextFormatter::isBreakable( str, i );
	    // we flush when the string is getting too long
	    flush |= ( i - paintStart >= 256 );
	    // we flush when the selection state changes
	    if ( drawSelections ) {
		for ( QMap<int, QTextParagraphSelection>::ConstIterator it = mSelections->begin();
		      it != mSelections->end(); ++it )
		    selectionStateChanged |=( (*it).start == i || (*it).start == i+1 || (*it).end == i+1 );
		flush |= selectionStateChanged;
	    }
	}

	// init a new line
	if ( chr->lineStart ) {
	    if (fullWidthStyle && drawSelections && selection >= 0)
		painter.fillRect( xend, y, fullSelectionWidth - xend, h,
				  (selection == QTextDocument::Standard || !hasdoc) ?
				  cg.color( QColorGroup::Highlight ) :
				  document()->selectionColor( selection ) );
	    ++line;
	    paintStart = i;
	    lineInfo( line, y, h, baseLine );
	    if ( clipy != -1 && cliph != 0 && y + r.y() - h > clipy + cliph ) { // outside clip area, leave
		break;
	    }

	    // if this is the first line and we are a list item, draw the the bullet label
	    if ( line == 0 && isListItem() )
		drawLabel( &painter, chr->x, y, 0, 0, baseLine, cg );
	}

	// check for cursor mark
	if ( cursor && this == cursor->paragraph() && i == cursor->index() ) {
	    QTextStringChar *c = i == 0 ? chr : chr - 1;
	    cursorRect.setRect( cursor->x() , y + baseLine - c->format()->ascent(),
				1, c->format()->height() );
	}

	// check if we are in a selection and store which one it is
	selection = -1;
	if ( drawSelections ) {
	    for ( QMap<int, QTextParagraphSelection>::ConstIterator it = mSelections->begin();
		  it != mSelections->end(); ++it )
		if ( (*it).start <= i && i < (*it).end + ( (*it).end == length()-1 && n && n->hasSelection(it.key()) ) ? 1:0
		     // exclude the standard selection from printing
		     && (it.key() != QTextDocument::Standard || !is_printer( &painter) ) ) {
		    selection = it.key();
		    break;
		}
	}

	if ( flush ) {  // something changed, draw what we have so far
	    if ( chr->rightToLeft ) {
		xstart = chr->x;
		xend = at( paintStart )->x + str->width( paintStart );
	    } else {
		xstart = at( paintStart )->x;
		if ( !selectionStateChanged && i < length() - 1 && !str->at( i + 1 ).lineStart )
		    xend = str->at( i + 1 ).x;
		else
		    xend = chr->x + str->width( i );
	    }

	    if ( (clipx == -1 || clipw == -1) || (xend >= clipx && xstart <= clipx + clipw) ) {
		if ( !chr->isCustom() )
		    drawString( painter, qstr, paintStart, i - paintStart + 1, xstart, y,
				baseLine, xend-xstart, h, selection,
				chr, cg, chr->rightToLeft );
		else if ( chr->customItem()->placement() == QTextCustomItem::PlaceInline )
		    chr->customItem()->draw( &painter, chr->x, y,
					     clipx == -1 ? clipx : (clipx - r.x()),
					     clipy == -1 ? clipy : (clipy - r.y()),
					     clipw, cliph, cg, selection >= 0 );
	    }
	    paintStart = i+1;
	}
	
    }

    if (fullWidthStyle && drawSelections && selection >= 0 && next() && next()->mSelections)
	for ( QMap<int, QTextParagraphSelection>::ConstIterator it = next()->mSelections->begin();
	      it != next()->mSelections->end(); ++it )
	    if (((*it).start) == 0) {
		painter.fillRect( xend, y, fullSelectionWidth - xend, h,
				  (selection == QTextDocument::Standard || !hasdoc) ?
				  cg.color( QColorGroup::Highlight ) :
				  document()->selectionColor( selection ) );
		break;
	    }

    // time to draw the cursor
    const int cursor_extent = 4;
    if ( !cursorRect.isNull() && cursor &&
	 ((clipx == -1 || clipw == -1) || (cursorRect.right()+cursor_extent >= clipx && cursorRect.left()-cursor_extent <= clipx + clipw)) ) {
	painter.fillRect( cursorRect, cg.color( QColorGroup::Text ) );
	painter.save();
	if ( string()->isBidi() ) {
	    if ( at( cursor->index() )->rightToLeft ) {
		painter.setPen( Qt::black );
		painter.drawLine( cursorRect.x(), cursorRect.y(), cursorRect.x() - cursor_extent / 2, cursorRect.y() + cursor_extent / 2 );
		painter.drawLine( cursorRect.x(), cursorRect.y() + cursor_extent, cursorRect.x() - cursor_extent / 2, cursorRect.y() + cursor_extent / 2 );
	    } else {
		painter.setPen( Qt::black );
		painter.drawLine( cursorRect.x(), cursorRect.y(), cursorRect.x() + cursor_extent / 2, cursorRect.y() + cursor_extent / 2 );
		painter.drawLine( cursorRect.x(), cursorRect.y() + cursor_extent, cursorRect.x() + cursor_extent / 2, cursorRect.y() + cursor_extent / 2 );
	    }
	}
	painter.restore();
    }
}

//#define BIDI_DEBUG

void QTextParagraph::drawString( QPainter &painter, const QString &s, int start, int len, int xstart,
			     int y, int baseLine, int w, int h, int selection,
			     QTextStringChar *formatChar, const QColorGroup& cg,
			     bool rightToLeft )
{
    int i = start + len - 1;
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

    if ( selection >= 0 )  {
	if ( !hasdoc || document()->invertSelectionText( selection ) )
	    painter.setPen( cg.color( QColorGroup::HighlightedText ) );
	painter.fillRect( xstart, y, w, h,
			  (selection == QTextDocument::Standard || !hasdoc) ?
  			  cg.color( QColorGroup::Highlight ) : document()->selectionColor( selection ) );
    }

    if ( str[ start ] != '\t' && str[ start ].unicode() != 0xad ) {
	if ( format->vAlign() == QTextFormat::AlignNormal ) {
	    painter.drawText( xstart, y + baseLine, str.mid( start ), len );
#ifdef BIDI_DEBUG
	    painter.save();
	    painter.setPen ( Qt::red );
	    painter.drawLine( xstart, y, xstart, y + baseLine );
	    painter.drawLine( xstart, y + baseLine/2, xstart + 10, y + baseLine/2 );
	    int w = 0;
	    int i = 0;
	    while( i < len )
		w += painter.fontMetrics().charWidth( str, start + i++ );
	    painter.setPen ( Qt::blue );
	    painter.drawLine( xstart + w - 1, y, xstart + w - 1, y + baseLine );
	    painter.drawLine( xstart + w - 1, y + baseLine/2, xstart + w - 1 - 10, y + baseLine/2 );
	    painter.restore();
#endif
	} else if ( format->vAlign() == QTextFormat::AlignSuperScript ) {
	    QFont f( painter.font() );
	    if ( format->fontSizesInPixels() )
		f.setPixelSize( ( f.pixelSize() * 2 ) / 3 );
	    else
		f.setPointSize( ( f.pointSize() * 2 ) / 3 );
	    painter.setFont( f );
	    painter.drawText( xstart, y + baseLine - ( painter.fontMetrics().height() / 2 ),
	    		      str.mid( start ), len );
	} else if ( format->vAlign() == QTextFormat::AlignSubScript ) {
	    QFont f( painter.font() );
	    if ( format->fontSizesInPixels() )
		f.setPixelSize( ( f.pixelSize() * 2 ) / 3 );
	    else
		f.setPointSize( ( f.pointSize() * 2 ) / 3 );
	    painter.setFont( f );
	    painter.drawText( xstart, y + baseLine + painter.fontMetrics().height() / 6, str.mid( start ), len );
	}
    }
    if ( i + 1 < length() && at( i + 1 )->lineStart && at( i )->c.unicode() == 0xad ) {
	painter.drawText( xstart + w, y + baseLine, "\xad" );
    }
    if ( format->isMisspelled() ) {
	painter.save();
	painter.setPen( QPen( Qt::red, 1, Qt::DotLine ) );
	painter.drawLine( xstart, y + baseLine + 1, xstart + w, y + baseLine + 1 );
	painter.restore();
    }

    i -= len;

    if ( hasdoc && formatChar->isAnchor() && !formatChar->anchorHref().isEmpty() &&
	 document()->focusIndicator.parag == this &&
	 ( document()->focusIndicator.start >= i  &&
	   document()->focusIndicator.start + document()->focusIndicator.len <= i + len ||
	   document()->focusIndicator.start <= i &&
	   document()->focusIndicator.start + document()->focusIndicator.len >= i + len ) ) {
	painter.drawWinFocusRect( QRect( xstart, y, w, h ) );
    }

}

void QTextParagraph::drawLabel( QPainter* p, int x, int y, int w, int h, int base, const QColorGroup& cg )
{
    QRect r ( x, y, w, h );
    QStyleSheetItem::ListStyle s = listStyle();

    p->save();
    QTextFormat *format = at( 0 )->format();
    if ( format ) {
	p->setPen( format->color() );
	p->setFont( format->font() );
    }
    QFontMetrics fm( p->fontMetrics() );
    int size = fm.lineSpacing() / 3;

    switch ( s ) {
    case QStyleSheetItem::ListDecimal:
    case QStyleSheetItem::ListLowerAlpha:
    case QStyleSheetItem::ListUpperAlpha:
	{
	    if ( list_val == -1 ) { // uninitialised list value, calcluate the right one
		int depth = listDepth();
		list_val--;
		// ### evil, square and expensive. This needs to be done when formatting, not when painting
		QTextParagraph* s = prev();
		int depth_s;
		while ( s && (depth_s = s->listDepth()) >= depth ) {
		    if ( depth_s == depth && s->isListItem() )
			list_val--;
		    s = s->prev();
		}
	    }

	    int n = list_val;
	    if ( n < -1 )
		n = -n - 1;
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
	    p->fillRect( er , cg.brush( QColorGroup::Text ) );
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
	    p->setBrush( cg.brush( QColorGroup::Text ));
	    QRect er( r.right()-size*2, r.top() + fm.height() / 2 - size / 2, size, size);
	    p->drawEllipse( er );
	    p->setBrush( Qt::NoBrush );
	}
	break;
    }

    p->restore();
}

void QTextParagraph::readStyleInformation( QDataStream& stream )
{
    int int_align, int_lstyle;
    uchar uchar_litem, uchar_rtext, uchar_dir;
    stream >> int_align >> int_lstyle >> utm >> ubm >> ulm >> urm >> uflm
	   >> ulinespacing >> ldepth >> uchar_litem >> uchar_rtext >> uchar_dir;
    align = int_align; lstyle = (QStyleSheetItem::ListStyle) int_lstyle;
    litem = uchar_litem; rtext = uchar_rtext; str->setDirection( (QChar::Direction)uchar_dir );
    QTextParagraph* s = prev() ? prev() : this;
    while ( s ) {
	s->invalidate( 0 );
	s = s->next();
    }
}

void QTextParagraph::writeStyleInformation( QDataStream& stream ) const
{
    stream << (int) align << (int) lstyle << utm << ubm << ulm << urm << uflm << ulinespacing << ldepth << (uchar)litem << (uchar)rtext << (uchar)str->direction();
}



void QTextParagraph::setListDepth( int depth ) {
    if ( !hasdoc || depth == ldepth )
	return;
    ldepth = depth;
    QTextParagraph* s = prev() ? prev() : this;
    while ( s ) {
	s->invalidate( 0 );
	s = s->next();
    }
}

int *QTextParagraph::tabArray() const
{
    int *ta = tArray;
    if ( !ta && hasdoc )
	ta = document()->tabArray();
    return ta;
}

int QTextParagraph::nextTab( int, int x )
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

void QTextParagraph::adjustToPainter( QPainter *p )
{
    for ( int i = 0; i < length(); ++i ) {
	if ( at( i )->isCustom() )
	    at( i )->customItem()->adjustToPainter( p );
    }
}

QTextFormatCollection *QTextParagraph::formatCollection() const
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

QString QTextParagraph::richText() const
{
    QString s;
    QTextStringChar *formatChar = 0;
    QString spaces;
    bool doStart = richTextExportStart && richTextExportStart->paragraph() == this;
    bool doEnd = richTextExportEnd && richTextExportEnd->paragraph() == this;
    int i;
    for ( i = 0; i < length()-1; ++i ) {
	if ( doStart && i && richTextExportStart->index() == i )
	    s += "<selstart/>";
	if ( doEnd && richTextExportEnd->index() == i )
	    s += "<selend/>";
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
	    s += c->format()->makeFormatChangeTags( formatCollection()->defaultFormat(),
						    0, QString::null, c->anchorHref() );
	    formatChar = c;
	} else if ( ( formatChar->format()->key() != c->format()->key() ) ||
		  (c->anchorHref() != formatChar->anchorHref() ) )  {
	    s += c->format()->makeFormatChangeTags( formatCollection()->defaultFormat(),
						    formatChar->format() , formatChar->anchorHref(), c->anchorHref() );
	    formatChar = c;
	}
	if ( c->c == '<' )
	    s += "&lt;";
	else if ( c->c == '>' )
	    s += "&gt;";
	else if ( c->isCustom() )
	    s += c->customItem()->richText();
	else if ( c->c == '\n' || c->c == QChar_linesep )
	    s += "<br />"; // space on purpose for compatibility with Netscape, Lynx & Co.
	else
	    s += c->c;
    }
    if ( doEnd && richTextExportEnd->index() == i )
	s += "<selend/>";
    if ( formatChar )
	s += formatChar->format()->makeFormatEndTags( formatCollection()->defaultFormat(), formatChar->anchorHref() );
    return s;
}

void QTextParagraph::addCommand( QTextCommand *cmd )
{
    if ( !hasdoc )
	pseudoDocument()->commandHistory->addCommand( cmd );
    else
	document()->commands()->addCommand( cmd );
}

QTextCursor *QTextParagraph::undo( QTextCursor *c )
{
    if ( !hasdoc )
	return pseudoDocument()->commandHistory->undo( c );
    return document()->commands()->undo( c );
}

QTextCursor *QTextParagraph::redo( QTextCursor *c )
{
    if ( !hasdoc )
	return pseudoDocument()->commandHistory->redo( c );
    return document()->commands()->redo( c );
}

int QTextParagraph::topMargin() const
{
    int m = 0;
    if ( rtext ) {
	m = isListItem() ? (document()->li_tm/QMAX(1,listDepth())) : document()->par_tm;
	if ( listDepth() == 1 &&(  !prev() || prev()->listDepth() < listDepth() ) )
	    m = QMAX( m, document()->list_tm );
    }
    m += utm;
    return scale( m, QTextFormat::painter() );
}

int QTextParagraph::bottomMargin() const
{
    int m = 0;
    if ( rtext ) {
	m = isListItem() ? (document()->li_bm/QMAX(1,listDepth())) : document()->par_bm;
	if ( listDepth() == 1 &&(  !next() || next()->listDepth() < listDepth() ) )
	    m = QMAX( m, document()->list_bm );
    }
    m += ubm;
    return scale( m, QTextFormat::painter() );
}

int QTextParagraph::leftMargin() const
{
    int m = ulm;
    if ( listDepth() )
	m += listDepth() * document()->list_lm;
    return scale( m, QTextFormat::painter() );
}

int QTextParagraph::firstLineMargin() const
{
    int m = uflm;
    return scale( m, QTextFormat::painter() );
}

int QTextParagraph::rightMargin() const
{
    int m = urm;
    return scale( m, QTextFormat::painter() );
}

int QTextParagraph::lineSpacing() const
{
    int l = ulinespacing;
    l = scale( l, QTextFormat::painter() );
    return l;
}

void QTextParagraph::copyParagData( QTextParagraph *parag )
{
    rtext = parag->rtext;
    lstyle = parag->lstyle;
    ldepth = parag->ldepth;
    litem = parag->litem;
    align = parag->align;
    utm = parag->utm;
    ubm = parag->ubm;
    urm = parag->urm;
    ulm = parag->ulm;
    uflm = parag->uflm;
    ulinespacing = parag->ulinespacing;
    QColor *c = parag->backgroundColor();
    if ( c )
	setBackgroundColor( *c );
    str->setDirection( parag->str->direction() );
}

void QTextParagraph::show()
{
    if ( visible || !hasdoc )
	return;
    visible = TRUE;
}

void QTextParagraph::hide()
{
    if ( !visible || !hasdoc )
	return;
    visible = FALSE;
}

void QTextParagraph::setDirection( QChar::Direction d )
{
    if ( str && str->direction() != d ) {
	str->setDirection( d );
	invalidate( 0 );
    }
}

QChar::Direction QTextParagraph::direction() const
{
    return (str ? str->direction() : QChar::DirON );
}

void QTextParagraph::setChanged( bool b, bool recursive )
{
    changed = b;
    if ( recursive ) {
	if ( document() && document()->parentParagraph() )
	    document()->parentParagraph()->setChanged( b, recursive );
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

QTextLineStart *QTextFormatter::formatLine( QTextParagraph *parag, QTextString *string, QTextLineStart *line,
						   QTextStringChar *startChar, QTextStringChar *lastChar, int align, int space )
{
#ifndef QT_NO_COMPLEXTEXT
    if( string->isBidi() )
	return bidiReorderLine( parag, string, line, startChar, lastChar, align, space );
#endif
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
	// End at "last-1", the last space ends up with a width of 0
	for ( int j = last-1; j >= start; --j ) {
	    // Start at last tab, if any.
	    if ( string->at( j ).c == '\t' ) {
		start = j+1;
		break;
	    }
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

    return new QTextLineStart();
}

#ifndef QT_NO_COMPLEXTEXT

#ifdef BIDI_DEBUG
#include <iostream>
#endif

// collects one line of the paragraph and transforms it to visual order
QTextLineStart *QTextFormatter::bidiReorderLine( QTextParagraph * /*parag*/, QTextString *text, QTextLineStart *line,
							QTextStringChar *startChar, QTextStringChar *lastChar, int align, int space )
{
    int start = (startChar - &text->at(0));
    int last = (lastChar - &text->at(0) );

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
	// End at "last-1", the last space ends up with a width of 0
	for ( int j = last-1; j >= start; --j ) {
	    // Start at last tab, if any.
	    if ( text->at( j ).c == '\t' ) {
		start = j+1;
		break;
	    }
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
		if ( xmax < x + toAdd + ww ) xmax = x + toAdd + ww;
		x += ww;
		pos++;
	    }
	}
	text->at( r->start + start ).startOfRun = TRUE;
	r = runs->next();
    }

    line->w = xmax + 10;
    QTextLineStart *ls = new QTextLineStart( control->context, control->status );
    delete control;
    delete runs;
    return ls;
}
#endif

bool QTextFormatter::isBreakable( QTextString *string, int pos )
{
    const QChar &c = string->at( pos ).c;
    char ch = c.latin1();
    if ( c == QChar_linesep )
	return TRUE;
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

void QTextFormatter::insertLineStart( QTextParagraph *parag, int index, QTextLineStart *ls )
{
    if ( index > 0 ) { // we can assume that only first line starts are insrted multiple times
	parag->lineStartList().insert( index, ls );
	return;
    }
    QMap<int, QTextLineStart*>::Iterator it;
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
int QTextFormatter::formatVertically( QTextDocument* doc, QTextParagraph* parag )
{
    int oldHeight = parag->rect().height();
    QMap<int, QTextLineStart*>& lineStarts = parag->lineStartList();
    QMap<int, QTextLineStart*>::Iterator it = lineStarts.begin();
    int h = parag->prev() ? QMAX(parag->prev()->bottomMargin(),parag->topMargin() ) / 2: 0;
    for ( ; it != lineStarts.end() ; ++it  ) {
	QTextLineStart * ls = it.data();
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
    if ( !parag->next() )
	m = 0;
    else
	m = QMAX(m, parag->next()->topMargin() ) / 2;
    h += m;
    parag->setHeight( h );
    return h - oldHeight;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

QTextFormatterBreakInWords::QTextFormatterBreakInWords()
{
}

#define SPACE(s) doc?(s>0?s:0):s

int QTextFormatterBreakInWords::format( QTextDocument *doc,QTextParagraph *parag,
					int start, const QMap<int, QTextLineStart*> & )
{
    QTextStringChar *c = 0;
    QTextStringChar *firstChar = 0;
    int left = doc ? parag->leftMargin() + doc->leftMargin() : 0;
    int x = left + ( doc ? parag->firstLineMargin() : 0 );
    int dw = parag->documentVisibleWidth() - ( doc ? doc->rightMargin() : 0 );
    int y = parag->prev() ? QMAX(parag->prev()->bottomMargin(),parag->topMargin()) / 2: 0;
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
    QTextLineStart *lineStart = new QTextLineStart( y, y, 0 );
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
	    lineStart = new QTextLineStart( y, h, h );
	    insertLineStart( parag, i, lineStart );
	    c->lineStart = 1;
	    firstChar = c;
	    x = 0xffffff;
	    continue;
	}

	if ( wrapEnabled &&
	     ( wrapAtColumn() == -1 && x + ww > w ||
	       wrapAtColumn() != -1 && col >= wrapAtColumn() ) ) {
	    x = doc ? parag->document()->flow()->adjustLMargin( y + parag->rect().y(), parag->rect().height(), left, 4 ) : left;
	    w = dw;
	    y += h;
	    h = c->height();
	    lineStart = formatLine( parag, parag->string(), lineStart, firstChar, SPACE(c-1) );
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
    if ( !parag->next() )
	m = 0;
    else
	m = QMAX(m, parag->next()->topMargin() ) / 2;
    parag->setFullWidth( fullWidth );
    y += h + m;
    if ( doc )
	minw += doc->rightMargin();
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

int QTextFormatterBreakWords::format( QTextDocument *doc, QTextParagraph *parag,
				      int start, const QMap<int, QTextLineStart*> & )
{
    QTextStringChar *c = 0;
    QTextStringChar *firstChar = 0;
    QTextString *string = parag->string();
    int left = doc ? parag->leftMargin() + doc->leftMargin() : 0;
    int x = left + ( doc ? parag->firstLineMargin() : 0 );
    int y = parag->prev() ? QMAX(parag->prev()->bottomMargin(),parag->topMargin()) / 2: 0;
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
    int linespacing = doc ? parag->lineSpacing() : 0;
    bool wrapEnabled = isWrapEnabled( parag );

    start = 0;
    if ( start == 0 )
	c = &parag->string()->at( 0 );

    int i = start;
    QTextLineStart *lineStart = new QTextLineStart( y, y, 0 );
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
	    QTextLineStart *lineStart2 = formatLine( parag, string, lineStart, firstChar, c-1, align, SPACE(w - x) );
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
		y += QMAX( h, QMAX( tmph, linespacing ) );
		tmph = c->height();
		h = tmph;
		lineStart = lineStart2;
		lineStart->y = y;
		insertLineStart( parag, i, lineStart );
		c->lineStart = 1;
		firstChar = c;
	    } else {
		tmph = c->height();
		h = tmph;
		delete lineStart2;
	    }
	    lineStart->h = h;
	    lineStart->baseLine = h;
	    tmpBaseLine = lineStart->baseLine;
	    lastBreak = -2;
	    x = 0xffffff;
	    minw = QMAX( minw, tminw );

	    int tw = ci->minimumWidth() + ( doc ? doc->leftMargin() : 0 );
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

	bool lastWasOwnLineCustomItem = lastBreak == -2;
	bool hadBreakableChar = lastBreak != -1;
	bool lastWasHardBreak = lastChr == QChar_linesep;

	// we break if
	// 1. the last character was a hard break (QChar_linesep) or
	// 2. the last charater was a own-line custom item (eg. table or ruler) or
	// 3. wrapping was enabled, it was not a space and following
	// condition is true: We either had a breakable character
	// previously or we ar allowed to break in words and - either
	// we break at w pixels and the current char would exceed that
	// or - we break at a column and the current character would
	// exceed that.
	if ( lastWasHardBreak || lastWasOwnLineCustomItem ||
	     ( wrapEnabled &&
	       ( (!c->c.isSpace() && (hadBreakableChar || allowBreakInWords()) &&
		  ( (wrapAtColumn() == -1 && x + ww > w) ||
		    (wrapAtColumn() != -1 && col >= wrapAtColumn()) ) ) )
	       )
	     ) {
	    if ( wrapAtColumn() != -1 )
		minw = QMAX( minw, x + ww );
	    // if a break was forced (no breakable char, hard break or own line custom item), break immediately....
	    if ( !hadBreakableChar || lastWasHardBreak || lastWasOwnLineCustomItem ) {
		if ( lineStart ) {
		    lineStart->baseLine = QMAX( lineStart->baseLine, tmpBaseLine );
		    h = QMAX( h, tmph );
		    lineStart->h = h;
  		    DO_FLOW( lineStart );
		}
		lineStart = formatLine( parag, string, lineStart, firstChar, c-1, align, SPACE(w - x) );
		x = doc ? doc->flow()->adjustLMargin( y + parag->rect().y(), parag->rect().height(), left, 4 ) : left;
		w = dw - ( doc ? doc->flow()->adjustRMargin( y + parag->rect().y(), parag->rect().height(), rm, 4 ) : 0 );
		if ( !doc && c->c == '\t' ) { // qt_format_text tab handling
		    int nx = parag->nextTab( i, x - left ) + left;
		    if ( nx < x )
			ww = w - x;
		    else
			ww = nx - x;
		}
		curLeft = x;
		y += QMAX( h, linespacing );
		tmph = c->height();
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
	    } else { // ... otherwise if we had a breakable char, break there
  		DO_FLOW( lineStart );
		i = lastBreak;
		lineStart = formatLine( parag, string, lineStart, firstChar, parag->at( lastBreak ),align, SPACE(w - string->at( i ).x) );
		x = doc ? doc->flow()->adjustLMargin( y + parag->rect().y(), parag->rect().height(), left, 4 ) : left;
		w = dw - ( doc ? doc->flow()->adjustRMargin( y + parag->rect().y(), parag->rect().height(), rm, 4 ) : 0 );
		if ( !doc && c->c == '\t' ) { // qt_format_text tab handling
		    int nx = parag->nextTab( i, x - left ) + left;
		    if ( nx < x )
			ww = w - x;
		    else
			ww = nx - x;
		}
		curLeft = x;
		y += QMAX( h, linespacing );
		tmph = c->height();
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
	} else if ( lineStart && isBreakable( string, i ) ) {
	    if ( len <= 2 || i < len - 1 ) {
		tmpBaseLine = QMAX( tmpBaseLine, c->ascent() );
		tmph = QMAX( tmph, c->height() );
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
	    int belowBaseLine = QMAX( tmph - tmpBaseLine, c->height()- c->ascent() );
	    tmpBaseLine = QMAX( tmpBaseLine, c->ascent() );
	    tmph = tmpBaseLine + belowBaseLine;
	}

	c->x = x;
	x += ww;
	wused = QMAX( wused, x );
    }

    // ### hack. The last char in the paragraph is always invisible,
    // ### and somehow sometimes has a wrong format. It changes
    // ### between // layouting and printing. This corrects some
    // ### layouting errors in BiDi mode due to this.
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
	if ( align == Qt3::AlignJustify || lastChr == QChar_linesep )
	    align = Qt3::AlignAuto;
 	DO_FLOW( lineStart );
	lineStart = formatLine( parag, string, lineStart, firstChar, c, align, SPACE(w - x) );
	delete lineStart;
    }

    minw = QMAX( minw, tminw );
    if ( doc )
	minw += doc->rightMargin();

    int m = parag->bottomMargin();
    if ( !parag->next() )
	m = 0;
    else
	m = QMAX(m, parag->next()->topMargin() ) / 2;
    parag->setFullWidth( fullWidth );
    y += QMAX( h, linespacing ) + m;

    wused += rm;
    if ( !wrapEnabled || wrapAtColumn() != -1 )
	minw = QMAX(minw, wused);

    // This is the case where we are breaking wherever we darn well please
    // in cases like that, the minw should not be the length of the entire
    // word, because we necessarily want to show the word on the whole line.
    // example: word wrap in iconview
    if ( allowBreakInWords() && minw > wused )
	minw = wused;

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
    : cKey( 307 )
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
	lastFormat = f;
	lastFormat->addRef();
	return lastFormat;
    }

    if ( f == lastFormat || ( lastFormat && f->key() == lastFormat->key() ) ) {
	lastFormat->addRef();
	return lastFormat;
    }

    QTextFormat *fm = cKey.find( f->key() );
    if ( fm ) {
	lastFormat = fm;
	lastFormat->addRef();
	return lastFormat;
    }

    if ( f->key() == defFormat->key() )
	return defFormat;

    lastFormat = createFormat( *f );
    lastFormat->collection = this;
    cKey.insert( lastFormat->key(), lastFormat );
    return lastFormat;
}

QTextFormat *QTextFormatCollection::format( QTextFormat *of, QTextFormat *nf, int flags )
{
    if ( cres && kof == of->key() && knf == nf->key() && cflags == flags ) {
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
    if ( flags & QTextFormat::StrikeOut )
	cres->fn.setStrikeOut( nf->fn.strikeOut() );
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
	cres->collection = this;
	cKey.insert( cres->key(), cres );
    } else {
	delete cres;
	cres = fm;
	cres->addRef();
    }

    return cres;
}

QTextFormat *QTextFormatCollection::format( const QFont &f, const QColor &c )
{
    if ( cachedFormat && cfont == f && ccol == c ) {
	cachedFormat->addRef();
	return cachedFormat;
    }

    QString key = QTextFormat::getKey( f, c, FALSE,  QTextFormat::AlignNormal );
    cachedFormat = cKey.find( key );
    cfont = f;
    ccol = c;

    if ( cachedFormat ) {
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

#define UPDATE( up, lo, rest ) \
	if ( font.lo##rest() != defFormat->fn.lo##rest() && fm->fn.lo##rest() == defFormat->fn.lo##rest() ) \
	    fm->fn.set##up##rest( font.lo##rest() )

void QTextFormatCollection::updateDefaultFormat( const QFont &font, const QColor &color, QStyleSheet *sheet )
{
    QDictIterator<QTextFormat> it( cKey );
    QTextFormat *fm;
    bool usePixels = font.pointSize() == -1;
    bool changeSize = usePixels ? font.pixelSize() != defFormat->fn.pixelSize() :
	font.pointSize() != defFormat->fn.pointSize();
    int base = usePixels ? font.pixelSize() : font.pointSize();
    while ( ( fm = it.current() ) ) {
	++it;
	UPDATE( F, f, amily );
	UPDATE( W, w, eight );
	UPDATE( B, b, old );
	UPDATE( I, i, talic );
	UPDATE( U, u, nderline );
	if ( changeSize ) {
	    fm->stdSize = base;
	    fm->usePixelSizes = usePixels;
	    if ( usePixels )
		fm->fn.setPixelSize( fm->stdSize );
	    else
		fm->fn.setPointSize( fm->stdSize );
	    sheet->scaleFont( fm->fn, fm->logicalFontSize );
	}
	if ( color.isValid() && color != defFormat->col && fm->col == defFormat->col )
	    fm->col = color;
	fm->update();
    }

    defFormat->fn = font;
    defFormat->col = color;
    defFormat->update();
    defFormat->stdSize = base;
    defFormat->usePixelSizes = usePixels;

    updateKeys();
}

// the keys in cKey have changed, rebuild the hashtable
void QTextFormatCollection::updateKeys()
{
    if ( cKey.isEmpty() )
	return;
    cKey.setAutoDelete( FALSE );
    QTextFormat** formats = new QTextFormat*[ cKey.count() + 1 ];
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
    delete [] formats;
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

void QTextFormat::setStrikeOut( bool b )
{
    if ( b == fn.strikeOut() )
	return;
    fn.setStrikeOut( b );
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

QString QTextFormat::makeFormatChangeTags( QTextFormat* defaultFormat, QTextFormat *f,
			   const QString& oldAnchorHref, const QString& anchorHref  ) const
{
    QString tag;
    if ( f )
	tag += f->makeFormatEndTags( defaultFormat, oldAnchorHref );

    if ( !anchorHref.isEmpty() )
	tag += "<a href=\"" + anchorHref + "\">";

    if ( font() != defaultFormat->font()
	 || vAlign() != defaultFormat->vAlign()
	 || color().rgb() != defaultFormat->color().rgb() ) {
	QString s;
	if ( font().family() != defaultFormat->font().family() )
	    s += QString(!!s?";":"") + "font-family:" + fn.family();
	if ( font().italic() && font().italic() != defaultFormat->font().italic() )
	    s += QString(!!s?";":"") + "font-style:" + (font().italic() ? "italic" : "normal");
	if ( font().pointSize() != defaultFormat->font().pointSize() )
	    s += QString(!!s?";":"") + "font-size:" + QString::number( fn.pointSize() ) + "pt";
	if ( font().weight() != defaultFormat->font().weight() )
	    s += QString(!!s?";":"") + "font-weight:" + QString::number( fn.weight() * 8 );
	if ( font().underline() != defaultFormat->font().underline() )
	    s += QString(!!s?";":"") + "text-decoration:" + ( font().underline() ? "underline" : "none");
	if ( vAlign() != defaultFormat->vAlign() ) {
	    s += QString(!!s?";":"") + "vertical-align:";
	    if ( vAlign() == QTextFormat::AlignSuperScript )
		s += "super";
	    else if ( vAlign() == QTextFormat::AlignSubScript )
		s += "sub";
	    else
		s += "normal";
	}
	if ( color().rgb() != defaultFormat->color().rgb() )
	    s += QString(!!s?";":"") + "color:" + col.name();
	if ( !s.isEmpty() )
	    tag += "<span style=\"" + s + "\">";
    }

    return tag;
}

QString QTextFormat::makeFormatEndTags( QTextFormat* defaultFormat, const QString& anchorHref ) const
{
    QString tag;
    if ( font().family() != defaultFormat->font().family()
	 || font().pointSize() != defaultFormat->font().pointSize()
	 || font().weight() != defaultFormat->font().weight()
	 || font().italic() != defaultFormat->font().italic()
	 || font().underline() != defaultFormat->font().underline()
	 || font().strikeOut() != defaultFormat->font().strikeOut()
	 || vAlign() != defaultFormat->vAlign()
	 || color().rgb() != defaultFormat->color().rgb() )
	tag += "</span>";
    if ( !anchorHref.isEmpty() )
	tag += "</a>";
    return tag;
}

QTextFormat QTextFormat::makeTextFormat( const QStyleSheetItem *style, const QMap<QString,QString>& attr, double scaleFontsFactor ) const
{
    QTextFormat format(*this);
    if (!style )
	return format;

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
    if ( style->definesFontStrikeOut() )
	format.fn.setStrikeOut( style->fontStrikeOut() );


    if ( style->name() == "font") {
	if ( attr.contains("color") ) {
	    QString s = attr["color"];
	    if ( !s.isEmpty() ) {
		format.col.setNamedColor( s );
		format.linkColor = FALSE;
	    }
	}
	if ( attr.contains("face") ) {
	    QString a = attr["face"];
	    QString family = QTextDocument::section( a, ",", 0, 0 );
	    if ( !!family )
		format.fn.setFamily( family );
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
    }
    if ( attr.contains("style" ) ) {
	QString a = attr["style"];
	for ( int s = 0; s < a.contains(';')+1; s++ ) {
	    QString style = QTextDocument::section( a, ";", s, s );
	    if ( style.startsWith("font-size:" ) && QTextDocument::endsWith(style, "pt") ) {
		format.logicalFontSize = 0;
		format.setPointSize( int( scaleFontsFactor * style.mid( 10, style.length() - 12 ).toInt() ) );
	    } if ( style.startsWith("font-style:" ) ) {
		QString s = style.mid( 11 ).stripWhiteSpace();
		if ( s == "normal" )
		    format.fn.setItalic( FALSE );
		else if ( s == "italic" || s == "oblique" )
		    format.fn.setItalic( TRUE );
	    } else if ( style.startsWith("font-weight:" ) ) {
		QString s = style.mid( 12 );
		bool ok = TRUE;
		int n = s.toInt( &ok );
		if ( ok )
		    format.fn.setWeight( n/8 );
	    } else if ( style.startsWith("font-family:" ) ) {
		format.fn.setFamily( QTextDocument::section(style.mid(12),",",0,0).stripWhiteSpace() );
	    } else if ( style.startsWith("text-decoration:" ) ) {
		QString s = style.mid( 16 ).stripWhiteSpace();
		format.fn.setUnderline( s == "underline" );
	    } else if ( style.startsWith("vertical-align:" ) ) {
		QString s = style.mid( 15 ).stripWhiteSpace();
		if ( s == "sub" )
		    format.setVAlign( QTextFormat::AlignSubScript );
		else if ( s == "super" )
		    format.setVAlign( QTextFormat::AlignSuperScript );
		else
		    format.setVAlign( QTextFormat::AlignNormal );
	    } else if ( style.startsWith("color:" ) ) {
		format.col.setNamedColor( style.mid(6) );
		format.linkColor = FALSE;
	    }
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
    width = height = 0;
    if ( attr.contains("width") )
	width = attr["width"].toInt();
    if ( attr.contains("height") )
	height = attr["height"].toInt();

    reg = 0;
    QString imageName = attr["src"];

    if (!imageName)
	imageName = attr["source"];

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
    delete reg;
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
					    const QChar* doc, int length, int& pos, QTextParagraph *curpar )
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
					    attr2, s, fmt.makeTextFormat( s, attr2, scaleFontsFactor ),
					    contxt, *factory_, sheet_,
					    QString( doc, length).mid( pos, end - pos ) );
			cell->richText()->parentPar = curpar;
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
	    if ( c == '\n' )
		return QChar_linesep;
	    else
		return c;
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
	s += "<td";
	it = cell->attributes.begin();
	for ( ; it != cell->attributes.end(); ++it )
	    s += " " + it.key() + "=" + *it;
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

bool QTextTable::enter( QTextCursor *c, QTextDocument *&doc, QTextParagraph *&parag, int &idx, int &ox, int &oy, bool atEnd )
{
    currCell.remove( c );
    if ( !atEnd )
	return next( c, doc, parag, idx, ox, oy );
    currCell.insert( c, cells.count() );
    return prev( c, doc, parag, idx, ox, oy );
}

bool QTextTable::enterAt( QTextCursor *c, QTextDocument *&doc, QTextParagraph *&parag, int &idx, int &ox, int &oy, const QPoint &pos )
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
    parag = doc->firstParagraph();
    idx = 0;
    ox += cell->geometry().x() + cell->horizontalAlignmentOffset() + outerborder + parent->x();
    oy += cell->geometry().y() + cell->verticalAlignmentOffset() + outerborder;
    return TRUE;
}

bool QTextTable::next( QTextCursor *c, QTextDocument *&doc, QTextParagraph *&parag, int &idx, int &ox, int &oy )
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
    parag = doc->firstParagraph();
    idx = 0;
    ox += cell->geometry().x() + cell->horizontalAlignmentOffset() + outerborder + parent->x();
    oy += cell->geometry().y() + cell->verticalAlignmentOffset() + outerborder;
    return TRUE;
}

bool QTextTable::prev( QTextCursor *c, QTextDocument *&doc, QTextParagraph *&parag, int &idx, int &ox, int &oy )
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
    parag = doc->lastParagraph();
    idx = parag->length() - 1;
    ox += cell->geometry().x() + cell->horizontalAlignmentOffset() + outerborder + parent->x();
    oy += cell->geometry().y()  + cell->verticalAlignmentOffset() + outerborder;
    return TRUE;
}

bool QTextTable::down( QTextCursor *c, QTextDocument *&doc, QTextParagraph *&parag, int &idx, int &ox, int &oy )
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
    parag = doc->firstParagraph();
    idx = 0;
    ox += cell->geometry().x() + cell->horizontalAlignmentOffset() + outerborder + parent->x();
    oy += cell->geometry().y()  + cell->verticalAlignmentOffset() + outerborder;
    return TRUE;
}

bool QTextTable::up( QTextCursor *c, QTextDocument *&doc, QTextParagraph *&parag, int &idx, int &ox, int &oy )
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
    parag = doc->lastParagraph();
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
    richtext->setDefaultFormat( table->parent->formatCollection()->defaultFormat()->font(),
				table->parent->formatCollection()->defaultFormat()->color() );
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
    QTextParagraph *parag = richtext->firstParagraph();
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
    if ( cx >= 0 && cy >= 0 )
	richtext->draw( p, cx - ( x + horizontalAlignmentOffset() + geom.x() ),
			cy - ( y + geom.y() + verticalAlignmentOffset() ),
			cw, ch, g, FALSE, FALSE, 0 );
    else
	richtext->draw( p, -1, -1, -1, -1, g, FALSE, FALSE, 0 );

    p->restore();
}

QString QTextDocument::section( QString str, const QString &sep, int start, int end )
{
    const QChar *uc = str.unicode();
    if ( !uc )
	return QString();
    QString _sep = sep;
    const QChar *uc_sep = _sep.unicode();
    if(!uc_sep)
	return QString();
    bool match = FALSE, last_match = TRUE;

    //find start
    int n = str.length(), sep_len = _sep.length();
    const QChar *begin = start < 0 ? uc + n : uc;
    while(start) {
	match = FALSE;
	int c = 0;
	for(const QChar *tmp = start < 0 ? begin - sep_len : begin;
	    c < sep_len && tmp < uc + n && tmp >= uc; tmp++, c++) {
	    if( *tmp != *(uc_sep + c) )
		break;
	    if(c == sep_len - 1) {
		match = TRUE;
		break;
	    }
	}
	last_match = match;

	if(start < 0) {
	    if(match) {
		begin -= sep_len;
		if(!++start)
		    break;
	    } else {
		if(start == -1 && begin == uc)
		    break;
		begin--;
	    }
	} else {
	    if(match) {
		if(!--start)
		    break;
		begin += sep_len;
	    } else {
		if(start == 1 && begin == uc + n)
		    break;
		begin++;
	    }
	}
	if(begin > uc + n || begin < uc)
	    return QString();
    }
    if(match)
	begin+=sep_len;
    if(begin > uc + n || begin < uc)
	return QString();

    //now find last
    match = FALSE;
    const QChar *last = end < 0 ? uc + n : uc;
    if(end == -1) {
	int c = 0;
	for(const QChar *tmp = end < 0 ? last - sep_len : last;
	    c < sep_len && tmp < uc + n && tmp >= uc; tmp++, c++) {
	    if( *tmp != *(uc_sep + c) )
		break;
	    if(c == sep_len - 1) {
		match = TRUE;
		break;
	    }
	}
    } else {
	end++;
	last_match = TRUE;
	while(end) {
	    match = FALSE;
	    int c = 0;
	    for(const QChar *tmp = end < 0 ? last - sep_len : last;
		c < sep_len && tmp < uc + n && tmp >= uc; tmp++, c++) {
		if( *tmp != *(uc_sep + c) )
		    break;
		if(c == sep_len - 1) {
		    match = TRUE;
		    break;
		}
	    }
	    last_match = match;

	    if(end < 0) {
		if(match) {
		    if(!++end)
			break;
		    last -= sep_len;
		} else {
		    last--;
		}
	    } else {
		if(match) {
		    last += sep_len;
		    if(!--end)
			break;
		} else {
		    last++;
		}
	    }
	    if(last >= uc + n) {
		last = uc + n;
		break;
	    } else if(last < uc) {
		return QString();
	    }
	}
    }
    if(match)
	last -= sep_len;
    if(last < uc || last > uc + n || begin >= last)
	return QString();

    //done
    return QString(begin, last - begin);
}

bool QTextDocument::endsWith( QString str, const QString &s)
{
    if ( str.isNull() )
	return s.isNull();
    int pos = str.length() - s.length();
    if ( pos < 0 )
	return FALSE;
    for ( uint i = 0; i < s.length(); i++ ) {
	if ( str.unicode()[pos+i] != s[(int)i] )
	    return FALSE;
    }
    return TRUE;
}
