/* This file is part of the KDE libraries
    Copyright (C) 1998, 1999 Christian Tibirna <ctibirna@total.net>
              (C) 1998, 1999 Daniel M. Duley <mosfet@kde.org>
              (C) 1998, 1999 Dirk A. Mueller <mueller@kde.org>

*/

// $Id: opixmapeffect.cpp,v 1.2 2004-03-13 19:51:49 zecke Exp $

/* QT */

#include <qimage.h>
#include <qpainter.h>

/* OPIE */

#include <opie2/opixmapeffect.h>
#include <opie2/oimageeffect.h>


using namespace Opie::Ui;
//======================================================================
//
// Gradient effects
//
//======================================================================


OPixmap& OPixmapEffect::gradient(OPixmap &pixmap, const QColor &ca,
	const QColor &cb, GradientType eff, int ncols)
{
    if(pixmap.depth() > 8 &&
       (eff == VerticalGradient || eff == HorizontalGradient)) {

        int rDiff, gDiff, bDiff;
        int rca, gca, bca /*, rcb, gcb, bcb*/;

        register int x, y;

        rDiff = (/*rcb = */ cb.red())   - (rca = ca.red());
        gDiff = (/*gcb = */ cb.green()) - (gca = ca.green());
        bDiff = (/*bcb = */ cb.blue())  - (bca = ca.blue());

        register int rl = rca << 16;
        register int gl = gca << 16;
        register int bl = bca << 16;

        int rcdelta = ((1<<16) / (eff == VerticalGradient ? pixmap.height() : pixmap.width())) * rDiff;
        int gcdelta = ((1<<16) / (eff == VerticalGradient ? pixmap.height() : pixmap.width())) * gDiff;
        int bcdelta = ((1<<16) / (eff == VerticalGradient ? pixmap.height() : pixmap.width())) * bDiff;

        QPainter p(&pixmap);

        // these for-loops could be merged, but the if's in the inner loop
        // would make it slow
        switch(eff) {
        case VerticalGradient:
            for ( y = 0; y < pixmap.height(); y++ ) {
                rl += rcdelta;
                gl += gcdelta;
                bl += bcdelta;

                p.setPen(QColor(rl>>16, gl>>16, bl>>16));
                p.drawLine(0, y, pixmap.width()-1, y);
            }
            break;
        case HorizontalGradient:
            for( x = 0; x < pixmap.width(); x++) {
                rl += rcdelta;
                gl += gcdelta;
                bl += bcdelta;

                p.setPen(QColor(rl>>16, gl>>16, bl>>16));
                p.drawLine(x, 0, x, pixmap.height()-1);
            }
            break;
        default:
            ;
        }
    }
    else {
        QImage image = OImageEffect::gradient(pixmap.size(), ca, cb,
                                              (OImageEffect::GradientType) eff, ncols);
        pixmap.convertFromImage(image);
    }

    return pixmap;
}


// -----------------------------------------------------------------------------

OPixmap& OPixmapEffect::unbalancedGradient(OPixmap &pixmap, const QColor &ca,
         const QColor &cb, GradientType eff, int xfactor, int yfactor,
         int ncols)
{
    QImage image = OImageEffect::unbalancedGradient(pixmap.size(), ca, cb,
                                 (OImageEffect::GradientType) eff,
                                 xfactor, yfactor, ncols);
    pixmap.convertFromImage(image);

    return pixmap;
}


//======================================================================
//
// Intensity effects
//
//======================================================================



OPixmap& OPixmapEffect::intensity(OPixmap &pixmap, float percent)
{
    QImage image = pixmap.convertToImage();
    OImageEffect::intensity(image, percent);
    pixmap.convertFromImage(image);

    return pixmap;
}


// -----------------------------------------------------------------------------

OPixmap& OPixmapEffect::channelIntensity(OPixmap &pixmap, float percent,
                                     RGBComponent channel)
{
    QImage image = pixmap.convertToImage();
    OImageEffect::channelIntensity(image, percent,
                   (OImageEffect::RGBComponent) channel);
    pixmap.convertFromImage(image);

    return pixmap;
}


//======================================================================
//
// Blend effects
//
//======================================================================


OPixmap& OPixmapEffect::blend(OPixmap &pixmap, float initial_intensity,
			  const QColor &bgnd, GradientType eff,
			  bool anti_dir, int ncols)
{

    QImage image = pixmap.convertToImage();
    if (image.depth() <=8)
        image = image.convertDepth(32); //Sloww..

    OImageEffect::blend(image, initial_intensity, bgnd,
                  (OImageEffect::GradientType) eff, anti_dir);

    unsigned int tmp;

    if(pixmap.depth() <= 8 ) {
        if ( ncols < 2 || ncols > 256 )
            ncols = 3;
        QColor *dPal = new QColor[ncols];
        for (int i=0; i<ncols; i++) {
            tmp = 0 + 255 * i / ( ncols - 1 );
            dPal[i].setRgb ( tmp, tmp, tmp );
        }
        OImageEffect::dither(image, dPal, ncols);
        pixmap.convertFromImage(image);
        delete [] dPal;
    }
    else
        pixmap.convertFromImage(image);

    return pixmap;
}


//======================================================================
//
// Hash effects
//
//======================================================================

OPixmap& OPixmapEffect::hash(OPixmap &pixmap, Lighting lite,
			 unsigned int spacing, int ncols)
{
    QImage image = pixmap.convertToImage();
    OImageEffect::hash(image, (OImageEffect::Lighting) lite, spacing);

    unsigned int tmp;

    if(pixmap.depth() <= 8 ) {
        if ( ncols < 2 || ncols > 256 )
            ncols = 3;
        QColor *dPal = new QColor[ncols];
        for (int i=0; i<ncols; i++) {
            tmp = 0 + 255 * i / ( ncols - 1 );
            dPal[i].setRgb ( tmp, tmp, tmp );
        }
        OImageEffect::dither(image, dPal, ncols);
        pixmap.convertFromImage(image);
        delete [] dPal;
    }
    else
        pixmap.convertFromImage(image);

    return pixmap;
}


//======================================================================
//
// Pattern effects
//
//======================================================================

#if 0
void OPixmapEffect::pattern(OPixmap &pixmap, const QColor &ca,
	const QColor &cb, unsigned pat[8])
{
    QImage img = pattern(pixmap.size(), ca, cb, pat);
    pixmap.convertFromImage(img);
}
#endif

// -----------------------------------------------------------------------------

OPixmap OPixmapEffect::pattern(const OPixmap& pmtile, QSize size,
                       const QColor &ca, const QColor &cb, int ncols)
{
    if (pmtile.depth() > 8)
	ncols = 0;

    QImage img = pmtile.convertToImage();
    OImageEffect::flatten(img, ca, cb, ncols);
    OPixmap pixmap;
    pixmap.convertFromImage(img);

    return OPixmapEffect::createTiled(pixmap, size);
}


// -----------------------------------------------------------------------------

OPixmap OPixmapEffect::createTiled(const OPixmap& pixmap, QSize size)
{
    OPixmap pix;

    QPainter p(&pix);
    p.drawTiledPixmap(0, 0, size.width(), size.height(), pixmap);

    return pix;
}


//======================================================================
//
// Fade effects
//
//======================================================================

OPixmap& OPixmapEffect::fade(OPixmap &pixmap, double val, const QColor &color)
{
    QImage img = pixmap.convertToImage();
    OImageEffect::fade(img, val, color);
    pixmap.convertFromImage(img);

    return pixmap;
}


// -----------------------------------------------------------------------------
OPixmap& OPixmapEffect::toGray(OPixmap &pixmap, bool fast)
{
    QImage img = pixmap.convertToImage();
    OImageEffect::toGray(img, fast);
    pixmap.convertFromImage(img);

    return pixmap;
}

// -----------------------------------------------------------------------------
OPixmap& OPixmapEffect::desaturate(OPixmap &pixmap, float desat)
{
    QImage img = pixmap.convertToImage();
    OImageEffect::desaturate(img, desat);
    pixmap.convertFromImage(img);

    return pixmap;
}
// -----------------------------------------------------------------------------
OPixmap& OPixmapEffect::contrast(OPixmap &pixmap, int c)
{
    QImage img = pixmap.convertToImage();
    OImageEffect::contrast(img, c);
    pixmap.convertFromImage(img);

    return pixmap;
}

//======================================================================
//
// Dither effects
//
//======================================================================

// -----------------------------------------------------------------------------
OPixmap& OPixmapEffect::dither(OPixmap &pixmap, const QColor *palette, int size)
{
    QImage img = pixmap.convertToImage();
    OImageEffect::dither(img, palette, size);
    pixmap.convertFromImage(img);

    return pixmap;
}

//======================================================================
//
// Other effects
//
//======================================================================

OPixmap OPixmapEffect::selectedPixmap( const OPixmap &pix, const QColor &col )
{
    QImage img = pix.convertToImage();
    OImageEffect::selectedImage(img, col);
    OPixmap outPix;
    outPix.convertFromImage(img);
    return outPix;
}
