/*
                            This file is part of the Opie Project

                             Copyright (c)  2002 Max Reiss <harlekin@handhelds.org>
                             Copyright (c)  2002 L. Potter <ljp@llornkcor.com>
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

#ifndef VIDEO_WIDGET_H
#define VIDEO_WIDGET_H

#include <qwidget.h>
#include <qimage.h>
#include <qpixmap.h>
#include "xinevideowidget.h"

#include "mediawidget.h"

class QPixmap;
class QSlider;

enum VideoButtons {
    VideoStop = 0,
    VideoPlay,
//    VideoPause,
    VideoPrevious,
    VideoNext,
    VideoVolUp,
    VideoVolDown,
    VideoFullscreen
};

class VideoWidget : public MediaWidget {
    Q_OBJECT
public:
    VideoWidget( PlayListWidget &playList, MediaPlayerState &mediaPlayerState, QWidget* parent=0, const char* name=0 );
    ~VideoWidget();


    XineVideoWidget* vidWidget();
public slots:
    void updateSlider( long, long );
    void sliderPressed( );
    void sliderReleased( );
    void setFullscreen( bool b );
    void makeVisible();
    void backToNormal();
    void setPosition( long );

public:
    virtual void setPlaying( bool b);
    virtual void setLength( long );
    virtual void setDisplayType( MediaPlayerState::DisplayType displayType );

signals:
    void moreClicked();
    void lessClicked();
    void moreReleased();
    void lessReleased();
    void sliderMoved( long );
    void videoResized ( const QSize &s );

protected:

    void resizeEvent( QResizeEvent * );
    void paintEvent( QPaintEvent *pe );
    void showEvent( QShowEvent *se );
    void mouseMoveEvent( QMouseEvent *event );
    void mousePressEvent( QMouseEvent *event );
    void mouseReleaseEvent( QMouseEvent *event );
    void keyReleaseEvent( QKeyEvent *e);

private:
//    Ticker songInfo;
    QPixmap pixBg;
    QImage  imgUp;
    QImage  imgDn;
    QBitmap *masks[7];
    QPixmap *buttonPixUp[7];
    QPixmap *buttonPixDown[7];
    QString skin;
//    QPixmap *pixmaps[4];


    void paintButton( QPainter *p, int i );
    void toggleButton( int );
    void setToggleButton( int, bool );

    QString backgroundPix;
    QSlider *slider;
    QPixmap *pixmaps[3];
    QImage *currentFrame;
    int scaledWidth;
    int scaledHeight;
    XineVideoWidget* videoFrame;
};

#endif // VIDEO_WIDGET_H



