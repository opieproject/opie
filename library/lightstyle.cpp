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
#include "lightstyle.h"

#if QT_VERSION < 300

#define INCLUDE_MENUITEM_DEF
#include "qmenubar.h"
#include "qapplication.h"
#include "qpainter.h"
#include "qpalette.h"
#include "qframe.h"
#include "qpushbutton.h"
#include "qdrawutil.h"
#include "qscrollbar.h"
#include "qtabbar.h"
#include "qguardedptr.h"
#include "qlayout.h"
#include "qlineedit.h"


class LightStylePrivate
{
public:
    LightStylePrivate()
	: hoverWidget(0), ref(1), savePalette(0)
    {
    }

    QGuardedPtr<QWidget> hoverWidget;
    QPalette oldPalette, hoverPalette;
    int ref;
    QPoint mousePos;
    QPalette *savePalette;
};


static LightStylePrivate *singleton = 0;


LightStyle::LightStyle()
    : QWindowsStyle()
{
    if (! singleton) {
	singleton = new LightStylePrivate;

	QPalette pal = QApplication::palette();
	singleton->oldPalette = pal;

	QColor bg = pal.color(QPalette::Active, QColorGroup::Background);
	QColor prelight;

	if ( (bg.red() + bg.green() + bg.blue()) / 3 > 128)
	    prelight = pal.color(QPalette::Active,
				 QColorGroup::Background).light(110);
	else
	    prelight = pal.color(QPalette::Active,
				 QColorGroup::Background).light(120);

	QColorGroup active2(pal.color(QPalette::Active,
				      QColorGroup::Foreground),      // foreground
			    prelight,                                // button
			    prelight.light(),                        // light
			    prelight.dark(),                         // dark
			    prelight.dark(120),                      // mid
			    pal.color(QPalette::Active,
				      QColorGroup::Text),            // text
			    pal.color(QPalette::Active,
				      QColorGroup::BrightText),      // bright text
			    pal.color(QPalette::Active,
				      QColorGroup::Base),            // base
			    bg);                                     // background
	active2.setColor(QColorGroup::Highlight,
			 pal.color(QPalette::Active, QColorGroup::Highlight));

	singleton->hoverPalette = pal;
	singleton->hoverPalette.setActive(active2);
 	singleton->hoverPalette.setInactive(active2);
    } else
	singleton->ref++;
}


LightStyle::~LightStyle()
{
    if (singleton && singleton->ref-- <= 0) {
	delete singleton;
	singleton = 0;
    }
}


QSize LightStyle::scrollBarExtent() const
{
    return QSize(12 + defaultFrameWidth(), 12 + defaultFrameWidth());
}


int LightStyle::buttonDefaultIndicatorWidth() const
{
    return 2;
}


int LightStyle::sliderThickness() const
{
    return 16;
}

int LightStyle::sliderLength() const
{
    return 13;
}


int LightStyle::buttonMargin() const
{
    return 4;
}


QSize LightStyle::exclusiveIndicatorSize() const
{
    return QSize(13, 13);
}


int LightStyle::defaultFrameWidth() const
{
    return 2;
}


QSize LightStyle::indicatorSize() const
{
    return QSize(13, 13);
}


void LightStyle::polish(QWidget *widget)
{
    if (widget->inherits("QPushButton"))
	widget->installEventFilter(this);

#if QT_VERSION >= 300
    if (widget->inherits("QLineEdit")) {
	QLineEdit *lineedit = (QLineEdit *) widget;
	lineedit->setFrameShape(QFrame::StyledPanel);
	lineedit->setLineWidth(2);
    }
#endif

    QWindowsStyle::polish(widget);
}


void LightStyle::unPolish(QWidget *widget)
{
    if (widget->inherits("QPushButton"))
	widget->removeEventFilter(this);

#if QT_VERSION >= 300
    if (widget->inherits("QLineEdit")) {
	QLineEdit *lineedit = (QLineEdit *) widget;
	lineedit->setLineWidth(1);
	lineedit->setFrameShape(QFrame::WinPanel);
    }
#endif

    QWindowsStyle::unPolish(widget);
}


void LightStyle::polish(QApplication *app)
{
    QPalette pal = app->palette();

    QColorGroup active(pal.color(QPalette::Active,
				 QColorGroup::Foreground),           // foreground
		       pal.color(QPalette::Active,
				 QColorGroup::Button),               // button
		       pal.color(QPalette::Active,
				 QColorGroup::Background).light(),   // light
		       pal.color(QPalette::Active,
				 QColorGroup::Background).dark(175), // dark
		       pal.color(QPalette::Active,
				 QColorGroup::Background).dark(110), // mid
		       pal.color(QPalette::Active,
				 QColorGroup::Text),                 // text
		       pal.color(QPalette::Active,
				 QColorGroup::BrightText),           // bright text
		       pal.color(QPalette::Active,
				 QColorGroup::Base),                 // base
		       pal.color(QPalette::Active,
				 QColorGroup::Background)),          // background


	disabled(pal.color(QPalette::Disabled,
			   QColorGroup::Foreground),                 // foreground
		 pal.color(QPalette::Disabled,
			   QColorGroup::Button),                     // button
		 pal.color(QPalette::Disabled,
			   QColorGroup::Background).light(),         // light
		 pal.color(QPalette::Disabled,
			   QColorGroup::Background).dark(),          // dark
		 pal.color(QPalette::Disabled,
			   QColorGroup::Background).dark(110),       // mid
		 pal.color(QPalette::Disabled,
			   QColorGroup::Text),                       // text
		 pal.color(QPalette::Disabled,
			   QColorGroup::BrightText),                 // bright text
		 pal.color(QPalette::Disabled,
			   QColorGroup::Base),                       // base
		 pal.color(QPalette::Disabled,
			   QColorGroup::Background));                // background

    active.setColor(QColorGroup::Highlight,
		    pal.color(QPalette::Active, QColorGroup::Highlight));
    disabled.setColor(QColorGroup::Highlight,
		      pal.color(QPalette::Disabled, QColorGroup::Highlight));

    active.setColor(QColorGroup::HighlightedText,
		    pal.color(QPalette::Active, QColorGroup::HighlightedText));
    disabled.setColor(QColorGroup::HighlightedText,
		      pal.color(QPalette::Disabled, QColorGroup::HighlightedText));

    pal.setActive(active);
    pal.setInactive(active);
    pal.setDisabled(disabled);

    singleton->oldPalette = pal;

    QColor bg = pal.color(QPalette::Active, QColorGroup::Background);
    QColor prelight;

    if ( (bg.red() + bg.green() + bg.blue()) / 3 > 128)
	prelight = pal.color(QPalette::Active,
			     QColorGroup::Background).light(110);
    else
	prelight = pal.color(QPalette::Active,
			     QColorGroup::Background).light(120);

    QColorGroup active2(pal.color(QPalette::Active,
				  QColorGroup::Foreground),      // foreground
			prelight,                                // button
			prelight.light(),                        // light
			prelight.dark(),                         // dark
			prelight.dark(120),                      // mid
			pal.color(QPalette::Active,
				  QColorGroup::Text),            // text
			pal.color(QPalette::Active,
				  QColorGroup::BrightText),      // bright text
			pal.color(QPalette::Active,
				  QColorGroup::Base),            // base
			bg);                                     // background
    active2.setColor(QColorGroup::Highlight,
		     pal.color(QPalette::Active, QColorGroup::Highlight));

    singleton->hoverPalette = pal;
    singleton->hoverPalette.setActive(active2);
    singleton->hoverPalette.setInactive(active2);

    app->setPalette(pal);
}


void LightStyle::unPolish(QApplication *app)
{
    app->setPalette(singleton->oldPalette);
}


void LightStyle::polishPopupMenu(QPopupMenu *menu)
{
    menu->setMouseTracking(TRUE);
}


void LightStyle::drawPushButton(QPushButton *button, QPainter *p)
{
    int x1, y1, x2, y2;
    button->rect().coords(&x1, &y1, &x2, &y2);

    if (button->isDefault()) {
	p->save();
	p->setPen(button->palette().active().color(QColorGroup::Highlight));
	p->setBrush(button->palette().active().brush(QColorGroup::Highlight));
	p->drawRoundRect(x1, y1, x2 - x1 + 1, y2 - y1 + 1, 15, 15);
	p->restore();
    }

    if (button->isDefault() || button->autoDefault()) {
        x1 += buttonDefaultIndicatorWidth();
        y1 += buttonDefaultIndicatorWidth();
        x2 -= buttonDefaultIndicatorWidth();
        y2 -= buttonDefaultIndicatorWidth();

	if (button->isDefault()) {
	    QPointArray pa(8);
	    pa.setPoint(0, x1 + 2, y1    );
	    pa.setPoint(1, x2 - 1, y1    );
	    pa.setPoint(2, x2 + 1, y1 + 2);
	    pa.setPoint(3, x2 + 1, y2 - 2);
	    pa.setPoint(4, x2 - 2, y2 + 1);
	    pa.setPoint(5, x1 + 2, y2 + 1);
	    pa.setPoint(6, x1,     y2 - 1);
	    pa.setPoint(7, x1,     y1 + 2);
	    QRegion r(pa);
	    p->setClipRegion(r);
	}
    }

    QBrush fill;
    if (button->isDown() || button->isOn())
        fill = button->colorGroup().brush(QColorGroup::Mid);
    else
        fill = button->colorGroup().brush(QColorGroup::Button);

    if ( !button->isFlat() || button->isOn() || button->isDown() )
        drawButton(p, x1, y1, x2 - x1 + 1, y2 - y1 + 1,
                   button->colorGroup(), button->isOn() || button->isDown(), &fill);
}


void LightStyle::drawButton(QPainter *p, int x, int y, int w, int h,
                                 const QColorGroup &g,
                                 bool sunken, const QBrush *fill)
{
    p->save();
    if ( fill )
	p->fillRect(x + 2, y + 2, w - 4, h - 4, *fill);
    else
	p->fillRect(x + 2, y + 2, w - 4, h - 4,
		    QBrush(sunken ? g.mid() : g.button()));

    // frame
    p->setPen(g.dark());
    p->drawLine(x, y + 2, x, y + h - 3); // left
    p->drawLine(x + 2, y, x + w - 3, y); // top
    p->drawLine(x + w - 1, y + 2, x + w - 1, y + h - 3); // right
    p->drawLine(x + 2, y + h - 1, x + w - 3, y + h - 1); // bottom
    p->drawPoint(x + 1, y + 1);
    p->drawPoint(x + 1, y + h - 2);
    p->drawPoint(x + w - 2, y + 1);
    p->drawPoint(x + w - 2, y + h - 2);

    // bevel
    if (sunken)
	p->setPen(g.mid());
    else
	p->setPen(g.light());

    p->drawLine(x + 1, y + 2, x + 1, y + h - 3); // left
    p->drawLine(x + 2, y + 1, x + w - 3, y + 1); // top

    if (sunken)
	p->setPen(g.light());
    else
	p->setPen(g.mid());

    p->drawLine(x + w - 2, y + 2, x + w - 2, y + h - 3); // right + 1
    p->drawLine(x + 2, y + h - 2, x + w - 3, y + h - 2); // bottom + 1

    p->restore();
}


void LightStyle::drawBevelButton(QPainter *p, int x, int y, int w, int h,
                                      const QColorGroup &g,
                                      bool sunken, const QBrush *fill)
{
    drawButton(p, x, y, w, h, g, sunken, fill);
}


void LightStyle::getButtonShift(int &x, int &y) const
{
    x = y = 0;
}


void LightStyle::drawComboButton(QPainter *p, int x, int y, int w, int h,
			       const QColorGroup &g, bool,
			       bool editable, bool,
			       const QBrush *fill)
{
    drawButton(p, x, y, w, h, g, FALSE, fill);

    if (editable) {
        QRect r = comboButtonRect(x, y, w, h);
        qDrawShadePanel(p, r.x() - 1, r.y() - 1,
			r.width() + defaultFrameWidth(),
			r.height() + defaultFrameWidth(),
			g, TRUE);
    }

    int indent = ((y + h) / 2) - 3;
    int xpos = x;

#if QT_VERSION >= 300
    if( QApplication::reverseLayout() )
        xpos += indent;
    else
#endif
        xpos += w - indent - 5;

    drawArrow(p, Qt::DownArrow, TRUE, xpos, indent, 5, 5, g, TRUE, fill);
}


QRect LightStyle::comboButtonRect( int x, int y, int w, int h ) const
{
    QRect r(x + 3, y + 3, w - 6, h - 6);
    int indent = ((y + h) / 2) - 3;
    r.setRight(r.right() - indent - 10);

#if QT_VERSION >= 300
    if( QApplication::reverseLayout() )
        r.moveBy( indent + 10, 0 );
#endif

    return r;
}


QRect LightStyle::comboButtonFocusRect(int x, int y, int w, int h ) const
{
    return comboButtonRect(x, y, w, h);
}


void LightStyle::drawPanel(QPainter *p, int x, int y, int w, int h,
			 const QColorGroup &g, bool sunken,
			 int lw, const QBrush *fill)
{
    if (lw >= 2) {
	if ( fill )
	    p->fillRect(x + 2, y + 2, w - 4, h - 4, *fill);

	QPen oldpen = p->pen();

	// frame
	p->setPen(g.dark());
	p->drawLine(x, y + 2, x, y + h - 3); // left
	p->drawLine(x + 2, y, x + w - 3, y); // top
	p->drawLine(x + w - 1, y + 2, x + w - 1, y + h - 3); // right
	p->drawLine(x + 2, y + h - 1, x + w - 3, y + h - 1); // bottom
	p->drawPoint(x + 1, y + 1);
	p->drawPoint(x + 1, y + h - 2);
	p->drawPoint(x + w - 2, y + 1);
	p->drawPoint(x + w - 2, y + h - 2);

	// bevel
	if (sunken)
	    p->setPen(g.mid());
	else
	    p->setPen(g.light());

	p->drawLine(x + 1, y + 2, x + 1, y + h - 3); // left
	p->drawLine(x + 2, y + 1, x + w - 3, y + 1); // top

	if (sunken)
	    p->setPen(g.light());
	else
	    p->setPen(g.mid());

	p->drawLine(x + w - 2, y + 2, x + w - 2, y + h - 3); // right + 1
	p->drawLine(x + 2, y + h - 2, x + w - 3, y + h - 2); // bottom + 1

	// corners
	p->setPen(g.background());
	p->drawLine(x, y, x + 1, y);
	p->drawLine(x, y + h - 1, x + 1, y + h - 1);
	p->drawLine(x + w - 2, y, x + w - 1, y);
	p->drawLine(x + w - 2, y + h - 1, x + w - 1, y + h - 1);
	p->drawPoint(x, y + 1);
	p->drawPoint(x, y + h - 2);
	p->drawPoint(x + w - 1, y + 1);
	p->drawPoint(x + w - 1, y + h - 2);

	p->setPen(oldpen);
    } else
	qDrawShadePanel(p, x, y, w, h, g, sunken, lw, fill);
}


void LightStyle::drawIndicator(QPainter *p, int x, int y ,int w, int h,
                               const QColorGroup &g, int state,
                               bool down, bool)
{
    drawButton(p, x, y, w, h, g, TRUE,
	       &g.brush(down ? QColorGroup::Mid : QColorGroup::Base));

    p->save();

    p->setPen(g.foreground());
    if (state == QButton::NoChange) {
	p->drawLine(x + 3, y + h / 2, x + w - 4, y + h / 2);
	p->drawLine(x + 3, y + 1 + h / 2, x + w - 4, y + 1 + h / 2);
	p->drawLine(x + 3, y - 1 + h / 2, x + w - 4, y - 1 + h / 2);
    } else if (state == QButton::On) {
	p->drawLine(x + 4, y + 3, x + w - 4, y + h - 5);
	p->drawLine(x + 3, y + 3, x + w - 4, y + h - 4);
	p->drawLine(x + 3, y + 4, x + w - 5, y + h - 4);
	p->drawLine(x + 3, y + h - 5, x + w - 5, y + 3);
	p->drawLine(x + 3, y + h - 4, x + w - 4, y + 3);
	p->drawLine(x + 4, y + h - 4, x + w - 4, y + 4);
    }

    p->restore();
}


void LightStyle::drawExclusiveIndicator(QPainter *p, int x, int y, int w, int h,
					const QColorGroup &g, bool on,
					bool down, bool)
{
    p->save();

    p->fillRect(x, y, w, h, g.brush(QColorGroup::Background));

    p->setPen(g.dark());
    p->drawArc(x, y, w, h, 0, 16*360);
    p->setPen(g.mid());
    p->drawArc(x + 1, y + 1, w - 2, h - 2, 45*16, 180*16);
    p->setPen(g.light());
    p->drawArc(x + 1, y + 1, w - 2, h - 2, 235*16, 180*16);

    p->setPen(down ? g.mid() : g.base());
    p->setBrush(down ? g.mid() : g.base());
    p->drawEllipse(x + 2, y + 2, w - 4, h - 4);

    if (on) {
	p->setBrush(g.foreground());
	p->drawEllipse(x + 3, y + 3, w - x - 6, h - y - 6);
    }

    p->restore();
}



#if 1
//copied from QPE style
void LightStyle::drawTab( QPainter *p, const QTabBar *tb, QTab *t, bool selected )
{
#if 0
    //We can't do this, because QTabBar::focusInEvent redraws the
    // tab label with the default font.
	QFont f = tb->font();
	f.setBold( selected );
	p->setFont( f );
#endif    
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

#else

void LightStyle::drawTab(QPainter *p, const QTabBar *tabbar, QTab *tab,
                              bool selected)
{
    p->save();

    QColorGroup g = tabbar->colorGroup();
    QRect fr(tab->r);
    fr.setLeft(fr.left() + 2);

    if (! selected) {
        if (tabbar->shape() == QTabBar::RoundedAbove ||
            tabbar->shape() == QTabBar::TriangularAbove) {

            fr.setTop(fr.top() + 2);
        } else {
            fr.setBottom(fr.bottom() - 2);
        }
    }

    QRegion tabr(tab->r);

    QPointArray cliptri(4);
    cliptri.setPoint(0, fr.left(), fr.top());
    cliptri.setPoint(1, fr.left(), fr.top() + 5);
    cliptri.setPoint(2, fr.left() + 5, fr.top());
    cliptri.setPoint(3, fr.left(), fr.top());
    QRegion trir(cliptri);
    p->setClipRegion(tabr - trir);

    p->setPen( NoPen ); 
    p->setBrush(g.brush(selected ? QColorGroup::Background : QColorGroup::Mid));

    fr.setWidth(fr.width() - 1);
    p->drawRect(fr.left() + 1, fr.top() + 1, fr.width() - 2, fr.height() - 2);

    if (tabbar->shape() == QTabBar::RoundedAbove) {
        // "rounded" tabs on top
        fr.setBottom(fr.bottom() - 1);

        p->setPen(g.dark());
	p->drawLine(fr.left(), fr.top() + 5, fr.left(), fr.bottom() - 1);
	p->drawLine(fr.left(), fr.top() + 5, fr.left() + 5, fr.top());
	p->drawLine(fr.left() + 5, fr.top(), fr.right() - 1, fr.top());
	p->drawLine(fr.right(), fr.top() + 1, fr.right(), fr.bottom() - 1);

	if (selected) {
	    p->drawLine(fr.right(), fr.bottom(), fr.right() + 2, fr.bottom());
	    p->drawPoint(fr.left(), fr.bottom());
	} else
	    p->drawLine(fr.left(), fr.bottom(), fr.right() + 2, fr.bottom());

	if (fr.left() == 2) {
	    p->drawPoint(fr.left() - 1, fr.bottom() + 1);
	    p->drawPoint(fr.left() - 2, fr.bottom() + 2);
	}

	if (selected) {
	    p->setPen(g.mid());
	    p->drawLine(fr.right() - 1, fr.top() + 1, fr.right() - 1, fr.bottom() - 2);
	}

	p->setPen(g.light()); p->setPen(red);
	p->drawLine(fr.left() + 1, fr.top() + 6, fr.left() + 1,
		    fr.bottom() - (selected ? 0 : 1));
	p->drawLine(fr.left() + 1, fr.top() + 5, fr.left() + 5, fr.top() + 1);
	p->drawLine(fr.left() + 6, fr.top() + 1, fr.right() - 3, fr.top() + 1);
	if (selected) {
	    p->drawLine(fr.right() + 1, fr.bottom() + 1,
			fr.right() + 2, fr.bottom() + 1);
	    p->drawLine(fr.left(), fr.bottom() + 1, fr.left() + 1, fr.bottom() + 1);
	} else
	    p->drawLine(fr.left(), fr.bottom() + 1,
			fr.right() + 2, fr.bottom() + 1);
    } else if (tabbar->shape() == QTabBar::RoundedBelow) {
        // "rounded" tabs on bottom
        fr.setTop(fr.top() + 1);

        p->setPen(g.dark());
	p->drawLine(fr.left(), fr.top(), fr.left(), fr.bottom() - 1);
	p->drawLine(fr.left() + 1, fr.bottom(), fr.right() - 1, fr.bottom());
	p->drawLine(fr.right(), fr.top(), fr.right(), fr.bottom() - 1);

	if (! selected)
	    p->drawLine(fr.left(), fr.top(), fr.right() + 3, fr.top());
	else
	    p->drawLine(fr.right(), fr.top(), fr.right() + 3, fr.top());

	p->setPen(g.mid());
	if (selected)
	    p->drawLine(fr.right() - 1, fr.top() + 1, fr.right() - 1, fr.bottom() - 1);
	else
	    p->drawLine(fr.left(), fr.top() - 1, fr.right() + 3, fr.top() - 1);

	p->setPen(g.light());
	p->drawLine(fr.left() + 1, fr.top() + (selected ? -1 : 2),
		    fr.left() + 1, fr.bottom() - 1);

    } else {
        // triangular drawing code
        QCommonStyle::drawTab(p, tabbar, tab, selected);
    }

    p->restore();
}
#endif

void LightStyle::drawSlider(QPainter *p, int x, int y, int w, int h,
			  const QColorGroup &g, Qt::Orientation orientation,
			  bool above, bool below)
{
    drawButton(p, x, y, w, h, g, FALSE, &g.brush(QColorGroup::Button));

    if (orientation == Horizontal) {
	if (above && below) {
	    drawArrow(p, Qt::UpArrow, FALSE, x + 1, y + 1, w, h / 2, g, TRUE);
	    drawArrow(p, Qt::DownArrow, FALSE, x + 1, y + (h / 2) - 1,
		      w, h / 2, g, TRUE);
	} else
	    drawArrow(p, (above) ? Qt::UpArrow : Qt::DownArrow,
		      FALSE, x + 1, y, w, h, g, TRUE);
    } else {
	if (above && below) {
	    drawArrow(p, Qt::LeftArrow, FALSE, x + 1, y, w / 2, h, g, TRUE);
	    drawArrow(p, Qt::RightArrow, FALSE, x + (w / 2) - 2, y, w / 2, h, g, TRUE);
	} else
	    drawArrow(p, (above) ? Qt::LeftArrow : Qt::RightArrow,
		      FALSE, x, y, w, h, g, TRUE);
    }
}


void LightStyle::drawSliderGroove(QPainter *p, int x, int y, int w, int h,
				const QColorGroup& g, QCOORD c,
				Qt::Orientation orientation)
{
    if (orientation == Horizontal)
	drawButton(p, x, y+c - 3, w, 6, g, TRUE, &g.brush(QColorGroup::Mid));
    else
	drawButton(p, x+c - 3, y, 6, h, g, TRUE, &g.brush(QColorGroup::Mid));
}


void LightStyle::scrollBarMetrics(const QScrollBar *scrollbar,
                                       int &sliderMin, int &sliderMax,
                                       int &sliderLength, int &buttonDim) const
{
    int maxLength;
    int length = ((scrollbar->orientation() == Horizontal) ?
		  scrollbar->width() : scrollbar->height());
    int extent = ((scrollbar->orientation() == Horizontal) ?
		  scrollbar->height() : scrollbar->width());
    extent--;

    if (length > (extent + defaultFrameWidth() - 1) * 2 + defaultFrameWidth())
	buttonDim = extent - defaultFrameWidth();
    else
	buttonDim = (length - defaultFrameWidth()) / 2 - 1;

    sliderMin = buttonDim;
    maxLength = length - buttonDim * 3;

    if (scrollbar->maxValue() != scrollbar->minValue()) {
	uint range = scrollbar->maxValue() - scrollbar->minValue();
	sliderLength = (scrollbar->pageStep() * maxLength) /
		       (range + scrollbar->pageStep());

	if (sliderLength < buttonDim || range > INT_MAX / 2)
	    sliderLength = buttonDim;
	if (sliderLength > maxLength)
	    sliderLength = maxLength;
    } else
	sliderLength = maxLength;

    sliderMax = sliderMin + maxLength - sliderLength;
}


QStyle::ScrollControl LightStyle::scrollBarPointOver(const QScrollBar *scrollbar,
						   int sliderStart, const QPoint &p)
{
    if (! scrollbar->rect().contains(p))
	return NoScroll;

    int sliderMin, sliderMax, sliderLength, buttonDim, pos;
    scrollBarMetrics( scrollbar, sliderMin, sliderMax, sliderLength, buttonDim );

    if (scrollbar->orientation() == Horizontal)
	pos = p.x();
    else
	pos = p.y();

    if (pos < buttonDim)
	return SubLine;
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



void LightStyle::drawScrollBarControls( QPainter* p, const QScrollBar* scrollbar,
                                             int sliderStart, uint controls,
                                             uint activeControl )
{
    QColorGroup g  = scrollbar->colorGroup();

    int sliderMin, sliderMax, sliderLength, buttonDim;
    scrollBarMetrics( scrollbar, sliderMin, sliderMax, sliderLength, buttonDim );

    if (sliderStart > sliderMax) { // sanity check
        sliderStart = sliderMax;
    }

    QRect addR, subR, subR2, addPageR, subPageR, sliderR;
    int length =  ((scrollbar->orientation() == Horizontal) ?
		   scrollbar->width()  : scrollbar->height());
    int extent =  ((scrollbar->orientation() == Horizontal) ?
		   scrollbar->height() : scrollbar->width());

    
    int fudge = 3; //####disgusting hack
    
    if (scrollbar->orientation() == Horizontal) {
	subR.setRect(0, defaultFrameWidth(),
		     buttonDim + fudge, buttonDim);
	subR2.setRect(length - (buttonDim * 2), defaultFrameWidth() ,
		      buttonDim, buttonDim);
	addR.setRect(length - buttonDim, defaultFrameWidth(),
		     buttonDim, buttonDim);
    } else {
	subR.setRect(defaultFrameWidth() + 1, 0,
		     buttonDim, buttonDim + fudge);
	subR2.setRect(defaultFrameWidth() + 1, length - (buttonDim * 2),
		      buttonDim, buttonDim);
	addR.setRect(defaultFrameWidth() + 1, length - buttonDim,
		     buttonDim, buttonDim);
    }

    int sliderEnd = sliderStart + sliderLength;
    int sliderW = extent - defaultFrameWidth() - 1;
    if (scrollbar->orientation() == Horizontal) {
        subPageR.setRect( subR.right() + 1, defaultFrameWidth(),
                          sliderStart - subR.right() - 1 , sliderW );
        addPageR.setRect( sliderEnd, defaultFrameWidth(),
			  subR2.left() - sliderEnd, sliderW );
        sliderR.setRect( sliderStart, defaultFrameWidth(), sliderLength, sliderW );
    } else {
        subPageR.setRect( defaultFrameWidth(), subR.bottom() + 1,
			  sliderW, sliderStart - subR.bottom() - 1 );
        addPageR.setRect( defaultFrameWidth(), sliderEnd,
			  sliderW, subR2.top() - sliderEnd );
        sliderR .setRect( defaultFrameWidth(), sliderStart,
			  sliderW, sliderLength );
    }

    if ( controls == ( AddLine | SubLine | AddPage | SubPage |
                       Slider | First | Last ) ) {
	if (scrollbar->orientation() == Horizontal)
	    qDrawShadePanel(p, 0, 0, length, 2, g, TRUE, 1,
			    &g.brush(QColorGroup::Background));
	else
	    qDrawShadePanel(p, 0, 0, 2, length, g, TRUE, 1,
			    &g.brush(QColorGroup::Background));
    }

    if ( controls & AddLine )
        drawArrow( p, (scrollbar->orientation() == Vertical) ? DownArrow : RightArrow,
		   FALSE, addR.x(), addR.y(),
                   addR.width(), addR.height(),
		   (( activeControl == AddLine ) ?
		    singleton->hoverPalette.active() : g),
		   TRUE, &g.brush(QColorGroup::Background));
    if ( controls & SubLine ) {
        drawArrow( p, (scrollbar->orientation() == Vertical) ? UpArrow : LeftArrow,
		   FALSE, subR.x(), subR.y(),
                   subR.width(), subR.height(),
                   (( activeControl == SubLine ) ?
		    singleton->hoverPalette.active() : g),
		   TRUE, &g.brush(QColorGroup::Background));
        drawArrow( p, (scrollbar->orientation() == Vertical) ? UpArrow : LeftArrow,
		   FALSE, subR2.x(), subR2.y(),
                   subR2.width(), subR2.height(),
                   (( activeControl == SubLine ) ?
		    singleton->hoverPalette.active() : g),
		   TRUE, &g.brush(QColorGroup::Background));
    }

    if ( controls & SubPage )
        p->fillRect( subPageR,
		     ((activeControl == SubPage) ?
		      g.brush( QColorGroup::Dark ) :
		      g.brush( QColorGroup::Mid )));
    if ( controls & AddPage )
        p->fillRect( addPageR,
		     ((activeControl == AddPage) ?
    		      g.brush( QColorGroup::Dark ) :
		      g.brush( QColorGroup::Mid )));

    if ( controls & Slider ) {

        QPoint bo = p->brushOrigin();
        p->setBrushOrigin(sliderR.topLeft());
        if ( sliderR.isValid() ) {
	    p->fillRect( sliderR.x(), sliderR.y(), 2, 2,
		      g.brush( QColorGroup::Mid ));
	    p->fillRect( sliderR.x() + sliderR.width() - 2, 
		    sliderR.y(), 2, 2,
		      g.brush( QColorGroup::Mid ));
	    p->fillRect( sliderR.x() + sliderR.width() - 2, 
		    sliderR.y() + sliderR.height() - 2, 2, 2,
		      g.brush( QColorGroup::Mid ));
	    p->fillRect( sliderR.x(), 
		    sliderR.y() + sliderR.height() - 2, 2, 2,
		      g.brush( QColorGroup::Mid ));

	    QColorGroup cg( g );
	    cg.setBrush( QColorGroup::Background, g.brush( QColorGroup::Mid ) );
            drawBevelButton( p, sliderR.x(), sliderR.y(),
                             sliderR.width(), sliderR.height(),
			     cg, FALSE, &g.brush( QColorGroup::Button ) );
        }

        p->setBrushOrigin(bo);
    }
}


void LightStyle::drawToolBarHandle(QPainter *p, const QRect &rect,
				   Qt::Orientation orientation,
				   bool, const QColorGroup &g, bool)
{
    p->save();
    p->setPen(g.mid());
    p->setBrush(g.brush(QColorGroup::Mid));

    if (orientation == Qt::Horizontal) {
	QRect l, r;
	l.setRect(rect.x() + 1, rect.y() + 1, rect.width() - 5, rect.height() - 2);
	r.setRect(l.right() + 1, l.y(), 3, l.height());

	p->drawRect(l);
       	qDrawShadePanel(p, r, g, FALSE);
    } else {
	QRect t, b;
	t.setRect(rect.x() + 1, rect.y() + 1, rect.width() - 2, rect.height() - 5);
	b.setRect(t.x(), t.bottom() + 1, t.width(), 3);

	p->drawRect(t);
       	qDrawShadePanel(p, b, g, FALSE);
    }

    p->restore();
}


bool LightStyle::eventFilter(QObject *object, QEvent *event)
{
    switch(event->type()) {
    case QEvent::Enter:
        {
            if (! object->isWidgetType() ||
		! object->inherits("QPushButton"))
		break;

	    singleton->hoverWidget = (QWidget *) object;
	    if (! singleton->hoverWidget->isEnabled()) {
		singleton->hoverWidget = 0;
		break;
	    }

	    QPalette pal = singleton->hoverWidget->palette();
	    if (singleton->hoverWidget->ownPalette())
		singleton->savePalette = new QPalette(pal);

	    singleton->hoverWidget->setPalette(singleton->hoverPalette);

	    break;
	}

    case QEvent::Leave:
	{
	    if (object != singleton->hoverWidget)
		break;

	    if (singleton->savePalette) {
		singleton->hoverWidget->setPalette(*(singleton->savePalette));
		delete singleton->savePalette;
		singleton->savePalette = 0;
	    } else
		singleton->hoverWidget->unsetPalette();

	    singleton->hoverWidget = 0;

	    break;
	}

    default:
	{
	    ;
	}
    }

    return QWindowsStyle::eventFilter(object, event);
}


static const int motifItemFrame		= 1;	// menu item frame width
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
int LightStyle::extraPopupMenuItemWidth( bool checkable, int maxpmw, QMenuItem* mi, const QFontMetrics& /*fm*/ )
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
int LightStyle::popupMenuItemHeight( bool /*checkable*/, QMenuItem* mi, const QFontMetrics& fm )
{
#ifndef QT_NO_MENUDATA    
    int h = 0;
    if ( mi->isSeparator() )			// separator height
	h = motifSepHeight;
    else if ( mi->pixmap() )		// pixmap height
	h = mi->pixmap()->height() + 2*motifItemFrame;
    else					// text height
	h = fm.height() + 2*motifItemVMargin + 2*motifItemFrame;

    if ( !mi->isSeparator() && mi->iconSet() != 0 ) {
	h = QMAX( h, mi->iconSet()->pixmap( QIconSet::Small, QIconSet::Normal ).height() + 2*motifItemFrame );
    }
    if ( mi->custom() )
	h = QMAX( h, mi->custom()->sizeHint().height() + 2*motifItemVMargin + 2*motifItemFrame ) - 1;
    return h;
#endif
}

void LightStyle::drawPopupMenuItem( QPainter* p, bool checkable, int maxpmw, int tab, QMenuItem* mi,
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
