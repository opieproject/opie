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
#include <qwidget.h>
#include <qpixmap.h>
#include <qbutton.h>
#include <qpainter.h>
#include <qframe.h>
#include <qpe/resource.h>
#include "audiowidget.h"
#include "mediaplayerstate.h"

extern MediaPlayerState *mediaPlayerState;


static const int xo = -2; // movable x offset
static const int yo = 22; // movable y offset


struct MediaButton {
    int  xPos, yPos;
    int  color;
    bool isToggle, isBig, isHeld, isDown;
};


// Layout information for the audioButtons (and if it is a toggle button or not)
MediaButton audioButtons[] = {
    { 3*30-15+xo, 3*30-13+yo, 0,  TRUE,  TRUE, FALSE, FALSE }, // play
    {    1*30+xo,    5*30+yo, 2, FALSE, FALSE, FALSE, FALSE }, // stop
    {    5*30+xo,    5*30+yo, 2,  TRUE, FALSE, FALSE, FALSE }, // pause
    {  6*30-5+xo,    3*30+yo, 1, FALSE, FALSE, FALSE, FALSE }, // next
    {  0*30+5+xo,    3*30+yo, 1, FALSE, FALSE, FALSE, FALSE }, // previous
    {    3*30+xo,  0*30+5+yo, 3, FALSE, FALSE, FALSE, FALSE }, // volume up
    {    3*30+xo,  6*30-5+yo, 3, FALSE, FALSE, FALSE, FALSE }, // volume down
    {    5*30+xo,    1*30+yo, 0,  TRUE, FALSE, FALSE, FALSE }, // repeat/loop
    {    1*30+xo,    1*30+yo, 0, FALSE, FALSE, FALSE, FALSE }  // playlist
};


static const int numButtons = (sizeof(audioButtons)/sizeof(MediaButton));


AudioWidget::AudioWidget(QWidget* parent, const char* name, WFlags f) :
    QWidget( parent, name, f )
{
    setCaption( tr("OpiePlayer") );
    setBackgroundPixmap( Resource::loadPixmap( "mpegplayer/metalFinish" ) );
    pixmaps[0] = new QPixmap( Resource::loadPixmap( "mpegplayer/mediaButtonsAll" ) );
    pixmaps[1] = new QPixmap( Resource::loadPixmap( "mpegplayer/mediaButtonsBig" ) );
    pixmaps[2] = new QPixmap( Resource::loadPixmap( "mpegplayer/mediaControls" ) );
    pixmaps[3] = new QPixmap( Resource::loadPixmap( "mpegplayer/animatedButton" ) );

    songInfo = new Ticker( this );
    songInfo->setFocusPolicy( QWidget::NoFocus );
    songInfo->setGeometry( QRect( 7, 3, 220, 20 ) );

    slider = new QSlider( Qt::Horizontal, this );
    slider->setFixedWidth( 220 );
    slider->setFixedHeight( 20 );
    slider->setMinValue( 0 );
    slider->setMaxValue( 1 );
    slider->setBackgroundPixmap( Resource::loadPixmap( "mpegplayer/metalFinish" ) );
    slider->setFocusPolicy( QWidget::NoFocus );
    slider->setGeometry( QRect( 7, 262, 220, 20 ) );

    connect( slider,           SIGNAL( sliderPressed() ),      this, SLOT( sliderPressed() ) );
    connect( slider,           SIGNAL( sliderReleased() ),     this, SLOT( sliderReleased() ) );

    connect( mediaPlayerState, SIGNAL( lengthChanged(long) ),  this, SLOT( setLength(long) ) );
    connect( mediaPlayerState, SIGNAL( positionChanged(long) ),this, SLOT( setPosition(long) ) );
    connect( mediaPlayerState, SIGNAL( positionUpdated(long) ),this, SLOT( setPosition(long) ) );
    connect( mediaPlayerState, SIGNAL( viewChanged(char) ),    this, SLOT( setView(char) ) );
    connect( mediaPlayerState, SIGNAL( loopingToggled(bool) ), this, SLOT( setLooping(bool) ) );
    connect( mediaPlayerState, SIGNAL( pausedToggled(bool) ),  this, SLOT( setPaused(bool) ) );
    connect( mediaPlayerState, SIGNAL( playingToggled(bool) ), this, SLOT( setPlaying(bool) ) );

    // Intialise state
    setLength( mediaPlayerState->length() );
    setPosition( mediaPlayerState->position() );
    setLooping( mediaPlayerState->fullscreen() );
    setPaused( mediaPlayerState->paused() );
    setPlaying( mediaPlayerState->playing() );

}


AudioWidget::~AudioWidget() {
    for ( int i = 0; i < 4; i++ )
  delete pixmaps[i];
}


static bool audioSliderBeingMoved = FALSE;


void AudioWidget::sliderPressed() {
    audioSliderBeingMoved = TRUE;
}


void AudioWidget::sliderReleased() {
    audioSliderBeingMoved = FALSE;
    if ( slider->width() == 0 )
  return;
    long val = long((double)slider->value() * mediaPlayerState->length() / slider->width());
    mediaPlayerState->setPosition( val );
}


void AudioWidget::setPosition( long i ) {
    updateSlider( i, mediaPlayerState->length() );
}


void AudioWidget::setLength( long max ) {
    updateSlider( mediaPlayerState->position(), max );
}


void AudioWidget::setView( char view ) {
    if ( view == 'a' ) {
  startTimer( 150 );
  showMaximized();
    } else {
  killTimers();
  hide();
    }
}


void AudioWidget::updateSlider( long i, long max ) {
    if ( max == 0 )
  return;
    // Will flicker too much if we don't do this
    // Scale to something reasonable 
    int width = slider->width();
    int val = int((double)i * width / max);
    if ( !audioSliderBeingMoved ) {
  if ( slider->value() != val )
      slider->setValue( val );
  if ( slider->maxValue() != width )
      slider->setMaxValue( width );
    }
}


void AudioWidget::setToggleButton( int i, bool down ) {
    if ( down != audioButtons[i].isDown )
  toggleButton( i );
}


void AudioWidget::toggleButton( int i ) {
    audioButtons[i].isDown = !audioButtons[i].isDown;
    QPainter p(this);
    paintButton ( &p, i );
}


void AudioWidget::paintButton( QPainter *p, int i ) {
    int x = audioButtons[i].xPos;
    int y = audioButtons[i].yPos;
    int offset = 22 + 14 * audioButtons[i].isBig + audioButtons[i].isDown;
    int buttonSize = 64 + audioButtons[i].isBig * (90 - 64);
    p->drawPixmap( x, y, *pixmaps[audioButtons[i].isBig], buttonSize * (audioButtons[i].isDown + 2 * audioButtons[i].color), 0, buttonSize, buttonSize );
    p->drawPixmap( x + offset, y + offset, *pixmaps[2], 18 * i, 0, 18, 18 );
}


void AudioWidget::timerEvent( QTimerEvent * ) {
    static int frame = 0;
    if ( !mediaPlayerState->paused() && audioButtons[ AudioPlay ].isDown ) {
  frame = frame >= 7 ? 0 : frame + 1;
  int x = audioButtons[AudioPlay].xPos;
  int y = audioButtons[AudioPlay].yPos;
  QPainter p( this );
  // Optimize to only draw the little bit of the changing images which is different
  p.drawPixmap( x + 14, y +  8, *pixmaps[3], 32 * frame, 0, 32, 32 );
  p.drawPixmap( x + 37, y + 37, *pixmaps[2], 18 * AudioPlay, 0, 6, 3 );
    }
}


void AudioWidget::mouseMoveEvent( QMouseEvent *event ) {
    for ( int i = 0; i < numButtons; i++ ) {
  int size = audioButtons[i].isBig;
  int x = audioButtons[i].xPos;
  int y = audioButtons[i].yPos;
  if ( event->state() == QMouseEvent::LeftButton ) {
      // The test to see if the mouse click is inside the circular button or not
      // (compared with the radius squared to avoid a square-root of our distance)
      int radius = 32 + 13 * size;
      QPoint center = QPoint( x + radius, y + radius );
      QPoint dXY = center - event->pos();
      int dist = dXY.x() * dXY.x() + dXY.y() * dXY.y();
      bool isOnButton = dist <= (radius * radius);
//      QRect r( x, y, 64 + 22*size, 64 + 22*size );
//      bool isOnButton = r.contains( event->pos() ); // Rectangular Button code
      if ( isOnButton && !audioButtons[i].isHeld ) {
    audioButtons[i].isHeld = TRUE;
    toggleButton(i);
    switch (i) {
                    case AudioVolumeUp:   emit moreClicked(); return;
                    case AudioVolumeDown: emit lessClicked(); return;
    }
      } else if ( !isOnButton && audioButtons[i].isHeld ) {
    audioButtons[i].isHeld = FALSE;
    toggleButton(i);
      }
  } else {
      if ( audioButtons[i].isHeld ) {
    audioButtons[i].isHeld = FALSE;
    if ( !audioButtons[i].isToggle )
        setToggleButton( i, FALSE );
    switch (i) {
        case AudioPlay:       mediaPlayerState->setPlaying(audioButtons[i].isDown); return;
        case AudioStop:       mediaPlayerState->setPlaying(FALSE); return;
        case AudioPause:      mediaPlayerState->setPaused(audioButtons[i].isDown); return;
        case AudioNext:       mediaPlayerState->setNext(); return;
        case AudioPrevious:   mediaPlayerState->setPrev(); return;
        case AudioLoop:       mediaPlayerState->setLooping(audioButtons[i].isDown); return;
        case AudioVolumeUp:   emit moreReleased(); return;
        case AudioVolumeDown: emit lessReleased(); return;
        case AudioPlayList:   mediaPlayerState->setList();  return;
    }
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


void AudioWidget::closeEvent( QCloseEvent* ) {
    mediaPlayerState->setList();
}


void AudioWidget::paintEvent( QPaintEvent * ) {
    QPainter p( this );
    for ( int i = 0; i < numButtons; i++ )
  paintButton( &p, i );
}


