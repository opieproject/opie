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



#include <qwidget.h>

#include "lib.h"

class QImage;
class XineVideoWidget : public QWidget {
    Q_OBJECT
public:
    XineVideoWidget( QWidget* parent,  const char* name );
    ~XineVideoWidget();
    QImage *image() { return m_image; };
    void setImage( QImage* image );
    void setImage( uchar* image, int width, int height, int linestep);
    void clear() ;

protected:
    void paintEvent( QPaintEvent* p );
    void resizeEvent ( QResizeEvent *r );

    void mousePressEvent ( QMouseEvent *e );
    void mouseReleaseEvent ( QMouseEvent *e );

signals:
    void clicked();
    void videoResized ( const QSize &s );

private:
    QRect m_lastframe;
    QRect m_thisframe;

    uchar* m_buff;
    int m_bytes_per_line_fb;
    int m_bytes_per_line_frame;
    int m_bytes_per_pixel;
    QImage* m_image;
    int m_rotation;
};

