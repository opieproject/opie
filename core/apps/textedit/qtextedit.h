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

#ifndef QTEXTEDIT_H
#define QTEXTEDIT_H

#include <qscrollview.h>
#include <qstylesheet.h>
#include <qpainter.h>

class QPainter;
class QTextDocument;
class QTextCursor;
class QKeyEvent;
class QResizeEvent;
class QMouseEvent;
class QTimer;
class QTextString;
class QVBox;
class QListBox;
class QTextCommand;
class QTextParag;
class QTextFormat;
class QFont;
class QColor;

class QTextEdit : public QScrollView
{
    Q_OBJECT

public:
    QTextEdit( QWidget *parent, const QString &fn, bool tabify = FALSE );
    QTextEdit( QWidget *parent = 0, const char *name = 0 );
    virtual ~QTextEdit();

#if defined(QTEXTEDIT_OPEN_API)
    QTextDocument *document() const;
    QTextCursor *textCursor() const;
#endif

    QString text() const;
    QString text( int parag, bool formatted = FALSE ) const;
    Qt::TextFormat textFormat() const;
    QString fileName() const;

    void cursorPosition( int &parag, int &index );
    void selection( int &parag_from, int &index_from,
		    int &parag_to, int &index_to );
    virtual bool find( const QString &expr, bool cs, bool wo, bool forward = TRUE,
		       int *parag = 0, int *index = 0 );
    void insert( const QString &text, bool indent = FALSE, bool checkNewLine = FALSE );

    int paragraphs() const;
    int lines() const;
    int linesOfParagraph( int parag ) const;
    int lineOfChar( int parag, int chr );

    bool isModified() const;

    bool italic() const;
    bool bold() const;
    bool underline() const;
    QString family() const;
    int pointSize() const;
    QColor color() const;
    QFont font() const;
    int alignment() const;
    int maxLines() const;

    const QStyleSheet* styleSheet() const;
    void setStyleSheet( const QStyleSheet* styleSheet );

    void setPaper( const QBrush& pap);
    QBrush paper() const;

    void setLinkColor( const QColor& );
    QColor linkColor() const;

    void setLinkUnderline( bool );
    bool linkUnderline() const;

    void setMimeSourceFactory( const QMimeSourceFactory* factory );
    const QMimeSourceFactory* mimeSourceFactory() const;

    int heightForWidth( int w ) const;

    void append( const QString& text );

    bool hasSelectedText() const;
    QString selectedText() const;

    QString context() const;

    QString documentTitle() const;

    void scrollToAnchor( const QString& name );
    QString anchorAt(const QPoint& pos);

public slots:
    virtual void undo();
    virtual void redo();

    virtual void cut();
    virtual void copy();
    virtual void paste();

    virtual void indent();

    virtual void setItalic( bool b );
    virtual void setBold( bool b );
    virtual void setUnderline( bool b );
    virtual void setFamily( const QString &f );
    virtual void setPointSize( int s );
    virtual void setColor( const QColor &c );
    virtual void setFont( const QFont &f );

    virtual void setAlignment( int );

    virtual void setParagType( QStyleSheetItem::DisplayMode, int listStyle );

    virtual void setTextFormat( Qt::TextFormat f );
    virtual void setText( const QString &txt, const QString &context = QString::null ) { setText( txt, context, FALSE ); }
    virtual void setText( const QString &txt, const QString &context, bool tabify );

    virtual void load( const QString &fn ) { load( fn, FALSE ); }
    virtual void load( const QString &fn, bool tabify );
    virtual void save( bool untabify = FALSE ) { save( QString::null, untabify ); }
    virtual void save( const QString &fn, bool untabify = FALSE  );

    virtual void setCursorPosition( int parag, int index );
    virtual void setSelection( int parag_from, int index_from,
			       int parag_to, int index_to );

    virtual void setModified( bool m );
    virtual void selectAll( bool select );

    virtual void setMaxLines( int l );
    virtual void resetFormat();

signals:
    void currentFontChanged( const QFont &f );
    void currentColorChanged( const QColor &c );
    void currentAlignmentChanged( int );
    void textChanged();
    void highlighted( const QString& );
    void linkClicked( const QString& );

protected:
    void setFormat( QTextFormat *f, int flags );
    void drawContents( QPainter *p, int cx, int cy, int cw, int ch );
    void keyPressEvent( QKeyEvent *e );
    void resizeEvent( QResizeEvent *e );
    void contentsMousePressEvent( QMouseEvent *e );
    void contentsMouseMoveEvent( QMouseEvent *e );
    void contentsMouseReleaseEvent( QMouseEvent *e );
    void contentsMouseDoubleClickEvent( QMouseEvent *e );
#ifndef QT_NO_DRAGANDDROP
    void contentsDragEnterEvent( QDragEnterEvent *e );
    void contentsDragMoveEvent( QDragMoveEvent *e );
    void contentsDragLeaveEvent( QDragLeaveEvent *e );
    void contentsDropEvent( QDropEvent *e );
#endif
    bool eventFilter( QObject *o, QEvent *e );
    bool focusNextPrevChild( bool next );
#if !defined(QTEXTEDIT_OPEN_API)
    QTextDocument *document() const;
    QTextCursor *textCursor() const;
#endif

private slots:
    void formatMore();
    void doResize();
    void doAutoScroll();
    void doChangeInterval();
    void blinkCursor();
    void setModified();
    void startDrag();

private:
    enum MoveDirection {
	MoveLeft,
	MoveRight,
	MoveUp,
	MoveDown,
	MoveHome,
	MoveEnd,
	MovePgUp,
	MovePgDown
    };
    enum KeyboardAction {
	ActionBackspace,
	ActionDelete,
	ActionReturn
    };

    struct UndoRedoInfo {
	enum Type { Invalid, Insert, Delete, Backspace, Return, RemoveSelected };
	UndoRedoInfo( QTextDocument *d ) : type( Invalid ), doc( d )
	{ text = QString::null; id = -1; index = -1; }
	void clear();
	inline bool valid() const { return !text.isEmpty() && id >= 0&& index >= 0; }

    	QString text;
	int id;
	int index;
	Type type;
	QTextDocument *doc;
    };

private:
    virtual bool isReadOnly() const { return FALSE; }
    virtual bool linksEnabled() const { return TRUE; }
    void init();
    void ensureCursorVisible();
    void drawCursor( bool visible );
    void placeCursor( const QPoint &pos, QTextCursor *c = 0 );
    void moveCursor( int direction, bool shift, bool control );
    void moveCursor( int direction, bool control );
    void removeSelectedText();
    void doKeyboardAction( int action );
    bool doCompletion();
    void checkUndoRedoInfo( UndoRedoInfo::Type t );
    void repaintChanged();
    void updateCurrentFormat();
    void handleReadOnlyKeyEvent( QKeyEvent *e );
    void makeParagVisible( QTextParag *p );

private:
    QTextDocument *doc;
    QTextCursor *cursor;
    bool drawAll;
    bool mousePressed;
    QTimer *formatTimer, *scrollTimer, *changeIntervalTimer, *blinkTimer, *dragStartTimer, *resizeTimer;
    QTextParag *lastFormatted;
    int interval;
    QVBox *completionPopup;
    QListBox *completionListBox;
    int completionOffset;
    UndoRedoInfo undoRedoInfo;
    QTextFormat *currentFormat;
    QPainter painter;
    int currentAlignment;
    bool inDoubleClick;
    QPoint oldMousePos, mousePos;
    QPixmap *buf_pixmap;
    bool cursorVisible, blinkCursorVisible;
    bool readOnly, modified, mightStartDrag;
    QPoint dragStartPos;
    int mLines;
    bool firstResize;
    QString onLink;

};

inline QTextDocument *QTextEdit::document() const
{
    return doc;
}

inline QTextCursor *QTextEdit::textCursor() const
{
    return cursor;
}

#endif
