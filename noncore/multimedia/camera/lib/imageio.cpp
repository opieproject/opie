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

#include "imageio.h"

#include <opie2/odebug.h>
#include <qimage.h>

using namespace Opie::Core;

void bufferToImage( int _width, int _height, unsigned char* bp, QImage* image )
{
    unsigned char* p;

    image->create( _width, _height, 16 );
    for ( int i = 0; i < _height; ++i )
    {
        p = image->scanLine( i );
        for ( int j = 0; j < _width; j++ )
        {
            *p = *bp;
            p++;
            bp++;
            *p = *bp;
            p++;
            bp++;
        }
    }
}


void imageToFile( QImage* i, const QString& name, const QString& format, int quality )
{
    QImage im = i->convertDepth( 32 );
    bool result = im.save( name, format, quality );
    if ( !result )
    {
        oerr << "imageio-Problem while writing to " << name << oendl;
    }
    else
    {
        odebug << format << "-image has been successfully captured" << oendl;
    }
}
