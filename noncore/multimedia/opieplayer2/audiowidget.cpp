
#include <qpe/qpeapplication.h>
#include <qpe/resource.h>
#include <qpe/config.h>

#include <qwidget.h>
#include <qpixmap.h>
#include <qbutton.h>
#include <qpainter.h>
#include <qframe.h>
#include <qlayout.h>

#include "audiowidget.h"
#include "mediaplayerstate.h"

extern MediaPlayerState *mediaPlayerState;

static const int xo = -2; // movable x offset
static const int yo = 22; // movable y offset


Ticker::Ticker( QWidget* parent=0 ) : QFrame( parent ) {
        setFrameStyle( WinPanel | Sunken );
        setText( "No Song" );
}

Ticker::~Ticker() {
}

void Ticker::setText( const QString& text ) {
    pos = 0; // reset it everytime the text is changed
    scrollText = text;
    pixelLen = fontMetrics().width( scrollText );
    killTimers();
    if ( pixelLen > width() ) {
        startTimer( 50 );
    }
    update();
}


void Ticker::timerEvent( QTimerEvent * ) {
    pos = ( pos + 1 > pixelLen ) ? 0 : pos + 1;
    repaint( FALSE );
}

void Ticker::drawContents( QPainter *p ) {
    QPixmap pm( width(), height() );
    pm.fill( colorGroup().base() );
    QPainter pmp( &pm );
    for ( int i = 0; i - pos < width() && (i < 1 || pixelLen > width()); i += pixelLen ) {
        pmp.drawText( i - pos, 0, INT_MAX, height(), AlignVCenter, scrollText );
    }
    p->drawPixmap( 0, 0, pm );
}




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
    setCaption( tr("OpiePlayer - Audio") );
    Config cfg("OpiePlayer");
    cfg.setGroup("AudioWidget");

    QString backgroundPix, buttonsAllPix, buttonsBigPix, controlsPix, animatedPix;
    backgroundPix=cfg.readEntry( " backgroundPix", "opieplayer/metalFinish");
    buttonsAllPix=cfg.readEntry( "buttonsAllPix","opieplayer/mediaButtonsAll");
    buttonsBigPix=cfg.readEntry( "buttonsBigPix","opieplayer/mediaButtonsBig");
    controlsPix=cfg.readEntry( "controlsPix","opieplayer/mediaControls");

    setBackgroundPixmap( Resource::loadPixmap( backgroundPix) );
    pixmaps[0] = new QPixmap( Resource::loadPixmap( buttonsAllPix ) );
    pixmaps[1] = new QPixmap( Resource::loadPixmap( buttonsBigPix ) );
    pixmaps[2] = new QPixmap( Resource::loadPixmap( controlsPix ) );

    songInfo = new Ticker( this );
    songInfo->setFocusPolicy( QWidget::NoFocus );
    songInfo->setGeometry( QRect( 7, 3, 220, 20 ) );

    slider = new QSlider( Qt::Horizontal, this );
    slider->setFixedWidth( 220 );
    slider->setFixedHeight( 20 );
    slider->setMinValue( 0 );
    slider->setMaxValue( 1 );
    slider->setBackgroundPixmap( Resource::loadPixmap( backgroundPix ) );
    slider->setFocusPolicy( QWidget::NoFocus );
    slider->setGeometry( QRect( 7, 262, 220, 20 ) );

    connect( slider,           SIGNAL( sliderPressed() ),      this, SLOT( sliderPressed() ) );
    connect( slider,           SIGNAL( sliderReleased() ),     this, SLOT( sliderReleased() ) );
    connect( mediaPlayerState, SIGNAL( lengthChanged(long) ),  this, SLOT( setLength(long) ) );
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
    mediaPlayerState->isStreaming = FALSE;
    for ( int i = 0; i < 3; i++ ) {
        delete pixmaps[i];
    }
}


static bool audioSliderBeingMoved = FALSE;


void AudioWidget::sliderPressed() {
    audioSliderBeingMoved = TRUE;
}


void AudioWidget::sliderReleased() {
    audioSliderBeingMoved = FALSE;
    if ( slider->width() == 0 ) {
        return;
    }
    long val = long((double)slider->value() * mediaPlayerState->length() / slider->width());
    mediaPlayerState->setPosition( val );
}


void AudioWidget::setPosition( long i ) {
    //    qDebug("set position %d",i);
    updateSlider( i, mediaPlayerState->length() );
}


void AudioWidget::setLength( long max ) {
    updateSlider( mediaPlayerState->position(), max );
}


void AudioWidget::setView( char view ) {
    if (mediaPlayerState->isStreaming) {
        if( !slider->isHidden()) slider->hide();
        disconnect( mediaPlayerState, SIGNAL( positionChanged(long) ),this, SLOT( setPosition(long) ) );
        disconnect( mediaPlayerState, SIGNAL( positionUpdated(long) ),this, SLOT( setPosition(long) ) );
    } else {
        // this stops the slider from being moved, thus
        // does not stop stream when it reaches the end
        slider->show();
        connect( mediaPlayerState, SIGNAL( positionChanged(long) ),this, SLOT( setPosition(long) ) );
        connect( mediaPlayerState, SIGNAL( positionUpdated(long) ),this, SLOT( setPosition(long) ) );
    }

    if ( view == 'a' ) {
        startTimer( 150 );
        showMaximized();
    } else {
        killTimers();
        hide();
    }
}


void AudioWidget::updateSlider( long i, long max ) {
    if ( max == 0 ) {
        return;
    }
    // Will flicker too much if we don't do this
    // Scale to something reasonable
    int width = slider->width();
    int val = int((double)i * width / max);
    if ( !audioSliderBeingMoved ) {
        if ( slider->value() != val ) {
            slider->setValue( val );
        }

        if ( slider->maxValue() != width ) {
            slider->setMaxValue( width );
        }
    }
}


void AudioWidget::setToggleButton( int i, bool down ) {
    if ( down != audioButtons[i].isDown ) {
        toggleButton( i );
    }
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
            if ( isOnButton && !audioButtons[i].isHeld ) {
                audioButtons[i].isHeld = TRUE;
                toggleButton(i);
                qDebug("button toggled1  %d",i);
                switch (i) {
                  case AudioVolumeUp:   emit moreClicked(); return;
                  case AudioVolumeDown: emit lessClicked(); return;
                }
            } else if ( !isOnButton && audioButtons[i].isHeld ) {
                audioButtons[i].isHeld = FALSE;
                toggleButton(i);
                qDebug("button toggled2  %d",i);
            }
        } else {
            if ( audioButtons[i].isHeld ) {
                audioButtons[i].isHeld = FALSE;
                if ( !audioButtons[i].isToggle )
                    setToggleButton( i, FALSE );
                qDebug("button toggled3  %d",i);
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

void AudioWidget::keyReleaseEvent( QKeyEvent *e)
{
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
          break;
      case Key_F12: //home
          break;
      case Key_F13: //mail
          break;
      case Key_Space: {
          if(mediaPlayerState->playing()) {
//                toggleButton(1);
              mediaPlayerState->setPlaying(FALSE);
//                toggleButton(1);
          } else {
//                toggleButton(0);
              mediaPlayerState->setPlaying(TRUE);
//                toggleButton(0);
          }
      }
          break;
      case Key_Down:
            toggleButton(6);
          emit lessClicked();
          emit lessReleased();
          toggleButton(6);
          break;
      case Key_Up:
           toggleButton(5);
           emit moreClicked();
           emit moreReleased();
           toggleButton(5);
           break;
      case Key_Right:
//            toggleButton(3);
          mediaPlayerState->setNext();
//            toggleButton(3);
          break;
      case Key_Left:
//            toggleButton(4);
          mediaPlayerState->setPrev();
//            toggleButton(4);
          break;
      case Key_Escape:
          break;

    };
}
