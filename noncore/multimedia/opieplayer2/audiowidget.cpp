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
#include <opie/oticker.h>

#include <qwidget.h>
#include <qpixmap.h>
#include <qbutton.h>
#include <qpainter.h>
#include <qframe.h>
#include <qlayout.h>
#include <qdir.h>
#include <stdlib.h>
#include <stdio.h>

#include "audiowidget.h"
#include "mediaplayerstate.h"
#include "playlistwidget.h"

namespace
{

const int xo = -2; // movable x offset
const int yo = 22; // movable y offset

const MediaWidget::SkinButtonInfo skinInfo[] =
{
    { MediaWidget::Play, "play", MediaWidget::ToggleButton },
    { MediaWidget::Stop, "stop", MediaWidget::NormalButton },
    { MediaWidget::Next, "next", MediaWidget::NormalButton },
    { MediaWidget::Previous, "prev", MediaWidget::NormalButton },
    { MediaWidget::VolumeUp, "up", MediaWidget::NormalButton },
    { MediaWidget::VolumeDown, "down", MediaWidget::NormalButton },
    { MediaWidget::Loop, "loop", MediaWidget::ToggleButton },
    { MediaWidget::PlayList, "playlist", MediaWidget::NormalButton },
    { MediaWidget::Forward, "forward", MediaWidget::NormalButton },
    { MediaWidget::Back, "back", MediaWidget::NormalButton }
};

const uint buttonCount = sizeof( skinInfo ) / sizeof( skinInfo[ 0 ] );

void changeTextColor( QWidget * w) {
   QPalette p = w->palette();
   p.setBrush( QColorGroup::Background, QColor( 167, 212, 167 ) );
   p.setBrush( QColorGroup::Base, QColor( 167, 212, 167 ) );
   w->setPalette( p );
}

}

AudioWidget::AudioWidget( PlayListWidget &playList, MediaPlayerState &mediaPlayerState, QWidget* parent, const char* name) :

    MediaWidget( playList, mediaPlayerState, parent, name ), songInfo( this ), slider( Qt::Horizontal, this ),  time( this ),
    audioSliderBeingMoved( false )
{
    setCaption( tr("OpiePlayer") );

    Config cfg("OpiePlayer");
    cfg.setGroup("Options");
    skin = cfg.readEntry("Skin","default");
    //skin = "scaleTest";
    // color of background, frame, degree of transparency

    QString skinPath = "opieplayer2/skins/" + skin;
    backgroundPixmap = QPixmap( Resource::loadPixmap( QString("%1/background").arg(skinPath) ) );
    imgUp = QImage( Resource::loadImage( QString("%1/skin_up").arg(skinPath) ) );
    imgDn = QImage( Resource::loadImage( QString("%1/skin_down").arg(skinPath) ) );

    buttonMask = QImage( imgUp.width(), imgUp.height(), 8, 255 );
    buttonMask.fill( 0 );

    for ( uint i = 0; i < buttonCount; i++ ) {
        Button button;
        button.command = skinInfo[ i ].command;
        button.type = skinInfo[ i ].type;

        QString filename = QString( QPEApplication::qpeDir()  + "/pics/" + skinPath + "/skin_mask_" + skinInfo[i].fileName + ".png" );
        button.mask =QBitmap( filename );

        if ( !button.mask.isNull() ) {
            QImage imgMask = button.mask.convertToImage();
            uchar **dest = buttonMask.jumpTable();
            for ( int y = 0; y < imgUp.height(); y++ ) {
                uchar *line = dest[y];
                for ( int x = 0; x < imgUp.width(); x++ )
                    if ( !qRed( imgMask.pixel( x, y ) ) )
                        line[x] = button.command + 1;
            }
        }

        buttons.push_back( button );
    }

    setBackgroundPixmap( backgroundPixmap );

    songInfo.setFocusPolicy( QWidget::NoFocus );
//    changeTextColor( &songInfo );
//    songInfo.setBackgroundColor( QColor( 167, 212, 167 ));
//    songInfo.setFrameStyle( QFrame::NoFrame);
    songInfo.setFrameStyle( QFrame::WinPanel | QFrame::Sunken );
//    songInfo.setForegroundColor(Qt::white);

    slider.setFixedHeight( 20 );
    slider.setMinValue( 0 );
    slider.setMaxValue( 1 );
    slider.setFocusPolicy( QWidget::NoFocus );
    slider.setBackgroundPixmap( backgroundPixmap );

//     Config cofg("qpe");
//     cofg.setGroup("Appearance");
//     QColor backgroundcolor = QColor( cofg.readEntry( "Background", "#E5E1D5" ) );

    time.setFocusPolicy( QWidget::NoFocus );
    time.setAlignment( Qt::AlignCenter );
    
//    time.setFrame(FALSE);
//    changeTextColor( &time );

    resizeEvent( NULL );

    connect( &mediaPlayerState, SIGNAL( loopingToggled(bool) ), this, SLOT( setLooping(bool) ) );
    connect( &mediaPlayerState, SIGNAL( isSeekableToggled( bool ) ), this, SLOT( setSeekable( bool ) ) );

    connect( this,  SIGNAL( forwardClicked() ), this, SLOT( skipFor() ) );
    connect( this,  SIGNAL( backClicked() ),  this, SLOT( skipBack() ) );
    connect( this,  SIGNAL( forwardReleased() ), this, SLOT( stopSkip() ) );
    connect( this,  SIGNAL( backReleased() ), this, SLOT( stopSkip() ) );

    // Intialise state
    setLength( mediaPlayerState.length() );
    setPosition( mediaPlayerState.position() );
    setLooping( mediaPlayerState.isFullscreen() );
    //    setPaused( mediaPlayerState->paused() );
    setPlaying( mediaPlayerState.isPlaying() );

}

AudioWidget::~AudioWidget() {

//    mediaPlayerState->setPlaying(false);
}

namespace {

QPixmap combineImageWithBackground( QImage img, QPixmap bg, QPoint offset ) {
    QPixmap pix( img.width(), img.height() );
    QPainter p( &pix );
    p.drawTiledPixmap( pix.rect(), bg, offset );
    p.drawImage( 0, 0, img );
    return pix;
}


QPixmap maskPixToMask( QPixmap pix, QBitmap mask ) {
    QPixmap pixmap( pix );
    pixmap.setMask( mask );
    return pixmap;
}

};

void AudioWidget::resizeEvent( QResizeEvent * ) {
    int h = height();
    int w = width();

    songInfo.setGeometry( QRect( 2, 2, w - 4, 20 ) );
    slider.setFixedWidth( w - 110 );
    slider.setGeometry( QRect( 15, h - 22, w - 90, 20 ) );
    slider.setBackgroundOrigin( QWidget::ParentOrigin );
    time.setGeometry( QRect( w - 85, h - 30, 70, 20 ) );

    upperLeftOfButtonMask.rx() = ( w - imgUp.width() ) / 2;
    upperLeftOfButtonMask.ry() = (( h - imgUp.height() ) / 2) - 10;
    QPoint p = upperLeftOfButtonMask;

    QPixmap pixUp = combineImageWithBackground( imgUp, backgroundPixmap, p );
    QPixmap pixDn = combineImageWithBackground( imgDn, backgroundPixmap, p );

    for ( uint i = 0; i < buttons.size(); i++ ) {
        if ( !buttons[i].mask.isNull() ) {
            buttons[i].pixUp = maskPixToMask( pixUp, buttons[i].mask );
            buttons[i].pixDown = maskPixToMask( pixDn, buttons[i].mask );
        }
    }
}

void AudioWidget::sliderPressed() {
    audioSliderBeingMoved = TRUE;
}


void AudioWidget::sliderReleased() {
    audioSliderBeingMoved = FALSE;
    if ( slider.width() == 0 )
  return;
    long val = long((double)slider.value() * mediaPlayerState.length() / slider.width());
    mediaPlayerState.setPosition( val );
}

void AudioWidget::setPosition( long i ) {
    //    qDebug("<<<<<<<<<<<<<<<<<<<<<<<<set position %d",i);
    updateSlider( i, mediaPlayerState.length() );
}


void AudioWidget::setLength( long max ) {
    updateSlider( mediaPlayerState.position(), max );
}


void AudioWidget::setDisplayType( MediaPlayerState::DisplayType mediaType ) {
    if ( mediaType == MediaPlayerState::Audio ) {
        // startTimer( 150 );
        showMaximized();
        return;
    }

    killTimers();
    hide();
}


void AudioWidget::setSeekable( bool isSeekable ) {

    if ( !isSeekable ) {
        qDebug("<<<<<<<<<<<<<<file is STREAMING>>>>>>>>>>>>>>>>>>>");
        if( !slider.isHidden()) {
            slider.hide();
        }
        disconnect( &mediaPlayerState, SIGNAL( positionChanged(long) ),this, SLOT( setPosition(long) ) );
        disconnect( &mediaPlayerState, SIGNAL( positionUpdated(long) ),this, SLOT( setPosition(long) ) );
        disconnect( &slider, SIGNAL( sliderPressed() ), this, SLOT( sliderPressed() ) );
        disconnect( &slider, SIGNAL( sliderReleased() ), this, SLOT( sliderReleased() ) );
    } else {
        // this stops the slider from being moved, thus
        // does not stop stream when it reaches the end
        slider.show();
        qDebug( " CONNECT SET POSTION " );
        connect( &mediaPlayerState, SIGNAL( positionChanged(long) ),this, SLOT( setPosition(long) ) );
        connect( &mediaPlayerState, SIGNAL( positionUpdated(long) ),this, SLOT( setPosition(long) ) );
        connect( &slider, SIGNAL( sliderPressed() ), this, SLOT( sliderPressed() ) );
        connect( &slider, SIGNAL( sliderReleased() ), this, SLOT( sliderReleased() ) );
    }
}


static QString timeAsString( long length ) {
    int minutes = length / 60;
    int seconds = length % 60;
    return QString("%1:%2%3").arg( minutes ).arg( seconds / 10 ).arg( seconds % 10 );
}

void AudioWidget::updateSlider( long i, long max ) {

    time.setText( timeAsString( i ) + " / " + timeAsString( max ) );
//    qDebug( timeAsString( i ) + " / " + timeAsString( max ) ) ;

    if ( max == 0 ) {
        return;
    }
    // Will flicker too much if we don't do this
    // Scale to something reasonable
    int width = slider.width();
    int val = int((double)i * width / max);
    if ( !audioSliderBeingMoved ) {
        if ( slider.value() != val ) {
            slider.setValue( val );
        }

        if ( slider.maxValue() != width ) {
            slider.setMaxValue( width );
        }
    }
}

void AudioWidget::skipFor() {
    skipDirection = +1;
    startTimer( 50 );
    mediaPlayerState.setPosition( mediaPlayerState.position() + 2 );
}

void AudioWidget::skipBack() {
    skipDirection = -1;
    startTimer( 50 );
    mediaPlayerState.setPosition( mediaPlayerState.position() - 2 );
}



void AudioWidget::stopSkip() {
    killTimers();
}


void AudioWidget::timerEvent( QTimerEvent * ) {
    if ( skipDirection == +1 ) {
        mediaPlayerState.setPosition( mediaPlayerState.position() + 2 );
    }  else if ( skipDirection == -1 ) {
        mediaPlayerState.setPosition( mediaPlayerState.position() - 2 );
    }
}


void AudioWidget::mouseMoveEvent( QMouseEvent *event ) {
    for ( unsigned int i = 0; i < buttons.size(); i++ ) {

        Button &button = buttons[ i ];
        Command command = button.command;

        if ( event->state() == QMouseEvent::LeftButton ) {
            // The test to see if the mouse click is inside the button or not
            bool isOnButton = isOverButton( event->pos() - upperLeftOfButtonMask, command );

            if ( isOnButton && !button.isHeld ) {
                button.isHeld = TRUE;
                toggleButton( button );
                switch ( command ) {
                case VolumeUp:
                    emit moreClicked();
                    return;
                case VolumeDown:
                    emit lessClicked();
                    return;
                case Forward:
                    emit forwardClicked();
                    return;
                case Back:
                    emit backClicked();
                    return;
                default: break;
                }
            } else if ( !isOnButton && button.isHeld ) {
                button.isHeld = FALSE;
                toggleButton( button );
            }
        } else {
            if ( button.isHeld ) {
                button.isHeld = FALSE;
                if ( button.type != ToggleButton ) {
                    setToggleButton( button, FALSE );
                }
                qDebug("mouseEvent %d", i);
                handleCommand( command, button.isDown );
            }
        }
    }
}


void AudioWidget::mousePressEvent( QMouseEvent *event ) {
    mouseMoveEvent( event );
}


void AudioWidget::mouseReleaseEvent( QMouseEvent *event ) {
    mouseMoveEvent( event );
}


void AudioWidget::showEvent( QShowEvent* ) {
    QMouseEvent event( QEvent::MouseMove, QPoint( 0, 0 ), 0, 0 );
    mouseMoveEvent( &event );
}

void AudioWidget::keyReleaseEvent( QKeyEvent *e) {
    switch ( e->key() ) {
        ////////////////////////////// Zaurus keys
      case Key_Home:
          break;
      case Key_F9: //activity
           hide();
           //           qDebug("Audio F9");
          break;
      case Key_F10: //contacts
          break;
      case Key_F11: //menu
              mediaPlayerState.toggleBlank();
          break;
      case Key_F12: //home
          break;
      case Key_F13: //mail
             mediaPlayerState.toggleBlank();
          break;
      case Key_Space: {
          mediaPlayerState.togglePaused();
      }
          break;
      case Key_Down:
          //              toggleButton(6);
          emit lessClicked();
          emit lessReleased();
          //          toggleButton(6);
          break;
      case Key_Up:
          //             toggleButton(5);
           emit moreClicked();
           emit moreReleased();
           //             toggleButton(5);
           break;
      case Key_Right:
          //            toggleButton(3);
          mediaPlayerState.setNext();
          //            toggleButton(3);
          break;
      case Key_Left:
          //            toggleButton(4);
          mediaPlayerState.setPrev();
          //            toggleButton(4);
          break;
      case Key_Escape: {
      }
          break;

    };
}
