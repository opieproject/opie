/**********************************************************************
** Copyright (C) 2002 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Opie Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#include "resource.h"

#define PIXMAPPATH "/home/mickey/work/opie/pics"

namespace Resource
{

QPixmap loadPixmap( const QString& pix )
{
    QString filename;
    filename.sprintf( "%s/%s.png", (const char*) PIXMAPPATH, (const char*) pix );
    QPixmap pixmap( filename );
    if ( pixmap.isNull() )
    {
        qDebug( "Wellenreiter::Resource: can't find pixmap " + filename );
    }
    return pixmap;
};

};
