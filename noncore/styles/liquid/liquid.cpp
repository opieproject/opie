/*-
 * Copyright (C)QPL 2001 Daniel M. Duley. All rights reserved.
 */



#ifndef INCLUDE_MENUITEM_DEF
#define INCLUDE_MENUITEM_DEF
#endif

#include <qmenudata.h>
#include "liquid.h"
#include "liquiddeco.h"
#include <qapplication.h>
#include <qpe/config.h>
#include "effects.h"
#include <qpalette.h>
#include <qbitmap.h>
#include <qtabbar.h>
#include <qpopupmenu.h>
#include <qobjectlist.h>
#include <qimage.h>
#include <qtimer.h>
#include <qpixmapcache.h>
#include <qradiobutton.h>
#include <qcombobox.h>
#include <qdrawutil.h>
#include <qwidgetlist.h>
#include <qtoolbutton.h>
#include <qheader.h>
#include <unistd.h>
#include <qmenubar.h>


#include <stdio.h>

#include "htmlmasks.h"
#include "embeddata.h"

typedef void (QStyle::*QDrawMenuBarItemImpl) (QPainter *, int, int, int, int, QMenuItem *,
                          QColorGroup &, bool, bool);

QDrawMenuBarItemImpl qt_set_draw_menu_bar_impl(QDrawMenuBarItemImpl impl);

void TransMenuHandler::stripePixmap(QPixmap &pix, const QColor &color)
{
    QImage img(pix.convertToImage());
    QImageEffect::fade(img, 0.9, color);
    int x, y;
    int r, g, b;
    for(y=0; y < img.height(); y+=3){
        unsigned int *data = (unsigned int  *) img.scanLine(y);
        for(x=0; x < img.width(); ++x){
            r = qRed(data[x]);
            g = qGreen(data[x]);
            b = qBlue(data[x]);
            if(r-10)
                r-=10;
            if(g-10)
                g-=10;
            if(b-10)
                b-=10;
            data[x] = qRgb(r, g, b);
        }
    }
    pix.convertFromImage(img);
}

TransMenuHandler::TransMenuHandler(QObject *parent)
    : QObject(parent)
{
    pixDict.setAutoDelete(true);
    reloadSettings();
}

void TransMenuHandler::reloadSettings()
{
    pixDict.clear();

    Config config ( "qpe" );
    config. setGroup ( "Liquid-Style" );

    type = config. readNumEntry("Type", TransStippleBg);
    color = QColor ( config. readEntry("Color",  QApplication::palette().active().button().name()));
    fgColor = QColor ( config. readEntry("TextColor", QApplication::palette().active().text().name()));
    opacity = config. readNumEntry("Opacity", 10);
    if ( opacity < -20 )
    	opacity = 20;
    else if ( opacity > 20 )
    	opacity = 20;
    
    shadowText = config. readBoolEntry("ShadowText", true);
}

bool TransMenuHandler::eventFilter(QObject *obj, QEvent *ev)
{
    QWidget *p = (QWidget *)obj;

    if(ev->type() == QEvent::Show){
        if(type == TransStippleBg || type == TransStippleBtn ||
           type == Custom){
            QApplication::syncX();
            QPixmap *pix = new QPixmap;
            if(p->testWFlags(Qt::WType_Popup)){
                QRect r(p->x(), p->y(), p->width(), p->height());
                QRect deskR = QApplication::desktop()->rect();
                if(r.right() > deskR.right() || r.bottom() > deskR.bottom()){
                    r.setBottom(deskR.bottom());
                    r.setRight(deskR.right());
                }
                *pix = QPixmap::grabWindow(QApplication::desktop()-> winId(), r.x(), r.y(),
                                           r.width(), r.height());
            }
            else{ // tear off menu
                pix->resize(p->width(), p->height());
                pix->fill(Qt::black.rgb());
            }
            if(type == TransStippleBg){
                stripePixmap(*pix, p->colorGroup().background());
            }
            else if(type == TransStippleBtn){
                stripePixmap(*pix, p->colorGroup().button());
            }
            else{
                QPixmapEffect::fade(*pix, (((float)opacity)+80)*0.01, color);
            }

            pixDict.insert(p->winId(), pix);
            
            if (!p->inherits("QPopupMenu")) {
            	p->setBackgroundPixmap(*pix);
            	
            	QObjectList *ol = p-> queryList("QWidget");
				for ( QObjectListIt it( *ol ); it. current ( ); ++it ) {
					QWidget *wid = (QWidget *) it.current ( );
					
					wid-> setBackgroundPixmap(*pix);
			    	wid-> setBackgroundOrigin(QWidget::ParentOrigin);					
                }
				delete ol;
   			}
        }
    }
    else if(ev->type() == QEvent::Hide){
        if(type == TransStippleBg || type == TransStippleBtn ||
           type == Custom){
//            qWarning("Deleting menu pixmap, width %d", pixDict.find(p->winId())->width());

            pixDict.remove(p->winId());
            if (!p->inherits("QPopupMenu")) {
                p->setBackgroundMode(QWidget::PaletteBackground);

            	QObjectList *ol = p-> queryList("QWidget");
				for ( QObjectListIt it( *ol ); it. current ( ); ++it ) {
					QWidget *wid = (QWidget *) it.current ( );
					
					wid-> setBackgroundMode( QWidget::PaletteBackground );
                }
				delete ol;
            }
        }
    }
    return(false);
}


LiquidStyle::LiquidStyle()
    :QWindowsStyle()
{
    setName ( "LiquidStyle" );

	flatTBButtons = false;

    btnMaskBmp = QBitmap(37, 26, buttonmask_bits, true);
    btnMaskBmp.setMask(btnMaskBmp);
    htmlBtnMaskBmp = QBitmap(37, 26, htmlbuttonmask_bits, true);
    htmlBtnMaskBmp.setMask(htmlBtnMaskBmp);
    headerHoverID = -1;
    highlightWidget = NULL;
    setButtonDefaultIndicatorWidth(0);
    btnDict.setAutoDelete(true);
    bevelFillDict.setAutoDelete(true);
    smallBevelFillDict.setAutoDelete(true);
    customBtnColorList.setAutoDelete(true);
    customBtnIconList.setAutoDelete(true);
    customBtnLabelList.setAutoDelete(true);

    rMatrix.rotate(270.0);
    highcolor = QPixmap::defaultDepth() > 8;
    btnBorderPix = new QPixmap;
    btnBorderPix->convertFromImage(qembed_findImage("buttonfill"));
    btnBlendPix = new QPixmap;
    btnBlendPix->convertFromImage(qembed_findImage("buttonborder"));
    bevelFillPix = new QPixmap;
    bevelFillPix->convertFromImage(qembed_findImage("clear_fill_large"));
    smallBevelFillPix = new QPixmap;
    smallBevelFillPix->convertFromImage(qembed_findImage("clear_fill_small"));
    // new stuff
    vsbSliderFillPix = menuPix = NULL;
    menuHandler = new TransMenuHandler(this);
    setScrollBarExtent(15, 15);
    int i;
    for(i=0; i < BITMAP_ITEMS; ++i){
        pixmaps[i] = NULL;
    }
    oldSliderThickness = sliderThickness();
    setSliderThickness(11);
}

LiquidStyle::~LiquidStyle()
{
    if(btnBorderPix)
        delete btnBorderPix;
    if(btnBlendPix)
        delete btnBlendPix;
    if(bevelFillPix)
        delete bevelFillPix;
    if(smallBevelFillPix)
        delete smallBevelFillPix;
    if(vsbSliderFillPix)
        delete vsbSliderFillPix;
    if(menuPix)
        delete menuPix;

    setScrollBarExtent(16, 16);
    setSliderThickness(oldSliderThickness);
    int i;
    for(i=0; i < BITMAP_ITEMS; ++i){
        if(pixmaps[i])
            delete pixmaps[i];
    }
}

void LiquidStyle::drawClearBevel(QPainter *p, int x, int y, int w, int h,
                                const QColor &c, const QColor &bg)
{

    QPen oldPen = p->pen(); // headers need this
    int x2 = x+w-1;
    int y2 = y+h-1;
    // outer dark rect
    p->setPen(c.dark(130));
    p->drawLine(x, y+2, x, y2-2); // l
    p->drawLine(x2, y+2, x2, y2-2); // r
    p->drawLine(x+2, y, x2-2, y); // t
    p->drawLine(x+2, y2, x2-2, y2); // b
    p->drawPoint(x+1, y+1); // tl
    p->drawPoint(x2-1, y+1); // tr
    p->drawPoint(x+1, y2-1); // bl
    p->drawPoint(x2-1, y2-1); // br

    // inner top light lines
    p->setPen(c.light(105));
    p->drawLine(x+2, y+1, x2-2, y+1);
    p->drawLine(x+1, y+2, x2-1, y+2);
    p->drawLine(x+1, y+3, x+2, y+3);
    p->drawLine(x2-2, y+3, x2-1, y+3);
    p->drawPoint(x+1, y+4);
    p->drawPoint(x2-1, y+4);

    // inner bottom light lines
    p->setPen(c.light(110));
    p->drawLine(x+2, y2-1, x2-2, y2-1);
    p->drawLine(x+1, y2-2, x2-1, y2-2);
    p->drawLine(x+1, y2-3, x+2, y2-3);
    p->drawLine(x2-2, y2-3, x2-1, y2-3);
    p->drawPoint(x+1, y2-4);
    p->drawPoint(x2-1, y2-4);

    // inner left mid lines
    //p->setPen(c.light(105));
    p->setPen(c);
    p->drawLine(x+1, y+5, x+1, y2-5);
    p->drawLine(x+2, y+4, x+2, y2-4);

    // inner right mid lines
    p->drawLine(x2-1, y+5, x2-1, y2-5);
    p->drawLine(x2-2, y+4, x2-2, y2-4);

    // fill
    QPixmap *pix;
    if(h >= 32){
        pix = bevelFillDict.find(c.rgb());
        if(!pix){
            int h, s, v;
            c.hsv(&h, &s, &v);
            pix = new QPixmap(*bevelFillPix);
            adjustHSV(*pix, h, s, v);
            bevelFillDict.insert(c.rgb(), pix);
        }
    }
    else{
        pix = smallBevelFillDict.find(c.rgb());
        if(!pix){
            int h, s, v;
            c.hsv(&h, &s, &v);
            pix = new QPixmap(*smallBevelFillPix);
            adjustHSV(*pix, h, s, v);
            smallBevelFillDict.insert(c.rgb(), pix);
        }
    }
    p->drawTiledPixmap(x+3, y+3, w-6, h-6, *pix);
    // blend
    int red, green, blue;
    QColor btnColor(c.dark(130));
    red = (btnColor.red() >> 1) + (bg.red() >> 1);
    green = (btnColor.green() >> 1) + (bg.green() >> 1);
    blue = (btnColor.blue() >> 1) + (bg.blue() >> 1);
    btnColor.setRgb(red, green, blue);

    p->setPen(btnColor);
    p->drawPoint(x+1, y);
    p->drawPoint(x, y+1);
    p->drawPoint(x+1, y2);
    p->drawPoint(x, y2-1);

    p->drawPoint(x2-1, y);
    p->drawPoint(x2, y+1);
    p->drawPoint(x2-1, y2);
    p->drawPoint(x2, y2-1);

    p->setPen(oldPen);

}

void LiquidStyle::drawRoundButton(QPainter *painter, const QColor &c,
                                  const QColor &back, int x, int y, int w, int h,
                                  bool supportPushDown, bool pushedDown,
                                  bool autoDefault, bool isMasked)
{
    if(w < 21 || h < 21){
        drawClearBevel(painter, x, y, w, h, c, back);
        return;
    }
    if(supportPushDown){
        --w, --h;
    }
    /* We don't make the round buttons smaller, since they don't look as good
    if(autoDefault){
        w = w-buttonDefaultIndicatorWidth()*2;
        h = h-buttonDefaultIndicatorWidth()*2;
    }*/


    QPixmap *pix = btnDict.find(c.rgb());
    if(!pix){
        int h, s, v;
        c.hsv(&h, &s, &v);
        pix = new QPixmap(*btnBorderPix);
        adjustHSV(*pix, h, s, v);
        btnDict.insert(c.rgb(), pix);
    }
    int x2 = x+w-1;
    int y2 = y+h-1;
    int bx2 = pix->width()-1;
    int by2 = pix->height()-1;

    QPixmap tmpPix(w, h);
    QPixmap tilePix;
    QPainter p;
    p.begin(&tmpPix);

    // do the fill
    p.drawPixmap(0, 0, *pix, 0, 0, 10, 10); // tl
    p.drawPixmap(x2-9, 0, *pix, bx2-9, 0, 10, 10); // tr
    p.drawPixmap(0, y2-9, *pix, 0, by2-9, 10, 10); // bl
    p.drawPixmap(x2-9, y2-9, *pix, bx2-9, by2-9, 10, 10); // br

    // edges
    tilePix.resize(pix->width()-20, 10);
    // top
    bitBlt(&tilePix, 0, 0, pix, 10, 0, pix->width()-20, 10);
    p.drawTiledPixmap(10, 0, w-20, 10, tilePix);
    // bottom
    bitBlt(&tilePix, 0, 0, pix, 10, by2-9, pix->width()-20, 20);
    p.drawTiledPixmap(10, y2-9, w-20, 10, tilePix);
    // left
    tilePix.resize(10, pix->height()-20);
    bitBlt(&tilePix, 0, 0, pix, 0, 10, 10, pix->height()-20);
    p.drawTiledPixmap(0, 10, 10, h-20, tilePix);
    // right
    bitBlt(&tilePix, 0, 0, pix, bx2-9, 10, 10, pix->height()-20);
    p.drawTiledPixmap(x2-9, 10, 10, h-20, tilePix);

    // middle
    tilePix.resize(pix->width()-20, pix->height()-20);
    bitBlt(&tilePix, 0, 0, pix, 10, 10, pix->width()-20, pix->height()-20);
    p.drawTiledPixmap(10, 10, w-20, h-20, tilePix);


    // do the blend
    QBitmap blendMask;
    if(!isMasked){
        //QRgb bgRgb = back.rgb();
        //QRgb fgRgb = c.rgb();
        //int r, g, b;

        //r = (int)(qRed(bgRgb)*0.75 + qRed(fgRgb)*0.25);
        //g = (int)(qGreen(bgRgb)*0.75 + qGreen(fgRgb)*0.25);
        //b = (int)(qBlue(bgRgb)*0.75 + qBlue(fgRgb)*0.25);
        //QColor blendColor(r, g, b);
        int hue, sat, v1, v2;
        QColor blendColor(autoDefault ? c : back);
        back.hsv(&hue, &sat, &v1);
        blendColor.hsv(&hue, &sat, &v2);
        if(v2 > v1)
            blendColor.setHsv(hue, sat, (int)(v1*0.75 + v2*0.25));

        pix = btnBorderDict.find(blendColor.rgb());
        if(!pix){
            int h, s, v;
            blendColor.hsv(&h, &s, &v);
            pix = new QPixmap(*btnBlendPix);
            adjustHSV(*pix, h, s, v);
            btnBorderDict.insert(blendColor.rgb(), pix);
        }
    }
    else{
        pix = pushedDown ? getPixmap(HTMLBtnBorderDown) :
            getPixmap(HTMLBtnBorder);
    }
    p.drawPixmap(0, 0, *pix, 0, 0, 10, 10); // tl
    p.drawPixmap(x2-9, 0, *pix, bx2-9, 0, 10, 10); // tr
    p.drawPixmap(0, y2-9, *pix, 0, by2-9, 10, 10); // bl
    p.drawPixmap(x2-9, y2-9, *pix, bx2-9, by2-9, 10, 10); // br

    // edges
    tilePix.resize(pix->width()-20, 10);
    blendMask.resize(pix->width()-20, 10);
    // top
    bitBlt(&tilePix, 0, 0, pix, 10, 0, pix->width()-20, 10);
    bitBlt(&blendMask, 0, 0, pix->mask(), 10, 0, pix->width()-20, 10);
    tilePix.setMask(blendMask);
    p.drawTiledPixmap(10, 0, w-20, 10, tilePix);
    // bottom
    bitBlt(&tilePix, 0, 0, pix, 10, by2-9, pix->width()-20, 20);
    bitBlt(&blendMask, 0, 0, pix->mask(), 10, by2-9, pix->width()-20, 20);
    tilePix.setMask(blendMask);
    p.drawTiledPixmap(10, y2-9, w-20, 10, tilePix);
    // left
    tilePix.resize(10, pix->height()-20);
    blendMask.resize(10, pix->height()-20);
    bitBlt(&tilePix, 0, 0, pix, 0, 10, 10, pix->height()-20);
    bitBlt(&blendMask, 0, 0, pix->mask(), 0, 10, 10, pix->height()-20);
    tilePix.setMask(blendMask);
    p.drawTiledPixmap(0, 10, 10, h-20, tilePix);
    // right
    bitBlt(&tilePix, 0, 0, pix, bx2-9, 10, 10, pix->height()-20);
    bitBlt(&blendMask, 0, 0, pix->mask(), bx2-9, 10, 10, pix->height()-20);
    tilePix.setMask(blendMask);
    p.drawTiledPixmap(x2-9, 10, 10, h-20, tilePix);

    p.end();

    // do the button mask - we don't automask buttons
    QBitmap btnMask(w, h);
    QBitmap *mask = isMasked ? &htmlBtnMaskBmp : &btnMaskBmp;
    p.begin(&btnMask);
    p.fillRect(0, 0, w, h, Qt::color0);
    p.drawPixmap(0, 0, *mask, 0, 0, 10, 10); // tl
    p.drawPixmap(x2-9, 0, *mask, bx2-9, 0, 10, 10); // tr
    p.drawPixmap(0, y2-9, *mask, 0, by2-9, 10, 10); // bl
    p.drawPixmap(x2-9, y2-9, *mask, bx2-9, by2-9, 10, 10); // br
    // fills
    p.fillRect(10, 0, w-20, 10, Qt::color1); // top
    p.fillRect(10, y2-9, w-20, 10, Qt::color1); // bottom
    p.fillRect(0, 10, w, h-20, Qt::color1); // middle
    p.end();
    tmpPix.setMask(btnMask);
    /*if(autoDefault){
        if(supportPushDown && pushedDown){
            painter->drawPixmap(x+3, y+3, tmpPix);
        }
        else{
            painter->drawPixmap(x+2, y+2, tmpPix);
        }
    }
    else */if(supportPushDown && pushedDown)
        painter->drawPixmap(x+1, y+1, tmpPix);
    else
        painter->drawPixmap(x, y, tmpPix);



}


QPixmap* LiquidStyle::processEmbedded(const char *label, int h, int s, int v,
                                      bool blend)
{
    QImage img(qembed_findImage(label));
    img.detach();
    if(img.isNull()){ // shouldn't happen, been tested
        qWarning("Invalid embedded label %s", label);
        return(NULL);
    }
    if(img.depth() != 32)
        img = img.convertDepth(32);
    unsigned int *data = (unsigned int *)img.bits();
    int total = img.width()*img.height();
    int current;
    QColor c;
    int oldH, oldS, oldV;
    int alpha;
    if(v < 235)
        v += 20;
    else
        v = 255;
    float intensity = v/255.0;

    for(current=0; current<total; ++current){
        alpha = qAlpha(data[current]);
        c.setRgb(data[current]);
        c.hsv(&oldH, &oldS, &oldV);
        oldV = (int)(oldV*intensity);
        c.setHsv(h, s, oldV);
        if(blend && alpha != 255 && alpha != 0){
            float srcPercent = ((float)alpha)/255.0;
            float destPercent = 1.0-srcPercent;
            oldH = (int)((srcPercent*h) + (destPercent*bH));
            oldS = (int)((srcPercent*s) + (destPercent*bS));
            oldV = (int)((srcPercent*oldV) + (destPercent*bV));
            c.setHsv(oldH, oldS, oldV);
            alpha = 255;
        }
        data[current] = qRgba(c.red(), c.green(), c.blue(), alpha);
    }
    QPixmap *pix = new QPixmap;
    pix->convertFromImage(img);
    return(pix);
}




QPixmap* LiquidStyle::getPixmap(BitmapData item)
{

    if(pixmaps[item])
        return(pixmaps[item]);

    switch(item){
    case HTMLBtnBorder:
        pixmaps[HTMLBtnBorder] = processEmbedded("htmlbtnborder", btnH, btnS, btnV);
        break;
    case HTMLBtnBorderDown:
        pixmaps[HTMLBtnBorderDown] = processEmbedded("htmlbtnborder", btnHoverH, btnHoverS, btnHoverV);
        break;
    case HTMLCB:
        pixmaps[HTMLCB] = processEmbedded("htmlcheckbox", bH, bS, bV);
        break;
    case HTMLCBDown:
        pixmaps[HTMLCBDown] = processEmbedded("htmlcheckboxdown", btnH, btnS, btnV);
        break;
    case HTMLCBHover:
        pixmaps[HTMLCBHover] = processEmbedded("htmlcheckbox", btnH, btnS, btnV);
        break;
    case HTMLCBDownHover:
        pixmaps[HTMLCBDownHover] = processEmbedded("htmlcheckboxdown",
                                                   btnHoverH, btnHoverS,
                                                   btnHoverV);
        break;
    case HTMLRadio:
        pixmaps[HTMLRadio] = processEmbedded("htmlradio", bH, bS, bV);
    case HTMLRadioDown:
        pixmaps[HTMLRadioDown] = processEmbedded("htmlradiodown", btnH, btnS, btnV);
    case HTMLRadioHover:
        pixmaps[HTMLRadioHover] = processEmbedded("htmlradio", btnH, btnS, btnV);
    case HTMLRadioDownHover:
        pixmaps[HTMLRadioDownHover] = processEmbedded("htmlradiodown", 
                                                      btnHoverH, btnHoverS,
                                                      btnHoverV);
    case RadioOn:
        pixmaps[RadioOn] = processEmbedded("radio_down", btnH, btnS, btnV, true);
        break;
    case RadioOnHover:
        pixmaps[RadioOnHover] = processEmbedded("radio_down", btnHoverH, btnHoverS,
                                                btnHoverV, true);
        break;
    case RadioOffHover:
        pixmaps[RadioOffHover] = processEmbedded("radio", btnH, btnS, btnV, true);
        break;
    case TabDown:
        pixmaps[TabDown] = processEmbedded("tab", btnH, btnS, btnV, true);
        break;
    case TabFocus:
        pixmaps[TabFocus] = processEmbedded("tab", btnHoverH, btnHoverS,
                                            btnHoverS, true);
        break;
    case CBDown:
        pixmaps[CBDown] = processEmbedded("checkboxdown", btnH, btnS, btnV, true);
        break;
    case CBDownHover:
        pixmaps[CBDownHover] = processEmbedded("checkboxdown", btnHoverH,
                                               btnHoverS, btnHoverV, true);
        break;
    case CBHover:
        pixmaps[CBHover] = processEmbedded("checkbox", btnH, btnS, btnV, true);
        break;
    case HSlider:
        pixmaps[HSlider] = processEmbedded("sliderarrow", btnH, btnS, btnV, true);
        break;
    case VSlider:
        pixmaps[VSlider] = processEmbedded("sliderarrow", btnH, btnS, btnV, true);
        *pixmaps[VSlider] = pixmaps[VSlider]->xForm(rMatrix);
        break;
    case RadioOff:
        pixmaps[RadioOff] = processEmbedded("radio", bH, bS, bV, true);
        break;
    case Tab:
        pixmaps[Tab] = processEmbedded("tab", bH, bS, bV, true);
        break;
    case CB:
        pixmaps[CB] = processEmbedded("checkbox", bH, bS, bV, true);
        break;
    case VSBSliderTop:
        pixmaps[VSBSliderTop] = processEmbedded("sbslider_top", btnH, btnS, btnV, true);
        break;
    case VSBSliderBtm:
        pixmaps[VSBSliderBtm] = processEmbedded("sbslider_btm", btnH, btnS, btnV, true);
        break;
    case VSBSliderMid:
        pixmaps[VSBSliderMid] = processEmbedded("sbslider_mid", btnH, btnS, btnV);
        break;
    case VSBSliderTopHover:
        pixmaps[VSBSliderTopHover] = processEmbedded("sbslider_top", btnHoverH, btnHoverS, btnHoverV, true);
        break;
    case VSBSliderBtmHover:
        pixmaps[VSBSliderBtmHover] = processEmbedded("sbslider_btm", btnHoverH, btnHoverS, btnHoverV, true);
        break;
    case VSBSliderMidHover:
        pixmaps[VSBSliderMidHover] = processEmbedded("sbslider_mid", btnHoverH, btnHoverS, btnHoverV);
        break;

    case HSBSliderTop:
        pixmaps[HSBSliderTop] = processEmbedded("sbslider_top", btnH, btnS, btnV, true);
        *pixmaps[HSBSliderTop] = pixmaps[HSBSliderTop]->xForm(rMatrix);
        break;
    case HSBSliderBtm:
        pixmaps[HSBSliderBtm] = processEmbedded("sbslider_btm", btnH, btnS, btnV, true);
        *pixmaps[HSBSliderBtm] = pixmaps[HSBSliderBtm]->xForm(rMatrix);
        break;
    case HSBSliderMid:
        pixmaps[HSBSliderMid] = processEmbedded("sbslider_mid", btnH, btnS, btnV);
        *pixmaps[HSBSliderMid] = pixmaps[HSBSliderMid]->xForm(rMatrix);
        break;
    case HSBSliderTopHover:
        pixmaps[HSBSliderTopHover] = processEmbedded("sbslider_top", btnHoverH, btnHoverS, btnHoverV, true);
        *pixmaps[HSBSliderTopHover] = pixmaps[HSBSliderTopHover]->xForm(rMatrix);
        break;
    case HSBSliderBtmHover:
        pixmaps[HSBSliderBtmHover] = processEmbedded("sbslider_btm", btnHoverH, btnHoverS, btnHoverV, true);
        *pixmaps[HSBSliderBtmHover] = pixmaps[HSBSliderBtmHover]->xForm(rMatrix);
        break;
    case HSBSliderMidHover:
        pixmaps[HSBSliderMidHover] = processEmbedded("sbslider_mid", btnHoverH, btnHoverS, btnHoverV);
        *pixmaps[HSBSliderMidHover] = pixmaps[HSBSliderMidHover]->xForm(rMatrix);
        break;
    case VSBSliderTopBg:
        pixmaps[VSBSliderTopBg] = processEmbedded("sbslider_top", bH, bS, bV, true);
        break;
    case VSBSliderBtmBg:
        pixmaps[VSBSliderBtmBg] = processEmbedded("sbslider_btm", bH, bS, bV, true);
        break;
    case VSBSliderMidBg:
        pixmaps[VSBSliderMidBg] = processEmbedded("sbslider_mid", bH, bS, bV);
        break;
    case HSBSliderTopBg:
        pixmaps[HSBSliderTopBg] = processEmbedded("sbslider_top", bH, bS, bV, true);
        *pixmaps[HSBSliderTopBg] = pixmaps[HSBSliderTopBg]->xForm(rMatrix);
        break;
    case HSBSliderBtmBg:
        pixmaps[HSBSliderBtmBg] = processEmbedded("sbslider_btm", bH, bS, bV, true);
        *pixmaps[HSBSliderBtmBg] = pixmaps[HSBSliderBtmBg]->xForm(rMatrix);
        break;
    case HSBSliderMidBg:
        pixmaps[HSBSliderMidBg] = processEmbedded("sbslider_mid", bH, bS, bV);
        *pixmaps[HSBSliderMidBg] = pixmaps[HSBSliderMidBg]->xForm(rMatrix);
        break;
    default:
        break;
    }
    return(pixmaps[item]);
}

void LiquidStyle::polish(QPalette &appPal)
{
    int i;
    for(i=0; i < BITMAP_ITEMS; ++i){
        if(pixmaps[i]){
            delete pixmaps[i];
            pixmaps[i] = NULL;
        }
    }
    QWidgetList *list = QApplication::allWidgets();
    QWidgetListIt it( *list );
    QWidget *w;
    while ((w=it.current()) != 0 ){
        ++it;
        if(w->inherits("QPushButton")){
            unapplyCustomAttributes((QPushButton *)w);
        }
    }

    loadCustomButtons();
    lowLightVal = 100 + (2* /*KGlobalSettings::contrast()*/ 3 +4)*10;
    btnDict.clear();
    btnBorderDict.clear();
    bevelFillDict.clear();
    smallBevelFillDict.clear();

    Config config ( "qpe" );
    config. setGroup ( "Liquid-Style" );
	int contrast = config. readNumEntry ( "StippleContrast", 5 );
	if ( contrast < 0 )
		contrast = 0;
	else if ( contrast > 10 )
		contrast = 10;

//    QPalette pal = QApplication::palette();

    // button color stuff
    config. setGroup ( "Appearance" );
    QColor c = // QColor ( config. readEntry("Button", ( Qt::lightGray ). name ( )));
               appPal. color ( QPalette::Active, QColorGroup::Button );
    if ( c == appPal. color ( QPalette::Active, QColorGroup::Background )
              //QColor ( config. readEntry ( "background", ( Qt::lightGray ). name ( ))) 
             ) {
        // force button color to be different from background
        QBrush btnBrush(QColor(200, 202, 228));
        appPal.setBrush(QColorGroup::Button, btnBrush);
    }
    c.hsv(&btnH, &btnS, &btnV);
    c.light(120).hsv(&btnHoverH, &btnHoverS, &btnHoverV);

    // menu pixmap
    if(!menuPix){
        menuPix = new QPixmap;
        menuPix->resize(64, 64);
    }
    QPainter painter;
    menuPix->fill(c.rgb());
    painter.begin(menuPix);
    painter.setPen(c.dark(105));
    for(i=0; i < 63; i+=4){
        painter.drawLine(0, i, 63, i);
        painter.drawLine(0, i+1, 63, i+1);
    };
    painter.end();
    menuBrush.setColor(c);
    menuBrush.setPixmap(*menuPix);

    // pager brush
    c = c.dark(120);
    QPixmap *pix = smallBevelFillDict.find(c.rgb()); // better be NULL ;-)
    if(!pix){
        int h, s, v;
        c.hsv(&h, &s, &v);
        pix = new QPixmap(*smallBevelFillPix);
        adjustHSV(*pix, h, s, v);
        smallBevelFillDict.insert(c.rgb(), pix);
    }
    pagerHoverBrush.setColor(c);
    pagerHoverBrush.setPixmap(*pix);

    c = c.dark(120);
    pix = smallBevelFillDict.find(c.rgb()); // better be NULL ;-)
    if(!pix){
        int h, s, v;
        c.hsv(&h, &s, &v);
        pix = new QPixmap(*smallBevelFillPix);
        adjustHSV(*pix, h, s, v);
        smallBevelFillDict.insert(c.rgb(), pix);
    }
    pagerBrush.setColor(c);
    pagerBrush.setPixmap(*pix);

    // background color stuff
    c = /*QColor ( config. readEntry ( "Background", ( Qt::lightGray ).name ( )));*/ appPal. color ( QPalette::Active, QColorGroup::Background );
    c.hsv(&bH, &bS, &bV);
    c.light(120).hsv(&bHoverH, &bHoverS, &bHoverV);

    // FIXME?
    if(vsbSliderFillPix)
        delete vsbSliderFillPix;
    vsbSliderFillPix = new QPixmap(bevelFillPix->xForm(rMatrix));
    adjustHSV(*vsbSliderFillPix, bH, bS, bV);

    // background brush
    QPixmap wallPaper(32, 32);
    wallPaper.fill(c.rgb());
    painter.begin(&wallPaper);
    for(i=0; i < 32; i+=4){
        painter.setPen(c.dark(100 + contrast));
        painter.drawLine(0, i, 32, i);
        painter.setPen(c.dark(100 + 3 * contrast / 5 ) );
        painter.drawLine(0, i+1, 32, i+1);
    };
    painter.end();
    bgBrush.setColor(c);
    bgBrush.setPixmap(wallPaper);
    appPal.setBrush(QColorGroup::Background, bgBrush);

    // lineedits
    c = /*QColor ( config. readEntry("Base", ( Qt::white). name ( )));*/ appPal. color ( QPalette::Active, QColorGroup::Base );
    QPixmap basePix;
    basePix.resize(32, 32);
    basePix.fill(c.rgb());
    painter.begin(&basePix);
    painter.setPen(c.dark(105));
    for(i=0; i < 32; i+=4){
        painter.drawLine(0, i, 32, i);
        painter.drawLine(0, i+1, 32, i+1);
    };
    painter.end();
    baseBrush.setColor(c);
    baseBrush.setPixmap(basePix);
    it.toFirst();
    while ((w=it.current()) != 0 ){
        ++it;
        if(w->inherits("QLineEdit")){
            QPalette pal = w->palette();
            pal.setBrush(QColorGroup::Base, baseBrush);
            w->setPalette(pal);
        }
        else if(w->inherits("QPushButton")){
            applyCustomAttributes((QPushButton *)w);
        }
    }
}

void LiquidStyle::polish(QWidget *w)
{
    if(w->inherits("QMenuBar")){
        //((QFrame*)w)->setLineWidth(0);
        w->setBackgroundMode(QWidget::PaletteBackground);
        w->setBackgroundOrigin(QWidget::ParentOrigin);
        return;
    }
    if(w->inherits("QToolBar")){
        w->installEventFilter(this);
        w->setBackgroundMode(QWidget::PaletteBackground);
        w->setBackgroundOrigin(QWidget::WidgetOrigin);
        return;
    }
    if(w->inherits("QPopupMenu"))
        w->setBackgroundMode(QWidget::NoBackground);
    else if(w-> testWFlags(Qt::WType_Popup) && !w->inherits("QListBox")) {
    	w->installEventFilter(menuHandler);
    }
    
    if(w->isTopLevel()){
        return;
    }
   
    
    if ( !w-> inherits("QFrame") || (((QFrame*) w)-> frameShape () == QFrame::NoFrame ))
	    w-> setBackgroundOrigin ( QWidget::ParentOrigin );

    if(w->inherits("QComboBox") ||
       w->inherits("QLineEdit") || w->inherits("QRadioButton") ||
       w->inherits("QCheckBox") || w->inherits("QScrollBar")) {
        w->installEventFilter(this);
    }
    if(w->inherits("QLineEdit")){
        QPalette pal = w->palette();
        pal.setBrush(QColorGroup::Base, baseBrush);
        w->setPalette(pal);
    }
    if(w->inherits("QPushButton")){
        applyCustomAttributes((QPushButton *)w);
        w->installEventFilter(this);
    }
    if(w->inherits("QButton") || w-> inherits("QComboBox")){
    	w-> setBackgroundMode ( QWidget::PaletteBackground );
    }

    bool isViewport = qstrcmp(w->name(), "qt_viewport") == 0 ||
        qstrcmp(w->name(), "qt_clipped_viewport") == 0;
    bool isViewportChild = w->parent() &&
        ((qstrcmp(w->parent()->name(), "qt_viewport") == 0) ||
         (qstrcmp(w->parent()->name(), "qt_clipped_viewport") == 0));

    if(isViewport && w->parent() && qstrcmp(w->parent()->name(), "proxyview") == 0){
        w->setBackgroundMode(QWidget::X11ParentRelative);
        return;
    }
    if(isViewportChild){
        if(w->inherits("QButton") || w->inherits("QComboBox")){
            if(w->parent()){ // heh, only way to test for KHTML children ;-)
                if(w->parent()->parent()){
                    if(w->parent()->parent()->parent() &&
                       w->parent()->parent()->parent()->inherits("KHTMLView")){
                        w->setAutoMask(true);
                        w->setBackgroundMode(QWidget::NoBackground);
                    }
                }
            }
            return;
        }
    }
    if(w->inherits("QHeader")){
        w->setMouseTracking(true);
        w->installEventFilter(this);
    }
    if(w-> inherits("QToolButton")&&w->parent()->inherits("QToolBar")) {
    	((QToolButton*)w)->setAutoRaise (flatTBButtons);
    	if ( flatTBButtons )
    		w->setBackgroundOrigin(QWidget::ParentOrigin);
    }
    if(w->ownPalette() && !w->inherits("QButton") && !w->inherits("QComboBox")){
        return;
    }

    if(w->parent() && w->parent()->isWidgetType() && !((QWidget*)w->parent())->
       palette().active().brush(QColorGroup::Background).pixmap()){
        qWarning("No parent pixmap for child widget %s", w->className());
        return;
    }
    if(!isViewport && !isViewportChild && !w->testWFlags(WType_Popup) &&
	   !( !w-> inherits("QLineEdit") && w-> parent() && w-> parent()-> isWidgetType ( ) && w-> parent()-> inherits ( "QMultiLineEdit" ))) {
        if(w->backgroundMode() == QWidget::PaletteBackground ||
           w->backgroundMode() == QWidget::PaletteButton){
            w->setBackgroundMode(QWidget::X11ParentRelative);
        }
    }

}

void LiquidStyle::unPolish(QWidget *w)
{
    if(w->inherits("QMenuBar")){
        ((QFrame *)w)->setLineWidth(1);
        w->setBackgroundMode(QWidget::PaletteBackground);
        return;
    }

    if(w->inherits("QPopupMenu"))
        w->setBackgroundMode(QWidget::PaletteButton);
    else if(w-> testWFlags(Qt::WType_Popup) && !w->inherits("QListBox")) {
    	w->removeEventFilter(menuHandler);
    }

    if(w->isTopLevel())
        return;

    // for viewport children, don't just check for NoBackground....
    bool isViewportChild = w->parent() &&
        ((qstrcmp(w->parent()->name(), "qt_viewport") == 0) ||
         (qstrcmp(w->parent()->name(), "qt_clipped_viewport") == 0));

    w->unsetPalette();
    if(w->backgroundMode() == QWidget::X11ParentRelative || isViewportChild){
        if(w->inherits("QPushButton"))
            w->setBackgroundMode(QWidget::PaletteButton);
        else
            w->setBackgroundMode(QWidget::PaletteBackground);
    }

    if(isViewportChild)
        w->setAutoMask(false);

    if(w->inherits("QPushButton")){
        unapplyCustomAttributes((QPushButton *)w);
        w->removeEventFilter(this);
    }
/*
    if(w->inherits("QPushButton") || w-> inherits("QComboBox")){
    	w-> setBackgroundMode ( PaletteBackground );
    }
*/
    if(w->inherits("QComboBox") ||
       w->inherits("QLineEdit") || w->inherits("QRadioButton") ||
       w->inherits("QCheckBox") || w->inherits("QScrollBar")) {
        w->removeEventFilter(this);
    }
    if(w->inherits("QButton") || w->inherits("QComboBox")){
        if(w->parent() && w->parent()->inherits("KHTMLPart")){
            w->setAutoMask(false);
        }
    }
    if(w->inherits("QToolBar")){
        w->removeEventFilter(this);
        w->setBackgroundMode(QWidget::PaletteBackground);
        return;
    }
    if(w->inherits("QHeader")){
        w->setMouseTracking(false);
        w->removeEventFilter(this);
    }
}

void LiquidStyle::polish(QApplication *app)
{

    QWindowsStyle::polish(app);
    menuAni = app->isEffectEnabled(UI_AnimateMenu);
    menuFade = app->isEffectEnabled(UI_FadeMenu);
    if(menuAni)
        app->setEffectEnabled(UI_AnimateMenu, false);
    if(menuFade)
        app->setEffectEnabled(UI_FadeMenu, false);
	
    qt_set_draw_menu_bar_impl((QDrawMenuBarItemImpl) &LiquidStyle::drawMenuBarItem);
    
    Config config ( "qpe" );
    config. setGroup ( "Liquid-Style" );
    
//    if ( config. readBoolEntry ( "WinDecoration", true ))	
//	    QApplication::qwsSetDecoration ( new LiquidDecoration ( ));
	    
	flatTBButtons = config. readBoolEntry ( "FlatToolButtons", false );
}

void LiquidStyle::unPolish(QApplication *app)
{
    QWindowsStyle::unPolish(app);
    app->setEffectEnabled(UI_AnimateMenu, menuAni);
    app->setEffectEnabled(UI_FadeMenu, menuFade);

    qt_set_draw_menu_bar_impl ( 0 );
    
//    QApplication::qwsSetDecoration ( new QPEDecoration ( ));
}

/*
 * This is a fun method ;-) Here's an overview. KToolBar grabs resize to
 * force everything to erase and repaint on resize. This is going away, I'm
 * trying to get shaped widgets to work right without masking. QPushButton,
 * QComboBox, and Panel applet handles capture mouse enter and leaves in order
 * to set the highlightwidget and repaint for mouse hovers. CheckBoxes and
 * RadioButtons need to do this differently. Qt buffers these in pixmaps and
 * caches them in QPixmapCache, which is bad for doing things like hover
 * because the style methods aren't called in paintEvents if everything
 * is cached. We use our own Paint event handler instead. Taskbuttons and
 * pager buttons draw into a pixmap buffer, so we handle those with palette
 * modifications. For QHeader, different header items are actually one widget
 * that draws multiple items, so we need to check which ID is hightlighted
 * and draw it. Finally, we also check enter and leave events for QLineEdit,
 * since if it's inside a combobox we want to highlight the combobox during
 * hovering in the edit.
 */
bool LiquidStyle::eventFilter(QObject *obj, QEvent *ev)
{
    if(obj->inherits("QToolBar")){
        if(ev->type() == QEvent::Resize){
            const QObjectList *tbChildList = obj->children();
            QObjectListIt it(*tbChildList);
            QObject *child;
            while((child = it.current()) != NULL){
                ++it;
                if(child->isWidgetType())
                    ((QWidget *)child)->repaint(true);
            }

        }
    }
    else if(obj->inherits("QPushButton") || obj->inherits("QComboBox")){
        QWidget *btn = (QWidget *)obj;
        if(ev->type() == QEvent::Enter){
            if(btn->isEnabled()){
                highlightWidget = btn;
                btn->repaint(false);
            }
        }
        else if(ev->type() == QEvent::Leave){
            if(btn == highlightWidget){
                highlightWidget = NULL;
                btn->repaint(false);
            }
        }
    }
    else if(obj->inherits("QToolButton")){
        QToolButton *btn = (QToolButton *)btn;
        if(!btn->autoRaise()){
            if(btn->isEnabled()){
                highlightWidget = btn;
                btn->repaint(false);
            }
        }
        else if(ev->type() == QEvent::Leave){
            QWidget *btn = (QWidget *)obj;
            if(btn == highlightWidget){
                highlightWidget = NULL;
                btn->repaint(false);
            }
        }
        else
            highlightWidget = NULL;
    }
    else if(obj->inherits("QScrollBar")){
        QScrollBar *sb = (QScrollBar *)obj;
        if(ev->type() == QEvent::Enter){
            if(sb->isEnabled()){
                highlightWidget = sb;
                sb->repaint(false);
            }
        }
        else if(ev->type() == QEvent::Leave){
            if(sb == highlightWidget && !sb->draggingSlider()){
                highlightWidget = NULL;
                sb->repaint(false);
            }
        }
        else if(ev->type() == QEvent::MouseButtonRelease){
            QMouseEvent *me = (QMouseEvent *)ev;
            if(sb == highlightWidget && !sb->rect().contains(me->pos())){
                highlightWidget = NULL;
                sb->repaint(false);
            }
        }
    }
    else if(obj->inherits("QLineEdit")){
        if(obj->parent() && obj->parent()->inherits("QComboBox")){
            QWidget *btn = (QComboBox *)obj->parent();
            if(ev->type() == QEvent::Enter){
                if (btn->isEnabled()){
                    highlightWidget = btn;
                    btn->repaint(false);
                }
            }
            else if(ev->type() == QEvent::Leave){
                if (btn == highlightWidget)
                    highlightWidget = NULL;
                btn->repaint(false);
            }
        }
    }
    else if(obj->inherits("QRadioButton") || obj->inherits("QCheckBox")){
        QButton *btn = (QButton *)obj;
        bool isRadio = obj->inherits("QRadioButton");
        if(ev->type() == QEvent::Paint){
            //if(btn->autoMask())
            btn->erase();
            QPainter p;
            p.begin(btn);
            QFontMetrics fm = btn->fontMetrics();
            QSize lsz = fm.size(ShowPrefix, btn->text());
            QSize sz = isRadio ? exclusiveIndicatorSize()
                : indicatorSize();

            if(btn->hasFocus()){
                QRect r = QRect(0, 0, btn->width(), btn->height());
                p.setPen(btn->colorGroup().button().dark(140));
                p.drawLine(r.x()+1, r.y(), r.right()-1, r.y());
                p.drawLine(r.x(), r.y()+1, r.x(), r.bottom()-1);
                p.drawLine(r.right(), r.y()+1, r.right(), r.bottom()-1);
                p.drawLine(r.x()+1, r.bottom(), r.right()-1, r.bottom());
            }
            int x = 0;
            int y = (btn->height()-lsz.height()+fm.height()-sz.height())/2;
            if(isRadio)
                drawExclusiveIndicator(&p, x, y, sz.width(), sz.height(),
                                       btn->colorGroup(), btn->isOn(),
                                       btn->isDown(), btn->isEnabled());
            else
                drawIndicator(&p, x, y, sz.width(), sz.height(),
                              btn->colorGroup(), btn->state(), btn->isDown(),
                              btn->isEnabled());
            x = sz.width() + 6;
            y = 0;
            drawItem(&p, sz.width()+6+1, 0, btn->width()-(sz.width()+6+1),
                     btn->height(), AlignLeft|AlignVCenter|ShowPrefix,
                     btn->colorGroup(), btn->isEnabled(),
                     btn->pixmap(), btn->text());
            p.end();
            return(true);
        }
        // for hover, just redraw the indicator (not the text)
        else if((ev->type() == QEvent::Enter && btn->isEnabled()) ||
                (ev->type() == QEvent::Leave && btn == highlightWidget)){
            QButton *btn = (QButton *)obj;
            bool isRadio = obj->inherits("QRadioButton");

            if(ev->type() == QEvent::Enter)
                highlightWidget = btn;
            else 
                highlightWidget = NULL;
            QFontMetrics fm = btn->fontMetrics();
            QSize lsz = fm.size(ShowPrefix, btn->text());
            QSize sz = isRadio ? exclusiveIndicatorSize()
                : indicatorSize();
            int x = 0;
            int y = (btn->height()-lsz.height()+fm.height()-sz.height())/2;
            //if(btn->autoMask())
            //    btn->erase(x+1, y+1, sz.width()-2, sz.height()-2);
            QPainter p;
            p.begin(btn);
            if(isRadio)
                drawExclusiveIndicator(&p, x, y, sz.width(), sz.height(),
                                       btn->colorGroup(), btn->isOn(),
                                       btn->isDown(), btn->isEnabled());
            else
                drawIndicator(&p, x, y, sz.width(), sz.height(),
                              btn->colorGroup(), btn->state(), btn->isDown(),
                              btn->isEnabled());
            p.end();
        }
    }
    else if(obj->inherits("QHeader")){
        QHeader *hw = (QHeader *)obj;
        if(ev->type() == QEvent::Enter){
            currentHeader = hw;
            headerHoverID = -1;
        }
        else if(ev->type() == QEvent::Leave){
            currentHeader = NULL;
            if(headerHoverID != -1){
                hw->repaint(hw->sectionPos(headerHoverID), 0,
                            hw->sectionSize(headerHoverID), hw->height());
            }
            headerHoverID = -1;
        }
        else if(ev->type() == QEvent::MouseMove){
            QMouseEvent *me = (QMouseEvent *)ev;
            int oldHeader = headerHoverID;
            headerHoverID = hw->sectionAt(me->x());
            if(oldHeader != headerHoverID){
                // reset old header
                if(oldHeader != -1){
                    hw->repaint(hw->sectionPos(oldHeader), 0,
                                hw->sectionSize(oldHeader), hw->height());
                }
                if(headerHoverID != -1){
                    hw->repaint(hw->sectionPos(headerHoverID), 0,
                                hw->sectionSize(headerHoverID), hw->height());
                }
            }
        }
    }
    return(false);
}

void LiquidStyle::drawButton(QPainter *p, int x, int y, int w, int h,
                         const QColorGroup &g, bool sunken,
                         const QBrush *)
{
    drawRoundButton(p, sunken ? g.background() : g.button(), g.background(),
                    x, y, w, h);
}

void LiquidStyle::drawToolButton(QPainter *p, int x, int y, int w, int h,
                                 const QColorGroup &g, bool sunken,
                                 const QBrush *)
{
    if(p->device()->devType() != QInternal::Widget){
        // drawing into a temp pixmap, don't use mask
        QColor c = sunken ? g.button() : g.background();
        p->setPen(c.dark(130));
        p->drawRect(x, y, w, h);
        p->setPen(c.light(105));
        p->drawRect(x+1, y+1, w-2, h-2);


        // fill
        QPixmap *pix = bevelFillDict.find(c.rgb());
        if(!pix){
            int h, s, v;
            c.hsv(&h, &s, &v);
            pix = new QPixmap(*bevelFillPix);
            adjustHSV(*pix, h, s, v);
            bevelFillDict.insert(c.rgb(), pix);
        }

        p->drawTiledPixmap(x+2, y+2, w-4, h-4, *pix);
    }
    else{
        drawClearBevel(p, x, y, w, h, sunken ? g.button() :
                       highlightWidget == p->device() ? g.button().light(110) :
                       g.background(), g.background());
    }
}

void LiquidStyle::drawPushButton(QPushButton *btn, QPainter *p)
{
    QRect r = btn->rect();
    bool sunken = btn->isOn() || btn->isDown();
    QColorGroup g = btn->colorGroup();


    //int dw = buttonDefaultIndicatorWidth();
    if(btn->hasFocus() || btn->isDefault()){
        QColor c = btn->hasFocus() ? g.button().light(110) : g.background();
        QPixmap *pix = bevelFillDict.find(c.rgb());
        if(!pix){
            int h, s, v;
            c.hsv(&h, &s, &v);
            pix = new QPixmap(*bevelFillPix);
            adjustHSV(*pix, h, s, v);
            bevelFillDict.insert(c.rgb(), pix);
        }
        p->setPen(c.dark(150));
        p->drawLine(r.x()+1, r.y(), r.right()-1, r.y());
        p->drawLine(r.x(), r.y()+1, r.x(), r.bottom()-1);
        p->drawLine(r.right(), r.y()+1, r.right(), r.bottom()-1);
        p->drawLine(r.x()+1, r.bottom(), r.right()-1, r.bottom());
        p->drawTiledPixmap(r.x()+1, r.y()+1, r.width()-2, r.height()-2, *pix);
    }

    QColor newColor = btn == highlightWidget || sunken ?
        g.button().light(120) : g.button();

    drawRoundButton(p, newColor, g.background(),
                    r.x(), r.y(), r.width(), r.height(), !btn->autoMask(),
                    sunken, btn->isDefault() || btn->autoDefault() || btn->hasFocus(),
                    btn->autoMask());
}

void LiquidStyle::drawPushButtonLabel(QPushButton *btn, QPainter *p)
{
    int x1, y1, x2, y2, w, h;
    btn->rect().coords(&x1, &y1, &x2, &y2);
    w = btn->width();
    h = btn->height();

    bool act = btn->isOn() || btn->isDown();
    if(act){
        ++x1, ++y1;
    }

    // Draw iconset first, if any
    if ( btn->iconSet() && !btn->iconSet()->isNull() )
    {
	QIconSet::Mode mode = btn->isEnabled()
			      ? QIconSet::Normal : QIconSet::Disabled;
	if ( mode == QIconSet::Normal && btn->hasFocus() )
	    mode = QIconSet::Active;
	QPixmap pixmap = btn->iconSet()->pixmap( QIconSet::Small, mode );
	int pixw = pixmap.width();
	int pixh = pixmap.height();

	p->drawPixmap( x1+6, y1+h/2-pixh/2, pixmap );
	x1 += pixw + 8;
	w -= pixw + 8;
    }
    
    if(act){
        QFont font = btn->font();
        font.setBold(true);
        p->setFont(font);
        QColor shadow(btn->colorGroup().button().dark(130));
        drawItem( p, x1+1, y1+1, w, h,
                  AlignCenter | ShowPrefix, btn->colorGroup(), btn->isEnabled(),
                  btn->pixmap(), btn->text(), -1,
                  &shadow);

        drawItem( p, x1, y1, w, h,
                  AlignCenter | ShowPrefix, btn->colorGroup(), btn->isEnabled(),
                  btn->pixmap(), btn->text(), -1, &btn->colorGroup().light());
    }
    else{
        /* Too blurry
        drawItem( p, x1+1, y1+1, w, h,
                  AlignCenter | ShowPrefix, btn->colorGroup(), btn->isEnabled(),
                  btn->pixmap(), btn->text(), -1,
                  &btn->colorGroup().button().dark(115));
                  */
        drawItem( p, x1, y1, w, h,
                  AlignCenter | ShowPrefix,
		  btn->colorGroup(), btn->isEnabled(),
                  btn->pixmap(), btn->text(), -1,
                  &btn->colorGroup().buttonText());
    }
}

void LiquidStyle::drawButtonMask(QPainter *p, int x, int y, int w, int h)
{
    int x2 = x+w-1;
    int y2 = y+h-1;

    p->setPen(Qt::color1);
    p->fillRect(x, y, w, h, Qt::color0);
    if(w < 21 || h < 21){
        // outer rect
        p->drawLine(x, y+2, x, y2-2); // l
        p->drawLine(x2, y+2, x2, y2-2); // r
        p->drawLine(x+2, y, x2-2, y); // t
        p->drawLine(x+2, y2, x2-2, y2); // b
        p->drawLine(x+1, y+1, x2-1, y+1); // top second line
        p->drawLine(x+1, y2-1, x2-1, y2-1); // bottom second line
        p->fillRect(x+1, y+2, w-2, h-4, Qt::color1);
    }
    else{
        int x2 = x+w-1;
        int y2 = y+h-1;
        int bx2 = htmlBtnMaskBmp.width()-1;
        int by2 = htmlBtnMaskBmp.height()-1;
        p->drawPixmap(0, 0, htmlBtnMaskBmp, 0, 0, 10, 10); // tl
        p->drawPixmap(x2-9, 0, htmlBtnMaskBmp, bx2-9, 0, 10, 10); // tr
        p->drawPixmap(0, y2-9, htmlBtnMaskBmp, 0, by2-9, 10, 10); // bl
        p->drawPixmap(x2-9, y2-9, htmlBtnMaskBmp, bx2-9, by2-9, 10, 10); // br
        // fills
        p->fillRect(10, 0, w-20, 10, Qt::color1); // top
        p->fillRect(10, y2-9, w-20, 10, Qt::color1); // bottom
        p->fillRect(0, 10, w, h-20, Qt::color1); // middle
    }
}

void LiquidStyle::drawBevelButton(QPainter *p, int x, int y, int w, int h,
                              const QColorGroup &g, bool sunken,
                              const QBrush */*fill*/)
{
    if(currentHeader && p->device() == currentHeader){
        int id = currentHeader->sectionAt(x);
        bool isHeaderHover = id != -1 && id == headerHoverID;
        drawClearBevel(p, x, y, w, h, sunken ?
                       g.button() : isHeaderHover ? g.button().light(110) :
                       g.background(), g.background());
    }
    else
        drawClearBevel(p, x, y, w, h, sunken ? g.button() : g.background(),
                      g.background());
}

QRect LiquidStyle::buttonRect(int x, int y, int w, int h)
{
    return(QRect(x+5, y+5, w-10, h-10));
}

void LiquidStyle::drawComboButton(QPainter *painter, int x, int y, int w, int h,
                                 const QColorGroup &g, bool sunken,
                                 bool edit, bool, const QBrush *)
{
    bool isHover = highlightWidget == painter->device();
    bool isMasked = false;
    if(painter->device()->devType() == QInternal::Widget)
        isMasked = ((QWidget*)painter->device())->autoMask();
    // TODO: Do custom code, don't just call drawRoundButton into a pixmap    
    QPixmap tmpPix(w, h);
    QPainter p(&tmpPix);

    drawRoundButton(&p, g.button(), g.background(), 0, 0, w, h, false,
                    sunken, false, isMasked);
    if(!isHover){
        p.setClipRect(0, 0, w-17, h);
        drawRoundButton(&p, g.background(), g.background(), 0, 0, w, h, false,
                        sunken, false, isMasked);
    }
    p.end();
    int x2 = x+w-1;
    int y2 = y+h-1;
    int bx2 = btnMaskBmp.width()-1;
    int by2 = btnMaskBmp.height()-1;
    QBitmap btnMask(w, h);
    QBitmap *mask = isMasked ? &htmlBtnMaskBmp : &btnMaskBmp;
    p.begin(&btnMask);
    p.fillRect(0, 0, w, h, Qt::color0);
    p.drawPixmap(0, 0, *mask, 0, 0, 10, 10); // tl
    p.drawPixmap(x2-9, 0, *mask, bx2-9, 0, 10, 10); // tr
    p.drawPixmap(0, y2-9, *mask, 0, by2-9, 10, 10); // bl
    p.drawPixmap(x2-9, y2-9, *mask, bx2-9, by2-9, 10, 10); // br
    // fills
    p.fillRect(10, 0, w-20, 10, Qt::color1); // top
    p.fillRect(10, y2-9, w-20, 10, Qt::color1); // bottom
    p.fillRect(0, 10, w, h-20, Qt::color1); // middle
    p.end();
    tmpPix.setMask(btnMask);

    painter->drawPixmap(x, y, tmpPix);

    painter->setPen(g.button().dark(120));
    painter->drawLine(x2-16, y+1, x2-16, y2-1);

    if(edit){
        painter->setPen(g.mid());
        painter->drawRect(x+8, y+2, w-25, h-4);
    }
    int arrow_h = h / 3;
    int arrow_w = arrow_h;
    int arrow_x = w - arrow_w - 6;
    int arrow_y = (h - arrow_h) / 2;
    drawArrow(painter, DownArrow, false, arrow_x, arrow_y, arrow_w, arrow_h, g, true);
}

void LiquidStyle::drawComboButtonMask(QPainter *p, int x, int y, int w, int h)
{
    drawButtonMask(p, x, y, w, h);
}

QRect LiquidStyle::comboButtonRect(int x, int y, int w, int h)
{
    //return(QRect(x+3, y+3, w - (h / 3) - 13, h-6));
    return(QRect(x+9, y+3, w - (h / 3) - 20, h-6));
}

QRect LiquidStyle::comboButtonFocusRect(int x, int y, int w, int h)
{
    return(QRect(x+5, y+3, w-(h/3)-13, h-5));
}

void LiquidStyle::drawScrollBarControls(QPainter *p, const QScrollBar *sb,
                                       int sliderStart, uint controls,
                                       uint activeControl)
{
    bool isHover = highlightWidget == p->device();
    int sliderMin, sliderMax, sliderLength, buttonDim;
    scrollBarMetrics( sb, sliderMin, sliderMax, sliderLength, buttonDim );

    if (sliderStart > sliderMax)
        sliderStart = sliderMax;

    bool horiz = sb->orientation() == QScrollBar::Horizontal;
    QColorGroup g = sb->colorGroup();
    QRect addB, subHC, subB;
    QRect addPageR, subPageR, sliderR;
    int addX, addY, subX, subY;
    int len = horiz ? sb->width() : sb->height();
    int extent = horiz ? sb->height() : sb->width();

    // a few apps (ie: KSpread), are broken and use a hardcoded scrollbar
    // extent of 16. Luckily, most apps (ie: Kate), seem fixed by now.
    bool brokenApp;
    if(extent == 16)
        brokenApp = true;
    else
        brokenApp = false;

    if (horiz) {
        subY = addY = ( extent - buttonDim ) / 2;
        subX = 0;
        addX = len - buttonDim;
        if(sbBuffer.size() != sb->size())
            sbBuffer.resize(sb->size());
    }
    else {
        subX = addX = ( extent - buttonDim ) / 2;
        subY = 0;
        addY = len - buttonDim;
        if(sbBuffer.size() != sb->size())
            sbBuffer.resize(sb->size());
    }
    subB.setRect( subX,subY,0,0); // buttonDim,buttonDim );
    addB.setRect( addX,addY,buttonDim,buttonDim );
    if(horiz)
        subHC.setRect(addX-buttonDim,addY,buttonDim,buttonDim );
    else
        subHC.setRect(addX,addY-buttonDim,buttonDim,buttonDim );

    int sliderEnd = sliderStart + sliderLength;
    int sliderW = extent;

    if (horiz) {
        subPageR.setRect( subB.right() + 1, 0,
                          sliderStart - subB.right() - 1 , sliderW );
        addPageR.setRect( sliderEnd, 0, addX - sliderEnd - buttonDim, sliderW );
        sliderR .setRect( sliderStart, 0, sliderLength, sliderW );
    }
    else {
        subPageR.setRect( 0, subB.bottom() + 1, sliderW,
                          sliderStart - subB.bottom() - 1 );
        addPageR.setRect( 0, sliderEnd, sliderW, addY - buttonDim - sliderEnd);
        sliderR .setRect( 0, sliderStart, sliderW, sliderLength );
    }

    bool maxed = sb->maxValue() == sb->minValue();

    QPainter painter;
    if(!horiz){
        painter.begin(&sbBuffer);
        QRect bgR(0, subB.bottom()+1, sb->width(), (len-(buttonDim*2))+1);
        if(sliderR.height() >= 8){
            painter.drawPixmap(bgR.x()+1, bgR.y(), *vsbSliderFillPix, 0, 0,
                          13, 8);
            painter.drawPixmap(bgR.x()+1, bgR.y(), *getPixmap(VSBSliderTopBg));
            painter.drawTiledPixmap(bgR.x()+1, bgR.y()+8, 13,
                                    bgR.height()-16, *getPixmap(VSBSliderMidBg));
            painter.drawPixmap(bgR.x()+1, bgR.bottom()-8, *vsbSliderFillPix,
                          0, 0, 13, 8);
            painter.drawPixmap(bgR.x()+1, bgR.bottom()-8, *getPixmap(VSBSliderBtmBg));
        }
        else{
            painter.drawTiledPixmap(bgR.x()+1, bgR.y(), 13, bgR.height(),
                                    *getPixmap(VSBSliderMidBg));
            painter.setPen(g.background().dark(210));
            painter.drawRect(bgR.x()+1, bgR.y(), 13, bgR.height()-1);
            painter.setPen(g.mid());
            painter.drawPoint(bgR.x()+1, bgR.y());
            painter.drawPoint(bgR.x()+13, bgR.y());
            painter.drawPoint(bgR.x()+1, bgR.bottom()-1);
            painter.drawPoint(bgR.x()+13, bgR.bottom()-1);
        }
        if(controls & Slider){
            if(sliderR.height() >= 16){
                painter.drawPixmap(sliderR.x()+1, sliderR.y(),
                                   isHover ? *getPixmap(VSBSliderTopHover):
                                   *getPixmap(VSBSliderTop));
                painter.drawTiledPixmap(sliderR.x()+1, sliderR.y()+8, 13,
                                        sliderR.height()-16, isHover ?
                                        *getPixmap(VSBSliderMidHover) :
                                        *getPixmap(VSBSliderMid));
                painter.drawPixmap(sliderR.x()+1, sliderR.bottom()-8,
                                   isHover ? *getPixmap(VSBSliderBtmHover) :
                                   *getPixmap(VSBSliderBtm));
            }
            else if(sliderR.height() >= 8){
                int m = sliderR.height()/2;
                painter.drawPixmap(sliderR.x()+1, sliderR.y(),
                                   isHover ? *getPixmap(VSBSliderTopHover):
                                   *getPixmap(VSBSliderTop), 0, 0, 13, m);
                painter.drawPixmap(sliderR.x()+1, sliderR.y()+m,
                                   isHover ? *getPixmap(VSBSliderBtmHover):
                                   *getPixmap(VSBSliderBtm), 0, 8-m, 13, m);
            }
            else{
                painter.setPen(g.button().dark(210));
                drawRoundRect(&painter, sliderR.x()+1, sliderR.y(),
                              13, sliderR.height());
                painter.drawTiledPixmap(sliderR.x()+2, sliderR.y()+1,
                                        11, sliderR.height()-2,
                                        isHover ? *getPixmap(VSBSliderMidHover) :
                                        *getPixmap(VSBSliderMid), 1, 0);
            }
        }
        painter.setPen(g.mid());
        painter.drawLine(bgR.x(), bgR.y(), bgR.x(), bgR.bottom());
        painter.drawLine(bgR.right(), bgR.y(), bgR.right(), bgR.bottom());
        if(brokenApp && (controls & Slider)){
            painter.setPen(g.background());
            painter.drawLine(bgR.right()-1, bgR.y(), bgR.right()-1,
                             bgR.bottom());
        }
        painter.end();
    }
    else{
        painter.begin(&sbBuffer);
        QRect bgR(subB.right()+1, 0, (len-(buttonDim*2))+1, sb->height());
        if(sliderR.width() >= 8){
            painter.drawPixmap(bgR.x(), bgR.y()+1, *vsbSliderFillPix, 0, 0,
                               8, 13);
            painter.drawPixmap(bgR.x(), bgR.y()+1, *getPixmap(HSBSliderTopBg));
            painter.drawTiledPixmap(bgR.x()+8, bgR.y()+1, bgR.width()-16,
                                    13, *getPixmap(HSBSliderMidBg));
            painter.drawPixmap(bgR.right()-8, bgR.y()+1, *vsbSliderFillPix,
                               0, 0, 8, 13);
            painter.drawPixmap(bgR.right()-8, bgR.y()+1, *getPixmap(HSBSliderBtmBg));
        }
        else{
            painter.drawTiledPixmap(bgR.x(), bgR.y()+1, bgR.width(), 13,
                                    *getPixmap(HSBSliderMidBg));
            painter.setPen(g.background().dark(210));
            painter.drawRect(bgR.x(), bgR.y()+1, bgR.width()-1, 13);
            painter.setPen(g.mid());
            painter.drawPoint(bgR.x(), bgR.y()+1);
            painter.drawPoint(bgR.x(), bgR.bottom()-1);
            painter.drawPoint(bgR.right()-1, bgR.y()+1);
            painter.drawPoint(bgR.right()-1, bgR.bottom()-1);
        }
        if(controls & Slider){
            if(sliderR.width() >= 16){
                painter.drawPixmap(sliderR.x(), sliderR.y()+1,
                                   isHover ? *getPixmap(HSBSliderTopHover) :
                                   *getPixmap(HSBSliderTop));
                painter.drawTiledPixmap(sliderR.x()+8, sliderR.y()+1, sliderR.width()-16,
                                        13, isHover ? *getPixmap(HSBSliderMidHover) :
                                        *getPixmap(HSBSliderMid));
                painter.drawPixmap(sliderR.right()-8, sliderR.y()+1, isHover ?
                                   *getPixmap(HSBSliderBtmHover) :
                                   *getPixmap(HSBSliderBtm));
            }
            else if(sliderR.width() >= 8){
                int m = sliderR.width()/2;
                painter.drawPixmap(sliderR.x(), sliderR.y()+1,
                                   isHover ? *getPixmap(HSBSliderTopHover) :
                                   *getPixmap(HSBSliderTop), 0, 0, m, 13);
                painter.drawPixmap(sliderR.right()-8, sliderR.y()+1, isHover ?
                                   *getPixmap(HSBSliderBtmHover) :
                                   *getPixmap(HSBSliderBtm), 8-m, 0, m, 13);
            }
            else{
                painter.setPen(g.button().dark(210));
                drawRoundRect(&painter, sliderR.x(), sliderR.y()+1,
                              sliderR.width(), 13);
                painter.drawTiledPixmap(sliderR.x()+1, sliderR.y()+2,
                                        sliderR.width()-2, 11, isHover ?
                                        *getPixmap(HSBSliderMidHover) :
                                        *getPixmap(HSBSliderMid), 0, 1);
            }
        }
        painter.setPen(g.mid());
        painter.drawLine(bgR.x(), bgR.y(), bgR.right(), bgR.y());
        painter.drawLine(bgR.x(), bgR.bottom(), bgR.right(), bgR.bottom());
        if(brokenApp && (controls & Slider)){
            painter.setPen(g.background());
            painter.drawLine(bgR.x(), bgR.bottom()-1, bgR.right(),
                             bgR.bottom()-1);
        }
        painter.end();
    }

    if ( controls & AddLine ) {
        drawSBButton(p, addB, g, activeControl == AddLine);
        drawArrow( p, horiz ? RightArrow : DownArrow,
                   false, addB.x()+4, addB.y()+4,
                   addB.width()-8, addB.height()-8, g, !maxed);
    }
    if ( controls & SubLine ) {
     //   drawSBButton(p, subB, g, activeControl == SubLine);
     //   drawArrow( p, horiz ? LeftArrow : UpArrow,
     //              false, subB.x()+4, subB.y()+4,
     //              subB.width()-8, subB.height()-8, g, !maxed);
        drawSBButton(p, subHC, g, activeControl == SubLine);
        drawArrow( p, horiz ? LeftArrow : UpArrow,
                   false, subHC.x()+4, subHC.y()+4,
                   subHC.width()-8, subHC.height()-8, g, !maxed);
    }

    if(controls & AddPage){
        if(addPageR.width()){
            p->drawPixmap(addPageR.x(), addPageR.y(), sbBuffer,
                          addPageR.x(), addPageR.y(), addPageR.width(),
                          addPageR.height());
        }
    }
    if(controls & SubPage){
        if(subPageR.height()){
            p->drawPixmap(subPageR.x(), subPageR.y(), sbBuffer,
                          subPageR.x(), subPageR.y(), subPageR.width(),
                          subPageR.height());
        }
    }
    if ( controls & Slider ) {
        p->drawPixmap(sliderR.x(), sliderR.y(), sbBuffer,
                      sliderR.x(), sliderR.y(), sliderR.width(),
                      sliderR.height());
    }
}

void LiquidStyle::drawSBButton(QPainter *p, const QRect &r, const QColorGroup &g,
                           bool down, bool /*fast*/)
{
    p->setPen(g.mid());
    p->drawRect(r);
    QColor c(down ? g.button() : g.background());
    QPixmap *pix = bevelFillDict.find(c.rgb());
    if(!pix){
        int h, s, v;
        c.hsv(&h, &s, &v);
        pix = new QPixmap(*bevelFillPix);
        adjustHSV(*pix, h, s, v);
        bevelFillDict.insert(c.rgb(), pix);
    }
    p->drawTiledPixmap(r.x()+1, r.y()+1, r.width()-2, r.height()-2, *pix);

}

void LiquidStyle::drawSBDeco(QPainter *p, const QRect &r, const QColorGroup &g,
                         bool horiz)
{
    if(horiz){
        if(r.width() >= 15){
            int y = r.y()+3;
            int x = r.x() + (r.width()-7)/2;
            int y2 = r.bottom()-3;
            p->setPen(g.light());
            p->drawLine(x, y, x, y2);
            p->drawLine(x+3, y, x+3, y2);
            p->drawLine(x+6, y, x+6, y2);

            p->setPen(g.mid());
            p->drawLine(x+1, y, x+1, y2);
            p->drawLine(x+4, y, x+4, y2);
            p->drawLine(x+7, y, x+7, y2);
        }
    }
    else{
        if(r.height() >= 15){
            int x = r.x()+3;
            int y = r.y() + (r.height()-7)/2;
            int x2 = r.right()-3;
            p->setPen(g.light());
            p->drawLine(x, y, x2, y);
            p->drawLine(x, y+3, x2, y+3);
            p->drawLine(x, y+6, x2, y+6);

            p->setPen(g.mid());
            p->drawLine(x, y+1, x2, y+1);
            p->drawLine(x, y+4, x2, y+4);
            p->drawLine(x, y+7, x2, y+7);
        }
    }

}


void LiquidStyle::scrollBarMetrics(const QScrollBar *sb, int &sliderMin,
                                  int &sliderMax, int &sliderLength,
                                  int &buttonDim)
{

    int maxLength;
    int b = 0;
    bool horiz = sb->orientation() == QScrollBar::Horizontal;
    int length = horiz ? sb->width()  : sb->height();
    int extent = horiz ? sb->height() : sb->width();

    if ( length > ( extent - b*2 - 1 )*2 + b*2  )
	buttonDim = extent - b*2;
    else
	buttonDim = ( length - b*2 )/2 - 1;

    sliderMin = b + 0; // buttonDim;
    maxLength  = length - b*2 - buttonDim*2; // 3;

    if ( sb->maxValue() == sb->minValue() ) {
	sliderLength = maxLength;
    } else {
	sliderLength = (sb->pageStep()*maxLength)/
			(sb->maxValue()-sb->minValue()+sb->pageStep());
	uint range = sb->maxValue()-sb->minValue();
	if ( sliderLength < 9 || range > INT_MAX/2 )
	    sliderLength = 9;
	if ( sliderLength > maxLength )
	    sliderLength = maxLength;
    }
    sliderMax = sliderMin + maxLength - sliderLength;

}

QStyle::ScrollControl LiquidStyle::scrollBarPointOver(const QScrollBar *sb,
                                                     int sliderStart,
                                                     const QPoint &p)
{
    if ( !sb->rect().contains( p ) )
        return NoScroll;
    int sliderMin, sliderMax, sliderLength, buttonDim, pos;
    scrollBarMetrics( sb, sliderMin, sliderMax, sliderLength, buttonDim );
    pos = (sb->orientation() == QScrollBar::Horizontal)? p.x() : p.y();
    if ( pos < sliderMin )
	return SubLine;
    if ( pos < sliderStart )
	return SubPage;
    if ( pos < sliderStart + sliderLength )
        return Slider;
    if ( pos < sliderMax + sliderLength)
        return AddPage;
    if(pos > sliderMax + sliderLength + 16)
        return AddLine;

    return SubLine;
}

#define QCOORDARRLEN(x) sizeof(x)/(sizeof(QCOORD)*2)


QSize LiquidStyle::exclusiveIndicatorSize() const
{
    return(QSize(16, 16));
}

void LiquidStyle::drawExclusiveIndicator(QPainter *p, int x, int y, int /*w*/,
                                        int /*h*/, const QColorGroup &/*g*/, bool on,
                                        bool down, bool)
{
    bool isHover = highlightWidget == p->device();
    bool isMasked = p->device() && p->device()->devType() == QInternal::Widget
        && ((QWidget*)p->device())->autoMask();

    if(isMasked){
        if(on || down){
            p->drawPixmap(x, y, isHover ? *getPixmap(HTMLRadioDownHover) :
                          *getPixmap(HTMLRadioDown));
        }
        else
            p->drawPixmap(x, y, isHover ? *getPixmap(HTMLRadioHover) :
                          *getPixmap(HTMLRadio));

    }
    else{
        if(on || down){
            p->drawPixmap(x, y, isHover ? *getPixmap(RadioOnHover) :
                          *getPixmap(RadioOn));
        }
        else
            p->drawPixmap(x, y, isHover ? *getPixmap(RadioOffHover) :
                          *getPixmap(RadioOff));
    }
}

void LiquidStyle::drawExclusiveIndicatorMask(QPainter *p, int x, int y, int w,
                                            int h, bool)
{
    p->fillRect(x, y, w, h, Qt::color0);
    p->setPen(Qt::color1);
    p->drawPixmap(x, y, *getPixmap(RadioOn)->mask());
}


QSize LiquidStyle::indicatorSize() const
{
    return(QSize(20, 22));
}

void LiquidStyle::drawIndicator(QPainter *p, int x, int y, int /*w*/, int /*h*/,
                            const QColorGroup &/*g*/, int state, bool /*down*/, bool)
{
    bool isHover = highlightWidget == p->device();
    bool isMasked = p->device() && p->device()->devType() == QInternal::Widget
        && ((QWidget*)p->device())->autoMask();
    if(isMasked){
        if(state != QButton::Off){
            p->drawPixmap(x, y, isHover ? *getPixmap(HTMLCBDownHover) :
                          *getPixmap(HTMLCBDown));
        }
        else
            p->drawPixmap(x, y, isHover ? *getPixmap(HTMLCBHover) :
                          *getPixmap(HTMLCB));

    }
    else{
        if(state != QButton::Off){
            p->drawPixmap(x, y, isHover ? *getPixmap(CBDownHover) :
                          *getPixmap(CBDown));
            /*  Todo - tristate
             if(state == QButton::On){
             p->setPen(Qt::black);
             p->drawPixmap(3, 3, xBmp);
             }
             else{
             p->setPen(g.dark());
             p->drawRect(x+2, y+2, w-4, h-4);
             p->setPen(Qt::black);
             p->drawLine(x+3, (y+h)/2-2, x+w-4, (y+h)/2-2);
             p->drawLine(x+3, (y+h)/2, x+w-4, (y+h)/2);
             p->drawLine(x+3, (y+h)/2+2, x+w-4, (y+h)/2+2);
             }*/
        }
        else
            p->drawPixmap(x, y, isHover ? *getPixmap(CBHover) : *getPixmap(CB));
    }
}

void LiquidStyle::drawIndicatorMask(QPainter *p, int x, int y, int /*w*/, int /*h*/,
                                   int /*state*/)
{
    // needed for some reason by KHtml, even tho it's all filled ;P
    p->drawPixmap(x, y, *getPixmap(HTMLCB)->mask());

}

void LiquidStyle::drawSlider(QPainter *p, int x, int y, int w, int h,
                         const QColorGroup &/*g*/, Orientation orient,
                         bool, bool)
{
    QWidget *parent = (QWidget *)p->device();
    p->setBrushOrigin(parent->pos());
    parent->erase(x, y, w, h);
    p->drawPixmap(x, y, orient == Qt::Horizontal ? *getPixmap(HSlider) :
                  *getPixmap(VSlider));
}

void LiquidStyle::drawSliderMask(QPainter *p, int x, int y, int /*w*/, int /*h*/,
                             Orientation orient, bool, bool)
{
    p->drawPixmap(x, y, orient == Qt::Horizontal ? *getPixmap(HSlider)->mask() :
                  *getPixmap(VSlider)->mask());
}

int LiquidStyle::sliderLength() const
{
    return(10);
}

#define QCOORDARRLEN(x) sizeof(x)/(sizeof(QCOORD)*2)

void LiquidStyle::drawArrow(QPainter *p, Qt::ArrowType type, bool on, int x,
                            int y, int w, int h, const QColorGroup &g,
                            bool enabled, const QBrush *)
{
    static const QCOORD u_arrow[]={3,1, 4,1, 2,2, 5,2, 1,3, 6,3, 0,4, 7,4, 0,5, 7,5};
    static const QCOORD d_arrow[]={0,2, 7,2, 0,3, 7,3, 1,4, 6,4, 2,5, 5,5, 3,6, 4,6};
    static const QCOORD l_arrow[]={1,3, 1,4, 2,2, 2,5, 3,1, 3,6, 4,0, 4,7, 5,0, 5,7};
    static const QCOORD r_arrow[]={2,0, 2,7, 3,0, 3,7, 4,1, 4,6, 5,2, 5,5, 6,3, 6,4};

    p->setPen(enabled ? on ? g.light() : g.buttonText() : g.mid());
    if(w > 8){
        x = x + (w-8)/2;
        y = y + (h-8)/2;
    }

    QPointArray a;
    switch(type){
    case Qt::UpArrow:
        a.setPoints(QCOORDARRLEN(u_arrow), u_arrow);
        break;
    case Qt::DownArrow:
        a.setPoints(QCOORDARRLEN(d_arrow), d_arrow);
        break;
    case Qt::LeftArrow:
        a.setPoints(QCOORDARRLEN(l_arrow), l_arrow);
        break;
    default:
        a.setPoints(QCOORDARRLEN(r_arrow), r_arrow);
        break;
    }

    a.translate(x, y);
    p->drawLineSegments(a);
}


void LiquidStyle::drawMenuBarItem(QPainter *p, int x, int y, int w, int h,
                            QMenuItem *mi, QColorGroup &g, bool /*enabled*/, bool active )
{	
    if(active){
        x -= 2; // Bug in Qt/E 
        y -= 2;
        w += 2;
        h += 2;
    }
    
    QWidget *parent = (QWidget *)p->device();
    p->setBrushOrigin(parent->pos());
    parent->erase(x, y, w, h);
   
    if(menuHandler->useShadowText()){
        QColor shadow;
        if(p->device() && p->device()->devType() == QInternal::Widget &&
           ((QWidget *)p->device())->inherits("QMenuBar")){
            shadow = ((QMenuBar*)p->device())->isTopLevel() ? g.button().dark(130) :
                g.background().dark(130);
        }
        else
            shadow = g.background().dark(130);

        if(active){
            drawClearBevel(p, x+1, y+1, w-1, h-1, g.button(), g.background());
            QApplication::style().drawItem(p, x+1, y+1, w, h,
                                           AlignCenter|ShowPrefix|DontClip|SingleLine,
                                           g, mi->isEnabled(), NULL, mi->text(),
                                           -1, &shadow);
            QApplication::style().drawItem(p, x, y, w, h,
                                           AlignCenter|ShowPrefix|DontClip|SingleLine,
                                           g, mi->isEnabled(), NULL, mi->text(),
                                           -1, &g.text());
        }
        else{
            QApplication::style().drawItem(p, x+1, y+1, w, h,
                                           AlignCenter|ShowPrefix|DontClip|SingleLine,
                                           g, mi->isEnabled(), NULL, mi->text(),
                                           -1, &shadow);
            QApplication::style().drawItem(p, x, y, w, h,
                                           AlignCenter|ShowPrefix|DontClip|SingleLine,
                                           g, mi->isEnabled(), NULL, mi->text(),
                                           -1, &g.text());
        }
    }
    else{
        if(active)
            drawClearBevel(p, x+1, y+1, w-1, h-1, g.button(), g.background());
        QApplication::style().drawItem(p, x, y, w, h,
                                       AlignCenter|ShowPrefix|DontClip|SingleLine,
                                       g, mi->isEnabled(), NULL, mi->text(),
                                       -1, &g.text());
    }
}

void LiquidStyle::drawPopupPanel(QPainter *p, int x, int y, int w, int h,
                                 const QColorGroup &g, int /*lineWidth*/,
                                 const QBrush * /*fill*/)
{
    QColor c;
    switch(menuHandler->transType()){
    case None:
    case StippledBg:
    case TransStippleBg:
        c = g.background();
        break;
    case StippledBtn:
    case TransStippleBtn:
        c = g.button();
        break;
    default:
        c = menuHandler->bgColor();
    }
    p->setPen(c.dark(140));
    p->drawRect(x, y, w, h);
    p->setPen(c.light(120));
    p->drawRect(x+1, y+1, w-2, h-2);
}

void LiquidStyle::drawPopupMenuItem( QPainter* p, bool checkable, int maxpmw,
                                     int tab, QMenuItem* mi,
                                     const QPalette& pal, bool act,
                                     bool enabled, int x, int y, int w, int h)
{
static const int motifItemFrame         = 2;
static const int motifItemHMargin       = 3;
static const int motifItemVMargin       = 2;
static const int motifArrowHMargin      = 6;
static const int windowsRightBorder     = 12;

    maxpmw = QMAX( maxpmw, 20 );


    bool dis = !enabled;
    QColorGroup itemg = dis ? pal.disabled() : pal.active();

    int checkcol = maxpmw;
    if(act){

        // FIXME
        drawClearBevel(p, x, y, w, h, itemg.button(), itemg.background());
    }
    //else if(((QWidget*)p->device())->backgroundPixmap()){
    //    p->drawPixmap(x, y, *((QWidget*)p->device())->backgroundPixmap(),
    //                  x, y, w, h);
    //}
    else{
        if(menuHandler->transType() == None){
            p->fillRect(x, y, w, h, pal.active().background());
        }
        else if(menuHandler->transType() == StippledBg){
            p->fillRect(x, y, w, h, bgBrush);
        }
        else if(menuHandler->transType() == StippledBtn){
            p->fillRect(x, y, w, h, menuBrush);
        }
        else{
            QPixmap *pix = menuHandler->pixmap(((QWidget*)p->device())->winId());
            if(pix)
                p->drawPixmap(x, y, *pix, x, y, w, h);
        }
    }

    if(!mi)
        return;

    QColor discol;
    if (dis) {
        discol = itemg.mid();
        p->setPen(discol);
    }

    QColorGroup cg2(itemg);

    if(menuHandler->transType() == Custom){
        cg2.setColor(QColorGroup::Foreground, menuHandler->textColor());
        cg2.setColor(QColorGroup::Text, menuHandler->textColor());
        cg2.setColor(QColorGroup::Light, menuHandler->textColor().light(120));
        cg2.setColor(QColorGroup::Mid, menuHandler->textColor().dark(120));
    }
    else{
        cg2 = QColorGroup(discol, itemg.highlight(), black, black,
                          dis ? discol : black, discol, black);
    }

    if(mi->isSeparator()){
        QColor c;
        switch(menuHandler->transType()){
        case None:
        case StippledBg:
        case TransStippleBg:
            c = QApplication::palette().active().background();
            break;
        case StippledBtn:
        case TransStippleBtn:
            c = QApplication::palette().active().button();
            break;
        default:
            c = menuHandler->bgColor();
        }
        p->setPen(c.dark(140));
        p->drawLine(x, y, x+w, y );
        p->setPen(c.light(115));
        p->drawLine(x, y+1, x+w, y+1 );
        return;
    }
    if(mi->iconSet()) {
        QIconSet::Mode mode = dis? QIconSet::Disabled : QIconSet::Normal;
        if (!dis)
            mode = QIconSet::Active;
        QPixmap pixmap = mi->iconSet()->pixmap(QIconSet::Small, mode);
        int pixw = pixmap.width();
        int pixh = pixmap.height();
        QRect cr(x, y, checkcol, h);
        QRect pmr(0, 0, pixw, pixh);
        pmr.moveCenter( cr.center() );
        p->setPen(itemg.highlightedText());
        p->drawPixmap(pmr.topLeft(), pixmap );

    }
    else if(checkable) {
        int mw = checkcol + motifItemFrame;
        int mh = h - 2*motifItemFrame;
        if (mi->isChecked()){
            drawCheckMark( p, x + motifItemFrame,
                           y+motifItemFrame, mw, mh, cg2, act, dis );
        }
    }
    if(menuHandler->transType() == Custom)
        p->setPen(menuHandler->textColor());
    else
        p->setPen(itemg.text());


    int xm = motifItemFrame + checkcol + motifItemHMargin;
    QString s = mi->text();
    if (!s.isNull()) {
        int t = s.find( '\t' );
        int m = motifItemVMargin;
        const int text_flags = AlignVCenter|ShowPrefix | DontClip | SingleLine;
        QPen oldPen = p->pen();
        if(menuHandler->useShadowText()){
            if(menuHandler->transType() == Custom)
                p->setPen(menuHandler->bgColor().dark(130));
            else if(menuHandler->transType() == StippledBtn ||
                    menuHandler->transType() == TransStippleBtn)
                p->setPen(itemg.button().dark(130));
            else
                p->setPen(bgBrush.color().dark(130));

            if (t >= 0) {
                p->drawText(x+w-tab-windowsRightBorder-motifItemHMargin-motifItemFrame+2,
                            y+m+2, tab-1, h-2*m-1, text_flags, s.mid( t+1 ));
            }
            p->drawText(x+xm+1, y+m+1, w-xm-tab, h-2*m-1, text_flags, s, t);
        }
        p->setPen(oldPen);
        if (t >= 0) {
            p->drawText(x+w-tab-windowsRightBorder-motifItemHMargin-motifItemFrame+1,
                        y+m+1, tab, h-2*m, text_flags, s.mid( t+1 ));
        }
        p->drawText(x+xm, y+m, w-xm-tab+1, h-2*m, text_flags, s, t);

    }
    else if (mi->pixmap()) {
        QPixmap *pixmap = mi->pixmap();
        if (pixmap->depth() == 1)
            p->setBackgroundMode(OpaqueMode);
        p->drawPixmap( x+xm, y+motifItemFrame, *pixmap);
        if (pixmap->depth() == 1)
            p->setBackgroundMode(TransparentMode);
    }
    if (mi->popup()) {
        int dim = (h-2*motifItemFrame) / 2;
        drawArrow(p, RightArrow, true,
                  x+w - motifArrowHMargin - motifItemFrame - dim,  y+h/2-dim/2,
                  dim, dim, cg2, TRUE);
    }
}

int LiquidStyle::popupMenuItemHeight(bool /*checkable*/, QMenuItem *mi,
                                 const QFontMetrics &fm)
{
    if (mi->isSeparator())
        return 2;

    int h = 0;
    if (mi->pixmap())
        h = mi->pixmap()->height();

    if (mi->iconSet())
        h = QMAX(mi->iconSet()->
                 pixmap(QIconSet::Small, QIconSet::Normal).height(), h);

    h = QMAX(fm.height() + 4, h);

    // we want a minimum size of 18
    h = QMAX(h, 18);

    return h;
}


void LiquidStyle::drawFocusRect(QPainter *p, const QRect &r,
                            const QColorGroup &g, const QColor *c,
                            bool atBorder)
{
    // are we painting a widget?
    if(p->device()->devType() == QInternal::Widget){
        // if so does it use a special focus rectangle?
        QWidget *w = (QWidget *)p->device();
        if(w->inherits("QPushButton") || w->inherits("QSlider")){
            return;
        }
        else{
            QWindowsStyle::drawFocusRect(p, r, g, c, atBorder);
        }
    }
    else
        QWindowsStyle::drawFocusRect(p, r, g, c, atBorder);

}

void LiquidStyle::polishPopupMenu(QPopupMenu *mnu)
{
    mnu->installEventFilter(menuHandler);
    QWindowsStyle::polishPopupMenu(mnu);
}

void LiquidStyle::drawTab(QPainter *p, const QTabBar *tabBar, QTab *tab,
                      bool selected)
{
    if(tabBar->shape() != QTabBar::RoundedAbove){
        QWindowsStyle::drawTab(p, tabBar, tab, selected);
        return;
    }
    QPixmap tilePix;
    QRect r = tab->rect();
    //QPixmap *pix = selected ? tabBar->hasFocus() ? pixmaps[TabFocus] :
    //    pixmaps[TabDown] : pixmaps[Tab];

    QPixmap *pix = selected ? getPixmap(TabDown) : getPixmap(Tab);
    p->drawPixmap(r.x(), r.y(), *pix, 0, 0, 9, r.height());
    p->drawPixmap(r.right()-9, r.y(), *pix, pix->width()-9, 0, 9, r.height());
    tilePix.resize(pix->width()-18, r.height());
    bitBlt(&tilePix, 0, 0, pix, 9, 0, pix->width()-18, r.height());
    p->drawTiledPixmap(r.x()+9, r.y(), r.width()-18, r.height(), tilePix);
    QColor c = tabBar->colorGroup().button();
    if(!selected){
        p->setPen(c.dark(130));
        p->drawLine(r.x(), r.bottom(), r.right(), r.bottom());
    }

    /*
    p->setPen(c.light(110));
    p->drawLine(r.x(), r.bottom()+1, r.right(), r.bottom()+1);
    p->setPen(c);
    p->drawLine(r.x(), r.bottom()+2, r.right(), r.bottom()+2);
    p->setPen(c.light(110));
    p->drawLine(r.x(), r.bottom()+3, r.right(), r.bottom()+3);
    p->setPen(c.light(120));
    p->drawLine(r.x(), r.bottom()+4, r.right(), r.bottom()+4);
    */

    /*
    QColor c = tabBar->colorGroup().button();
    pix = bevelFillDict.find(c.rgb());
    if(!pix){
        int h, s, v;
        c.hsv(&h, &s, &v);
        pix = new QPixmap(*bevelFillPix);
        adjustHSV(*pix, h, s);
        bevelFillDict.insert(c.rgb(), pix);
    }*/

    /*
    QRect r = tab->rect();
    QColorGroup g = tabBar->colorGroup();
    if(!selected){
        r.setY(r.y()+1);
        p->setPen(g.dark());
        p->drawLine(r.x(), r.y()+4, r.x(), r.bottom()-2);
        p->drawLine(r.x(), r.y()+4, r.x()+4, r.y());
        p->drawLine(r.x()+4, r.y(), r.right()-1, r.y());
        p->drawLine(r.right(), r.y()+1, r.right(), r.bottom()-2);

        p->setPen(g.midlight());
        p->drawLine(r.x()+1, r.y()+4, r.x()+1, r.bottom());
        p->drawLine(r.x()+1, r.y()+4, r.x()+4, r.y()+1);
        p->drawLine(r.x()+5, r.y()+1, r.right()-2, r.y()+1);
        p->drawLine(r.x(), r.bottom(), r.right(), r.bottom());

        p->setPen(g.mid());
        p->drawPoint(r.right()-1, r.y()+1);
        p->drawLine(r.x()+4, r.y()+2, r.right()-1, r.y()+2);
        p->drawLine(r.x()+3, r.y()+3, r.right()-1, r.y()+3);

        p->fillRect(r.x()+2, r.y()+4, r.width()-3, r.height()-6,
                    g.brush(QColorGroup::Mid));

        p->setPen(g.light());
        p->drawLine(r.x(), r.bottom()-1, r.right(), r.bottom()-1);

    }
    else{
        p->setPen(g.light());
        p->drawLine(r.x(), r.y()+4, r.x(), r.bottom());
        p->drawLine(r.x(), r.y()+4, r.x()+4, r.y());
        p->drawLine(r.x()+4, r.y(), r.right()-1, r.y());
        p->setPen(g.dark());
        p->drawLine(r.right(), r.y()+1, r.right(), r.bottom());

        p->setPen(g.midlight());
        p->drawLine(r.x()+1, r.y()+4, r.x()+1, r.bottom());
        p->drawLine(r.x()+1, r.y()+4, r.x()+4, r.y()+1);
        p->drawLine(r.x()+5, r.y()+1, r.right() - 2, r.y()+1);
        p->setPen(g.mid());
        p->drawLine(r.right()-1, r.y()+1, r.right()-1, r.bottom());
    }*/

}



void LiquidStyle::drawTabMask(QPainter *p, const QTabBar*, QTab *tab,
                             bool selected)


{
    //p->fillRect(tab->rect(), Qt::color1);
    QRect r = tab->rect();
    QPixmap *pix = selected ? getPixmap(TabDown) : getPixmap(Tab);
    p->drawPixmap(r.x(), r.y(), *pix->mask(), 0, 0, 9, r.height());
    p->drawPixmap(r.right()-9, r.y(), *pix->mask(), pix->width()-9, 0, 9, r.height());
    p->fillRect(r.x()+9, r.y(), r.width()-18, r.height(), Qt::color1);
}

void LiquidStyle::tabbarMetrics(const QTabBar *t, int &hFrame, int &vFrame,
                            int &overlap)
{
    if(t->shape() == QTabBar::RoundedAbove){
        overlap = 1;
        hFrame = 18; // was 24
        vFrame = 8; // was 10
    }
    else
        QWindowsStyle::tabbarMetrics(t, hFrame, vFrame, overlap);
}



void LiquidStyle::drawSplitter(QPainter *p, int x, int y, int w, int h,
                           const QColorGroup &g, Orientation)
{
    drawClearBevel(p, x, y, w, h, highlightWidget == p->device() ?
                   g.button().light(120) : g.button(), g.background());
}


void LiquidStyle::drawPanel(QPainter *p, int x, int y, int w, int h,
                        const QColorGroup &g, bool sunken,
                        int lineWidth, const QBrush *fill)
{
    if(p->device()->devType() == QInternal::Widget &&
       ((QWidget *)p->device())->inherits("QLineEdit")){
        int x2 = x+w-1;
        int y2 = y+h-1;
        p->setPen(g.dark());
        p->drawRect(x, y, w, h);
        p->setPen(g.mid());
        p->drawLine(x+1, y+1, x2-2, y+1);
        p->drawLine(x+1, y+1, x+1, y2-1);
    }
    else if(lineWidth != 2 || !sunken)
        QWindowsStyle::drawPanel(p, x, y, w, h, g, sunken, lineWidth, fill);
    else{
        QPen oldPen = p->pen();
        int x2 = x+w-1;
        int y2 = y+h-1;
        p->setPen(g.light());
        p->drawLine(x, y2, x2, y2);
        p->drawLine(x2, y, x2, y2);
        p->setPen(g.mid());
        p->drawLine(x, y, x2, y);
        p->drawLine(x, y, x, y2);
 
        p->setPen(g.midlight());
        p->drawLine(x+1, y2-1, x2-1, y2-1);
        p->drawLine(x2-1, y+1, x2-1, y2-1);
        p->setPen(g.dark());
        p->drawLine(x+1, y+1, x2-1, y+1);
        p->drawLine(x+1, y+1, x+1, y2-1);
        p->setPen(oldPen);
        if(fill){
            // I believe here we are only supposed to fill if there is a
            // specified fill brush...
            p->fillRect(x+2, y+2, w-4, h-4, *fill);
        }
    }
}


void LiquidStyle::adjustHSV(QPixmap &pix, int h, int s, int v)
{
    QBitmap *maskBmp = NULL;
    if(pix.mask())
        maskBmp = new QBitmap(*pix.mask());
    QImage img = pix.convertToImage();
    if(img.depth() != 32)
        img = img.convertDepth(32);
    unsigned int *data = (unsigned int *)img.bits();
    int total = img.width()*img.height();
    int current;
    QColor c;
    int oldH, oldS, oldV;
    if(v < 235)
        v += 20;
    else
        v = 255;
    float intensity = v/255.0;

    for(current=0; current<total; ++current){
        c.setRgb(data[current]);
        c.hsv(&oldH, &oldS, &oldV);
        oldV = (int)(oldV*intensity);
        c.setHsv(h, s, oldV);
        data[current] = c.rgb();
    }
    pix.convertFromImage(img);
    if(maskBmp)
        pix.setMask(*maskBmp);
}

void LiquidStyle::intensity(QPixmap &pix, float percent)
{
    QImage image = pix.convertToImage();
    int i, tmp, r, g, b;
    int segColors = image.depth() > 8 ? 256 : image.numColors();
    unsigned char *segTbl = new unsigned char[segColors];
    int pixels = image.depth() > 8 ? image.width()*image.height() :
        image.numColors();
    unsigned int *data = image.depth() > 8 ? (unsigned int *)image.bits() :
        (unsigned int *)image.colorTable();
 
    bool brighten = (percent >= 0);
    if(percent < 0)
        percent = -percent;
 
    if(brighten){ // keep overflow check out of loops
        for(i=0; i < segColors; ++i){
            tmp = (int)(i*percent);
            if(tmp > 255)
                tmp = 255;
            segTbl[i] = tmp;
        }
    }
    else{
        for(i=0; i < segColors; ++i){
            tmp = (int)(i*percent);
            if(tmp < 0)
                tmp = 0;
            segTbl[i] = tmp;
        }
    }
 
    if(brighten){ // same here
        for(i=0; i < pixels; ++i){
            r = qRed(data[i]);
            g = qGreen(data[i]);
            b = qBlue(data[i]);
            r = r + segTbl[r] > 255 ? 255 : r + segTbl[r];
            g = g + segTbl[g] > 255 ? 255 : g + segTbl[g];
            b = b + segTbl[b] > 255 ? 255 : b + segTbl[b];
            data[i] = qRgb(r, g, b);
        }
    }
    else{
        for(i=0; i < pixels; ++i){
            r = qRed(data[i]);
            g = qGreen(data[i]);
            b = qBlue(data[i]);
            r = r - segTbl[r] < 0 ? 0 : r - segTbl[r];
            g = g - segTbl[g] < 0 ? 0 : g - segTbl[g];
            b = b - segTbl[b] < 0 ? 0 : b - segTbl[b];
            data[i] = qRgb(r, g, b);
        }
    }
    delete [] segTbl;
    pix.convertFromImage(image);
}

void LiquidStyle::drawRoundRect(QPainter *p, int x, int y, int w, int h)
{
    int x2 = x+w-1;
    int y2 = y+h-1;
    p->drawLine(x+1, y, x2-1, y);
    p->drawLine(x, y+1, x, y2-1);
    p->drawLine(x+1, y2, x2-1, y2);
    p->drawLine(x2, y+1, x2, y2-1);
}

void LiquidStyle::drawSliderGroove (QPainter * p, int x, int y, int w,  int h,
                                    const QColorGroup &g, QCOORD,
                                    Orientation orient)
{
    bool isFocus = ((QWidget *)p->device())->hasFocus();
    QColor c = isFocus ? g.background().dark(120) : g.background();
    if(orient == Qt::Horizontal){
        int x2 = x+w-1;
        y+=2;
        p->setPen(c.dark(130));
        p->drawLine(x+1, y, x2-1, y);
        p->setPen(c.dark(150));
        p->drawLine(x, y+1, x2, y+1);
        p->setPen(c.dark(125));
        p->drawLine(x, y+2, x2, y+2);
        p->setPen(c.dark(130));
        p->drawLine(x, y+3, x2, y+3);
        p->setPen(c.dark(120));
        p->drawLine(x, y+4, x2, y+4);
        p->setPen(c.light(110));
        p->drawLine(x+1, y+5, x2-1, y+5);
    }
    else{
        int y2 = y+h-1;
        x+=2;
        p->setPen(c.dark(130));
        p->drawLine(x, y+1, x, y2-1);
        p->setPen(c.dark(150));
        p->drawLine(x+1, y, x+1, y2);
        p->setPen(c.dark(125));
        p->drawLine(x+2, y, x+2, y2);
        p->setPen(c.dark(130));
        p->drawLine(x+3, y, x+3, y2);
        p->setPen(c.dark(120));
        p->drawLine(x+4, y, x+4, y2);
        p->setPen(c.light(110));
        p->drawLine(x+5, y+1, x+5, y2-1);
    }
    //QWindowsStyle::drawSliderGroove(p, x, y, w, h, g, c, orient);

}

void LiquidStyle::drawSliderGrooveMask (QPainter * p, int x, int y, int w,
                                        int h, QCOORD, Orientation orient)
{
    p->fillRect(x, y, w, h, Qt::color0);
    p->setPen(Qt::color1);
    if(orient == Qt::Horizontal){
        int x2 = x+w-1;
        y+=2;
        p->drawLine(x+1, y, x2-1, y);
        p->fillRect(x, y+1, w, 4, Qt::color1);
        p->drawLine(x+1, y+5, x2-1, y+5);
    }
    else{
        int y2 = y+h-1;
        x+=2;
        p->drawLine(x, y+1, x, y2-1);
        p->fillRect(x+1, y, 4, h, Qt::color1);
        p->drawLine(x+5, y+1, x+5, y2-1);
    }
}

// I'm debating if to use QValueList or QList here. I like QValueList better,
// but QList handles pointers which is good for a lot of empty icons...

void LiquidStyle::loadCustomButtons()
{
    return; // TODO
    customBtnColorList.clear();
    customBtnIconList.clear();
    customBtnLabelList.clear();

//    KConfig *config = KGlobal::config();
//    QString oldGrp = config->group();
//    config->setGroup("MosfetButtons");

    QStrList iconList, colorList; //temp, we store QPixmaps and QColors
    iconList.setAutoDelete(true);
    colorList.setAutoDelete(true);
//    config->readListEntry("Labels", customBtnLabelList);
//    config->readListEntry("Icons", iconList);
//    config->readListEntry("Colors", colorList);

    const char *labelStr = customBtnLabelList.first();
    const char *colorStr = colorList.first();
    const char *iconStr = iconList.first();

//    KIconLoader *ldr = KGlobal::iconLoader();
    while(labelStr != NULL){
        QColor *c = new QColor;
        c->setNamedColor(QString(colorStr));
        customBtnColorList.append(c);

        QString tmpStr(iconStr);
        if(!tmpStr.isEmpty()){
            QPixmap *pixmap = 
                new QPixmap();//ldr->loadIcon(tmpStr, KIcon::Small));
            if(pixmap->isNull()){
                delete pixmap;
                customBtnIconList.append(NULL);
            }
            else
                customBtnIconList.append(pixmap);
        }
        else
            customBtnIconList.append(NULL);

        labelStr = customBtnLabelList.next();
        colorStr = colorList.next();
        iconStr = iconList.next();
    }
}

void LiquidStyle::applyCustomAttributes(QPushButton *btn)
{
    return; // TODO
    QString str = btn->text();
    if(str.isEmpty())
        return;
    while(str.contains('&') != 0)
        str = str.remove(str.find('&'), 1);

    const char *s;
    int idx = 0;
    for(s = customBtnLabelList.first(); s != NULL;
        ++idx, s = customBtnLabelList.next()){
        if(qstricmp(s, str.latin1()) == 0){
            QPalette pal = btn->palette();
            pal.setColor(QColorGroup::Button,
                         *customBtnColorList.at(idx));
            btn->setPalette(pal);
            /*
            if(customBtnIconList.at(idx) != NULL){
                QPixmap *pix = customBtnIconList.at(idx);
                btn->setIconSet(QIconSet(*pix));
            }*/
            break;
        }
    }
}

void LiquidStyle::unapplyCustomAttributes(QPushButton *btn)
{
    return; // TODO
    QString str = btn->text();
    if(str.isEmpty())
        return;
    while(str.contains('&') != 0)
        str = str.remove(str.find('&'), 1);

    const char *s;
    for(s = customBtnLabelList.first(); s != NULL; s = customBtnLabelList.next()){
        if(qstricmp(s, str.latin1()) == 0){
            btn->setPalette(QApplication::palette());
            btn->setIconSet(QIconSet());
            break;
        }
    }
}

// #include "liquid.moc"






/* vim: set noet sw=8 ts=8: */
