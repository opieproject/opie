/* ----------------------------------------------------------------------- */
/*                                                                         */
/* [widget.h]           Terminal Emulation Widget                       */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*                                                                         */
/* Copyright (c) 1997,1998 by Lars Doelle <lars.doelle@on-line.de>         */
/*                                                                         */
/* This file was part of Konsole - an X terminal for KDE                    */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*                        */
/* Ported Konsole to Qt/Embedded                                              */
/*                        */
/* Copyright (C) 2000 by John Ryland <jryland@trolltech.com>                  */
/*                        */
/* -------------------------------------------------------------------------- */
/*                                                                          */
/* modified to suit opie-console                                            */
/*                                                                          */
/* Copyright (c) 2002 by  opie developers <opie@handhelds.org>              */
/*                                                                          */
/* ------------------------------------------------------------------------ */

// ibot: TODO *

#ifndef WIDGET_H
#define WIDGET_H

#include <qapplication.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qcolor.h>
#include <qkeycode.h>
#include <qscrollbar.h>

#include <qpopupmenu.h>

#include "common.h"

extern unsigned short vt100_graphics[32];

class Session;

// class Konsole;

class Widget : public QFrame
// a widget representing attributed text
{ Q_OBJECT

//  friend class Konsole;

public:

    Widget(QWidget *parent=0, const char *name=0);
    virtual ~Widget();

public:

    QColor getDefaultBackColor();

    const ColorEntry* getColorTable() const;
    const ColorEntry* getdefaultColorTable() const;
    void              setColorTable(const ColorEntry table[]);

    void setScrollbarLocation(int loc);
    enum { SCRNONE=0, SCRLEFT=1, SCRRIGHT=2 };

    void setScroll(int cursor, int lines);
    void doScroll(int lines);

    void emitSelection();

public:

    void setImage(const Character* const newimg, int lines, int columns);

    int  Lines()   { return lines;   }
    int  Columns() { return columns; }

    void calcGeometry();
    void propagateSize();
    QSize calcSize(int cols, int lins) const;

    QSize sizeHint() const;

public:

    void Bell();
    void emitText(QString text);
    void pasteClipboard();

signals:

    void keyPressedSignal(QKeyEvent *e);
    void mouseSignal(int cb, int cx, int cy);
    void changedImageSizeSignal(int lines, int columns);
    void changedHistoryCursor(int value);
    void configureRequest( Widget*, int state, int x, int y );

    void clearSelectionSignal();
    void beginSelectionSignal( const int x, const int y );
    void extendSelectionSignal( const int x, const int y );
    void endSelectionSignal(const BOOL preserve_line_breaks);


protected:

    virtual void styleChange( QStyle& );

    bool eventFilter( QObject *, QEvent * );

    void drawAttrStr(QPainter &paint, QRect rect,
                     QString& str, Character attr, BOOL pm, BOOL clear);
    void paintEvent( QPaintEvent * );

    void resizeEvent(QResizeEvent*);

    void fontChange(const QFont &font);
    void frameChanged();

    void mouseDoubleClickEvent(QMouseEvent* ev);
    void mousePressEvent( QMouseEvent* );
    void mouseReleaseEvent( QMouseEvent* );
    void mouseMoveEvent( QMouseEvent* );

    void focusInEvent( QFocusEvent * );
    void focusOutEvent( QFocusEvent * );
    bool focusNextPrevChild( bool next );

#ifndef QT_NO_DRAGANDDROP
    // Dnd
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);
#endif

    virtual int charClass(char) const;

    void clearImage();

public:
    const QPixmap *backgroundPixmap();
    
    void setSelection(const QString &t);

    virtual void setFont(const QFont &);
    void setVTFont(const QFont &);
    QFont getVTFont();

    void setMouseMarks(bool on);

public slots:

    void onClearSelection();

protected slots:

    void scrollChanged(int value);
    void blinkEvent();

private:

    QChar (*fontMap)(QChar); // possible vt100 font extention

    bool fixed_font; // has fixed pitch
    int  font_h;     // height
    int  font_w;     // width
    int  font_a;     // ascend

    int blX;    // actual offset (left)
    int brX;    // actual offset (right)
    int bY;     // actual offset

    int lines;
    int columns;
    Character *image; // [lines][columns]

    ColorEntry color_table[TABLE_COLORS];

    BOOL resizing;
    bool mouse_marks;

    void makeImage();

    QPoint iPntSel; // initial selection point
    QPoint pntSel; // current selection point
    int    actSel; // selection state
    BOOL    word_selection_mode;
    BOOL    preserve_line_breaks;

    QClipboard*    cb;
    QScrollBar* scrollbar;
    int         scrollLoc;

//#define SCRNONE  0
//#define SCRLEFT  1
//#define SCRRIGHT 2

    BOOL blinking;   // hide text in paintEvent
    BOOL hasBlinker; // has characters to blink
    QTimer* blinkT;  // active when hasBlinker
    QPopupMenu* m_drop;
    QString dropText;
 public:
    // current session in this widget
    // ibot: switch from TESession to Session!
    Session *currentSession;
private slots:
    void drop_menu_activated(int item);
};

#endif // TE_WIDGET_H
