/* This file is part of the KDE libraries
    Copyright (C) 1998, 1999 Christian Tibirna <ctibirna@total.net>
              (C) 1998, 1999 Daniel M. Duley <mosfet@kde.org>
              (C) 1998, 1999 Dirk A. Mueller <mueller@kde.org>

*/

// $Id: ogfxeffect.h,v 1.1 2002-06-29 23:45:19 sandman Exp $

#ifndef __OGFX_EFFECT_H
#define __OGFX_EFFECT_H

#include <qpixmap.h>
#include <qimage.h>

/**
 * This class includes various pixmap-based graphical effects.
 *
 * Everything is
 * static, so there is no need to create an instance of this class. You can
 * just call the static methods. They are encapsulated here merely to provide
 * a common namespace.
 */
class OGfxEffect
{
public:
    enum GradientType { VerticalGradient, HorizontalGradient,
			DiagonalGradient, CrossDiagonalGradient,
			PyramidGradient, RectangleGradient,
			PipeCrossGradient, EllipticGradient };
    enum RGBComponent { Red, Green, Blue };

    enum Lighting {NorthLite, NWLite, WestLite, SWLite,
		   SouthLite, SELite, EastLite, NELite};

    /**
     * Create a gradient from color a to color b of the specified type.
     *
     * @param pixmap The pixmap to process.
     * @param ca Color a.
     * @param cb Color b.
     * @param type The type of gradient.
     * @param ncols The number of colors to use when not running on a
     * truecolor display. The gradient will be dithered to this number of
     * colors. Pass 0 to prevent dithering.
     * @return Returns the generated pixmap, for convenience.
     */
    static QPixmap& gradient(QPixmap& pixmap, const QColor &ca, const QColor &cb,
                            GradientType type, int ncols=3);


    /**
     * Blend the provided pixmap into a background of the indicated color
     *
     * @param pixmap The pixmap to process.
     * @param initial_intensity this parameter takes values from -1 to 1:
     *              @li If positive, it tells how much to fade the image in its
     *                              less affected spot.
     *              @li If negative, it tells roughly indicates how much of the image
     *                              remains unaffected
     * @param bgnd Indicates the color of the background to blend in.
     * @param eff Lets you choose what kind of blending you like.
     * @param anti_dir Blend in the opposite direction (makes no much sense
     *                  with concentric blending effects).
     * @return Returns the @ref pixmap(), provided for convenience.
     */
    static QPixmap& blend(QPixmap& pixmap, float initial_intensity,
                         const QColor &bgnd, GradientType eff,
                         bool anti_dir=false, int ncols=3);


	static QImage& blend(QImage &image, float initial_intensity,
                            const QColor &bgnd, GradientType eff,
                            bool anti_dir);
};


#endif
