
/*
                            This file is part of the Opie Project

                             Copyright (c)  2002 Max Reiss <harlekin@handhelds.org>
                             Copyright (c)  2002 LJP <>
                             Copyright (c)  2002 Holger Freyther <zecke@handhelds.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include <qimage.h>
#include <qpainter.h>
#include <qgfx_qws.h>
#include <qdirectpainter_qws.h>
#include <qsize.h>

#include <qpe/resource.h>

#include "xinevideowidget.h"

XineVideoWidget::XineVideoWidget( int width,
                                  int height,
                                  QWidget* parent,
                                  const char* name )
    : QWidget( parent, name )
{
    m_image = new QImage( width, height,  qt_screen->depth() );
    m_buff = 0;
    setBackgroundMode( NoBackground);
/*    QImage image = Resource::loadImage("SoundPlayer");
    image = image.smoothScale( width, height );

    m_image = new QImage( image );*/
}
XineVideoWidget::~XineVideoWidget() {
    delete m_image;
}
void XineVideoWidget::clear() {
    m_buff = 0;
    repaint();
}
void XineVideoWidget::paintEvent( QPaintEvent* e ) {
    qWarning("painting");
    QPainter p(this );
    p.setBrush( QBrush( Qt::black ) );
    p.drawRect( rect() );
    if (m_buff == 0 )
        p.drawImage( 0, 0, *m_image );
    else {
        qWarning("paitnevent\n");

        QDirectPainter dp( this );
        uchar* dst =  dp.frameBuffer() + (m_yOff + dp.yOffset()  ) * linestep +
                        (m_xOff + dp.xOffset() )  * m_bytes_per_pixel;
        uchar* frame = m_buff;
        for(int  y = 0; y < m_Height; y++ ) {
            memcpy( dst, frame, m_bytes );
            frame += m_bytes;
            dst += linestep;
        }
        // QVFB hack by MArtin Jones
//        QPainter dp2(this);
        //      dp2.fillRect( rect(), QBrush( NoBrush ) );
    }
//    QWidget::paintEvent( e );
}
int XineVideoWidget::height() const{
    return m_image->height();
}
int XineVideoWidget::width() const{
    return m_image->width();
}
void XineVideoWidget::setImage( QImage* image ) {
    delete m_image;
    m_image = image;
}
void XineVideoWidget::setImage( uchar* image, int yoffsetXLine,
                                int xoffsetXBytes,  int width,
                                int height, int linestep,  int bytes, int bpp ) {
/*    if (m_buff != 0 )
        free(m_buff );
*/
    m_buff = image;
    m_yOff = yoffsetXLine;
    m_xOff = xoffsetXBytes;
    m_Width = width;
    m_Height = height;
    this->linestep = linestep;
    m_bytes = bytes;
    m_bytes_per_pixel = bpp;
    ////
    qWarning("width %d  %d", width, height );
/*    QDirectPainter dp( this );
    uchar* dst =  dp.frameBuffer() + (m_yOff + dp.yOffset()  ) * linestep +
                  (m_xOff + dp.xOffset() )  * m_bytes_per_pixel;
    uchar* frame = m_buff;
    for(int  y = 0; y < m_Height; y++ ) {
        memcpy( dst, frame, m_bytes );
        frame += m_bytes;
        dst += linestep;
    }
    // QVFB hack
    QPainter dp2(this);
    dp2.fillRect( rect(), QBrush( NoBrush ) );
*/
}
