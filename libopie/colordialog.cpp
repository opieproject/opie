/****************************************************************************
** $Id: colordialog.cpp,v 1.1 2002-04-25 23:32:41 drw Exp $
**
** Implementation of QColorDialog class
**
** Created : 990222
**
** Copyright (C) 1999-2000 Trolltech AS.  All rights reserved.
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

#include "colordialog.h"

#include "qpainter.h"
#include "qlayout.h"
#include "qlabel.h"
#include "qpushbutton.h"
#include "qlineedit.h"
#include "qimage.h"
#include "qpixmap.h"
#include "qdrawutil.h"
#include "qvalidator.h"
#include "qdragobject.h"
#include "qapplication.h"
#include "qdragobject.h"

static inline void rgb2hsv( QRgb rgb, int&h, int&s, int&v )
{
    QColor c;
    c.setRgb( rgb );
    c.getHsv(h,s,v);
}

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

static int pWidth = 200;
static int pHeight = 200;

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


int QColorLuminancePicker::y2val( int y )
{
    int d = height() - 2*coff - 1;
    return 255 - (y - coff)*255/d;
}

int QColorLuminancePicker::val2y( int v )
{
    int d = height() - 2*coff - 1;
    return coff + (255-v)*d/255;
}

QColorLuminancePicker::QColorLuminancePicker(QWidget* parent,
						  const char* name)
    :QWidget( parent, name )
{
    hue = 100; val = 100; sat = 100;
    pix = 0;
    //    setBackgroundMode( NoBackground );
}

QColorLuminancePicker::~QColorLuminancePicker()
{
    delete pix;
}

void QColorLuminancePicker::mouseMoveEvent( QMouseEvent *m )
{
    setVal( y2val(m->y()) );
}
void QColorLuminancePicker::mousePressEvent( QMouseEvent *m )
{
    setVal( y2val(m->y()) );
}

void QColorLuminancePicker::setVal( int v )
{
    if ( val == v )
	return;
    val = QMAX( 0, QMIN(v,255));
    delete pix; pix=0;
    repaint( FALSE ); //###
    emit newHsv( hue, sat, val );
}

//receives from a hue,sat chooser and relays.
void QColorLuminancePicker::setCol( int h, int s )
{
    setCol( h, s, val );
    emit newHsv( h, s, val );
}

void QColorLuminancePicker::paintEvent( QPaintEvent * )
{
    int w = width() - 5;

    QRect r( 0, foff, w, height() - 2*foff );
    int wi = r.width() - 2;
    int hi = r.height() - 2;
    if ( !pix || pix->height() != hi || pix->width() != wi ) {
	delete pix;
	QImage img( wi, hi, 32 );
	int y;
	for ( y = 0; y < hi; y++ ) {
	    QColor c( hue, sat, y2val(y+coff), QColor::Hsv );
	    QRgb r = c.rgb();
	    int x;
	    for ( x = 0; x < wi; x++ )
		img.setPixel( x, y, r );
	}
	pix = new QPixmap;
	pix->convertFromImage(img);
    }
    QPainter p(this);
    p.drawPixmap( 1, coff, *pix );
    QColorGroup g = colorGroup();
    qDrawShadePanel( &p, r, g, TRUE );
    p.setPen( g.foreground() );
    p.setBrush( g.foreground() );
    QPointArray a;
    int y = val2y(val);
    a.setPoints( 3, w, y, w+5, y+5, w+5, y-5 );
    erase( w, 0, 5, height() );
    p.drawPolygon( a );
}

void QColorLuminancePicker::setCol( int h, int s , int v )
{
    val = v;
    hue = h;
    sat = s;
    delete pix; pix=0;
    repaint( FALSE );//####
}

QPoint QColorPicker::colPt()
{ return QPoint( (360-hue)*(pWidth-1)/360, (255-sat)*(pHeight-1)/255 ); }
int QColorPicker::huePt( const QPoint &pt )
{ return 360 - pt.x()*360/(pWidth-1); }
int QColorPicker::satPt( const QPoint &pt )
{ return 255 - pt.y()*255/(pHeight-1) ; }
void QColorPicker::setCol( const QPoint &pt )
{ setCol( huePt(pt), satPt(pt) ); }

QColorPicker::QColorPicker(QWidget* parent, const char* name )
    : QFrame( parent, name )
{
    hue = 0; sat = 0;
    setCol( 150, 255 );

    QImage img( pWidth, pHeight, 32 );
    int x,y;
    for ( y = 0; y < pHeight; y++ )
	for ( x = 0; x < pWidth; x++ ) {
	    QPoint p( x, y );
	    img.setPixel( x, y, QColor(huePt(p), satPt(p),
				       200, QColor::Hsv).rgb() );
	}
    pix = new QPixmap;
    pix->convertFromImage(img);
    setBackgroundMode( NoBackground );
}

QColorPicker::~QColorPicker()
{
    delete pix;
}

QSize QColorPicker::sizeHint() const
{
    return QSize( pWidth + 2*frameWidth(), pHeight + 2*frameWidth() );
}

QSizePolicy QColorPicker::sizePolicy() const
{
    return QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
}

void QColorPicker::setCol( int h, int s )
{
    int nhue = QMIN( QMAX(0,h), 360 );
    int nsat = QMIN( QMAX(0,s), 255);
    if ( nhue == hue && nsat == sat )
	return;
    QRect r( colPt(), QSize(20,20) );
    hue = nhue; sat = nsat;
    r = r.unite( QRect( colPt(), QSize(20,20) ) );
    r.moveBy( contentsRect().x()-9, contentsRect().y()-9 );
    //    update( r );
    repaint( r, FALSE );
}

void QColorPicker::mouseMoveEvent( QMouseEvent *m )
{
    QPoint p = m->pos() - contentsRect().topLeft();
    setCol( p );
    emit newCol( hue, sat );
}

void QColorPicker::mousePressEvent( QMouseEvent *m )
{
    QPoint p = m->pos() - contentsRect().topLeft();
    setCol( p );
    emit newCol( hue, sat );
}

void QColorPicker::drawContents(QPainter* p)
{
    QRect r = contentsRect();

    p->drawPixmap( r.topLeft(), *pix );
    QPoint pt = colPt() + r.topLeft();
    p->setPen( QPen(black) );

    p->fillRect( pt.x()-9, pt.y(), 20, 2, black );
    p->fillRect( pt.x(), pt.y()-9, 2, 20, black );

}

class QColorShowLabel;



class QColIntValidator: public QIntValidator
{
public:
    QColIntValidator( int bottom, int top,
		   QWidget * parent, const char *name = 0 )
	:QIntValidator( bottom, top, parent, name ) {}

    QValidator::State validate( QString &, int & ) const;
};

QValidator::State QColIntValidator::validate( QString &s, int &pos ) const
{
    State state = QIntValidator::validate(s,pos);
    if ( state == Valid ) {
	long int val = s.toLong();
	// This is not a general solution, assumes that top() > 0 and
	// bottom >= 0
	if ( val < 0 ) {
	    s = "0";
	    pos = 1;
	} else if ( val > top() ) {
	    s.setNum( top() );
	    pos = s.length();
	}
    }
    return state;
}



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
    void mouseReleaseEvent( QMouseEvent *e );

private:
    QColor col;
    bool mousePressed;
    QPoint pressPos;

};

void QColorShowLabel::drawContents( QPainter *p )
{
    p->fillRect( contentsRect(), col );
}

void QColorShower::showAlpha( bool b )
{
    if ( b ) {
	alphaLab->show();
	alphaEd->show();
    } else {
	alphaLab->hide();
	alphaEd->hide();
    }
}

void QColorShowLabel::mousePressEvent( QMouseEvent *e )
{
    mousePressed = TRUE;
    pressPos = e->pos();
}

void QColorShowLabel::mouseReleaseEvent( QMouseEvent * )
{
    if ( !mousePressed )
	return;
    mousePressed = FALSE;
}

QColorShower::QColorShower( QWidget *parent, const char *name )
    :QWidget( parent, name)
{
    curCol = qRgb( -1, -1, -1 );
    QColIntValidator *val256 = new QColIntValidator( 0, 255, this );
    QColIntValidator *val360 = new QColIntValidator( 0, 360, this );

    QGridLayout *gl = new QGridLayout( this, 1, 1, 2 );
    gl->setMargin( 0 );
    lab = new QColorShowLabel( this );
    lab->setMinimumWidth( 60 ); //###
    gl->addMultiCellWidget(lab, 0,-1,0,0);
    connect( lab, SIGNAL( colorDropped( QRgb ) ),
	     this, SIGNAL( newCol( QRgb ) ) );
    connect( lab, SIGNAL( colorDropped( QRgb ) ),
	     this, SLOT( setRgb( QRgb ) ) );

    hEd = new QColNumLineEdit( this );
    hEd->setValidator( val360 );
    QLabel *l = new QLabel( hEd, QColorDialog::tr("Hue:"), this );
    l->setAlignment( AlignRight|AlignVCenter );
    gl->addWidget( l, 0, 1 );
    gl->addWidget( hEd, 0, 2 );

    sEd = new QColNumLineEdit( this );
    sEd->setValidator( val256 );
    l = new QLabel( sEd, QColorDialog::tr("Sat:"), this );
    l->setAlignment( AlignRight|AlignVCenter );
    gl->addWidget( l, 1, 1 );
    gl->addWidget( sEd, 1, 2 );

    vEd = new QColNumLineEdit( this );
    vEd->setValidator( val256 );
    l = new QLabel( vEd, QColorDialog::tr("Val:"), this );
    l->setAlignment( AlignRight|AlignVCenter );
    gl->addWidget( l, 2, 1 );
    gl->addWidget( vEd, 2, 2 );

    rEd = new QColNumLineEdit( this );
    rEd->setValidator( val256 );
    l = new QLabel( rEd, QColorDialog::tr("Red:"), this );
    l->setAlignment( AlignRight|AlignVCenter );
    gl->addWidget( l, 0, 3 );
    gl->addWidget( rEd, 0, 4 );

    gEd = new QColNumLineEdit( this );
    gEd->setValidator( val256 );
    l = new QLabel( gEd, QColorDialog::tr("Green:"), this );
    l->setAlignment( AlignRight|AlignVCenter );
    gl->addWidget( l, 1, 3 );
    gl->addWidget( gEd, 1, 4 );

    bEd = new QColNumLineEdit( this );
    bEd->setValidator( val256 );
    l = new QLabel( bEd, QColorDialog::tr("Blue:"), this );
    l->setAlignment( AlignRight|AlignVCenter );
    gl->addWidget( l, 2, 3 );
    gl->addWidget( bEd, 2, 4 );

    alphaEd = new QColNumLineEdit( this );
    alphaEd->setValidator( val256 );
    alphaLab = new QLabel( alphaEd, QColorDialog::tr("Alpha channel:"), this );
    alphaLab->setAlignment( AlignRight|AlignVCenter );
    gl->addMultiCellWidget( alphaLab, 3, 3, 1, 3 );
    gl->addWidget( alphaEd, 3, 4 );
    alphaEd->hide();
    alphaLab->hide();

    connect( hEd, SIGNAL(textChanged(const QString&)), this, SLOT(hsvEd()) );
    connect( sEd, SIGNAL(textChanged(const QString&)), this, SLOT(hsvEd()) );
    connect( vEd, SIGNAL(textChanged(const QString&)), this, SLOT(hsvEd()) );

    connect( rEd, SIGNAL(textChanged(const QString&)), this, SLOT(rgbEd()) );
    connect( gEd, SIGNAL(textChanged(const QString&)), this, SLOT(rgbEd()) );
    connect( bEd, SIGNAL(textChanged(const QString&)), this, SLOT(rgbEd()) );
}

void QColorShower::showCurrentColor()
{
    lab->setColor( currentColor() );
    lab->repaint(FALSE); //###
}

void QColorShower::rgbEd()
{
    rgbOriginal = TRUE;
    curCol = qRgb( rEd->val(), gEd->val(), bEd->val() );
    rgb2hsv(currentColor(), hue, sat, val );

    hEd->setNum( hue );
    sEd->setNum( sat );
    vEd->setNum( val );

    showCurrentColor();
    emit newCol( currentColor() );
}

void QColorShower::hsvEd()
{
    rgbOriginal = FALSE;
    hue = hEd->val();
    sat = sEd->val();
    val = vEd->val();

    curCol = QColor( hue, sat, val, QColor::Hsv ).rgb();

    rEd->setNum( qRed(currentColor()) );
    gEd->setNum( qGreen(currentColor()) );
    bEd->setNum( qBlue(currentColor()) );

    showCurrentColor();
    emit newCol( currentColor() );
}

void QColorShower::setRgb( QRgb rgb )
{
    rgbOriginal = TRUE;
    curCol = rgb;

    rgb2hsv( currentColor(), hue, sat, val );

    hEd->setNum( hue );
    sEd->setNum( sat );
    vEd->setNum( val );

    rEd->setNum( qRed(currentColor()) );
    gEd->setNum( qGreen(currentColor()) );
    bEd->setNum( qBlue(currentColor()) );

    showCurrentColor();
}

void QColorShower::setHsv( int h, int s, int v )
{
    rgbOriginal = FALSE;
    hue = h; val = v; sat = s; //Range check###
    curCol = QColor( hue, sat, val, QColor::Hsv ).rgb();

    hEd->setNum( hue );
    sEd->setNum( sat );
    vEd->setNum( val );

    rEd->setNum( qRed(currentColor()) );
    gEd->setNum( qGreen(currentColor()) );
    bEd->setNum( qBlue(currentColor()) );


    showCurrentColor();
}

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
    void newHsv( int h, int s, int v );
    void newColorTypedIn( QRgb rgb );
private:
    QColorPicker *cp;
    QColorLuminancePicker *lp;
    QColorShower *cs;
};

//sets all widgets to display h,s,v
void QColorDialogPrivate::newHsv( int h, int s, int v )
{
    cs->setHsv( h, s, v );
    cp->setCol( h, s );
    lp->setCol( h, s, v );
}

//sets all widgets to display rgb
void QColorDialogPrivate::setCurrentColor( QRgb rgb )
{
    cs->setRgb( rgb );
    newColorTypedIn( rgb );
}

//sets all widgets exept cs to display rgb
void QColorDialogPrivate::newColorTypedIn( QRgb rgb )
{
    int h, s, v;
    rgb2hsv(rgb, h, s, v );
    cp->setCol( h, s );
    lp->setCol( h, s, v);
}

QColorDialogPrivate::QColorDialogPrivate( QColorDialog *dialog ) :
    QObject(dialog)
{
    int border = 2;
    QVBoxLayout *topLay = new QVBoxLayout( dialog, border, 2 );

    QHBoxLayout *pickLay = new QHBoxLayout( topLay );


    cp = new QColorPicker( dialog );
    cp->setFrameStyle( QFrame::Panel + QFrame::Sunken );
    pickLay->addWidget( cp );

    pickLay->addStretch();

    lp = new QColorLuminancePicker( dialog );
    lp->setFixedWidth( 20 ); //###
    pickLay->addWidget( lp );

    connect( cp, SIGNAL(newCol(int,int)), lp, SLOT(setCol(int,int)) );
    connect( lp, SIGNAL(newHsv(int,int,int)), this, SLOT(newHsv(int,int,int)) );

    topLay->addStretch();

    cs = new QColorShower( dialog );
    connect( cs, SIGNAL(newCol(QRgb)), this, SLOT(newColorTypedIn(QRgb)));
    topLay->addWidget( cs );

}


// BEING REVISED: jo
/*!
  \class QColorDialog qcolordialog.h
  \brief The QColorDialog class provides a dialog widget for specifying colors.
  \ingroup dialogs

  The color dialog's function is to allow users to choose colors -
  for instance, you might use this in a drawing program to allow the
  user to set the brush color.

  This version of Qt only provides modal color dialogs. The static
  getColor() function shows the dialog and allows the user to specify a color,
  while getRgba() does the same but allows the user to specify a color with an
  alpha channel (transparency) value.

  The user can store customCount() different custom colors. The custom
  colors are shared by all color dialogs, and remembered during the
  execution of the program. Use setCustomColor() to set the
  custom colors, and customColor() to get them.

  <img src=qcolordlg-m.png> <img src=qcolordlg-w.png>
*/

/*!
  Constructs a default color dialog. Use setColor() for setting an initial value.

  \sa getColor()
*/

QColorDialog::QColorDialog(QWidget* parent, const char* name, bool modal) :
    QDialog(parent, name, modal )
{
    d = new QColorDialogPrivate( this );
}


/*!
  Pops up a modal color dialog letting the user choose a color and returns
  that color. The color is initially set to \a initial. Returns an  \link QColor::isValid() invalid\endlink color if the user cancels
  the dialog. All colors allocated by the dialog will be deallocated
  before this function returns.
*/

QColor QColorDialog::getColor( QColor initial, QWidget *parent,
			       const char *name )
{
    int allocContext = QColor::enterAllocContext();
    QColorDialog *dlg = new QColorDialog( parent, name, TRUE );  //modal
    if ( parent && parent->icon() && !parent->icon()->isNull() )
	dlg->setIcon( *parent->icon() );
    else if ( qApp->mainWidget() && qApp->mainWidget()->icon() && !qApp->mainWidget()->icon()->isNull() )
	dlg->setIcon( *qApp->mainWidget()->icon() );

    dlg->setCaption( QColorDialog::tr( "Select color" ) );
    dlg->setColor( initial );
    dlg->showMaximized();
    int resultCode = dlg->exec();
    QColor::leaveAllocContext();
    QColor result;
    if ( resultCode == QDialog::Accepted ) {
	result = dlg->color();
    } else {
        result = initial;
    }
    QColor::destroyAllocContext(allocContext);
    delete dlg;
    return result;
}


/*!
  Pops up a modal color dialog, letting the user choose a color and an
  alpha channel value. The color+alpha is initially set to \a initial.

  If \a ok is non-null, \c *ok is set to TRUE if the user clicked OK,
  and FALSE if the user clicked Cancel.

  If the user clicks Cancel the \a initial value is returned.
*/

QRgb QColorDialog::getRgba( QRgb initial, bool *ok,
			    QWidget *parent, const char* name )
{
    int allocContext = QColor::enterAllocContext();
    QColorDialog *dlg = new QColorDialog( parent, name, TRUE );  //modal
    dlg->setColor( initial );
    dlg->setSelectedAlpha( qAlpha(initial) );
    dlg->showMaximized();
    int resultCode = dlg->exec();
    QColor::leaveAllocContext();
    QRgb result = initial;
    if ( resultCode == QDialog::Accepted ) {
	QRgb c = dlg->color().rgb();
	int alpha = dlg->selectedAlpha();
	result = qRgba( qRed(c), qGreen(c), qBlue(c), alpha );
    }
    if ( ok )
	*ok = resultCode == QDialog::Accepted;

    QColor::destroyAllocContext(allocContext);
    delete dlg;
    return result;
}





/*!
  Returns the color currently selected in the dialog.

  \sa setColor()
*/

QColor QColorDialog::color() const
{
    return QColor(d->currentColor());
}


/*! Destructs the dialog and frees any memory it allocated.

*/

QColorDialog::~QColorDialog()
{
    //d inherits QObject, so it is deleted by Qt.
}


/*!
  Sets the color shown in the dialog to \a c.

  \sa color()
*/

void QColorDialog::setColor( QColor c )
{
    d->setCurrentColor( c.rgb() );
}




/*!
  Sets the initial alpha channel value to \a a, and show the alpha channel
  entry box.
*/

void QColorDialog::setSelectedAlpha( int a )
{
    d->showAlpha( TRUE );
    d->setCurrentAlpha( a );
}


/*!
  Returns the value selected for the alpha channel.
*/

int QColorDialog::selectedAlpha() const
{
    return d->currentAlpha();
}

#include "colordialog.moc"
