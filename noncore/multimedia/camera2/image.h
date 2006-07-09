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

#ifndef IMAGE_H
#define IMAGE_H

#include <qimage.h>
#include <qtopia/qpeglobal.h>

class Image {
public:
    // namespace
    enum ScaleMode { ScaleFree, ScaleMin, ScaleMax };
    static QImage loadScaled(const QString &filename, const int width, const int height, ScaleMode mode=ScaleMin);
    static QSize  loadScaledImageSize(const QString &filename, int target_width, int target_height, int maxscale=1, ScaleMode mode=ScaleMin);
    static QImage loadPortrait(const QString &filename, const int width, const int height);
    static QImage sizeToPortrait(const QImage &image, const int width, const int height);
    static QSize  aspectScaleSize(const int w, const int h, const int target_width, const int target_height, ScaleMode mode=ScaleMin);
};

#endif // IMAGE_H
