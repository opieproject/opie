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

#include <qpe/resource.h>
#include <qpe/mediaplayerplugininterface.h>
#include <qpe/config.h>

#include <qwidget.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qslider.h>
#include <qdrawutil.h>
#include "videowidget.h"
#include "mediaplayerstate.h"


#ifdef Q_WS_QWS
# define USE_DIRECT_PAINTER
# include <qdirectpainter_qws.h>
# include <qgfxraster_qws.h>
#endif


extern MediaPlayerState *mediaPlayerState;


static const int xo = 2; // movable x offset
static const int yo = 0; // movable y offset


struct MediaButton {
    int  xPos, yPos;
    bool isToggle, isHeld, isDown;
    int  controlType;
};


// Layout information for the videoButtons (and if it is a toggle button or not)
MediaButton videoButtons[] = {
    { 5+0*32+xo, 200+yo, FALSE, FALSE, FALSE, 4 }, // previous
    { 5+1*32+xo, 200+yo, FALSE, FALSE, FALSE, 1 }, // stop
    { 5+2*32+xo, 200+yo,  TRUE, FALSE, FALSE, 0 }, // play
    { 5+3*32+xo, 200+yo,  TRUE, FALSE, FALSE, 2 }, // pause
    { 5+4*32+xo, 200+yo, FALSE, FALSE, FALSE, 3 }, // next
    { 5+5*32+xo, 200+yo, FALSE, FALSE, FALSE, 8 }, // playlist
    { 5+6*32+xo, 200+yo,  TRUE, FALSE, FALSE, 9 }  // fullscreen
};


static const int numButtons = (sizeof(videoButtons)/sizeof(MediaButton));


VideoWidget::VideoWidget(QWidget* parent, const char* name, WFlags f) :
    QWidget( parent, name, f ), scaledWidth( 0 ), scaledHeight( 0 ) {
    setCaption( tr("OpiePlayer - Video") );
    Config cfg("OpiePlayer");
    cfg.setGroup("VideoWidget");

    QString backgroundPix, Button0aPix, Button0bPix, controlsPix;
    backgroundPix=cfg.readEntry( "backgroundPix", "opieplayer/metalFinish");
    Button0aPix=cfg.readEntry( "Button0aPix", "opieplayer/mediaButton0a");
    Button0bPix=cfg.readEntry( "Button0bPix","opieplayer/mediaButton0b");
    controlsPix=cfg.readEntry( "controlsPix","opieplayer/mediaControls0" );

    setBackgroundPixmap( Resource::loadPixmap( backgroundPix) );
    pixmaps[0] = new QPixmap( Resource::loadPixmap( Button0aPix ) );
    pixmaps[1] = new QPixmap( Resource::loadPixmap( Button0bPix ) );
    pixmaps[2] = new QPixmap( Resource::loadPixmap( controlsPix) );
    currentFrame = new QImage( 220 + 2, 160, (QPixmap::defaultDepth() == 16) ? 16 : 32 );

    slider = new QSlider( Qt::Horizontal, this );
    slider->setMinValue( 0 );
    slider->setMaxValue( 1 );
    slider->setBackgroundPixmap( Resource::loadPixmap( backgroundPix ) );
    slider->setFocusPolicy( QWidget::NoFocus );
    slider->setGeometry( QRect( 7, 250, 220, 20 ) );

    connect( slider, SIGNAL( sliderPressed() ), this, SLOT( sliderPressed() ) );
    connect( slider, SIGNAL( sliderReleased() ), this, SLOT( sliderReleased() ) );

    connect( mediaPlayerState, SIGNAL( lengthChanged(long) ),  this, SLOT( setLength(long) ) );
    connect( mediaPlayerState, SIGNAL( positionChanged(long) ),this, SLOT( setPosition(long) ) );
    connect( mediaPlayerState, SIGNAL( positionUpdated(long) ),this, SLOT( setPosition(long) ) );
    connect( mediaPlayerState, SIGNAL( viewChanged(char) ),    this, SLOT( setView(char) ) );
    connect( mediaPlayerState, SIGNAL( pausedToggled(bool) ),  this, SLOT( setPaused(bool) ) );
    connect( mediaPlayerState, SIGNAL( playingToggled(bool) ), this, SLOT( setPlaying(bool) ) );

    // Intialise state
    setLength( mediaPlayerState->length() );
    setPosition( mediaPlayerState->position() );
    setFullscreen( mediaPlayerState->fullscreen() );
    setPaused( mediaPlayerState->paused() );
    setPlaying( mediaPlayerState->playing() );

    videoFrame = new XineVideoWidget( 200, 150 ,this, "Video frame" );
    videoFrame->setGeometry( QRect( 10, 20, 220, 160  ) );
}


VideoWidget::~VideoWidget() {
    for ( int i = 0; i < 3; i++ ) {
        delete pixmaps[i];
    }
    delete currentFrame;
}


static bool videoSliderBeingMoved = FALSE;


void VideoWidget::sliderPressed() {
    videoSliderBeingMoved = TRUE;
}


void VideoWidget::sliderReleased() {
    videoSliderBeingMoved = FALSE;
    if ( slider->width() == 0 ) {
        return;
    }
    long val = long((double)slider->value() * mediaPlayerState->length() / slider->width());
    mediaPlayerState->setPosition( val );
}


void VideoWidget::setPosition( long i ) {
    updateSlider( i, mediaPlayerState->length() );
}


void VideoWidget::setLength( long max ) {
    updateSlider( mediaPlayerState->position(), max );
}


void VideoWidget::setView( char view ) {
    if ( view == 'v' ) {
        makeVisible();
    } else {
        // Effectively blank the view next time we show it so it looks nicer
        scaledWidth = 0;
        scaledHeight = 0;
        hide();
    }
}


void VideoWidget::updateSlider( long i, long max ) {
    // Will flicker too much if we don't do this
    if ( max == 0 ) {
        return;
    }
    int width = slider->width();
    int val = int((double)i * width / max);
    if ( !mediaPlayerState->fullscreen() && !videoSliderBeingMoved ) {
        if ( slider->value() != val ) {
            slider->setValue( val );
        }
        if ( slider->maxValue() != width ) {
            slider->setMaxValue( width );
        }
    }
}


void VideoWidget::setToggleButton( int i, bool down ) {
    if ( down != videoButtons[i].isDown ) {
        toggleButton( i );
    }
}


void VideoWidget::toggleButton( int i ) {
    videoButtons[i].isDown = !videoButtons[i].isDown;
    QPainter p(this);
    paintButton ( &p, i );
}


void VideoWidget::paintButton( QPainter *p, int i ) {
    int x = videoButtons[i].xPos;
    int y = videoButtons[i].yPos;
    int offset = 10 + videoButtons[i].isDown;
    p->drawPixmap( x, y, *pixmaps[videoButtons[i].isDown] );
    p->drawPixmap( x + 1 + offset, y + offset, *pixmaps[2], 9 * videoButtons[i].controlType, 0, 9, 9 );
}


void VideoWidget::mouseMoveEvent( QMouseEvent *event ) {
    for ( int i = 0; i < numButtons; i++ ) {
        int x = videoButtons[i].xPos;
        int y = videoButtons[i].yPos;
        if ( event->state() == QMouseEvent::LeftButton ) {
              // The test to see if the mouse click is inside the circular button or not
              // (compared with the radius squared to avoid a square-root of our distance)
            int radius = 16;
            QPoint center = QPoint( x + radius, y + radius );
            QPoint dXY = center - event->pos();
            int dist = dXY.x() * dXY.x() + dXY.y() * dXY.y();
            bool isOnButton = dist <= (radius * radius);
            if ( isOnButton != videoButtons[i].isHeld ) {
                videoButtons[i].isHeld = isOnButton;
                toggleButton(i);
            }
        } else {
            if ( videoButtons[i].isHeld ) {
                videoButtons[i].isHeld = FALSE;
                if ( !videoButtons[i].isToggle )
                    setToggleButton( i, FALSE );
            }
        }
        switch (i) {
          case VideoPlay:       mediaPlayerState->setPlaying(videoButtons[i].isDown); return;
          case VideoStop:       mediaPlayerState->setPlaying(FALSE); return;
          case VideoPause:      mediaPlayerState->setPaused(videoButtons[i].isDown); return;
          case VideoNext:       mediaPlayerState->setNext(); return;
          case VideoPrevious:   mediaPlayerState->setPrev(); return;
          case VideoPlayList:   mediaPlayerState->setList(); return;
          case VideoFullscreen: mediaPlayerState->setFullscreen( TRUE ); makeVisible(); return;
        }

    }
}


void VideoWidget::mousePressEvent( QMouseEvent *event ) {
    mouseMoveEvent( event );
}


void VideoWidget::mouseReleaseEvent( QMouseEvent *event ) {
    if ( mediaPlayerState->fullscreen() ) {
        mediaPlayerState->setFullscreen( FALSE );
        makeVisible();

        mouseMoveEvent( event );
    }
}


void VideoWidget::makeVisible() {
    if ( mediaPlayerState->fullscreen() ) {
        setBackgroundMode( QWidget::NoBackground );
        showFullScreen();
        resize( qApp->desktop()->size() );
        slider->hide();
    } else {
        setBackgroundPixmap( Resource::loadPixmap( "opieplayer/metalFinish" ) );
        showNormal();
        showMaximized();
        slider->show();
    }
}


void VideoWidget::paintEvent( QPaintEvent * ) {
    QPainter p( this );

    if ( mediaPlayerState->fullscreen() ) {
        // Clear the background
//        p.setBrush( QBrush( Qt::black ) );
        //p.drawRect( rect() );

    } else {
        // draw border
        qDrawShadePanel( &p, 4, 15, 230, 170, colorGroup(), TRUE, 5, NULL );

        // Clear the movie screen first
//        p.setBrush( QBrush( Qt::black ) );
//        p.drawRect( 9, 20, 220, 160 );

        // draw current frame (centrally positioned from scaling to maintain aspect ratio)
        //p.drawImage( 9 + (220 - scaledWidth) / 2, 20 + (160 - scaledHeight) / 2, *currentFrame, 0, 0, scaledWidth, scaledHeight );

        // draw the buttons
        for ( int i = 0; i < numButtons; i++ ) {
            paintButton( &p, i );
        }

        // draw the slider
        slider->repaint( TRUE );
//	videoFrame->repaint( TRUE );
    }
}


void VideoWidget::closeEvent( QCloseEvent* ) {
    mediaPlayerState->setList();
}


bool VideoWidget::playVideo() {
    bool result = FALSE;

    int stream = 0;

    int sw = 240;
    int sh = 320;
    int dd = QPixmap::defaultDepth();
    int w = height();
    int h = width();

    return true;
}



void VideoWidget::keyReleaseEvent( QKeyEvent *e)
{
    switch ( e->key() ) {
////////////////////////////// Zaurus keys
      case Key_Home:
          break;
      case Key_F9: //activity
          break;
      case Key_F10: //contacts
//           hide();
          break;
      case Key_F11: //menu
          break;
      case Key_F12: //home
          break;
      case Key_F13: //mail
          break;
      case Key_Space: {
          if(mediaPlayerState->playing()) {
              mediaPlayerState->setPlaying(FALSE);
          } else {
              mediaPlayerState->setPlaying(TRUE);
          }
      }
          break;
      case Key_Down:
//            toggleButton(6);
//            emit lessClicked();
//            emit lessReleased();
//            toggleButton(6);
          break;
      case Key_Up:
//             toggleButton(5);
//             emit moreClicked();
//             emit moreReleased();
//             toggleButton(5);
           break;
      case Key_Right:
          mediaPlayerState->setNext();
          break;
      case Key_Left:
          mediaPlayerState->setPrev();
          break;
      case Key_Escape:
          break;

    };
}
XineVideoWidget* VideoWidget::vidWidget() {
    return videoFrame;
}
