#ifndef LIQUID_STYLE_H
#define LIQUID_STYLE_H


#include <qwindowsstyle.h>
#include <qpainter.h>
#include <qdrawutil.h>
#include <qpalette.h>
#include <qwidget.h>
#include <qpushbutton.h>
#include <qscrollbar.h>
#include <qbitmap.h>
#include <qintdict.h>
#include <qstrlist.h>
#include <qobject.h>
#include <limits.h>


/*-
 * Copyright (C)QPL 2001 Daniel M. Duley. All rights reserved.
 */

class QPixmap;

#define BITMAP_ITEMS 41
#define LIQUID_MENU_CHANGE 667
#define MOSFET_BUTTON_CHANGE 661

enum BitmapData{RadioOn=0, RadioOff, RadioOnHover, RadioOffHover, VSBSliderTop,
VSBSliderMid, VSBSliderBtm, VSBSliderTopHover, VSBSliderMidHover,
VSBSliderBtmHover, VSBSliderTopBg, VSBSliderMidBg, VSBSliderBtmBg,
HSBSliderTop, HSBSliderMid, HSBSliderBtm, HSBSliderTopHover, HSBSliderMidHover,
HSBSliderBtmHover, HSBSliderTopBg, HSBSliderMidBg,
HSBSliderBtmBg, Tab, TabDown, TabFocus, CB, CBDown, CBHover, CBDownHover,
HSlider, VSlider, HTMLBtnBorder, HTMLBtnBorderDown, HTMLCB, HTMLCBDown,
HTMLCBHover, HTMLCBDownHover, HTMLRadio, HTMLRadioDown, HTMLRadioHover,
HTMLRadioDownHover};


class QHeader;

enum TransType{None=0, StippledBg, StippledBtn, TransStippleBg,
    TransStippleBtn, Custom};

class TransMenuHandler : public QObject
{
    Q_OBJECT
public:
    TransMenuHandler(QObject *parent);
    ~TransMenuHandler(){;}
    void reloadSettings();
    int transType(){return(type);}
    QPixmap *pixmap(WId id){return(pixDict.find(id));}
    const QColor& textColor(){return(fgColor);}
    const QColor& bgColor(){return(color);}
    bool useShadowText(){return(shadowText);}
protected:
    bool eventFilter(QObject *obj, QEvent *ev);
    void stripePixmap(QPixmap &pix, const QColor &color);

    QColor color, fgColor;
    int opacity;
    int type;
    bool shadowText;
    QIntDict<QPixmap>pixDict;
};

class LiquidStyle : public QWindowsStyle
{
    friend class TransMenuHandler;
public:
    LiquidStyle();
    ~LiquidStyle();
    virtual void polish(QWidget *w);
    virtual void unPolish(QWidget *w);
    virtual void polish(QApplication *a);
    virtual void unPolish(QApplication *a);
    virtual void polish(QPalette &p);
    void drawButton(QPainter *p, int x, int y, int w, int h,
                    const QColorGroup &g, bool sunken = FALSE,
                    const QBrush *fill = 0 );
    void drawButtonMask(QPainter *p, int x, int y, int w, int h);
    void drawComboButtonMask(QPainter *p, int x, int y, int w, int h);
    QRect buttonRect(int x, int y, int w, int h);
    void drawBevelButton(QPainter *p, int x, int y, int w, int h,
                         const QColorGroup &g, bool sunken = FALSE,
                         const QBrush *fill = 0 );
    void drawPushButton(QPushButton *btn, QPainter *p);
    virtual void drawPushButtonLabel (QPushButton *btn, QPainter *p);
    void drawScrollBarControls(QPainter*,  const QScrollBar*, int sliderStart,
                               uint controls, uint activeControl );
    QStyle::ScrollControl scrollBarPointOver(const QScrollBar *sb,
                                             int sliderStart, const QPoint &p);
    void scrollBarMetrics(const QScrollBar *sb, int &sliderMin, int &sliderMax,
                          int &sliderLength, int &buttonDim);
    QSize indicatorSize() const;
    void drawIndicator(QPainter* p, int x, int y, int w, int h,
                       const QColorGroup &g, int state, bool down = FALSE,
                       bool enabled = TRUE );
    void drawIndicatorMask(QPainter *p, int x, int y, int w, int h, int);
    QSize exclusiveIndicatorSize() const;
    void drawExclusiveIndicator(QPainter* p,  int x, int y, int w, int h,
                                const QColorGroup &g, bool on,
                                bool down = FALSE, bool enabled =  TRUE );
    void drawExclusiveIndicatorMask(QPainter *p, int x, int y, int w,
                                    int h, bool);
    void drawComboButton(QPainter *p, int x, int y, int w, int h,
                         const QColorGroup &g, bool sunken = FALSE,
                         bool editable = FALSE, bool enabled = TRUE,
                         const QBrush *fill = 0 );
    QRect comboButtonRect(int x, int y, int w, int h);
    QRect comboButtonFocusRect(int x, int y, int w, int h);
    int sliderLength() const;
    void drawArrow(QPainter *p, Qt::ArrowType type, bool down,
                   int x, int y, int w, int h, const QColorGroup &g,
                   bool enabled=true, const QBrush *fill = 0);
    void drawSlider(QPainter *p, int x, int y, int w, int h,
                    const QColorGroup &g, Orientation orient,
                    bool tickAbove, bool tickBelow);
    void drawSliderMask(QPainter *p, int x, int y, int w, int h,
                        Orientation orient, bool, bool);
	void drawMenuBarItem(QPainter *p, int x, int y, int w, int h,
                            QMenuItem *mi, QColorGroup &g, bool enabled, bool active );

    void drawPopupMenuItem(QPainter *p, bool checkable, int maxpmw,
                           int tab, QMenuItem *mi, const QPalette &pal,
                           bool act, bool enabled, int x, int y, int w,
                           int h);
    int popupMenuItemHeight(bool c, QMenuItem *mi, const QFontMetrics &fm);
    void drawFocusRect(QPainter *p, const QRect &r, const QColorGroup &g,
                       const QColor *pen, bool atBorder);
    int defaultFrameWidth() const {return(2);}
    void polishPopupMenu(QPopupMenu *mnu);
    void drawTab(QPainter *p, const QTabBar *tabBar, QTab *tab,
                 bool selected);
    void drawTabMask( QPainter*, const QTabBar*, QTab*, bool selected );
    void tabbarMetrics(const QTabBar *t, int &hFrame, int &vFrame,
                       int &overlap);

    void drawSplitter(QPainter *p, int x, int y, int w, int h,
                      const QColorGroup &g, Orientation orient);
    int splitterWidth() const {return(6);}
    void drawPanel(QPainter *p, int x, int y, int w, int h,
                   const QColorGroup &g, bool sunken, int lineWidth,
                   const QBrush *fill);
    void drawToolButton(QPainter *p, int x, int y, int w,
                        int h, const QColorGroup &g,
                        bool sunken, const QBrush *fill);
    // for repainting toolbuttons when the toolbar is resized
    bool eventFilter(QObject *obj, QEvent *ev);
    void drawSliderGroove(QPainter * p, int x, int y, int w,  int h,
                          const QColorGroup &g, QCOORD c, Orientation);
    void drawSliderGrooveMask(QPainter * p, int x, int y, int w, int h,
                              QCOORD c, Orientation);
    int buttonDefaultIndicatorWidth() const {return(2);}
    void drawPopupPanel(QPainter *p, int x, int y, int w, int h,
                        const QColorGroup &g, int lineWidth,
                        const QBrush * fill);
protected:
    void adjustHSV(QPixmap &pix, int h, int s, int v);
    void intensity(QPixmap &pix, float percent);

    void drawRoundButton(QPainter *p, const QColor &c, const QColor &bg, int x,
                         int y, int w, int h, bool  supportPushDown = false,
                         bool pushedDown = false, bool autoDefault = false,
                         bool isMasked = false);
    void drawClearBevel(QPainter *p, int x, int y, int w, int h,
                        const QColor &c, const QColor &bg);
    void drawSBDeco(QPainter *p, const QRect &r, const QColorGroup &g,
                   bool horiz);
    void drawSBButton(QPainter *p, const QRect &r, const QColorGroup &g,
                      bool down=false, bool fast = true);
    void drawRoundRect(QPainter *p, int x, int y, int w, int h);
    QPixmap* getPixmap(BitmapData item);
    QPixmap* processEmbedded(const char *label, int h, int s, int v, bool blend=false);
private:
	bool oldqte;
	bool flatTBButtons;
    QWidget *highlightWidget;
    QBitmap btnMaskBmp, htmlBtnMaskBmp;
    QPixmap *btnBorderPix, *btnBlendPix, *bevelFillPix, *smallBevelFillPix, *menuPix;
    QBrush bgBrush, menuBrush;
    bool menuAni, menuFade;

    QIntDict<QPixmap>btnDict;
    QIntDict<QPixmap>btnBorderDict;
    QIntDict<QPixmap>bevelFillDict;
    QIntDict<QPixmap>smallBevelFillDict;

    QPixmap *vsbSliderFillPix;
    TransMenuHandler *menuHandler;
    QPixmap *pixmaps[BITMAP_ITEMS];
    QPixmap sbBuffer;
    int oldSliderThickness;
    int lowLightVal;
    QHeader *currentHeader;
    int headerHoverID;
    QWMatrix rMatrix;
    int bH, bS, bV;
    int bHoverH, bHoverS, bHoverV;
    int btnH, btnS, btnV;
    int btnHoverH, btnHoverS, btnHoverV;
};

#endif
