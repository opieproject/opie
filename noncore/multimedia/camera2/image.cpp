/**********************************************************************
** Copyright (C) 2000-2006 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This program is free software; you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by the
** Free Software Foundation; either version 2 of the License, or (at your
** option) any later version.
**
** A copy of the GNU GPL license version 2 is included in this package as
** LICENSE.GPL.
**
** This program is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** In addition, as a special exception Trolltech gives permission to link
** the code of this program with Qtopia applications copyrighted, developed
** and distributed by Trolltech under the terms of the Qtopia Personal Use
** License Agreement. You must comply with the GNU General Public License
** in all respects for all of the code used other than the applications
** licensed under the Qtopia Personal Use License Agreement. If you modify
** this file, you may extend this exception to your version of the file,
** but you are not obligated to do so. If you do not wish to do so, delete
** this exception statement from your version.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "image.h"

#include <qfileinfo.h>
#include <qimage.h>
#include <qstring.h>

/*!
   Returns an image created by loading the \a filename,
   and scalining it, preserving aspect ratio, to fit in
   \a width by \a height pixels.

   First availability: Qtopia 2.0
*/
QImage Image::loadScaled(const QString & filename, const int width, const int height,
                         ScaleMode mode)
{
    QImageIO iio;
    QString param;
    int w = width;
    int h = height;

    if (width == 0 || height == 0)
        return QImage();

    iio.setFileName(filename);

    //
    // Try and load and scale the image in one hit.
    //
    param.sprintf("GetHeaderInformation");
    iio.setParameters(param);
    iio.read();

    //
    // If we don't have bits(), read() understands parameters,
    // and we can setup for fast reading.  Otherwise, take the
    // performance hit and do things slowly.
    //
    if (!iio.image().bits()) {
        if ((iio.image().width() < w) && (iio.image().height() < h)) {
            param.sprintf("%s", "Fast");        // No Tr
        }
        else {
            int shrink_factor = 1;
            QString smode;
            switch (mode) {
            case ScaleMin:
                smode = "ScaleMin";
                shrink_factor = QMAX(iio.image().width() / w, iio.image().height() / h);
                break;
            case ScaleMax:
                smode = "ScaleMax";
                shrink_factor = QMIN(iio.image().width() / w, iio.image().height() / h);
                break;
            case ScaleFree:
                smode = "ScaleFree";
                shrink_factor = QMIN(iio.image().width() / w, iio.image().height() / h);
                break;
            }
            param.sprintf("Scale( %i, %i, %s )%s, Shrink( %i )",        // No tr
                          w, h, smode.latin1(), ", Fast", shrink_factor);       // No tr
        }

        iio.setParameters(param);
        iio.read();
        return iio.image();
    }

    if ((iio.image().width() > w) || (iio.image().height() > h)) {
        QSize s = aspectScaleSize(iio.image().width(), iio.image().height(), w, h, mode);
        return iio.image().smoothScale(s.width(), s.height());
    }

    return iio.image();
}

//
// Returns new size of image, scaled to target_width and target_height,
// whilst preserving aspect ratio.  Useful when it's not possible to
// scale an image prior to using it (eg. using RichText).  Should be in
// global image utils.
//
// NOTE
// - expensive for images whose drivers do not allow us to retrieve
//   parameters without reading the entire image.  Currently (20030930)
//   only the jpeg driver allows parameter reading.
//
QSize Image::loadScaledImageSize(const QString & filename, int target_width, int target_height,
                                 int maxscale, ScaleMode mode)
{
    QImageIO iio;
    iio.setFileName(filename);
    iio.setParameters("GetHeaderInformation");
    iio.read();

    int w = iio.image().width();
    int h = iio.image().height();

    //
    // Scaling up small pictures can be very ugly.
    // Leave them alone if they are too small.
    //
    if (maxscale && w * maxscale < target_width && h * maxscale < target_height)
        return QSize(w * maxscale, h * maxscale);

    return aspectScaleSize(w, h, target_width, target_height, mode);
}

// Load an image to be used as a portrait. Force its height to height,
// scale width to new height, and crop the width if it's still > width
// XXX : Should be using integer math
QImage Image::loadPortrait(const QString & filename, const int width, const int height)
{
    QImageIO iio;
    QString param;
    int w = width;
    int h = height;

    if (width == 0 || height == 0)
        return QImage();

    iio.setFileName(filename);

    //
    // Try and load and scale the image in one hit.
    //
    param.sprintf("GetHeaderInformation");
    iio.setParameters(param);
    iio.read();

    //
    // If we don't have bits(), read() understands parameters,
    // and we can setup for fast reading.  Otherwise, take the
    // performance hit and do things slowly.
    //
    if (!iio.image().bits()) {

        w = (int) ((double) iio.image().width() / (double) (iio.image().height()) *
                   (double) height);
        param.sprintf("Scale( %i, %i, ScaleFree )%s",   // No tr
                      w, h, ", Fast");  // No tr

        iio.setParameters(param);
        iio.read();

        if (w > width) {
            int x = (int) ((double) (w - width) / 2);
            QRect r(x, 0, width, height);
            return iio.image().copy(r);
        }
        else
            return iio.image();
    }
    else
        //scale width to the new height
        return sizeToPortrait(iio.image(), width, height);
}

QImage Image::sizeToPortrait(const QImage & image, const int width, const int height)
{
    int w = (image.width() * height) / image.height();
    if (w > width) {
        int x = (w - width) / 2;
        QRect r(x, 0, width, height);
        return image.smoothScale(w, height).copy(r);
    }
    else
        return image.smoothScale(width, height);
}

//
// Return (w x h) scaled to (target_width x target_height) with aspect
// ratio preserved.
//
QSize
    Image::aspectScaleSize(const int w, const int h, const int target_width,
                           const int target_height, ScaleMode mode)
{
    QSize s;
    if (mode == ScaleFree) {
        s = QSize(target_width, target_height);
    }
    else {
        bool useHeight = TRUE;
        int rw = target_height * w / h;

        if (mode == ScaleMin) {
            useHeight = (rw <= target_width);
        }
        else {                  // mode == ScaleMax
            useHeight = (rw >= target_width);
        }

        if (useHeight) {
            s = QSize(rw, target_height);
        }
        else {
            s = QSize(target_width, target_width * h / w);
        }
    }

    return s;
}

