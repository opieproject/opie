/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
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

#include "qpestyle.h"
#include <qpe/qpeapplication.h>
#include <qpushbutton.h>

#include <qpainter.h>

#define QCOORDARRLEN(x) sizeof(x)/(sizeof(QCOORD)*2)

#if QT_VERSION >= 300

#include <qdrawutil.h>
#include <qcombobox.h>
#include <qtabbar.h>

QPEStyle::QPEStyle()
{
}

QPEStyle::~QPEStyle()
{
}

void QPEStyle::drawPrimitive( PrimitiveElement pe, QPainter *p, const QRect &r,
			      const QColorGroup &cg, SFlags flags, const QStyleOption &data) const
{
    switch ( pe ) {
    case PE_ButtonTool:
	{
	    QColorGroup mycg = cg;
	    if ( flags & Style_On ) {
		QBrush fill( cg.mid(), Dense4Pattern );
		mycg.setBrush( QColorGroup::Button, fill );
	    }
	    drawPrimitive( PE_ButtonBevel, p, r, mycg, flags, data );
	    break;
	}
    case PE_ButtonCommand:
    case PE_ButtonDefault:
    case PE_ButtonBevel:
    case PE_HeaderSection:
	{
	    QPen oldPen = p->pen();
	    p->fillRect( r.x()+1, r.y()+1, r.width()-2, r.height()-2, cg.brush(QColorGroup::Button) );

	    int x2 = r.right();
	    int y2 = r.bottom();

	    if ( flags & (Style_Sunken | Style_Down | Style_On) )
		p->setPen( cg.dark() );
	    else
		p->setPen( cg.light() );
	    p->drawLine( r.x(), r.y()+1, r.x(), y2-1 );
	    p->drawLine( r.x()+1, r.y(), x2-1, r.y() );

	    if ( flags & (Style_Sunken | Style_Down | Style_On) )
		p->setPen( cg.light() );
	    else
		p->setPen( cg.dark() );
	    p->drawLine( x2, r.y()+1, x2, y2-1 );
	    p->drawLine( r.x()+1, y2, x2-1, y2 );
	    p->setPen( oldPen );
	    break;
	}
    case PE_FocusRect:
	break;
    case PE_Indicator:
	{
	    QColorGroup mycg( cg );
	    QBrush fill;
	    if ( flags & Style_Down )
		fill = cg.brush( QColorGroup::Button );
	    else
		fill = cg.brush( (flags&Style_Enabled) ? QColorGroup::Base : QColorGroup::Background );
	    mycg.setBrush( QColorGroup::Button, fill );
	    if ( flags&Style_Enabled )
		flags |= Style_Sunken;
	    drawPrimitive( PE_ButtonBevel, p, r, mycg, flags );
	    if ( flags & Style_On ) {
		QPointArray a( 7*2 );
		int i, xx, yy;
		xx = r.x()+3;
		yy = r.y()+5;
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
		if ( flags & Style_NoChange ) {
		    p->setPen( mycg.dark() );
		} else {
		    p->setPen( mycg.text() );
		}
		p->drawLineSegments( a );
	    }
	    break;
	}
    case PE_ExclusiveIndicator:
	{
	    static const QCOORD pts1[] = {              // dark lines
		1,9, 1,8, 0,7, 0,4, 1,3, 1,2, 2,1, 3,1, 4,0, 7,0, 8,1, 9,1 };
	    static const QCOORD pts4[] = {              // white lines
		2,10, 3,10, 4,11, 7,11, 8,10, 9,10, 10,9, 10,8, 11,7,
		11,4, 10,3, 10,2 };
	    static const QCOORD pts5[] = {              // inner fill
		4,2, 7,2, 9,4, 9,7, 7,9, 4,9, 2,7, 2,4 };

	    int x, y, w, h;
	    r.rect( &x, &y, &w, &h );
	    p->eraseRect( x, y, w, h );
	    QPointArray a( QCOORDARRLEN(pts1), pts1 );
	    a.translate( x, y );
	    p->setPen( cg.dark() );
	    p->drawPolyline( a );
	    a.setPoints( QCOORDARRLEN(pts4), pts4 );
	    a.translate( x, y );
	    p->setPen( cg.light() );
	    p->drawPolyline( a );
	    a.setPoints( QCOORDARRLEN(pts5), pts5 );
	    a.translate( x, y );
	    QColor fillColor = ( flags&Style_Down || !(flags&Style_Enabled) ) ? cg.button() : cg.base();
	    p->setPen( fillColor );
	    p->setBrush( fillColor  ) ;
	    p->drawPolygon( a );
	    if ( flags&Style_On ) {
		p->setPen( NoPen );
		p->setBrush( cg.text() );
		p->drawRect( x+5, y+4, 2, 4 );
		p->drawRect( x+4, y+5, 4, 2 );
	    }
	    break;
	}
    default:
	QWindowsStyle::drawPrimitive( pe, p, r, cg, flags, data );
	break;
    }
}

void QPEStyle::drawControl( ControlElement ce, QPainter *p,
			    const QWidget *widget, const QRect &r,
			    const QColorGroup &cg, SFlags how, const QStyleOption &data) const
{
    switch ( ce ) {
    case CE_PushButton:
	{
	    const QPushButton *btn = (QPushButton*)widget;
	    SFlags flags;
	    flags = Style_Default;
	    if ( btn->isDown() )
		flags |= Style_Down;
	    if ( btn->isOn() )
		flags |= Style_On;
	    if ( btn->isEnabled() )
		flags |= Style_Enabled;
	    if ( btn->isDefault() )
		flags |= Style_Default;
	    if (! btn->isFlat() && !(flags & Style_Down))
		flags |= Style_Raised;
	    p->setPen( cg.foreground() );
	    p->setBrush( QBrush(cg.button(), NoBrush) );
	    QColorGroup mycg( cg );
	    if ( flags & Style_On ) {
		QBrush fill = QBrush( cg.mid(), Dense4Pattern );
		mycg.setBrush( QColorGroup::Button, fill );
	    }
	    drawPrimitive( PE_ButtonBevel, p, r, mycg, flags, data );
	    break;
	}
    case CE_TabBarTab:
	{
	    if ( !widget || !widget->parentWidget() )
		break;

	    const QTabBar *tb = (const QTabBar *) widget;
	    bool selected = how & Style_Selected;

	    QRect r2(r);
	    if ( tb->shape()  == QTabBar::RoundedAbove ) {
		p->setPen( cg.light() );
		p->drawLine( r2.left(), r2.bottom(), r2.right(), r2.bottom() );
		if ( r2.left() == 0 )
		    p->drawPoint( tb->rect().bottomLeft() );
		else {
		    p->setPen( cg.light() );
		    p->drawLine( r2.left(), r2.bottom(), r2.right(), r2.bottom() );
		}

		if ( selected ) {
		    p->setPen( cg.background() );
		    p->drawLine( r2.left()+2, r2.top()+1, r2.right()-2, r2.top()+1 );
		    p->fillRect( QRect( r2.left()+1, r2.top()+2, r2.width()-2, r2.height()-2),
			    cg.brush( QColorGroup::Background ));
		} else {
		    r2.setRect( r2.left() + 2, r2.top() + 2,
			    r2.width() - 4, r2.height() - 2 );
		    p->setPen( cg.button() );
		    p->drawLine( r2.left()+2, r2.top()+1, r2.right()-2, r2.top()+1 );
		    p->fillRect( QRect( r2.left()+1, r2.top()+2, r2.width()-2, r2.height()-3),
			    cg.brush( QColorGroup::Button ));

		    //do shading; will not work for pixmap brushes
		    QColor bg = cg.button();
		    //	    int h,s,v;
		    //	    bg.hsv( &h, &s, &v );
		    int n = r2.height()/2;
		    int dark = 100;
		    for ( int i = 1; i < n; i++ ) {
			dark = (dark * (100+(i*15)/n) )/100;
			p->setPen( bg.dark( dark ) );
			int y = r2.bottom()-n+i;
			int x1 = r2.left()+1;
			int x2 = r2.right()-1;
			p->drawLine( x1, y, x2, y );
		    }
		}

		p->setPen( cg.light() );
		p->drawLine( r2.left(), r2.bottom()-1, r2.left(), r2.top() + 2 );
		p->drawPoint( r2.left()+1, r2.top() + 1 );
		p->drawLine( r2.left()+2, r2.top(),
			r2.right() - 2, r2.top() );

		p->setPen( cg.dark() );
		p->drawPoint( r2.right() - 1, r2.top() + 1 );
		p->drawLine( r2.right(), r2.top() + 2, r2.right(), r2.bottom() - 1);
	    } else if ( tb->shape() == QTabBar::RoundedBelow ) {
		if ( selected ) {
		    p->setPen( cg.background() );
		    p->drawLine( r2.left()+2, r2.bottom()-1, r2.right()-2, r2.bottom()-1 );
		    p->fillRect( QRect( r2.left()+1, r2.top(), r2.width()-2, r2.height()-2),
			    tb->palette().normal().brush( QColorGroup::Background ));
		} else {
		    p->setPen( cg.dark() );
		    p->drawLine( r2.left(), r2.top(),
			    r2.right(), r2.top() );
		    r2.setRect( r2.left() + 2, r2.top(),
			    r2.width() - 4, r2.height() - 2 );
		    p->setPen( cg.button() );
		    p->drawLine( r2.left()+2, r2.bottom()-1, r2.right()-2, r2.bottom()-1 );
		    p->fillRect( QRect( r2.left()+1, r2.top()+1, r2.width()-2, r2.height()-3),
			    tb->palette().normal().brush( QColorGroup::Button ));
		}

		p->setPen( cg.dark() );
		p->drawLine( r2.right(), r2.top(),
			r2.right(), r2.bottom() - 2 );
		p->drawPoint( r2.right() - 1, r2.bottom() - 1 );
		p->drawLine( r2.right() - 2, r2.bottom(),
			r2.left() + 2, r2.bottom() );

		p->setPen( cg.light() );
		p->drawLine( r2.left(), r2.top()+1,
			r2.left(), r2.bottom() - 2 );
		p->drawPoint( r2.left() + 1, r2.bottom() - 1 );
		if ( r2.left() == 0 )
		    p->drawPoint( tb->rect().topLeft() );

	    } else {
		QCommonStyle::drawControl( ce, p, widget, r, cg, how, data );
	    }
	    break;
	}
    default:
	QWindowsStyle::drawControl( ce, p, widget, r, cg, how, data );
	break;
    }
}

void QPEStyle::drawComplexControl( ComplexControl control, QPainter *p,
				   const QWidget *widget, const QRect &r,
				   const QColorGroup &cg, SFlags how,
				   SCFlags sub, SCFlags subActive, const QStyleOption &data) const
{
    switch ( control ) {
    case CC_ComboBox:
	if ( sub & SC_ComboBoxArrow ) {
	    SFlags flags = Style_Default;

	    drawPrimitive( PE_ButtonBevel, p, r, cg, flags, data );

	    QRect ar =
		QStyle::visualRect( querySubControlMetrics( CC_ComboBox, widget,
							    SC_ComboBoxArrow ), widget );

	    if ( subActive == SC_ComboBoxArrow ) {
		p->setPen( cg.dark() );
		p->setBrush( cg.brush( QColorGroup::Button ) );
		p->drawRect( ar );
	    }

	    ar.addCoords( 2, 2, -2, -2 );
	    if ( widget->isEnabled() )
		flags |= Style_Enabled;

	    if ( subActive & Style_Sunken ) {
		flags |= Style_Sunken;
	    }
	    drawPrimitive( PE_ArrowDown, p, ar, cg, flags );
	}

	if ( sub & SC_ComboBoxEditField ) {
	    const QComboBox * cb = (const QComboBox *) widget;
	    QRect re =
		QStyle::visualRect( querySubControlMetrics( CC_ComboBox, widget,
							    SC_ComboBoxEditField ), widget );
	    if ( cb->hasFocus() && !cb->editable() )
		p->fillRect( re.x(), re.y(), re.width(), re.height(),
			     cg.brush( QColorGroup::Highlight ) );

	    if ( cb->hasFocus() ) {
		p->setPen( cg.highlightedText() );
		p->setBackgroundColor( cg.highlight() );

	    } else {
		p->setPen( cg.text() );
		p->setBackgroundColor( cg.background() );
	    }

	    if ( cb->hasFocus() && !cb->editable() ) {
		QRect re =
		    QStyle::visualRect( subRect( SR_ComboBoxFocusRect, cb ), widget );
		drawPrimitive( PE_FocusRect, p, re, cg, Style_FocusAtBorder, QStyleOption(cg.highlight()));
	    }
	}
	break;
    default:
	QWindowsStyle::drawComplexControl( control, p, widget, r, cg, how,
					   sub, subActive, data );
	break;
    }
}

int QPEStyle::pixelMetric( PixelMetric metric, const QWidget *widget ) const
{
    int ret;
    switch( metric ) {
	case PM_ButtonMargin:
	    ret = 2;
	    break;
	case PM_DefaultFrameWidth:
	    ret = 1;
	    break;
	case PM_ButtonDefaultIndicator:
	    ret = 2;
	    break;
	case PM_ButtonShiftHorizontal:
	case PM_ButtonShiftVertical:
	    ret = -1;
	    break;
	case PM_IndicatorWidth:
	    ret = 15;
	    break;
	case PM_IndicatorHeight:
	    ret = 13;
	    break;
	case PM_ExclusiveIndicatorHeight:
	case PM_ExclusiveIndicatorWidth:
	    ret = 15;
	    break;
	case PM_ScrollBarExtent:
	    ret = 13;
	    break;
	case PM_SliderLength:
	    ret = 12;
	    break;
	default:
	    ret = QWindowsStyle::pixelMetric( metric, widget );
	    break;
    }
    return ret;
}

QSize QPEStyle::sizeFromContents( ContentsType contents, const QWidget *widget,
				const QSize &contentsSize, const QStyleOption &data) const
{
    QSize sz(contentsSize);

    switch ( contents ) {
    case CT_PopupMenuItem:
	{
	    if ( !widget || data.isDefault() )
		break;
	    sz = QWindowsStyle::sizeFromContents( contents, widget, contentsSize, data );
	    sz = QSize( sz.width(), sz.height()-2 );
	    break;
	}
    default:
	sz = QWindowsStyle::sizeFromContents( contents, widget, contentsSize, data );
	break;
    }

    return sz;
}

#else

#include <qfontmetrics.h>
#include <qpalette.h>
#include <qdrawutil.h>
#include <qscrollbar.h>
#include <qbutton.h>
#include <qframe.h>
#include <qtabbar.h>

#define INCLUDE_MENUITEM_DEF
#include <qmenudata.h>

QPEStyle::QPEStyle()
{
#if QT_VERSION < 300
    setButtonMargin(buttonMargin());
    setScrollBarExtent(scrollBarExtent().width(),scrollBarExtent().height());
#endif
}

QPEStyle::~QPEStyle()
{
}

int QPEStyle::buttonMargin() const
{
    return 2;
}

QSize QPEStyle::scrollBarExtent() const
{
    return QSize(13,13);
}

void QPEStyle::polish ( QPalette & )
{
}

void QPEStyle::polish( QWidget *w )
{
    if ( w->inherits( "QListBox" ) ||
	 w->inherits( "QListView" ) ||
	 w->inherits( "QPopupMenu" ) ||
	 w->inherits( "QSpinBox" ) ) {
	QFrame *f = (QFrame *)w;
	f->setFrameShape( QFrame::StyledPanel );
	f->setLineWidth( 1 );
    }
}

void QPEStyle::unPolish( QWidget *w )
{
    if ( w->inherits( "QListBox" ) ||
	 w->inherits( "QListView" ) ||
	 w->inherits( "QPopupMenu" ) ||
	 w->inherits( "QSpinBox" ) ) {
	QFrame *f = (QFrame *)w;
	f->setFrameShape( QFrame::StyledPanel );
	f->setLineWidth( 2 );
    }
}

int QPEStyle::defaultFrameWidth() const
{
    return 1;
}

void QPEStyle::drawPanel ( QPainter * p, int x, int y, int w, int h,
			    const QColorGroup &g, bool sunken, int lineWidth, const QBrush * fill )
{
    qDrawShadePanel( p, QRect(x, y, w, h), g, sunken, lineWidth, fill );
}

void QPEStyle::drawButton( QPainter *p, int x, int y, int w, int h,
                                const QColorGroup &g, bool sunken, const QBrush* fill )
{
    QPen oldPen = p->pen();
    if ( sunken )
	p->setPen( g.dark() );
    else
	p->setPen( g.light() );

    int x2 = x+w-1;
    int y2 = y+h-1;

    p->drawLine( x, y, x, y2 );
    p->drawLine( x, y, x2, y );

    if ( sunken )
	p->setPen( g.light() );
    else
	p->setPen( g.dark() );

    p->drawLine( x2, y, x2, y2 );
    p->drawLine( x, y2, x2, y2 );
    p->setPen( oldPen );

    p->fillRect( x+1, y+1, w-2, h-2, fill?(*fill):g.brush(QColorGroup::Button) );
}

void QPEStyle::drawButtonMask ( QPainter * p, int x, int y, int w, int h )
{
    p->fillRect( x, y, w, h, color1 );
}

void QPEStyle::drawBevelButton( QPainter *p, int x, int y, int w, int h,
                                const QColorGroup &g, bool sunken, const QBrush* fill )
{
    drawButton( p, x, y, w, h, g, sunken, fill );
}

QRect QPEStyle::comboButtonRect( int x, int y, int w, int h)
{
    return QRect(x+1, y+1, w-2-14, h-2);
}


QRect QPEStyle::comboButtonFocusRect( int x, int y, int w, int h)
{
    return QRect(x+2, y+2, w-4-14, h-4);
}

void QPEStyle::drawComboButton( QPainter *p, int x, int y, int w, int h,
				     const QColorGroup &g, bool sunken,
				     bool /*editable*/,
				     bool enabled,
				     const QBrush *fill )
{
    drawBevelButton( p, x, y, w, h, g, FALSE, fill );
    drawBevelButton( p, x+w-14, y, 14, h, g, sunken, fill );
    drawArrow( p, QStyle::DownArrow, sunken,
	       x+w-14+ 2, y+ 2, 14- 4, h- 4, g, enabled,
	       &g.brush( QColorGroup::Button ) );

}


void QPEStyle::drawExclusiveIndicator ( QPainter * p, int x, int y, int w,
	int h, const QColorGroup & g, bool on, bool down, bool enabled )
{
    static const QCOORD pts1[] = {              // dark lines
        1,9, 1,8, 0,7, 0,4, 1,3, 1,2, 2,1, 3,1, 4,0, 7,0, 8,1, 9,1 };
    static const QCOORD pts4[] = {              // white lines
	2,10, 3,10, 4,11, 7,11, 8,10, 9,10, 10,9, 10,8, 11,7,
	11,4, 10,3, 10,2 };
    static const QCOORD pts5[] = {              // inner fill
	4,2, 7,2, 9,4, 9,7, 7,9, 4,9, 2,7, 2,4 };

    p->eraseRect( x, y, w, h );
    QPointArray a( QCOORDARRLEN(pts1), pts1 );
    a.translate( x, y );
    p->setPen( g.dark() );
    p->drawPolyline( a );
    a.setPoints( QCOORDARRLEN(pts4), pts4 );
    a.translate( x, y );
    p->setPen( g.light() );
    p->drawPolyline( a );
    a.setPoints( QCOORDARRLEN(pts5), pts5 );
    a.translate( x, y );
    QColor fillColor = ( down || !enabled ) ? g.button() : g.base();
    p->setPen( fillColor );
    p->setBrush( fillColor  ) ;
    p->drawPolygon( a );
    if ( on ) {
	p->setPen( NoPen );
	p->setBrush( g.text() );
	p->drawRect( x+5, y+4, 2, 4 );
	p->drawRect( x+4, y+5, 4, 2 );
    }
}

void QPEStyle::drawIndicator ( QPainter * p, int x, int y, int w, int h,
	const QColorGroup & g, int state, bool down, bool enabled )
{

    QBrush fill;
    if ( state == QButton::NoChange ) {
	QBrush b = p->brush();
	QColor c = p->backgroundColor();
	p->setBackgroundMode( TransparentMode );
	p->setBackgroundColor( green );
	fill = QBrush(g.base(), Dense4Pattern);
	p->setBackgroundColor( c );
	p->setBrush( b );
    } else if ( down )
	fill = g.brush( QColorGroup::Button );
    else
	fill = g.brush( enabled ? QColorGroup::Base : QColorGroup::Background );
    drawPanel( p, x, y, w, h, g, TRUE, 1, &fill );
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
	    p->setPen( g.dark() );
	} else {
	    p->setPen( g.text() );
	}
	p->drawLineSegments( a );
    }
}

#define HORIZONTAL	(sb->orientation() == QScrollBar::Horizontal)
#define VERTICAL	!HORIZONTAL
#define MOTIF_BORDER	2
#define SLIDER_MIN	9 // ### motif says 6 but that's too small

/*! \reimp */

void QPEStyle::scrollBarMetrics( const QScrollBar* sb, int &sliderMin, int &sliderMax, int &sliderLength, int& buttonDim )
{
    int maxLength;
    int length = HORIZONTAL ? sb->width()  : sb->height();
    int extent = HORIZONTAL ? sb->height() : sb->width();

    if ( length > (extent - 1)*2 )
	buttonDim = extent;
    else
	buttonDim = length/2 - 1;

    sliderMin = 0;
    maxLength  = length - buttonDim*2;

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
QStyle::ScrollControl QPEStyle::scrollBarPointOver( const QScrollBar* sb, int sliderStart, const QPoint& p )
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

void QPEStyle::drawScrollBarControls( QPainter* p, const QScrollBar* sb, int sliderStart, uint controls, uint activeControl )
{
#define ADD_LINE_ACTIVE ( activeControl == AddLine )
#define SUB_LINE_ACTIVE ( activeControl == SubLine )
    QColorGroup g  = sb->colorGroup();

    int sliderMin, sliderMax, sliderLength, buttonDim;
    scrollBarMetrics( sb, sliderMin, sliderMax, sliderLength, buttonDim );

    if ( controls == (AddLine | SubLine | AddPage | SubPage | Slider | First | Last ) )
	p->fillRect( 0, 0, sb->width(), sb->height(), g.brush( QColorGroup::Mid ));

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
	subX = length - dimB - dimB;
	addX = length - dimB;
    } else {
	subX = addX = ( extent - dimB ) / 2;
	subY = length - dimB - dimB;
	addY = length - dimB;
    }

    int sliderEnd = sliderStart + sliderLength;
    int sliderW = extent;
    if ( HORIZONTAL ) {
	subB.setRect( subX,subY+1,dimB,dimB-1 );
	addB.setRect( addX,addY+1,dimB,dimB-1 );

	subPageR.setRect( 0, 0,
			  sliderStart+1, sliderW );
	addPageR.setRect( sliderEnd-1, 0, subX - sliderEnd+1, sliderW );
	sliderR .setRect( sliderStart, 1, sliderLength, sliderW-1 );

    } else {
	subB.setRect( subX+1,subY,dimB-1,dimB );
	addB.setRect( addX+1,addY,dimB-1,dimB );

	subPageR.setRect( 0, 0, sliderW,
			  sliderStart+1 );
	addPageR.setRect( 0, sliderEnd-1, sliderW, subY - sliderEnd+1 );
	sliderR .setRect( 1, sliderStart, sliderW-1, sliderLength );
    }

    bool maxedOut = (sb->maxValue() == sb->minValue());
    if ( controls & AddLine ) {
	drawBevelButton( p, addB.x(), addB.y(),
			 addB.width(), addB.height(), g,
			 ADD_LINE_ACTIVE);
	p->setPen(g.shadow());
	drawArrow( p, VERTICAL ? DownArrow : RightArrow,
		   FALSE, addB.x()+2, addB.y()+2,
		   addB.width()-4, addB.height()-4, g, !maxedOut,
				     &g.brush( QColorGroup::Button ));
    }
    if ( controls & SubLine ) {
	drawBevelButton( p, subB.x(), subB.y(),
			 subB.width(), subB.height(), g,
			 SUB_LINE_ACTIVE );
	p->setPen(g.shadow());
	drawArrow( p, VERTICAL ? UpArrow : LeftArrow,
		    FALSE, subB.x()+2, subB.y()+2,
		   subB.width()-4, subB.height()-4, g, !maxedOut,
				     &g.brush( QColorGroup::Button ));
    }


    if ( controls & SubPage )
	p->fillRect( subPageR.x(), subPageR.y(), subPageR.width(),
		    subPageR.height(), g.brush( QColorGroup::Mid ));
    if ( controls & AddPage )
	p->fillRect( addPageR.x(), addPageR.y(), addPageR.width(),
		     addPageR.height(), g.brush( QColorGroup::Mid ));
    if ( controls & Slider ) {
	QPoint bo = p->brushOrigin();
	p->setBrushOrigin(sliderR.topLeft());
	drawBevelButton( p, sliderR.x(), sliderR.y(),
			 sliderR.width(), sliderR.height(), g,
			 FALSE, &g.brush( QColorGroup::Button ) );
	p->setBrushOrigin(bo);
	drawRiffles( p, sliderR.x(), sliderR.y(),
		     sliderR.width(), sliderR.height(), g, HORIZONTAL );
    }

    // ### perhaps this should not be able to accept focus if maxedOut?
    if ( sb->hasFocus() && (controls & Slider) )
	p->drawWinFocusRect( sliderR.x()+2, sliderR.y()+2,
			     sliderR.width()-5, sliderR.height()-5,
			     sb->backgroundColor() );

}

void QPEStyle::drawRiffles( QPainter* p,  int x, int y, int w, int h,
                      const QColorGroup &g, bool horizontal )
{
    if (!horizontal) {
	if (h > 20) {
	    y += (h-20)/2 ;
	    h = 20;
	}
	if (h > 12) {
	    int n = 3;
	    int my = y+h/2-4;
	    int i ;
	    p->setPen(g.light());
	    for (i=0; i<n; i++) {
		p->drawLine(x+2, my+3*i, x+w-4, my+3*i);
	    }
	    p->setPen(g.dark());
	    my++;
	    for (i=0; i<n; i++) {
		p->drawLine(x+2, my+3*i, x+w-4, my+3*i);
	    }
	}
    }
    else {
	if (w > 20) {
	    x += (w-20)/2 ;
	    w = 20;
	}
	if (w > 12) {
	    int n = 3;
	    int mx = x+w/2-4;
	    int i ;
	    p->setPen(g.light());
	    for (i=0; i<n; i++) {
		p->drawLine(mx+3*i, y+2, mx + 3*i, y+h-4);
	    }
	    p->setPen(g.dark());
	    mx++;
	    for (i=0; i<n; i++) {
		p->drawLine(mx+3*i, y+2, mx + 3*i, y+h-4);
	    }
	}
    }
}

int QPEStyle::sliderLength() const
{
    return 12;
}

void QPEStyle::drawSlider( QPainter *p, int x, int y, int w, int h,
	const QColorGroup &g, Orientation o, bool tickAbove, bool tickBelow )
{
    int a = tickAbove ? 3 : 0;
    int b = tickBelow ? 3 : 0;

    if ( o == Horizontal ) {
	drawBevelButton( p, x, y+a, w, h-a-b, g, FALSE, &g.brush( QColorGroup::Button ) );
	int xp = x + w/2;
	qDrawShadeLine( p, xp, y+a+2, xp, y+h-b-3, g );
    } else {
	drawBevelButton( p, x+a, y, w-a-b, h, g, FALSE, &g.brush( QColorGroup::Button ) );
	int yp = y + h/2;
	qDrawShadeLine( p, x+a+2, yp, x+w-b-3, yp, g );
    }
}

void QPEStyle::drawSliderMask ( QPainter * p, int x, int y, int w, int h,
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
void QPEStyle::drawSliderGrooveMask( QPainter *p,
					int x, int y, int w, int h,
					const QColorGroup& , QCOORD c,
					Orientation orient )
{
    if ( orient == Horizontal )
	p->fillRect( x, y + c - 2,  w, 4, color1 );
    else
	p->fillRect( x + c - 2, y, 4, h, color1 );
}

void QPEStyle::drawTab( QPainter *p, const QTabBar *tb, QTab *t, bool selected )
{
    QRect r( t->rect() );
    if ( tb->shape()  == QTabBar::RoundedAbove ) {
	p->setPen( tb->colorGroup().light() );
	p->drawLine( r.left(), r.bottom(), r.right(), r.bottom() );
	if ( r.left() == 0 )
	    p->drawPoint( tb->rect().bottomLeft() );
	else {
	    p->setPen( tb->colorGroup().light() );
	    p->drawLine( r.left(), r.bottom(), r.right(), r.bottom() );
	}

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

	    //do shading; will not work for pixmap brushes
	    QColor bg = tb->colorGroup().button();
	    //	    int h,s,v;
	    //	    bg.hsv( &h, &s, &v );
	    int n = r.height()/2;
	    int dark = 100;
	    for ( int i = 1; i < n; i++ ) {
		dark = (dark * (100+(i*15)/n) )/100;
		p->setPen( bg.dark( dark ) );
		int y = r.bottom()-n+i;
		int x1 = r.left()+1;
		int x2 = r.right()-1;
		p->drawLine( x1, y, x2, y );
	    }
	}

	p->setPen( tb->colorGroup().light() );
	p->drawLine( r.left(), r.bottom()-1, r.left(), r.top() + 2 );
	p->drawPoint( r.left()+1, r.top() + 1 );
	p->drawLine( r.left()+2, r.top(),
		     r.right() - 2, r.top() );

	p->setPen( tb->colorGroup().dark() );
	p->drawPoint( r.right() - 1, r.top() + 1 );
	p->drawLine( r.right(), r.top() + 2, r.right(), r.bottom() - 1);
    } else if ( tb->shape() == QTabBar::RoundedBelow ) {
	if ( selected ) {
	    p->setPen( tb->colorGroup().background() );
	    p->drawLine( r.left()+2, r.bottom()-1, r.right()-2, r.bottom()-1 );
	    p->fillRect( QRect( r.left()+1, r.top(), r.width()-2, r.height()-2),
			 tb->palette().normal().brush( QColorGroup::Background ));
	} else {
	    p->setPen( tb->colorGroup().dark() );
	    p->drawLine( r.left(), r.top(),
			 r.right(), r.top() );
	    r.setRect( r.left() + 2, r.top(),
		       r.width() - 4, r.height() - 2 );
	    p->setPen( tb->colorGroup().button() );
	    p->drawLine( r.left()+2, r.bottom()-1, r.right()-2, r.bottom()-1 );
	    p->fillRect( QRect( r.left()+1, r.top()+1, r.width()-2, r.height()-3),
			 tb->palette().normal().brush( QColorGroup::Button ));
	}

	p->setPen( tb->colorGroup().dark() );
	p->drawLine( r.right(), r.top(),
		     r.right(), r.bottom() - 2 );
	p->drawPoint( r.right() - 1, r.bottom() - 1 );
	p->drawLine( r.right() - 2, r.bottom(),
		     r.left() + 2, r.bottom() );

	p->setPen( tb->colorGroup().light() );
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
int QPEStyle::extraPopupMenuItemWidth( bool checkable, int maxpmw, QMenuItem* mi, const QFontMetrics& /*fm*/ )
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
int QPEStyle::popupMenuItemHeight( bool /*checkable*/, QMenuItem* mi, const QFontMetrics& fm )
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

void QPEStyle::drawPopupMenuItem( QPainter* p, bool checkable, int maxpmw, int tab, QMenuItem* mi,
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
	p->setPen( g.light() );
	p->drawLine( x, y+1, x+w, y+1 );
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
	if ( dis && !act ) {
	    p->setPen( g.light() );
	    p->drawText( x+xm+1, y+m+1, w-xm+1, h-2*m, text_flags, s, t );
	    p->setPen( discol );
	}
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

#endif
