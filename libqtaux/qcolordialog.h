/****************************************************************************
** $Id: qcolordialog.h,v 1.2 2004-02-21 22:54:19 drw Exp $
**
** Definition of QColorDialog class
**
** Created : 990222
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the dialogs module of the Qt GUI Toolkit.
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

#ifndef QCOLORDIALOG_H
#define QCOLORDIALOG_H

#ifndef QT_H
#include <qdialog.h>
#endif // QT_H

#include <qlabel.h>
#include <qlineedit.h>
#include <qtableview.h>
#include <qvalidator.h>

struct QWellArrayData;

class QWellArray : public QTableView
{
    Q_OBJECT
//    Q_PROPERTY( int numCols READ numCols )
//    Q_PROPERTY( int numRows READ numRows )
//    Q_PROPERTY( int selectedColumn READ selectedColumn )
//    Q_PROPERTY( int selectedRow READ selectedRow )

public:
    QWellArray( QWidget *parent=0, const char *name=0, bool popup = FALSE );

    ~QWellArray() {}
    QString cellContent( int row, int col ) const;
    // ### Paul !!! virtual void setCellContent( int row, int col, const QString &);

    // ##### Obsolete since not const
    int numCols() { return nCols; }
    int numRows() { return nRows; }

    int numCols() const { return nCols; }
    int numRows() const { return nRows; }

    // ##### Obsolete since not const
    int selectedColumn() { return selCol; }
    int selectedRow() { return selRow; }

    int selectedColumn() const { return selCol; }
    int selectedRow() const { return selRow; }

    virtual void setSelected( int row, int col );

    void setCellSize( int w, int h ) { setCellWidth(w);setCellHeight( h ); }

    QSize sizeHint() const;

    virtual void setDimension( int rows, int cols );
    virtual void setCellBrush( int row, int col, const QBrush & );
    QBrush cellBrush( int row, int col );

signals:
    void selected( int row, int col );

protected:
    virtual void setCurrent( int row, int col );

    virtual void drawContents( QPainter *, int row, int col, const QRect& );
    void drawContents( QPainter * );

    void paintCell( QPainter*, int row, int col );
    void mousePressEvent( QMouseEvent* );
    void mouseReleaseEvent( QMouseEvent* );
    void mouseMoveEvent( QMouseEvent* );
    void keyPressEvent( QKeyEvent* );
    void focusInEvent( QFocusEvent* );
    void focusOutEvent( QFocusEvent* );

private:
    int curRow;
    int curCol;
    int selRow;
    int selCol;
    int nCols;
    int nRows;
    bool smallStyle;
    QWellArrayData *d;

private:    // Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    QWellArray( const QWellArray & );
    QWellArray& operator=( const QWellArray & );
#endif
};

struct QWellArrayData {
    QBrush *brush;
};

class QColNumLineEdit : public QLineEdit
{
public:
    QColNumLineEdit( QWidget *parent, const char* name = 0 )
    : QLineEdit( parent, name ) { setMaxLength( 3 );}
    QSize sizeHint() const {
    return QSize( 30, //#####
             QLineEdit::sizeHint().height() ); }
    void setNum( int i ) {
    QString s;
    s.setNum(i);
    bool block = signalsBlocked();
    blockSignals(TRUE);
    setText( s );
    blockSignals(block);
    }
    int val() const { return text().toInt(); }
};

class QColorShowLabel : public QFrame
{
    Q_OBJECT

public:
    QColorShowLabel( QWidget *parent ) :QFrame( parent ) {
    setFrameStyle( QFrame::Panel|QFrame::Sunken );
    setBackgroundMode( PaletteBackground );
    setAcceptDrops( TRUE );
    mousePressed = FALSE;
    }
    void setColor( QColor c ) { col = c; }

signals:
    void colorDropped( QRgb );

protected:
    void drawContents( QPainter *p );
    void mousePressEvent( QMouseEvent *e );
    void mouseMoveEvent( QMouseEvent *e );
    void mouseReleaseEvent( QMouseEvent *e );
#ifndef QT_NO_DRAGANDDROP
    void dragEnterEvent( QDragEnterEvent *e );
    void dragLeaveEvent( QDragLeaveEvent *e );
    void dropEvent( QDropEvent *e );
#endif

private:
    QColor col;
    bool mousePressed;
    QPoint pressPos;

};

class QColorShower : public QWidget
{
    Q_OBJECT
public:
    QColorShower( QWidget *parent, const char *name = 0 );

    //things that don't emit signals
    void setHsv( int h, int s, int v );

    int currentAlpha() const { return alphaEd->val(); }
    void setCurrentAlpha( int a ) { alphaEd->setNum( a ); }
    void showAlpha( bool b );


    QRgb currentColor() const { return curCol; }

public slots:
    void setRgb( QRgb rgb );

signals:
    void newCol( QRgb rgb );
private slots:
    void rgbEd();
    void hsvEd();
private:
    void showCurrentColor();
    int hue, sat, val;
    QRgb curCol;
    QColNumLineEdit *hEd;
    QColNumLineEdit *sEd;
    QColNumLineEdit *vEd;
    QColNumLineEdit *rEd;
    QColNumLineEdit *gEd;
    QColNumLineEdit *bEd;
    QColNumLineEdit *alphaEd;
    QLabel *alphaLab;
    QColorShowLabel *lab;
    bool rgbOriginal;
};

class QColorPicker : public QFrame
{
    Q_OBJECT
public:
    QColorPicker(QWidget* parent=0, const char* name=0);
    ~QColorPicker();

public slots:
    void setCol( int h, int s );

signals:
    void newCol( int h, int s );

protected:
    QSize sizeHint() const;
    QSizePolicy sizePolicy() const;
    void drawContents(QPainter* p);
    void mouseMoveEvent( QMouseEvent * );
    void mousePressEvent( QMouseEvent * );

private:
    int hue;
    int sat;

    QPoint colPt();
    int huePt( const QPoint &pt );
    int satPt( const QPoint &pt );
    void setCol( const QPoint &pt );

    QPixmap *pix;
};

class QColorLuminancePicker : public QWidget
{
    Q_OBJECT
public:
    QColorLuminancePicker(QWidget* parent=0, const char* name=0);
    ~QColorLuminancePicker();

public slots:
    void setCol( int h, int s, int v );
    void setCol( int h, int s );

signals:
    void newHsv( int h, int s, int v );

protected:
//    QSize sizeHint() const;
//    QSizePolicy sizePolicy() const;
    void paintEvent( QPaintEvent*);
    void mouseMoveEvent( QMouseEvent * );
    void mousePressEvent( QMouseEvent * );

private:
    enum { foff = 3, coff = 4 }; //frame and contents offset
    int val;
    int hue;
    int sat;

    int y2val( int y );
    int val2y( int val );
    void setVal( int v );

    QPixmap *pix;
};

class QColorWell : public QWellArray
{
public:
    QColorWell( QWidget *parent, int r, int c, QRgb *vals )
    :QWellArray( parent, "" ), values( vals ), mousePressed( FALSE ), oldCurrent( -1, -1 )
    { setDimension(r,c); setWFlags( WResizeNoErase ); }
    QSizePolicy sizePolicy() const;

protected:
    void drawContents( QPainter *, int row, int col, const QRect& );
    void drawContents( QPainter *p ) { QWellArray::drawContents(p); }
    void mousePressEvent( QMouseEvent *e );
    void mouseMoveEvent( QMouseEvent *e );
    void mouseReleaseEvent( QMouseEvent *e );
#ifndef QT_NO_DRAGANDDROP
    void dragEnterEvent( QDragEnterEvent *e );
    void dragLeaveEvent( QDragLeaveEvent *e );
    void dragMoveEvent( QDragMoveEvent *e );
    void dropEvent( QDropEvent *e );
#endif

private:
    QRgb *values;
    bool mousePressed;
    QPoint pressPos;
    QPoint oldCurrent;

};

class QColIntValidator: public QIntValidator
{
public:
    QColIntValidator( int bottom, int top,
           QWidget * parent, const char *name = 0 )
    :QIntValidator( bottom, top, parent, name ) {}

    QValidator::State validate( QString &, int & ) const;
};

class QColorDialogPrivate;

class Q_EXPORT QColorDialog : public QDialog
{
    Q_OBJECT

public:
    static QColor getColor( QColor, QWidget *parent=0, const char* name=0 ); // ### 3.0: make const QColor&
    static QRgb getRgba( QRgb, bool* ok = 0,
             QWidget *parent=0, const char* name=0 );


    static int customCount();
    static QRgb customColor( int );
    static void setCustomColor( int, QRgb );

private:
    ~QColorDialog();

    QColorDialog( QWidget* parent=0, const char* name=0, bool modal=FALSE );
    void setColor( QColor ); // ### 3.0: make const QColor&
    QColor color() const;

private:
    void setSelectedAlpha( int );
    int selectedAlpha() const;

    void showCustom( bool=TRUE );
private:
    QColorDialogPrivate *d;
    friend class QColorDialogPrivate;

private:    // Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    QColorDialog( const QColorDialog & );
    QColorDialog& operator=( const QColorDialog & );
#endif
};

class QColorDialogPrivate : public QObject
{
Q_OBJECT
public:
    QColorDialogPrivate( QColorDialog *p );
    QRgb currentColor() const { return cs->currentColor(); }
    void setCurrentColor( QRgb rgb );

    int currentAlpha() const { return cs->currentAlpha(); }
    void setCurrentAlpha( int a ) { cs->setCurrentAlpha( a ); }
    void showAlpha( bool b ) { cs->showAlpha( b ); }

private slots:
    void addCustom();

    void newHsv( int h, int s, int v );
    void newColorTypedIn( QRgb rgb );
    void newCustom( int, int );
    void newStandard( int, int );
private:
    QColorPicker *cp;
    QColorLuminancePicker *lp;
    QWellArray *custom;
    QWellArray *standard;
    QColorShower *cs;
    int nextCust;
    bool compact;
};

#endif
