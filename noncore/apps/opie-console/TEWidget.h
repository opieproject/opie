/* ----------------------------------------------------------------------- */
/*                                                                         */
/* [te_widget.h]           Terminal Emulation Widget                       */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*                                                                         */
/* Copyright (c) 1997,1998 by Lars Doelle <lars.doelle@on-line.de>         */
/*                                                                         */
/* This file is part of Konsole - an X terminal for KDE                    */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*                        */
/* Ported Konsole to Qt/Embedded                                              */
/*                        */
/* Copyright (C) 2000 by John Ryland <jryland@trolltech.com>                  */
/*                        */
/* -------------------------------------------------------------------------- */
#ifndef TE_WIDGET_H
#define TE_WIDGET_H

#include <qwidget.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qcolor.h>
#include <qkeycode.h>
#include <qscrollbar.h>

#include <qpopupmenu.h>
#include <qpushbutton.h>

#include "TECommon.h"

extern unsigned short vt100_graphics[32];



static char * menu_xpm[] = {
"12 12 5 1",
" 	c None",
".	c #000000",
"+	c #FFFDAD",
"@	c #FFFF00",
"#	c #E5E100",
"            ",
"            ",
"  ......... ",
"  .+++++++. ",
"  .+@@@@#.  ",
"  .+@@@#.   ",
"  .+@@#.    ",
"  .+@#.     ",
"  .+#.      ",
"  .+.       ",
"  ..        ",
"            "};

class TESession;

// class Konsole;

class TEWidget : public QFrame
// a widget representing attributed text
{
    Q_OBJECT

//  friend class Konsole;

public:

    TEWidget(QWidget *parent=0, const char *name=0);
    virtual ~TEWidget();

public:

    QColor getDefaultBackColor();
    QPushButton *cornerButton();

    const ColorEntry* getColorTable() const;
    const ColorEntry* getdefaultColorTable() const;
    void              setColorTable(const ColorEntry table[]);

    void setScrollbarLocation(int loc);
    enum { SCRNONE=0, SCRLEFT=1, SCRRIGHT=2 };

    void setScroll(int cursor, int lines);
    void doScroll(int lines);

    void emitSelection();

public:

    void setImage(const ca* const newimg, int lines, int columns);

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
    void configureRequest( TEWidget*, int state, int x, int y );

    void clearSelectionSignal();
    void beginSelectionSignal( const int x, const int y );
    void extendSelectionSignal( const int x, const int y );
    void endSelectionSignal(const BOOL preserve_line_breaks);


protected:

    virtual void styleChange( QStyle& );

    bool eventFilter( QObject *, QEvent * );

    void drawAttrStr(QPainter &paint, QRect rect,
                     QString& str, ca attr, BOOL pm, BOOL clear);
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
    ca *image; // [lines][columns]

    ColorEntry color_table[TABLE_COLORS];

    BOOL resizing;
    bool mouse_marks;

    void makeImage();

    QPoint iPntSel; // initial selection point
    QPoint pntSel; // current selection point
    int    actSel; // selection state
    BOOL    word_selection_mode;
    BOOL    preserve_line_breaks;

    QPushButton *m_cornerButton;
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
    TESession *currentSession;
private slots:
    void drop_menu_activated(int item);
};

#endif // TE_WIDGET_H
