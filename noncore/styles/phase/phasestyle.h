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

#ifndef PHASESTYLE_H
#define PHASESTYLE_H

#include <qstyle.h>
#include <qwindowsstyle.h>
#include <qcolor.h>

class KPixmap;

class PhaseStyle : public QWindowsStyle
{
    Q_OBJECT
public:
    PhaseStyle();
    virtual ~PhaseStyle();

    void polish( QWidget * );
    void polish( QPalette& );
    void polish( QApplication* );


    void drawCheckMark ( QPainter * p, int x, int y, int w,
                         int h, const QColorGroup & g,
                         bool act, bool dis );
    void drawArrow(QPainter *p, Qt::ArrowType type, bool down,
                   int x, int y, int w, int h, const QColorGroup &g,
                   bool enabled=true, const QBrush *fill = 0);

    void drawPushButton(QPushButton* btn, QPainter* p );
    void drawPushButtonLabel(QPushButton* btn, QPainter* p );
    void drawPanel(QPainter* p, int , int, int, int,
                   const QColorGroup&, bool sunken = false,
                   int lineWidth = 1, const QBrush  *b = 0l );
    void drawButton( QPainter*, int x, int y, int w, int h,
                     const QColorGroup&, bool sunken = false,
                     const QBrush* fill = 0);
    void drawBevelButton(QPainter*, int, int, int, int,
                         const QColorGroup&, bool sunken = false,
                         const QBrush* fill = 0 );
    void drawFocusRect( QPainter* p, const QRect& r,
                        const QColorGroup&, const QColor* = 0,
                        bool atBorder = false );
    void drawButtonMask( QPainter* p, int x, int y,
                         int w, int h );


    /* Menu Stuff */
    void drawSeperator( QPainter* p, int x, int y, int w,
                        int h, const QColorGroup& group,  bool sunken = true,
                        int lineWidth = 1, int midLineWidtth = 0 );
    void drawMenuBarItem(QPainter* p, int x, int y, int w, int h,
                         QMenuItem *mi, QColorGroup& g, bool enabled,
                         bool act);

    /* RadioButton, CheckBox... */
    void drawIndicator(QPainter* p, int x, int y, int w, int h,
                       const QColorGroup &g, int state, bool down = FALSE,
                       bool enabled = TRUE );
    void drawExclusiveIndicator( QPainter*, int, int, int, int,
                                 const QColorGroup&, bool on,
                                 bool down = false,bool enabled = true );
    void drawExclusiveIndicatorMask (QPainter*, int, int, int, int, bool );

    /* spacing,dimensions */
    int defaultFrameWidth () const;
    int popupMenuItemHeight ( bool checkable,
                              QMenuItem * mi,
                              const QFontMetrics & fm );
    int extraPopupMenuItemWidth(bool checkable, int maxpmw,
                                QMenuItem* mi, const QFontMetrics& fm );

    QSize indicatorSize()const;
    QSize exclusiveIndicatorSize()const;
    void getButtonShift( int &x, int &y );

    /* popup drawing */
    void drawPopupMenuItem ( QPainter * p, bool checkable, int maxpmw,
                             int tab, QMenuItem * mi,
                             const QPalette & pal, bool act,
                             bool enabled, int x, int y,
                             int w, int h );

    /* combo box */
    QRect comboButtonRect ( int x, int y, int w, int h );
    void drawComboButton( QPainter * p, int x, int y,
                          int w, int h,
                          const QColorGroup & g,
                          bool sunken = FALSE,
                          bool editable = FALSE,
                          bool enabled = TRUE,
                          const QBrush * fill = 0 );

    /* tabbar */
    void drawTab(QPainter*, const QTabBar*, QTab*,
                 bool selected );
    void drawTabMask( QPainter*, const QTabBar*, QTab*,
                      bool );
    void tabbarMetrics( const QTabBar* t, int &hframe, int &vframe, int &overlap);

    /* tool button */
    void drawToolButton ( QPainter * p, int x, int y,
                          int w, int h,
                          const QColorGroup & g,
                          bool sunken = FALSE,
                          const QBrush * fill = 0 );

    /* scrollbar */
    void drawScrollBarControls ( QPainter *, const QScrollBar *,
                                 int sliderStart, uint controls,
                                 uint activeControl );
    void scrollBarMetrics( const QScrollBar*, int&, int&, int&, int& );
    QStyle::ScrollControl scrollBarPointOver( const QScrollBar*,  int sliderStart,
                                              const QPoint& );

    /* slider drawing */
    void drawSlider ( QPainter * p, int x, int y, int w, int h,
                      const QColorGroup & g, Orientation,
                      bool tickAbove, bool tickBelow );

    void drawSliderGroove ( QPainter * p, int x, int y, int w,
                            int h, const QColorGroup & g,
                            QCOORD c, Orientation );
private:
    void drawPhaseGradient(QPainter* p,
                           const QRect& rec,
                           const QColor& col )const;
    static void scrollBarItemPositions( const QScrollBar *,
                                        const bool horizontal,
                                        int sliderStart,
                                        int sliderMax,
                                        int sliderLength,
                                        int buttomDim,
                                        QRect& sub,
                                        QRect& add,
                                        QRect& subPage,
                                        QRect& addPage,
                                        QRect& slider );
};


#endif
