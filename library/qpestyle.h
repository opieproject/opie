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

#ifndef QPESTYLE_H
#define QPESTYLE_H

#ifndef QT_H
#include "qwindowsstyle.h"
#endif // QT_H

#if QT_VERSION >= 300

class Q_EXPORT QPEStyle : public QWindowsStyle
{
public:
    QPEStyle();
    virtual ~QPEStyle();

    virtual void drawPrimitive( PrimitiveElement pe, QPainter *p, const QRect &r, const QColorGroup &cg, SFlags flags=Style_Default, const QStyleOption & = QStyleOption::Default) const;
    virtual void drawControl( ControlElement ce, QPainter *p, const QWidget *widget, const QRect &r, const QColorGroup &cg, SFlags how=Style_Default, const QStyleOption & = QStyleOption::Default) const;
    virtual void drawComplexControl( ComplexControl control, QPainter *p, const QWidget *widget, const QRect &r, const QColorGroup &cg, SFlags how=Style_Default, SCFlags sub=SC_All, SCFlags subActive=SC_None, const QStyleOption & = QStyleOption::Default) const;
    virtual int pixelMetric( PixelMetric metric, const QWidget *widget=0 ) const;
    virtual QSize sizeFromContents( ContentsType contents, const QWidget *widget, const QSize &contentsSize, const QStyleOption & = QStyleOption::Default) const;
};

#else

class Q_EXPORT QPEStyle : public QWindowsStyle
{
public:
    QPEStyle();
    virtual ~QPEStyle();
    virtual void polish( QPalette &p );
    virtual void polish( QWidget *w );
    virtual void unPolish( QWidget *w );

    int defaultFrameWidth () const;
    void drawPanel ( QPainter * p, int x, int y, int w, int h,
		    const QColorGroup &, bool sunken=FALSE, int lineWidth = 1, const QBrush * fill = 0 );
    void drawButton( QPainter *p, int x, int y, int w, int h,
		    const QColorGroup &g, bool sunken, const QBrush* fill );
    void drawButtonMask ( QPainter * p, int x, int y, int w, int h );
    void drawBevelButton( QPainter *p, int x, int y, int w, int h,
		    const QColorGroup &g, bool sunken=FALSE, const QBrush* fill=0 );
    QRect comboButtonRect( int x, int y, int w, int h);
    QRect comboButtonFocusRect( int x, int y, int w, int h);
    void drawComboButton( QPainter *p, int x, int y, int w, int h,
                    const QColorGroup &g, bool sunken, bool, bool enabled,
		    const QBrush *fill );
    void drawExclusiveIndicator ( QPainter * p, int x, int y, int w, int h,
		    const QColorGroup & g, bool on, bool down = FALSE, bool enabled = TRUE );
    void drawIndicator ( QPainter * p, int x, int y, int w, int h,
		    const QColorGroup & g, int state, bool down = FALSE, bool enabled = TRUE );
    void scrollBarMetrics( const QScrollBar*, int&, int&, int&, int&);
    void drawScrollBarControls( QPainter*,  const QScrollBar*, int sliderStart, uint controls, uint activeControl );
    ScrollControl scrollBarPointOver( const QScrollBar* sb, int sliderStart, const QPoint& p );
    void drawRiffles( QPainter* p,  int x, int y, int w, int h,
                          const QColorGroup &g, bool horizontal );
    int sliderLength() const;
    void drawSlider( QPainter *p, int x, int y, int w, int h,
		    const QColorGroup &g, Orientation, bool tickAbove, bool tickBelow );
    void drawSliderMask( QPainter *p, int x, int y, int w, int h,
		    Orientation, bool tickAbove, bool tickBelow );
    void drawSliderGrooveMask( QPainter *p, int x, int y, int w, int h,
		    const QColorGroup& , QCOORD c, Orientation orient );
    void drawTab( QPainter *, const QTabBar *, QTab *, bool selected );
    int extraPopupMenuItemWidth( bool checkable, int maxpmw, QMenuItem*, const QFontMetrics& );
    int popupMenuItemHeight( bool checkable, QMenuItem*, const QFontMetrics& );
    void drawPopupMenuItem( QPainter* p, bool checkable, int maxpmw, int tab, QMenuItem* mi,
				       const QPalette& pal,
				       bool act, bool enabled, int x, int y, int w, int h);

    int buttonMargin() const;
    QSize scrollBarExtent() const;

private:	// Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    QPEStyle( const QPEStyle & );
    QPEStyle& operator=( const QPEStyle & );
#endif
};

#endif

#endif // QPESTYLE_H
