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
#ifndef LIGHTSTYLE_H
#define LIGHTSTYLE_H


#ifndef QT_H
#include <qstyle.h>
#include <qwindowsstyle.h>
#endif // QT_H

#if QT_VERSION < 300

#ifdef QT_PLUGIN_STYLE_LIGHT
#  define Q_EXPORT_STYLE_LIGHT
#else
#  define Q_EXPORT_STYLE_LIGHT Q_EXPORT
#endif // QT_PLUGIN_STYLE_LIGHT


class Q_EXPORT_STYLE_LIGHT LightStyle : public QWindowsStyle
{
public:
    LightStyle();
    virtual ~LightStyle();

    void polish(QWidget *widget);
    void unPolish(QWidget*widget);

    void polish(QApplication *app);
    void unPolish(QApplication *app);

    void polishPopupMenu(QPopupMenu *menu);

    void drawPushButton(QPushButton *button, QPainter *p);
    void drawButton(QPainter *p, int x, int y, int w, int h,
                    const QColorGroup &g, bool sunken = FALSE,
                    const QBrush *fill = 0);
    void drawBevelButton(QPainter *p, int x, int y, int w, int h,
                         const QColorGroup &g, bool sunken = FALSE,
                         const QBrush *fill = 0);
    void getButtonShift(int &x, int &y) const;

    void drawComboButton(QPainter *p, int x, int y, int w, int h,
                         const QColorGroup &g, bool sunken = FALSE,
                         bool editable = FALSE, bool = TRUE,
                         const QBrush *fill = 0);
    QRect comboButtonRect(int x, int y, int w, int h) const;
    QRect comboButtonFocusRect(int x, int y, int w, int h) const;

    void drawIndicator(QPainter *p, int x, int y ,int w, int h,
                       const QColorGroup &g, int state,
                       bool = FALSE, bool = TRUE);
    QSize indicatorSize() const;

    void drawExclusiveIndicator(QPainter *p, int x, int y ,int w, int h,
                                const QColorGroup &g, bool on,
                                bool = FALSE, bool = TRUE);
    QSize exclusiveIndicatorSize() const;

    void drawPanel(QPainter * p, int x, int y, int w, int h,
                   const QColorGroup &g, bool sunken = FALSE,
                   int = 1, const QBrush * = 0);

    void scrollBarMetrics( const QScrollBar *,
                           int &, int &, int &, int & ) const;
    void drawScrollBarControls(QPainter* p, const QScrollBar* sb,
                               int sliderStart, uint controls,
                               uint activeControl);
    QStyle::ScrollControl scrollBarPointOver(const QScrollBar *, int, const QPoint& p);

    void drawTab(QPainter *p, const QTabBar *tabbar, QTab *tab, bool selected);

    void drawSlider(QPainter *p, int x, int y, int w, int h,
                    const QColorGroup &g, Qt::Orientation orientation,
                    bool, bool);
    void drawSliderGroove(QPainter *p, int x, int y, int w, int h,
                          const QColorGroup& g, QCOORD,
			  Qt::Orientation );

    void drawToolBarHandle(QPainter *, const QRect &, Qt::Orientation,
			   bool, const QColorGroup &, bool = FALSE );

    QSize scrollBarExtent() const;
    int buttonDefaultIndicatorWidth() const;
    int buttonMargin() const;
    int sliderThickness() const;
    int sliderLength() const;
    int defaultFrameWidth() const;

    int extraPopupMenuItemWidth( bool checkable, int maxpmw, QMenuItem*, const QFontMetrics& );
    int popupMenuItemHeight( bool checkable, QMenuItem*, const QFontMetrics& );
    void drawPopupMenuItem( QPainter* p, bool checkable, int maxpmw, int tab, QMenuItem* mi,
				       const QPalette& pal,
				       bool act, bool enabled, int x, int y, int w, int h);

protected:
    bool eventFilter(QObject *, QEvent *);
};

#endif

#endif // LIGHTSTYLE_H
