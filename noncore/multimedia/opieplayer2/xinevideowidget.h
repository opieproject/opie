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

class QImage;
class XineVideoWidget : public QWidget {
    Q_OBJECT
public:
    XineVideoWidget( int width,  int height, QWidget* parent,  const char* name );
    ~XineVideoWidget();
    QImage *image() { return m_image; };
    void setImage( QImage* image );
    void setImage( uchar* image, int yoffsetXLine, int xoffsetXBytes,
                   int width, int height,  int linestep,  int bytes,  int bpp);
    int width() const;
    int height() const;
    void clear() ;
protected:
    void paintEvent( QPaintEvent* p );
private:
	QRect m_lastframe;
	QRect m_thisframe;

    int m_wid;
    int m_height;
    int m_yOff, m_xOff;
    uchar* m_buff;
    int m_Width,  m_Height, linestep;
    int m_bytes;
    int m_bytes_per_pixel;
    QImage* m_image;

};

