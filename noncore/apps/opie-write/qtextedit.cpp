/****************************************************************************
** $Id: qtextedit.cpp,v 1.2 2002-07-15 23:22:50 leseb Exp $
**
** Implementation of the QTextEdit class
**
** Created : 990101
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the widgets module of the Qt GUI Toolkit.
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

#include "qtextedit.h"

#include "qrichtext_p.h"
#include "qpainter.h"
#include "qpen.h"
#include "qbrush.h"
#include "qpixmap.h"
#include "qfont.h"
#include "qcolor.h"
#include "qstyle.h"
#include "qsize.h"
#include "qevent.h"
#include "qtimer.h"
#include "qapplication.h"
#include "qlistbox.h"
#include "qvbox.h"
#include "qapplication.h"
#include "qclipboard.h"
#include "qcolordialog.h"
#include "qfontdialog.h"
#include "qstylesheet.h"
#include "qdragobject.h"
#include "qurl.h"
#include "qcursor.h"
#include "qregexp.h"
#include "qpopupmenu.h"

#define ACCEL_KEY(k) "\t" + QString("Ctrl+" #k)

using namespace Qt3;

static bool qt_enable_richtext_copy = FALSE;

struct QUndoRedoInfoPrivate
{
    QTextString text;
};

namespace Qt3 {

class QTextEditPrivate
{
public:
    QTextEditPrivate()
	:preeditStart(-1),preeditLength(-1),ensureCursorVisibleInShowEvent(FALSE)
    {
	for ( int i=0; i<7; i++ )
	    id[i] = 0;
    }
    int id[ 7 ];
    int preeditStart;
    int preeditLength;
    bool ensureCursorVisibleInShowEvent;
    QString scrollToAnchor; // used to deferr scrollToAnchor() until the show event when we are resized
};

}

static bool block_set_alignment = FALSE;

/*!
    \class QTextEdit qtextedit.h
    \brief The QTextEdit widget provides a powerful single-page rich text editor.

    \ingroup basic
    \ingroup text
    \mainclass

    \tableofcontents

    \section1 Introduction and Concepts

    QTextEdit is an advanced WYSIWYG viewer/editor supporting rich
    text formatting using HTML-style tags. It is optimized to handle
    large documents and to respond quickly to user input.

    QTextEdit has three modes of operation:
    \table
    \header \i Mode \i Command \i Notes
    \row \i Plain Text Editor \i setTextFormat(PlainText)
	 \i Set text with setText(); text() returns plain text. Text
	 attributes (e.g. colors) can be set, but plain text is always
	 returned.<sup>1.</sup>
    \row \i Rich Text Editor \i setTextFormat(RichText)
	 \i Set text with setText(); text() returns rich text. Rich
	 text editing is fairly limited. You can't set margins or
	 insert images for example (although you can read and
	 correctly display files that have margins set and that
	 include images). This mode is mostly useful for editing small
	 amounts of rich text. <sup>2.</sup>
    \row \i Text Viewer<sup>3.</sup> \i setReadOnly(TRUE)
         \i Set text with setText() or append() (which has no undo
	 history so is faster and uses less memory); text() returns
	 plain or rich text depending on the textFormat(). This mode
	 can correctly display a large subset of HTML tags.
    \endtable

    <sup>1.</sup><small>We do \e not recommend using QTextEdit to
    create syntax highlighting editors because the current API is
    insufficient for this purpose. We hope to release a more complete
    API that will support syntax highlighting in a later
    release.</small>

    <sup>2.</sup><small>A more complete API that supports setting
    margins, images, etc., is planned for a later Qt release.</small>

    <sup>3.</sup><small>Qt 3.1 will provide a Log Viewer mode which is
    optimised for the fast and memory efficient display of large
    amounts of read only text.</small>

    We recommend that you always call setTextFormat() to set the mode
    you want to use. If you use \c AutoText then setText() and
    append() will try to determine whether the text they are given is
    plain text or rich text. If you use \c RichText then setText() and
    append() will assume that the text they are given is rich text.
    insert() simply inserts the text it is given.

    QTextEdit works on paragraphs and characters. A paragraph is a
    formatted string which is word-wrapped to fit into the width of
    the widget. By default when reading plain text, two newlines
    signify a paragraph. A document consists of zero or more
    paragraphs, indexed from 0. Characters are indexed on a
    per-paragraph basis, also indexed from 0. The words in the
    paragraph are aligned in accordance with the paragraph's
    alignment(). Paragraphs are separated by hard line breaks. Each
    character within a paragraph has its own attributes, for example,
    font and color.

    The text edit documentation uses the following concepts:
    \list
    \i \e{current format} --
    this is the format at the current cursor position, \e and it
    is the format of the selected text if any.
    \i \e{current paragraph} -- the paragraph which contains the
    cursor.
    \endlist

    QTextEdit can display images (using QMimeSourceFactory), lists and
    tables. If the text is too large to view within the text edit's
    viewport, scrollbars will appear. The text edit can load both
    plain text and HTML files (a subset of HTML 3.2 and 4).  The
    rendering style and the set of valid tags are defined by a
    styleSheet(). Custom tags can be created and placed in a custom
    style sheet. Change the style sheet with \l{setStyleSheet()}; see
    QStyleSheet for details. The images identified by image tags are
    displayed if they can be interpreted using the text edit's
    \l{QMimeSourceFactory}; see setMimeSourceFactory().

    If you want a text browser with more navigation use QTextBrowser.
    If you just need to display a small piece of rich text use QLabel
    or QSimpleRichText.

    If you create a new QTextEdit, and want to allow the user to edit
    rich text, call setTextFormat(Qt::RichText) to ensure that the
    text is treated as rich text. (Rich text uses HTML tags to set
    text formatting attributes. See QStyleSheet for information on the
    HTML tags that are supported.). If you don't call setTextFormat()
    explicitly the text edit will guess from the text itself whether
    it is rich text or plain text. This means that if the text looks
    like HTML or XML it will probably be interpreted as rich text, so
    you should call setTextFormat(Qt::PlainText) to preserve such
    text.

    Note that we do not intend to add a full-featured web browser
    widget to Qt (because that would easily double Qt's size and only
    a few applications would benefit from it). The rich
    text support in Qt is designed to provide a fast, portable and
    efficient way to add reasonable online help facilities to
    applications, and to provide a basis for rich text editors.
    \section1 Using QTextEdit as a Display Widget

    QTextEdit can display a large HTML subset, including tables and
    images.

    The text is set or replaced using setText() which deletes any
    existing text and replaces it with the text passed in the
    setText() call. If you call setText() with legacy HTML (with
    setTextFormat(RichText) in force), and then call text(), the text
    that is returned may have different markup, but will render the
    same. Text can be inserted with insert(), paste(), pasteSubType()
    and append(). Text that is appended does not go into the undo
    history; this makes append() faster and consumes less memory. Text
    can also be cut(). The entire text is deleted with clear() and the
    selected text is deleted with removeSelectedText(). Selected
    (marked) text can also be deleted with del() (which will delete
    the character to the right of the cursor if no text is selected).

    Loading and saving text is achieved using setText() and text(),
    for example:
    \code
    QFile file( fileName ); // Read the text from a file
    if ( file.open( IO_ReadOnly ) ) {
	QTextStream ts( &file );
	textEdit->setText( ts.read() );
    }
    \endcode
    \code
    QFile file( fileName ); // Write the text to a file
    if ( file.open( IO_WriteOnly ) ) {
	QTextStream ts( &file );
	ts << textEdit->text();
	textEdit->setModified( FALSE );
    }
    \endcode

    By default the text edit wraps words at whitespace to fit within
    the text edit widget. The setWordWrap() function is used to
    specify the kind of word wrap you want, or \c NoWrap if you don't
    want any wrapping.  Call setWordWrap() to set a fixed pixel width
    \c FixedPixelWidth, or character column (e.g. 80 column) \c
    FixedColumnWidth with the pixels or columns specified with
    setWrapColumnOrWidth(). If you use word wrap to the widget's width
    \c WidgetWidth, you can specify whether to break on whitespace or
    anywhere with setWrapPolicy().

    The background color is set differently than other widgets, using
    setPaper(). You specify a brush style which could be a plain color
    or a complex pixmap.

    Hypertext links are automatically underlined; this can be changed
    with setLinkUnderline(). The tab stop width is set with
    setTabStopWidth().

    The zoomIn() and zoomOut() functions can be used to resize the
    text by increasing (decreasing for zoomOut()) the point size used.
    Images are not affected by the zoom functions.

    The lines() function returns the number of lines in the text and
    paragraphs() returns the number of paragraphs. The number of lines
    within a particular paragraph is returned by linesOfParagraph().
    The length of the entire text in characters is returned by
    length().

    You can scroll to an anchor in the text, e.g. \c{<a
    name="anchor">} with scrollToAnchor(). The find() function can be
    used to find and select a given string within the text.

    A read-only QTextEdit provides the same functionality as the
    (obsolete) QTextView. (QTextView is still supplied for
    compatibility with old code.)

    \section2 Read-only key bindings

    When QTextEdit is used read-only the key-bindings are limited to
    navigation, and text may only be selected with the mouse:
    \table
    \header \i Keypresses \i Action
    \row \i \e{UpArrow} \i Move one line up
    \row \i \e{DownArrow} \i Move one line down
    \row \i \e{LeftArrow} \i Move one character left
    \row \i \e{RightArrow} \i Move one character right
    \row \i \e{PageUp} \i Move one (viewport) page up
    \row \i \e{PageDown} \i Move one (viewport) page down
    \row \i \e{Home} \i Move to the beginning of the text
    \row \i \e{End} \i Move to the end of the text
    \row \i \e{Shift+Wheel} \i Scroll the page horizontally (the Wheel is the mouse wheel)
    \row \i \e{Ctrl+Wheel} \i Zoom the text
    \endtable

    The text edit may be able to provide some meta-information. For
    example, the documentTitle() function will return the text from
    within HTML \c{<title>} tags.

    The text displayed in a text edit has a \e context. The context is
    a path which the text edit's QMimeSourceFactory uses to resolve
    the locations of files and images. It is passed to the
    mimeSourceFactory() when quering data. (See QTextEdit() and
    \l{context()}.)

    \section1 Using QTextEdit as an Editor

    All the information about using QTextEdit as a display widget also
    applies here.

    The current format's attributes are set with setItalic(),
    setBold(), setUnderline(), setFamily() (font family),
    setPointSize(), setColor() and setCurrentFont().  The current
    paragraph's alignment is set with setAlignment().

    Use setSelection() to select text. The setSelectionAttributes()
    function is used to indicate how selected text should be
    displayed. Use hasSelectedText() to find out if any text is
    selected. The currently selected text's position is available
    using getSelection() and the selected text itself is returned by
    selectedText(). The selection can be copied to the clipboard with
    copy(), or cut to the clipboard with cut(). It can be deleted with
    removeSelectedText(). The entire text can be selected (or
    unselected) using selectAll(). QTextEdit supports multiple
    selections. Most of the selection functions operate on the default
    selection, selection 0. If the user presses a non-selecting key,
    e.g. a cursor key without also holding down Shift, all selections
    are cleared.

    Set and get the position of the cursor with setCursorPosition()
    and getCursorPosition() respectively. When the cursor is moved,
    the signals currentFontChanged(), currentColorChanged() and
    currentAlignmentChanged() are emitted to reflect the font, color
    and alignment at the new cursor position.

    If the text changes, the textChanged() signal is emitted, and if
    the user inserts a new line by pressing Return or Enter,
    returnPressed() is emitted. The isModified() function will return
    TRUE if the text has been modified.

    QTextEdit provides command-based undo and redo. To set the depth
    of the command history use setUndoDepth() which defaults to 100
    steps. To undo or redo the last operation call undo() or redo().
    The signals undoAvailable() and redoAvailable() indicate whether
    the undo and redo operations can be executed.

    The indent() function is used to reindent a paragraph. It is
    useful for code editors, for example in <em>Qt Designer</em>'s
    code editor \e{Ctrl+I} invokes the indent() function.

    \section2 Editing key bindings

    The list of key-bindings which are implemented for editing:
    \table
    \header \i Keypresses \i Action
    \row \i \e{Backspace} \i Delete the character to the left of the cursor
    \row \i \e{Delete} \i Delete the character to the right of the cursor
    \row \i \e{Ctrl+A} \i Move the cursor to the beginning of the line
    \row \i \e{Ctrl+B} \i Move the cursor one character left
    \row \i \e{Ctrl+C} \i Copy the marked text to the clipboard (also
			  \e{Ctrl+Insert} under Windows)
    \row \i \e{Ctrl+D} \i Delete the character to the right of the cursor
    \row \i \e{Ctrl+E} \i Move the cursor to the end of the line
    \row \i \e{Ctrl+F} \i Move the cursor one character right
    \row \i \e{Ctrl+H} \i Delete the character to the left of the cursor
    \row \i \e{Ctrl+K} \i Delete to end of line
    \row \i \e{Ctrl+N} \i Move the cursor one line down
    \row \i \e{Ctrl+P} \i Move the cursor one line up
    \row \i \e{Ctrl+V} \i Paste the clipboard text into line edit
			  (also \e{Shift+Insert} under Windows)
    \row \i \e{Ctrl+X} \i Cut the marked text, copy to clipboard
			  (also \e{Shift+Delete} under Windows)
    \row \i \e{Ctrl+Z} \i Undo the last operation
    \row \i \e{Ctrl+Y} \i Redo the last operation
    \row \i \e{LeftArrow} \i Move the cursor one character left
    \row \i \e{Ctrl+LeftArrow} \i Move the cursor one word left
    \row \i \e{RightArrow} \i Move the cursor one character right
    \row \i \e{Ctrl+RightArrow} \i Move the cursor one word right
    \row \i \e{UpArrow} \i Move the cursor one line up
    \row \i \e{Ctrl+UpArrow} \i Move the cursor one word up
    \row \i \e{DownArrow} \i Move the cursor one line down
    \row \i \e{Ctrl+Down Arrow} \i Move the cursor one word down
    \row \i \e{PageUp} \i Move the cursor one page up
    \row \i \e{PageDown} \i Move the cursor one page down
    \row \i \e{Home} \i Move the cursor to the beginning of the line
    \row \i \e{Ctrl+Home} \i Move the cursor to the beginning of the text
    \row \i \e{End} \i Move the cursor to the end of the line
    \row \i \e{Ctrl+End} \i Move the cursor to the end of the text
    \row \i \e{Shift+Wheel} \i Scroll the page horizontally
			       (the Wheel is the mouse wheel)
    \row \i \e{Ctrl+Wheel} \i Zoom the text
    \endtable

    To select (mark) text hold down the Shift key whilst pressing one
    of the movement keystrokes, for example, <i>Shift+Right Arrow</i>
    will select the character to the right, and <i>Shift+Ctrl+Right
    Arrow</i> will select the word to the right, etc.

    By default the text edit widget operates in insert mode so all
    text that the user enters is inserted into the text edit and any
    text to the right of the cursor is moved out of the way. The mode
    can be changed to overwrite, where new text overwrites any text to
    the right of the cursor, using setOverwriteMode().

*/

/*! \enum QTextEdit::KeyboardAction

  This enum is used by doKeyboardAction() to specify which action
  should be executed:

  \value ActionBackspace  Delete the character to the left of the
  cursor.

  \value ActionDelete  Delete the character to the right of the cursor.

  \value ActionReturn  Split the paragraph at the cursor position.

  \value ActionKill If the cursor is not at the end of the paragraph,
  delete the text from the cursor position until the end of the
  paragraph. If the cursor is at the end of the paragraph, delete the
  hard line break at the end of the paragraph - this will cause this
  paragraph to be joined with the following paragraph.
*/

/*! \enum QTextEdit::VerticalAlignment

  This enum is used to set the vertical alignment of the text.

  \value AlignNormal Normal alignment
  \value AlignSuperScript Superscript
  \value AlignSubScript Subscript
*/

/*!  \fn void QTextEdit::copyAvailable (bool yes)

  This signal is emitted when text is selected or de-selected in the text
  edit.

  When text is selected this signal will be emitted with \a yes set to
  TRUE. If no text has been selected or if the selected text is
  de-selected this signal is emitted with \a yes set to FALSE.

    If \a yes is TRUE then copy() can be used to copy the selection to the
    clipboard. If \a yes is FALSE then copy() does nothing.

    \sa selectionChanged()
*/


/*!  \fn void QTextEdit::textChanged()

  This signal is emitted whenever the text in the text edit changes.

  \sa setText() append()
 */

/*!  \fn void QTextEdit::selectionChanged()

  This signal is emitted whenever the selection changes.

  \sa setSelection() copyAvailable()
*/

/*!  \fn QTextDocument *QTextEdit::document() const

    \internal

  This function returns the QTextDocument which is used by the text
  edit.
*/

/*!  \fn void QTextEdit::setDocument( QTextDocument *doc )

    \internal

  This function sets the QTextDocument which should be used by the text
  edit to \a doc. This can be used, for example, if you want to
  display a document using multiple views. You would create a
  QTextDocument and set it to the text edits which should display it.
  You would need to connect to the textChanged() and
  selectionChanged() signals of all the text edits and update them all
  accordingly (preferably with a slight delay for efficiency reasons).
*/

/*! \enum QTextEdit::CursorAction

  This enum is used by moveCursor() to specify in which direction
  the cursor should be moved:

  \value MoveBackward  Moves the cursor one character backward

  \value MoveWordBackward Moves the cursor one word backward

  \value MoveForward  Moves the cursor one character forward

  \value MoveWordForward Moves the cursor one word forward

  \value MoveUp  Moves the cursor up one line

  \value MoveDown  Moves the cursor down one line

  \value MoveLineStart  Moves the cursor to the beginning of the line

  \value MoveLineEnd Moves the cursor to the end of the line

  \value MoveHome  Moves the cursor to the beginning of the document

  \value MoveEnd Moves the cursor to the end of the document

  \value MovePgUp  Moves the cursor one page up

  \value MovePgDown  Moves the cursor one page down
*/


/*!
  \property QTextEdit::overwriteMode
  \brief the text edit's overwrite mode

  If FALSE (the default) characters entered by the user are inserted
  with any characters to the right being moved out of the way.
  If TRUE, the editor is in overwrite mode, i.e. characters entered by
  the user overwrite any characters to the right of the cursor position.
*/

/*! \fn void QTextEdit::setCurrentFont( const QFont &f )

  Sets the font of the current format to \a f.

  \sa font() setPointSize() setFamily()
*/

/*!
  \property QTextEdit::undoDepth
  \brief the depth of the undo history

  The maximum number of steps in the undo/redo history.
  The default is 100.

  \sa undo() redo()
*/

/*! \fn void QTextEdit::undoAvailable( bool yes )

  This signal is emitted when the availability of undo changes.  If \a
  yes is TRUE, then undo() will work until undoAvailable( FALSE ) is
  next emitted.

  \sa undo() undoDepth()
*/

/*! \fn void QTextEdit::modificationChanged( bool m )

  This signal is emitted when the modification of the document
  changed. If \a m is TRUE, the document was modified, otherwise the
  modification state has been reset to unmodified.

  \sa modified
*/

/*! \fn void QTextEdit::redoAvailable( bool yes )

  This signal is emitted when the availability of redo changes.  If \a
  yes is TRUE, then redo() will work until redoAvailable( FALSE ) is
  next emitted.

  \sa redo() undoDepth()
*/

/*! \fn void QTextEdit::currentFontChanged( const QFont &f )

  This signal is emitted if the font of the current format has changed.

  The new font is \a f.

  \sa setCurrentFont()
*/

/*! \fn void QTextEdit::currentColorChanged( const QColor &c )

  This signal is emitted if the color of the current format has changed.

  The new color is \a c.

  \sa setColor()
*/

/*! \fn void QTextEdit::currentVerticalAlignmentChanged( VerticalAlignment a )

  This signal is emitted if the vertical alignment of the current
  format has changed.

  The new vertical alignment is \a a.

  \sa setVerticalAlignment()
*/

/*! \fn void QTextEdit::currentAlignmentChanged( int a )

  This signal is emitted if the alignment of the current paragraph
  has changed.

  The new alignment is \a a.

  \sa setAlignment()
*/

/*! \fn void QTextEdit::cursorPositionChanged( QTextCursor *c )

  This signal is emitted if the position of the cursor changed. \a c
  points to the text cursor object.

  \sa setCursorPosition()
*/

/*! \overload void QTextEdit::cursorPositionChanged( int para, int pos )

  This signal is emitted if the position of the cursor changed. \a
  para contains the paragraph index and \a pos contains the character
  position within the paragraph.

  \sa setCursorPosition()
*/

/*! \fn void QTextEdit::returnPressed()

  This signal is emitted if the user pressed the Return or the Enter key.
*/

/*!
    \fn QTextCursor *QTextEdit::textCursor() const

    Returns the text edit's text cursor.

    \warning QTextCursor is not in the public API, but in special
    circumstances you might wish to use it.
*/

/*!  Constructs an empty QTextEdit with parent \a parent and name \a
  name.
*/

QTextEdit::QTextEdit( QWidget *parent, const char *name )
    : QScrollView( parent, name, WStaticContents | WRepaintNoErase | WResizeNoErase ),
      doc( new QTextDocument( 0 ) ), undoRedoInfo( doc )
{
    init();
}

/*!
    Constructs a QTextEdit with parent \a parent and name \a name. The
    text edit will display the text \a text using context \a context.

    The \a context is a path which the text edit's QMimeSourceFactory
    uses to resolve the locations of files and images. It is passed to
    the mimeSourceFactory() when quering data.

    For example if the text contains an image tag,
    \c{<img src="image.png">}, and the context is "path/to/look/in", the
    QMimeSourceFactory will try to load the image from
    "path/to/look/in/image.png". If the tag was
    \c{<img src="/image.png">}, the context will not be used (because
    QMimeSourceFactory recognizes that we have used an absolute path)
    and will try to load "/image.png". The context is applied in exactly
    the same way to \e hrefs, for example,
    \c{<a href="target.html">Target</a>}, would resolve to
    "path/to/look/in/target.html".

*/

QTextEdit::QTextEdit( const QString& text, const QString& context,
		      QWidget *parent, const char *name)
    : QScrollView( parent, name, WStaticContents | WRepaintNoErase | WResizeNoErase ),
      doc( new QTextDocument( 0 ) ), undoRedoInfo( doc )
{
    init();
    setText( text, context );
}

/*! \reimp */

QTextEdit::~QTextEdit()
{
    delete undoRedoInfo.d;
    undoRedoInfo.d = 0;
    delete cursor;
    delete doc;
    delete d;
}

void QTextEdit::init()
{
    setFrameStyle( Sunken );
    undoEnabled = TRUE;
    readonly = TRUE;
    setReadOnly( FALSE );
    d = new QTextEditPrivate;
    connect( doc, SIGNAL( minimumWidthChanged( int ) ),
	     this, SLOT( documentWidthChanged( int ) ) );

    mousePressed = FALSE;
    inDoubleClick = FALSE;
    modified = FALSE;
    onLink = QString::null;
    overWrite = FALSE;
    wrapMode = WidgetWidth;
    wrapWidth = -1;
    wPolicy = AtWhiteSpace;
    inDnD = FALSE;

    doc->setFormatter( new QTextFormatterBreakWords );
    doc->formatCollection()->defaultFormat()->setFont( QScrollView::font() );
    doc->formatCollection()->defaultFormat()->setColor( colorGroup().color( QColorGroup::Text ) );
    currentFormat = doc->formatCollection()->defaultFormat();
    currentAlignment = Qt3::AlignAuto;

    viewport()->setBackgroundMode( PaletteBase );
    viewport()->setAcceptDrops( TRUE );
    resizeContents( 0, doc->lastParagraph() ?
		    ( doc->lastParagraph()->paragId() + 1 ) * doc->formatCollection()->defaultFormat()->height() : 0 );

    setKeyCompression( TRUE );
    viewport()->setMouseTracking( TRUE );
#ifndef QT_NO_CURSOR
    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif
    cursor = new QTextCursor( doc );

    formatTimer = new QTimer( this );
    connect( formatTimer, SIGNAL( timeout() ),
	     this, SLOT( formatMore() ) );
    lastFormatted = doc->firstParagraph();

    scrollTimer = new QTimer( this );
    connect( scrollTimer, SIGNAL( timeout() ),
	     this, SLOT( autoScrollTimerDone() ) );

    interval = 0;
    changeIntervalTimer = new QTimer( this );
    connect( changeIntervalTimer, SIGNAL( timeout() ),
	     this, SLOT( doChangeInterval() ) );

    cursorVisible = TRUE;
    blinkTimer = new QTimer( this );
    connect( blinkTimer, SIGNAL( timeout() ),
	     this, SLOT( blinkCursor() ) );

#ifndef QT_NO_DRAGANDDROP
    dragStartTimer = new QTimer( this );
    connect( dragStartTimer, SIGNAL( timeout() ),
	     this, SLOT( startDrag() ) );
#endif


    formatMore();

    blinkCursorVisible = FALSE;

    viewport()->setFocusProxy( this );
    viewport()->setFocusPolicy( WheelFocus );
    viewport()->installEventFilter( this );
    installEventFilter( this );
}

void QTextEdit::paintDocument( bool drawAll, QPainter *p, int cx, int cy, int cw, int ch )
{
    bool drawCur = hasFocus() || viewport()->hasFocus();
    if ( hasSelectedText() || isReadOnly() || !cursorVisible )
	drawCur = FALSE;
    QColorGroup g = colorGroup();
    if ( doc->paper() )
	g.setBrush( QColorGroup::Base, *doc->paper() );

    if ( contentsY() < doc->y() ) {
	p->fillRect( contentsX(), contentsY(), visibleWidth(), doc->y(),
		     g.brush( QColorGroup::Base ) );
    }
    if ( drawAll && doc->width() - contentsX() < cx + cw ) {
	p->fillRect( doc->width() - contentsX(), cy, cx + cw - doc->width() + contentsX(), ch,
		     g.brush( QColorGroup::Base ) );
    }

    p->setBrushOrigin( -contentsX(), -contentsY() );

    lastFormatted = doc->draw( p, cx, cy, cw, ch, g, !drawAll, drawCur, cursor );

    if ( lastFormatted == doc->lastParagraph() )
	resizeContents( contentsWidth(), doc->height() );

    if ( contentsHeight() < visibleHeight() && ( !doc->lastParagraph() || doc->lastParagraph()->isValid() ) && drawAll )
	p->fillRect( 0, contentsHeight(), visibleWidth(),
		     visibleHeight() - contentsHeight(), g.brush( QColorGroup::Base ) );
}

/*! \reimp */

void QTextEdit::drawContents( QPainter *p, int cx, int cy, int cw, int ch )
{
    paintDocument( TRUE, p, cx, cy, cw, ch );
    int v;
    p->setPen( foregroundColor() );
    if ( document()->isPageBreakEnabled() &&  ( v = document()->flow()->pageSize() ) > 0 ) {
	int l = int(cy / v) * v;
	while ( l < cy + ch ) {
	    p->drawLine( cx, l, cx + cw - 1, l );
	    l += v;
	}
    }

}

/*! \reimp */

void QTextEdit::drawContents( QPainter * )
{
}

/*! \reimp */

bool QTextEdit::event( QEvent *e )
{
    if ( e->type() == QEvent::AccelOverride && !isReadOnly() ) {
	QKeyEvent* ke = (QKeyEvent*) e;
	if ( ke->state() == NoButton || ke->state() == Keypad ) {
	    if ( ke->key() < Key_Escape ) {
		ke->accept();
	    } else {
		switch ( ke->key() ) {
		case Key_Return:
		case Key_Enter:
		case Key_Delete:
		case Key_Home:
		case Key_End:
		case Key_Backspace:
		    ke->accept();
		default:
		    break;
		}
	    }
	} else if ( ke->state() & ControlButton ) {
	    switch ( ke->key() ) {
// Those are too frequently used for application functionality
/*	    case Key_A:
	    case Key_B:
	    case Key_D:
	    case Key_E:
	    case Key_F:
	    case Key_H:
	    case Key_I:
	    case Key_K:
	    case Key_N:
	    case Key_P:
	    case Key_T:
*/
	    case Key_C:
	    case Key_V:
	    case Key_X:
	    case Key_Y:
	    case Key_Z:
	    case Key_Left:
	    case Key_Right:
	    case Key_Up:
	    case Key_Down:
	    case Key_Home:
	    case Key_End:
	    case Key_Tab:
#if defined (Q_WS_WIN)
	    case Key_Insert:
	    case Key_Delete:
#endif
		ke->accept();
	    default:
		break;
	    }
	} else {
	    switch ( ke->key() ) {
#if defined (Q_WS_WIN)
	    case Key_Insert:
		ke->accept();
#endif
	    default:
		break;
	    }
	}
    }

    if ( e->type() == QEvent::Show ) {
	if ( d->ensureCursorVisibleInShowEvent ) {
	    sync();
	    ensureCursorVisible();
	    d->ensureCursorVisibleInShowEvent = FALSE;
	}
	if ( !d->scrollToAnchor.isEmpty()  ) {
	    scrollToAnchor( d->scrollToAnchor );
	    d->scrollToAnchor = QString::null;
	}
    }
    return QWidget::event( e );
}

/*!
    Processes the key event, \a e.
    By default key events are used to provide keyboard navigation and
    text editing.
*/

void QTextEdit::keyPressEvent( QKeyEvent *e )
{
    changeIntervalTimer->stop();
    interval = 10;
    bool unknown = FALSE;
    if ( isReadOnly() ) {
	if ( !handleReadOnlyKeyEvent( e ) )
	    QScrollView::keyPressEvent( e );
	changeIntervalTimer->start( 100, TRUE );
	return;
    }


    bool selChanged = FALSE;
    for ( int i = 1; i < doc->numSelections(); ++i ) // start with 1 as we don't want to remove the Standard-Selection
	selChanged = doc->removeSelection( i ) || selChanged;

    if ( selChanged ) {
	cursor->paragraph()->document()->nextDoubleBuffered = TRUE;
	repaintChanged();
    }

    bool clearUndoRedoInfo = TRUE;


    switch ( e->key() ) {
    case Key_Left:
    case Key_Right: {
	// a bit hacky, but can't change this without introducing new enum values for move and keeping the
	// correct semantics and movement for BiDi and non BiDi text.
	CursorAction a;
	if ( cursor->paragraph()->string()->isRightToLeft() == (e->key() == Key_Right) )
	    a = e->state() & ControlButton ? MoveWordBackward : MoveBackward;
	else
	    a = e->state() & ControlButton ? MoveWordForward : MoveForward;
	moveCursor( a, e->state() & ShiftButton );
	break;
    }
    case Key_Up:
	moveCursor( e->state() & ControlButton ? MovePgUp : MoveUp, e->state() & ShiftButton );
	break;
    case Key_Down:
	moveCursor( e->state() & ControlButton ? MovePgDown : MoveDown, e->state() & ShiftButton );
	break;
    case Key_Home:
	moveCursor( e->state() & ControlButton ? MoveHome : MoveLineStart, e->state() & ShiftButton );
	break;
    case Key_End:
	moveCursor( e->state() & ControlButton ? MoveEnd : MoveLineEnd, e->state() & ShiftButton );
	break;
    case Key_Prior:
	moveCursor( MovePgUp, e->state() & ShiftButton );
	break;
    case Key_Next:
	moveCursor( MovePgDown, e->state() & ShiftButton );
	break;
    case Key_Return: case Key_Enter:
	if ( doc->hasSelection( QTextDocument::Standard, FALSE ) )
	    removeSelectedText();
	if ( textFormat() == Qt::RichText && ( e->state() & ControlButton ) ) {
	    // Ctrl-Enter inserts a line break in rich text mode
	    insert( QString( QChar( 0x2028) ), TRUE, FALSE, TRUE );
	} else {
#ifndef QT_NO_CURSOR
	    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif
	    clearUndoRedoInfo = FALSE;
	    doKeyboardAction( ActionReturn );
	    emit returnPressed();
	}
	break;
    case Key_Delete:
#if defined (Q_WS_WIN)
	if ( e->state() & ShiftButton ) {
	    cut();
	    break;
	} else
#endif
        if ( doc->hasSelection( QTextDocument::Standard, TRUE ) ) {
	    removeSelectedText();
	    break;
	}
	doKeyboardAction( ActionDelete );
	clearUndoRedoInfo = FALSE;

	break;
    case Key_Insert:
	if ( e->state() & ShiftButton )
	    paste();
#if defined (Q_WS_WIN)
	else if ( e->state() & ControlButton )
	    copy();
#endif
	break;
    case Key_Backspace:
	if ( doc->hasSelection( QTextDocument::Standard, TRUE ) ) {
	    removeSelectedText();
	    break;
	}

	doKeyboardAction( ActionBackspace );
	clearUndoRedoInfo = FALSE;

	break;
    case Key_F16: // Copy key on Sun keyboards
	copy();
	break;
    case Key_F18:  // Paste key on Sun keyboards
	paste();
	break;
    case Key_F20:  // Cut key on Sun keyboards
	cut();
	break;
    default: {
	    if ( e->text().length() &&
		( !( e->state() & ControlButton ) &&
		  !( e->state() & AltButton ) ||
		 ( ( e->state() & ControlButton | AltButton ) == (ControlButton|AltButton) ) ) &&
		 ( !e->ascii() || e->ascii() >= 32 || e->text() == "\t" ) ) {
		clearUndoRedoInfo = FALSE;
		if ( e->key() == Key_Tab ) {
		    if ( textFormat() == Qt::RichText && cursor->paragraph()->isListItem() ) {
			clearUndoRedo();
			undoRedoInfo.type = UndoRedoInfo::Style;
			undoRedoInfo.id = cursor->paragraph()->paragId();
			undoRedoInfo.eid = undoRedoInfo.id;
			undoRedoInfo.styleInformation = QTextStyleCommand::readStyleInformation( doc, undoRedoInfo.id, undoRedoInfo.eid );
			cursor->paragraph()->setListDepth( cursor->paragraph()->listDepth() +1 );
			clearUndoRedo();
			drawCursor( FALSE );
			repaintChanged();
			drawCursor( TRUE );
			break;
		    }
		}

		if ( textFormat() == Qt::RichText && !cursor->paragraph()->isListItem() ) {
		    if ( cursor->index() == 0 && ( e->text()[0] == '-' || e->text()[0] == '*' ) ) {
			clearUndoRedo();
			undoRedoInfo.type = UndoRedoInfo::Style;
			undoRedoInfo.id = cursor->paragraph()->paragId();
			undoRedoInfo.eid = undoRedoInfo.id;
			undoRedoInfo.styleInformation = QTextStyleCommand::readStyleInformation( doc, undoRedoInfo.id, undoRedoInfo.eid );
			setParagType( QStyleSheetItem::DisplayListItem, QStyleSheetItem::ListDisc );
			clearUndoRedo();
			drawCursor( FALSE );
			repaintChanged();
			drawCursor( TRUE );
			break;
		    }
		}
		if ( overWrite && !cursor->atParagEnd() )
		    cursor->remove();
		QString t = e->text();
		QTextParagraph *p = cursor->paragraph();
		if ( p && p->string() && p->string()->isRightToLeft() ) {
		    QChar *c = (QChar *)t.unicode();
		    int l = t.length();
		    while( l-- ) {
			if ( c->mirrored() )
			    *c = c->mirroredChar();
			c++;
		    }
		}
		insert( t, TRUE, FALSE, TRUE );
		break;
	    } else if ( e->state() & ControlButton ) {
		switch ( e->key() ) {
		case Key_C: case Key_F16: // Copy key on Sun keyboards
		    copy();
		    break;
		case Key_V:
		    paste();
		    break;
		case Key_X:
		    cut();
		    break;
		case Key_I: case Key_T: case Key_Tab:
		    indent();
		    break;
		case Key_A:
#if defined(Q_WS_X11)
		    moveCursor( MoveLineStart, e->state() & ShiftButton );
#else
		    selectAll( TRUE );
#endif
		    break;
		case Key_B:
		    moveCursor( MoveBackward, e->state() & ShiftButton );
		    break;
		case Key_F:
		    moveCursor( MoveForward, e->state() & ShiftButton );
		    break;
		case Key_D:
		    if ( doc->hasSelection( QTextDocument::Standard ) ) {
			removeSelectedText();
			break;
		    }
		    doKeyboardAction( ActionDelete );
		    clearUndoRedoInfo = FALSE;
		    break;
		case Key_H:
		    if ( doc->hasSelection( QTextDocument::Standard ) ) {
			removeSelectedText();
			break;
		    }
		    if ( !cursor->paragraph()->prev() &&
			 cursor->atParagStart() )
			break;

		    doKeyboardAction( ActionBackspace );
		    clearUndoRedoInfo = FALSE;
		    break;
		case Key_E:
		    moveCursor( MoveLineEnd, e->state() & ShiftButton );
		    break;
		case Key_N:
		    moveCursor( MoveDown, e->state() & ShiftButton );
		    break;
		case Key_P:
		    moveCursor( MoveUp, e->state() & ShiftButton );
		    break;
		case Key_Z:
		    if(e->state() & ShiftButton)
			redo();
		    else
			undo();
		    break;
		case Key_Y:
		    redo();
		    break;
		case Key_K:
		    doKeyboardAction( ActionKill );
		    break;
#if defined(Q_WS_WIN)
		case Key_Insert:
		    copy();
		    break;
		case Key_Delete:
		    del();
		    break;
#endif
		default:
		    unknown = FALSE;
		    break;
		}
	    } else {
		unknown = TRUE;
	    }
        }
    }

    emit cursorPositionChanged( cursor );
    emit cursorPositionChanged( cursor->paragraph()->paragId(), cursor->index() );
    if ( clearUndoRedoInfo )
	clearUndoRedo();
    changeIntervalTimer->start( 100, TRUE );
    if ( unknown )
	e->ignore();
}

/*!
  Executes keyboard action \a action. This is normally called by
  a key event handler.
*/

void QTextEdit::doKeyboardAction( KeyboardAction action )
{
    if ( isReadOnly() )
	return;

    if ( cursor->nestedDepth() != 0 ) // #### for 3.0, disable editing of tables as this is not advanced enough
	return;

    lastFormatted = cursor->paragraph();
    drawCursor( FALSE );
    bool doUpdateCurrentFormat = TRUE;

    switch ( action ) {
    case ActionDelete:
	if ( !cursor->atParagEnd() ) {
	    checkUndoRedoInfo( UndoRedoInfo::Delete );
	    if ( !undoRedoInfo.valid() ) {
		undoRedoInfo.id = cursor->paragraph()->paragId();
		undoRedoInfo.index = cursor->index();
		undoRedoInfo.d->text = QString::null;
	    }
	    undoRedoInfo.d->text.insert( undoRedoInfo.d->text.length(), cursor->paragraph()->at( cursor->index() ), TRUE );
	    cursor->remove();
	} else {
	    clearUndoRedo();
	    doc->setSelectionStart( QTextDocument::Temp, *cursor );
	    cursor->gotoNextLetter();
	    doc->setSelectionEnd( QTextDocument::Temp, *cursor );
	    removeSelectedText( QTextDocument::Temp );
	}
	break;
    case ActionBackspace:
	if ( textFormat() == Qt::RichText && cursor->paragraph()->isListItem() && cursor->index() == 0 ) {
	    clearUndoRedo();
	    undoRedoInfo.type = UndoRedoInfo::Style;
	    undoRedoInfo.id = cursor->paragraph()->paragId();
	    undoRedoInfo.eid = undoRedoInfo.id;
	    undoRedoInfo.styleInformation = QTextStyleCommand::readStyleInformation( doc, undoRedoInfo.id, undoRedoInfo.eid );
	    int ldepth = cursor->paragraph()->listDepth();
	    ldepth = QMAX( ldepth-1, 0 );
	    cursor->paragraph()->setListDepth( ldepth );
	    if ( ldepth == 0 )
		cursor->paragraph()->setListItem( FALSE );
	    clearUndoRedo();
	    lastFormatted = cursor->paragraph();
	    repaintChanged();
	    drawCursor( TRUE );
	    return;
	}
	if ( !cursor->atParagStart() ) {
	    checkUndoRedoInfo( UndoRedoInfo::Delete );
	    if ( !undoRedoInfo.valid() ) {
		undoRedoInfo.id = cursor->paragraph()->paragId();
		undoRedoInfo.index = cursor->index();
		undoRedoInfo.d->text = QString::null;
	    }
	    cursor->gotoPreviousLetter();
	    undoRedoInfo.d->text.insert( 0, cursor->paragraph()->at( cursor->index() ), TRUE );
	    undoRedoInfo.index = cursor->index();
	    cursor->remove();
	    lastFormatted = cursor->paragraph();
	} else if ( cursor->paragraph()->prev() ){
	    clearUndoRedo();
	    doc->setSelectionStart( QTextDocument::Temp, *cursor );
	    cursor->gotoPreviousLetter();
	    doc->setSelectionEnd( QTextDocument::Temp, *cursor );
	    removeSelectedText( QTextDocument::Temp );
	}
	break;
    case ActionReturn:
	checkUndoRedoInfo( UndoRedoInfo::Return );
	if ( !undoRedoInfo.valid() ) {
	    undoRedoInfo.id = cursor->paragraph()->paragId();
	    undoRedoInfo.index = cursor->index();
	    undoRedoInfo.d->text = QString::null;
	}
	undoRedoInfo.d->text += "\n";
	cursor->splitAndInsertEmptyParagraph();
	if ( cursor->paragraph()->prev() ) {
	    lastFormatted = cursor->paragraph()->prev();
	    lastFormatted->invalidate( 0 );
	}
	doUpdateCurrentFormat = FALSE;
	break;
    case ActionKill:
	    clearUndoRedo();
	    doc->setSelectionStart( QTextDocument::Temp, *cursor );
	    if ( cursor->atParagEnd() )
		cursor->gotoNextLetter();
	    else
		cursor->setIndex( cursor->paragraph()->length() - 1 );
	    doc->setSelectionEnd( QTextDocument::Temp, *cursor );
	    removeSelectedText( QTextDocument::Temp );
	    break;
    }

    formatMore();
    repaintChanged();
    ensureCursorVisible();
    drawCursor( TRUE );
    updateMicroFocusHint();
    if ( doUpdateCurrentFormat )
	updateCurrentFormat();
    setModified();
    emit textChanged();
}

void QTextEdit::readFormats( QTextCursor &c1, QTextCursor &c2, QTextString &text, bool fillStyles )
{
    QDataStream styleStream( undoRedoInfo.styleInformation, IO_WriteOnly );
    c2.restoreState();
    c1.restoreState();
    int lastIndex = text.length();
    if ( c1.paragraph() == c2.paragraph() ) {
	for ( int i = c1.index(); i < c2.index(); ++i )
	    text.insert( lastIndex + i - c1.index(), c1.paragraph()->at( i ), TRUE );
	if ( fillStyles ) {
	    styleStream << (int) 1;
	    c1.paragraph()->writeStyleInformation( styleStream );
	}
    } else {
	int i;
	for ( i = c1.index(); i < c1.paragraph()->length()-1; ++i )
	    text.insert( lastIndex++, c1.paragraph()->at( i ), TRUE );
	int num = 2; // start and end, being different
	text += "\n"; lastIndex++;
	QTextParagraph *p = c1.paragraph()->next();
	while ( p && p != c2.paragraph() ) {
	    for ( i = 0; i < p->length()-1; ++i )
		text.insert( lastIndex++ , p->at( i ), TRUE );
	    text += "\n"; num++; lastIndex++;
	    p = p->next();
	}
	for ( i = 0; i < c2.index(); ++i )
	    text.insert( i + lastIndex, c2.paragraph()->at( i ), TRUE );
	if ( fillStyles ) {
	    styleStream << num;
	    for ( QTextParagraph *p = c1.paragraph(); --num >= 0; p = p->next() )
		p->writeStyleInformation( styleStream );
	}
    }
}

/*! Removes the selection \a selNum (by default 0). This does not
  remove the selected text.

  \sa removeSelectedText()
*/

void QTextEdit::removeSelection( int selNum )
{
    doc->removeSelection( selNum );
    repaintChanged();
}

/*!  Deletes the selected text (i.e. the default selection's text) of
  the selection \a selNum (by default, 0). If there is no selected text
  nothing happens.

  \sa selectedText removeSelection()
*/

void QTextEdit::removeSelectedText( int selNum )
{
    if ( isReadOnly() )
	return;

    QTextCursor c1 = doc->selectionStartCursor( selNum );
    c1.restoreState();
    QTextCursor c2 = doc->selectionEndCursor( selNum );
    c2.restoreState();

    // ### no support for editing tables yet, plus security for broken selections
    if ( c1.nestedDepth() || c2.nestedDepth() )
	return;

    for ( int i = 0; i < (int)doc->numSelections(); ++i ) {
	if ( i == selNum )
	    continue;
	doc->removeSelection( i );
    }

    drawCursor( FALSE );
    checkUndoRedoInfo( UndoRedoInfo::RemoveSelected );
    if ( !undoRedoInfo.valid() ) {
	doc->selectionStart( selNum, undoRedoInfo.id, undoRedoInfo.index );
	undoRedoInfo.d->text = QString::null;
    }
    readFormats( c1, c2, undoRedoInfo.d->text, TRUE );


    doc->removeSelectedText( selNum, cursor );
    if ( cursor->isValid() ) {
	ensureCursorVisible();
	lastFormatted = cursor->paragraph();
	formatMore();
	repaintChanged();
	ensureCursorVisible();
	drawCursor( TRUE );
	clearUndoRedo();
#if defined(Q_WS_WIN)
	// there seems to be a problem with repainting or erasing the area
	// of the scrollview which is not the contents on windows
	if ( contentsHeight() < visibleHeight() )
	    viewport()->repaint( 0, contentsHeight(), visibleWidth(), visibleHeight() - contentsHeight(), TRUE );
#endif
#ifndef QT_NO_CURSOR
	viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif
	updateMicroFocusHint();
    } else {
	delete cursor;
	cursor = new QTextCursor( doc );
	drawCursor( TRUE );
	viewport()->repaint( TRUE );
    }
    setModified();
    emit textChanged();
    emit selectionChanged();
}

/*!  Moves the text cursor according to \a action. This is normally
  used by some key event handler. \a select specifies whether the text
  between the current cursor position and the new position should be
  selected.
*/

void QTextEdit::moveCursor( CursorAction action, bool select )
{
    drawCursor( FALSE );
    if ( select ) {
	if ( !doc->hasSelection( QTextDocument::Standard ) )
	    doc->setSelectionStart( QTextDocument::Standard, *cursor );
	moveCursor( action );
	if ( doc->setSelectionEnd( QTextDocument::Standard, *cursor ) ) {
	    cursor->paragraph()->document()->nextDoubleBuffered = TRUE;
	    repaintChanged();
	} else {
	    drawCursor( TRUE );
	}
	ensureCursorVisible();
	emit selectionChanged();
	emit copyAvailable( doc->hasSelection( QTextDocument::Standard ) );
    } else {
	bool redraw = doc->removeSelection( QTextDocument::Standard );
	moveCursor( action );
	if ( !redraw ) {
	    ensureCursorVisible();
	    drawCursor( TRUE );
	} else {
	    cursor->paragraph()->document()->nextDoubleBuffered = TRUE;
	    repaintChanged();
	    ensureCursorVisible();
	    drawCursor( TRUE );
#ifndef QT_NO_CURSOR
	    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif
	}
	if ( redraw ) {
	    emit copyAvailable( doc->hasSelection( QTextDocument::Standard ) );
	    emit selectionChanged();
	}
    }

    drawCursor( TRUE );
    updateCurrentFormat();
    updateMicroFocusHint();
}

/*! \overload
*/

void QTextEdit::moveCursor( CursorAction action )
{
    switch ( action ) {
    case MoveBackward:
	cursor->gotoPreviousLetter();
	break;
    case MoveWordBackward:
	cursor->gotoPreviousWord();
	break;
    case MoveForward:
	cursor->gotoNextLetter();
	break;
    case MoveWordForward:
	cursor->gotoNextWord();
	break;
    case MoveUp:
	cursor->gotoUp();
	break;
    case MovePgUp:
	cursor->gotoPageUp( visibleHeight() );
	break;
    case MoveDown:
	cursor->gotoDown();
	break;
    case MovePgDown:
	cursor->gotoPageDown( visibleHeight() );
	break;
    case MoveLineStart:
	cursor->gotoLineStart();
	break;
    case MoveHome:
	cursor->gotoHome();
	break;
    case MoveLineEnd:
	cursor->gotoLineEnd();
	break;
    case MoveEnd:
	ensureFormatted( doc->lastParagraph() );
	cursor->gotoEnd();
	break;
    }
    updateMicroFocusHint();
    updateCurrentFormat();
}

/*! \reimp */

void QTextEdit::resizeEvent( QResizeEvent *e )
{
    QScrollView::resizeEvent( e );
    if ( doc->visibleWidth() == 0 )
	doResize();
}

/*! \reimp */

void QTextEdit::viewportResizeEvent( QResizeEvent *e )
{
    QScrollView::viewportResizeEvent( e );
    if ( e->oldSize().width() != e->size().width() ) {
	bool stayAtBottom = e->oldSize().height() != e->size().height() &&
	       contentsY() > 0 && contentsY() >= doc->height() - e->oldSize().height();
	doResize();
	if ( stayAtBottom )
	    scrollToBottom();
    }
}

/*!
  Ensures that the cursor is visible by scrolling the text edit if
  necessary.

  \sa setCursorPosition()
*/

void QTextEdit::ensureCursorVisible()
{
    if ( !isVisible() ) {
	d->ensureCursorVisibleInShowEvent = TRUE;
	return;
    }
    lastFormatted = cursor->paragraph();
    formatMore();
    QTextStringChar *chr = cursor->paragraph()->at( cursor->index() );
    int h = cursor->paragraph()->lineHeightOfChar( cursor->index() );
    int x = cursor->paragraph()->rect().x() + chr->x + cursor->offsetX();
    int y = 0; int dummy;
    cursor->paragraph()->lineHeightOfChar( cursor->index(), &dummy, &y );
    y += cursor->paragraph()->rect().y() + cursor->offsetY();
    int w = 1;
    ensureVisible( x, y + h / 2, w, h / 2 + 2 );
}

/*!
    \internal
*/
void QTextEdit::drawCursor( bool visible )
{
    if ( !isUpdatesEnabled() ||
	 !viewport()->isUpdatesEnabled() ||
	 !cursor->paragraph() ||
	 !cursor->paragraph()->isValid() ||
	 !selectedText().isEmpty() ||
	 ( visible && !hasFocus() && !viewport()->hasFocus() && !inDnD ) ||
	 isReadOnly() )
	return;

    QPainter p( viewport() );
    QRect r( cursor->topParagraph()->rect() );
    cursor->paragraph()->setChanged( TRUE );
    p.translate( -contentsX() + cursor->totalOffsetX(), -contentsY() + cursor->totalOffsetY() );
    QPixmap *pix = 0;
    QColorGroup cg( colorGroup() );
    if ( cursor->paragraph()->background() )
	cg.setBrush( QColorGroup::Base, *cursor->paragraph()->background() );
    else if ( doc->paper() )
	cg.setBrush( QColorGroup::Base, *doc->paper() );
    p.setBrushOrigin( -contentsX(), -contentsY() );
    cursor->paragraph()->document()->nextDoubleBuffered = TRUE;
    if ( !cursor->nestedDepth() ) {
	int h = cursor->paragraph()->lineHeightOfChar( cursor->index() );
	int dist = 5;
	if ( ( cursor->paragraph()->alignment() & Qt3::AlignJustify ) == Qt3::AlignJustify )
	    dist = 50;
	int x = r.x() - cursor->totalOffsetX() + cursor->x() - dist;
	x = QMAX( x, 0 );
	p.setClipRect( QRect( x - contentsX(),
			      r.y() - cursor->totalOffsetY() + cursor->y() - contentsY(), 2 * dist, h ) );
	doc->drawParagraph( &p, cursor->paragraph(), x,
			r.y() - cursor->totalOffsetY() + cursor->y(), 2 * dist, h, pix, cg, visible, cursor );
    } else {
	doc->drawParagraph( &p, cursor->paragraph(), r.x() - cursor->totalOffsetX(),
			r.y() - cursor->totalOffsetY(), r.width(), r.height(),
			pix, cg, visible, cursor );
    }
    cursorVisible = visible;
}

enum {
    IdUndo = 0,
    IdRedo = 1,
    IdCut = 2,
    IdCopy = 3,
    IdPaste = 4,
    IdClear = 5,
    IdSelectAll = 6
};

/*! \reimp */
#ifndef QT_NO_WHEELEVENT
void QTextEdit::contentsWheelEvent( QWheelEvent *e )
{
    if ( isReadOnly() ) {
	if ( e->state() & ControlButton ) {
	    if ( e->delta() > 0 )
		zoomOut();
	    else if ( e->delta() < 0 )
		zoomIn();
	    return;
	}
    }
    QScrollView::contentsWheelEvent( e );
}
#endif

/*! \reimp */

void QTextEdit::contentsMousePressEvent( QMouseEvent *e )
{
    clearUndoRedo();
    QTextCursor oldCursor = *cursor;
    QTextCursor c = *cursor;
    mousePos = e->pos();
    mightStartDrag = FALSE;
    pressedLink = QString::null;

    if ( e->button() == LeftButton ) {
	mousePressed = TRUE;
	drawCursor( FALSE );
	placeCursor( e->pos() );
	ensureCursorVisible();

	if ( isReadOnly() && linksEnabled() ) {
	    QTextCursor c = *cursor;
	    placeCursor( e->pos(), &c, TRUE );
	    if ( c.paragraph() && c.paragraph()->at( c.index() ) &&
		 c.paragraph()->at( c.index() )->isAnchor() ) {
		pressedLink = c.paragraph()->at( c.index() )->anchorHref();
	    }
	}

#ifndef QT_NO_DRAGANDDROP
	if ( doc->inSelection( QTextDocument::Standard, e->pos() ) ) {
	    mightStartDrag = TRUE;
	    drawCursor( TRUE );
	    dragStartTimer->start( QApplication::startDragTime(), TRUE );
	    dragStartPos = e->pos();
	    return;
	}
#endif

	bool redraw = FALSE;
	if ( doc->hasSelection( QTextDocument::Standard ) ) {
	    if ( !( e->state() & ShiftButton ) ) {
		redraw = doc->removeSelection( QTextDocument::Standard );
		doc->setSelectionStart( QTextDocument::Standard, *cursor );
	    } else {
		redraw = doc->setSelectionEnd( QTextDocument::Standard, *cursor ) || redraw;
	    }
	} else {
	    if ( isReadOnly() || !( e->state() & ShiftButton ) ) {
		doc->setSelectionStart( QTextDocument::Standard, *cursor );
	    } else {
		doc->setSelectionStart( QTextDocument::Standard, c );
		redraw = doc->setSelectionEnd( QTextDocument::Standard, *cursor ) || redraw;
	    }
	}

	for ( int i = 1; i < doc->numSelections(); ++i ) // start with 1 as we don't want to remove the Standard-Selection
	    redraw = doc->removeSelection( i ) || redraw;

	if ( !redraw ) {
	    drawCursor( TRUE );
	} else {
	    repaintChanged();
#ifndef QT_NO_CURSOR
	    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif
	}
    } else if ( e->button() == MidButton ) {
	bool redraw = doc->removeSelection( QTextDocument::Standard );
	if ( !redraw ) {
	    drawCursor( TRUE );
	} else {
	    repaintChanged();
#ifndef QT_NO_CURSOR
	    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif
	}
    }

    if ( *cursor != oldCursor )
	updateCurrentFormat();
}

/*! \reimp */

void QTextEdit::contentsMouseMoveEvent( QMouseEvent *e )
{
    if ( mousePressed ) {
#ifndef QT_NO_DRAGANDDROP
	if ( mightStartDrag ) {
	    dragStartTimer->stop();
	    if ( ( e->pos() - dragStartPos ).manhattanLength() > QApplication::startDragDistance() )
		startDrag();
#ifndef QT_NO_CURSOR
	    if ( !isReadOnly() )
		viewport()->setCursor( ibeamCursor );
#endif
	    return;
	}
#endif
	mousePos = e->pos();
	handleMouseMove( mousePos );
	oldMousePos = mousePos;
    }

#ifndef QT_NO_CURSOR
    if ( !isReadOnly() && !mousePressed ) {
	if ( doc->hasSelection( QTextDocument::Standard ) && doc->inSelection( QTextDocument::Standard, e->pos() ) )
	    viewport()->setCursor( arrowCursor );
	else
	    viewport()->setCursor( ibeamCursor );
    }
#endif
    updateCursor( e->pos() );
}

/*! \reimp */

void QTextEdit::contentsMouseReleaseEvent( QMouseEvent * e )
{
    QTextCursor oldCursor = *cursor;
    if ( scrollTimer->isActive() )
	scrollTimer->stop();
#ifndef QT_NO_DRAGANDDROP
    if ( dragStartTimer->isActive() )
	dragStartTimer->stop();
    if ( mightStartDrag ) {
	selectAll( FALSE );
	mousePressed = FALSE;
    }
#endif
    if ( mousePressed ) {
	mousePressed = FALSE;
    }
    emit cursorPositionChanged( cursor );
    emit cursorPositionChanged( cursor->paragraph()->paragId(), cursor->index() );
    if ( oldCursor != *cursor )
	updateCurrentFormat();
    inDoubleClick = FALSE;

#ifndef QT_NO_NETWORKPROTOCOL
    if ( !onLink.isEmpty() && onLink == pressedLink && linksEnabled() ) {
	QUrl u( doc->context(), onLink, TRUE );
	emitLinkClicked( u.toString( FALSE, FALSE ) );

	// emitting linkClicked() may result in that the cursor winds
	// up hovering over a different valid link - check this and
	// set the appropriate cursor shape
	updateCursor( e->pos() );
    }
#endif
    drawCursor( TRUE );
    if ( !doc->hasSelection( QTextDocument::Standard, TRUE ) )
	doc->removeSelection( QTextDocument::Standard );

    emit copyAvailable( doc->hasSelection( QTextDocument::Standard ) );
    emit selectionChanged();
}

/*! \reimp */

void QTextEdit::contentsMouseDoubleClickEvent( QMouseEvent * )
{
    QTextCursor c1 = *cursor;
    QTextCursor c2 = *cursor;
    if ( cursor->index() > 0 && !cursor->paragraph()->at( cursor->index()-1 )->c.isSpace() )
	c1.gotoPreviousWord();
    if ( !cursor->paragraph()->at( cursor->index() )->c.isSpace() && !cursor->atParagEnd() )
	c2.gotoNextWord();

    doc->setSelectionStart( QTextDocument::Standard, c1 );
    doc->setSelectionEnd( QTextDocument::Standard, c2 );

    *cursor = c2;

    repaintChanged();

    inDoubleClick = TRUE;
    mousePressed = TRUE;
}

#ifndef QT_NO_DRAGANDDROP

/*! \reimp */

void QTextEdit::contentsDragEnterEvent( QDragEnterEvent *e )
{
    if ( isReadOnly() || !QTextDrag::canDecode( e ) ) {
	e->ignore();
	return;
    }
    e->acceptAction();
    inDnD = TRUE;
}

/*! \reimp */

void QTextEdit::contentsDragMoveEvent( QDragMoveEvent *e )
{
    if ( isReadOnly() || !QTextDrag::canDecode( e ) ) {
	e->ignore();
	return;
    }
    drawCursor( FALSE );
    placeCursor( e->pos(),  cursor );
    drawCursor( TRUE );
    e->acceptAction();
}

/*! \reimp */

void QTextEdit::contentsDragLeaveEvent( QDragLeaveEvent * )
{
    inDnD = FALSE;
}

/*! \reimp */

void QTextEdit::contentsDropEvent( QDropEvent *e )
{
    if ( isReadOnly() )
	return;
    inDnD = FALSE;
    e->acceptAction();
    QString text;
    bool intern = FALSE;
    if ( QTextDrag::decode( e, text ) ) {
	bool hasSel = doc->hasSelection( QTextDocument::Standard );
	bool internalDrag = e->source() == this || e->source() == viewport();
	int dropId, dropIndex;
	QTextCursor insertCursor = *cursor;
	dropId = cursor->paragraph()->paragId();
	dropIndex = cursor->index();
	if ( hasSel && internalDrag ) {
	    QTextCursor c1, c2;
	    int selStartId, selStartIndex;
	    int selEndId, selEndIndex;
	    c1 = doc->selectionStartCursor( QTextDocument::Standard );
	    c1.restoreState();
	    c2 = doc->selectionEndCursor( QTextDocument::Standard );
	    c2.restoreState();
	    selStartId = c1.paragraph()->paragId();
	    selStartIndex = c1.index();
	    selEndId = c2.paragraph()->paragId();
	    selEndIndex = c2.index();
	    if ( ( ( dropId > selStartId ) ||
		   ( dropId == selStartId && dropIndex > selStartIndex ) ) &&
		 ( ( dropId < selEndId ) ||
		   ( dropId == selEndId && dropIndex <= selEndIndex ) ) )
		insertCursor = c1;
	    if ( dropId == selEndId && dropIndex > selEndIndex ) {
		insertCursor = c1;
		if ( selStartId == selEndId ) {
		    insertCursor.setIndex( dropIndex -
					   ( selEndIndex - selStartIndex ) );
		} else {
		    insertCursor.setIndex( dropIndex - selEndIndex +
					   selStartIndex );
		}
	    }
	 }

	if ( internalDrag && e->action() == QDropEvent::Move ) {
	    removeSelectedText();
	    intern = TRUE;
	} else {
	    doc->removeSelection( QTextDocument::Standard );
#ifndef QT_NO_CURSOR
	    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif
	}
	drawCursor( FALSE );
	cursor->setParagraph( insertCursor.paragraph() );
	cursor->setIndex( insertCursor.index() );
	drawCursor( TRUE );
	if ( !cursor->nestedDepth() ) {
	    insert( text, FALSE, TRUE, FALSE );
	} else {
	    if ( intern )
		undo();
	    e->ignore();
	}
    }
}

#endif

void QTextEdit::autoScrollTimerDone()
{
    if ( mousePressed )
	handleMouseMove(  viewportToContents( viewport()->mapFromGlobal( QCursor::pos() )  ) );
}

void QTextEdit::handleMouseMove( const QPoint& pos )
{
    if ( !mousePressed )
	return;

    if ( !scrollTimer->isActive() && pos.y() < contentsY() || pos.y() > contentsY() + visibleHeight() )
	scrollTimer->start( 100, FALSE );
    else if ( scrollTimer->isActive() && pos.y() >= contentsY() && pos.y() <= contentsY() + visibleHeight() )
	scrollTimer->stop();

    drawCursor( FALSE );
    QTextCursor oldCursor = *cursor;

    placeCursor( pos );

    if ( inDoubleClick ) {
	QTextCursor cl = *cursor;
	cl.gotoPreviousWord();
	QTextCursor cr = *cursor;
	cr.gotoNextWord();

	int diff = QABS( oldCursor.paragraph()->at( oldCursor.index() )->x - mousePos.x() );
	int ldiff = QABS( cl.paragraph()->at( cl.index() )->x - mousePos.x() );
	int rdiff = QABS( cr.paragraph()->at( cr.index() )->x - mousePos.x() );


	if ( cursor->paragraph()->lineStartOfChar( cursor->index() ) !=
	     oldCursor.paragraph()->lineStartOfChar( oldCursor.index() ) )
	    diff = 0xFFFFFF;

	if ( rdiff < diff && rdiff < ldiff )
	    *cursor = cr;
	else if ( ldiff < diff && ldiff < rdiff )
	    *cursor = cl;
	else
	    *cursor = oldCursor;

    }
    ensureCursorVisible();

    bool redraw = FALSE;
    if ( doc->hasSelection( QTextDocument::Standard ) ) {
	redraw = doc->setSelectionEnd( QTextDocument::Standard, *cursor ) || redraw;
    }

    if ( !redraw ) {
	drawCursor( TRUE );
    } else {
	repaintChanged();
	drawCursor( TRUE );
    }

    if ( currentFormat && currentFormat->key() != cursor->paragraph()->at( cursor->index() )->format()->key() ) {
	currentFormat->removeRef();
	currentFormat = doc->formatCollection()->format( cursor->paragraph()->at( cursor->index() )->format() );
	if ( currentFormat->isMisspelled() ) {
	    currentFormat->removeRef();
	    currentFormat = doc->formatCollection()->format( currentFormat->font(), currentFormat->color() );
	}
	emit currentFontChanged( currentFormat->font() );
	emit currentColorChanged( currentFormat->color() );
	emit currentVerticalAlignmentChanged( (VerticalAlignment)currentFormat->vAlign() );
    }

    if ( currentAlignment != cursor->paragraph()->alignment() ) {
	currentAlignment = cursor->paragraph()->alignment();
	block_set_alignment = TRUE;
	emit currentAlignmentChanged( currentAlignment );
	block_set_alignment = FALSE;
    }
}

/*!
  \fn void QTextEdit::placeCursor( const QPoint &pos, QTextCursor *c )
  Places the cursor \a c at the character which is closest to position
  \a pos (in contents coordinates). If \a c is 0, the default text
  cursor is used.

  \sa setCursorPosition()
*/

void QTextEdit::placeCursor( const QPoint &pos, QTextCursor *c, bool link )
{
    if ( !c )
	c = cursor;

    c->restoreState();
    QTextParagraph *s = doc->firstParagraph();
    c->place( pos, s, link );
    updateMicroFocusHint();
}


void QTextEdit::updateMicroFocusHint()
{
    QTextCursor c( *cursor );
    if ( d->preeditStart != -1 )
	c.setIndex( d->preeditStart );

    if ( hasFocus() || viewport()->hasFocus() ) {
	int h = c.paragraph()->lineHeightOfChar( cursor->index() );
	if ( !readonly ) {
	    QFont f = c.paragraph()->at( c.index() )->format()->font();
	    setMicroFocusHint( c.x() - contentsX() + frameWidth(),
			       c.y() + cursor->paragraph()->rect().y() - contentsY() + frameWidth(), 0, h, TRUE );
	}
    }
}



void QTextEdit::formatMore()
{
    if ( !lastFormatted )
	return;

    int bottom = contentsHeight();
    int lastBottom = -1;
    int to = 20;
    bool firstVisible = FALSE;
    QRect cr( contentsX(), contentsY(), visibleWidth(), visibleHeight() );
    for ( int i = 0; ( i < to || firstVisible ) && lastFormatted; ++i ) {
	lastFormatted->format();
	if ( i == 0 )
	    firstVisible = lastFormatted->rect().intersects( cr );
	else if ( firstVisible )
	    firstVisible = lastFormatted->rect().intersects( cr );
	bottom = QMAX( bottom, lastFormatted->rect().top() +
		       lastFormatted->rect().height() );
	lastBottom = lastFormatted->rect().top() + lastFormatted->rect().height();
	lastFormatted = lastFormatted->next();
	if ( lastFormatted )
	    lastBottom = -1;
    }

    if ( bottom > contentsHeight() ) {
	resizeContents( contentsWidth(), QMAX( doc->height(), bottom ) );
    } else if ( lastBottom != -1 && lastBottom < contentsHeight() ) {
 	resizeContents( contentsWidth(), QMAX( doc->height(), lastBottom ) );
	if ( contentsHeight() < visibleHeight() )
	    updateContents( 0, contentsHeight(), visibleWidth(),
			    visibleHeight() - contentsHeight() );
    }

    if ( lastFormatted )
	formatTimer->start( interval, TRUE );
    else
	interval = QMAX( 0, interval );
}

void QTextEdit::doResize()
{
    if ( wrapMode == FixedPixelWidth )
	return;
    doc->setMinimumWidth( -1 );
    resizeContents( 0, 0 );
    doc->setWidth( visibleWidth() );
    doc->invalidate();
    lastFormatted = doc->firstParagraph();
    interval = 0;
    formatMore();
    repaintContents( contentsX(), contentsY(), visibleWidth(), visibleHeight(), FALSE );
}

/*! \internal */

void QTextEdit::doChangeInterval()
{
    interval = 0;
}

/*! \reimp */

bool QTextEdit::eventFilter( QObject *o, QEvent *e )
{
    if ( o == this || o == viewport() ) {
	if ( e->type() == QEvent::FocusIn ) {
	    blinkTimer->start( QApplication::cursorFlashTime() / 2 );
	    drawCursor( TRUE );
	    updateMicroFocusHint();
	} else if ( e->type() == QEvent::FocusOut ) {
	    blinkTimer->stop();
	    drawCursor( FALSE );
	}
    }

    return QScrollView::eventFilter( o, e );
}

/*!  Inserts \a text at the current cursor position. If \a indent is
  TRUE, the paragraph is re-indented. If \a checkNewLine is TRUE,
  newline characters in \a text result in hard line breaks (i.e. new
  paragraphs). If \a checkNewLine is FALSE and there are newlines in
  \a text, the behavior is undefined. If \a checkNewLine is FALSE the
  behaviour of the editor is undefined if the \a text contains
  newlines. If \a removeSelected is TRUE, any selected text (in
  selection 0) is removed before the text is inserted.

  \sa paste() pasteSubType()
*/

void QTextEdit::insert( const QString &text, bool indent, bool checkNewLine, bool removeSelected )
{
    if ( cursor->nestedDepth() != 0 ) // #### for 3.0, disable editing of tables as this is not advanced enough
	return;
    QString txt( text );
    drawCursor( FALSE );
    if ( !isReadOnly() && doc->hasSelection( QTextDocument::Standard ) && removeSelected )
	removeSelectedText();
    QTextCursor c2 = *cursor;
    int oldLen = 0;

    if ( undoEnabled && !isReadOnly() ) {
	checkUndoRedoInfo( UndoRedoInfo::Insert );
	if ( !undoRedoInfo.valid() ) {
	    undoRedoInfo.id = cursor->paragraph()->paragId();
	    undoRedoInfo.index = cursor->index();
	    undoRedoInfo.d->text = QString::null;
	}
	oldLen = undoRedoInfo.d->text.length();
    }

    lastFormatted = checkNewLine && cursor->paragraph()->prev() ?
		    cursor->paragraph()->prev() : cursor->paragraph();
    QTextCursor oldCursor = *cursor;
    cursor->insert( txt, checkNewLine );
    if ( doc->useFormatCollection() ) {
	doc->setSelectionStart( QTextDocument::Temp, oldCursor );
	doc->setSelectionEnd( QTextDocument::Temp, *cursor );
	doc->setFormat( QTextDocument::Temp, currentFormat, QTextFormat::Format );
	doc->removeSelection( QTextDocument::Temp );
    }

    if ( indent && ( txt == "{" || txt == "}" || txt == ":" || txt == "#" ) )
	cursor->indent();
    formatMore();
    repaintChanged();
    ensureCursorVisible();
    drawCursor( TRUE );

    if ( undoEnabled && !isReadOnly() ) {
	undoRedoInfo.d->text += txt;
	if ( !doc->preProcessor() ) {
	    for ( int i = 0; i < (int)txt.length(); ++i ) {
		if ( txt[ i ] != '\n' && c2.paragraph()->at( c2.index() )->format() ) {
		    c2.paragraph()->at( c2.index() )->format()->addRef();
		    undoRedoInfo.d->text.setFormat( oldLen + i, c2.paragraph()->at( c2.index() )->format(), TRUE );
		}
		c2.gotoNextLetter();
	    }
	}
    }

    if ( !removeSelected ) {
	doc->setSelectionStart( QTextDocument::Standard, oldCursor );
	doc->setSelectionEnd( QTextDocument::Standard, *cursor );
	repaintChanged();
    }
    updateMicroFocusHint();
    setModified();
    emit textChanged();
}

/*! Inserts \a text in the paragraph \a para and position \a index */

void QTextEdit::insertAt( const QString &text, int para, int index )
{
    removeSelection( QTextDocument::Standard );
    QTextParagraph *p = doc->paragAt( para );
    if ( !p )
	return;
    QTextCursor tmp = *cursor;
    cursor->setParagraph( p );
    cursor->setIndex( index );
    insert( text, FALSE, TRUE, FALSE );
    *cursor = tmp;
    removeSelection( QTextDocument::Standard );
}

/*! Inserts \a text as the paragraph at position \a para. If \a para
  is -1, the text is appended.
*/

void QTextEdit::insertParagraph( const QString &text, int para )
{
    QTextParagraph *p = doc->paragAt( para );
    if ( p ) {
	QTextCursor tmp( doc );
	tmp.setParagraph( p );
	tmp.setIndex( 0 );
	tmp.insert( text, TRUE );
	tmp.splitAndInsertEmptyParagraph();
	repaintChanged();
    } else {
	append( text );
    }
}

/*! Removes the paragraph \a para */

void QTextEdit::removeParagraph( int para )
{
    QTextParagraph *p = doc->paragAt( para );
    if ( !p )
	return;
    for ( int i = 0; i < doc->numSelections(); ++i )
	doc->removeSelection( i );

    if ( p == doc->firstParagraph() && p == doc->lastParagraph() ) {
	p->remove( 0, p->length() - 1 );
	repaintChanged();
	return;
    }
    drawCursor( FALSE );
    bool resetCursor = cursor->paragraph() == p;
    if ( p->prev() )
	p->prev()->setNext( p->next() );
    else
	doc->setFirstParagraph( p->next() );
    if ( p->next() )
	p->next()->setPrev( p->prev() );
    else
	doc->setLastParagraph( p->prev() );
    QTextParagraph *start = p->next();
    int h = p->rect().height();
    delete p;
    p = start;
    int dy = -h;
    while ( p ) {
	p->setParagId( p->prev() ? p->prev()->paragId() + 1 : 0 );
	p->move( dy );
	p->invalidate( 0 );
	p->setEndState( -1 );
	p = p->next();
    }

    if ( resetCursor ) {
	cursor->setParagraph( doc->firstParagraph() );
	cursor->setIndex( 0 );
    }
    repaintChanged();
    drawCursor( TRUE );
}

/*!
  Undoes the last operation.

  If there is no operation to undo, e.g. there is no undo step in the
  undo/redo history, nothing happens.

  \sa undoAvailable() redo() undoDepth()
*/

void QTextEdit::undo()
{
    // XXX FIXME The next line is here because there may be a command
    // that needs to be 'flushed'.  The FIXME is because I am not
    // 100% certain this is the right call to do this.
    clearUndoRedo();
    if ( isReadOnly() || !doc->commands()->isUndoAvailable() || !undoEnabled )
	return;

    for ( int i = 0; i < (int)doc->numSelections(); ++i )
	doc->removeSelection( i );

#ifndef QT_NO_CURSOR
    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif

    clearUndoRedo();
    drawCursor( FALSE );
    QTextCursor *c = doc->undo( cursor );
    if ( !c ) {
	drawCursor( TRUE );
	return;
    }
    lastFormatted = 0;
    ensureCursorVisible();
    repaintChanged();
    drawCursor( TRUE );
    updateMicroFocusHint();
    setModified();
    emit textChanged();
}

/*!
  Redoes the last operation.

  If there is no operation to redo, e.g. there is no redo step in the
  undo/redo history, nothing happens.

  \sa redoAvailable() undo() undoDepth()
*/

void QTextEdit::redo()
{
    if ( isReadOnly() || !doc->commands()->isRedoAvailable() || !undoEnabled )
	return;

    for ( int i = 0; i < (int)doc->numSelections(); ++i )
	doc->removeSelection( i );

#ifndef QT_NO_CURSOR
    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif

    clearUndoRedo();
    drawCursor( FALSE );
    QTextCursor *c = doc->redo( cursor );
    if ( !c ) {
	drawCursor( TRUE );
	return;
    }
    lastFormatted = 0;
    ensureCursorVisible();
    repaintChanged();
    ensureCursorVisible();
    drawCursor( TRUE );
    updateMicroFocusHint();
    setModified();
    emit textChanged();
}

/*!
    Pastes the text from the clipboard into the text edit at the current
    cursor position. Only plain text is pasted.

    If there is no text in the clipboard nothing happens.

    \sa pasteSubType() cut() QTextEdit::copy()
*/

void QTextEdit::paste()
{
#ifndef QT_NO_CLIPBOARD
    if ( isReadOnly() )
	return;
    pasteSubType( "plain" );
    updateMicroFocusHint();
#endif
}

void QTextEdit::checkUndoRedoInfo( UndoRedoInfo::Type t )
{
    if ( undoRedoInfo.valid() && t != undoRedoInfo.type ) {
	clearUndoRedo();
    }
    undoRedoInfo.type = t;
}

/*! Repaints any paragraphs that have changed.

    Although used extensively internally you shouldn't need to call this
    yourself.
*/

void QTextEdit::repaintChanged()
{
    if ( !isUpdatesEnabled() || !viewport()->isUpdatesEnabled() )
	return;
    QPainter p( viewport() );
    p.translate( -contentsX(), -contentsY() );
    paintDocument( FALSE, &p, contentsX(), contentsY(), visibleWidth(), visibleHeight() );
}

/*!
    Copies the selected text (from selection 0) to the clipboard and
    deletes it from the text edit.

    If there is no selected text (in selection 0) nothing happens.

    \sa QTextEdit::copy() paste() pasteSubType()
*/

void QTextEdit::cut()
{
    if ( isReadOnly() )
	return;

    QString t;
    if ( doc->hasSelection( QTextDocument::Standard ) &&
	 !( t = doc->selectedText( QTextDocument::Standard, qt_enable_richtext_copy ) ).isEmpty() ) {
	QApplication::clipboard()->setText( t );
	removeSelectedText();
    }
    updateMicroFocusHint();
}

/*! Copies any selected text (from selection 0) to the clipboard.

    \sa hasSelectedText() copyAvailable()
 */

void QTextEdit::copy()
{
    QString t = doc->selectedText( QTextDocument::Standard, qt_enable_richtext_copy );
    if ( doc->hasSelection( QTextDocument::Standard ) &&
	 !t.isEmpty() && t.simplifyWhiteSpace() != "<selstart/>" )
	QApplication::clipboard()->setText( t );
}

/*!
  Re-indents the current paragraph.
*/

void QTextEdit::indent()
{
    if ( isReadOnly() )
	return;

    drawCursor( FALSE );
    if ( !doc->hasSelection( QTextDocument::Standard ) )
	cursor->indent();
    else
	doc->indentSelection( QTextDocument::Standard );
    repaintChanged();
    drawCursor( TRUE );
    setModified();
    emit textChanged();
}

/*! Reimplemented to allow tabbing through links.
    If \a n is TRUE the tab moves the focus to the next child; if \a n
    is FALSE the tab moves the focus to the previous child.
    Returns TRUE if the focus was moved; otherwise returns FALSE.
 */

bool QTextEdit::focusNextPrevChild( bool n )
{
    if ( !isReadOnly() || !linksEnabled() )
	return FALSE;
    bool b = doc->focusNextPrevChild( n );
    repaintChanged();
    if ( b )
	//##### this does not work with tables. The focusIndicator
	//should really be a QTextCursor. Fix 3.1
	makeParagVisible( doc->focusIndicator.parag );
    return b;
}

/*!
    \internal

  This functions sets the current format to \a f. Only the fields of \a
  f which are specified by the \a flags are used.
*/

void QTextEdit::setFormat( QTextFormat *f, int flags )
{
    if ( doc->hasSelection( QTextDocument::Standard ) ) {
	drawCursor( FALSE );
	QTextCursor c1 = doc->selectionStartCursor( QTextDocument::Standard );
	c1.restoreState();
	QTextCursor c2 = doc->selectionEndCursor( QTextDocument::Standard );
	c2.restoreState();
	clearUndoRedo();
	undoRedoInfo.type = UndoRedoInfo::Format;
	undoRedoInfo.id = c1.paragraph()->paragId();
	undoRedoInfo.index = c1.index();
	undoRedoInfo.eid = c2.paragraph()->paragId();
	undoRedoInfo.eindex = c2.index();
	readFormats( c1, c2, undoRedoInfo.d->text );
	undoRedoInfo.format = f;
	undoRedoInfo.flags = flags;
	clearUndoRedo();
	doc->setFormat( QTextDocument::Standard, f, flags );
	repaintChanged();
	formatMore();
	drawCursor( TRUE );
	setModified();
	emit textChanged();
    }
    if ( currentFormat && currentFormat->key() != f->key() ) {
	currentFormat->removeRef();
	currentFormat = doc->formatCollection()->format( f );
	if ( currentFormat->isMisspelled() ) {
	    currentFormat->removeRef();
	    currentFormat = doc->formatCollection()->format( currentFormat->font(), currentFormat->color() );
	}
	emit currentFontChanged( currentFormat->font() );
	emit currentColorChanged( currentFormat->color() );
	emit currentVerticalAlignmentChanged( (VerticalAlignment)currentFormat->vAlign() );
	if ( cursor->index() == cursor->paragraph()->length() - 1 ) {
	    currentFormat->addRef();
	    cursor->paragraph()->string()->setFormat( cursor->index(), currentFormat, TRUE );
	    if ( cursor->paragraph()->length() == 1 ) {
		cursor->paragraph()->invalidate( 0 );
		cursor->paragraph()->format();
		repaintChanged();
	    }
	}
    }
}

/*! \reimp */

void QTextEdit::setPalette( const QPalette &p )
{
    QScrollView::setPalette( p );
    if ( textFormat() == PlainText ) {
	QTextFormat *f = doc->formatCollection()->defaultFormat();
	f->setColor( colorGroup().text() );
	updateContents( contentsX(), contentsY(), visibleWidth(), visibleHeight() );
    }
}

/*! \internal

  \warning In Qt 3.1 we will provide a cleaer API for the
  functionality which is provided by this function and in Qt 4.0 this
  function will go away.

  Sets the paragraph style of the current paragraph
  to \a dm. If \a dm is QStyleSheetItem::DisplayListItem, the
  type of the list item is set to \a listStyle.

  \sa setAlignment()
*/

void QTextEdit::setParagType( QStyleSheetItem::DisplayMode dm, QStyleSheetItem::ListStyle listStyle )
{
    if ( isReadOnly() )
	return;

    drawCursor( FALSE );
    QTextParagraph *start = cursor->paragraph();
    QTextParagraph *end = start;
    if ( doc->hasSelection( QTextDocument::Standard ) ) {
	start = doc->selectionStartCursor( QTextDocument::Standard ).topParagraph();
	end = doc->selectionEndCursor( QTextDocument::Standard ).topParagraph();
	if ( end->paragId() < start->paragId() )
	    return; // do not trust our selections
    }

    clearUndoRedo();
    undoRedoInfo.type = UndoRedoInfo::Style;
    undoRedoInfo.id = start->paragId();
    undoRedoInfo.eid = end->paragId();
    undoRedoInfo.styleInformation = QTextStyleCommand::readStyleInformation( doc, undoRedoInfo.id, undoRedoInfo.eid );

    while ( start != end->next() ) {
	start->setListStyle( listStyle );
	if ( dm == QStyleSheetItem::DisplayListItem ) {
	    start->setListItem( TRUE );
	    if( start->listDepth() == 0 )
		start->setListDepth( 1 );
	} else if ( start->isListItem() ) {
	    start->setListItem( FALSE );
	    start->setListDepth( QMAX( start->listDepth()-1, 0 ) );
	}
	start = start->next();
    }

    clearUndoRedo();
    repaintChanged();
    formatMore();
    drawCursor( TRUE );
    setModified();
    emit textChanged();
}

/*!
  Sets the alignment of the current paragraph to \a a. Valid alignments
  are \c Qt::AlignLeft, \c Qt::AlignRight, Qt::AlignJustify and
  Qt::AlignCenter (which centers horizontally).

*/

void QTextEdit::setAlignment( int a )
{
    if ( isReadOnly() || block_set_alignment )
	return;

    drawCursor( FALSE );
    QTextParagraph *start = cursor->paragraph();
    QTextParagraph *end = start;
    if ( doc->hasSelection( QTextDocument::Standard ) ) {
	start = doc->selectionStartCursor( QTextDocument::Standard ).topParagraph();
	end = doc->selectionEndCursor( QTextDocument::Standard ).topParagraph();
	if ( end->paragId() < start->paragId() )
	    return; // do not trust our selections
    }

    clearUndoRedo();
    undoRedoInfo.type = UndoRedoInfo::Style;
    undoRedoInfo.id = start->paragId();
    undoRedoInfo.eid = end->paragId();
    undoRedoInfo.styleInformation = QTextStyleCommand::readStyleInformation( doc, undoRedoInfo.id, undoRedoInfo.eid );

    while ( start != end->next() ) {
	start->setAlignment( a );
	start = start->next();
    }

    clearUndoRedo();
    repaintChanged();
    formatMore();
    drawCursor( TRUE );
    if ( currentAlignment != a ) {
	currentAlignment = a;
	emit currentAlignmentChanged( currentAlignment );
    }
    setModified();
    emit textChanged();
}

void QTextEdit::updateCurrentFormat()
{
    int i = cursor->index();
    if ( i > 0 )
	--i;
    if ( doc->useFormatCollection() &&
	 ( !currentFormat || currentFormat->key() != cursor->paragraph()->at( i )->format()->key() ) ) {
	if ( currentFormat )
	    currentFormat->removeRef();
	currentFormat = doc->formatCollection()->format( cursor->paragraph()->at( i )->format() );
	if ( currentFormat->isMisspelled() ) {
	    currentFormat->removeRef();
	    currentFormat = doc->formatCollection()->format( currentFormat->font(), currentFormat->color() );
	}
	emit currentFontChanged( currentFormat->font() );
	emit currentColorChanged( currentFormat->color() );
	emit currentVerticalAlignmentChanged( (VerticalAlignment)currentFormat->vAlign() );
    }

    if ( currentAlignment != cursor->paragraph()->alignment() ) {
	currentAlignment = cursor->paragraph()->alignment();
	block_set_alignment = TRUE;
	emit currentAlignmentChanged( currentAlignment );
	block_set_alignment = FALSE;
    }
}

/*!
    If \a b is TRUE sets the current format to italic; otherwise sets
    the current format to non-italic.

    \sa italic()
*/

void QTextEdit::setItalic( bool b )
{
    QTextFormat f( *currentFormat );
    f.setItalic( b );
    QTextFormat *f2 = doc->formatCollection()->format( &f );
    setFormat( f2, QTextFormat::Italic );
}

/*!
    If \a b is TRUE sets the current format to bold; otherwise sets the
    current format to non-bold.

    \sa bold()
*/

void QTextEdit::setBold( bool b )
{
    QTextFormat f( *currentFormat );
    f.setBold( b );
    QTextFormat *f2 = doc->formatCollection()->format( &f );
    setFormat( f2, QTextFormat::Bold );
}

/*!
    If \a b is TRUE sets the current format to underline; otherwise sets
    the current format to non-underline.

    \sa underline()
*/

void QTextEdit::setUnderline( bool b )
{
    QTextFormat f( *currentFormat );
    f.setUnderline( b );
    QTextFormat *f2 = doc->formatCollection()->format( &f );
    setFormat( f2, QTextFormat::Underline );
}

/*!
  Sets the font family of the current format to \a fontFamily.

  \sa family() setCurrentFont()
*/

void QTextEdit::setFamily( const QString &fontFamily )
{
    QTextFormat f( *currentFormat );
    f.setFamily( fontFamily );
    QTextFormat *f2 = doc->formatCollection()->format( &f );
    setFormat( f2, QTextFormat::Family );
}

/*!
  Sets the point size of the current format to \a s.

  Note that if \a s is zero or negative, the behaviour of this
  function is not defined.

  \sa pointSize() setCurrentFont() setFamily()
*/

void QTextEdit::setPointSize( int s )
{
    QTextFormat f( *currentFormat );
    f.setPointSize( s );
    QTextFormat *f2 = doc->formatCollection()->format( &f );
    setFormat( f2, QTextFormat::Size );
}

/*!
  Sets the color of the current format, i.e. of the text, to \a c.

  \sa color() setPaper()
*/

void QTextEdit::setColor( const QColor &c )
{
    QTextFormat f( *currentFormat );
    f.setColor( c );
    QTextFormat *f2 = doc->formatCollection()->format( &f );
    setFormat( f2, QTextFormat::Color );
}

/*!
  Sets the vertical alignment of the current format, i.e. of the text, to \a a.

  \sa color() setPaper()
*/

void QTextEdit::setVerticalAlignment( VerticalAlignment a )
{
    QTextFormat f( *currentFormat );
    f.setVAlign( (QTextFormat::VerticalAlignment)a );
    QTextFormat *f2 = doc->formatCollection()->format( &f );
    setFormat( f2, QTextFormat::VAlign );
}

void QTextEdit::setFontInternal( const QFont &f_ )
{
    QTextFormat f( *currentFormat );
    f.setFont( f_ );
    QTextFormat *f2 = doc->formatCollection()->format( &f );
    setFormat( f2, QTextFormat::Font );
}


QString QTextEdit::text() const
{
    if ( isReadOnly() )
	return doc->originalText();
    return doc->text();
}

/*!
    \overload
    Returns the text of paragraph \a para.

    If textFormat() is \c RichText the text will contain HTML
    formatting tags.
*/

QString QTextEdit::text( int para ) const
{
    return doc->text( para );
}

/*!
    \overload

  Changes the text of the text edit to the string \a text and the
  context to \a context. Any previous text is removed.

  \a text may be interpreted either as plain text or as rich text,
  depending on the textFormat(). The default setting is \c AutoText,
  i.e. the text edit autodetects the format from \a text.

  The optional \a context is a path which the text edit's
  QMimeSourceFactory uses to resolve the locations of files and images.
  (See \l{QTextEdit::QTextEdit()}.) It is passed to the text edit's
  QMimeSourceFactory when quering data.

  Note that the undo/redo history is cleared by this function.

  \sa text(), setTextFormat()
*/

void QTextEdit::setText( const QString &text, const QString &context )
{
    if ( !isModified() && isReadOnly() &&
	 this->context() == context && this->text() == text )
	return;

    emit undoAvailable( FALSE );
    emit redoAvailable( FALSE );
    undoRedoInfo.clear();
    doc->commands()->clear();

    lastFormatted = 0;
    cursor->restoreState();
    doc->setText( text, context );

    if ( wrapMode == FixedPixelWidth ) {
	resizeContents( wrapWidth, 0 );
	doc->setWidth( wrapWidth );
	doc->setMinimumWidth( wrapWidth );
    } else {
	doc->setMinimumWidth( -1 );
	resizeContents( 0, 0 );
    }

    lastFormatted = doc->firstParagraph();
    delete cursor;
    cursor = new QTextCursor( doc );
    updateContents( contentsX(), contentsY(), visibleWidth(), visibleHeight() );

    if ( isModified() )
	setModified( FALSE );
    emit textChanged();
    formatMore();
    updateCurrentFormat();
    d->scrollToAnchor = QString::null;
}

/*!
  \property QTextEdit::text
  \brief the text edit's text

    There is no default text.

  On setting, any previous text is deleted.

  The text may be interpreted either as plain text or as rich text,
  depending on the textFormat(). The default setting is \c AutoText,
  i.e. the text edit autodetects the format of the text.

  For richtext, calling text() on an editable QTextEdit will cause the text
  to be regenerated from the textedit.  This may mean that the QString returned
  may not be exactly the same as the one that was set.

  \sa textFormat
*/


/*!
  \property QTextEdit::readOnly
  \brief whether the text edit is read-only

  In a read-only text edit the user can only navigate through the text
  and select text; modifying the text is not possible.

  This property's default is FALSE.
*/

/*!
    Finds the next occurrence of the string, \a expr. Returns TRUE if
    \a expr is found; otherwise returns FALSE.

    If \a para and \a index are both null the search begins from the
    current cursor position. If \a para and \a index are both not
    null, the search begins from the \e *\a index character position
    in the \e *\a para paragraph.

    If \a cs is TRUE the search is case sensitive, otherwise it is
    case insensitive. If \a wo is TRUE the search looks for whole word
    matches only; otherwise it searches for any matching text. If \a
    forward is TRUE (the default) the search works forward from the
    starting position to the end of the text, otherwise it works
    backwards to the beginning of the text.

    If \a expr is found the function returns TRUE. If \a index and \a
    para are not null, the number of the paragraph in which the first
    character of the match was found is put into \e *\a para, and the
    index position of that character within the paragraph is put into
    \e *\a index.

    If \a expr is not found the function returns FALSE. If \a index
    and \a para are not null and \a expr is not found, \e *\a index
    and \e *\a para are undefined.
*/

bool QTextEdit::find( const QString &expr, bool cs, bool wo, bool forward,
		      int *para, int *index )
{
    drawCursor( FALSE );
#ifndef QT_NO_CURSOR
    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif
    QTextCursor findcur = *cursor;
    if ( para && index ) {
	if ( doc->paragAt( *para ) )
	    findcur.gotoPosition( doc->paragAt(*para), *index );
	else
	    findcur.gotoEnd();
    } else if ( doc->hasSelection( QTextDocument::Standard ) ){
	// maks sure we do not find the same selection again
	if ( forward )
	    findcur.gotoNextLetter();
	else
	    findcur.gotoPreviousLetter();
    }
    removeSelection( QTextDocument::Standard );
    bool found = doc->find( findcur, expr, cs, wo, forward );
    if ( found ) {
	if ( para )
	    *para = findcur.paragraph()->paragId();
	if ( index )
	    *index = findcur.index();
	*cursor = findcur;
	repaintChanged();
	ensureCursorVisible();
    }
    drawCursor( TRUE );
    return found;
}

void QTextEdit::blinkCursor()
{
    if ( !cursorVisible )
	return;
    bool cv = cursorVisible;
    blinkCursorVisible = !blinkCursorVisible;
    drawCursor( blinkCursorVisible );
    cursorVisible = cv;
}

/*!
  Sets the cursor to position \a index in paragraph \a para.

  \sa getCursorPosition()
*/

void QTextEdit::setCursorPosition( int para, int index )
{
    QTextParagraph *p = doc->paragAt( para );
    if ( !p )
	return;

    if ( index > p->length() - 1 )
	index = p->length() - 1;

    drawCursor( FALSE );
    cursor->setParagraph( p );
    cursor->setIndex( index );
    ensureCursorVisible();
    drawCursor( TRUE );
    updateCurrentFormat();
    emit cursorPositionChanged( cursor );
    emit cursorPositionChanged( cursor->paragraph()->paragId(), cursor->index() );
}

/*!
  This function sets the \e *\a para and \e *\a index parameters to the
  current cursor position. \a para and \a index must be non-null int
  pointers.

  \sa setCursorPosition()
 */

void QTextEdit::getCursorPosition( int *para, int *index ) const
{
    if ( !para || !index )
	return;
    *para = cursor->paragraph()->paragId();
    *index = cursor->index();
}

/*!  Sets a selection which starts at position \a indexFrom in
  paragraph \a paraFrom and ends at position \a indexTo in paragraph
  \a paraTo. Existing selections which have a different id (selNum)
  are not removed, existing selections which have the same id as \a
  selNum are removed.

  Uses the selection settings of selection \a selNum. If \a selNum is 0,
  this is the default selection.

  The cursor is moved to the end of the selection if \a selNum is 0,
  otherwise the cursor position remains unchanged.

  \sa getSelection() selectedText
*/

void QTextEdit::setSelection( int paraFrom, int indexFrom,
			      int paraTo, int indexTo, int selNum )
{
    if ( doc->hasSelection( selNum ) ) {
	doc->removeSelection( selNum );
	repaintChanged();
    }
    if ( selNum > doc->numSelections() - 1 )
	doc->addSelection( selNum );
    QTextParagraph *p1 = doc->paragAt( paraFrom );
    if ( !p1 )
	return;
    QTextParagraph *p2 = doc->paragAt( paraTo );
    if ( !p2 )
	return;

    if ( indexFrom > p1->length() - 1 )
	indexFrom = p1->length() - 1;
    if ( indexTo > p2->length() - 1 )
	indexTo = p2->length() - 1;

    drawCursor( FALSE );
    QTextCursor c = *cursor;
    QTextCursor oldCursor = *cursor;
    c.setParagraph( p1 );
    c.setIndex( indexFrom );
    cursor->setParagraph( p2 );
    cursor->setIndex( indexTo );
    doc->setSelectionStart( selNum, c );
    doc->setSelectionEnd( selNum, *cursor );
    repaintChanged();
    ensureCursorVisible();
    if ( selNum != QTextDocument::Standard )
	*cursor = oldCursor;
    drawCursor( TRUE );
}

/*!
    If there is a selection, \e *\a paraFrom is set to the number of the
    paragraph in which the selection begins and \e *\a paraTo is set to
    the number of the paragraph in which the selection ends. (They could
    be the same.) \e *\a indexFrom is set to the index at which the
    selection begins within \e *\a paraFrom, and \e *\a indexTo is set to
    the index at which the selection ends within \e *\a paraTo.

    If there is no selection, \e *\a paraFrom, \e *\a indexFrom, \e *\a
    paraTo and \e *\a indexTo are all set to -1.

    \a paraFrom, \a indexFrom, \a paraTo and \a indexTo must be non-null
    int pointers.

    The \a selNum is the number of the selection (multiple selections
    are supported). It defaults to 0 (the default selection).

    \sa setSelection() selectedText
*/

void QTextEdit::getSelection( int *paraFrom, int *indexFrom,
			      int *paraTo, int *indexTo, int selNum ) const
{
    if ( !paraFrom || !paraTo || !indexFrom || !indexTo )
	return;
    if ( !doc->hasSelection( selNum ) ) {
	*paraFrom = -1;
	*indexFrom = -1;
	*paraTo = -1;
	*indexTo = -1;
	return;
    }

    doc->selectionStart( selNum, *paraFrom, *indexFrom );
    doc->selectionEnd( selNum, *paraTo, *indexTo );
}

/*!
  \property QTextEdit::textFormat
  \brief the text format: rich text, plain text or auto text

  The text format is one of the following:
  \list
  \i PlainText - all characters, except newlines, are displayed
  verbatim, including spaces. Whenever a newline appears in the text the
  text edit inserts a hard line break and begins a new paragraph.
  \i RichText - rich text rendering. The available styles are
  defined in the default stylesheet QStyleSheet::defaultSheet().
  \i AutoText - this is the default. The text edit autodetects
  which rendering style is best, \c PlainText or \c RichText. This is
  done by using the QStyleSheet::mightBeRichText() function.
  \endlist
*/

void QTextEdit::setTextFormat( TextFormat format )
{
    doc->setTextFormat( format );
}

Qt::TextFormat QTextEdit::textFormat() const
{
    return doc->textFormat();
}

/*!
    Returns the number of paragraphs in the text; this could be 0.
*/

int QTextEdit::paragraphs() const
{
    return doc->lastParagraph()->paragId() + 1;
}

/*!
    Returns the number of lines in paragraph \a para, or -1 if there
    is no paragraph with index \a para.
*/

int QTextEdit::linesOfParagraph( int para ) const
{
    QTextParagraph *p = doc->paragAt( para );
    if ( !p )
	return -1;
    return p->lines();
}

/*!
    Returns the length of the paragraph \a para (number of
    characters), or -1 if there is no paragraph with index \a para
*/

int QTextEdit::paragraphLength( int para ) const
{
    QTextParagraph *p = doc->paragAt( para );
    if ( !p )
	return -1;
    return p->length() - 1;
}

/*!
    Returns the number of lines in the text edit; this could be 0.

    \warning This function may be slow. Lines change all the time
    during word wrapping, so this function has to iterate over all the
    paragraphs and get the number of lines from each one individually.
*/

int QTextEdit::lines() const
{
    QTextParagraph *p = doc->firstParagraph();
    int l = 0;
    while ( p ) {
	l += p->lines();
	p = p->next();
    }

    return l;
}

/*!
    Returns the line number of the line in paragraph \a para in which
    the character at position \a index appears. The \a index position is
    relative to the beginning of the paragraph. If there is no such
    paragraph or no such character at the \a index position (e.g. the
    index is out of range) -1 is returned.
*/

int QTextEdit::lineOfChar( int para, int index )
{
    QTextParagraph *p = doc->paragAt( para );
    if ( !p )
	return -1;

    int idx, line;
    QTextStringChar *c = p->lineStartOfChar( index, &idx, &line );
    if ( !c )
	return -1;

    return line;
}

void QTextEdit::setModified( bool m )
{
    bool oldModified = modified;
    modified = m;
    if ( modified && doc->oTextValid )
	doc->invalidateOriginalText();
    if ( oldModified != modified )
	emit modificationChanged( modified );
}

/*! \property QTextEdit::modified
  \brief whether the document has been modified by the user
*/

bool QTextEdit::isModified() const
{
    return modified;
}

void QTextEdit::setModified()
{
    if ( !isModified() )
	setModified( TRUE );
}

/*!
    Returns TRUE if the current format is italic; otherwise returns FALSE.

    \sa setItalic()
*/

bool QTextEdit::italic() const
{
    return currentFormat->font().italic();
}

/*!
    Returns TRUE if the current format is bold; otherwise returns FALSE.

    \sa setBold()
*/

bool QTextEdit::bold() const
{
    return currentFormat->font().bold();
}

/*!
    Returns TRUE if the current format is underlined; otherwise returns
    FALSE.

    \sa setUnderline()
*/

bool QTextEdit::underline() const
{
    return currentFormat->font().underline();
}

/*!
  Returns the font family of the current format.

  \sa setFamily() setCurrentFont() setPointSize()
*/

QString QTextEdit::family() const
{
    return currentFormat->font().family();
}

/*!
  Returns the point size of the font of the current format.

  \sa setFamily() setCurrentFont() setPointSize()

*/

int QTextEdit::pointSize() const
{
    return currentFormat->font().pointSize();
}

/*!
  Returns the color of the current format.

  \sa setColor() setPaper()
*/

QColor QTextEdit::color() const
{
    return currentFormat->color();
}

/*!
  Returns the font of the current format.

  \sa setCurrentFont() setFamily() setPointSize()

*/

QFont QTextEdit::font() const
{
    return currentFormat->font();
}

/*!
  Returns the alignment of the current paragraph.

  \sa setAlignment()
*/

int QTextEdit::alignment() const
{
    return currentAlignment;
}

void QTextEdit::startDrag()
{
#ifndef QT_NO_DRAGANDDROP
    mousePressed = FALSE;
    inDoubleClick = FALSE;
    QDragObject *drag = new QTextDrag( doc->selectedText( QTextDocument::Standard, qt_enable_richtext_copy ), viewport() );
    if ( isReadOnly() ) {
	drag->dragCopy();
    } else {
	if ( drag->drag() && QDragObject::target() != this && QDragObject::target() != viewport() )
	    removeSelectedText();
    }
#endif
}

/*!
    If \a select is TRUE (the default), all the text is selected as
    selection 0.
    If \a select is FALSE any selected text is unselected, i.e., the
    default selection (selection 0) is cleared.

  \sa selectedText
*/

void QTextEdit::selectAll( bool select )
{
    if ( !select )
	doc->removeSelection( QTextDocument::Standard );
    else
	doc->selectAll( QTextDocument::Standard );
    repaintChanged();
    emit copyAvailable( doc->hasSelection( QTextDocument::Standard ) );
    emit selectionChanged();
#ifndef QT_NO_CURSOR
    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif
}

void QTextEdit::UndoRedoInfo::clear()
{
    if ( valid() ) {
	if ( type == Insert || type == Return )
	    doc->addCommand( new QTextInsertCommand( doc, id, index, d->text.rawData(), styleInformation ) );
	else if ( type == Format )
	    doc->addCommand( new QTextFormatCommand( doc, id, index, eid, eindex, d->text.rawData(), format, flags ) );
	else if ( type == Style )
	    doc->addCommand( new QTextStyleCommand( doc, id, eid, styleInformation ) );
	else if ( type != Invalid ) {
	    doc->addCommand( new QTextDeleteCommand( doc, id, index, d->text.rawData(), styleInformation ) );
	}
    }
    type = Invalid;
    d->text = QString::null;
    id = -1;
    index = -1;
    styleInformation = QByteArray();
}


/*!
    If there is some selected text (in selection 0) it is deleted. If
    there is no selected text (in selection 0) the character to the
    right of the text cursor is deleted.

    \sa removeSelectedText() cut()

*/

void QTextEdit::del()
{
    if ( doc->hasSelection( QTextDocument::Standard ) ) {
	removeSelectedText();
	return;
    }

    doKeyboardAction( ActionDelete );
}


QTextEdit::UndoRedoInfo::UndoRedoInfo( QTextDocument *dc )
    : type( Invalid ), doc( dc )
{
    d = new QUndoRedoInfoPrivate;
    d->text = QString::null;
    id = -1;
    index = -1;
}

QTextEdit::UndoRedoInfo::~UndoRedoInfo()
{
    delete d;
}

bool QTextEdit::UndoRedoInfo::valid() const
{
    return id >= 0 &&  type != Invalid;
}

/*!
    \internal

  Resets the current format to the default format.
*/

void QTextEdit::resetFormat()
{
    setAlignment( Qt3::AlignAuto );
    setParagType( QStyleSheetItem::DisplayBlock, QStyleSheetItem::ListDisc );
    setFormat( doc->formatCollection()->defaultFormat(), QTextFormat::Format );
}

/*! Returns the QStyleSheet which is currently used in this text edit.

    \sa setStyleSheet()
 */

QStyleSheet* QTextEdit::styleSheet() const
{
    return doc->styleSheet();
}

/*! Sets the stylesheet to use with this text edit to \a styleSheet. Changes
  will only take effect for new text added with setText() or append().

    \sa styleSheet()
 */

void QTextEdit::setStyleSheet( QStyleSheet* styleSheet )
{
    doc->setStyleSheet( styleSheet );
}

/*!
  \property QTextEdit::paper
  \brief the background (paper) brush.

  The brush that is currently used to draw the background of the
  text edit. The initial setting is an empty brush.
 */

void QTextEdit::setPaper( const QBrush& pap )
{
    doc->setPaper( new QBrush( pap ) );
    viewport()->setBackgroundColor( pap.color() );
    updateContents( contentsX(), contentsY(), visibleWidth(), visibleHeight() );
}

QBrush QTextEdit::paper() const
{
    if ( doc->paper() )
	return *doc->paper();
    return QBrush();
}

/*!
  \property QTextEdit::linkUnderline
  \brief whether hypertext links will be underlined

  If TRUE (the default) hypertext links will be displayed underlined.
  If FALSE links will not be displayed underlined.
*/

void QTextEdit::setLinkUnderline( bool b )
{
    doc->setUnderlineLinks( b );
}

bool QTextEdit::linkUnderline() const
{
    return doc->underlineLinks();
}

/*! Sets the text edit's mimesource factory to \a factory. See
    QMimeSourceFactory for further details.

    \sa mimeSourceFactory()
 */

void QTextEdit::setMimeSourceFactory( QMimeSourceFactory* factory )
{
    doc->setMimeSourceFactory( factory );
}

/*! Returns the QMimeSourceFactory which is currently used by this
  text edit.

  \sa setMimeSourceFactory()
*/

QMimeSourceFactory* QTextEdit::mimeSourceFactory() const
{
    return doc->mimeSourceFactory();
}

/*!
    Returns how many pixels high the text edit needs to be to display
    all the text if the text edit is \a w pixels wide.
*/

int QTextEdit::heightForWidth( int w ) const
{
    int oldw = doc->width();
    doc->doLayout( 0, w );
    int h = doc->height();
    doc->setWidth( oldw );
    doc->invalidate();
    ( (QTextEdit*)this )->formatMore();
    return h;
}

/*! Appends the text \a text to the end of the text edit.
    Note that the undo/redo history is cleared by this function.
 */

void QTextEdit::append( const QString &text )
{
    // flush and clear the undo/redo stack if necessary
    undoRedoInfo.clear();
    doc->commands()->clear();

    doc->removeSelection( QTextDocument::Standard );
    TextFormat f = doc->textFormat();
    if ( f == AutoText ) {
	if ( QStyleSheet::mightBeRichText( text ) )
	    f = RichText;
	else
	    f = PlainText;
    }

    drawCursor( FALSE );
    QTextCursor oldc( *cursor );
    ensureFormatted( doc->lastParagraph() );
    bool atBottom = contentsY() >= contentsHeight() - visibleHeight();
    cursor->gotoEnd();
    if ( cursor->index() > 0 )
	cursor->splitAndInsertEmptyParagraph();
    QTextCursor oldCursor2 = *cursor;

    if ( f == Qt::PlainText ) {
	cursor->insert( text, TRUE );
	if ( doc->useFormatCollection() &&
	     currentFormat != cursor->paragraph()->at( cursor->index() )->format() ) {
	    doc->setSelectionStart( QTextDocument::Temp, oldCursor2 );
	    doc->setSelectionEnd( QTextDocument::Temp, *cursor );
	    doc->setFormat( QTextDocument::Temp, currentFormat, QTextFormat::Format );
	    doc->removeSelection( QTextDocument::Temp );
	}
    } else {
	if ( cursor->paragraph()->prev() )
	    cursor->paragraph()->prev()->invalidate(0); // vertical margins might have to change
	doc->setRichTextInternal( text );
    }
    formatMore();
    repaintChanged();
    if ( atBottom )
        scrollToBottom();
    *cursor = oldc;
    if ( !isReadOnly() )
	cursorVisible = TRUE;
    setModified();
    emit textChanged();
}

/*! \property QTextEdit::hasSelectedText
  \brief whether some text is selected in selection 0
 */

bool QTextEdit::hasSelectedText() const
{
    return doc->hasSelection( QTextDocument::Standard );
}

/*!\property QTextEdit::selectedText
   \brief The selected text (from selection 0) or an empty string if
   there is no currently selected text (in selection 0).

   The text is always returned as \c PlainText regardless of the text
   format. In a future version of Qt an HTML subset \e may be returned
   depending on the text format.

   \sa hasSelectedText
 */

QString QTextEdit::selectedText() const
{
    return doc->selectedText( QTextDocument::Standard );
}

bool QTextEdit::handleReadOnlyKeyEvent( QKeyEvent *e )
{
    switch( e->key() ) {
    case Key_Down:
	setContentsPos( contentsX(), contentsY() + 10 );
	break;
    case Key_Up:
	setContentsPos( contentsX(), contentsY() - 10 );
	break;
    case Key_Left:
	setContentsPos( contentsX() - 10, contentsY() );
	break;
    case Key_Right:
	setContentsPos( contentsX() + 10, contentsY() );
	break;
    case Key_PageUp:
	setContentsPos( contentsX(), contentsY() - visibleHeight() );
	break;
    case Key_PageDown:
	setContentsPos( contentsX(), contentsY() + visibleHeight() );
	break;
    case Key_Home:
	setContentsPos( contentsX(), 0 );
	break;
    case Key_End:
	setContentsPos( contentsX(), contentsHeight() - visibleHeight() );
	break;
    case Key_F16: // Copy key on Sun keyboards
	copy();
	break;
#ifndef QT_NO_NETWORKPROTOCOL
    case Key_Return:
    case Key_Enter:
    case Key_Space: {
	if ( !doc->focusIndicator.href.isEmpty() ) {
	    QUrl u( doc->context(), doc->focusIndicator.href, TRUE );
	    emitLinkClicked( u.toString( FALSE, FALSE ) );
#ifndef QT_NO_CURSOR
	    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif
	}
    } break;
#endif
    default:
	if ( e->state() & ControlButton ) {
	    switch ( e->key() ) {
	    case Key_C: case Key_F16: // Copy key on Sun keyboards
		copy();
		break;
	    }
	}
	return FALSE;
    }
    return TRUE;
}

/*!  Returns the context of the edit.
    The context is a path which the text edit's QMimeSourceFactory
    uses to resolve the locations of files and images.

  \sa text
*/

QString QTextEdit::context() const
{
    return doc->context();
}

/*!
  \property QTextEdit::documentTitle
  \brief the title of the document parsed from the text.

  For \c PlainText the title will be an empty string. For \c RichText
  the title will be the text between the \c{<title>} tags, if present,
  otherwise an empty string.
*/

QString QTextEdit::documentTitle() const
{
    return doc->attributes()[ "title" ];
}

void QTextEdit::makeParagVisible( QTextParagraph *p )
{
    setContentsPos( contentsX(), QMIN( p->rect().y(), contentsHeight() - visibleHeight() ) );
}

/*! Scrolls the text edit to make the text at the anchor called \a name
 visible, if it can be found in the document. If the anchor isn't found
 no scrolling will occur. An anchor is defined using the HTML anchor
 tag, e.g. \c{<a name="target">}.
*/

void QTextEdit::scrollToAnchor( const QString& name )
{
    if ( !isVisible() ) {
	d->scrollToAnchor = name;
	return;
    }
    if ( name.isEmpty() )
	return;
    sync();
    QTextCursor cursor( doc );
    QTextParagraph* last = doc->lastParagraph();
    for (;;) {
	QTextStringChar* c = cursor.paragraph()->at( cursor.index() );
	if( c->isAnchor() ) {
	    QString a = c->anchorName();
	    if ( a == name ||
		 (a.contains( '#' ) && QStringList::split( '#', a ).contains( name ) ) ) {
		setContentsPos( contentsX(), QMIN( cursor.paragraph()->rect().top() + cursor.totalOffsetY(), contentsHeight() - visibleHeight() ) );
		break;
	    }
	}
	if ( cursor.paragraph() == last && cursor.atParagEnd()  )
	    break;
	cursor.gotoNextLetter();
    }
}

/*! If there is an anchor at position \a pos (in contents
  coordinates), its name is returned, otherwise an empty string is
  returned.
*/

QString QTextEdit::anchorAt( const QPoint& pos )
{
    QTextCursor c( doc );
    placeCursor( pos, &c );
    return c.paragraph()->at( c.index() )->anchorHref();
}

void QTextEdit::documentWidthChanged( int w )
{
    resizeContents( QMAX( visibleWidth(), w), contentsHeight() );
}

/*! \internal

  This function does nothing
*/

void QTextEdit::updateStyles()
{
}

void QTextEdit::setDocument( QTextDocument *dc )
{
    if ( dc == doc )
	return;
    doc = dc;
    delete cursor;
    cursor = new QTextCursor( doc );
    clearUndoRedo();
    undoRedoInfo.doc = doc;
    lastFormatted = 0;
}

#ifndef QT_NO_CLIPBOARD

/*!
    Pastes the text with format \a subtype from the clipboard into the
    text edit at the current cursor position. The \a subtype can be
    "plain" or "html".

    If there is no text with format \a subtype in the clipboard nothing
    happens.

    \sa paste() cut() QTextEdit::copy()
*/
void QTextEdit::pasteSubType( const QCString& subtype )
{
    QCString st = subtype;
    QString t = QApplication::clipboard()->text(st);
    if ( doc->hasSelection( QTextDocument::Standard ) )
	removeSelectedText();
    if ( !t.isEmpty() ) {
	if ( t.startsWith( "<selstart/>" ) ) {
	    t.remove( 0, 11 );
	    QTextCursor oldC = *cursor;
	    lastFormatted = cursor->paragraph();
	    if ( lastFormatted->prev() )
		lastFormatted = lastFormatted->prev();
	    doc->setRichTextInternal( t, cursor );
	
	    if ( undoEnabled && !isReadOnly() ) {
		doc->setSelectionStart( QTextDocument::Temp, oldC );
		doc->setSelectionEnd( QTextDocument::Temp, *cursor );

		checkUndoRedoInfo( UndoRedoInfo::Insert );
		if ( !undoRedoInfo.valid() ) {
		    undoRedoInfo.id = oldC.paragraph()->paragId();
		    undoRedoInfo.index = oldC.index();
		    undoRedoInfo.d->text = QString::null;
		}
		int oldLen = undoRedoInfo.d->text.length();
		if ( !doc->preProcessor() ) {
		    QString txt = doc->selectedText( QTextDocument::Temp );
		    undoRedoInfo.d->text += txt;
		    for ( int i = 0; i < (int)txt.length(); ++i ) {
			if ( txt[ i ] != '\n' && oldC.paragraph()->at( oldC.index() )->format() ) {
			    oldC.paragraph()->at( oldC.index() )->format()->addRef();
			    undoRedoInfo.d->text.
				setFormat( oldLen + i, oldC.paragraph()->at( oldC.index() )->format(), TRUE );
			}
			oldC.gotoNextLetter();
		    }
		}
		undoRedoInfo.clear();
		removeSelection( QTextDocument::Temp );
	    }
	
	    formatMore();
	    setModified();
	    emit textChanged();
	    repaintChanged();
	    ensureCursorVisible();
	    return;
	}

#if defined(Q_OS_WIN32)
	// Need to convert CRLF to LF
	int index = t.find( QString::fromLatin1("\r\n"), 0 );
	while ( index != -1 ) {
	    t.replace( index, 2, QChar('\n') );
	    index = t.find( "\r\n", index );
	}
#elif defined(Q_OS_MAC)
	//need to convert CR to LF
	for( unsigned int index = 0; index < t.length(); index++ )
	    if(t[index] == '\r')
		t[index] = '\n';
#endif
	for ( int i=0; (uint) i<t.length(); i++ ) {
	    if ( t[ i ] < ' ' && t[ i ] != '\n' && t[ i ] != '\t' )
		t[ i ] = ' ';
	}
	if ( !t.isEmpty() )
	    insert( t, FALSE, TRUE, TRUE );
    }
}

#ifndef QT_NO_MIMECLIPBOARD
/*!
  Prompts the user to choose a type from a list of text types available,
  then copies text from the clipboard (if there is any) into the text
  edit at the current text cursor position. Any selected text (in
  selection 0) is first deleted.
*/
void QTextEdit::pasteSpecial( const QPoint& pt )
{
    QCString st = pickSpecial( QApplication::clipboard()->data(), TRUE, pt );
    if ( !st.isEmpty() )
	pasteSubType( st );
}
#endif
#ifndef QT_NO_MIME
QCString QTextEdit::pickSpecial( QMimeSource* ms, bool always_ask, const QPoint& pt )
{
    if ( ms )  {
#ifndef QT_NO_POPUPMENU
	QPopupMenu popup( this, "qt_pickspecial_menu" );
	QString fmt;
	int n = 0;
	QDict<void> done;
	for (int i = 0; !( fmt = ms->format( i ) ).isNull(); i++) {
	    int semi = fmt.find( ";" );
	    if ( semi >= 0 )
		fmt = fmt.left( semi );
	    if ( fmt.left( 5 ) == "text/" ) {
		fmt = fmt.mid( 5 );
		if ( !done.find( fmt ) ) {
		    done.insert( fmt,(void*)1 );
		    popup.insertItem( fmt, i );
		    n++;
		}
	    }
	}
	if ( n ) {
	    int i = n ==1 && !always_ask ? popup.idAt( 0 ) : popup.exec( pt );
	    if ( i >= 0 )
		return popup.text(i).latin1();
	}
#else
	QString fmt;
	for (int i = 0; !( fmt = ms->format( i ) ).isNull(); i++) {
	    int semi = fmt.find( ";" );
	    if ( semi >= 0 )
		fmt = fmt.left( semi );
	    if ( fmt.left( 5 ) == "text/" ) {
		fmt = fmt.mid( 5 );
		return fmt.latin1();
	    }
	}
#endif
    }
    return QCString();
}
#endif // QT_NO_MIME
#endif // QT_NO_CLIPBOARD

/*! \enum QTextEdit::WordWrap

  This enum defines the QTextEdit's word wrap modes.  The following
  values are valid:

  \value NoWrap	 Do not wrap the text.

  \value WidgetWidth Wrap the text at the current width of the
  widget (this is the default). Wrapping is at whitespace by default;
  this can be changed with setWrapPolicy().

  \value FixedPixelWidth Wrap the text at a fixed number of pixels from
  the widget's left side. The number of pixels is set with
  wrapColumnOrWidth().

  \value FixedColumnWidth Wrap the text at a fixed number of character
  columns from the widget's left side. The number of characters is set
  with wrapColumnOrWidth().
  This is useful if you need formatted text that can also be
  displayed gracefully on devices with monospaced fonts, for example a
  standard VT100 terminal, where you might set wrapColumnOrWidth() to
  80.

 \sa setWordWrap() wordWrap()
*/

/*!
  \property QTextEdit::wordWrap
  \brief the word wrap mode

  The default mode is \c WidgetWidth which causes words to be wrapped
  at the right edge of the text edit. Wrapping occurs at whitespace,
  keeping whole words intact. If you want wrapping to occur within
  words use setWrapPolicy(). If you set a wrap mode of \c
  FixedPixelWidth or \c FixedColumnWidth you should also call
  setWrapColumnOrWidth() with the width you want.

  \sa WordWrap, wrapColumnOrWidth, wrapPolicy,
*/

void QTextEdit::setWordWrap( WordWrap mode )
{
    if ( wrapMode == mode )
	return;
    wrapMode = mode;
    switch ( mode ) {
    case NoWrap:
	document()->formatter()->setWrapEnabled( FALSE );
	document()->formatter()->setWrapAtColumn( -1 );
	doc->setWidth( visibleWidth() );
	doc->setMinimumWidth( -1 );
	doc->invalidate();
	updateContents( contentsX(), contentsY(), visibleWidth(), visibleHeight() );
	lastFormatted = doc->firstParagraph();
	interval = 0;
	formatMore();
	break;
    case WidgetWidth:
	document()->formatter()->setWrapEnabled( TRUE );
	document()->formatter()->setWrapAtColumn( -1 );
	doResize();
	break;
    case FixedPixelWidth:
	document()->formatter()->setWrapEnabled( TRUE );
	document()->formatter()->setWrapAtColumn( -1 );
	if ( wrapWidth < 0 )
	    wrapWidth = 200;
	setWrapColumnOrWidth( wrapWidth );
	break;
    case FixedColumnWidth:
	if ( wrapWidth < 0 )
	    wrapWidth = 80;
	document()->formatter()->setWrapEnabled( TRUE );
	document()->formatter()->setWrapAtColumn( wrapWidth );
	setWrapColumnOrWidth( wrapWidth );
	break;
    }
}

QTextEdit::WordWrap QTextEdit::wordWrap() const
{
    return wrapMode;
}

/*!
  \property QTextEdit::wrapColumnOrWidth
  \brief the position (in pixels or columns depending on the wrap mode) where text will be wrapped

  If the wrap mode is \c FixedPixelWidth, the value is the number
  of pixels from the left edge of the text edit at which text should
  be wrapped. If the wrap mode is \c FixedColumnWidth, the value is
  the column number (in character columns) from the left edge of the
  text edit at which text should be wrapped.

  \sa wordWrap
 */
void QTextEdit::setWrapColumnOrWidth( int value )
{
    wrapWidth = value;
    if ( wrapMode == FixedColumnWidth ) {
	document()->formatter()->setWrapAtColumn( wrapWidth );
	resizeContents( 0, 0 );
	doc->setWidth( visibleWidth() );
	doc->setMinimumWidth( -1 );
    } else if (wrapMode == FixedPixelWidth ) {
	document()->formatter()->setWrapAtColumn( -1 );
	resizeContents( wrapWidth, 0 );
	doc->setWidth( wrapWidth );
	doc->setMinimumWidth( wrapWidth );
    } else {
	return;
    }
    doc->invalidate();
    updateContents( contentsX(), contentsY(), visibleWidth(), visibleHeight() );
    lastFormatted = doc->firstParagraph();
    interval = 0;
    formatMore();
}

int QTextEdit::wrapColumnOrWidth() const
{
    if ( wrapMode == WidgetWidth )
	return visibleWidth();
    return wrapWidth;
}


/*! \enum QTextEdit::WrapPolicy

  This enum defines where text can be wrapped in word wrap mode.

  The following values are valid:
  \value AtWhiteSpace  Break lines at whitespace, e.g. spaces or
  newlines.
  \value Anywhere  Break anywhere, including within words.
  \value AtWordBoundary Don't use this deprecated value (it is a
  synonym for AtWhiteSpace which you should use instead).

   \sa setWrapPolicy()
*/

/*!
  \property QTextEdit::wrapPolicy
  \brief the word wrap policy, at whitespace or anywhere

  Defines where text can be wrapped when word wrap mode is not
  \c NoWrap. The choices are \c AtWhiteSpace (the default) and \c
  Anywhere.

  \sa wordWrap
 */

void QTextEdit::setWrapPolicy( WrapPolicy policy )
{
    if ( wPolicy == policy )
	return;
    wPolicy = policy;
    QTextFormatter *formatter;
    if ( policy == AtWhiteSpace )
	formatter = new QTextFormatterBreakWords;
    else
	formatter = new QTextFormatterBreakInWords;
    formatter->setWrapAtColumn( document()->formatter()->wrapAtColumn() );
    formatter->setWrapEnabled( document()->formatter()->isWrapEnabled( 0 ) );
    document()->setFormatter( formatter );
    doc->invalidate();
    updateContents( contentsX(), contentsY(), visibleWidth(), visibleHeight() );
    lastFormatted = doc->firstParagraph();
    interval = 0;
    formatMore();
}

QTextEdit::WrapPolicy QTextEdit::wrapPolicy() const
{
    return wPolicy;
}

/*!
  Deletes all the text in the text edit.

  \sa cut() removeSelectedText() setText()

*/

void QTextEdit::clear()
{
    // make clear undoable
    doc->selectAll( QTextDocument::Temp );
    removeSelectedText( QTextDocument::Temp );

    setContentsPos( 0, 0 );
    if ( cursor->isValid() )
	cursor->restoreState();
    doc->clear( TRUE );
    delete cursor;
    cursor = new QTextCursor( doc );
    lastFormatted = 0;
    updateContents( contentsX(), contentsY(), visibleWidth(), visibleHeight() );

    emit cursorPositionChanged( cursor );
    emit cursorPositionChanged( cursor->paragraph()->paragId(), cursor->index() );
}

int QTextEdit::undoDepth() const
{
    return document()->undoDepth();
}

/*!
  \property QTextEdit::length
  \brief the number of characters in the text

*/

int QTextEdit::length() const
{
    return document()->length();
}

/*!
  \property QTextEdit::tabStopWidth
  \brief the tab stop width in pixels

*/

int QTextEdit::tabStopWidth() const
{
    return document()->tabStopWidth();
}

void QTextEdit::setUndoDepth( int d )
{
    document()->setUndoDepth( d );
}

void QTextEdit::setTabStopWidth( int ts )
{
    document()->setTabStops( ts );
    doc->invalidate();
    lastFormatted = doc->firstParagraph();
    interval = 0;
    formatMore();
    updateContents( contentsX(), contentsY(), visibleWidth(), visibleHeight() );
}

/*! \reimp */

QSize QTextEdit::sizeHint() const
{
    // ### calculate a reasonable one
    return QSize( 100, 100 );
}

void QTextEdit::clearUndoRedo()
{
    undoRedoInfo.clear();
    emit undoAvailable( doc->commands()->isUndoAvailable() );
    emit redoAvailable( doc->commands()->isRedoAvailable() );
}

/*!  \internal

  \warning In Qt 3.1 we will provide a cleaer API for the
  functionality which is provided by this function and in Qt 4.0 this
  function will go away.

  This function gets the format of the character at position \a
  index in paragraph \a para. Sets \a font to the character's font, \a
  color to the character's color and \a verticalAlignment to the
  character's vertical alignment.

  Returns FALSE if \a para or \a index is out of range otherwise
  returns TRUE.
*/

bool QTextEdit::getFormat( int para, int index, QFont *font, QColor *color, VerticalAlignment *verticalAlignment )
{
    if ( !font || !color )
	return FALSE;
    QTextParagraph *p = doc->paragAt( para );
    if ( !p )
	return FALSE;
    if ( index < 0 || index >= p->length() )
	return FALSE;
    *font = p->at( index )->format()->font();
    *color = p->at( index )->format()->color();
    *verticalAlignment = (VerticalAlignment)p->at( index )->format()->vAlign();
    return TRUE;
}

/*!  \internal

  \warning In Qt 3.1 we will provide a cleaer API for the
  functionality which is provided by this function and in Qt 4.0 this
  function will go away.

  This function gets the format of the paragraph \a para. Sets \a
  font to the paragraphs's font, \a color to the paragraph's color, \a
  verticalAlignment to the paragraph's vertical alignment, \a
  alignment to the paragraph's alignment, \a displayMode to the
  paragraph's display mode, \a listStyle to the paragraph's list style
  (if the display mode is QStyleSheetItem::DisplayListItem) and \a
  listDepth to the depth of the list (if the display mode is
  QStyleSheetItem::DisplayListItem).

  Returns FALSE if \a para is out of range otherwise returns TRUE.
*/

bool QTextEdit::getParagraphFormat( int para, QFont *font, QColor *color,
				    VerticalAlignment *verticalAlignment, int *alignment,
				    QStyleSheetItem::DisplayMode *displayMode,
				    QStyleSheetItem::ListStyle *listStyle,
				    int *listDepth )
{
    if ( !font || !color || !alignment || !displayMode || !listStyle )
	return FALSE;
    QTextParagraph *p = doc->paragAt( para );
    if ( !p )
	return FALSE;
    *font = p->at(0)->format()->font();
    *color = p->at(0)->format()->color();
    *verticalAlignment = (VerticalAlignment)p->at(0)->format()->vAlign();
    *alignment = p->alignment();
    *displayMode = p->isListItem() ? QStyleSheetItem::DisplayListItem : QStyleSheetItem::DisplayBlock;
    *listStyle = p->listStyle();
    *listDepth = p->listDepth();
    return TRUE;
}



/*!

    This function is called to create a right mouse button popup menu
     at the document position \a pos.  If you want to create a custom
     popup menu, reimplement this function and return the created
     popup menu. Ownership of the popup menu is transferred to the
     caller.
*/

QPopupMenu *QTextEdit::createPopupMenu( const QPoint& pos )
{
#ifndef QT_NO_POPUPMENU
    QPopupMenu *popup = new QPopupMenu( this, "qt_edit_menu" );
    if ( !isReadOnly() ) {
	d->id[ IdUndo ] = popup->insertItem( tr( "&Undo" ) + ACCEL_KEY( Z ) );
	d->id[ IdRedo ] = popup->insertItem( tr( "&Redo" ) + ACCEL_KEY( Y ) );
	popup->insertSeparator();
    }
#ifndef QT_NO_CLIPBOARD
    if ( !isReadOnly() )
	d->id[ IdCut ] = popup->insertItem( tr( "Cu&t" ) + ACCEL_KEY( X ) );
    d->id[ IdCopy ] = popup->insertItem( tr( "&Copy" ) + ACCEL_KEY( C ) );
    if ( !isReadOnly() )
	d->id[ IdPaste ] = popup->insertItem( tr( "&Paste" ) + ACCEL_KEY( V ) );
#endif
    if ( !isReadOnly() ) {
	d->id[ IdClear ] = popup->insertItem( tr( "Clear" ) );
	popup->insertSeparator();
    }
#if defined(Q_WS_X11)
    d->id[ IdSelectAll ] = popup->insertItem( tr( "Select All" ) );
#else
    d->id[ IdSelectAll ] = popup->insertItem( tr( "Select All" ) + ACCEL_KEY( A ) );
#endif
    popup->setItemEnabled( d->id[ IdUndo ], !isReadOnly() && doc->commands()->isUndoAvailable() );
    popup->setItemEnabled( d->id[ IdRedo ], !isReadOnly() && doc->commands()->isRedoAvailable() );
#ifndef QT_NO_CLIPBOARD
    popup->setItemEnabled( d->id[ IdCut ], !isReadOnly() && doc->hasSelection( QTextDocument::Standard, TRUE ) );
    popup->setItemEnabled( d->id[ IdCopy ], doc->hasSelection( QTextDocument::Standard, TRUE ) );
    popup->setItemEnabled( d->id[ IdPaste ], !isReadOnly() && !QApplication::clipboard()->text().isEmpty() );
#endif
    popup->setItemEnabled( d->id[ IdClear ], !isReadOnly() && !text().isEmpty() );
    popup->setItemEnabled( d->id[ IdSelectAll ], (bool)text().length() );
    return popup;
#else
    return 0;
#endif
}

/*! \overload
    \obsolete
    This function is called to create a right mouse button popup menu.
    If you want to create a custom popup menu, reimplement this function
    and return the created popup menu. Ownership of the popup menu is
    transferred to the caller.

    This function is only called if createPopupMenu( const QPoint & )
    returns 0.
*/

QPopupMenu *QTextEdit::createPopupMenu()
{
    return 0;
}

/*! \reimp */

void QTextEdit::setFont( const QFont &f )
{
    QFont old( QScrollView::font() );
    QScrollView::setFont( f );
    doc->setMinimumWidth( -1 );
    doc->setDefaultFormat( f, doc->formatCollection()->defaultFormat()->color() );
    lastFormatted = doc->firstParagraph();
    formatMore();
    repaintChanged();
}

/*! \fn QTextEdit::zoomIn()

    \overload

    Zooms in on the text by by making the base font size one
    point larger and recalculating all font sizes. This does not change
    the size of any images.

    \sa zoomOut()

*/

/*! \fn QTextEdit::zoomOut()

    \overload

    Zooms out on the text by by making the base font size one
    point smaller and recalculating all font sizes. This does not change
    the size of any images.

    \sa zoomIn()
*/


/*!
    Zooms in on the text by by making the base font size \a range
    points larger and recalculating all font sizes. This does not change
    the size of any images.

    \sa zoomOut()
*/

void QTextEdit::zoomIn( int range )
{
    QFont f( QScrollView::font() );
    f.setPointSize( f.pointSize() + range );
    setFont( f );
}

/*! Zooms out on the text by making the base font size \a range
    points smaller and recalculating all font sizes. This does not
    change the size of any images.

    \sa zoomIn()
*/

void QTextEdit::zoomOut( int range )
{
    QFont f( QScrollView::font() );
    f.setPointSize( QMAX( 1, f.pointSize() - range ) );
    setFont( f );
}

/*! Zooms the text by making the base font size \a size points and
  recalculating all font sizes. This does not change the size of any
  images.
*/

void QTextEdit::zoomTo( int size )
{
    QFont f( QScrollView::font() );
    f.setPointSize( size );
    setFont( f );
}

/*!
    \internal

   QTextEdit is optimized for large amounts text. One of its
   optimizations is to format only the visible text, formatting the rest
   on demand, e.g. as the user scrolls, so you don't usually need to
   call this function.

    In some situations you may want to force the whole text
    to be formatted. For example, if after calling setText(), you wanted
    to know the height of the document (using contentsHeight()), you
    would call this function first.
*/

void QTextEdit::sync()
{
    while ( lastFormatted ) {
	lastFormatted->format();
	lastFormatted = lastFormatted->next();
    }
    resizeContents( contentsWidth(), doc->height() );
}

/*! \reimp */

void QTextEdit::setEnabled( bool b )
{
    QScrollView::setEnabled( b );
    if ( !b ) {
	blinkTimer->stop();
	drawCursor( FALSE );
    }
    if ( textFormat() == PlainText ) {
	QTextFormat *f = doc->formatCollection()->defaultFormat();
	f->setColor( colorGroup().text() );
	updateContents( contentsX(), contentsY(), visibleWidth(), visibleHeight() );
    }
    if ( b ) {
	blinkTimer->start( QApplication::cursorFlashTime() / 2 );
	drawCursor( TRUE );
    }
}

/*!
  Sets the background color of selection number \a selNum to \a back and
  specifies whether the text of this selection should be inverted with \a
  invertText.

  This only works for \a selNum > 0. The default selection (\a selNum ==
  0) gets its attributes from the colorGroup() of this widget.
*/

void QTextEdit::setSelectionAttributes( int selNum, const QColor &back, bool invertText )
{
    if ( selNum < 1 )
	return;
    if ( selNum > doc->numSelections() )
	doc->addSelection( selNum );
    doc->setSelectionColor( selNum, back );
    doc->setInvertSelectionText( selNum, invertText );
}

/*! \reimp */
void QTextEdit::windowActivationChange( bool )
{
    if ( !isVisible() )
	return;

    if ( palette().active() != palette().inactive() )
	updateContents( contentsX(), contentsY(), visibleWidth(), visibleHeight() );
}

void QTextEdit::setReadOnly( bool b )
{
    if ( readonly == b )
	return;
    readonly = b;
#ifndef QT_NO_CURSOR
    if ( readonly )
	viewport()->setCursor( arrowCursor );
    else
	viewport()->setCursor( ibeamCursor );
#endif
}

/*! Scrolls to the bottom of the document and does formatting if
  required */

void QTextEdit::scrollToBottom()
{
    sync();
    setContentsPos( contentsX(), contentsHeight() - visibleHeight() );
}

/*! Returns the rectangle of the paragraph \a para in contents
  coordinates, or an invalid rectangle if \a para is out of range.
*/

QRect QTextEdit::paragraphRect( int para ) const
{
    QTextEdit *that = (QTextEdit *)this;
    that->sync();
    QTextParagraph *p = doc->paragAt( para );
    if ( !p )
	return QRect( -1, -1, -1, -1 );
    return p->rect();
}

/*!
    Returns the paragraph which is at position \a pos (in contents
    coordinates), or -1 if there is no paragraph with index \a pos.
*/

int QTextEdit::paragraphAt( const QPoint &pos ) const
{
    QTextCursor c( doc );
    c.place( pos, doc->firstParagraph() );
    if ( c.paragraph() )
	return c.paragraph()->paragId();
    return -1;
}

/*!
    Returns the index of the character (relative to its paragraph) at
    position \a pos (in contents coordinates). If \a para is not null,
    \e *\a para is set to this paragraph. If there is no character at
    \a pos, -1 is returned.
*/

int QTextEdit::charAt( const QPoint &pos, int *para ) const
{
    QTextCursor c( doc );
    c.place( pos, doc->firstParagraph() );
    if ( c.paragraph() ) {
	if ( para )
	    *para = c.paragraph()->paragId();
	return c.index();
    }
    return -1;
}

/*! Sets the background color of the paragraph \a para to \a bg */

void QTextEdit::setParagraphBackgroundColor( int para, const QColor &bg )
{
    QTextParagraph *p = doc->paragAt( para );
    if ( !p )
	return;
    p->setBackgroundColor( bg );
    repaintChanged();
}

/*! Clears the background color of the paragraph \a para, so that the
  default color is used again.
*/

void QTextEdit::clearParagraphBackground( int para )
{
    QTextParagraph *p = doc->paragAt( para );
    if ( !p )
	return;
    p->clearBackgroundColor();
    repaintChanged();
}

/*! Returns the background color of the paragraph \a para or an
  invalid color if \a para is out of range or the paragraph has no
  background set
*/

QColor QTextEdit::paragraphBackgroundColor( int para ) const
{
    QTextParagraph *p = doc->paragAt( para );
    if ( !p )
	return QColor();
    QColor *c = p->backgroundColor();
    if ( c )
	return *c;
    return QColor();
}

/*! \property QTextEdit::undoRedoEnabled
  \brief whether undo/redo is enabled

  The default is TRUE.
*/

void QTextEdit::setUndoRedoEnabled( bool b )
{
    undoEnabled = b;
}

bool QTextEdit::isUndoRedoEnabled() const
{
    return undoEnabled;
}

/*! Returns whether undo is available */

bool QTextEdit::isUndoAvailable() const
{
    return doc->commands()->isUndoAvailable() || undoRedoInfo.valid();
}

/*! Returns whether redo is available */

bool QTextEdit::isRedoAvailable() const
{
    return doc->commands()->isRedoAvailable();
}

void QTextEdit::ensureFormatted( QTextParagraph *p )
{
    while ( !p->isValid() ) {
	if ( !lastFormatted )
	    return;
	formatMore();
    }
}

/*! \internal */
void QTextEdit::updateCursor( const QPoint & pos )
{
    if ( isReadOnly() && linksEnabled() ) {
	QTextCursor c = *cursor;
	placeCursor( pos, &c, TRUE );

#ifndef QT_NO_NETWORKPROTOCOL
	if ( c.paragraph() && c.paragraph()->at( c.index() ) &&
	     c.paragraph()->at( c.index() )->isAnchor() &&
	     !c.paragraph()->at( c.index() )->anchorHref().isEmpty() ) {
	    if ( c.index() < c.paragraph()->length() - 1 )
		onLink = c.paragraph()->at( c.index() )->anchorHref();
	    else
		onLink = QString::null;

#ifndef QT_NO_CURSOR
	    viewport()->setCursor( onLink.isEmpty() ? arrowCursor : pointingHandCursor );
#endif
	    QUrl u( doc->context(), onLink, TRUE );
	    emitHighlighted( u.toString( FALSE, FALSE ) );
	} else {
#ifndef QT_NO_CURSOR
	    viewport()->setCursor( isReadOnly() ? arrowCursor : ibeamCursor );
#endif
	    onLink = QString::null;
	    emitHighlighted( QString::null );
	}
#endif
    }
}

void QTextEdit::placeCursor( const QPoint &pos, QTextCursor *c )
{
    placeCursor( pos, c, FALSE );
}
