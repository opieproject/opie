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


#include "volume.h"

#include <qpe/resource.h>
#include <qpe/qpeapplication.h>
#include <qpe/config.h>
#if ( defined Q_WS_QWS || defined(_WS_QWS_) ) && !defined(QT_NO_COP)
#include <qpe/qcopenvelope_qws.h>
#endif

#include <qpainter.h>
#include <qcheckbox.h>
#include <qslider.h>
#include <qlayout.h>
#include <qframe.h>
#include <qpixmap.h>
#include <qlabel.h>

#include <qpushbutton.h>
#include <qtimer.h>

#define RATE_TIMER_INTERVAL 100
// Ten times per second is fine (RATE_TIMER_INTERVAL 100).  A shorter time
// results in "hanging" buttons on the iPAQ due to quite high CPU consumption.

VolumeControl::VolumeControl( bool showMic, QWidget *parent, const char *name )
    : QFrame( parent, name, WDestructiveClose | WStyle_StaysOnTop | WType_Popup )
{
  setFrameStyle( QFrame::PopupPanel | QFrame::Raised );
  createView(showMic);
}

void VolumeControl::createView(bool showMic)
{
    Config cfg("Sound");
    cfg.setGroup("System");
//showMic = TRUE;
    QHBoxLayout *hboxLayout = new QHBoxLayout(this);
    hboxLayout->setMargin( 3 );
    hboxLayout->setSpacing( 0);

    QVBoxLayout *vboxButtons = new QVBoxLayout(this);
    upButton = new QPushButton( this );
    upButton->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding ) );
    upButton->setPixmap( Resource::loadPixmap( "up" ) );
    downButton = new QPushButton( this );
    downButton->setSizePolicy( QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Expanding ) );
    downButton->setPixmap( Resource::loadPixmap( "down" ) );
    vboxButtons->setSpacing( 2 );
    
    vboxButtons->addWidget( upButton );
    vboxButtons->addWidget( downButton );

    QVBoxLayout *vbox = new QVBoxLayout( this );
    QHBoxLayout *hbox = NULL;

    slider = new QSlider( this );
    slider->setRange( 0, 100 );
    slider->setTickmarks( QSlider::Both );
    slider->setTickInterval( 20 );
    slider->setFocusPolicy( QWidget::NoFocus );
    slider->setValue(cfg.readNumEntry("Volume"));

    QVBoxLayout *sbox = new QVBoxLayout(this);
    sbox->setMargin( 3 );
    sbox->setSpacing( 3 );
    sbox->addWidget( new QLabel("Vol", this) , 0, Qt::AlignVCenter | Qt::AlignHCenter );
    sbox->addWidget( slider, 0, Qt::AlignVCenter | Qt::AlignHCenter );

    if (showMic == TRUE)  {
        mic = new QSlider(this);
        mic->setRange( 0, 100 );
        mic->setTickmarks( QSlider::Both );
        mic->setTickInterval( 20 );
        mic->setFocusPolicy( QWidget::NoFocus );
        mic->setValue(cfg.readNumEntry("Mic"));
    
        QVBoxLayout *mbox = new QVBoxLayout(this);
        mbox->setMargin( 3 );
        mbox->setSpacing( 3 );
        mbox->addWidget( new QLabel("Mic", this) , 0, Qt::AlignVCenter | Qt::AlignHCenter );
        mbox->addWidget( mic, 0, Qt::AlignVCenter | Qt::AlignHCenter );

        hbox = new QHBoxLayout( this );
        hbox->setMargin( 3 );
        hbox->setSpacing( 3 );
        hbox->addLayout( sbox, 1);
        hbox->addLayout( mbox, 1);
    }
  
    muteBox = new QCheckBox( tr("Mute"), this );
    muteBox->setFocusPolicy( QWidget::NoFocus );
  
    vbox->setMargin( 3 );
    vbox->setSpacing( 0 );
    if (showMic == TRUE)
        vbox->addLayout( hbox, 1 );
    else
        vbox->addLayout( sbox, 1);
    vbox->addWidget( muteBox, 0, Qt::AlignVCenter | Qt::AlignHCenter );

    hboxLayout->addLayout( vboxButtons );
    hboxLayout->addLayout(vbox);
  
    setFixedHeight( 120 );
    setFixedWidth( sizeHint().width() );
    setFocusPolicy(QWidget::NoFocus);
    connect( upButton, SIGNAL( pressed() ), this, SLOT( ButtonChanged() ) );
    connect( upButton, SIGNAL( released() ), this, SLOT( ButtonChanged() ) );
    connect( downButton, SIGNAL( pressed() ), this, SLOT( ButtonChanged() ) );
    connect( downButton, SIGNAL( released() ), this, SLOT( ButtonChanged() ) );

    rateTimer = new QTimer(this);
    connect( rateTimer,  SIGNAL( timeout() ), this, SLOT( rateTimerDone() ) );
}

void VolumeControl::keyPressEvent( QKeyEvent *e) 
{
  switch(e->key())
  { 
  case Key_Up:
    slider->subtractStep();
    break;
  case Key_Down:
    slider->addStep();
    break;
  case Key_Space:
    muteBox->toggle();
    break;
  case Key_Escape:
    close();
    break;
  }
}

void VolumeControl::ButtonChanged()
{
    if ( upButton->isDown() || downButton->isDown() )
    {
        rateTimerDone(); // Call it one time manually, otherwise it wont get
                         // called at all when a button is pressed for a time
                         // shorter than RATE_TIMER_INTERVAL.
        rateTimer->start( RATE_TIMER_INTERVAL, false );
    }
    else
        rateTimer->stop();
}

void VolumeControl::rateTimerDone()
{
    if ( upButton->isDown() )
        slider->setValue( slider->value() - 2 );
    else // downButton->isDown()
        slider->setValue( slider->value() + 2 );
}

//===========================================================================

VolumeApplet::VolumeApplet( QWidget *parent, const char *name )
    : QWidget( parent, name )
{
  Config cfg("Sound");
  cfg.setGroup("System");
  
  setFixedHeight( 18 );
  setFixedWidth( 14 );
  
  volumePixmap = Resource::loadPixmap( "volume" );
  
  volumePercent = cfg.readNumEntry("Volume",50);
  micPercent = cfg.readNumEntry("Mic", 50);
  muted = FALSE; // ### read from pref
  micMuted = FALSE; // ### read from pref

  advancedTimer = new QTimer(this);
  
  connect( qApp, SIGNAL( volumeChanged(bool) ), this, SLOT( volumeChanged(bool) ) );
  connect( qApp, SIGNAL( micChanged(bool) ), this, SLOT ( micChanged(bool) ) );
  connect( advancedTimer, SIGNAL( timeout() ),this, SLOT( advVolControl()) );
  
  writeSystemVolume();
  writeSystemMic();
}

VolumeApplet::~VolumeApplet()
{
}

void VolumeApplet::keyPressEvent ( QKeyEvent * e )
{
  QString s;
  s.setNum(e->key());
  qWarning(s);
}
void VolumeApplet::mousePressEvent( QMouseEvent * )
{
  advancedTimer->start( 750, TRUE );
}

void VolumeApplet::mouseReleaseEvent( QMouseEvent * )
{
  showVolControl(FALSE);  
}

void VolumeApplet::advVolControl()
{
  showVolControl(TRUE);
}

void VolumeApplet::showVolControl(bool showMic)
{
  Config cfg("Sound");
  cfg.setGroup("System");
  volumePercent = cfg.readNumEntry("Volume",50);
  micPercent = cfg.readNumEntry("Mic", 50);
  
  // Create a small volume control window to adjust the volume with
  VolumeControl *vc = new VolumeControl(showMic);
  vc->slider->setValue( 100 - volumePercent );
  if (showMic)
  {
    vc->mic->setValue( 100 - micPercent );
    connect( vc->mic, SIGNAL( valueChanged( int ) ), this, SLOT( micMoved( int ) ) );
  }

  vc->muteBox->setChecked( muted );
  connect( vc->slider, SIGNAL( valueChanged( int ) ), this, SLOT( sliderMoved( int ) ) );
  connect( vc->muteBox, SIGNAL( toggled( bool ) ), this, SLOT( mute( bool ) ) );
  QPoint curPos = mapToGlobal( rect().topLeft() );
  vc->move( curPos.x()-(vc->sizeHint().width()-width())/2, curPos.y() - 120 );
  vc->show();

  advancedTimer->stop();
}

void VolumeApplet::volumeChanged( bool nowMuted )
{
    int previousVolume = volumePercent;

    if ( !nowMuted )
        readSystemVolume();

    // Handle case where muting it toggled
    if ( muted != nowMuted ) {
  muted = nowMuted;
  repaint( TRUE );
  return;
    }

    // Avoid over repainting
    if ( previousVolume != volumePercent )
  repaint( 2, height() - 3, width() - 4, 2, FALSE );
}

void VolumeApplet::micChanged( bool nowMuted )
{
  if (!nowMuted)
  readSystemMic();
  micMuted = nowMuted;
}

void VolumeApplet::mute( bool toggled )
{
    muted = toggled;

    // clear if removing mute
    repaint( !toggled );
    writeSystemVolume();
}


void VolumeApplet::sliderMoved( int percent )
{
    setVolume( 100 - percent );
}

void VolumeApplet::micMoved( int percent )
{
    setMic( 100 - percent );
}

void VolumeApplet::readSystemVolume()
{
    Config cfg("Sound");
    cfg.setGroup("System");
    volumePercent = cfg.readNumEntry("Volume");
}

void VolumeApplet::readSystemMic()
{
    Config cfg("Sound");
    cfg.setGroup("System");
    micPercent = cfg.readNumEntry("Mic");
}

void VolumeApplet::setVolume( int percent )
{
    // clamp volume percent to be between 0 and 100
    volumePercent = (percent < 0) ? 0 : ((percent > 100) ? 100 : percent);
    // repaint just the little volume rectangle
    repaint( 2, height() - 3, width() - 4, 2, FALSE );
    writeSystemVolume();
}

void VolumeApplet::setMic( int percent )
{
  // clamp volume percent to be between 0 and 100
  micPercent = (percent < 0) ? 0 : ((percent > 100) ? 100 : percent);
  writeSystemMic();
}

void VolumeApplet::writeSystemVolume()
{
  {
  Config cfg("Sound");
  cfg.setGroup("System");
  cfg.writeEntry("Volume",volumePercent);
  }
#if ( defined Q_WS_QWS || defined(_WS_QWS_) ) && !defined(QT_NO_COP)
  // Send notification that the volume has changed
  QCopEnvelope( "QPE/System", "volumeChange(bool)" ) << muted;
#endif
}

void VolumeApplet::writeSystemMic()
{
  {
  Config cfg("Sound");
  cfg.setGroup("System");
  cfg.writeEntry("Mic",micPercent);
  }
#if ( defined Q_WS_QWS || defined(_WS_QWS_) ) && !defined(QT_NO_COP)
  // Send notification that the volume has changed
  QCopEnvelope( "QPE/System", "micChange(bool)" ) << micMuted;
#endif
}

void VolumeApplet::paintEvent( QPaintEvent* )
{
  QPainter p(this);
  
  if (volumePixmap.isNull())
  volumePixmap = Resource::loadPixmap( "volume" );
  p.drawPixmap( 0, 1, volumePixmap );
  p.setPen( darkGray );
  p.drawRect( 1, height() - 4, width() - 2, 4 );
  
  int pixelsWide = volumePercent * (width() - 4) / 100;
  p.fillRect( 2, height() - 3, pixelsWide, 2, red );
  p.fillRect( pixelsWide + 2, height() - 3, width() - 4 - pixelsWide, 2, lightGray );
  
  if ( muted ) {
  p.setPen( red );
  p.drawLine( 1, 2, width() - 2, height() - 5 );
  p.drawLine( 1, 3, width() - 2, height() - 4 );
  p.drawLine( width() - 2, 2, 1, height() - 5 );
  p.drawLine( width() - 2, 3, 1, height() - 4 );
  }
}
