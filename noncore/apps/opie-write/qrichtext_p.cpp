/****************************************************************************
** $Id: qrichtext_p.cpp,v 1.1 2002-07-14 21:21:35 leseb Exp $
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

using namespace Qt3;

QTextCommand::~QTextCommand() {}
QTextCommand::Commands QTextCommand::type() const { return Invalid; }


QTextCustomItem::~QTextCustomItem() {}
void QTextCustomItem::adjustToPainter( QPainter* p){ if ( p ) width = 0; }
QTextCustomItem::Placement QTextCustomItem::placement() const { return PlaceInline; }

bool QTextCustomItem::ownLine() const { return FALSE; }
void QTextCustomItem::resize( int nwidth ){ width = nwidth; }
void QTextCustomItem::invalidate() {}

bool QTextCustomItem::isNested() const { return FALSE; }
int QTextCustomItem::minimumWidth() const { return 0; }

QString QTextCustomItem::richText() const { return QString::null; }

bool QTextCustomItem::enter( QTextCursor *, QTextDocument *&doc, QTextParag *&parag, int &idx, int &ox, int &oy, bool atEnd )
{
    doc = doc; parag = parag; idx = idx; ox = ox; oy = oy; Q_UNUSED( atEnd ) return TRUE;
							
}
bool QTextCustomItem::enterAt( QTextCursor *, QTextDocument *&doc, QTextParag *&parag, int &idx, int &ox, int &oy, const QPoint & )
{
    doc = doc; parag = parag; idx = idx; ox = ox; oy = oy; return TRUE;
}
bool QTextCustomItem::next( QTextCursor *, QTextDocument *&doc, QTextParag *&parag, int &idx, int &ox, int &oy )
{
    doc = doc; parag = parag; idx = idx; ox = ox; oy = oy; return TRUE;
}
bool QTextCustomItem::prev( QTextCursor *, QTextDocument *&doc, QTextParag *&parag, int &idx, int &ox, int &oy )
{
    doc = doc; parag = parag; idx = idx; ox = ox; oy = oy; return TRUE;
}
bool QTextCustomItem::down( QTextCursor *, QTextDocument *&doc, QTextParag *&parag, int &idx, int &ox, int &oy )
{
    doc = doc; parag = parag; idx = idx; ox = ox; oy = oy; return TRUE;
}
bool QTextCustomItem::up( QTextCursor *, QTextDocument *&doc, QTextParag *&parag, int &idx, int &ox, int &oy )
{
    doc = doc; parag = parag; idx = idx; ox = ox; oy = oy; return TRUE;
}

void QTextFlow::setPageSize( int ps ) { pagesize = ps; }
bool QTextFlow::isEmpty() { return leftItems.isEmpty() && rightItems.isEmpty(); }

void QTextTableCell::invalidate() { cached_width = -1; cached_sizehint = -1; }

void QTextTable::invalidate() { cachewidth = -1; }

QTextParagData::~QTextParagData() {}
void QTextParagData::join( QTextParagData * ) {}

QTextFormatter::~QTextFormatter() {}
void QTextFormatter::setWrapEnabled( bool b ) { wrapEnabled = b; }
void QTextFormatter::setWrapAtColumn( int c ) { wrapColumn = c; }



int QTextCursor::x() const
{
    QTextStringChar *c = string->at( idx );
    int curx = c->x;
    if ( !c->rightToLeft &&
	 c->c.isSpace() &&
	 idx > 0 &&
	 ( string->alignment() & Qt3::AlignJustify ) == Qt3::AlignJustify )
	curx = string->at( idx - 1 )->x + string->string()->width( idx - 1 );
    if ( c->rightToLeft )
	curx += string->string()->width( idx );
    return curx;
}

int QTextCursor::y() const
{
    int dummy, line;
    string->lineStartOfChar( idx, &dummy, &line );
    return string->lineY( line );
}

bool QTextDocument::hasSelection( int id, bool visible ) const
{
    return ( selections.find( id ) != selections.end() &&
	     ( !visible ||
	       ( (QTextDocument*)this )->selectionStartCursor( id ) !=
	       ( (QTextDocument*)this )->selectionEndCursor( id ) ) );
}

void QTextDocument::setSelectionStart( int id, QTextCursor *cursor )
{
    QTextDocumentSelection sel;
    sel.startCursor = *cursor;
    sel.endCursor = *cursor;
    sel.swapped = FALSE;
    selections[ id ] = sel;
}

QTextParag *QTextDocument::paragAt( int i ) const
{
    QTextParag* p = curParag;
    if ( !p || p->paragId() > i )
	p = fParag;
    while ( p && p->paragId() != i )
	p = p->next();
    ((QTextDocument*)this)->curParag = p;
    return p;
}


QTextFormat::~QTextFormat()
{
}

QTextFormat::QTextFormat()
    : fm( QFontMetrics( fn ) ), linkColor( TRUE ), logicalFontSize( 3 ), stdSize( qApp->font().pointSize() ),
      different( NoFlags )
{
    ref = 0;

    usePixelSizes = FALSE;
    if ( stdSize == -1 ) {
	stdSize = qApp->font().pixelSize();
	usePixelSizes = TRUE;
    }
    
    missp = FALSE;
    ha = AlignNormal;
    collection = 0;
}

QTextFormat::QTextFormat( const QStyleSheetItem *style )
    : fm( QFontMetrics( fn ) ), linkColor( TRUE ), logicalFontSize( 3 ), stdSize( qApp->font().pointSize() ),
      different( NoFlags )
{
    ref = 0;

    usePixelSizes = FALSE;
    if ( stdSize == -1 ) {
	stdSize = qApp->font().pixelSize();
	usePixelSizes = TRUE;
    }

    this->style = style->name();
    missp = FALSE;
    ha = AlignNormal;
    collection = 0;
    fn = QFont( style->fontFamily(),
		style->fontSize(),
		style->fontWeight(),
		style->fontItalic() );
    fn.setUnderline( style->fontUnderline() );
    col = style->color();
    fm = QFontMetrics( fn );
    leftBearing = fm.minLeftBearing();
    rightBearing = fm.minRightBearing();
    hei = fm.lineSpacing();
    asc = fm.ascent() + (fm.leading()+1)/2;
    dsc = fm.descent();
    missp = FALSE;
    ha = AlignNormal;
    memset( widths, 0, 256 );
    generateKey();
    addRef();
    updateStyleFlags();
}

QTextFormat::QTextFormat( const QFont &f, const QColor &c, QTextFormatCollection *parent )
    : fn( f ), col( c ), fm( QFontMetrics( f ) ), linkColor( TRUE ),
      logicalFontSize( 3 ), stdSize( f.pointSize() ), different( NoFlags )
{
    ref = 0;
    usePixelSizes = FALSE;
    if ( stdSize == -1 ) {
	stdSize = f.pixelSize();
	usePixelSizes = TRUE;
    }
    collection = parent;
    leftBearing = fm.minLeftBearing();
    rightBearing = fm.minRightBearing();
    hei = fm.lineSpacing();
    asc = fm.ascent() + (fm.leading()+1)/2;
    dsc = fm.descent();
    missp = FALSE;
    ha = AlignNormal;
    memset( widths, 0, 256 );
    generateKey();
    addRef();
    updateStyleFlags();
}

QTextFormat::QTextFormat( const QTextFormat &f )
    : fm( f.fm )
{
    ref = 0;
    collection = 0;
    fn = f.fn;
    col = f.col;
    leftBearing = f.leftBearing;
    rightBearing = f.rightBearing;
    memset( widths, 0, 256 );
    hei = f.hei;
    asc = f.asc;
    dsc = f.dsc;
    stdSize = f.stdSize;
    usePixelSizes = f.usePixelSizes;
    logicalFontSize = f.logicalFontSize;
    missp = f.missp;
    ha = f.ha;
    k = f.k;
    linkColor = f.linkColor;
    style = f.style;
    different = f.different;
    addRef();
}

QTextFormat& QTextFormat::operator=( const QTextFormat &f )
{
    ref = 0;
    collection = f.collection;
    fn = f.fn;
    col = f.col;
    fm = f.fm;
    leftBearing = f.leftBearing;
    rightBearing = f.rightBearing;
    memset( widths, 0, 256 );
    hei = f.hei;
    asc = f.asc;
    dsc = f.dsc;
    stdSize = f.stdSize;
    usePixelSizes = f.usePixelSizes;
    logicalFontSize = f.logicalFontSize;
    missp = f.missp;
    ha = f.ha;
    k = f.k;
    linkColor = f.linkColor;
    style = f.style;
    different = f.different;
    addRef();
    return *this;
}

void QTextFormat::update()
{
    fm = QFontMetrics( fn );
    leftBearing = fm.minLeftBearing();
    rightBearing = fm.minRightBearing();
    hei = fm.lineSpacing();
    asc = fm.ascent() + (fm.leading()+1)/2;
    dsc = fm.descent();
    memset( widths, 0, 256 );
    generateKey();
    updateStyleFlags();
}


QPainter* QTextFormat::pntr = 0;

void QTextFormat::setPainter( QPainter *p )
{
    pntr = p;
}

QPainter*  QTextFormat::painter()
{
    return pntr;
}


int QTextFormat::minLeftBearing() const
{
    if ( !pntr || !pntr->isActive() )
	return leftBearing;
    pntr->setFont( fn );
    return pntr->fontMetrics().minLeftBearing();
}

int QTextFormat::minRightBearing() const
{
    if ( !pntr || !pntr->isActive() )
	return rightBearing;
    pntr->setFont( fn );
    return pntr->fontMetrics().minRightBearing();
}

int QTextFormat::height() const
{
    if ( !pntr || !pntr->isActive() )
	return hei;
    pntr->setFont( fn );
    return pntr->fontMetrics().lineSpacing();
}

int QTextFormat::ascent() const
{
    if ( !pntr || !pntr->isActive() )
	return asc;
    pntr->setFont( fn );
    return pntr->fontMetrics().ascent() + (pntr->fontMetrics().leading()+1)/2;
}

int QTextFormat::descent() const
{
    if ( !pntr || !pntr->isActive() )
	return dsc;
    pntr->setFont( fn );
    return pntr->fontMetrics().descent();
}

int QTextFormat::leading() const
{
    if ( !pntr || !pntr->isActive() )
	return fm.leading();
    pntr->setFont( fn );
    return pntr->fontMetrics().leading();
}

void QTextFormat::generateKey()
{
    k = getKey( fn, col, isMisspelled(), vAlign() );
}

QString QTextFormat::getKey( const QFont &fn, const QColor &col, bool misspelled, VerticalAlignment a )
{
    QString k = fn.key();
    k += '/';
    k += QString::number( (uint)col.rgb() );
    k += '/';
    k += QString::number( (int)misspelled );
    k += '/';
    k += QString::number( (int)a );
    return k;
}

void QTextFormat::updateStyle()
{
    if ( !collection || !collection->styleSheet() )
	return;
    QStyleSheetItem *item = collection->styleSheet()->item( style );
    if ( !item )
	return;
    if ( !( different & Color ) && item->color().isValid() )
	col = item->color();
    if ( !( different & Size ) && item->fontSize() != -1 )
	fn.setPointSize( item->fontSize() );
    if ( !( different & Family ) && !item->fontFamily().isEmpty() )
	fn.setFamily( item->fontFamily() );
    if ( !( different & Bold ) && item->fontWeight() != -1 )
	fn.setWeight( item->fontWeight() );
    if ( !( different & Italic ) && item->definesFontItalic() )
	fn.setItalic( item->fontItalic() );
    if ( !( different & Underline ) && item->definesFontUnderline() )
	fn.setUnderline( item->fontUnderline() );
    generateKey();
    update();

}

void QTextFormat::updateStyleFlags()
{
    different = NoFlags;
    if ( !collection || !collection->styleSheet() )
	return;
    QStyleSheetItem *item = collection->styleSheet()->item( style );
    if ( !item )
	return;
    if ( item->color() != col )
	different |= Color;
    if ( item->fontSize() != fn.pointSize() )
	different |= Size;
    if ( item->fontFamily() != fn.family() )
	different |= Family;
    if ( item->fontItalic() != fn.italic() )
	different |= Italic;
    if ( item->fontUnderline() != fn.underline() )
	different |= Underline;
    if ( item->fontWeight() != fn.weight() )
	different |= Bold;
}

QString QTextString::toString( const QMemArray<QTextStringChar> &data )
{
    QString s;
    int l = data.size();
    s.setUnicode( 0, l );
    QTextStringChar *c = data.data();
    QChar *uc = (QChar *)s.unicode();
    while ( l-- ) {
	*uc = c->c;
	// ### workaround so that non-breaking whitespaces are drawn
	// properly, actually this should be fixed in QFont somewhere
	if ( *uc == (char)0xa0 )
	    *uc = 0x20;
	uc++;
	c++;
    }

    return s;
}

QString QTextString::toString() const
{
    return toString( data );
}

void QTextParag::setSelection( int id, int start, int end )
{
    QMap<int, QTextParagSelection>::ConstIterator it = selections().find( id );
    if ( it != mSelections->end() ) {
	if ( start == ( *it ).start && end == ( *it ).end )
	    return;
    }

    QTextParagSelection sel;
    sel.start = start;
    sel.end = end;
    (*mSelections)[ id ] = sel;
    setChanged( TRUE, TRUE );
}

void QTextParag::removeSelection( int id )
{
    if ( !hasSelection( id ) )
	return;
    if ( mSelections )
	mSelections->remove( id );
    setChanged( TRUE, TRUE );
}

int QTextParag::selectionStart( int id ) const
{
    if ( !mSelections )
	return -1;
    QMap<int, QTextParagSelection>::ConstIterator it = mSelections->find( id );
    if ( it == mSelections->end() )
	return -1;
    return ( *it ).start;
}

int QTextParag::selectionEnd( int id ) const
{
    if ( !mSelections )
	return -1;
    QMap<int, QTextParagSelection>::ConstIterator it = mSelections->find( id );
    if ( it == mSelections->end() )
	return -1;
    return ( *it ).end;
}

bool QTextParag::hasSelection( int id ) const
{
    if ( !mSelections )
	return FALSE;
    QMap<int, QTextParagSelection>::ConstIterator it = mSelections->find( id );
    if ( it == mSelections->end() )
	return FALSE;
    return ( *it ).start != ( *it ).end || length() == 1;
}

bool QTextParag::fullSelected( int id ) const
{
    if ( !mSelections )
	return FALSE;
    QMap<int, QTextParagSelection>::ConstIterator it = mSelections->find( id );
    if ( it == mSelections->end() )
	return FALSE;
    return ( *it ).start == 0 && ( *it ).end == str->length() - 1;
}

int QTextParag::lineY( int l ) const
{
    if ( l > (int)lineStarts.count() - 1 ) {
	qWarning( "QTextParag::lineY: line %d out of range!", l );
	return 0;
    }

    if ( !isValid() )
	( (QTextParag*)this )->format();

    QMap<int, QTextParagLineStart*>::ConstIterator it = lineStarts.begin();
    while ( l-- > 0 )
	++it;
    return ( *it )->y;
}

int QTextParag::lineBaseLine( int l ) const
{
    if ( l > (int)lineStarts.count() - 1 ) {
	qWarning( "QTextParag::lineBaseLine: line %d out of range!", l );
	return 10;
    }

    if ( !isValid() )
	( (QTextParag*)this )->format();

    QMap<int, QTextParagLineStart*>::ConstIterator it = lineStarts.begin();
    while ( l-- > 0 )
	++it;
    return ( *it )->baseLine;
}

int QTextParag::lineHeight( int l ) const
{
    if ( l > (int)lineStarts.count() - 1 ) {
	qWarning( "QTextParag::lineHeight: line %d out of range!", l );
	return 15;
    }

    if ( !isValid() )
	( (QTextParag*)this )->format();

    QMap<int, QTextParagLineStart*>::ConstIterator it = lineStarts.begin();
    while ( l-- > 0 )
	++it;
    return ( *it )->h;
}

void QTextParag::lineInfo( int l, int &y, int &h, int &bl ) const
{
    if ( l > (int)lineStarts.count() - 1 ) {
	qWarning( "QTextParag::lineInfo: line %d out of range!", l );
	qDebug( "%d %d", (int)lineStarts.count() - 1, l );
	y = 0;
	h = 15;
	bl = 10;
	return;
    }

    if ( !isValid() )
	( (QTextParag*)this )->format();

    QMap<int, QTextParagLineStart*>::ConstIterator it = lineStarts.begin();
    while ( l-- > 0 )
	++it;
    y = ( *it )->y;
    h = ( *it )->h;
    bl = ( *it )->baseLine;
}

int QTextParag::alignment() const
{
    if ( align != -1 )
	return align;
    QStyleSheetItem *item = style();
    if ( !item )
	return Qt3::AlignAuto;
    if ( mStyleSheetItemsVec ) {
	for ( int i = 0; i < (int)mStyleSheetItemsVec->size(); ++i ) {
	    item = (*mStyleSheetItemsVec)[ i ];
	    if ( item->alignment() != QStyleSheetItem::Undefined )
		return item->alignment();
	}
    }
    return Qt3::AlignAuto;
}

QPtrVector<QStyleSheetItem> QTextParag::styleSheetItems() const
{
    QPtrVector<QStyleSheetItem> vec;
    if ( mStyleSheetItemsVec ) {
	vec.resize( mStyleSheetItemsVec->size() );
	for ( int i = 0; i < (int)vec.size(); ++i )
	    vec.insert( i, (*mStyleSheetItemsVec)[ i ] );
    }
    return vec;
}

QStyleSheetItem *QTextParag::style() const
{
    if ( !mStyleSheetItemsVec || mStyleSheetItemsVec->size() == 0 )
	return 0;
    return (*mStyleSheetItemsVec)[ mStyleSheetItemsVec->size() - 1 ];
}

int QTextParag::numberOfSubParagraph() const
{
    if ( list_val != -1 )
	return list_val;
    if ( numSubParag != -1 )
 	return numSubParag;
    int n = 0;
    QTextParag *p = (QTextParag*)this;
    while ( p && ( styleSheetItemsVec().size() >= p->styleSheetItemsVec().size() &&
	    styleSheetItemsVec()[ (int)p->styleSheetItemsVec().size() - 1 ] == p->style() ||
		   p->styleSheetItemsVec().size() >= styleSheetItemsVec().size() &&
		   p->styleSheetItemsVec()[ (int)styleSheetItemsVec().size() - 1 ] == style() ) ) {
	if ( p->style() == style() && listStyle() != p->listStyle()
	     && p->styleSheetItemsVec().size() == styleSheetItemsVec().size() )
	    break;
	if ( p->style()->displayMode() == QStyleSheetItem::DisplayListItem
	     && p->style() != style() || styleSheetItemsVec().size() == p->styleSheetItemsVec().size() )
	    ++n;
	p = p->prev();
    }
    ( (QTextParag*)this )->numSubParag = n;
    return n;
}

void QTextParag::setFormat( QTextFormat *fm )
{
    bool doUpdate = FALSE;
    if (defFormat && (defFormat != formatCollection()->defaultFormat()))
       doUpdate = TRUE;
    defFormat = formatCollection()->format( fm );
    if ( !doUpdate )
	return;
    for ( int i = 0; i < length(); ++i ) {
	if ( at( i )->format()->styleName() == defFormat->styleName() )
	    at( i )->format()->updateStyle();
    }
}

QTextFormatter *QTextParag::formatter() const
{
    if ( hasdoc )
	return document()->formatter();
    if ( pseudoDocument()->pFormatter )
	return pseudoDocument()->pFormatter;
    return ( ( (QTextParag*)this )->pseudoDocument()->pFormatter = new QTextFormatterBreakWords );
}

void QTextParag::setTabArray( int *a )
{
    delete [] tArray;
    tArray = a;
}

void QTextParag::setTabStops( int tw )
{
    if ( hasdoc )
	document()->setTabStops( tw );
    else
	tabStopWidth = tw;
}

QMap<int, QTextParagSelection> &QTextParag::selections() const
{
    if ( !mSelections )
	((QTextParag *)this)->mSelections = new QMap<int, QTextParagSelection>;
    return *mSelections;
}

QPtrVector<QStyleSheetItem> &QTextParag::styleSheetItemsVec() const
{
    if ( !mStyleSheetItemsVec )
	((QTextParag *)this)->mStyleSheetItemsVec = new QPtrVector<QStyleSheetItem>;
    return *mStyleSheetItemsVec;
}

QPtrList<QTextCustomItem> &QTextParag::floatingItems() const
{
    if ( !mFloatingItems )
	((QTextParag *)this)->mFloatingItems = new QPtrList<QTextCustomItem>;
    return *mFloatingItems;
}

QTextStringChar::~QTextStringChar()
{
    if ( format() )
	format()->removeRef();
    if ( type ) // not Regular
	delete d.custom;
}

QTextParagPseudoDocument::QTextParagPseudoDocument():pFormatter(0),commandHistory(0), minw(0),wused(0){}
QTextParagPseudoDocument::~QTextParagPseudoDocument(){ delete pFormatter; delete commandHistory; }
