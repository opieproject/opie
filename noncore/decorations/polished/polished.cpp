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

#include "polished.h"
#include <qpainter.h>
#include <qapplication.h>

PolishedDecoration::PolishedDecoration() : ref(0)
{
}

PolishedDecoration::~PolishedDecoration()
{
}

int PolishedDecoration::metric( Metric m, const WindowData *wd ) const
{
    switch ( m ) {
	case TopBorder:
	    return 1;
	    break;
	case LeftBorder:
	case RightBorder:
	    return 2;
	case BottomBorder:
	    return 6;
	case TitleHeight:
	    if ( QApplication::desktop()->height() > 320 )
		return 20;
	    else
		return 18;
	default:
	    return WindowDecorationInterface::metric( m, wd );
	    break;
    }

    return 0;
}

void PolishedDecoration::drawArea( Area a, QPainter *p, const WindowData *wd ) const
{
    int th = metric( TitleHeight, wd );
    QRect r = wd->rect;

    switch ( a ) {
	case Border:
	    {
		const QColorGroup &cg = wd->palette.active();
		QColor c;
		if ( wd->flags & WindowData::Active ) {
		    c = cg.color(QColorGroup::Highlight);
		} else {
		    c = cg.color(QColorGroup::Background);
		}
		drawBlend( p, QRect(r.x(),r.bottom()+1,r.width(),metric(BottomBorder,wd)), c.dark(180), c.light() );
		int lb = metric(LeftBorder,wd);
		int rb = metric(RightBorder,wd);
		int tb = metric(TopBorder,wd);
		int bb = metric(BottomBorder,wd);
		p->fillRect( r.x()-lb, r.y()-th-tb, lb,
			     r.height()+th+tb+bb, c.dark(180) );
		p->fillRect( r.right()+1, r.y()-th-tb, rb,
			     r.height()+th+tb+bb, c.dark(180) );
		p->fillRect( r.left(), r.y()-th-tb, r.width(),
			     tb, c.dark(180) );
	    }
	    break;
	case Title:
	    if ( r.height() < 2 ) {
		WindowDecorationInterface::drawArea( a, p, wd );
	    } else {
		const QColorGroup &cg = wd->palette.active();
		QColor c1, c2;
		if ( wd->flags & WindowData::Active ) {
		    c1 = cg.color(QColorGroup::Highlight).light();
		    c2 = cg.color(QColorGroup::Highlight).dark(180);
		} else {
		    c1 = cg.color(QColorGroup::Background);
		    c2 = cg.color(QColorGroup::Background).dark(180);
		}
		drawBlend( p, QRect(r.x(),r.y()-th,r.width(),th), c2, c1 );
	    }
	    break;
	case TitleText:
	    p->drawText( r.left()+5+metric(HelpWidth,wd), r.top()-th, r.width(), th,
			 Qt::AlignVCenter, wd->caption );
	    break;
	default:
	    PolishedDecoration::drawArea( a, p, wd );
	    break;
    }
}

void PolishedDecoration::drawButton( Button b, QPainter *p, const WindowData *wd, int x, int y, int w, int h, QWSButton::State state ) const
{
    WindowDecorationInterface::drawButton( b, p, wd, x, y, w, h, state );
}

QRegion PolishedDecoration::mask( const WindowData *wd ) const
{
    return WindowDecorationInterface::mask( wd );
}

QString PolishedDecoration::name() const
{
    return qApp->translate( "Decoration", "Polished" );
}

QPixmap PolishedDecoration::icon() const
{
    return QPixmap();
}

void PolishedDecoration::drawBlend( QPainter *p, const QRect &r, const QColor &c1, const QColor &c2 ) const
{
    int h1, h2, s1, s2, v1, v2;
    c1.hsv( &h1, &s1, &v1 );
    c2.hsv( &h2, &s2, &v2 );

    int split = r.height()/3;

    for ( int j = 0; j < split; j++ ) {
	p->setPen( QColor( h1 + ((h2-h1)*(j+split))/(2*split-1),
		    s1 + ((s2-s1)*(j+split))/(2*split-1),
		    v1 + ((v2-v1)*(j+split))/(2*split-1), QColor::Hsv ) );
	p->drawLine( r.x(), r.y()+j, r.right(), r.y()+j );
    }

    for ( int j = 0; j < r.height()-split; j++ ) {
	p->setPen( QColor( h1 + ((h2-h1)*j)/(r.height()-split-1),
		    s1 + ((s2-s1)*j)/(r.height()-split-1),
		    v1 + ((v2-v1)*j)/(r.height()-split-1), QColor::Hsv ) );
	p->drawLine( r.x(), r.bottom()-j, r.right(), r.bottom()-j );
    }
}

QRESULT PolishedDecoration::queryInterface( const QUuid &uuid, QUnknownInterface **iface )
{
    *iface = 0;
    if ( uuid == IID_QUnknown )
	*iface = this;
    else if ( uuid == IID_WindowDecoration )
	*iface = this;

    if ( *iface )
	(*iface)->addRef();
    return QS_OK;
}

Q_EXPORT_INTERFACE()
{
    Q_CREATE_INSTANCE( PolishedDecoration )
}

