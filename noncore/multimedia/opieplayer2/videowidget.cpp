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

const char * const skinV_mask_file_names[7] = {
"stop","play","back","fwd","up","down","full"
};

}

VideoWidget::VideoWidget( PlayListWidget &playList, MediaPlayerState &mediaPlayerState, QWidget* parent, const char* name ) 
    : MediaWidget( playList, mediaPlayerState, parent, name ), scaledWidth( 0 ), scaledHeight( 0 ) 
{
    setCaption( tr("OpiePlayer - Video") );

    Button defaultButton; 
    defaultButton.isToggle = defaultButton.isHeld = defaultButton.isDown = false;
    Button toggleButton;
    toggleButton.isToggle = true;
    toggleButton.isHeld = toggleButton.isDown = false;

    buttons.reserve( 7 );
    buttons.push_back( defaultButton ); // stop
    buttons.push_back( toggleButton ); // play
    buttons.push_back( defaultButton ); // previous
    buttons.push_back( defaultButton ); // next
    buttons.push_back( defaultButton ); // volUp
    buttons.push_back( defaultButton ); // volDown
    buttons.push_back( toggleButton ); //fullscreen

    videoFrame = new XineVideoWidget ( this, "Video frame" );

    connect ( videoFrame, SIGNAL( videoResized ( const QSize & )), this, SIGNAL( videoResized ( const QSize & )));
    connect ( videoFrame,  SIGNAL( clicked () ),  this,  SLOT ( backToNormal() ) );

    Config cfg("OpiePlayer");
    cfg.setGroup("Options");
    skin = cfg.readEntry("Skin","default");

    QString skinPath = "opieplayer2/skins/" + skin;
    pixBg = QPixmap( Resource::loadPixmap( QString("%1/background").arg(skinPath) ) );
    imgUp = QImage( Resource::loadImage( QString("%1/skinV_up").arg(skinPath) ) );
    imgDn = QImage( Resource::loadImage( QString("%1/skinV_down").arg(skinPath) ) );

    buttonMask = QImage( imgUp.width(), imgUp.height(), 8, 255 );
    buttonMask.fill( 0 );

    for ( int i = 0; i < 7; i++ ) {
        QString filename = QString( QPEApplication::qpeDir() + "/pics/" + skinPath + "/skinV_mask_" + skinV_mask_file_names[i] + ".png" );
        masks[i] = new QBitmap( filename );

        if ( !masks[i]->isNull() ) {
            QImage imgMask = masks[i]->convertToImage();
            uchar **dest = buttonMask.jumpTable();
            for ( int y = 0; y < imgUp.height(); y++ ) {
                uchar *line = dest[y];
                for ( int x = 0; x < imgUp.width(); x++ ) {
                    if ( !qRed( imgMask.pixel( x, y ) ) )
                        line[x] = i + 1;
                }
            }
        }
    }

    for ( int i = 0; i < 7; i++ ) {
        buttonPixUp[i] = NULL;
        buttonPixDown[i] = NULL;
    }

    setBackgroundPixmap( pixBg );

    slider = new QSlider( Qt::Horizontal, this );
    slider->setMinValue( 0 );
    slider->setMaxValue( 1 );
    slider->setBackgroundPixmap( Resource::loadPixmap( backgroundPix ) );
    //slider->setFocusPolicy( QWidget::NoFocus );

    resizeEvent( NULL );

    setLength( mediaPlayerState.length() );
    setPosition( mediaPlayerState.position() );
    setFullscreen( mediaPlayerState.isFullscreen() );
    setPlaying( mediaPlayerState.isPlaying() );
}


VideoWidget::~VideoWidget() {

    for ( int i = 0; i < 7; i++ ) {
        delete buttonPixUp[i];
        delete buttonPixDown[i];
    }

    for ( int i = 0; i < 7; i++ ) {
        delete masks[i];
    }

}

QPixmap *combineVImageWithBackground( QImage img, QPixmap bg, QPoint offset ) {
    QPixmap pix( img.width(), img.height() );
    QPainter p( &pix );
    p.drawTiledPixmap( pix.rect(), bg, offset );
    p.drawImage( 0, 0, img );
    return new QPixmap( pix );
}

QPixmap *maskVPixToMask( QPixmap pix, QBitmap mask ) {
    QPixmap *pixmap = new QPixmap( pix );
    pixmap->setMask( mask );
    return pixmap;
}

void VideoWidget::resizeEvent( QResizeEvent * ) {
    int h = height();
    int w = width();
    //int Vh = 160;
    //int Vw = 220;

    slider->setFixedWidth( w - 20 );
    slider->setGeometry( QRect( 15, h - 22, w - 90, 20 ) );
    slider->setBackgroundOrigin( QWidget::ParentOrigin );
    slider->setFocusPolicy( QWidget::NoFocus );
    slider->setBackgroundPixmap( pixBg );

    upperLeftOfButtonMask.rx() = 0;// ( imgUp->width() ) / 2;
    if(w>h)
        upperLeftOfButtonMask.ry() = 0;
    else
        upperLeftOfButtonMask.ry() = 185;//(( Vh  - imgUp->height() ) / 2) - 10;
    QPoint p = upperLeftOfButtonMask;

    QPixmap *pixUp = combineVImageWithBackground( imgUp, pixBg, p );
    QPixmap *pixDn = combineVImageWithBackground( imgDn, pixBg, p );

    for ( int i = 0; i < 7; i++ ) {
        if ( !masks[i]->isNull() ) {
            delete buttonPixUp[i];
            delete buttonPixDown[i];
            buttonPixUp[i] = maskVPixToMask( *pixUp, *masks[i] );
            buttonPixDown[i] = maskVPixToMask( *pixDn, *masks[i] );
        }
    }

    delete pixUp;
    delete pixDn;
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

void VideoWidget::setToggleButton( int i, bool down ) {
    if ( down != buttons[i].isDown ) {
        toggleButton( i );
    }
}

void VideoWidget::toggleButton( int i ) {
    buttons[i].isDown = !buttons[i].isDown;
    QPainter p(this);
    paintButton ( &p, i );
}

void VideoWidget::paintButton( QPainter *p, int i ) {

    if ( buttons[i].isDown ) {
        p->drawPixmap( upperLeftOfButtonMask, *buttonPixDown[i] );
    } else {
        p->drawPixmap( upperLeftOfButtonMask, *buttonPixUp[i] );
    }
}

void VideoWidget::mouseMoveEvent( QMouseEvent *event ) {
    for ( unsigned int i = 0; i < buttons.size(); i++ ) {
        if ( event->state() == QMouseEvent::LeftButton ) {
            // The test to see if the mouse click is inside the button or not
            int x = event->pos().x() - upperLeftOfButtonMask.x();
            int y = event->pos().y() - upperLeftOfButtonMask.y();

            bool isOnButton = ( x > 0 && y > 0 && x < buttonMask.width()
                                && y < buttonMask.height()
                                && buttonMask.pixelIndex( x, y ) == i + 1 );

            if ( isOnButton && !buttons[i].isHeld ) {
                buttons[i].isHeld = TRUE;
                toggleButton(i);

                switch (i) {
                case VideoVolUp:
                    emit moreClicked();
                    return;
                case VideoVolDown:
                    emit lessClicked();
                    return;
                }
            } else if ( !isOnButton && buttons[i].isHeld ) {
                        buttons[i].isHeld = FALSE;
                        toggleButton(i);
            }
        } else {

            if ( buttons[i].isHeld ) {
                buttons[i].isHeld = FALSE;
                if ( !buttons[i].isToggle ) {
                    setToggleButton( i, FALSE );
                }

                switch(i) {

                case VideoPlay: {
                    if( mediaPlayerState.isPaused() ) {
                        setToggleButton( i, FALSE );
                        mediaPlayerState.setPaused( FALSE );
                        return;
                    } else if( !mediaPlayerState.isPaused() ) {
                        setToggleButton( i, TRUE );
                        mediaPlayerState.setPaused( TRUE );
                        return;
                    } else {
                        return;
                    }
                }

                case VideoStop:       mediaPlayerState.setPlaying( FALSE ); return;
		case VideoNext:       if( playList.currentTab() == PlayListWidget::CurrentPlayList ) mediaPlayerState.setNext(); return;
		case VideoPrevious:   if( playList.currentTab() == PlayListWidget::CurrentPlayList ) mediaPlayerState.setPrev(); return;
                case VideoVolUp:      emit moreReleased(); return;
                case VideoVolDown:    emit lessReleased(); return;
                case VideoFullscreen: mediaPlayerState.setFullscreen( TRUE ); makeVisible(); return;
                }
            }
        }
    }
}

void VideoWidget::mousePressEvent( QMouseEvent *event ) {
    mouseMoveEvent( event );
}

void VideoWidget::mouseReleaseEvent( QMouseEvent *event ) {
    if ( mediaPlayerState.isFullscreen() ) {
        mediaPlayerState.setFullscreen( FALSE );
        makeVisible();
    }
    mouseMoveEvent( event );
}

void VideoWidget::showEvent( QShowEvent* ) {
    QMouseEvent event( QEvent::MouseMove, QPoint( 0, 0 ), 0, 0 );
    mouseMoveEvent( &event );
}


 void VideoWidget::backToNormal() {
     mediaPlayerState.setFullscreen( FALSE );
     makeVisible();
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
        setBackgroundPixmap( pixBg );
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




void VideoWidget::paintEvent( QPaintEvent * pe) {
    QPainter p( this );

    if ( mediaPlayerState.isFullscreen() ) {
        // Clear the background
        p.setBrush( QBrush( Qt::black ) );
    } else {
        if ( !pe->erased() ) {
            // Combine with background and double buffer
            QPixmap pix( pe->rect().size() );
            QPainter p( &pix );
            p.translate( -pe->rect().topLeft().x(), -pe->rect().topLeft().y() );
            p.drawTiledPixmap( pe->rect(), pixBg, pe->rect().topLeft() );
            for ( unsigned int i = 0; i < buttons.size(); i++ ) {
                paintButton( &p, i );
            }
            QPainter p2( this );
            p2.drawPixmap( pe->rect().topLeft(), pix );
        } else {
            QPainter p( this );
            for ( unsigned int i = 0; i < buttons.size(); i++ )
                paintButton( &p, i );
        }
        //slider->repaint( TRUE );
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
    setToggleButton( VideoFullscreen, b );
}


void VideoWidget::setPlaying( bool b) {
    setToggleButton( VideoPlay, b );
}

