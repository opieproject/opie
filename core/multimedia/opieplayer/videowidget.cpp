/**********************************************************************
 ** Copyright (C) 2000 Trolltech AS.  All rights reserved.
 **
 ** This file is part of Qtopia Environment.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.trolltech.com/gpl/ for GPL licensing information.
 **
 ** Contact info@trolltech.com if any conditions of this licensing are
 ** not clear to you.
 **
 **********************************************************************/
#include <qpe/resource.h>
#include <qpe/mediaplayerplugininterface.h>
#include <qpe/config.h>
#include <qpe/qpeapplication.h>

#include <qdir.h>
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
//     int  xPos, yPos;
   bool isToggle, isHeld, isDown;
//     int  controlType;
};


// Layout information for the videoButtons (and if it is a toggle button or not)
MediaButton videoButtons[] = {
   { FALSE, FALSE, FALSE }, // stop
   { FALSE, FALSE, FALSE }, // play
   { FALSE, FALSE, FALSE }, // previous
   { FALSE, FALSE, FALSE }, // next
   { FALSE, FALSE, FALSE }, // volUp
   { FALSE, FALSE, FALSE }, // volDown
   { TRUE, FALSE, FALSE }  // fullscreen
};

//static const int numButtons = (sizeof(videoButtons)/sizeof(MediaButton));

const char *skinV_mask_file_names[7] = {
   "stop","play","back","fwd","up","down","full"
};

static const int numVButtons = (sizeof(videoButtons)/sizeof(MediaButton));

VideoWidget::VideoWidget(QWidget* parent, const char* name, WFlags f) :
   QWidget( parent, name, f ), scaledWidth( 0 ), scaledHeight( 0 )
{
   setCaption( tr("OpiePlayer") );
   Config cfg("OpiePlayer");

   cfg.setGroup("Options");
   skin = cfg.readEntry("Skin","default");

   QString skinPath;
   skinPath = "opieplayer2/skins/" + skin;
    if(!QDir(QString(getenv("OPIEDIR")) +"/pics/"+skinPath).exists())
      skinPath = "opieplayer2/skins/default";

   qDebug("skin path " + skinPath);

//     QString skinPath = "opieplayer2/skins/" + skin;

   pixBg = new QPixmap( Resource::loadPixmap( QString("%1/background").arg(skinPath) ) );
   imgUp = new QImage( Resource::loadImage( QString("%1/skinV_up").arg(skinPath) ) );
   imgDn = new QImage( Resource::loadImage( QString("%1/skinV_down").arg(skinPath) ) );

   imgButtonMask = new QImage( imgUp->width(), imgUp->height(), 8, 255 );
   imgButtonMask->fill( 0 );

   for ( int i = 0; i < 7; i++ )
   {
      QString filename = QString( QPEApplication::qpeDir() + "/pics/" + skinPath +
                                  "/skinV_mask_" + skinV_mask_file_names[i] + ".png" );
      qDebug("loading "+filename);
      masks[i] = new QBitmap( filename );

      if ( !masks[i]->isNull() )
      {
         QImage imgMask = masks[i]->convertToImage();
         uchar **dest = imgButtonMask->jumpTable();
         for ( int y = 0; y < imgUp->height(); y++ )
         {
            uchar *line = dest[y];
            for ( int x = 0; x < imgUp->width(); x++ )
            {
               if ( !qRed( imgMask.pixel( x, y ) ) )
                  line[x] = i + 1;
            }
         }
      }
   }
   qDebug("finished loading first pics");
   for ( int i = 0; i < 7; i++ )
   {
      buttonPixUp[i] = NULL;
      buttonPixDown[i] = NULL;
   }

   setBackgroundPixmap( *pixBg );

   currentFrame = new QImage( 220 + 2, 160, (QPixmap::defaultDepth() == 16) ? 16 : 32 );

   slider = new QSlider( Qt::Horizontal, this );
   slider->setMinValue( 0 );
   slider->setMaxValue( 1 );
   slider->setBackgroundPixmap( Resource::loadPixmap( backgroundPix ) );
   slider->setFocusPolicy( QWidget::NoFocus );
//    slider->setGeometry( QRect( 7, 250, 220, 20 ) );

   connect( slider,         SIGNAL( sliderPressed() ),      this, SLOT( sliderPressed() ) );
   connect( slider,         SIGNAL( sliderReleased() ),     this, SLOT( sliderReleased() ) );

   connect( mediaPlayerState, SIGNAL( lengthChanged(long) ),  this, SLOT( setLength(long) ) );
   connect( mediaPlayerState, SIGNAL( positionChanged(long) ),this, SLOT( setPosition(long) ) );
   connect( mediaPlayerState, SIGNAL( positionUpdated(long) ),this, SLOT( setPosition(long) ) );
   connect( mediaPlayerState, SIGNAL( viewChanged(char) ),    this, SLOT( setView(char) ) );
//   connect( mediaPlayerState, SIGNAL( pausedToggled(bool) ),  this, SLOT( setPaused(bool) ) );
   connect( mediaPlayerState, SIGNAL( playingToggled(bool) ), this, SLOT( setPlaying(bool) ) );

   // Intialise state
   setLength( mediaPlayerState->length() );
   setPosition( mediaPlayerState->position() );
   setFullscreen( mediaPlayerState->fullscreen() );
//   setPaused( mediaPlayerState->paused() );
   setPlaying( mediaPlayerState->playing() );
}


VideoWidget::~VideoWidget() {

   for ( int i = 0; i < 7; i++ )
   {
      delete buttonPixUp[i];
      delete buttonPixDown[i];
   }

   delete pixBg;
   delete imgUp;
   delete imgDn;
   delete imgButtonMask;
   for ( int i = 0; i < 7; i++ )
   {
      delete masks[i];
   }

//     for ( int i = 0; i < 3; i++ )
//   delete pixmaps[i];
//     delete currentFrame;
}


static bool videoSliderBeingMoved = FALSE;

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
   slider->setBackgroundPixmap( *pixBg );

   xoff = 0;// ( imgUp->width() ) / 2;
   if(w>h)
      yoff = 0;
   else
      yoff = 185;//(( Vh  - imgUp->height() ) / 2) - 10;
   QPoint p( xoff, yoff );

   QPixmap *pixUp = combineVImageWithBackground( *imgUp, *pixBg, p );
   QPixmap *pixDn = combineVImageWithBackground( *imgDn, *pixBg, p );

   for ( int i = 0; i < 7; i++ )
   {
      if ( !masks[i]->isNull() )
      {
         delete buttonPixUp[i];
         delete buttonPixDown[i];
         buttonPixUp[i] = maskVPixToMask( *pixUp, *masks[i] );
         buttonPixDown[i] = maskVPixToMask( *pixDn, *masks[i] );
      }
   }

   delete pixUp;
   delete pixDn;
}


void VideoWidget::sliderPressed() {
   videoSliderBeingMoved = TRUE;
}


void VideoWidget::sliderReleased() {
   videoSliderBeingMoved = FALSE;
   if ( slider->width() == 0 )
      return;
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
   if ( view == 'v' )
   {
      makeVisible();
   }
   else
   {
      // Effectively blank the view next time we show it so it looks nicer
      scaledWidth = 0;
      scaledHeight = 0;
      hide();
   }
}


void VideoWidget::updateSlider( long i, long max ) {
   // Will flicker too much if we don't do this
   if ( max == 0 )
      return;
   int width = slider->width();
   int val = int((double)i * width / max);
   if ( !mediaPlayerState->fullscreen() && !videoSliderBeingMoved )
   {
      if ( slider->value() != val )
         slider->setValue( val );
      if ( slider->maxValue() != width )
         slider->setMaxValue( width );
   }
}


void VideoWidget::setToggleButton( int i, bool down ) {
   if ( down != videoButtons[i].isDown )
      toggleButton( i );
}


void VideoWidget::toggleButton( int i ) {
   videoButtons[i].isDown = !videoButtons[i].isDown;
   QPainter p(this);
   paintButton ( &p, i );
}


void VideoWidget::paintButton( QPainter *p, int i ) {
   if ( videoButtons[i].isDown )
   {
      p->drawPixmap( xoff, yoff, *buttonPixDown[i] );
   }
   else
   {
      p->drawPixmap( xoff, yoff, *buttonPixUp[i] );
   }
//     int x = videoButtons[i].xPos;
//     int y = videoButtons[i].yPos;
//     int offset = 10 + videoButtons[i].isDown;
//     p->drawPixmap( x, y, *pixmaps[videoButtons[i].isDown] );
//     p->drawPixmap( x + 1 + offset, y + offset, *pixmaps[2], 9 * videoButtons[i].controlType, 0, 9, 9 );
}


void VideoWidget::mouseMoveEvent( QMouseEvent *event ) {
    for ( int i = 0; i < numVButtons; i++ ) {
        if ( event->state() == QMouseEvent::LeftButton ) {
            // The test to see if the mouse click is inside the button or not
            int x = event->pos().x() - xoff;
            int y = event->pos().y() - yoff;

            bool isOnButton = ( x > 0 && y > 0 && x < imgButtonMask->width()
                                && y < imgButtonMask->height()
                                && imgButtonMask->pixelIndex( x, y ) == i + 1 );

            if ( isOnButton && !videoButtons[i].isHeld ) {
                videoButtons[i].isHeld = TRUE;
                toggleButton(i);

                switch (i) {
                case VideoVolUp:
                    emit moreClicked();
                    return;
                case VideoVolDown:
                    emit lessClicked();
                    return;
                }
            } else if ( !isOnButton && videoButtons[i].isHeld ) {
                        videoButtons[i].isHeld = FALSE;
                        toggleButton(i);
            }
        } else {

            if ( videoButtons[i].isHeld ) {
                videoButtons[i].isHeld = FALSE;
                if ( !videoButtons[i].isToggle ) {
                    setToggleButton( i, FALSE );
                }

                switch(i) {

                case VideoPlay: {
                   qDebug("play");
                   if(  !mediaPlayerState->playing()) {
                      mediaPlayerState->setPlaying( true);
                      setToggleButton( i-1, false );
                      setToggleButton( i, false );
                      return;
                   }
                   if( mediaPlayerState->isPaused ) {
                      qDebug("isPaused");
                      setToggleButton( i, FALSE );
                      mediaPlayerState->setPaused( FALSE );
                        return;
                    } else if( !mediaPlayerState->isPaused ) {
                       qDebug("is not paused");
                        setToggleButton( i, TRUE );
                        mediaPlayerState->setPaused( TRUE );
                        return;
                    } else {
                        return;
                    }
                }

                case VideoStop:   qDebug("stop");    mediaPlayerState->setPlaying( FALSE ); setToggleButton( i+1, true); setToggleButton( i, true ); return;
                case VideoNext:        mediaPlayerState->setNext(); return;
                case VideoPrevious:    mediaPlayerState->setPrev(); return;
                case VideoVolUp:      emit moreReleased(); return;
                case VideoVolDown:    emit lessReleased(); return;
                case VideoFullscreen: mediaPlayerState->setFullscreen( TRUE ); makeVisible(); return;
                }
            }
        }
    }

//    for ( int i = 0; i < numVButtons; i++ )
//    {
//       if ( event->state() == QMouseEvent::LeftButton )
//       {
//          // The test to see if the mouse click is inside the button or not
//          int x = event->pos().x() - xoff;
//          int y = event->pos().y() - yoff;

//          bool isOnButton = ( x > 0 && y > 0 && x < imgButtonMask->width()
//                              && y < imgButtonMask->height()
//                              && imgButtonMask->pixelIndex( x, y ) == i + 1 );

//          if ( isOnButton && !videoButtons[i].isHeld )
//          {
//             qDebug("key %d", i);

//             videoButtons[i].isHeld = TRUE;
//             toggleButton(i);
//                 switch (i) {
//                 case VideoVolUp:
//                     emit moreClicked();
//                     return;
//                 case VideoVolDown:
//                     emit lessClicked();
//                     return;
//                 }
//             } else if ( !isOnButton && videoButtons[i].isHeld ) {
//                         videoButtons[i].isHeld = FALSE;
//                         toggleButton(i);
//             }


//       } else {

//          if ( videoButtons[i].isHeld )
//          {
//             videoButtons[i].isHeld = FALSE;
//             if ( !videoButtons[i].isToggle ) {
//                setToggleButton( i, FALSE );
//             }
//             qDebug("key %d", i);
//             switch(i)  {
//             case VideoPlay:
//             {
//                if( mediaPlayerState->isPaused ) {
//                   setToggleButton( i, FALSE );
//                   mediaPlayerState->setPaused( FALSE );
//                   return;
//                }
//                else if( !mediaPlayerState->isPaused )  {
//                   setToggleButton( i, TRUE );
//                   mediaPlayerState->setPaused( TRUE );
//                   return;
//                } else {
//                   return;
//                }
//             }

//             case VideoStop:       mediaPlayerState->setPlaying(FALSE); return;
// //            case VideoPlay:       mediaPlayerState->setPlaying(videoButtons[i].isDown); return;
// //           case VideoPause:      mediaPlayerState->setPaused(videoButtons[i].isDown); return;
//             case VideoNext:       mediaPlayerState->setNext(); return;
//             case VideoPrevious:   mediaPlayerState->setPrev(); return;
//                 case VideoVolUp:      emit moreReleased(); return;
//                 case VideoVolDown:    emit lessReleased(); return;
// //            case VideoPlayList:   mediaPlayerState->setList(); return;
//             case VideoFullscreen: mediaPlayerState->setFullscreen( TRUE ); makeVisible(); return;
//             }
//          }
//       }
//    }





//     for ( int i = 0; i < numButtons; i++ ) {
//         int x = videoButtons[i].xPos;
//         int y = videoButtons[i].yPos;
//         if ( event->state() == QMouseEvent::LeftButton ) {
//               // The test to see if the mouse click is inside the circular button or not
//               // (compared with the radius squared to avoid a square-root of our distance)
//             int radius = 16;
//             QPoint center = QPoint( x + radius, y + radius );
//             QPoint dXY = center - event->pos();
//             int dist = dXY.x() * dXY.x() + dXY.y() * dXY.y();
//             bool isOnButton = dist <= (radius * radius);
//             if ( isOnButton != videoButtons[i].isHeld ) {
//                 videoButtons[i].isHeld = isOnButton;
//                 toggleButton(i);
//             }
//         } else {
//             if ( videoButtons[i].isHeld ) {
//                 videoButtons[i].isHeld = FALSE;
//                 if ( !videoButtons[i].isToggle )
//                     setToggleButton( i, FALSE );
//             }
//         }


//    }
}


void VideoWidget::mousePressEvent( QMouseEvent *event ) {
   mouseMoveEvent( event );
}


void VideoWidget::mouseReleaseEvent( QMouseEvent *event ) {
   if ( mediaPlayerState->fullscreen() )
   {
      mediaPlayerState->setFullscreen( FALSE );
      makeVisible();
   }
      mouseMoveEvent( event );
//   }
}


void VideoWidget::makeVisible() {
   if ( mediaPlayerState->fullscreen() )
   {
      setBackgroundMode( QWidget::NoBackground );
      showFullScreen();
      resize( qApp->desktop()->size() );
      slider->hide();
   }
   else
   {
      setBackgroundPixmap( *pixBg );
      showNormal();
      showMaximized();
      slider->show();
   }
}


void VideoWidget::paintEvent( QPaintEvent * pe) {
   QPainter p( this );

   if ( mediaPlayerState->fullscreen() ) {
      // Clear the background
      p.setBrush( QBrush( Qt::black ) );
      p.drawRect( rect() );
   } else {
      if ( !pe->erased() ) {
         // Combine with background and double buffer
         QPixmap pix( pe->rect().size() );
         QPainter p( &pix );
         p.translate( -pe->rect().topLeft().x(), -pe->rect().topLeft().y() );
         p.drawTiledPixmap( pe->rect(), *pixBg, pe->rect().topLeft() );
         for ( int i = 0; i < numVButtons; i++ ) {
            paintButton( &p, i );
         }
         QPainter p2( this );
         p2.drawPixmap( pe->rect().topLeft(), pix );
      } else {
         QPainter p( this );
         for ( int i = 0; i < numVButtons; i++ )
            paintButton( &p, i );
      }
      slider->repaint( TRUE );
   }
}


void VideoWidget::closeEvent( QCloseEvent* ) {
   mediaPlayerState->setList();
}


bool VideoWidget::playVideo() {
   bool result = FALSE;
//   qDebug("<<<<<<<<<<<<<<<< play video");
   int stream = 0;

   int sw = mediaPlayerState->curDecoder()->videoWidth( stream );
   int sh = mediaPlayerState->curDecoder()->videoHeight( stream );
   int dd = QPixmap::defaultDepth();
   int w = height();
   int h = width();

   ColorFormat format = (dd == 16) ? RGB565 : BGRA8888;

   if ( mediaPlayerState->fullscreen() )
   {
#ifdef USE_DIRECT_PAINTER
      QDirectPainter p(this);

      if ( ( qt_screen->transformOrientation() == 3 ) &&
           ( ( dd == 16 ) || ( dd == 32 ) ) && ( p.numRects() == 1 ) )
      {

         w = 320;
         h = 240;

         if ( mediaPlayerState->scaled() )
         {
            // maintain aspect ratio
            if ( w * sh > sw * h )
               w = sw * h / sh;
            else
               h = sh * w / sw;
         }
         else
         {
            w = sw;
            h = sh;
         }

         w--; // we can't allow libmpeg to overwrite.
         QPoint roff = qt_screen->mapToDevice( p.offset(), QSize( qt_screen->width(), qt_screen->height() ) );

         int ox = roff.x() - height() + 2 + (height() - w) / 2;
         int oy = roff.y() + (width() - h) / 2;
         int sx = 0, sy = 0;

         uchar* fp = p.frameBuffer() + p.lineStep() * oy;
         fp += dd * ox / 8;
         uchar **jt = new uchar*[h];

         for ( int i = h; i; i-- )
         {
            jt[h - i] = fp;
            fp += p.lineStep();
         }

         result = mediaPlayerState->curDecoder()->videoReadScaledFrame( jt, sx, sy, sw, sh, w, h, format, 0) == 0;

         delete [] jt;
      }
      else
      {
#endif
         QPainter p(this);

         w = 320;
         h = 240;

         if ( mediaPlayerState->scaled() )
         {
            // maintain aspect ratio
            if ( w * sh > sw * h )
               w = sw * h / sh;
            else
               h = sh * w / sw;
         }
         else
         {
            w = sw;
            h = sh;
         }

         int bytes = ( dd == 16 ) ? 2 : 4;
         QImage tempFrame( w, h, bytes << 3 );
         result = mediaPlayerState->curDecoder()->videoReadScaledFrame( tempFrame.jumpTable(),
                                                                        0, 0, sw, sh, w, h, format, 0) == 0;

         if ( result && mediaPlayerState->fullscreen() )
         {

            int rw = h, rh = w;
            QImage rotatedFrame( rw, rh, bytes << 3 );

            ushort* in  = (ushort*)tempFrame.bits();
            ushort* out = (ushort*)rotatedFrame.bits();
            int spl = rotatedFrame.bytesPerLine() / bytes;

            for (int x=0; x<h; x++)
            {
               if ( bytes == 2 )
               {
                  ushort* lout = out++ + (w - 1)*spl;
                  for (int y=0; y<w; y++) {
                     *lout=*in++;
                     lout-=spl;
                  }
               }
               else
               {
                  ulong* lout = ((ulong *)out)++ + (w - 1)*spl;
                  for (int y=0; y<w; y++)
                  {
                     *lout=*((ulong*)in)++;
                     lout-=spl;
                  }
               }
            }

            p.drawImage( (240 - rw) / 2, (320 - rh) / 2, rotatedFrame, 0, 0, rw, rh );
         }
#ifdef USE_DIRECT_PAINTER
      }
#endif
   }
   else
   {

      w = 220;
      h = 160;

      // maintain aspect ratio
      if ( w * sh > sw * h )
         w = sw * h / sh;
      else
         h = sh * w / sw;

      result = mediaPlayerState->curDecoder()->videoReadScaledFrame( currentFrame->jumpTable(), 0, 0, sw, sh, w, h, format, 0) == 0;

      QPainter p( this );
      int deskW = qApp->desktop()->width();
      // Image changed size, therefore need to blank the possibly unpainted regions first
      if ( scaledWidth != w || scaledHeight != h )
      {
         p.setBrush( QBrush( Qt::black ) );
         p.drawRect( ( deskW -scaledWidth)/2, 20, scaledWidth, 160 );
      }

      scaledWidth = w;
      scaledHeight = h;

      if ( result )
      {
         p.drawImage(  (deskW  - scaledWidth) / 2, 20 + (160 - scaledHeight) / 2, *currentFrame, 0, 0, scaledWidth, scaledHeight );
      }

   }

   return result;
}



void VideoWidget::keyReleaseEvent( QKeyEvent *e)
{
   switch ( e->key() )
   {
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
   case Key_Space:
   {
      if(mediaPlayerState->playing())
      {
         mediaPlayerState->setPlaying(FALSE);
      }
      else
      {
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
