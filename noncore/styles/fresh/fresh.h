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

#ifndef FRESHSTYLE_H
#define FRESHSTYLE_H

#include <qwindowsstyle.h>
#include <qpe/styleinterface.h>

class Q_EXPORT FreshStyle : public QWindowsStyle
{
public:
    FreshStyle();
    virtual ~FreshStyle();
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
    FreshStyle( const FreshStyle & );
    FreshStyle& operator=( const FreshStyle & );
#endif
};


class FreshStyleImpl : public StyleInterface
{
public:
    FreshStyleImpl();
    virtual ~FreshStyleImpl();

    QRESULT queryInterface( const QUuid&, QUnknownInterface** );
    Q_REFCOUNT

    virtual QStyle *style();
    virtual QString name() const;

private:
    FreshStyle *fresh;
    ulong ref;
};

#endif // FRESHSTYLE_H
