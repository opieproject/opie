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

#ifndef FLATSTYLE_H
#define FLATSTYLE_H

#include <qwindowsstyle.h>
#include <qpe/styleinterface.h>

class FlatStylePrivate;

class Q_EXPORT FlatStyle : public QWindowsStyle
{
public:
    FlatStyle();
    virtual ~FlatStyle();
    virtual void polish( QPalette &p );
    virtual void polish( QWidget *w );
    virtual void unPolish( QWidget *w );

    int defaultFrameWidth () const;
    void drawItem( QPainter *p, int x, int y, int w, int h,
                    int flags, const QColorGroup &g, bool enabled,
		    const QPixmap *pixmap, const QString& text, int len, const QColor* penColor );
    void drawPanel ( QPainter * p, int x, int y, int w, int h,
		    const QColorGroup &, bool sunken=FALSE, int lineWidth = 1, const QBrush * fill = 0 );
    void drawButton( QPainter *p, int x, int y, int w, int h,
		    const QColorGroup &g, bool sunken, const QBrush* fill );
    void drawButtonMask ( QPainter * p, int x, int y, int w, int h );
    void drawBevelButton( QPainter *p, int x, int y, int w, int h,
		    const QColorGroup &g, bool sunken=FALSE, const QBrush* fill=0 );
    void drawToolButton( QPainter *p, int x, int y, int w, int h,
		    const QColorGroup &g, bool sunken=FALSE, const QBrush* fill=0 );
    void drawPushButton( QPushButton *btn, QPainter *p );
    void drawPushButtonLabel( QPushButton *btn, QPainter *p );
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
    void drawSliderGroove ( QPainter * p, int x, int y, int w, int h, const QColorGroup & g, QCOORD c, Orientation );
    void drawTab( QPainter *, const QTabBar *, QTab *, bool selected );
    int extraPopupMenuItemWidth( bool checkable, int maxpmw, QMenuItem*, const QFontMetrics& );
    int popupMenuItemHeight( bool checkable, QMenuItem*, const QFontMetrics& );
    void drawPopupMenuItem( QPainter* p, bool checkable, int maxpmw, int tab, QMenuItem* mi,
				       const QPalette& pal,
				       bool act, bool enabled, int x, int y, int w, int h);

    int buttonMargin() const;
    QSize scrollBarExtent() const;
    void getButtonShift( int &x, int &y );

private:
    FlatStylePrivate *d;
    bool revItem;
    // Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    FlatStyle( const FlatStyle & );
    FlatStyle& operator=( const FlatStyle & );
#endif
};


class FlatStyleImpl : public StyleInterface
{
public:
    FlatStyleImpl();
    virtual ~FlatStyleImpl();

    QRESULT queryInterface( const QUuid&, QUnknownInterface** );
    Q_REFCOUNT

    virtual QStyle *style();
    virtual QString name() const;

private:
    FlatStyle *flat;
    ulong ref;
};

#endif // FLATSTYLE_H
