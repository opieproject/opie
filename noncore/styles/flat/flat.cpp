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

#include "flat.h"
#include <qpe/qpeapplication.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qpainter.h>
#include <qfontmetrics.h>
#include <qpalette.h>
#include <qdrawutil.h>
#include <qscrollbar.h>
#include <qbutton.h>
#include <qframe.h>
#include <qtabbar.h>
#include <qspinbox.h>
#include <qlineedit.h>

#define INCLUDE_MENUITEM_DEF
#include <qmenudata.h>

#define QCOORDARRLEN(x) sizeof(x)/(sizeof(QCOORD)*2)

class SpinBoxHack : public QSpinBox
{
public:
    void setFlatButtons( bool f ) {
	upButton()->setFlat( f );
	downButton()->setFlat( f );
    }
};

class FlatStylePrivate : public QObject
{
    Q_OBJECT
public:
    FlatStylePrivate() : QObject() {}

    bool eventFilter( QObject *o, QEvent *e ) {
	if ( e->type() == QEvent::ParentPaletteChange && o->inherits( "QMenuBar" ) ) {
	    QWidget *w = (QWidget *)o;
	    if ( w->parentWidget() ) {
		QPalette p = w->parentWidget()->palette();
		QColorGroup a = p.active();
		a.setColor( QColorGroup::Light, a.foreground() );
		a.setColor( QColorGroup::Dark, a.foreground() );
		p.setActive( a );
		p.setInactive( a );
		w->setPalette( p );
	    }
	}
	return FALSE;
    }
};

FlatStyle::FlatStyle() : revItem(FALSE)
{
    setButtonMargin(3);
    setScrollBarExtent(13,13);
    setButtonDefaultIndicatorWidth(0);
    d = new FlatStylePrivate;
}

FlatStyle::~FlatStyle()
{
    delete d;
}

int FlatStyle::buttonMargin() const
{
    return 3;
}

QSize FlatStyle::scrollBarExtent() const
{
    return QSize(13,13);
}

void FlatStyle::polish ( QPalette & )
{
}

void FlatStyle::polish( QWidget *w )
{
    if ( w->inherits( "QFrame" ) ) {
	QFrame *f = (QFrame *)w;
	if ( f->frameShape() != QFrame::NoFrame )
	    f->setFrameShape( QFrame::StyledPanel );
	f->setLineWidth( 1 );
    }
    if ( w->inherits( "QSpinBox" ) )
	((SpinBoxHack*)w)->setFlatButtons( TRUE );
    if ( w->inherits( "QMenuBar" ) ) {
	// make selected item look flat
	QPalette p = w->palette();
	QColorGroup a = p.active();
	a.setColor( QColorGroup::Light, a.foreground() );
	a.setColor( QColorGroup::Dark, a.foreground() );
	p.setActive( a );
	p.setInactive( a );
	w->setPalette( p );
	w->installEventFilter( d );
    }
}

void FlatStyle::unPolish( QWidget *w )
{
    if ( w->inherits( "QFrame" ) ) {
	QFrame *f = (QFrame *)w;
	if ( f->frameShape() != QFrame::NoFrame )
	    f->setFrameShape( QFrame::StyledPanel );
	f->setLineWidth( 2 );
    }
    if ( w->inherits( "QSpinBox" ) )
	((SpinBoxHack*)w)->setFlatButtons( FALSE );
    if ( w->inherits( "QMenuBar" ) ) {
	w->unsetPalette();
	w->removeEventFilter( d );
    }
}

int FlatStyle::defaultFrameWidth() const
{
    return 1;
}

void FlatStyle::drawItem( QPainter *p, int x, int y, int w, int h,
                    int flags, const QColorGroup &g, bool enabled,
		    const QPixmap *pixmap, const QString& text, int len,
		    const QColor* penColor )
{
    QColor pc( penColor ? *penColor : g.foreground() );
    QColorGroup cg( g );
    if ( !enabled )
	cg.setColor( QColorGroup::Light, cg.background() );
    if ( revItem ) {
	pc = cg.button();
	revItem = FALSE;
    }
    QWindowsStyle::drawItem( p, x, y, w, h, flags, cg, enabled, pixmap, text, len, &pc );
}

void FlatStyle::drawPanel ( QPainter * p, int x, int y, int w, int h,
			    const QColorGroup &g, bool /*sunken*/, int lineWidth, const QBrush * fill )
{
    if ( fill )
	p->setBrush( *fill );
    p->setPen( QPen(g.foreground(), lineWidth) );
    p->drawRect( x, y, w, h );
}

void FlatStyle::drawButton( QPainter *p, int x, int y, int w, int h,
                                const QColorGroup &cg, bool /*sunken*/, const QBrush* fill )
{
    QPen oldPen = p->pen();

    if ( h >= 10 ) {
	x++; y++;
	w -= 2; h -= 2;
    }

    p->fillRect( x+1, y+1, w-2, h-2, fill?(*fill):cg.brush(QColorGroup::Button) );

    int x2 = x+w-1;
    int y2 = y+h-1;

    p->setPen( cg.foreground() );

    if ( h < 10 ) {
	p->setBrush( NoBrush );
	p->drawRect( x, y, w, h );
    } else {
	p->drawLine( x+3, y, x2-3, y );
	p->drawLine( x+3, y2, x2-3, y2 );
	p->drawLine( x, y+3, x, y2-3 );
	p->drawLine( x2, y+3, x2, y2-3 );

	p->drawLine( x+1, y+1, x+2, y+1 );
	p->drawPoint( x+1, y+2 );
	p->drawLine( x2-2, y+1, x2-1, y+1 );
	p->drawPoint( x2-1, y+2 );

	p->drawLine( x+1, y2-1, x+2, y2-1 );
	p->drawPoint( x+1, y2-2 );
	p->drawLine( x2-2, y2-1, x2-1, y2-1 );
	p->drawPoint( x2-1, y2-2 );
    }

    p->setPen( oldPen );
}

void FlatStyle::drawButtonMask ( QPainter * p, int x, int y, int w, int h )
{
    x++; y++;
    x-=2; y-=2;
    p->fillRect( x, y, w, h, color1 );
}

void FlatStyle::drawBevelButton( QPainter *p, int x, int y, int w, int h,
                                const QColorGroup &g, bool /*sunken*/, const QBrush* fill )
{
    p->fillRect( x+1, y+1, w-2, h-2, fill?(*fill):g.brush(QColorGroup::Button) );
    p->setPen( g.foreground() );
    p->setBrush( NoBrush );
    p->drawRect( x, y, w, h );
}

void FlatStyle::drawToolButton( QPainter *p, int x, int y, int w, int h,
                                const QColorGroup &g, bool sunken, const QBrush* fill )
{
    QBrush fb( fill ? *fill : g.button() );
    if ( sunken && fb == g.brush( QColorGroup::Button ) ) {
	fb = g.buttonText();
	revItem = TRUE;	// ugh
    }
    drawButton( p, x, y, w, h, g, sunken, &fb );
}

void FlatStyle::drawPushButton( QPushButton *btn, QPainter *p )
{
    QColorGroup g = btn->colorGroup();
    int x1, y1, x2, y2;

    btn->rect().coords( &x1, &y1, &x2, &y2 );	// get coordinates

    p->setPen( g.foreground() );
    p->setBrush( QBrush(g.button(),NoBrush) );

//    int diw = buttonDefaultIndicatorWidth();
    /*
    if ( btn->isDefault() || btn->autoDefault() ) {
	if ( btn->isDefault() ) {
	    p->setPen( g.shadow() );
	    p->drawRect( x1, y1, x2-x1+1, y2-y1+1 );
	}
	x1 += diw;
	y1 += diw;
	x2 -= diw;
	y2 -= diw;
    }
    */

    bool clearButton = TRUE;
    if ( btn->isDown() ) {
	drawButton( p, x1, y1, x2-x1+1, y2-y1+1, g, TRUE,
		&g.brush( QColorGroup::Text ) );
    } else {
	if ( btn->isToggleButton() && btn->isOn() && btn->isEnabled() ) {
	    QBrush fill(g.light(), Dense4Pattern );
	    drawButton( p, x1, y1, x2-x1+1, y2-y1+1, g, TRUE, &fill );
	    clearButton = FALSE;
	} else {
	    if ( !btn->isFlat() )
		drawButton( p, x1, y1, x2-x1+1, y2-y1+1, g, btn->isOn(),
			&g.brush( QColorGroup::Button ) );
	}
    }
    /*
    if ( clearButton ) {
	if (btn->isDown())
	    p->setBrushOrigin(p->brushOrigin() + QPoint(1,1));
	p->fillRect( x1+2, y1+2, x2-x1-3, y2-y1-3,
		     g.brush( QColorGroup::Button ) );
	if (btn->isDown())
	    p->setBrushOrigin(p->brushOrigin() - QPoint(1,1));
    }
    */

    if ( p->brush().style() != NoBrush )
	p->setBrush( NoBrush );
}

void FlatStyle::drawPushButtonLabel( QPushButton *btn, QPainter *p )
{
    QRect r = pushButtonContentsRect( btn );
    int x, y, w, h;
    r.rect( &x, &y, &w, &h );
    QColorGroup cg = btn->colorGroup();
    if ( btn->isToggleButton() && btn->isOn() && btn->isEnabled() && !btn->isDown() )
	cg.setColor( QColorGroup::ButtonText, btn->colorGroup().text() );
    else if ( btn->isDown() || btn->isOn() )
	cg.setColor( QColorGroup::ButtonText, btn->colorGroup().button() );
    if ( btn->isMenuButton() ) {
	int dx = menuButtonIndicatorWidth( btn->height() );
	drawArrow( p, DownArrow, FALSE,
		x+w-dx, y+2, dx-4, h-4,
		cg,
		btn->isEnabled() );
	w -= dx;
    }

    if ( btn->iconSet() && !btn->iconSet()->isNull() ) {
	QIconSet::Mode mode = btn->isEnabled()
	    ? QIconSet::Normal : QIconSet::Disabled;
	if ( mode == QIconSet::Normal && btn->hasFocus() )
	    mode = QIconSet::Active;
	QPixmap pixmap = btn->iconSet()->pixmap( QIconSet::Small, mode );
	int pixw = pixmap.width();
	int pixh = pixmap.height();
	p->drawPixmap( x+2, y+h/2-pixh/2, pixmap );
	x += pixw + 4;
	w -= pixw + 4;
    }
    drawItem( p, x, y, w, h,
	    AlignCenter | ShowPrefix,
	    cg, btn->isEnabled(),
	    btn->pixmap(), btn->text(), -1, &cg.buttonText() );

}

QRect FlatStyle::comboButtonRect( int x, int y, int w, int h)
{
    return QRect(x+2, y+2, w-4-13, h-4);
}
      
       
QRect FlatStyle::comboButtonFocusRect( int x, int y, int w, int h)
{
    return QRect(x+2, y+2, w-4-14, h-4);
}

void FlatStyle::drawComboButton( QPainter *p, int x, int y, int w, int h,
				     const QColorGroup &g, bool sunken,
				     bool /*editable*/,
				     bool enabled,
				     const QBrush * /*fill*/ )
{
    x++; y++;
    w-=2; h-=2;
    p->setPen( g.foreground() );
    p->setBrush( QBrush(NoBrush) );
    p->drawRect( x, y, w, h );
    p->setPen( g.background() );
    p->drawRect( x+1, y+1, w-14, h-2 );
    p->fillRect( x+2, y+2, w-16, h-4, g.brush( QColorGroup::Base ) );
    QColorGroup cg( g );
    if ( sunken ) {
	cg.setColor( QColorGroup::ButtonText, g.button() );
	cg.setColor( QColorGroup::Button, g.buttonText() );
    }
    drawArrow( p, QStyle::DownArrow, FALSE,
	       x+w-13, y+1, 12, h-2, cg, enabled,
	       &cg.brush( QColorGroup::Button ) );

}


void FlatStyle::drawExclusiveIndicator ( QPainter * p, int x, int y, int w,
	int h, const QColorGroup &cg, bool on, bool down, bool enabled )
{
    static const QCOORD pts1[] = {              // dark lines
	1,9, 1,8, 0,7, 0,4, 1,3, 1,2, 2,1, 3,1, 4,0, 7,0, 8,1, 9,1 };
    static const QCOORD pts4[] = {              // white lines
	2,10, 3,10, 4,11, 7,11, 8,10, 9,10, 10,9, 10,8, 11,7,
	11,4, 10,3, 10,2 };
    static const QCOORD pts5[] = {              // inner fill
	4,2, 7,2, 9,4, 9,7, 7,9, 4,9, 2,7, 2,4 };

    p->eraseRect( x, y, w, h );
    QPointArray a( QCOORDARRLEN(pts1), pts4 );
    a.translate( x, y );
    p->setPen( cg.foreground() );
    p->drawPolyline( a );
    a.setPoints( QCOORDARRLEN(pts4), pts1 );
    a.translate( x, y );
    p->setPen( cg.foreground() );
    p->drawPolyline( a );
    a.setPoints( QCOORDARRLEN(pts5), pts5 );
    a.translate( x, y );
    QColor fillColor = ( down || !enabled ) ? cg.button() : cg.base();
    p->setPen( fillColor );
    p->setBrush( fillColor  ) ;
    p->drawPolygon( a );
    if ( on ) {
	p->setPen( NoPen );
	p->setBrush( cg.text() );
	p->drawRect( x+5, y+4, 2, 4 );
	p->drawRect( x+4, y+5, 4, 2 );
    }
}

void FlatStyle::drawIndicator ( QPainter * p, int x, int y, int w, int h,
	const QColorGroup &cg, int state, bool down, bool enabled )
{
    QColorGroup mycg( cg );
    mycg.setBrush( QColorGroup::Button, QBrush() );
    QBrush fill;
    drawButton( p, x, y, w, h, mycg, TRUE, 0 );
    if ( down )
	fill = cg.brush( QColorGroup::Button );
    else
	fill = cg.brush( enabled ? QColorGroup::Base : QColorGroup::Background );
    mycg.setBrush( QColorGroup::Button, fill );
    p->fillRect( x+1, y+1, w-2, h-2, fill );
    if ( state != QButton::Off ) {
	QPointArray a( 7*2 );
	int i, xx, yy;
	xx = x+3;
	yy = y+5;
	for ( i=0; i<3; i++ ) {
	    a.setPoint( 2*i,   xx, yy );
	    a.setPoint( 2*i+1, xx, yy+2 );
	    xx++; yy++;
	}
	yy -= 2;
	for ( i=3; i<7; i++ ) {
	    a.setPoint( 2*i,   xx, yy );
	    a.setPoint( 2*i+1, xx, yy+2 );
	    xx++; yy--;
	}
	if ( state == QButton::NoChange ) {
	    p->setPen( mycg.dark() );
	} else {
	    p->setPen( mycg.text() );
	}
	p->drawLineSegments( a );
    }
}

#define HORIZONTAL	(sb->orientation() == QScrollBar::Horizontal)
#define VERTICAL	!HORIZONTAL
#define MOTIF_BORDER	2
#define SLIDER_MIN	9 // ### motif says 6 but that's too small

/*! \reimp */

void FlatStyle::scrollBarMetrics( const QScrollBar* sb, int &sliderMin, int &sliderMax, int &sliderLength, int& buttonDim )
{
    int maxLength;
    int length = HORIZONTAL ? sb->width()  : sb->height();
    int extent = HORIZONTAL ? sb->height() : sb->width();

    if ( length > (extent - 1)*2 )
	buttonDim = extent;
    else
	buttonDim = length/2 - 1;

    sliderMin = 0;
    maxLength  = length - buttonDim*2 + 2;

     if ( sb->maxValue() == sb->minValue() ) {
	sliderLength = maxLength;
     } else {
	sliderLength = (sb->pageStep()*maxLength)/
			(sb->maxValue()-sb->minValue()+sb->pageStep());
	uint range = sb->maxValue()-sb->minValue();
	if ( sliderLength < SLIDER_MIN || range > INT_MAX/2 )
	    sliderLength = SLIDER_MIN;
	if ( sliderLength > maxLength )
	    sliderLength = maxLength;
     }

    sliderMax = sliderMin + maxLength - sliderLength;
}

/*!\reimp
 */
QStyle::ScrollControl FlatStyle::scrollBarPointOver( const QScrollBar* sb, int sliderStart, const QPoint& p )
{
    if ( !sb->rect().contains( p ) )
	return NoScroll;
    int sliderMin, sliderMax, sliderLength, buttonDim, pos;
    scrollBarMetrics( sb, sliderMin, sliderMax, sliderLength, buttonDim );

    if (sb->orientation() == QScrollBar::Horizontal)
	pos = p.x();
    else
	pos = p.y();

    if (pos < sliderStart)
	return SubPage;
    if (pos < sliderStart + sliderLength)
	return Slider;
    if (pos < sliderMax + sliderLength)
	return AddPage;
    if (pos < sliderMax + sliderLength + buttonDim)
	return SubLine;
    return AddLine;
}

/*! \reimp */

void FlatStyle::drawScrollBarControls( QPainter* p, const QScrollBar* sb, int sliderStart, uint controls, uint activeControl )
{
#define ADD_LINE_ACTIVE ( activeControl == AddLine )
#define SUB_LINE_ACTIVE ( activeControl == SubLine )
    QColorGroup g  = sb->colorGroup();

    int sliderMin, sliderMax, sliderLength, buttonDim;
    scrollBarMetrics( sb, sliderMin, sliderMax, sliderLength, buttonDim );

    if ( controls == (AddLine | SubLine | AddPage | SubPage | Slider | First | Last ) ) {
	p->setPen( g.foreground() );
	p->setBrush( g.brush( QColorGroup::Mid ) );
	p->drawRect( 0, 0, sb->width(), sb->height() );
    }

    if (sliderStart > sliderMax) { // sanity check
	sliderStart = sliderMax;
    }

    int dimB = buttonDim;
    QRect addB;
    QRect subB;
    QRect addPageR;
    QRect subPageR;
    QRect sliderR;
    int addX, addY, subX, subY;
    int length = HORIZONTAL ? sb->width()  : sb->height();
    int extent = HORIZONTAL ? sb->height() : sb->width();

    if ( HORIZONTAL ) {
	subY = addY = ( extent - dimB ) / 2;
	subX = length - dimB - dimB + 1;
	addX = length - dimB;
    } else {
	subX = addX = ( extent - dimB ) / 2;
	subY = length - dimB - dimB + 1;
	addY = length - dimB;
    }

    int sliderEnd = sliderStart + sliderLength;
    int sliderW = extent;
    if ( HORIZONTAL ) {
	subB.setRect( subX,subY,dimB,dimB );
	addB.setRect( addX,addY,dimB,dimB );
	subPageR.setRect( 0, 0, sliderStart+1, sliderW );
	addPageR.setRect( sliderEnd-1, 0, subX - sliderEnd+2, sliderW );
	sliderR .setRect( sliderStart, 0, sliderLength, sliderW );

    } else {
	subB.setRect( subX,subY,dimB,dimB );
	addB.setRect( addX,addY,dimB,dimB );
	subPageR.setRect( 0, 0, sliderW, sliderStart+1 );
	addPageR.setRect( 0, sliderEnd-1, sliderW, subY - sliderEnd+2 );
	sliderR .setRect( 0, sliderStart, sliderW, sliderLength );
    }

    bool maxedOut = (sb->maxValue() == sb->minValue());
    p->setPen( g.foreground() );
    if ( controls & AddLine ) {
	p->setBrush( ADD_LINE_ACTIVE ? g.foreground() : g.button() );
	p->drawRect( addB.x(), addB.y(), addB.width(), addB.height() );
	p->setPen( ADD_LINE_ACTIVE ? g.button() : g.foreground() );
	QColorGroup cg( g );
	if ( maxedOut )
	    cg.setColor( QColorGroup::ButtonText, g.mid() );
	else if ( ADD_LINE_ACTIVE )
	    cg.setColor( QColorGroup::ButtonText, g.button() );
	int xo = VERTICAL ? 1 : 0;
	drawArrow( p, VERTICAL ? DownArrow : RightArrow, FALSE,
		   addB.x()+2+xo, addB.y()+2, addB.width()-4-xo, addB.height()-4,
		   cg, TRUE, &p->brush() );
    }
    if ( controls & SubLine ) {
	p->setBrush( SUB_LINE_ACTIVE ? g.foreground() : g.button() );
	p->drawRect( subB.x(), subB.y(), subB.width(), subB.height() );
	p->setPen( SUB_LINE_ACTIVE ? g.button() : g.foreground() );
	QColorGroup cg( g );
	if ( maxedOut )
	    cg.setColor( QColorGroup::ButtonText, g.mid() );
	else if ( SUB_LINE_ACTIVE )
	    cg.setColor( QColorGroup::ButtonText, g.button() );
	int xo = VERTICAL ? 1 : 0;
	drawArrow( p, VERTICAL ? UpArrow : LeftArrow, FALSE,
		   subB.x()+2+xo, subB.y()+2, subB.width()-4-xo, subB.height()-4,
		   cg, TRUE, &p->brush() );
    }


    p->setPen( g.foreground() );
    p->setBrush( g.brush( QColorGroup::Mid ) );
    if ( controls & SubPage )
	p->drawRect( subPageR.x(), subPageR.y(), subPageR.width(), subPageR.height() );
    if ( controls & AddPage && addPageR.y() < addPageR.bottom() )
	p->drawRect( addPageR.x(), addPageR.y(), addPageR.width(), addPageR.height() );
    if ( controls & Slider ) {
	QPoint bo = p->brushOrigin();
	p->setBrushOrigin(sliderR.topLeft());
	p->setPen( g.foreground() );
	p->setBrush( g.button() );
	p->drawRect( sliderR.x(), sliderR.y(), sliderR.width(), sliderR.height() );
	p->setBrushOrigin(bo);
	QColorGroup cg( g );
	if ( maxedOut )
	    cg.setColor( QColorGroup::ButtonText, g.mid() );
	drawRiffles( p, sliderR.x(), sliderR.y(),
		     sliderR.width(), sliderR.height(), cg, HORIZONTAL );
    }

    // ### perhaps this should not be able to accept focus if maxedOut?
    if ( sb->hasFocus() && (controls & Slider) )
	p->drawWinFocusRect( sliderR.x()+2, sliderR.y()+2,
			     sliderR.width()-5, sliderR.height()-5,
			     sb->backgroundColor() );

}

void FlatStyle::drawRiffles( QPainter* p,  int x, int y, int w, int h,
                      const QColorGroup &g, bool horizontal )
{
    if (!horizontal) {
	if (h > 20) {
	    y += (h-20)/2 ;
	    h = 20;
	}
	if (h > 12) {
	    int n = 3;
	    int my = y+h/2-2;
	    int i ;
	    p->setPen(g.buttonText());
	    for (i=0; i<n; i++) {
		p->drawLine(x+4, my+3*i, x+w-5, my+3*i);
	    }
	}
    } else {
	if (w > 20) {
	    x += (w-20)/2 ;
	    w = 20;
	}
	if (w > 12) {
	    int n = 3;
	    int mx = x+w/2-4;
	    int i ;
	    p->setPen(g.buttonText());
	    for (i=0; i<n; i++) {
		p->drawLine(mx+3*i, y+4, mx + 3*i, y+h-5);
	    }
	}
    }
}

int FlatStyle::sliderLength() const
{
    return 12;
}

void FlatStyle::drawSlider( QPainter *p, int x, int y, int w, int h,
	const QColorGroup &g, Orientation o, bool tickAbove, bool tickBelow )
{
    int a = tickAbove ? 3 : 0;
    int b = tickBelow ? 3 : 0;

    p->setPen( g.foreground() );
    p->setBrush( g.button() );
    if ( o == Horizontal ) {
	p->drawRect( x, y+a, w, h-a-b );
	int xp = x + w/2;
	p->drawLine( xp-1, y+a+3, xp-1, y+h-b-4 );
	p->drawLine( xp, y+a+3, xp, y+h-b-4 );
    } else {
	p->drawRect( x+a, y, w-a-b, h );
	int yp = y + h/2;
	p->drawLine( x+a+3, yp-1, x+w-b-4, yp-1 );
	p->drawLine( x+a+3, yp, x+w-b-4, yp );
    }
}

void FlatStyle::drawSliderMask ( QPainter * p, int x, int y, int w, int h,
	Orientation o, bool tickAbove, bool tickBelow )
{
    int a = tickAbove ? 3 : 0;
    int b = tickBelow ? 3 : 0;
    if ( o == Horizontal )
	p->fillRect( x, y+a, w, h-a-b, color1 );
    else
	p->fillRect( x+a, y, w-a-b, h, color1 );
}

/*!\reimp
 */
void FlatStyle::drawSliderGrooveMask( QPainter *p,
					int x, int y, int w, int h,
					const QColorGroup& , QCOORD c,
					Orientation orient )
{
    if ( orient == Horizontal )
	p->fillRect( x, y + c - 2,  w, 4, color1 );
    else
	p->fillRect( x + c - 2, y, 4, h, color1 );
}

void FlatStyle::drawSliderGroove( QPainter *p, int x, int y, int w, int h, const QColorGroup &g, QCOORD c, Orientation orient )
{
    if ( orient == Horizontal )
	p->fillRect( x, y + c - 2,  w, 4, g.foreground() );
    else
	p->fillRect( x + c - 2, y, 4, h, g.foreground() );
}

void FlatStyle::drawTab( QPainter *p, const QTabBar *tb, QTab *t, bool selected )
{
    QRect r( t->rect() );
    if ( tb->shape()  == QTabBar::RoundedAbove ) {
	p->setPen( tb->colorGroup().foreground() );
	p->drawLine( r.left(), r.bottom(), r.right(), r.bottom() );
	if ( r.left() == 0 )
	    p->drawPoint( tb->rect().bottomLeft() );
	else
	    p->drawLine( r.left(), r.bottom(), r.right(), r.bottom() );

	if ( selected ) {
	    p->setPen( tb->colorGroup().background() );
	    p->drawLine( r.left()+2, r.top()+1, r.right()-2, r.top()+1 );
	    p->fillRect( QRect( r.left()+1, r.top()+2, r.width()-2, r.height()-2),
			 tb->colorGroup().brush( QColorGroup::Background ));
	} else {
	    r.setRect( r.left() + 2, r.top() + 2,
		       r.width() - 4, r.height() - 2 );
	    p->setPen( tb->colorGroup().button() );
	    p->drawLine( r.left()+2, r.top()+1, r.right()-2, r.top()+1 );
	    p->fillRect( QRect( r.left()+1, r.top()+2, r.width()-2, r.height()-3),
			 tb->colorGroup().brush( QColorGroup::Button ));
	}

	p->setPen( tb->colorGroup().foreground() );
	p->drawLine( r.left(), r.bottom()-1, r.left(), r.top() + 2 );
	p->drawPoint( r.left()+1, r.top() + 1 );
	p->drawLine( r.left()+2, r.top(),
		     r.right() - 2, r.top() );

	p->drawPoint( r.right() - 1, r.top() + 1 );
	p->drawLine( r.right(), r.top() + 2, r.right(), r.bottom() - 1);
    } else if ( tb->shape() == QTabBar::RoundedBelow ) {
	if ( selected ) {
	    p->setPen( tb->colorGroup().background() );
	    p->drawLine( r.left()+2, r.bottom()-1, r.right()-2, r.bottom()-1 );
	    p->fillRect( QRect( r.left()+1, r.top(), r.width()-2, r.height()-2),
			 tb->palette().normal().brush( QColorGroup::Background ));
	} else {
	    p->setPen( tb->colorGroup().foreground() );
	    p->drawLine( r.left(), r.top(),
			 r.right(), r.top() );
	    r.setRect( r.left() + 2, r.top(),
		       r.width() - 4, r.height() - 2 );
	    p->setPen( tb->colorGroup().button() );
	    p->drawLine( r.left()+2, r.bottom()-1, r.right()-2, r.bottom()-1 );
	    p->fillRect( QRect( r.left()+1, r.top()+1, r.width()-2, r.height()-3),
			 tb->palette().normal().brush( QColorGroup::Button ));
	}

	p->setPen( tb->colorGroup().foreground() );
	p->drawLine( r.right(), r.top(),
		     r.right(), r.bottom() - 2 );
	p->drawPoint( r.right() - 1, r.bottom() - 1 );
	p->drawLine( r.right() - 2, r.bottom(),
		     r.left() + 2, r.bottom() );

	p->drawLine( r.left(), r.top()+1,
		     r.left(), r.bottom() - 2 );
	p->drawPoint( r.left() + 1, r.bottom() - 1 );
	if ( r.left() == 0 )
	    p->drawPoint( tb->rect().topLeft() );

    } else {
	QCommonStyle::drawTab( p, tb, t, selected );
    }
}

static const int motifItemFrame		= 0;	// menu item frame width
static const int motifSepHeight		= 2;	// separator item height
static const int motifItemHMargin	= 1;	// menu item hor text margin
static const int motifItemVMargin	= 2;	// menu item ver text margin
static const int motifArrowHMargin	= 0;	// arrow horizontal margin
static const int motifTabSpacing	= 12;	// space between text and tab
static const int motifCheckMarkHMargin	= 1;	// horiz. margins of check mark
static const int windowsRightBorder	= 8;    // right border on windows
static const int windowsCheckMarkWidth  = 2;    // checkmarks width on windows

/*! \reimp
*/
int FlatStyle::extraPopupMenuItemWidth( bool checkable, int maxpmw, QMenuItem* mi, const QFontMetrics& /*fm*/ )
{
#ifndef QT_NO_MENUDATA
    int w = 2*motifItemHMargin + 2*motifItemFrame; // a little bit of border can never harm

    if ( mi->isSeparator() )
	return 10; // arbitrary
    else if ( mi->pixmap() )
	w += mi->pixmap()->width();	// pixmap only

    if ( !mi->text().isNull() ) {
	if ( mi->text().find('\t') >= 0 )	// string contains tab
	    w += motifTabSpacing;
    }

    if ( maxpmw ) { // we have iconsets
	w += maxpmw;
	w += 6; // add a little extra border around the iconset
    }

    if ( checkable && maxpmw < windowsCheckMarkWidth ) {
	w += windowsCheckMarkWidth - maxpmw; // space for the checkmarks
    }

    if ( maxpmw > 0 || checkable ) // we have a check-column ( iconsets or checkmarks)
	w += motifCheckMarkHMargin; // add space to separate the columns

    w += windowsRightBorder; // windows has a strange wide border on the right side

    return w;
#endif
}

/*! \reimp
*/
int FlatStyle::popupMenuItemHeight( bool /*checkable*/, QMenuItem* mi, const QFontMetrics& fm )
{
#ifndef QT_NO_MENUDATA    
    int h = 0;
    if ( mi->isSeparator() )			// separator height
	h = motifSepHeight;
    else if ( mi->pixmap() )		// pixmap height
	h = mi->pixmap()->height() + 2*motifItemFrame;
    else					// text height
	h = fm.height() + 2*motifItemVMargin + 2*motifItemFrame - 1;

    if ( !mi->isSeparator() && mi->iconSet() != 0 ) {
	h = QMAX( h, mi->iconSet()->pixmap( QIconSet::Small, QIconSet::Normal ).height() + 2*motifItemFrame );
    }
    if ( mi->custom() )
	h = QMAX( h, mi->custom()->sizeHint().height() + 2*motifItemVMargin + 2*motifItemFrame ) - 1;
    return h;
#endif
}

void FlatStyle::drawPopupMenuItem( QPainter* p, bool checkable, int maxpmw, int tab, QMenuItem* mi,
				       const QPalette& pal,
				       bool act, bool enabled, int x, int y, int w, int h)
{
#ifndef QT_NO_MENUDATA    
    const QColorGroup & g = pal.active();
    bool dis	  = !enabled;
    QColorGroup itemg = dis ? pal.disabled() : pal.active();

    if ( checkable )
	maxpmw = QMAX( maxpmw, 8 ); // space for the checkmarks

    int checkcol	  =     maxpmw;

    if ( mi && mi->isSeparator() ) {			// draw separator
	p->setPen( g.dark() );
	p->drawLine( x, y, x+w, y );
	return;
    }

    QBrush fill = act? g.brush( QColorGroup::Highlight ) :
			    g.brush( QColorGroup::Button );
    p->fillRect( x, y, w, h, fill);

    if ( !mi )
	return;

    if ( mi->isChecked() ) {
	if ( act && !dis ) {
	    qDrawShadePanel( p, x, y, checkcol, h,
			     g, TRUE, 1, &g.brush( QColorGroup::Button ) );
	} else {
	    qDrawShadePanel( p, x, y, checkcol, h,
			     g, TRUE, 1, &g.brush( QColorGroup::Midlight ) );
	}
    } else if ( !act ) {
	p->fillRect(x, y, checkcol , h,
		    g.brush( QColorGroup::Button ));
    }

    if ( mi->iconSet() ) {		// draw iconset
	QIconSet::Mode mode = dis ? QIconSet::Disabled : QIconSet::Normal;
	if (act && !dis )
	    mode = QIconSet::Active;
	QPixmap pixmap = mi->iconSet()->pixmap( QIconSet::Small, mode );
	int pixw = pixmap.width();
	int pixh = pixmap.height();
	if ( act && !dis ) {
	    if ( !mi->isChecked() )
		qDrawShadePanel( p, x, y, checkcol, h, g, FALSE,  1, &g.brush( QColorGroup::Button ) );
	}
	QRect cr( x, y, checkcol, h );
	QRect pmr( 0, 0, pixw, pixh );
	pmr.moveCenter( cr.center() );
	p->setPen( itemg.text() );
	p->drawPixmap( pmr.topLeft(), pixmap );

	QBrush fill = act? g.brush( QColorGroup::Highlight ) :
			      g.brush( QColorGroup::Button );
	p->fillRect( x+checkcol + 1, y, w - checkcol - 1, h, fill);
    } else  if ( checkable ) {	// just "checking"...
	int mw = checkcol + motifItemFrame;
	int mh = h - 2*motifItemFrame;
	if ( mi->isChecked() ) {
	    drawCheckMark( p, x + motifItemFrame + 2,
			   y+motifItemFrame, mw, mh, itemg, act, dis );
	}
    }

    p->setPen( act ? g.highlightedText() : g.buttonText() );

    QColor discol;
    if ( dis ) {
	discol = itemg.text();
	p->setPen( discol );
    }

    int xm = motifItemFrame + checkcol + motifItemHMargin;

    if ( mi->custom() ) {
	int m = motifItemVMargin;
	p->save();
	if ( dis && !act ) {
	    p->setPen( g.light() );
	    mi->custom()->paint( p, itemg, act, enabled,
				 x+xm+1, y+m+1, w-xm-tab+1, h-2*m );
	    p->setPen( discol );
	}
	mi->custom()->paint( p, itemg, act, enabled,
			     x+xm, y+m, w-xm-tab+1, h-2*m );
	p->restore();
    }
    QString s = mi->text();
    if ( !s.isNull() ) {			// draw text
	int t = s.find( '\t' );
	int m = motifItemVMargin;
	const int text_flags = AlignVCenter|ShowPrefix | DontClip | SingleLine;
	if ( t >= 0 ) {				// draw tab text
	    if ( dis && !act ) {
		p->setPen( g.light() );
		p->drawText( x+w-tab-windowsRightBorder-motifItemHMargin-motifItemFrame+1,
			     y+m+1, tab, h-2*m, text_flags, s.mid( t+1 ));
		p->setPen( discol );
	    }
	    p->drawText( x+w-tab-windowsRightBorder-motifItemHMargin-motifItemFrame,
			 y+m, tab, h-2*m, text_flags, s.mid( t+1 ) );
	}
	if ( dis && !act )
	    p->setPen( discol );
	p->drawText( x+xm, y+m, w-xm-tab+1, h-2*m, text_flags, s, t );
    } else if ( mi->pixmap() ) {			// draw pixmap
	QPixmap *pixmap = mi->pixmap();
	if ( pixmap->depth() == 1 )
	    p->setBackgroundMode( OpaqueMode );
	p->drawPixmap( x+xm, y+motifItemFrame, *pixmap );
	if ( pixmap->depth() == 1 )
	    p->setBackgroundMode( TransparentMode );
    }
    if ( mi->popup() ) {			// draw sub menu arrow
	int dim = (h-2*motifItemFrame) / 2;
	if ( act ) {
	    if ( !dis )
		discol = white;
	    QColorGroup g2( discol, g.highlight(),
			    white, white,
			    dis ? discol : white,
			    discol, white );
	    drawArrow( p, RightArrow, FALSE,
			       x+w - motifArrowHMargin - motifItemFrame - dim,  y+h/2-dim/2,
			       dim, dim, g2, TRUE );
	} else {
	    drawArrow( p, RightArrow,
			       FALSE,
			       x+w - motifArrowHMargin - motifItemFrame - dim,  y+h/2-dim/2,
			       dim, dim, g, mi->isEnabled() );
	}
    }
#endif
}

void FlatStyle::getButtonShift( int &x, int &y )
{
    x = 0; y = 0;
}

//===========================================================================

FlatStyleImpl::FlatStyleImpl()
    : flat(0), ref(0)
{
}

FlatStyleImpl::~FlatStyleImpl()
{
    // We do not delete the style because Qt does that when a new style
    // is set.
}

QStyle *FlatStyleImpl::style()
{
    if ( !flat )
	flat = new FlatStyle();
    return flat;
}

QString FlatStyleImpl::name() const
{
    return QString("Flat");
}

QRESULT FlatStyleImpl::queryInterface( const QUuid &uuid, QUnknownInterface **iface )
{
    *iface = 0;
    if ( uuid == IID_QUnknown )
	*iface = this;
    else if ( uuid == IID_Style )
	*iface = this;

    if ( *iface )
	(*iface)->addRef();
    return QS_OK;
}

Q_EXPORT_INTERFACE()
{
    Q_CREATE_INSTANCE( FlatStyleImpl )
}

#include "flat.moc"
