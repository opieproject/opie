//////////////////////////////////////////////////////////////////////////////
// phasestyle.h
// -------------------
// A style for KDE
// -------------------
// Copyright (c) 2004 David Johnson <david@usermode.org>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//////////////////////////////////////////////////////////////////////////////

#include "phasestyle.h"

#include "bitmaps.h"

#define INCLUDE_MENUITEM_DEF
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qpainter.h>
#include <qbrush.h>
#include <qiconset.h>
#include <qtabbar.h>
#include <qscrollbar.h>


/* Spacing and sizeHint */
static unsigned contrast = 110;

static const int ITEMFRAME       = 1; // menu stuff
static const int ITEMHMARGIN     = 3;
static const int ITEMVMARGIN     = 0;

static const int ARROWMARGIN     = 6;
static const int RIGHTBORDER     = 10;
static const int MINICONSIZE     = 12;
static const int CHECKSIZE       = 9;
static const int SCROLLBAR_EXTENT = 12;


/*
 * Copyright (C) 1999 Daniel M. Duley <mosfet@kde.org>
 * LGPLv2 from kdelibs/kdefx/kdrawutil.cpp
 */
void kColorBitmaps(QPainter *p, const QColorGroup &g, int x, int y,
                   QBitmap *lightColor, QBitmap *midColor,
                   QBitmap *midlightColor, QBitmap *darkColor,
                   QBitmap *blackColor, QBitmap *whiteColor)
{
    QBitmap *bitmaps[]={lightColor, midColor, midlightColor, darkColor,
        blackColor, whiteColor};

    QColor colors[]={g.light(), g.mid(), g.midlight(), g.dark(),
        Qt::black, Qt::white};

    int i;
    for(i=0; i < 6; ++i){
        if(bitmaps[i]){
            if(!bitmaps[i]->mask())
                bitmaps[i]->setMask(*bitmaps[i]);
            p->setPen(colors[i]);
            p->drawPixmap(x, y, *bitmaps[i]);
        }
    }
}


/*
 * drawMenuBarItem is not virtual
 * this way we define our draw handler
 */
typedef void (QStyle::*QDrawMenuBarItemImpl) (QPainter *, int, int, int, int, QMenuItem *,
                          QColorGroup &, bool, bool);

extern QDrawMenuBarItemImpl qt_set_draw_menu_bar_impl(QDrawMenuBarItemImpl impl);


PhaseStyle::PhaseStyle()
    : QWindowsStyle()
{
    setName( "PhaseStyle" );

    /* small default sizes */
    setButtonDefaultIndicatorWidth(1);
    setScrollBarExtent(SCROLLBAR_EXTENT, SCROLLBAR_EXTENT);
    setSliderThickness(12);
    setButtonMargin( 1 );

    // create bitmaps
    uarrow = QBitmap(6, 6, uarrow_bits, true);
    uarrow.setMask(uarrow);
    darrow = QBitmap(6, 6, darrow_bits, true);
    darrow.setMask(darrow);
    larrow = QBitmap(6, 6, larrow_bits, true);
    larrow.setMask(larrow);
    rarrow = QBitmap(6, 6, rarrow_bits, true);
    rarrow.setMask(rarrow);
    bplus = QBitmap(6, 6, bplus_bits, true);
    bplus.setMask(bplus);
    bminus = QBitmap(6, 6, bminus_bits, true);
    bminus.setMask(bminus);
    bcheck = QBitmap(9, 9, bcheck_bits, true);
    bcheck.setMask(bcheck);
    dexpand = QBitmap(9, 9, dexpand_bits, true);
    dexpand.setMask(dexpand);
    rexpand = QBitmap(9, 9, rexpand_bits, true);
    rexpand.setMask(rexpand);
    doodad_mid = QBitmap(4, 4, doodad_mid_bits, true);
    doodad_light = QBitmap(4, 4, doodad_light_bits, true);
}

PhaseStyle::~PhaseStyle() {
    /* deleted by Qt */
}

void PhaseStyle::drawCheckMark ( QPainter * p, int x, int y, int w,
                                 int h, const QColorGroup & g,
                                 bool , bool) {
    p->setPen(g.text());
    p->drawPixmap(x+w/2-4, y+h/2-4, bcheck);
}

void PhaseStyle::drawArrow(QPainter *painter, Qt::ArrowType type, bool down,
                           int x, int y, int w, int h, const QColorGroup &group,
                           bool enabled , const QBrush * ) {
    switch( type ) {
    case UpArrow:
        if (enabled)
            painter->setPen(down ? group.midlight() : group.dark());
        else painter->setPen(group.mid());
        painter->drawPixmap(x+w/2-3, y+h/2-3, uarrow);
        break;
    case DownArrow:
        if (enabled) painter->setPen(down ? group.midlight() : group.dark());
        else painter->setPen(group.mid());
        painter->drawPixmap(x+w/2-3, y+h/2-3, darrow);
        break;
    case LeftArrow:
        if (enabled) painter->setPen(down ? group.midlight() : group.dark());
        else painter->setPen(group.mid());
        painter->drawPixmap(x+w/2-3, y+h/2-3, larrow);
        break;
    case RightArrow:
        if (enabled) painter->setPen(down ? group.midlight() : group.dark());
        else painter->setPen(group.mid());
        painter->drawPixmap(x+w/2-3, y+h/2-3, rarrow);
        break;
    }
}

void PhaseStyle::drawPushButton(QPushButton *btn, QPainter *painter) {
    QRect r = btn->rect();
    int x = r.x();
    int y = r.y();
    int h = r.height();
    int w = r.width();
    bool depress = btn->isOn() || btn->isDown();
    QColorGroup group = btn->colorGroup();
    QBrush brush(group.button() );
//    int bd = pixelMetric(PM_ButtonDefaultIndicator, widget) + 1;
    static int bd = 2;

    if (btn->isDefault() && !depress) {
        drawPanel(painter, x, y, w, h, group,
                  &group.brush(QColorGroup::Mid), true);
        drawBevelButton(painter, x+bd, y+bd, w-bd*2, h-bd*2, group,
                        false, &brush  );
    } else {
        drawButton(painter, x, y, w, h, group, depress,
                   &brush );
    }

    if (btn->hasFocus() ) { // draw focus
        QColor col;
        drawFocusRect(painter, r, group,
                      &col, false );
    }
}


void PhaseStyle::drawPanel(QPainter* painter, int x, int y, int w, int h,
                           const QColorGroup& group, bool sunken,
                           int , const QBrush* fill) {
    int x2 = x + w - 1;
    int y2 = y + h - 1;
    painter->save();

    if (sunken) {
        painter->setPen(group.dark());
        painter->drawRect(x+1, y+1, w-2, h-2);
        painter->setPen(group.midlight());
        painter->drawLine(x+1, y2, x2, y2);
        painter->drawLine(x2, y+1, x2, y2-1);
        painter->setPen(group.mid());
        painter->drawLine(x, y, x, y2-1);
        painter->drawLine(x+1, y, x2-1, y);
        painter->setPen(group.background());
        painter->drawPoint(x, y2);
        painter->drawPoint(x2, y);
    } else {
        painter->setPen(group.dark());
        painter->drawRect(x, y, w, h);
        painter->setPen(group.midlight());
        painter->drawLine(x+1, y+1, x2-2, y+1);
        painter->drawLine(x+1, y+2, x+1, y2-2);
        painter->setPen(group.mid());
        painter->drawLine(x+2, y2-1, x2-1, y2-1);
        painter->drawLine(x2-1, y+2, x2-1, y2-2);
        painter->setPen(group.background());
        painter->drawPoint(x+1, y2-1);
        painter->drawPoint(x2-1, y+1);
    }

    if (fill) {
        painter->fillRect(x+2, y+2, w-4, h-4, *fill );
    }
    painter->restore();
}



void  PhaseStyle::drawButton(QPainter *p, int x, int y, int w, int h,
                             const QColorGroup &group, bool sunken ,
                             const QBrush *fill  ) {
    int x2 = x + w - 1;
    int y2 = y + h - 1;

    p->setPen(group.midlight());
    p->drawLine(x+1, y2, x2, y2);
    p->drawLine(x2, y+1, x2, y2-1);

    p->setPen(group.mid());
    p->drawLine(x, y,  x2-1, y);
    p->drawLine(x, y+1, x, y2-1);

    p->setPen(group.button());
    p->drawPoint(x, y2);
    p->drawPoint(x2, y);

    drawBevelButton(p, x+1, y+1, w-2, h-2, group, sunken,
                    fill);
}

void PhaseStyle::drawButtonMask( QPainter* p, int x, int y,
                                 int w, int h ) {
    QRect rect(x, y, w, h );
    p->fillRect(rect, Qt::color1);
    p->setPen(Qt::color0);
}


void PhaseStyle::drawBevelButton(QPainter* p, int x, int y,
                                 int w, int h, const QColorGroup& group,
                                 bool sunken, const QBrush* fill ) {
    int x2 = x + w - 1;
    int y2 = y + h - 1;
    p->save();

    p->setPen(group.dark());
    p->drawRect(x, y, w, h);

    p->setPen(sunken ? group.mid() : group.midlight());
    p->drawLine(x+1, y+1, x2-2, y+1);
    p->drawLine(x+1, y+2, x+1, y2-2);

    p->setPen(sunken ? group.midlight() : group.mid());
    p->drawLine(x+2, y2-1, x2-1, y2-1);
    p->drawLine(x2-1, y+2, x2-1, y2-2);

    p->setPen(group.button());
    p->drawPoint(x+1, y2-1);
    p->drawPoint(x2-1, y+1);

    QBrush b = fill ? *fill : group.brush( QColorGroup::Button );
    if (sunken) {
        // sunken bevels don't get gradients
        p->fillRect(x+2, y+2, w-4, h-4, b);
    } else
        drawPhaseGradient(p, QRect(x+2, y+2, w-4, h-4), b.color() );

    p->restore();
}

void PhaseStyle::drawPhaseGradient(QPainter* painter,
                                   const QRect& rect,
                                   const QColor& color )const {
    painter->fillRect(rect, color);
}

void PhaseStyle::polish( QWidget* widget ) {
    QWindowsStyle::polish(widget );

#if 0
    if (widget->inherits("QMenuBar")    ||
        widget->inherits("QPopupMenu" ) ||
        widget->inherits("QToolButton") ||
        widget->inherits("QHeader" )   ) {
        widget->setBackgroundMode(QWidget::NoBackground);
    }
//    else if (widget->inherits("QFrame") ) {
//        widget->installEventFilter(this);
//    }
#endif
}

void PhaseStyle::unPolish( QWidget *w )
{
    QWindowsStyle::unPolish( w );
}

void PhaseStyle::polish( QPalette &pal ) {
    QWindowsStyle::polish( pal );
    // lighten up a bit, so the look is not so "crisp"
    if (QPixmap::defaultDepth() > 8) { // but not on low color displays
        pal.setColor(QPalette::Disabled, QColorGroup::Dark,
                     pal.color(QPalette::Disabled, QColorGroup::Dark).light(contrast));
        pal.setColor(QPalette::Active, QColorGroup::Dark,
                     pal.color(QPalette::Active, QColorGroup::Dark).light(contrast));
        pal.setColor(QPalette::Inactive, QColorGroup::Dark,
                     pal.color(QPalette::Inactive, QColorGroup::Dark).light(contrast));
    }
}

void PhaseStyle::polish( QApplication* app ) {
    QWindowsStyle::polish( app );

    qt_set_draw_menu_bar_impl((QDrawMenuBarItemImpl)&PhaseStyle::drawMenuBarItem);
}

void PhaseStyle::unPolish( QApplication* app ) {
    QWindowsStyle::unPolish(app);
    qt_set_draw_menu_bar_impl ( 0 );
}

void PhaseStyle::drawPushButtonLabel (QPushButton *button, QPainter *painter) {
    bool active = button->isOn() || button->isDown();
    QRect r = pushButtonContentsRect( button );
    QColorGroup group = button->colorGroup();
    int x, y, w, h;
    r.rect( &x, &y, &w, &h );
    bool sunken = false;
    QIconSet::Mode mode;
    QPixmap pixmap;

    if (active)  {// shift contents
        x++; y++;
        sunken = true;
    }


    if (button->isMenuButton()) { // draw the indicator
        //dx = pixelMetric(PM_MenuButtonIndicator, widget);
        int dx = menuButtonIndicatorWidth( button->height() );
        drawArrow(painter, Qt::DownArrow, active,
                  x+w-dx, y+2, dx-4, h-4, group,button->isEnabled() );
        w -= dx;
    }

    if (button->iconSet() && !button->iconSet()->isNull()) { // draw icon
        if (button->isEnabled()) {
            if (button->hasFocus()) {
                mode = QIconSet::Active;
            } else {
                mode = QIconSet::Normal;
            }
        } else {
            mode = QIconSet::Disabled;
        }

#if 0
        if (button->isToggleButton() && button->isOn()) {
            state = true;
        } else {
            state = false;
        }
#endif

        if ( mode == QIconSet::Disabled )
            pixmap = button->iconSet()->pixmap( QIconSet::Automatic, mode );
        else
            pixmap = button->iconSet()->pixmap();
        if (button->text().isEmpty() && !button->pixmap()) {
            painter->drawPixmap(x+w/2 - pixmap.width()/2,
                                y+h/2 - pixmap.height()/2, pixmap);
        } else {
            painter->drawPixmap(x+4, y+h/2 - pixmap.height()/2, pixmap);
        }
        x += pixmap.width() + 4;
        w -= pixmap.width() + 4;
    }

    if (active || button->isDefault()) { // default button
        for(int n=0; n<2; n++) {
            drawItem(painter, x+n, y, w, h,
                     AlignCenter | ShowPrefix,
                     button->colorGroup(),
                     button->isEnabled(),
                     button->pixmap(),
                     button->text(), -1,
                     (button->isEnabled()) ?
                     &button->colorGroup().buttonText() :
                     &button->colorGroup().mid());
        }
    } else { // normal button
        drawItem(painter, x, y, w, h,
                 AlignCenter | ShowPrefix,
                 button->colorGroup(),
                 button->isEnabled(),
                 button->pixmap(),
                 button->text(), -1,
                 (button->isEnabled()) ?
                 &button->colorGroup().buttonText() :
                 &button->colorGroup().mid());
    }
}


void PhaseStyle::drawFocusRect(QPainter *painter, const QRect &rect,
                               const QColorGroup &group,
                               const QColor *, bool atBorder) {


    QPen old = painter->pen();
    painter->setPen(group.highlight().dark(contrast));
    painter->setBrush(NoBrush);

    if ( atBorder )
        painter->drawRect(QRect(rect.x()+1,     rect.y()+1,
                                rect.width()-2, rect.height()-2 ));
    else
        painter->drawRect(rect);


    painter->setPen(old);

}

void PhaseStyle::drawSeperator( QPainter* painter, int x, int y, int w,
                                int h, const QColorGroup& group,  bool,
                                int , int  ) {
    qWarning( "Seperator" );
    QRect rect(x, y, w, h);
    int x2 = rect.right();
    int y2 = rect.bottom();

    painter->setPen(group.dark());
    if (w < h)
        painter->drawLine(w/2, y, w/2, y2);
    else
        painter->drawLine(x, h/2, x2, h/2);
}

void PhaseStyle::drawMenuBarItem(QPainter* p, int x, int y, int w, int h,
                         QMenuItem *mi, QColorGroup& g, bool enabled,
                         bool act ) {
    return QWindowsStyle::drawMenuBarItem(p, x, y, w, h, mi, g, enabled, act);
}


void PhaseStyle::drawIndicator(QPainter* painter, int x, int y, int w, int h,
                               const QColorGroup &group, int state, bool,
                               bool enabled  ) {
    drawPanel(painter, x, y, w, h, group, true, 1, enabled ?
                   &group.brush(QColorGroup::Base) :
                   &group.brush(QColorGroup::Background));

    if (QButton::On == state ) {
        painter->setPen(group.dark());
        painter->drawRect(x+3, y+3, w-6, h-6);
        painter->fillRect(x+4, y+4, w-8, h-8,
                          group.brush(QColorGroup::Highlight));
    }
}


void PhaseStyle::drawExclusiveIndicator(QPainter* painter,  int x, int y, int w, int h,
                                        const QColorGroup &group, bool on,
                                        bool /*down*/, bool enabled) {

    QRect r(x, y, w, h );


    /*
     * As Polygon and Polyline are broken in Qt2 lets use
     * something not that spectacilur -> ellipse
     */
    painter->save();
    painter->fillRect(x, y, w, h, group.background());


    painter->setBrush(enabled
                      ? group.brush(QColorGroup::Base)
                      : group.brush(QColorGroup::Background));
    painter->setPen(group.dark());

    if (0 == w % 2) --w;
    if (0 == h % 2) --h;
    painter->drawEllipse(x, y, w, h );

    if(on) {
        painter->setBrush(group.brush(QColorGroup::Highlight));
        painter->drawEllipse(x+3,y+3,w-6,h-6);
    }

    painter->restore();
}


/*
 * Does not get called in QWS as it seems
 */
void PhaseStyle::drawExclusiveIndicatorMask(QPainter *painter, int x, int y, int w,
                                            int h, bool /*on*/ ) {
    return;



    if (0 != w%2) --w;
    if (0 != h%2) --h;

    QRect r(x, y, w, h );
    int x2 = r.right();
    int y2 = r.bottom();
    int cx = (x + x2) / 2;
    int cy = (y + y2) / 2;
    QPointArray parray;


    painter->setBrush(Qt::color1);
    painter->setPen(Qt::color1);
    parray.putPoints(0, 8,
                     x,cy+1, x,cy,    cx,y,    cx+1,y,
                     x2,cy,  x2,cy+1, cx+1,y2, cx,y2);
    painter->drawPolygon(parray, 0, 8);
}

int PhaseStyle::defaultFrameWidth()const {
    return 1;
}

int PhaseStyle::popupMenuItemHeight ( bool ,
                                      QMenuItem * mi,
                                      const QFontMetrics & fm ) {
    int h = 0;
    if (mi->custom() ) {
        h = mi->custom()->sizeHint().height();
        if (!mi->custom()->fullSpan() )
            h += ITEMVMARGIN*2 + ITEMFRAME*2;
    }else if (mi->isSeparator() ) {
        h = 1;
    }else {
        if ( mi->pixmap() ) {
            h = QMAX(h, mi->pixmap()->height() + ITEMFRAME*2);
        }else {
            h = QMAX(h, MINICONSIZE+ITEMFRAME*2 );
            h = QMAX(h, fm.height()
                     + ITEMVMARGIN*2 + ITEMFRAME*2 );
        }
        if ( mi->iconSet() )
            h = QMAX(h, mi->iconSet()->pixmap().height()
                     + ITEMFRAME*2 );
    }


    return h;
}

int PhaseStyle::extraPopupMenuItemWidth(bool checkable, int maxpmw,
                                        QMenuItem* mi, const QFontMetrics& ) {
    int w = 0;
    if (mi->isSeparator() )
        return 3;

    else if ( mi->pixmap() )
        w = mi->pixmap()->width();

    if (!mi->text().isNull() &&
        mi->text().find('\t' ) >= 0 )
        w += 12;
    else if ( mi->popup() )
        w += 2*ARROWMARGIN;

    if ( maxpmw  )
        w += maxpmw +4;


    if (  maxpmw > 0 || checkable )
        w += ITEMHMARGIN*2+8;

    w += RIGHTBORDER;
    return w;
}

QSize PhaseStyle::indicatorSize()const {
    return QSize( 11, 11 );
}

QSize PhaseStyle::exclusiveIndicatorSize()const {
    return QSize( 11, 11 );
}

void PhaseStyle::getButtonShift( int &x, int &y ) {
    x++;
    y++;
}

void PhaseStyle::drawPopupMenuItem ( QPainter * p, bool checkable,
                                     int maxpmw,int tabwidth,
                                     QMenuItem * mi, const QPalette & pal,
                                     bool act, bool enabled, int x,
                                     int y, int w, int h ) {
    if ( !mi )
        return;

    QRect rect(x, y, w, h );
    int x2, y2;
    x2 = rect.right();
    y2 = rect.bottom();
    const QColorGroup& g = pal.active();
    QColorGroup itemg    = !enabled ? pal.disabled() : pal.active();

    if ( checkable || maxpmw ) maxpmw = QMAX(maxpmw, 20);

    if (act && enabled )
        p->fillRect(x, y, w, h, g.highlight() );
    else
        p->fillRect(x, y, w, h, g.background() );

    // draw seperator
    if (mi->isSeparator() ) {
    p->setPen( g.dark() );
    p->drawLine( x+8, y+1, x+w-8, y+1 );

        p->setPen( g.mid() );
        p->drawLine( x+8,   y, x+w-8, y );
        p->drawPoint(x+w,y+1);

    p->setPen( g.midlight() );
    p->drawLine( x+8, y-1, x+w-8, y-1 );
        p->drawPoint(x+8, y );
        return;
    }

    // draw icon
    QIconSet::Mode mode;
    if ( mi->iconSet() && !mi->isChecked() ) {
        if ( act )
            mode = enabled ? QIconSet::Active : QIconSet::Disabled;
        else
            mode = enabled ? QIconSet::Normal : QIconSet::Disabled;
        QPixmap pixmap;
        if ( mode == QIconSet::Disabled )
            pixmap = mi->iconSet()->pixmap( QIconSet::Automatic, mode );
        else
            pixmap = mi->iconSet()->pixmap();
        QRect pmrect(0, 0, pixmap.width(), pixmap.height() );
        QRect cr(x, y, maxpmw, h );
        pmrect.moveCenter( cr.center() );
        p->drawPixmap(pmrect.topLeft(), pixmap);
    }

    // draw check
    if(mi->isChecked() ) {
        drawCheckMark(p, x, y, maxpmw, h, itemg, act, !enabled );
    }


    // draw text
    int xm = maxpmw + 2;
    int xp = x + xm;
    int tw = w -xm - 2;

    p->setPen( enabled ? ( act ? g.highlightedText() : g.buttonText() ) :
                         g.mid() );


    if ( mi->custom() ) {
        p->save();
        mi->custom()->paint(p, g, act, enabled,
                              xp, y+1, tw, h-2 );
        p->restore();
    }else { // draw label
        QString text = mi->text();
        if (!text.isNull() ) {
            int t = text.find('\t');
            const int tflags = AlignVCenter | DontClip |
                               ShowPrefix |  SingleLine |
                               AlignLeft;

            if (t >= 0) {
                int tabx = x + w - tabwidth - RIGHTBORDER -
                           ITEMHMARGIN - ITEMFRAME;
                p->drawText(tabx, y+ITEMVMARGIN, tabwidth,
                            h-2*ITEMVMARGIN, tflags,
                            text.mid(t+1) );
                text = text.left(t );
            }

            // draw left label
             p->drawText(xp, y+ITEMVMARGIN,
                         tw, h-2*ITEMVMARGIN,
                         tflags, text, t);
        }else if ( mi->pixmap() ) { // pixmap as label
            QPixmap pixmap = *mi->pixmap();
            if ( pixmap.depth() == 1 )
                p->setBackgroundMode( OpaqueMode );

            int dx = ((w-pixmap.width() )  /2 ) +
                 ((w - pixmap.width()) %2 );
            p->drawPixmap(x+dx, y+ITEMFRAME, pixmap );

            if ( pixmap.depth() == 1 )
                p->setBackgroundMode( TransparentMode );
        }
    }

    if ( mi->popup() ) { // draw submenu arrow
        int dim = (h-2*ITEMFRAME) / 2;
        drawArrow( p, RightArrow, false,
                   x+w-ARROWMARGIN-ITEMFRAME-dim,
                   y+h/2-dim/2, dim, dim, g, enabled );
    }
}


QRect PhaseStyle::comboButtonRect ( int x, int y, int w, int h ) {
    return QRect(x+2, y+2, w-4-17, h-5 );
}

void PhaseStyle::drawComboButton( QPainter * p, int x, int y,
                                  int w, int h,
                                  const QColorGroup & g,
                                  bool sunken,
                                  bool editable,
                                  bool,
                                  const QBrush *) {
    drawButton(p, x, y, w, h, g,
               sunken, &g.brush(QColorGroup::Button ));

    for ( int n = 0; n < 2; ++n )
        kColorBitmaps(p, g, w-16+(6*n), y+(h/2)-2,
                      0, &doodad_mid, &doodad_light, 0, 0, 0 );


    if (editable ) {
        const int x2 = x+w-1; const int y2 = y+h-1;
        p->setPen(g.dark());
        p->drawLine(x2+1, y, x2+1, y2);
        p->setPen(g.midlight());
        p->drawLine(x2+2, y, x2+2, y2-1);
        p->setPen(g.button());
        p->drawPoint(x2+2, y2);
    }

    p->setPen(g.buttonText() );
}

void PhaseStyle::tabbarMetrics( const QTabBar* t, int &hframe, int &vframe,  int& ov ) {
    QCommonStyle::tabbarMetrics( t, hframe, vframe, ov );
    hframe -= 2;
    vframe += 1;
}


void PhaseStyle::drawTab(QPainter* painter, const QTabBar* bar, QTab* tab,
                         bool selected ) {
    bool edge = false;
    int x, y, w, h;
    QRect r = tab->rect();
    r.rect(&x,&y,&w,&h);
    const int x2 = x+w-1;
    const int y2 = y+h-1;
    const QColorGroup &group = bar->colorGroup();


    painter->save();

    // what position is the tab?
    if ((bar->count() == 1 ))
    edge = true;
    else
    edge = false;

    switch (QTabBar::Shape(bar->shape())) {
    case QTabBar::RoundedAbove:
    case QTabBar::TriangularAbove: {
        if (!selected) { // shorten
            y += 2; h -= 2;
        }
        if (selected) {
            painter->setPen(Qt::NoPen);
            painter->fillRect(x+1, y+1, w-1, h-1,
                              group.brush(QColorGroup::Background));
        } else
            drawPhaseGradient(painter, QRect(x+1, y+1, w-1, h-2),
                              group.background().dark(contrast) );


        // draw tab
        painter->setPen(group.dark());
        painter->drawLine(x, y, x, y2-2);
        painter->drawLine(x+1, y, x2, y);
        painter->drawLine(x2, y+1, x2, y2-2);

        painter->setPen(group.mid());
        painter->drawLine(x2-1, y+2, x2-1, y2-2);

        painter->setPen(group.midlight());
        painter->drawLine(x+1, y+1, x2-2, y+1);
        if ((selected) || edge) painter->drawLine(x+1, y+2, x+1, y2-2);

        // finish off bottom
        if (selected) {
            painter->setPen(group.dark());
            painter->drawPoint(x, y2-1);
            painter->drawPoint(x2, y2-1);

            painter->setPen(group.midlight());
            painter->drawPoint(x, y2);
            painter->drawLine(x+1, y2-1, x+1, y2);
            painter->drawPoint(x2, y2);

            painter->setPen(group.mid());
            painter->drawPoint(x2-1, y2-1);

            if (edge) {
                painter->setPen(group.dark());
                painter->drawLine(x, y2-1, x, y2);
                painter->setPen(group.midlight());
                painter->drawPoint(x+1, y2);
            }
        } else {
            painter->setPen(group.dark());
            painter->drawLine(x, y2-1, x2, y2-1);

            painter->setPen(group.midlight());
            painter->drawLine(x, y2, x2, y2);

            if (edge) {
                painter->setPen(group.dark());
                painter->drawLine(x, y2-1, x, y2);
            }
        }
        break;
    }
    case QTabBar::RoundedBelow:
    case QTabBar::TriangularBelow: {
        painter->setBrush( group.background().dark(contrast));
        painter->setPen(Qt::NoPen);
        painter->fillRect(x+1, y+1, w-1, h-1, painter->brush());

        // draw tab
        painter->setPen(group.dark());
        painter->drawLine(x, y+1, x, y2);
        painter->drawLine(x+1, y2, x2, y2);
        painter->drawLine(x2, y+1, x2, y2-1);

        painter->setPen(group.mid());
        painter->drawLine(x2-1, y+1, x2-1, y2-1);
        painter->drawLine(x+2, y2-1, x2-1, y2-1);
        painter->drawPoint(x, y);
        painter->drawPoint(x2, y);

        if ((selected) || edge) {
            painter->setPen(group.midlight());
            painter->drawLine(x+1, y+1, x+1, y2-2);
        }

        // finish off top
        if (selected) {
            if (edge) {
                painter->setPen(group.dark());
                painter->drawPoint(x, y);
                painter->setPen(group.midlight());
                painter->drawPoint(x+1, y);
            }
        } else {
            painter->setPen(group.dark());
            painter->drawLine(x, y+1, x2, y+1);

            painter->setPen(group.mid());
            painter->drawLine(x, y, x2, y);

            if (edge) {
                painter->setPen(group.dark());
                painter->drawPoint(x, y);
            }
        }
          break;
    }
    }

    painter->restore();
}

void PhaseStyle::drawTabMask( QPainter* p, const QTabBar*, QTab* tab,
                              bool ) {
    p->fillRect(tab->rect(), Qt::color1);
}

void PhaseStyle::drawToolButton(QPainter * p, int x, int y,
                                int w, int h, const QColorGroup & g,
                                bool sunken,
                                const QBrush * fill ) {
    QRect r(x, y, w, h );
    p->fillRect(r, g.background());

    drawPanel(p, x, y, w, h, g, sunken, 1,
              fill ? fill : &g.brush(QColorGroup::Button) );


    p->setPen(g.text() );
}


/*
 * LGPLv2  Copyright (C) 2001 Rik Hemsley (rikkus) <rik@kde.org>
 *
 * ../web/webstyle.cpp
 */
void PhaseStyle::scrollBarMetrics(const QScrollBar* sb, int& sliderMin,
                                  int& sliderMax, int &sliderLength,
                                  int& buttonDim ) {
    int maxlen;
    bool horizontal = sb->orientation() == QScrollBar::Horizontal;
    int len = (horizontal) ? sb->width() : sb->height();
    int extent = (horizontal) ? sb->height() : sb->width();

    if (len > (extent - 1) * 2)
        buttonDim = extent;
    else
        buttonDim = len / 2 - 1;

    if (horizontal)
        sliderMin = buttonDim * 2;
    else
        sliderMin = 1;

    maxlen = len - buttonDim * 2 - 1;
    int div = QMAX(1, (sb->maxValue() - sb->minValue() + sb->pageStep() ) );

    sliderLength =
        (sb->pageStep() * maxlen) / div;

    if (sliderLength < SCROLLBAR_EXTENT)
        sliderLength = SCROLLBAR_EXTENT;

    if (sliderLength > maxlen)
        sliderLength = maxlen;

    sliderMax = sliderMin + maxlen - sliderLength;
}

void PhaseStyle::drawScrollBarControls( QPainter* p, const QScrollBar *sb,
                                        int sliderStart, uint controls,
                                        uint activeControl) {
    const bool horizontal = (sb->orientation() == Qt::Horizontal );
    int sliderMin, sliderMax, sliderLength, buttonDim;
    const QColorGroup& g = sb->colorGroup();
    QRect sub, add, subPage, addPage, slider;
    int x, y, x2, y2;

    scrollBarMetrics( sb, sliderMin, sliderMax, sliderLength, buttonDim );

    /* lets get the positions */
    scrollBarItemPositions(sb, horizontal, sliderStart,
                           sliderMax, sliderLength, buttonDim,
                           sub, add, subPage, addPage,
                           slider );


    /* sanity */
    if ( sliderStart > sliderMax )
        sliderStart = sliderMax;

    // addline
    if ( controls & AddLine && add.isValid() ) {
        x =  add.x();     y  = add.y();
        x2 = add.width(); y2 = add.height();
        const bool down = (activeControl & AddLine );
        drawBevelButton(p, x, y, x2, y2, g, down,
                        &g.brush(QColorGroup::Button));

        Qt::ArrowType arrow = horizontal ? RightArrow : DownArrow;
        if (down ) {
            switch (arrow ) {
            case DownArrow:   y++; break;
            case RightArrow:  x++; break;
            default:               break;
            }
        }
        drawArrow( p, arrow, down, x, y, x2, y2, g, true );
    }

    if ( controls & SubLine && sub.isValid() ) {
        x =  sub.x();     y  = sub.y();
        x2 = sub.width(); y2 = sub.height();
        const bool down = (activeControl & SubLine );
        drawBevelButton(p, x, y, x2, y2, g, down,
                        &g.brush(QColorGroup::Button));

        Qt::ArrowType arrow = horizontal ? LeftArrow : UpArrow;
        if (down ) {
            switch (arrow ) {
            case UpArrow:   y--; break;
            case LeftArrow: x--; break;
            default:             break;
            }
        }
        drawArrow( p, arrow, down, x, y, x2, y2, g, true );
    }

    if ( controls & AddPage && addPage.isValid()  ) {
        x  = addPage.x();     y  = addPage.y();
        x2 = addPage.right(); y2 = addPage.bottom();

        p->fillRect(addPage, g.mid());
        p->setPen(g.dark());
        if (horizontal) { // vertical
            p->drawLine(x, y, x2, y);
            p->drawLine(x, y2, x2, y2);
        } else { // horizontal
            p->drawLine(x, y, x, y2);
            p->drawLine(x2, y, x2, y2);
        }
    }

    if ( controls & SubPage && subPage.isValid() ) {
        x  = subPage.x();     y  = subPage.y();
        x2 = subPage.right(); y2 = subPage.bottom();

        p->fillRect(subPage, g.mid());
        p->setPen(g.dark());
        if (horizontal) { // vertical
            p->drawLine(x, y, x2, y);
            p->drawLine(x, y2, x2, y2);
        } else { // horizontal
            p->drawLine(x, y, x, y2);
            p->drawLine(x2, y, x2, y2);
        }
}

    if ( controls & Slider && slider.isValid() ) {
        x  = slider.x();     y  = slider.y();
        x2 = slider.width(); y2 = slider.height();
        const bool down = ( activeControl & Slider );
        int cx = x + x2/2 -2; int cy = y + y2/2 -2;

        drawBevelButton(p, x, y, x2, y2, g, down,
                        &g.brush(QColorGroup::Button) );

        if (horizontal && (x2 >=20)) {
            for (int n = -5; n <= 5; n += 5)
                kColorBitmaps(p, g, cx+n, cy,
                                0, &doodad_mid, &doodad_light, 0, 0, 0);
        } else if (!horizontal && (y2 >= 20)) {
            for (int n = -5; n <= 5; n += 5)
                kColorBitmaps(p, g, cx, cy+n,
                              0, &doodad_mid, &doodad_light, 0, 0, 0);
        }

        if ( sb->hasFocus() && down ) {
            slider = QRect(slider.x()+2, slider.y()+2,
                           slider.width()-2, slider.height()-2 );
            drawFocusRect(p, slider, g, false );
        }

    }
}

/*
 * LGPLv2  Copyright (C) 2001 Rik Hemsley (rikkus) <rik@kde.org>
 *
 * ../web/webstyle.cpp
 */
QStyle::ScrollControl PhaseStyle::scrollBarPointOver( const QScrollBar * sb,
                                     int sliderStart,
                                     const QPoint & point ) {
    if (!sb->rect().contains(point))
        return NoScroll;

    int sliderMin, sliderMax, sliderLength, buttonDim;
    scrollBarMetrics(sb, sliderMin, sliderMax, sliderLength, buttonDim);

    if (sb->orientation() == QScrollBar::Horizontal) {
        int x = point.x();

        if (x <= buttonDim)
            return SubLine;
        else if (x <= buttonDim * 2)
            return AddLine;
        else if (x < sliderStart)
            return SubPage;
        else if (x < sliderStart+sliderLength)
            return Slider;
        else
            return AddPage;
    } else   {
        int y = point.y();

        if (y < sliderStart)
            return SubPage;
        else if (y < sliderStart + sliderLength)
            return Slider;
        else if (y < sliderMax + sliderLength)
            return AddPage;
        else if (y < sliderMax + sliderLength + buttonDim)
            return SubLine;
        else
            return AddLine;
  }
}

/*
 * LGPLv2  Copyright (C) 2001 Rik Hemsley (rikkus) <rik@kde.org>
 *
 * ../web/webstyle.cpp
 * scrollBarControlsMetrics
 */
void PhaseStyle::scrollBarItemPositions( const QScrollBar* sb, const bool horizontal,
                                         int sliderStart, int sliderMax,
                                         int sliderLength, int buttonDim,
                                         QRect &rSub, QRect& rAdd, QRect& rSubPage,
                                         QRect &rAddPage, QRect& rSlider ) {

    int len     = horizontal ? sb->width()  : sb->height();
    int extent  = horizontal ? sb->height() : sb->width();

    QColorGroup g = sb->colorGroup();

    if (sliderStart > sliderMax)
        sliderStart = sliderMax;

    int sliderEnd = sliderStart + sliderLength;

    int addX, addY;
    int subX, subY;
    int subPageX, subPageY, subPageW, subPageH;
    int addPageX, addPageY, addPageW, addPageH;
    int sliderX, sliderY, sliderW, sliderH;

    if (horizontal){
        subY      = 0;
        addY      = 0;
        subX      = 0;
        addX      = buttonDim;

        subPageX  = buttonDim * 2;
        subPageY  = 0;
        subPageW  = sliderStart - 1;
        subPageH  = extent;

        addPageX  = sliderEnd;
        addPageY  = 0;
        addPageW  = len - sliderEnd;
        addPageH  = extent;

        sliderX   = sliderStart;
        sliderY   = 0;
        sliderW   = sliderLength;
        sliderH   = extent;
    }else {
        subX    = 0;
        addX    = 0;
        subY    = len - buttonDim * 2;
        addY    = len - buttonDim;

        subPageX = 0;
        subPageY = 0;
        subPageW = extent;
        subPageH = sliderStart;

        addPageX  = 0;
        addPageY  = sliderEnd;
        addPageW  = extent;
        addPageH  = subY - sliderEnd;

        sliderX   = 0;
        sliderY   = sliderStart;
        sliderW   = extent;
        sliderH   = sliderLength;
    }

    rSub      .setRect(    subX,      subY, buttonDim, buttonDim);
    rAdd      .setRect(    addX,      addY, buttonDim, buttonDim);
    rSubPage  .setRect(subPageX,  subPageY,  subPageW,  subPageH);
    rAddPage  .setRect(addPageX,  addPageY,  addPageW,  addPageH);
    rSlider   .setRect( sliderX,   sliderY,   sliderW,   sliderH);

}


void PhaseStyle::drawSlider (QPainter * p, int x, int y,
                             int w, int h, const QColorGroup &g,
                             Orientation o, bool, bool ) {
    int cx = x + w/2;
    int cy = y + h/2;
    QBrush brush = g.brush( QColorGroup::Button );

    if ( o == Horizontal ) {
        drawBevelButton(p, cx-5, y, 6, h, g, false,
                        &brush );
        drawBevelButton(p, cx, y, 6, h, g, false,
                        &brush );
    }else {
        drawBevelButton(p, x, cy-5, w, 6, g, false,
                        &brush );
        drawBevelButton(p, x, cy, w, 6, g, false,
                        &brush );
    }
}

void PhaseStyle::drawSliderGroove(QPainter* p, int x, int y,
                                  int w, int h, const QColorGroup& g,
                                  QCOORD , Orientation o ) {
    int cx = x + w/2;
    int cy = y + h/2;

    if ( o == Horizontal ) {
        y = cy-3; h = 7;
    }else {
        x = cx-3; w = 7;
    }

    drawPanel(p, x, y, w, h, g, true, 1,
              &g.brush(QColorGroup::Mid ) );
}
