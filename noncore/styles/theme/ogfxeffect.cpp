/* This file is part of the KDE libraries
    Copyright (C) 1998, 1999 Christian Tibirna <ctibirna@total.net>
              (C) 1998, 1999 Daniel M. Duley <mosfet@kde.org>
              (C) 1998, 1999 Dirk A. Mueller <mueller@kde.org>

*/

// $Id: ogfxeffect.cpp,v 1.4 2002-11-03 11:14:01 zecke Exp $

#include <qimage.h>
#include <qpainter.h>

#include <qpe/qmath.h>

#include "ogfxeffect.h"

#include <cstdlib>
#include <cmath>

//======================================================================
//
// Gradient effects
//
//======================================================================


QPixmap& OGfxEffect::gradient(QPixmap &pixmap, const QColor &ca,
	const QColor &cb, GradientType eff, int ncols)
{
	QImage image = gradient(pixmap.size(), ca, cb, eff, ncols);
	pixmap.convertFromImage(image);

	return pixmap;
}

QImage OGfxEffect::gradient(const QSize &size, const QColor &ca,
                            const QColor &cb, GradientType eff, int /*ncols*/)
{    
	int rDiff, gDiff, bDiff;
	int rca, gca, bca, rcb, gcb, bcb;
	
	QImage image(size, 32);

	if (size.width() == 0 || size.height() == 0) {
		qDebug ( "WARNING: OGfxEffect::gradient: invalid image" );
		return image;
	}

    register int x, y;

    rDiff = (rcb = cb.red())   - (rca = ca.red());
    gDiff = (gcb = cb.green()) - (gca = ca.green());
    bDiff = (bcb = cb.blue())  - (bca = ca.blue());

    if( eff == VerticalGradient || eff == HorizontalGradient ){

        uint *p;
        uint rgb;

        register int rl = rca << 16;
        register int gl = gca << 16;
        register int bl = bca << 16;

        if( eff == VerticalGradient ) {

            int rcdelta = ((1<<16) / size.height()) * rDiff;
            int gcdelta = ((1<<16) / size.height()) * gDiff;
            int bcdelta = ((1<<16) / size.height()) * bDiff;

            for ( y = 0; y < size.height(); y++ ) {
                p = (uint *) image.scanLine(y);

                rl += rcdelta;
                gl += gcdelta;
                bl += bcdelta;

                rgb = qRgb( (rl>>16), (gl>>16), (bl>>16) );

                for( x = 0; x < size.width(); x++ ) {
                    *p = rgb;
                    p++;
                }
            }

        }
        else {                  // must be HorizontalGradient

            unsigned int *o_src = (unsigned int *)image.scanLine(0);
            unsigned int *src = o_src;

            int rcdelta = ((1<<16) / size.width()) * rDiff;
            int gcdelta = ((1<<16) / size.width()) * gDiff;
            int bcdelta = ((1<<16) / size.width()) * bDiff;

            for( x = 0; x < size.width(); x++) {

                rl += rcdelta;
                gl += gcdelta;
                bl += bcdelta;

                *src++ = qRgb( (rl>>16), (gl>>16), (bl>>16));
            }

            src = o_src;

            // Believe it or not, manually copying in a for loop is faster
            // than calling memcpy for each scanline (on the order of ms...).
            // I think this is due to the function call overhead (mosfet).

            for (y = 1; y < size.height(); ++y) {

                p = (unsigned int *)image.scanLine(y);
                src = o_src;
                for(x=0; x < size.width(); ++x)
                    *p++ = *src++;
            }
        }
    }

    else {

        float rfd, gfd, bfd;
        float rd = rca, gd = gca, bd = bca;

        unsigned char *xtable[3];
        unsigned char *ytable[3];

        unsigned int w = size.width(), h = size.height();
        xtable[0] = new unsigned char[w];
        xtable[1] = new unsigned char[w];
        xtable[2] = new unsigned char[w];
        ytable[0] = new unsigned char[h];
        ytable[1] = new unsigned char[h];
        ytable[2] = new unsigned char[h];
        w*=2, h*=2;

        if ( eff == DiagonalGradient || eff == CrossDiagonalGradient) {
            // Diagonal dgradient code inspired by BlackBox (mosfet)
            // BlackBox dgradient is (C) Brad Hughes, <bhughes@tcac.net> and
            // Mike Cole <mike@mydot.com>.

            rfd = (float)rDiff/w;
            gfd = (float)gDiff/w;
            bfd = (float)bDiff/w;

            int dir;
            for (x = 0; x < size.width(); x++, rd+=rfd, gd+=gfd, bd+=bfd) {
                dir = eff == DiagonalGradient? x : size.width() - x - 1;
                xtable[0][dir] = (unsigned char) rd;
                xtable[1][dir] = (unsigned char) gd;
                xtable[2][dir] = (unsigned char) bd;
            }
            rfd = (float)rDiff/h;
            gfd = (float)gDiff/h;
            bfd = (float)bDiff/h;
            rd = gd = bd = 0;
            for (y = 0; y < size.height(); y++, rd+=rfd, gd+=gfd, bd+=bfd) {
                ytable[0][y] = (unsigned char) rd;
                ytable[1][y] = (unsigned char) gd;
                ytable[2][y] = (unsigned char) bd;
            }

            for (y = 0; y < size.height(); y++) {
                unsigned int *scanline = (unsigned int *)image.scanLine(y);
                for (x = 0; x < size.width(); x++) {
                    scanline[x] = qRgb(xtable[0][x] + ytable[0][y],
                                       xtable[1][x] + ytable[1][y],
                                       xtable[2][x] + ytable[2][y]);
                }
            }
        }

        else if (eff == RectangleGradient ||
                 eff == PyramidGradient ||
                 eff == PipeCrossGradient ||
                 eff == EllipticGradient)
        {
            int rSign = rDiff>0? 1: -1;
            int gSign = gDiff>0? 1: -1;
            int bSign = bDiff>0? 1: -1;

            rfd = (float)rDiff / size.width();
            gfd = (float)gDiff / size.width();
            bfd = (float)bDiff / size.width();

            rd = (float)rDiff/2;
            gd = (float)gDiff/2;
            bd = (float)bDiff/2;

            for (x = 0; x < size.width(); x++, rd-=rfd, gd-=gfd, bd-=bfd)
            {
                xtable[0][x] = (unsigned char) abs((int)rd);
                xtable[1][x] = (unsigned char) abs((int)gd);
                xtable[2][x] = (unsigned char) abs((int)bd);
            }

            rfd = (float)rDiff/size.height();
            gfd = (float)gDiff/size.height();
            bfd = (float)bDiff/size.height();

            rd = (float)rDiff/2;
            gd = (float)gDiff/2;
            bd = (float)bDiff/2;

            for (y = 0; y < size.height(); y++, rd-=rfd, gd-=gfd, bd-=bfd)
            {
                ytable[0][y] = (unsigned char) abs((int)rd);
                ytable[1][y] = (unsigned char) abs((int)gd);
                ytable[2][y] = (unsigned char) abs((int)bd);
            }
            unsigned int rgb;
            int h = (size.height()+1)>>1;
            for (y = 0; y < h; y++) {
                unsigned int *sl1 = (unsigned int *)image.scanLine(y);
                unsigned int *sl2 = (unsigned int *)image.scanLine(QMAX(size.height()-y-1, y));

                int w = (size.width()+1)>>1;
                int x2 = size.width()-1;

                for (x = 0; x < w; x++, x2--) {
                    rgb = 0;
                    if (eff == PyramidGradient) {
                        rgb = qRgb(rcb-rSign*(xtable[0][x]+ytable[0][y]),
                                   gcb-gSign*(xtable[1][x]+ytable[1][y]),
                                   bcb-bSign*(xtable[2][x]+ytable[2][y]));
                    }
                    if (eff == RectangleGradient) {
                        rgb = qRgb(rcb - rSign *
                                   QMAX(xtable[0][x], ytable[0][y]) * 2,
                                   gcb - gSign *
                                   QMAX(xtable[1][x], ytable[1][y]) * 2,
                                   bcb - bSign *
                                   QMAX(xtable[2][x], ytable[2][y]) * 2);
                    }
                    if (eff == PipeCrossGradient) {
                        rgb = qRgb(rcb - rSign *
                                   QMIN(xtable[0][x], ytable[0][y]) * 2,
                                   gcb - gSign *
                                   QMIN(xtable[1][x], ytable[1][y]) * 2,
                                   bcb - bSign *
                                   QMIN(xtable[2][x], ytable[2][y]) * 2);
                    }
                    if (eff == EllipticGradient) {
                        rgb = qRgb(rcb - rSign *
                                   (int)sqrt((xtable[0][x]*xtable[0][x] +
                                              ytable[0][y]*ytable[0][y])*2.0),
                                   gcb - gSign *
                                   (int)sqrt((xtable[1][x]*xtable[1][x] +
                                              ytable[1][y]*ytable[1][y])*2.0),
                                   bcb - bSign *
                                   (int)sqrt((xtable[2][x]*xtable[2][x] +
                                              ytable[2][y]*ytable[2][y])*2.0));
                    }

                    sl1[x] = sl2[x] = rgb;
                    sl1[x2] = sl2[x2] = rgb;
                }
            }
        }

        delete [] xtable[0];
        delete [] xtable[1];
        delete [] xtable[2];
        delete [] ytable[0];
        delete [] ytable[1];
        delete [] ytable[2];
    }
    return image;
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
