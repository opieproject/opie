/* This file is part of the KDE libraries
    Copyright (C) 1998, 1999 Christian Tibirna <ctibirna@total.net>
              (C) 1998, 1999 Daniel M. Duley <mosfet@kde.org>
              (C) 1998, 1999 Dirk A. Mueller <mueller@kde.org>

*/

// $Id: ogfxeffect.cpp,v 1.2 2002-07-01 23:38:26 sandman Exp $

#include <qimage.h>
#include <qpainter.h>

#include <qpe/qmath.h>

#include "ogfxeffect.h"


//======================================================================
//
// Gradient effects
//
//======================================================================


QPixmap& OGfxEffect::gradient(QPixmap &pixmap, const QColor &ca,
	const QColor &cb, GradientType eff, int /*ncols*/)
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
//        QImage image = OGfxEffect::gradient(pixmap.size(), ca, cb,
//                                              (OGfxEffect::GradientType) eff, ncols);
//        pixmap.convertFromImage(image);
    }

    return pixmap;
}


//======================================================================
//
// Blend effects
//
//======================================================================


QPixmap& OGfxEffect::blend(QPixmap &pixmap, float initial_intensity,
			  const QColor &bgnd, GradientType eff,
			  bool anti_dir, int /*ncols*/)
{
    QImage image = pixmap.convertToImage();
    OGfxEffect::blend(image, initial_intensity, bgnd, eff, anti_dir);

	if ( pixmap. depth ( ) <= 8 )
		image. convertDepth ( pixmap. depth ( ));

    pixmap.convertFromImage(image);

    return pixmap;
}


QImage& OGfxEffect::blend(QImage &image, float initial_intensity,
                            const QColor &bgnd, GradientType eff,
                            bool anti_dir)
{
    if (image.width() == 0 || image.height() == 0) {
      qDebug ( "Invalid image\n" );
      return image;
    }

    int r_bgnd = bgnd.red(), g_bgnd = bgnd.green(), b_bgnd = bgnd.blue();
    int r, g, b;
    int ind;

    unsigned int xi, xf, yi, yf;
    unsigned int a;

    // check the boundaries of the initial intesity param
    float unaffected = 1;
    if (initial_intensity >  1) initial_intensity =  1;
    if (initial_intensity < -1) initial_intensity = -1;
    if (initial_intensity < 0) {
        unaffected = 1. + initial_intensity;
        initial_intensity = 0;
    }


    float intensity = initial_intensity;
    float var = 1. - initial_intensity;

    if (anti_dir) {
        initial_intensity = intensity = 1.;
        var = -var;
    }

    register int x, y;

    unsigned int *data =  (unsigned int *)image.bits();

    if( eff == VerticalGradient || eff == HorizontalGradient ) {

        // set the image domain to apply the effect to
        xi = 0, xf = image.width();
        yi = 0, yf = image.height();
        if (eff == VerticalGradient) {
            if (anti_dir) yf = (int)(image.height() * unaffected);
            else yi = (int)(image.height() * (1 - unaffected));
        }
        else {
            if (anti_dir) xf = (int)(image.width() * unaffected);
            else xi = (int)(image.height() * (1 - unaffected));
        }

        var /= (eff == VerticalGradient?yf-yi:xf-xi);

        for (y = yi; y < (int)yf; y++) {
            intensity = eff == VerticalGradient? intensity + var :
                initial_intensity;
            for (x = xi; x < (int)xf ; x++) {
                if (eff == HorizontalGradient) intensity += var;
                ind = x + image.width()  * y ;
                r = qRed  (data[ind]) + (int)(intensity *
                                              (r_bgnd - qRed  (data[ind])));
                g = qGreen(data[ind]) + (int)(intensity *
                                              (g_bgnd - qGreen(data[ind])));
                b = qBlue (data[ind]) + (int)(intensity *
                                              (b_bgnd - qBlue (data[ind])));
                if (r > 255) r = 255; if (r < 0 ) r = 0;
                if (g > 255) g = 255; if (g < 0 ) g = 0;
                if (b > 255) b = 255; if (b < 0 ) b = 0;
                a = qAlpha(data[ind]);
                data[ind] = qRgba(r, g, b, a);
            }
        }
    }
    else if (eff == DiagonalGradient  || eff == CrossDiagonalGradient) {
        float xvar = var / 2 / image.width();  // / unaffected;
        float yvar = var / 2 / image.height(); // / unaffected;
        float tmp;

        for (x = 0; x < image.width() ; x++) {
            tmp =  xvar * (eff == DiagonalGradient? x : image.width()-x-1);
            for (y = 0; y < image.height() ; y++) {
                intensity = initial_intensity + tmp + yvar * y;
                ind = x + image.width()  * y ;
                r = qRed  (data[ind]) + (int)(intensity *
                                              (r_bgnd - qRed  (data[ind])));
                g = qGreen(data[ind]) + (int)(intensity *
                                              (g_bgnd - qGreen(data[ind])));
                b = qBlue (data[ind]) + (int)(intensity *
                                              (b_bgnd - qBlue (data[ind])));
                if (r > 255) r = 255; if (r < 0 ) r = 0;
                if (g > 255) g = 255; if (g < 0 ) g = 0;
                if (b > 255) b = 255; if (b < 0 ) b = 0;
                a = qAlpha(data[ind]);
                data[ind] = qRgba(r, g, b, a);
            }
        }
    }

    else if (eff == RectangleGradient || eff == EllipticGradient) {
        float xvar;
        float yvar;

        for (x = 0; x < image.width() / 2 + image.width() % 2; x++) {
            xvar = var / image.width()  * (image.width() - x*2/unaffected-1);
            for (y = 0; y < image.height() / 2 + image.height() % 2; y++) {
                yvar = var / image.height()   * (image.height() - y*2/unaffected -1);

                if (eff == RectangleGradient)
                    intensity = initial_intensity + QMAX(xvar, yvar);
                else
                    intensity = initial_intensity + qSqrt(xvar * xvar + yvar * yvar);
                if (intensity > 1) intensity = 1;
                if (intensity < 0) intensity = 0;

                //NW
                ind = x + image.width()  * y ;
                r = qRed  (data[ind]) + (int)(intensity *
                                              (r_bgnd - qRed  (data[ind])));
                g = qGreen(data[ind]) + (int)(intensity *
                                              (g_bgnd - qGreen(data[ind])));
                b = qBlue (data[ind]) + (int)(intensity *
                                              (b_bgnd - qBlue (data[ind])));
                if (r > 255) r = 255; if (r < 0 ) r = 0;
                if (g > 255) g = 255; if (g < 0 ) g = 0;
                if (b > 255) b = 255; if (b < 0 ) b = 0;
                a = qAlpha(data[ind]);
                data[ind] = qRgba(r, g, b, a);

                //NE
                ind = image.width() - x - 1 + image.width()  * y ;
                r = qRed  (data[ind]) + (int)(intensity *
                                              (r_bgnd - qRed  (data[ind])));
                g = qGreen(data[ind]) + (int)(intensity *
                                              (g_bgnd - qGreen(data[ind])));
                b = qBlue (data[ind]) + (int)(intensity *
                                              (b_bgnd - qBlue (data[ind])));
                if (r > 255) r = 255; if (r < 0 ) r = 0;
                if (g > 255) g = 255; if (g < 0 ) g = 0;
                if (b > 255) b = 255; if (b < 0 ) b = 0;
                a = qAlpha(data[ind]);
                data[ind] = qRgba(r, g, b, a);
            }
        }

        //CT  loop is doubled because of stupid central row/column issue.
        //    other solution?
        for (x = 0; x < image.width() / 2; x++) {
            xvar = var / image.width()  * (image.width() - x*2/unaffected-1);
            for (y = 0; y < image.height() / 2; y++) {
                yvar = var / image.height()   * (image.height() - y*2/unaffected -1);

                if (eff == RectangleGradient)
                    intensity = initial_intensity + QMAX(xvar, yvar);
                else
                    intensity = initial_intensity + qSqrt(xvar * xvar + yvar * yvar);
                if (intensity > 1) intensity = 1;
                if (intensity < 0) intensity = 0;

                //SW
                ind = x + image.width()  * (image.height() - y -1) ;
                r = qRed  (data[ind]) + (int)(intensity *
                                              (r_bgnd - qRed  (data[ind])));
                g = qGreen(data[ind]) + (int)(intensity *
                                              (g_bgnd - qGreen(data[ind])));
                b = qBlue (data[ind]) + (int)(intensity *
                                              (b_bgnd - qBlue (data[ind])));
                if (r > 255) r = 255; if (r < 0 ) r = 0;
                if (g > 255) g = 255; if (g < 0 ) g = 0;
                if (b > 255) b = 255; if (b < 0 ) b = 0;
                a = qAlpha(data[ind]);
                data[ind] = qRgba(r, g, b, a);

                //SE
                ind = image.width()-x-1 + image.width() * (image.height() - y - 1) ;
                r = qRed  (data[ind]) + (int)(intensity *
                                              (r_bgnd - qRed  (data[ind])));
                g = qGreen(data[ind]) + (int)(intensity *
                                              (g_bgnd - qGreen(data[ind])));
                b = qBlue (data[ind]) + (int)(intensity *
                                              (b_bgnd - qBlue (data[ind])));
                if (r > 255) r = 255; if (r < 0 ) r = 0;
                if (g > 255) g = 255; if (g < 0 ) g = 0;
                if (b > 255) b = 255; if (b < 0 ) b = 0;
                a = qAlpha(data[ind]);
                data[ind] = qRgba(r, g, b, a);
            }
        }
    }

    else 
    	qDebug ( "not implemented\n" );

    return image;
}

#if 0
// Not very efficient as we create a third big image...
//
QImage& KQGfxEffect::blend(QImage &image1, QImage &image2,
			    GradientType gt, int xf, int yf)
{
  if (image1.width() == 0 || image1.height() == 0 ||
      image2.width() == 0 || image2.height() == 0)
    return image1;

  QImage image3;

  image3 = KQGfxEffect::unbalancedGradient(image1.size(),
				    QColor(0,0,0), QColor(255,255,255),
				    gt, xf, yf, 0);

  return blend(image1,image2,image3, Red); // Channel to use is arbitrary
}
#endif
