/****************************************************************************
** $Id: qrichtext_p.h,v 1.1 2002-11-01 00:10:43 kergoth Exp $
**
** Definition of internal rich text classes
**
** Created : 990124
**
** Copyright (C) 1999-2002 Trolltech AS.  All rights reserved.
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

#ifndef QRICHTEXT_P_H
#define QRICHTEXT_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of a number of Qt sources files.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//
//

#ifndef QT_H
#include "qstring.h"
#include "qptrlist.h"
#include "qrect.h"
#include "qfontmetrics.h"
#include "qintdict.h"
#include "qmap.h"
#include "qstringlist.h"
#include "qfont.h"
#include "qcolor.h"
#include "qsize.h"
#include "qvaluelist.h"
#include "qvaluestack.h"
#include "qobject.h"
#include "qdict.h"
#include "qpixmap.h"
#include "qstylesheet.h"
#include "qptrvector.h"
#include "qpainter.h"
#include "qlayout.h"
#include "qobject.h"
#include "private/qcomplextext_p.h"
#include "qapplication.h"
#endif // QT_H

#ifndef QT_NO_RICHTEXT

class QTextDocument;
class QTextString;
class QTextPreProcessor;
class QTextFormat;
class QTextCursor;
class QTextParagraph;
class QTextFormatter;
class QTextIndent;
class QTextFormatCollection;
class QStyleSheetItem;
#ifndef QT_NO_TEXTCUSTOMITEM
class QTextCustomItem;
#endif
class QTextFlow;
struct QBidiContext;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class Q_EXPORT QTextStringChar
{
    friend class QTextString;

public:
    // this is never called, initialize variables in QTextString::insert()!!!
    QTextStringChar() : lineStart( 0 ), type( Regular ), startOfRun( 0 ) {d.format=0;}
    ~QTextStringChar();

    QChar c;
    enum Type { Regular=0, Custom=1, Anchor=2, CustomAnchor=3 };
    uint lineStart : 1;
    uint rightToLeft : 1;
    uint hasCursor : 1;
    uint canBreak : 1;
    Type type : 2;
    uint startOfRun : 1;

    int x;
    int height() const;
    int ascent() const;
    int descent() const;
    bool isCustom() const { return (type & Custom) != 0; }
    QTextFormat *format() const;
#ifndef QT_NO_TEXTCUSTOMITEM
    QTextCustomItem *customItem() const;
#endif
    void setFormat( QTextFormat *f );
#ifndef QT_NO_TEXTCUSTOMITEM
    void setCustomItem( QTextCustomItem *i );
#endif
    struct CustomData
    {
	QTextFormat *format;
#ifndef QT_NO_TEXTCUSTOMITEM
	QTextCustomItem *custom;
#endif
	QString anchorName;
	QString anchorHref;
    };

#ifndef QT_NO_TEXTCUSTOMITEM
    void loseCustomItem();
#endif

    union {
	QTextFormat* format;
	CustomData* custom;
    } d;

    bool isAnchor() const { return ( type & Anchor) != 0; }
    bool isLink() const { return isAnchor() && !!d.custom->anchorHref; }
    QString anchorName() const;
    QString anchorHref() const;
    void setAnchor( const QString& name, const QString& href );

private:
    QTextStringChar &operator=( const QTextStringChar & ) {
	//abort();
	return *this;
    }
    friend class QComplexText;
    friend class QTextParagraph;
};

#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
Q_TEMPLATE_EXTERN template class Q_EXPORT QMemArray<QTextStringChar>;
// MOC_SKIP_END
#endif

class Q_EXPORT QTextString
{
public:

    QTextString();
    QTextString( const QTextString &s );
    virtual ~QTextString();

    static QString toString( const QMemArray<QTextStringChar> &data );
    QString toString() const;

    QTextStringChar &at( int i ) const;
#if defined(Q_STRICT_INLINING_RULES)
    // This is for the IRIX MIPSpro o32 ABI - it fails, claiming the
    // implementation to be a redefinition.
    inline int length() const;
#else
    int length() const;
#endif

    int width( int idx ) const;

    void insert( int index, const QString &s, QTextFormat *f );
    void insert( int index, const QChar *unicode, int len, QTextFormat *f );
    void insert( int index, QTextStringChar *c, bool doAddRefFormat = FALSE );
    void truncate( int index );
    void remove( int index, int len );
    void clear();

    void setFormat( int index, QTextFormat *f, bool useCollection );

    void setBidi( bool b ) { bidi = b; }
    bool isBidi() const;
    bool isRightToLeft() const;
    QChar::Direction direction() const;
    void setDirection( QChar::Direction d ) { dir = d; bidiDirty = TRUE; }

    QMemArray<QTextStringChar> subString( int start = 0, int len = 0xFFFFFF ) const;
    QMemArray<QTextStringChar> rawData() const { return data.copy(); }

    void operator=( const QString &s ) { clear(); insert( 0, s, 0 ); }
    void operator+=( const QString &s ) { insert( length(), s, 0 ); }
    void prepend( const QString &s ) { insert( 0, s, 0 ); }

private:
    void checkBidi() const;

    QMemArray<QTextStringChar> data;
    uint bidiDirty : 1;
    uint bidi : 1; // true when the paragraph has right to left characters
    uint rightToLeft : 1;
    uint dir : 5;
};

inline bool QTextString::isBidi() const
{
    if ( bidiDirty )
	checkBidi();
    return bidi;
}

inline bool QTextString::isRightToLeft() const
{
    if ( bidiDirty )
	checkBidi();
    return rightToLeft;
}

inline QChar::Direction QTextString::direction() const
{
    return (QChar::Direction) dir;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
Q_TEMPLATE_EXTERN template class Q_EXPORT QValueStack<int>;
Q_TEMPLATE_EXTERN template class Q_EXPORT QValueStack<QTextParagraph*>;
Q_TEMPLATE_EXTERN template class Q_EXPORT QValueStack<bool>;
// MOC_SKIP_END
#endif

class Q_EXPORT QTextCursor
{
public:
    QTextCursor( QTextDocument *d = 0 );
    QTextCursor( const QTextCursor &c );
    QTextCursor &operator=( const QTextCursor &c );
    virtual ~QTextCursor() {}

    bool operator==( const QTextCursor &c ) const;
    bool operator!=( const QTextCursor &c ) const { return !(*this == c); }

#if defined(Q_STRICT_INLINING_RULES)
    // This is for the IRIX MIPSpro o32 ABI - it fails, claiming the
    // implementation to be a redefinition.
    inline QTextParagraph *paragraph() const;
#else
    QTextParagraph *paragraph() const;
#endif

    void setParagraph( QTextParagraph*p ) { gotoPosition(p, 0 ); }
    QTextDocument *document() const;
    int index() const;
    void setIndex( int index ) { gotoPosition(paragraph(), index ); }

    void gotoPosition( QTextParagraph* p, int index = 0);
    void gotoLeft();
    void gotoRight();
    void gotoNextLetter();
    void gotoPreviousLetter();
    void gotoUp();
    void gotoDown();
    void gotoLineEnd();
    void gotoLineStart();
    void gotoHome();
    void gotoEnd();
    void gotoPageUp( int visibleHeight );
    void gotoPageDown( int visibleHeight );
    void gotoNextWord();
    void gotoPreviousWord();
    void gotoWordLeft();
    void gotoWordRight();

    void insert( const QString &s, bool checkNewLine, QMemArray<QTextStringChar> *formatting = 0 );
    void splitAndInsertEmptyParagraph( bool ind = TRUE, bool updateIds = TRUE );
    bool remove();
    void indent();

    bool atParagStart();
    bool atParagEnd();

    int x() const; // x in current paragraph
    int y() const; // y in current paragraph

    int globalX() const;
    int globalY() const;

    QTextParagraph *topParagraph() const { return paras.isEmpty() ? para : paras.first(); }
    int offsetX() const { return ox; } // inner document  offset
    int offsetY() const { return oy; } // inner document offset
    int totalOffsetX() const; // total document offset
    int totalOffsetY() const; // total document offset

    bool place( const QPoint &pos, QTextParagraph *s ) { return place( pos, s, FALSE ); }
    bool place( const QPoint &pos, QTextParagraph *s, bool link );
    void restoreState();


    int nestedDepth() const { return (int)indices.count(); } //### size_t/int cast
    void oneUp() { if ( !indices.isEmpty() ) pop(); }
    void setValid( bool b ) { valid = b; }
    bool isValid() const { return valid; }

private:
    enum Operation { EnterBegin, EnterEnd, Next, Prev, Up, Down };

    void push();
    void pop();
    void processNesting( Operation op );
    void invalidateNested();
    void gotoIntoNested( const QPoint &globalPos );

    QTextParagraph *para;
    int idx, tmpIndex;
    int ox, oy;
    QValueStack<int> indices;
    QValueStack<QTextParagraph*> paras;
    QValueStack<int> xOffsets;
    QValueStack<int> yOffsets;
    uint valid : 1;

};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class Q_EXPORT QTextCommand
{
public:
    enum Commands { Invalid, Insert, Delete, Format, Style };

    QTextCommand( QTextDocument *d ) : doc( d ), cursor( d ) {}
    virtual ~QTextCommand();

    virtual Commands type() const;

    virtual QTextCursor *execute( QTextCursor *c ) = 0;
    virtual QTextCursor *unexecute( QTextCursor *c ) = 0;

protected:
    QTextDocument *doc;
    QTextCursor cursor;

};

#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
Q_TEMPLATE_EXTERN template class Q_EXPORT QPtrList<QTextCommand>;
// MOC_SKIP_END
#endif

class Q_EXPORT QTextCommandHistory
{
public:
    QTextCommandHistory( int s ) : current( -1 ), steps( s ) { history.setAutoDelete( TRUE ); }
    virtual ~QTextCommandHistory();

    void clear() { history.clear(); current = -1; }

    void addCommand( QTextCommand *cmd );
    QTextCursor *undo( QTextCursor *c );
    QTextCursor *redo( QTextCursor *c );

    bool isUndoAvailable();
    bool isRedoAvailable();

    void setUndoDepth( int d ) { steps = d; }
    int undoDepth() const { return steps; }

    int historySize() const { return history.count(); }
    int currentPosition() const { return current; }

private:
    QPtrList<QTextCommand> history;
    int current, steps;

};

inline QTextCommandHistory::~QTextCommandHistory()
{
    clear();
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef QT_NO_TEXTCUSTOMITEM
class Q_EXPORT QTextCustomItem
{
public:
    QTextCustomItem( QTextDocument *p )
	:  xpos(0), ypos(-1), width(-1), height(0), parent( p )
    {}
    virtual ~QTextCustomItem();
    virtual void draw(QPainter* p, int x, int y, int cx, int cy, int cw, int ch, const QColorGroup& cg, bool selected ) = 0;

    virtual void adjustToPainter( QPainter* );

    enum Placement { PlaceInline = 0, PlaceLeft, PlaceRight };
    virtual Placement placement() const;
    bool placeInline() { return placement() == PlaceInline; }

    virtual bool ownLine() const;
    virtual void resize( int nwidth );
    virtual void invalidate();
    virtual int ascent() const { return height; }

    virtual bool isNested() const;
    virtual int minimumWidth() const;

    virtual QString richText() const;

    int xpos; // used for floating items
    int ypos; // used for floating items
    int width;
    int height;

    QRect geometry() const { return QRect( xpos, ypos, width, height ); }

    virtual bool enter( QTextCursor *, QTextDocument *&doc, QTextParagraph *&parag, int &idx, int &ox, int &oy, bool atEnd = FALSE );
    virtual bool enterAt( QTextCursor *, QTextDocument *&doc, QTextParagraph *&parag, int &idx, int &ox, int &oy, const QPoint & );
    virtual bool next( QTextCursor *, QTextDocument *&doc, QTextParagraph *&parag, int &idx, int &ox, int &oy );
    virtual bool prev( QTextCursor *, QTextDocument *&doc, QTextParagraph *&parag, int &idx, int &ox, int &oy );
    virtual bool down( QTextCursor *, QTextDocument *&doc, QTextParagraph *&parag, int &idx, int &ox, int &oy );
    virtual bool up( QTextCursor *, QTextDocument *&doc, QTextParagraph *&parag, int &idx, int &ox, int &oy );

    void setParagraph( QTextParagraph *p ) { parag = p; }
    QTextParagraph *paragraph() const { return parag; }

    QTextDocument *parent;
    QTextParagraph *parag;

    virtual void pageBreak( int  y, QTextFlow* flow );
};
#endif

#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
Q_TEMPLATE_EXTERN template class Q_EXPORT QMap<QString, QString>;
// MOC_SKIP_END
#endif

#ifndef QT_NO_TEXTCUSTOMITEM
class Q_EXPORT QTextImage : public QTextCustomItem
{
public:
    QTextImage( QTextDocument *p, const QMap<QString, QString> &attr, const QString& context,
		QMimeSourceFactory &factory );
    virtual ~QTextImage();

    Placement placement() const { return place; }
    void adjustToPainter( QPainter* );
    int minimumWidth() const { return width; }

    QString richText() const;

    void draw( QPainter* p, int x, int y, int cx, int cy, int cw, int ch, const QColorGroup& cg, bool selected );

private:
    QRegion* reg;
    QPixmap pm;
    Placement place;
    int tmpwidth, tmpheight;
    QMap<QString, QString> attributes;
    QString imgId;

};
#endif

#ifndef QT_NO_TEXTCUSTOMITEM
class Q_EXPORT QTextHorizontalLine : public QTextCustomItem
{
public:
    QTextHorizontalLine( QTextDocument *p, const QMap<QString, QString> &attr, const QString& context,
			 QMimeSourceFactory &factory );
    virtual ~QTextHorizontalLine();

    void adjustToPainter( QPainter* );
    void draw(QPainter* p, int x, int y, int cx, int cy, int cw, int ch, const QColorGroup& cg, bool selected );
    QString richText() const;

    bool ownLine() const { return TRUE; }

private:
    int tmpheight;
    QColor color;
    bool shade;

};
#endif

#ifndef QT_NO_TEXTCUSTOMITEM
#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
Q_TEMPLATE_EXTERN template class Q_EXPORT QPtrList<QTextCustomItem>;
// MOC_SKIP_END
#endif
#endif

class Q_EXPORT QTextFlow
{
    friend class QTextDocument;
#ifndef QT_NO_TEXTCUSTOMITEM
    friend class QTextTableCell;
#endif

public:
    QTextFlow();
    virtual ~QTextFlow();

    virtual void setWidth( int width );
    int width() const;

    virtual void setPageSize( int ps );
    int pageSize() const { return pagesize; }

    virtual int adjustLMargin( int yp, int h, int margin, int space );
    virtual int adjustRMargin( int yp, int h, int margin, int space );

#ifndef QT_NO_TEXTCUSTOMITEM
    virtual void registerFloatingItem( QTextCustomItem* item );
    virtual void unregisterFloatingItem( QTextCustomItem* item );
#endif
    virtual QRect boundingRect() const;
    virtual void drawFloatingItems(QPainter* p, int cx, int cy, int cw, int ch, const QColorGroup& cg, bool selected );

    virtual int adjustFlow( int  y, int w, int h ); // adjusts y according to the defined pagesize. Returns the shift.

    virtual bool isEmpty();

    void clear();

private:
    int w;
    int pagesize;

#ifndef QT_NO_TEXTCUSTOMITEM
    QPtrList<QTextCustomItem> leftItems;
    QPtrList<QTextCustomItem> rightItems;
#endif
};

inline int QTextFlow::width() const { return w; }

#ifndef QT_NO_TEXTCUSTOMITEM
class QTextTable;

class Q_EXPORT QTextTableCell : public QLayoutItem
{
    friend class QTextTable;

public:
    QTextTableCell( QTextTable* table,
		    int row, int column,
		    const QMap<QString, QString> &attr,
		    const QStyleSheetItem* style,
		    const QTextFormat& fmt, const QString& context,
		    QMimeSourceFactory &factory, QStyleSheet *sheet, const QString& doc );
    virtual ~QTextTableCell();

    QSize sizeHint() const ;
    QSize minimumSize() const ;
    QSize maximumSize() const ;
    QSizePolicy::ExpandData expanding() const;
    bool isEmpty() const;
    void setGeometry( const QRect& ) ;
    QRect geometry() const;

    bool hasHeightForWidth() const;
    int heightForWidth( int ) const;

    void adjustToPainter( QPainter* );

    int row() const { return row_; }
    int column() const { return col_; }
    int rowspan() const { return rowspan_; }
    int colspan() const { return colspan_; }
    int stretch() const { return stretch_; }

    QTextDocument* richText()  const { return richtext; }
    QTextTable* table() const { return parent; }

    void draw( QPainter* p, int x, int y, int cx, int cy, int cw, int ch, const QColorGroup& cg, bool selected );

    QBrush *backGround() const { return background; }
    virtual void invalidate();

    int verticalAlignmentOffset() const;
    int horizontalAlignmentOffset() const;

private:
    QRect geom;
    QTextTable* parent;
    QTextDocument* richtext;
    int row_;
    int col_;
    int rowspan_;
    int colspan_;
    int stretch_;
    int maxw;
    int minw;
    bool hasFixedWidth;
    QBrush *background;
    int cached_width;
    int cached_sizehint;
    QMap<QString, QString> attributes;
    int align;
};
#endif

#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
Q_TEMPLATE_EXTERN template class Q_EXPORT QPtrList<QTextTableCell>;
Q_TEMPLATE_EXTERN template class Q_EXPORT QMap<QTextCursor*, int>;
// MOC_SKIP_END
#endif

#ifndef QT_NO_TEXTCUSTOMITEM
class Q_EXPORT QTextTable: public QTextCustomItem
{
    friend class QTextTableCell;

public:
    QTextTable( QTextDocument *p, const QMap<QString, QString> &attr );
    virtual ~QTextTable();

    void adjustToPainter( QPainter *p );
    void pageBreak( int  y, QTextFlow* flow );
    void draw( QPainter* p, int x, int y, int cx, int cy, int cw, int ch,
	       const QColorGroup& cg, bool selected );

    bool noErase() const { return TRUE; }
    bool ownLine() const { return TRUE; }
    Placement placement() const { return place; }
    bool isNested() const { return TRUE; }
    void resize( int nwidth );
    virtual void invalidate();

    virtual bool enter( QTextCursor *c, QTextDocument *&doc, QTextParagraph *&parag, int &idx, int &ox, int &oy, bool atEnd = FALSE );
    virtual bool enterAt( QTextCursor *c, QTextDocument *&doc, QTextParagraph *&parag, int &idx, int &ox, int &oy, const QPoint &pos );
    virtual bool next( QTextCursor *c, QTextDocument *&doc, QTextParagraph *&parag, int &idx, int &ox, int &oy );
    virtual bool prev( QTextCursor *c, QTextDocument *&doc, QTextParagraph *&parag, int &idx, int &ox, int &oy );
    virtual bool down( QTextCursor *c, QTextDocument *&doc, QTextParagraph *&parag, int &idx, int &ox, int &oy );
    virtual bool up( QTextCursor *c, QTextDocument *&doc, QTextParagraph *&parag, int &idx, int &ox, int &oy );

    QString richText() const;

    int minimumWidth() const;

    QPtrList<QTextTableCell> tableCells() const { return cells; }

    bool isStretching() const { return stretch; }

private:
    void format( int w );
    void addCell( QTextTableCell* cell );

private:
    QGridLayout* layout;
    QPtrList<QTextTableCell> cells;
    int cachewidth;
    int fixwidth;
    int cellpadding;
    int cellspacing;
    int border;
    int outerborder;
    int stretch;
    int innerborder;
    int us_cp, us_ib, us_b, us_ob, us_cs;
    QMap<QString, QString> attributes;
    QMap<QTextCursor*, int> currCell;
    Placement place;
    void adjustCells( int y , int shift );
    int pageBreakFor;
};
#endif
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef QT_NO_TEXTCUSTOMITEM
class QTextTableCell;
class QTextParagraph;
#endif

struct Q_EXPORT QTextDocumentSelection
{
    QTextCursor startCursor, endCursor;
    bool swapped;
};

#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
Q_TEMPLATE_EXTERN template class Q_EXPORT QMap<int, QColor>;
Q_TEMPLATE_EXTERN template class Q_EXPORT QMap<int, bool>;
Q_TEMPLATE_EXTERN template class Q_EXPORT QMap<int, QTextDocumentSelection>;
Q_TEMPLATE_EXTERN template class Q_EXPORT QPtrList<QTextDocument>;
// MOC_SKIP_END
#endif

class Q_EXPORT QTextDocument : public QObject
{
    Q_OBJECT

#ifndef QT_NO_TEXTCUSTOMITEM
    friend class QTextTableCell;
#endif
    friend class QTextCursor;
    friend class QTextEdit;
    friend class QTextParagraph;

public:
    enum SelectionIds {
	Standard = 0,
	IMSelectionText		= 31998,
	IMCompositionText	= 31999, // this must be higher!
	Temp = 32000 // This selection must not be drawn, it's used e.g. by undo/redo to
	// remove multiple lines with removeSelectedText()
    };

    QTextDocument( QTextDocument *p );
    QTextDocument( QTextDocument *d, QTextFormatCollection *f );
    virtual ~QTextDocument();

    QTextDocument *parent() const { return par; }
    QTextParagraph *parentParagraph() const { return parentPar; }

    void setText( const QString &text, const QString &context );
    QMap<QString, QString> attributes() const { return attribs; }
    void setAttributes( const QMap<QString, QString> &attr ) { attribs = attr; }

    QString text() const;
    QString text( int parag ) const;
    QString originalText() const;

    int x() const;
    int y() const;
    int width() const;
    int widthUsed() const;
    int visibleWidth() const;
    int height() const;
    void setWidth( int w );
    int minimumWidth() const;
    bool setMinimumWidth( int needed, int used = -1, QTextParagraph *parag = 0 );

    void setY( int y );
    int leftMargin() const;
    void setLeftMargin( int lm );
    int rightMargin() const;
    void setRightMargin( int rm );

    QTextParagraph *firstParagraph() const;
    QTextParagraph *lastParagraph() const;
    void setFirstParagraph( QTextParagraph *p );
    void setLastParagraph( QTextParagraph *p );

    void invalidate();

    void setPreProcessor( QTextPreProcessor *sh );
    QTextPreProcessor *preProcessor() const;

    void setFormatter( QTextFormatter *f );
    QTextFormatter *formatter() const;

    void setIndent( QTextIndent *i );
    QTextIndent *indent() const;

    QColor selectionColor( int id ) const;
    bool invertSelectionText( int id ) const;
    void setSelectionColor( int id, const QColor &c );
    void setInvertSelectionText( int id, bool b );
    bool hasSelection( int id, bool visible = FALSE ) const;
    void setSelectionStart( int id, const QTextCursor &cursor );
    bool setSelectionEnd( int id, const QTextCursor &cursor );
    void selectAll( int id );
    bool removeSelection( int id );
    void selectionStart( int id, int &paragId, int &index );
    QTextCursor selectionStartCursor( int id );
    QTextCursor selectionEndCursor( int id );
    void selectionEnd( int id, int &paragId, int &index );
    void setFormat( int id, QTextFormat *f, int flags );
    int numSelections() const { return nSelections; }
    void addSelection( int id );

    QString selectedText( int id, bool asRichText = FALSE ) const;
    void removeSelectedText( int id, QTextCursor *cursor );
    void indentSelection( int id );

    QTextParagraph *paragAt( int i ) const;

    void addCommand( QTextCommand *cmd );
    QTextCursor *undo( QTextCursor *c = 0 );
    QTextCursor *redo( QTextCursor *c  = 0 );
    QTextCommandHistory *commands() const { return commandHistory; }

    QTextFormatCollection *formatCollection() const;

    bool find( QTextCursor &cursor, const QString &expr, bool cs, bool wo, bool forward);

    void setTextFormat( Qt::TextFormat f );
    Qt::TextFormat textFormat() const;

    bool inSelection( int selId, const QPoint &pos ) const;

    QStyleSheet *styleSheet() const { return sheet_; }
#ifndef QT_NO_MIME
    QMimeSourceFactory *mimeSourceFactory() const { return factory_; }
#endif
    QString context() const { return contxt; }

    void setStyleSheet( QStyleSheet *s );
    void setDefaultFormat( const QFont &font, const QColor &color );
#ifndef QT_NO_MIME
    void setMimeSourceFactory( QMimeSourceFactory *f ) { if ( f ) factory_ = f; }
#endif
    void setContext( const QString &c ) { if ( !c.isEmpty() ) contxt = c; }

    void setUnderlineLinks( bool b );
    bool underlineLinks() const { return underlLinks; }

    void setPaper( QBrush *brush ) { if ( backBrush ) delete backBrush; backBrush = brush; }
    QBrush *paper() const { return backBrush; }

    void doLayout( QPainter *p, int w );
    void draw( QPainter *p, const QRect& rect, const QColorGroup &cg, const QBrush *paper = 0 );
    void drawParagraph( QPainter *p, QTextParagraph *parag, int cx, int cy, int cw, int ch,
		    QPixmap *&doubleBuffer, const QColorGroup &cg,
		    bool drawCursor, QTextCursor *cursor, bool resetChanged = TRUE );
    QTextParagraph *draw( QPainter *p, int cx, int cy, int cw, int ch, const QColorGroup &cg,
		      bool onlyChanged = FALSE, bool drawCursor = FALSE, QTextCursor *cursor = 0,
		      bool resetChanged = TRUE );

#ifndef QT_NO_TEXTCUSTOMITEM
    void registerCustomItem( QTextCustomItem *i, QTextParagraph *p );
    void unregisterCustomItem( QTextCustomItem *i, QTextParagraph *p );
#endif

    void setFlow( QTextFlow *f );
    void takeFlow();
    QTextFlow *flow() const { return flow_; }
    bool isPageBreakEnabled() const { return pages; }
    void setPageBreakEnabled( bool b ) { pages = b; }

    void setUseFormatCollection( bool b ) { useFC = b; }
    bool useFormatCollection() const { return useFC; }

#ifndef QT_NO_TEXTCUSTOMITEM
    QTextTableCell *tableCell() const { return tc; }
    void setTableCell( QTextTableCell *c ) { tc = c; }
#endif

    void setPlainText( const QString &text );
    void setRichText( const QString &text, const QString &context );
    QString richText() const;
    QString plainText() const;

    bool focusNextPrevChild( bool next );

    int alignment() const;
    void setAlignment( int a );

    int *tabArray() const;
    int tabStopWidth() const;
    void setTabArray( int *a );
    void setTabStops( int tw );

    void setUndoDepth( int d ) { commandHistory->setUndoDepth( d ); }
    int undoDepth() const { return commandHistory->undoDepth(); }

    int length() const;
    void clear( bool createEmptyParag = FALSE );

    virtual QTextParagraph *createParagraph( QTextDocument *d, QTextParagraph *pr = 0, QTextParagraph *nx = 0, bool updateIds = TRUE );
    void insertChild( QObject *o ) { QObject::insertChild( o ); }
    void removeChild( QObject *o ) { QObject::removeChild( o ); }
    void insertChild( QTextDocument *d ) { childList.append( d ); }
    void removeChild( QTextDocument *d ) { childList.removeRef( d ); }
    QPtrList<QTextDocument> children() const { return childList; }

    bool hasFocusParagraph() const;
    QString focusHref() const;
    QString focusName() const;

    void invalidateOriginalText() { oTextValid = FALSE; oText = ""; }

signals:
    void minimumWidthChanged( int );

private:
    void init();
    QPixmap *bufferPixmap( const QSize &s );
    // HTML parser
    bool hasPrefix(const QChar* doc, int length, int pos, QChar c);
    bool hasPrefix(const QChar* doc, int length, int pos, const QString& s);
#ifndef QT_NO_TEXTCUSTOMITEM
    QTextCustomItem* parseTable( const QMap<QString, QString> &attr, const QTextFormat &fmt,
				 const QChar* doc, int length, int& pos, QTextParagraph *curpar );
#endif
    bool eatSpace(const QChar* doc, int length, int& pos, bool includeNbsp = FALSE );
    bool eat(const QChar* doc, int length, int& pos, QChar c);
    QString parseOpenTag(const QChar* doc, int length, int& pos, QMap<QString, QString> &attr, bool& emptyTag);
    QString parseCloseTag( const QChar* doc, int length, int& pos );
    QChar parseHTMLSpecialChar(const QChar* doc, int length, int& pos);
    QString parseWord(const QChar* doc, int length, int& pos, bool lower = TRUE);
    QChar parseChar(const QChar* doc, int length, int& pos, QStyleSheetItem::WhiteSpaceMode wsm );
    void setRichTextInternal( const QString &text, QTextCursor* cursor = 0 );
    void setRichTextMarginsInternal( QPtrList< QPtrVector<QStyleSheetItem> >& styles, QTextParagraph* stylesPar );

private:
    struct Q_EXPORT Focus {
	QTextParagraph *parag;
	int start, len;
	QString href;
	QString name;
    };

    int cx, cy, cw, vw;
    QTextParagraph *fParag, *lParag;
    QTextPreProcessor *pProcessor;
    QMap<int, QColor> selectionColors;
    QMap<int, QTextDocumentSelection> selections;
    QMap<int, bool> selectionText;
    QTextCommandHistory *commandHistory;
    QTextFormatter *pFormatter;
    QTextIndent *indenter;
    QTextFormatCollection *fCollection;
    Qt::TextFormat txtFormat;
    uint preferRichText : 1;
    uint pages : 1;
    uint useFC : 1;
    uint withoutDoubleBuffer : 1;
    uint underlLinks : 1;
    uint nextDoubleBuffered : 1;
    uint oTextValid : 1;
    uint mightHaveCustomItems : 1;
    int align;
    int nSelections;
    QTextFlow *flow_;
    QTextDocument *par;
    QTextParagraph *parentPar;
#ifndef QT_NO_TEXTCUSTOMITEM
    QTextTableCell *tc;
#endif
    QBrush *backBrush;
    QPixmap *buf_pixmap;
    Focus focusIndicator;
    int minw;
    int wused;
    int leftmargin;
    int rightmargin;
    QTextParagraph *minwParag, *curParag;
    QStyleSheet* sheet_;
#ifndef QT_NO_MIME
    QMimeSourceFactory* factory_;
#endif
    QString contxt;
    QMap<QString, QString> attribs;
    int *tArray;
    int tStopWidth;
    int uDepth;
    QString oText;
    QPtrList<QTextDocument> childList;
    QColor linkColor;
    double scaleFontsFactor;

    short list_tm,list_bm, list_lm, li_tm, li_bm, par_tm, par_bm;
#if defined(Q_DISABLE_COPY) // Disabled copy constructor and operator=
    QTextDocument( const QTextDocument & );
    QTextDocument &operator=( const QTextDocument & );
#endif
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


class Q_EXPORT QTextDeleteCommand : public QTextCommand
{
public:
    QTextDeleteCommand( QTextDocument *d, int i, int idx, const QMemArray<QTextStringChar> &str,
			const QByteArray& oldStyle );
    QTextDeleteCommand( QTextParagraph *p, int idx, const QMemArray<QTextStringChar> &str );
    virtual ~QTextDeleteCommand();

    Commands type() const { return Delete; }
    QTextCursor *execute( QTextCursor *c );
    QTextCursor *unexecute( QTextCursor *c );

protected:
    int id, index;
    QTextParagraph *parag;
    QMemArray<QTextStringChar> text;
    QByteArray styleInformation;

};

class Q_EXPORT QTextInsertCommand : public QTextDeleteCommand
{
public:
    QTextInsertCommand( QTextDocument *d, int i, int idx, const QMemArray<QTextStringChar> &str,
			const QByteArray& oldStyleInfo )
	: QTextDeleteCommand( d, i, idx, str, oldStyleInfo ) {}
    QTextInsertCommand( QTextParagraph *p, int idx, const QMemArray<QTextStringChar> &str )
	: QTextDeleteCommand( p, idx, str ) {}
    virtual ~QTextInsertCommand() {}

    Commands type() const { return Insert; }
    QTextCursor *execute( QTextCursor *c ) { return QTextDeleteCommand::unexecute( c ); }
    QTextCursor *unexecute( QTextCursor *c ) { return QTextDeleteCommand::execute( c ); }

};

class Q_EXPORT QTextFormatCommand : public QTextCommand
{
public:
    QTextFormatCommand( QTextDocument *d, int sid, int sidx, int eid, int eidx, const QMemArray<QTextStringChar> &old, QTextFormat *f, int fl );
    virtual ~QTextFormatCommand();

    Commands type() const { return Format; }
    QTextCursor *execute( QTextCursor *c );
    QTextCursor *unexecute( QTextCursor *c );

protected:
    int startId, startIndex, endId, endIndex;
    QTextFormat *format;
    QMemArray<QTextStringChar> oldFormats;
    int flags;

};

class Q_EXPORT QTextStyleCommand : public QTextCommand
{
public:
    QTextStyleCommand( QTextDocument *d, int fParag, int lParag, const QByteArray& beforeChange  );
    virtual ~QTextStyleCommand() {}

    Commands type() const { return Style; }
    QTextCursor *execute( QTextCursor *c );
    QTextCursor *unexecute( QTextCursor *c );

    static QByteArray readStyleInformation(  QTextDocument* d, int fParag, int lParag );
    static void writeStyleInformation(  QTextDocument* d, int fParag, const QByteArray& style );

private:
    int firstParag, lastParag;
    QByteArray before;
    QByteArray after;
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

struct Q_EXPORT QTextParagraphSelection
{
    int start, end;
};

struct Q_EXPORT QTextLineStart
{
    QTextLineStart() : y( 0 ), baseLine( 0 ), h( 0 )
#ifndef QT_NO_COMPLEXTEXT
	, bidicontext( 0 )
#endif
    {  }
    QTextLineStart( ushort y_, ushort bl, ushort h_ ) : y( y_ ), baseLine( bl ), h( h_ ),
	w( 0 )
#ifndef QT_NO_COMPLEXTEXT
	, bidicontext( 0 )
#endif
    {  }
#ifndef QT_NO_COMPLEXTEXT
    QTextLineStart( QBidiContext *c, QBidiStatus s ) : y(0), baseLine(0), h(0),
	status( s ), bidicontext( c ) { if ( bidicontext ) bidicontext->ref(); }
#endif

    virtual ~QTextLineStart()
    {
#ifndef QT_NO_COMPLEXTEXT
	if ( bidicontext && bidicontext->deref() )
	    delete bidicontext;
#endif
    }

#ifndef QT_NO_COMPLEXTEXT
    void setContext( QBidiContext *c ) {
	if ( c == bidicontext )
	    return;
	if ( bidicontext && bidicontext->deref() )
	    delete bidicontext;
	bidicontext = c;
	if ( bidicontext )
	    bidicontext->ref();
    }
    QBidiContext *context() const { return bidicontext; }
#endif

public:
    ushort y, baseLine, h;
#ifndef QT_NO_COMPLEXTEXT
    QBidiStatus status;
#endif
    int w;

private:
#ifndef QT_NO_COMPLEXTEXT
    QBidiContext *bidicontext;
#endif
};

#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
Q_TEMPLATE_EXTERN template class Q_EXPORT QMap<int, QTextParagraphSelection>;
Q_TEMPLATE_EXTERN template class Q_EXPORT QMap<int, QTextLineStart*>;
// MOC_SKIP_END
#endif

class Q_EXPORT QTextParagraphData
{
public:
    QTextParagraphData() {}
    virtual ~QTextParagraphData();
    virtual void join( QTextParagraphData * );
};

class Q_EXPORT QTextParagraphPseudoDocument
{
public:
    QTextParagraphPseudoDocument();
    ~QTextParagraphPseudoDocument();
    QRect docRect;
    QTextFormatter *pFormatter;
    QTextCommandHistory *commandHistory;
    int minw;
    int wused;
};

//nase
class Q_EXPORT QTextParagraph
{
    friend class QTextDocument;
    friend class QTextCursor;

public:
    QTextParagraph( QTextDocument *d, QTextParagraph *pr = 0, QTextParagraph *nx = 0, bool updateIds = TRUE );
    virtual ~QTextParagraph();

    QTextString *string() const;
    QTextStringChar *at( int i ) const; // maybe remove later
    int leftGap() const;
    int length() const; // maybe remove later

    void setListStyle( QStyleSheetItem::ListStyle ls ) { lstyle = ls; changed = TRUE; }
    QStyleSheetItem::ListStyle listStyle() const { return lstyle; }
    void setListItem( bool li );
    bool isListItem() const { return litem; }
    void setListValue( int v ) { list_val = v; }
    int listValue() const { return list_val > 0 ? list_val : -1; }

    void setListDepth( int depth );
    int listDepth() const { return ldepth; }

//     void setFormat( QTextFormat *fm );
//     QTextFormat *paragFormat() const;

    QTextDocument *document() const;
    QTextParagraphPseudoDocument *pseudoDocument() const;

    QRect rect() const;
    void setHeight( int h ) { r.setHeight( h ); }
    void show();
    void hide();
    bool isVisible() const { return visible; }

    QTextParagraph *prev() const;
    QTextParagraph *next() const;
    void setPrev( QTextParagraph *s );
    void setNext( QTextParagraph *s );

    void insert( int index, const QString &s );
    void insert( int index, const QChar *unicode, int len );
    void append( const QString &s, bool reallyAtEnd = FALSE );
    void truncate( int index );
    void remove( int index, int len );
    void join( QTextParagraph *s );

    void invalidate( int chr );

    void move( int &dy );
    void format( int start = -1, bool doMove = TRUE );

    bool isValid() const;
    bool hasChanged() const;
    void setChanged( bool b, bool recursive = FALSE );

    int lineHeightOfChar( int i, int *bl = 0, int *y = 0 ) const;
    QTextStringChar *lineStartOfChar( int i, int *index = 0, int *line = 0 ) const;
    int lines() const;
    QTextStringChar *lineStartOfLine( int line, int *index = 0 ) const;
    int lineY( int l ) const;
    int lineBaseLine( int l ) const;
    int lineHeight( int l ) const;
    void lineInfo( int l, int &y, int &h, int &bl ) const;

    void setSelection( int id, int start, int end );
    void removeSelection( int id );
    int selectionStart( int id ) const;
    int selectionEnd( int id ) const;
    bool hasSelection( int id ) const;
    bool hasAnySelection() const;
    bool fullSelected( int id ) const;

    void setEndState( int s );
    int endState() const;

    void setParagId( int i );
    int paragId() const;

    bool firstPreProcess() const;
    void setFirstPreProcess( bool b );

    void indent( int *oldIndent = 0, int *newIndent = 0 );

    void setExtraData( QTextParagraphData *data );
    QTextParagraphData *extraData() const;

    QMap<int, QTextLineStart*> &lineStartList();

    void setFormat( int index, int len, QTextFormat *f, bool useCollection = TRUE, int flags = -1 );

    void setAlignment( int a );
    int alignment() const;

    virtual void paint( QPainter &painter, const QColorGroup &cg, QTextCursor *cursor = 0, bool drawSelections = FALSE,
			int clipx = -1, int clipy = -1, int clipw = -1, int cliph = -1 );

    virtual int topMargin() const;
    virtual int bottomMargin() const;
    virtual int leftMargin() const;
    virtual int firstLineMargin() const;
    virtual int rightMargin() const;
    virtual int lineSpacing() const;

#ifndef QT_NO_TEXTCUSTOMITEM
    void registerFloatingItem( QTextCustomItem *i );
    void unregisterFloatingItem( QTextCustomItem *i );
#endif

    void setFullWidth( bool b ) { fullWidth = b; }
    bool isFullWidth() const { return fullWidth; }

#ifndef QT_NO_TEXTCUSTOMITEM
    QTextTableCell *tableCell() const;
#endif

    QBrush *background() const;

    int documentWidth() const;
    int documentVisibleWidth() const;
    int documentX() const;
    int documentY() const;
    QTextFormatCollection *formatCollection() const;
    QTextFormatter *formatter() const;

    virtual int nextTab( int i, int x );
    int *tabArray() const;
    void setTabArray( int *a );
    void setTabStops( int tw );

    void adjustToPainter( QPainter *p );

    void setNewLinesAllowed( bool b );
    bool isNewLinesAllowed() const;

    QString richText() const;

    void addCommand( QTextCommand *cmd );
    QTextCursor *undo( QTextCursor *c = 0 );
    QTextCursor *redo( QTextCursor *c  = 0 );
    QTextCommandHistory *commands() const;
    virtual void copyParagData( QTextParagraph *parag );

    void setBreakable( bool b ) { breakable = b; }
    bool isBreakable() const { return breakable; }

    void setBackgroundColor( const QColor &c );
    QColor *backgroundColor() const { return bgcol; }
    void clearBackgroundColor();

    void setMovedDown( bool b ) { movedDown = b; }
    bool wasMovedDown() const { return movedDown; }

    void setDirection( QChar::Direction d );
    QChar::Direction direction() const;
    void setPaintDevice( QPaintDevice *pd ) { paintdevice = pd; }

    void readStyleInformation( QDataStream& stream );
    void writeStyleInformation( QDataStream& stream ) const;

protected:
    virtual void setColorForSelection( QColor &c, QPainter &p, const QColorGroup& cg, int selection );
    virtual void drawLabel( QPainter* p, int x, int y, int w, int h, int base, const QColorGroup& cg );
    virtual void drawString( QPainter &painter, const QString &str, int start, int len, int xstart,
			     int y, int baseLine, int w, int h, int selection,
			     QTextStringChar *formatChar, const QColorGroup& cg,
			     bool rightToLeft );

private:
    QMap<int, QTextParagraphSelection> &selections() const;
#ifndef QT_NO_TEXTCUSTOMITEM
    QPtrList<QTextCustomItem> &floatingItems() const;
#endif
    QBrush backgroundBrush( const QColorGroup&cg ) { if ( bgcol ) return *bgcol; return cg.brush( QColorGroup::Base ); }
    void invalidateStyleCache();

    QMap<int, QTextLineStart*> lineStarts;
    int invalid;
    QRect r;
    QTextParagraph *p, *n;
    void *docOrPseudo;
    uint changed : 1;
    uint firstFormat : 1;
    uint firstPProcess : 1;
    uint needPreProcess : 1;
    uint fullWidth : 1;
    uint lastInFrame : 1;
    uint visible : 1;
    uint breakable : 1;
    uint movedDown : 1;
    uint mightHaveCustomItems : 1;
    uint hasdoc : 1;
    uint litem : 1; // whether the paragraph is a list item
    uint rtext : 1; // whether the paragraph needs rich text margin
    int align : 4;
    int state, id;
    QTextString *str;
    QMap<int, QTextParagraphSelection> *mSelections;
#ifndef QT_NO_TEXTCUSTOMITEM
    QPtrList<QTextCustomItem> *mFloatingItems;
#endif
    QStyleSheetItem::ListStyle lstyle;
    short utm, ubm, ulm, urm, uflm, ulinespacing;
    int *tArray;
    short tabStopWidth;
    QTextParagraphData *eData;
    short list_val;
    QColor *bgcol;
    ushort ldepth;
    QPaintDevice *paintdevice;
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class Q_EXPORT QTextFormatter
{
public:
    QTextFormatter();
    virtual ~QTextFormatter();

    virtual int format( QTextDocument *doc, QTextParagraph *parag, int start, const QMap<int, QTextLineStart*> &oldLineStarts ) = 0;
    virtual int formatVertically( QTextDocument* doc, QTextParagraph* parag );

    bool isWrapEnabled( QTextParagraph *p ) const { if ( !wrapEnabled ) return FALSE; if ( p && !p->isBreakable() ) return FALSE; return TRUE;}
    int wrapAtColumn() const { return wrapColumn;}
    virtual void setWrapEnabled( bool b );
    virtual void setWrapAtColumn( int c );
    virtual void setAllowBreakInWords( bool b ) { biw = b; }
    bool allowBreakInWords() const { return biw; }

    int minimumWidth() const { return thisminw; }
    int widthUsed() const { return thiswused; }

    static bool isBreakable( QTextString *string, int pos );

protected:
    virtual QTextLineStart *formatLine( QTextParagraph *parag, QTextString *string, QTextLineStart *line, QTextStringChar *start,
					       QTextStringChar *last, int align = Qt::AlignAuto, int space = 0 );
#ifndef QT_NO_COMPLEXTEXT
    virtual QTextLineStart *bidiReorderLine( QTextParagraph *parag, QTextString *string, QTextLineStart *line, QTextStringChar *start,
						    QTextStringChar *last, int align, int space );
#endif
    void insertLineStart( QTextParagraph *parag, int index, QTextLineStart *ls );

    int thisminw;
    int thiswused;

private:
    bool wrapEnabled;
    int wrapColumn;
    bool biw;

#ifdef HAVE_THAI_BREAKS
    static QCString *thaiCache;
    static QTextString *cachedString;
    static ThBreakIterator *thaiIt;
#endif
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class Q_EXPORT QTextFormatterBreakInWords : public QTextFormatter
{
public:
    QTextFormatterBreakInWords();
    virtual ~QTextFormatterBreakInWords() {}

    int format( QTextDocument *doc, QTextParagraph *parag, int start, const QMap<int, QTextLineStart*> &oldLineStarts );

};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class Q_EXPORT QTextFormatterBreakWords : public QTextFormatter
{
public:
    QTextFormatterBreakWords();
    virtual ~QTextFormatterBreakWords() {}

    int format( QTextDocument *doc, QTextParagraph *parag, int start, const QMap<int, QTextLineStart*> &oldLineStarts );

};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class Q_EXPORT QTextIndent
{
public:
    QTextIndent();
    virtual ~QTextIndent() {}

    virtual void indent( QTextDocument *doc, QTextParagraph *parag, int *oldIndent = 0, int *newIndent = 0 ) = 0;

};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class Q_EXPORT QTextPreProcessor
{
public:
    enum Ids {
	Standard = 0
    };

    QTextPreProcessor();
    virtual ~QTextPreProcessor() {}

    virtual void process( QTextDocument *doc, QTextParagraph *, int, bool = TRUE ) = 0;
    virtual QTextFormat *format( int id ) = 0;

};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class Q_EXPORT QTextFormat
{
    friend class QTextFormatCollection;
    friend class QTextDocument;

public:
    enum Flags {
	NoFlags,
	Bold = 1,
	Italic = 2,
	Underline = 4,
	Family = 8,
	Size = 16,
	Color = 32,
	Misspelled = 64,
	VAlign = 128,
	StrikeOut= 256,
	Font = Bold | Italic | Underline | Family | Size | StrikeOut,
	Format = Font | Color | Misspelled | VAlign
    };

    enum VerticalAlignment { AlignNormal, AlignSuperScript, AlignSubScript };

    QTextFormat();
    virtual ~QTextFormat();

    QTextFormat( const QStyleSheetItem *s );
    QTextFormat( const QFont &f, const QColor &c, QTextFormatCollection *parent = 0 );
    QTextFormat( const QTextFormat &fm );
    QTextFormat makeTextFormat( const QStyleSheetItem *style, const QMap<QString,QString>& attr, double scaleFontsFactor ) const;
    QTextFormat& operator=( const QTextFormat &fm );
    QColor color() const;
    QFont font() const;
    bool isMisspelled() const;
    VerticalAlignment vAlign() const;
    int minLeftBearing() const;
    int minRightBearing() const;
    int width( const QChar &c ) const;
    int width( const QString &str, int pos ) const;
    int height() const;
    int ascent() const;
    int descent() const;
    int leading() const;
    bool useLinkColor() const;

    void setBold( bool b );
    void setItalic( bool b );
    void setUnderline( bool b );
    void setStrikeOut( bool b );
    void setFamily( const QString &f );
    void setPointSize( int s );
    void setFont( const QFont &f );
    void setColor( const QColor &c );
    void setMisspelled( bool b );
    void setVAlign( VerticalAlignment a );

    bool operator==( const QTextFormat &f ) const;
    QTextFormatCollection *parent() const;
    const QString &key() const;

    static QString getKey( const QFont &f, const QColor &c, bool misspelled, VerticalAlignment vAlign );

    void addRef();
    void removeRef();

    QString makeFormatChangeTags( QTextFormat* defaultFormat, QTextFormat *f, const QString& oldAnchorHref, const QString& anchorHref ) const;
    QString makeFormatEndTags( QTextFormat* defaultFormat, const QString& anchorHref ) const;

    static void setPainter( QPainter *p );
    static QPainter* painter();

    bool fontSizesInPixels() { return usePixelSizes; }

protected:
    virtual void generateKey();

private:
    void update();
    static void applyFont( const QFont &f );

private:
    QFont fn;
    QColor col;
    QFontMetrics fm;
    uint missp : 1;
    uint linkColor : 1;
    uint usePixelSizes : 1;
    int leftBearing, rightBearing;
    VerticalAlignment ha;
    uchar widths[ 256 ];
    int hei, asc, dsc;
    QTextFormatCollection *collection;
    int ref;
    QString k;
    int logicalFontSize;
    int stdSize;
    static QPainter *pntr;
    static QFontMetrics *pntr_fm;
    static int pntr_asc;
    static int pntr_hei;
    static int pntr_ldg;
    static int pntr_dsc;

};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
Q_TEMPLATE_EXTERN template class Q_EXPORT QDict<QTextFormat>;
// MOC_SKIP_END
#endif

class Q_EXPORT QTextFormatCollection
{
    friend class QTextDocument;
    friend class QTextFormat;

public:
    QTextFormatCollection();
    virtual ~QTextFormatCollection();

    void setDefaultFormat( QTextFormat *f );
    QTextFormat *defaultFormat() const;
    virtual QTextFormat *format( QTextFormat *f );
    virtual QTextFormat *format( QTextFormat *of, QTextFormat *nf, int flags );
    virtual QTextFormat *format( const QFont &f, const QColor &c );
    virtual void remove( QTextFormat *f );
    virtual QTextFormat *createFormat( const QTextFormat &f ) { return new QTextFormat( f ); }
    virtual QTextFormat *createFormat( const QFont &f, const QColor &c ) { return new QTextFormat( f, c, this ); }

    void updateDefaultFormat( const QFont &font, const QColor &c, QStyleSheet *sheet );
    QDict<QTextFormat> dict() const { return cKey; }

    QPaintDevice *paintDevice() const { return paintdevice; }
    void setPaintDevice( QPaintDevice * );

private:
    void updateKeys();

private:
    QTextFormat *defFormat, *lastFormat, *cachedFormat;
    QDict<QTextFormat> cKey;
    QTextFormat *cres;
    QFont cfont;
    QColor ccol;
    QString kof, knf;
    int cflags;

    QPaintDevice *paintdevice;
};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline int QTextString::length() const
{
    return data.size();
}

inline int QTextParagraph::length() const
{
    return str->length();
}

inline QRect QTextParagraph::rect() const
{
    return r;
}

inline QTextParagraph *QTextCursor::paragraph() const
{
    return para;
}

inline int QTextCursor::index() const
{
    return idx;
}


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline int QTextDocument::x() const
{
    return cx;
}

inline int QTextDocument::y() const
{
    return cy;
}

inline int QTextDocument::width() const
{
    return QMAX( cw, flow_->width() );
}

inline int QTextDocument::visibleWidth() const
{
    return vw;
}

inline QTextParagraph *QTextDocument::firstParagraph() const
{
    return fParag;
}

inline QTextParagraph *QTextDocument::lastParagraph() const
{
    return lParag;
}

inline void QTextDocument::setFirstParagraph( QTextParagraph *p )
{
    fParag = p;
}

inline void QTextDocument::setLastParagraph( QTextParagraph *p )
{
    lParag = p;
}

inline void QTextDocument::setWidth( int w )
{
    cw = QMAX( w, minw );
    flow_->setWidth( cw );
    vw = w;
}

inline int QTextDocument::minimumWidth() const
{
    return minw;
}

inline void QTextDocument::setY( int y )
{
    cy = y;
}

inline int QTextDocument::leftMargin() const
{
    return leftmargin;
}

inline void QTextDocument::setLeftMargin( int lm )
{
    leftmargin = lm;
}

inline int QTextDocument::rightMargin() const
{
    return rightmargin;
}

inline void QTextDocument::setRightMargin( int rm )
{
    rightmargin = rm;
}

inline QTextPreProcessor *QTextDocument::preProcessor() const
{
    return pProcessor;
}

inline void QTextDocument::setPreProcessor( QTextPreProcessor * sh )
{
    pProcessor = sh;
}

inline void QTextDocument::setFormatter( QTextFormatter *f )
{
    delete pFormatter;
    pFormatter = f;
}

inline QTextFormatter *QTextDocument::formatter() const
{
    return pFormatter;
}

inline void QTextDocument::setIndent( QTextIndent *i )
{
    indenter = i;
}

inline QTextIndent *QTextDocument::indent() const
{
    return indenter;
}

inline QColor QTextDocument::selectionColor( int id ) const
{
    return selectionColors[ id ];
}

inline bool QTextDocument::invertSelectionText( int id ) const
{
    return selectionText[ id ];
}

inline void QTextDocument::setSelectionColor( int id, const QColor &c )
{
    selectionColors[ id ] = c;
}

inline void QTextDocument::setInvertSelectionText( int id, bool b )
{
    selectionText[ id ] = b;
}

inline QTextFormatCollection *QTextDocument::formatCollection() const
{
    return fCollection;
}

inline int QTextDocument::alignment() const
{
    return align;
}

inline void QTextDocument::setAlignment( int a )
{
    align = a;
}

inline int *QTextDocument::tabArray() const
{
    return tArray;
}

inline int QTextDocument::tabStopWidth() const
{
    return tStopWidth;
}

inline void QTextDocument::setTabArray( int *a )
{
    tArray = a;
}

inline void QTextDocument::setTabStops( int tw )
{
    tStopWidth = tw;
}

inline QString QTextDocument::originalText() const
{
    if ( oTextValid )
	return oText;
    return text();
}

inline void QTextDocument::setFlow( QTextFlow *f )
{
    if ( flow_ )
	delete flow_;
    flow_ = f;
}

inline void QTextDocument::takeFlow()
{
    flow_ = 0;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline QColor QTextFormat::color() const
{
    return col;
}

inline QFont QTextFormat::font() const
{
    return fn;
}

inline bool QTextFormat::isMisspelled() const
{
    return missp;
}

inline QTextFormat::VerticalAlignment QTextFormat::vAlign() const
{
    return ha;
}

inline bool QTextFormat::operator==( const QTextFormat &f ) const
{
    return k == f.k;
}

inline QTextFormatCollection *QTextFormat::parent() const
{
    return collection;
}

inline void QTextFormat::addRef()
{
    ref++;
}

inline void QTextFormat::removeRef()
{
    ref--;
    if ( !collection )
	return;
    if ( this == collection->defFormat )
	return;
    if ( ref == 0 )
	collection->remove( this );
}

inline const QString &QTextFormat::key() const
{
    return k;
}

inline bool QTextFormat::useLinkColor() const
{
    return linkColor;
}


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline QTextStringChar &QTextString::at( int i ) const
{
    return data[ i ];
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline QTextStringChar *QTextParagraph::at( int i ) const
{
    return &str->at( i );
}

inline bool QTextParagraph::isValid() const
{
    return invalid == -1;
}

inline bool QTextParagraph::hasChanged() const
{
    return changed;
}

inline void QTextParagraph::setBackgroundColor( const QColor & c )
{
    delete bgcol;
    bgcol = new QColor( c );
    setChanged( TRUE );
}

inline void QTextParagraph::clearBackgroundColor()
{
    delete bgcol; bgcol = 0; setChanged( TRUE );
}

inline void QTextParagraph::append( const QString &s, bool reallyAtEnd )
{
    if ( reallyAtEnd )
	insert( str->length(), s );
    else
	insert( QMAX( str->length() - 1, 0 ), s );
}

inline QTextParagraph *QTextParagraph::prev() const
{
    return p;
}

inline QTextParagraph *QTextParagraph::next() const
{
    return n;
}

inline bool QTextParagraph::hasAnySelection() const
{
    return mSelections ? !selections().isEmpty() : FALSE;
}

inline void QTextParagraph::setEndState( int s )
{
    if ( s == state )
	return;
    state = s;
}

inline int QTextParagraph::endState() const
{
    return state;
}

inline void QTextParagraph::setParagId( int i )
{
    id = i;
}

inline int QTextParagraph::paragId() const
{
    if ( id == -1 )
	qWarning( "invalid parag id!!!!!!!! (%p)", (void*)this );
    return id;
}

inline bool QTextParagraph::firstPreProcess() const
{
    return firstPProcess;
}

inline void QTextParagraph::setFirstPreProcess( bool b )
{
    firstPProcess = b;
}

inline QMap<int, QTextLineStart*> &QTextParagraph::lineStartList()
{
    return lineStarts;
}

inline QTextString *QTextParagraph::string() const
{
    return str;
}

inline QTextDocument *QTextParagraph::document() const
{
    if ( hasdoc )
	return (QTextDocument*) docOrPseudo;
    return 0;
}

inline QTextParagraphPseudoDocument *QTextParagraph::pseudoDocument() const
{
    if ( hasdoc )
	return 0;
    return (QTextParagraphPseudoDocument*) docOrPseudo;
}


#ifndef QT_NO_TEXTCUSTOMITEM
inline QTextTableCell *QTextParagraph::tableCell() const
{
    return hasdoc ? document()->tableCell () : 0;
}
#endif

inline QTextCommandHistory *QTextParagraph::commands() const
{
    return hasdoc ? document()->commands() : pseudoDocument()->commandHistory;
}


inline int QTextParagraph::alignment() const
{
    return align;
}

#ifndef QT_NO_TEXTCUSTOMITEM
inline void QTextParagraph::registerFloatingItem( QTextCustomItem *i )
{
    floatingItems().append( i );
}

inline void QTextParagraph::unregisterFloatingItem( QTextCustomItem *i )
{
    floatingItems().removeRef( i );
}
#endif

inline QBrush *QTextParagraph::background() const
{
#ifndef QT_NO_TEXTCUSTOMITEM
    return tableCell() ? tableCell()->backGround() : 0;
#else
    return 0;
#endif
}

inline int QTextParagraph::documentWidth() const
{
    return hasdoc ? document()->width() : pseudoDocument()->docRect.width();
}

inline int QTextParagraph::documentVisibleWidth() const
{
    return hasdoc ? document()->visibleWidth() : pseudoDocument()->docRect.width();
}

inline int QTextParagraph::documentX() const
{
    return hasdoc ? document()->x() : pseudoDocument()->docRect.x();
}

inline int QTextParagraph::documentY() const
{
    return hasdoc ? document()->y() : pseudoDocument()->docRect.y();
}

inline void QTextParagraph::setExtraData( QTextParagraphData *data )
{
    eData = data;
}

inline QTextParagraphData *QTextParagraph::extraData() const
{
    return eData;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline void QTextFormatCollection::setDefaultFormat( QTextFormat *f )
{
    defFormat = f;
}

inline QTextFormat *QTextFormatCollection::defaultFormat() const
{
    return defFormat;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

inline QTextFormat *QTextStringChar::format() const
{
    return (type == Regular) ? d.format : d.custom->format;
}


#ifndef QT_NO_TEXTCUSTOMITEM
inline QTextCustomItem *QTextStringChar::customItem() const
{
    return isCustom() ? d.custom->custom : 0;
}
#endif

inline int QTextStringChar::height() const
{
#ifndef QT_NO_TEXTCUSTOMITEM
    return !isCustom() ? format()->height() : ( customItem()->placement() == QTextCustomItem::PlaceInline ? customItem()->height : 0 );
#else
    return format()->height();
#endif
}

inline int QTextStringChar::ascent() const
{
#ifndef QT_NO_TEXTCUSTOMITEM
    return !isCustom() ? format()->ascent() : ( customItem()->placement() == QTextCustomItem::PlaceInline ? customItem()->ascent() : 0 );
#else
    return format()->ascent();
#endif
}

inline int QTextStringChar::descent() const
{
#ifndef QT_NO_TEXTCUSTOMITEM
    return !isCustom() ? format()->descent() : 0;
#else
    return format()->descent();
#endif
}

#endif //QT_NO_RICHTEXT

#endif
