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

#include <qpe/qpeapplication.h>
#include <qpe/resource.h>
#include <qpe/config.h>


#include <qwidget.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qslider.h>
#include <qdrawutil.h>
#include "videowidget.h"
#include "mediaplayerstate.h"
#include "playlistwidget.h"


#ifdef Q_WS_QWS
# define USE_DIRECT_PAINTER
# include <qdirectpainter_qws.h>
# include <qgfxraster_qws.h>
#endif


namespace
{

const int xo = 2; // movable x offset
const int yo = 0; // movable y offset

const MediaWidget::SkinButtonInfo skinInfo[] =
{
    { MediaWidget::Play, "play", MediaWidget::ToggleButton },
    { MediaWidget::Stop, "stop", MediaWidget::NormalButton },
    { MediaWidget::Next, "fwd", MediaWidget::NormalButton },
    { MediaWidget::Previous, "back", MediaWidget::NormalButton },
    { MediaWidget::VolumeUp, "up", MediaWidget::NormalButton },
    { MediaWidget::VolumeDown, "down", MediaWidget::NormalButton },
    { MediaWidget::FullScreen, "full", MediaWidget::ToggleButton }
};

const uint buttonCount = sizeof( skinInfo ) / sizeof( skinInfo[ 0 ] );

}

VideoWidget::VideoWidget( PlayListWidget &playList, MediaPlayerState &mediaPlayerState, QWidget* parent, const char* name ) 
    : MediaWidget( playList, mediaPlayerState, parent, name ), scaledWidth( 0 ), scaledHeight( 0 ), videoSliderBeingMoved( false )
{
    setCaption( tr("OpiePlayer - Video") );

    videoFrame = new XineVideoWidget ( this, "Video frame" );

    connect ( videoFrame, SIGNAL( videoResized ( const QSize & )), this, SIGNAL( videoResized ( const QSize & )));
    connect ( videoFrame,  SIGNAL( clicked () ),  this,  SLOT ( backToNormal() ) );

    slider = 0;

    loadSkin();

    setLength( mediaPlayerState.length() );
    setPosition( mediaPlayerState.position() );
    setFullscreen( mediaPlayerState.isFullscreen() );
    setPlaying( mediaPlayerState.isPlaying() );
}


VideoWidget::~VideoWidget() 
{
}

MediaWidget::GUIInfo VideoWidget::guiInfo()
{
    return GUIInfo( "V" /* infix */, ::skinInfo, ::buttonCount );
}

void VideoWidget::resizeEvent( QResizeEvent *e ) {
    int h = height();
    int w = width();
    //int Vh = 160;
    //int Vw = 220;

    slider->setFixedWidth( w - 20 );
    slider->setGeometry( QRect( 15, h - 22, w - 90, 20 ) );
    slider->setBackgroundOrigin( QWidget::ParentOrigin );
    slider->setFocusPolicy( QWidget::NoFocus );
    slider->setBackgroundPixmap( backgroundPixmap );

    upperLeftOfButtonMask.rx() = 0;// ( imgUp->width() ) / 2;
    if(w>h)
        upperLeftOfButtonMask.ry() = 0;
    else
        upperLeftOfButtonMask.ry() = 185;//(( Vh  - imgUp->height() ) / 2) - 10;

    MediaWidget::resizeEvent( e );
}

void VideoWidget::sliderPressed() {
    videoSliderBeingMoved = TRUE;
}

void VideoWidget::sliderReleased() {
    videoSliderBeingMoved = FALSE;
    if ( slider->width() == 0 ) {
        return;
    }
    long val = long((double)slider->value() * mediaPlayerState.length() / slider->width());
    mediaPlayerState.setPosition( val );
}

void VideoWidget::setPosition( long i ) {
    updateSlider( i, mediaPlayerState.length() );
}


void VideoWidget::setLength( long max ) {
    updateSlider( mediaPlayerState.position(), max );
}

void VideoWidget::setDisplayType( MediaPlayerState::DisplayType displayType )
{
    if ( displayType == MediaPlayerState::Video ) {
        makeVisible();
        return;
    }

    // Effectively blank the view next time we show it so it looks nicer
    scaledWidth = 0;
    scaledHeight = 0;
    hide();
}

void VideoWidget::loadSkin()
{
    loadDefaultSkin( guiInfo() );

    setBackgroundPixmap( backgroundPixmap );

    delete slider;
    slider = new QSlider( Qt::Horizontal, this );
    slider->setMinValue( 0 );
    slider->setMaxValue( 1 );
    slider->setBackgroundPixmap( backgroundPixmap );
    //slider->setFocusPolicy( QWidget::NoFocus );

    resizeEvent( 0 );
}

void VideoWidget::updateSlider( long i, long max ) {
    // Will flicker too much if we don't do this
    if ( max == 0 ) {
        return;
    }
    int width = slider->width();
    int val = int((double)i * width / max);
    if ( !mediaPlayerState.isFullscreen() && !videoSliderBeingMoved ) {
        if ( slider->value() != val ) {
            slider->setValue( val );
        }
        if ( slider->maxValue() != width ) {
            slider->setMaxValue( width );
        }
    }
}

void VideoWidget::mouseReleaseEvent( QMouseEvent *event ) {
    if ( mediaPlayerState.isFullscreen() ) {
        mediaPlayerState.setFullscreen( FALSE );
        makeVisible();
    }
    MediaWidget::mouseReleaseEvent( event );
}

void VideoWidget::backToNormal() {
     mediaPlayerState.setFullscreen( FALSE );
     makeVisible();
     setToggleButton( FullScreen, false );
}

void VideoWidget::makeVisible() {
    if ( mediaPlayerState.isFullscreen() ) {
        setBackgroundMode( QWidget::NoBackground );
        showFullScreen();
        resize( qApp->desktop()->size() );
        videoFrame-> setGeometry ( 0, 0, width ( ), height ( ));

        slider->hide();
        disconnect( &mediaPlayerState, SIGNAL( positionChanged(long) ),this, SLOT( setPosition(long) ) );
        disconnect( &mediaPlayerState, SIGNAL( positionUpdated(long) ),this, SLOT( setPosition(long) ) );
        disconnect( slider, SIGNAL( sliderPressed() ), this, SLOT( sliderPressed() ) );
        disconnect( slider, SIGNAL( sliderReleased() ), this, SLOT( sliderReleased() ) );

    } else {
        showNormal();
        showMaximized();
        setBackgroundPixmap( backgroundPixmap );
        QWidget *d = QApplication::desktop();
        int w = d->width();
        int h = d->height();

        if(w>h) {
            int newW=(w/2)-(246/2); //this will only work with 320x240
            videoFrame->setGeometry( QRect( newW, 4, 240, 170  ) );
        } else {
            videoFrame->setGeometry( QRect( 0, 30, 240, 170  ) );
        }

        if ( !mediaPlayerState.isSeekable()  ) {
            if( !slider->isHidden()) {
                slider->hide();
            }
            disconnect( &mediaPlayerState, SIGNAL( positionChanged(long) ),this, SLOT( setPosition(long) ) );
            disconnect( &mediaPlayerState, SIGNAL( positionUpdated(long) ),this, SLOT( setPosition(long) ) );
            disconnect( slider, SIGNAL( sliderPressed() ), this, SLOT( sliderPressed() ) );
            disconnect( slider, SIGNAL( sliderReleased() ), this, SLOT( sliderReleased() ) );
        } else {
            slider->show();
            connect( &mediaPlayerState, SIGNAL( positionChanged(long) ),this, SLOT( setPosition(long) ) );
            connect( &mediaPlayerState, SIGNAL( positionUpdated(long) ),this, SLOT( setPosition(long) ) );
            connect( slider, SIGNAL( sliderPressed() ), this, SLOT( sliderPressed() ) );
            connect( slider, SIGNAL( sliderReleased() ), this, SLOT( sliderReleased() ) );
        }
    }
}

void VideoWidget::keyReleaseEvent( QKeyEvent *e) {
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
          if(mediaPlayerState.isPlaying()) {
              mediaPlayerState.setPlaying(FALSE);
          } else {
              mediaPlayerState.setPlaying(TRUE);
          }
      }
          break;
      case Key_Down:
//            toggleButton(6);
            emit lessClicked();
            emit lessReleased();
//            toggleButton(6);
          break;
      case Key_Up:
//             toggleButton(5);
             emit moreClicked();
             emit moreReleased();
//             toggleButton(5);
           break;
      case Key_Right:
          mediaPlayerState.setNext();
          break;
      case Key_Left:
          mediaPlayerState.setPrev();
          break;
      case Key_Escape:
          break;

    };
}

XineVideoWidget* VideoWidget::vidWidget() {
    return videoFrame;
}


void VideoWidget::setFullscreen ( bool b ) {
    setToggleButton( FullScreen, b );
}


void VideoWidget::setPlaying( bool b) {
    setToggleButton( Play, b );
}

