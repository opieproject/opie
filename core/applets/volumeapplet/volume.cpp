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

#include <stdio.h>

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

#include <qtoolbutton.h>
#include <qpushbutton.h>
#include <qtimer.h>

#include <opie/odevice.h>

#include "oledbox.h"

using namespace Opie;

#define RATE_TIMER_INTERVAL 100 
// Ten times per second is fine (RATE_TIMER_INTERVAL 100).  A shorter time
// results in "hanging" buttons on the iPAQ due to quite high CPU consumption.


/* XPM */
static const char * vol_xpm[] = {
"20 20 3 1",
"   c None",
".  c #0000FF",
"+  c #000000",
"                    ",
"        .           ",
"        . . . .     ",
"    . . . . . .     ",
"    . . . . . . .   ",
"    . . ..... . .   ",
"  . ... ..... ...   ",
"  ........... ....  ",
"  ................. ",
"++++++++++++++++++++",
" .................. ",
"  . ............. . ",
"  . ..... .......   ",
"    . ... ..... .   ",
"    . ... ..... .   ",
"    . ... .....     ",
"    .  .  . . .     ",
"       .  . .       ",
"       .  . .       ",
"                    "};
/* XPM */
static const char * mic_xpm[] = {
"20 20 21 1",
"   c None",
".  c #000000",
"+  c #EEEEEE",
"@  c #B4B6B4",
"#  c #8B8D8B",
"$  c #D5D6D5",
"%  c #E6E6E6",
"&  c #9C9D9C",
"*  c #6A696A",
"=  c #E6E2E6",
"-  c #F6F2F6",
";  c #CDC6CD",
">  c #737573",
",  c #4A484A",
"'  c #DEDEDE",
")  c #F6EEF6",
"!  c #414041",
"~  c #202020",
"{  c #ACAEAC",
"]  c #838583",
"^  c #6A656A",
"                    ",
"            ....    ",
"           .+@+#.   ",
"          ..$%&%*.  ",
"         .=-.;=>=,. ",
"         .'+).&+!+. ",
"         .+;+;.~+~. ",
"         ..%{%,.... ",
"         ..&=>=~..  ",
"        .+..]^,..   ",
"       .+.......    ",
"      .%...         ",
"     .=...          ",
"    .+...           ",
"   .+...            ",
"   ....             ",
"  ....              ",
"  ..                ",
" .                  ",
".                   "};

/* XPM */
static const char * alarm_xpm[] = {
"20 20 33 1",
"   c None",
".  c #080602",
"+  c #AAA602",
"@  c #252002",
"#  c #434202",
"$  c #795602",
"%  c #C3C20D",
"&  c #DADAC2",
"*  c #826002",
"=  c #740502",
"-  c #D6D602",
";  c #322E02",
">  c #826A02",
",  c #F1F195",
"'  c #959215",
")  c #423602",
"!  c #4B0302",
"~  c #844315",
"{  c #AAAA2A",
"]  c #E2DE42",
"^  c #BA7E04",
"/  c #7F7502",
"(  c #828276",
"_  c #FEFE4E",
":  c #7D1902",
"<  c #989656",
"[  c #260B02",
"}  c #F7F7D8",
"|  c #DCDA5A",
"1  c #823102",
"2  c #B1AC6B",
"3  c #F7F710",
"4  c #838204",
"                    ",
"                    ",
"        4'4/        ",
"        /-^=        ",
"       42{4>4       ",
"      '2|+*$44      ",
"     +2&3+$1*44     ",
"    (%_}_+/$:>/4    ",
"    4%_}3+#;>:*4    ",
"    4%_}&+#[1$/4    ",
"    4%_,2')[~~>4    ",
"    4%33'4#@~1>4    ",
"    4%3344#[:>/4    ",
"   42&_3'4#@>:*44   ",
"  42|}}3'4#[;$)$44  ",
"444{]]2^~~:!!#.@##/ ",
"4444-%*:==!!=...../ ",
"       /:[..        ",
"        /@.         ",
"                    "};

VolumeControl::VolumeControl ( VolumeApplet *icon, bool /*showMic*/, QWidget *parent, const char *name )
    : QFrame ( parent, name, WStyle_StaysOnTop | WType_Popup )
{
  m_icon = icon;
  
  bool has_wav_alarm = true;
  
  switch ( ODevice::inst ( )-> model ( )) { // we need to add other devices eventually
    case Model_Zaurus_SL5000:
      has_wav_alarm = false; //poor guys probably feeling left out...
      break;
    default:
      break;
  }
  

  setFrameStyle ( QFrame::PopupPanel | QFrame::Raised );

  QGridLayout *grid = new QGridLayout ( this, 1, 1, 6, 4 );
  grid-> setSpacing ( 4 );
  grid-> setMargin ( 6 );
  
  QVBoxLayout *vbox;
  QLabel *l;
  
  vbox = new QVBoxLayout ( );
  vbox-> setSpacing ( 4 );
  grid-> addLayout ( vbox, 1, 0 );
  
  upButton = new QPushButton ( this );
  upButton-> setSizePolicy ( QSizePolicy ( QSizePolicy::Minimum, QSizePolicy::Expanding ));
  upButton-> setPixmap ( Resource::loadPixmap ( "up" ));
  upButton-> setFocusPolicy ( QWidget::NoFocus );

  vbox-> addWidget ( upButton );
  
  downButton = new QPushButton ( this );
  downButton-> setSizePolicy ( QSizePolicy ( QSizePolicy::Minimum, QSizePolicy::Expanding ));
  downButton-> setPixmap ( Resource::loadPixmap ( "down" ));
  downButton-> setFocusPolicy ( QWidget::NoFocus );

  vbox-> addWidget ( downButton );

  volSlider = new QSlider ( this );
  volSlider-> setRange ( 0, 100 );
  volSlider-> setTickmarks ( QSlider::Both );
  volSlider-> setTickInterval ( 20 );
  volSlider-> setFocusPolicy ( QWidget::NoFocus );
  
  l = new QLabel ( this );
  l-> setPixmap ( QPixmap ( vol_xpm ));
  
  grid-> addWidget ( l, 0, 1, AlignCenter );
  grid-> addWidget ( volSlider, 1, 1, AlignCenter );

  volLed = new OLedBox ( green, this );
  volLed-> setFocusPolicy ( QWidget::NoFocus );
  volLed-> setFixedSize ( 16, 16 );
  
  grid-> addWidget ( volLed, 2, 1, AlignCenter );

  micSlider = new QSlider ( this );
  micSlider-> setRange ( 0, 100 );
  micSlider-> setTickmarks ( QSlider::Both );
  micSlider-> setTickInterval ( 20 );
  micSlider-> setFocusPolicy ( QWidget::NoFocus );
  
  l = new QLabel ( this );
  l-> setPixmap ( QPixmap ( mic_xpm ));
  
  grid-> addWidget ( l, 0, 2, AlignCenter );
  grid-> addWidget ( micSlider, 1, 2, AlignCenter );

  micLed = new OLedBox ( red, this );
  micLed-> setFocusPolicy ( QWidget::NoFocus );
  micLed-> setFixedSize ( 16, 16 );
  
  grid-> addWidget ( micLed, 2, 2, AlignCenter );

  alarmSlider = new QSlider ( this );
  alarmSlider-> setRange ( 0, 100 );
  alarmSlider-> setTickmarks ( QSlider::Both );
  alarmSlider-> setTickInterval ( 20 );
  alarmSlider-> setFocusPolicy ( QWidget::NoFocus );

  QLabel *alarmLabel = new QLabel ( this );
  alarmLabel-> setPixmap ( QPixmap ( alarm_xpm ));
  
  grid-> addWidget ( alarmLabel, 0, 3, AlignCenter );
  grid-> addWidget ( alarmSlider, 1, 3, AlignCenter );

  alarmLed = new OLedBox ( yellow, this );
  alarmLed-> setFocusPolicy ( QWidget::NoFocus );
  alarmLed-> setFixedSize ( 16, 16 );
  
  grid-> addWidget ( alarmLed, 2, 3, AlignCenter );

  if ( !has_wav_alarm ) {
    alarmSlider-> hide ( );
    alarmLabel-> hide ( );
    alarmLed-> hide ( );
  }
  
  grid-> addWidget ( new QLabel ( tr( "Enable Sounds for:" ), this ), 0, 4, AlignVCenter | AlignLeft );

  vbox = new QVBoxLayout ( );
  vbox-> setSpacing ( 4 );
  grid-> addMultiCellLayout ( vbox, 1, 2, 4, 4 );
  
  tapBox = new QCheckBox ( tr( "Screen Taps" ), this );
  tapBox-> setFocusPolicy ( QWidget::NoFocus );
    
  vbox-> addWidget ( tapBox, AlignVCenter | AlignLeft );

  keyBox = new QCheckBox ( tr( "Key Clicks" ), this );
  keyBox-> setFocusPolicy ( QWidget::NoFocus );
    
  vbox-> addWidget ( keyBox, AlignVCenter | AlignLeft );

  alarmBox = new QCheckBox ( tr( "Alarm Sound" ), this );
  alarmBox-> setFocusPolicy ( QWidget::NoFocus );
    
  vbox-> addWidget ( alarmBox, AlignVCenter | AlignLeft );

  if ( has_wav_alarm ) {
    alarmBox-> hide ( );
  }

  vbox-> addStretch ( 100 );

  setFixedSize ( sizeHint ( ));
  setFocusPolicy ( QWidget::NoFocus );

  rateTimer = new QTimer( this );
  connect ( rateTimer, SIGNAL( timeout ( )), this, SLOT( rateTimerDone ( )));
  
  connect ( upButton,   SIGNAL( pressed ( )),  this, SLOT( buttonChanged ( )));
  connect ( upButton,   SIGNAL( released ( )), this, SLOT( buttonChanged ( )));
  connect ( downButton, SIGNAL( pressed ( )),  this, SLOT( buttonChanged ( )));
  connect ( downButton, SIGNAL( released ( )), this, SLOT( buttonChanged ( )));

  connect ( micSlider, SIGNAL( valueChanged ( int )), this, SLOT( micMoved( int )));
  connect ( volSlider, SIGNAL( valueChanged ( int )), this, SLOT( volMoved( int )));
  connect ( alarmSlider, SIGNAL( valueChanged ( int )), this, SLOT( alarmMoved( int )));
  
  connect ( volLed,   SIGNAL( toggled ( bool )), this, SLOT( volMuteToggled ( bool )));
  connect ( micLed,   SIGNAL( toggled ( bool )), this, SLOT( micMuteToggled ( bool )));
  connect ( alarmLed, SIGNAL( toggled ( bool )), this, SLOT( alarmSoundToggled ( bool )));

  connect ( alarmBox, SIGNAL( toggled ( bool )), this, SLOT( alarmSoundToggled ( bool )));
  connect ( keyBox,   SIGNAL( toggled ( bool )), this, SLOT( keyClickToggled ( bool )));
  connect ( tapBox,   SIGNAL( toggled ( bool )), this, SLOT( screenTapToggled ( bool )));

  // initialize variables

  readConfig ( true );
  
  // initialize the config file, in case some entries are missing
  
  writeConfigEntry ( "VolumePercent", m_vol_percent,   UPD_None );
  writeConfigEntry ( "Mute",          m_vol_muted,     UPD_None );
  writeConfigEntry ( "AlarmPercent",  m_alarm_percent, UPD_None );
  writeConfigEntry ( "TouchSound",    m_snd_touch,     UPD_None );
  writeConfigEntry ( "KeySound",      m_snd_key,       UPD_None );
  writeConfigEntry ( "AlarmSound",    m_snd_alarm,     UPD_Vol );
  
  writeConfigEntry ( "Mic",           m_mic_percent,   UPD_None );
  writeConfigEntry ( "MicMute",       m_mic_muted,     UPD_Mic );
}

bool VolumeControl::volMuted ( ) const
{
  return m_vol_muted;
}

int VolumeControl::volPercent ( ) const
{
  return m_vol_percent;
}

void VolumeControl::keyPressEvent ( QKeyEvent *e )
{
  switch ( e-> key ( )) {
    case Key_Up:
      volSlider-> subtractStep ( );
      break;
    case Key_Down:
      volSlider-> addStep ( );
      break;
    case Key_Space:
      volLed-> toggle ( );
      break;
    case Key_Escape:
      hide ( );
      break;
  }
}

void VolumeControl::buttonChanged ( )
{
  if ( upButton-> isDown ( ) || downButton->isDown ( )) {
    rateTimerDone ( ); // Call it one time manually, otherwise it wont get
    // called at all when a button is pressed for a time
    // shorter than RATE_TIMER_INTERVAL.
    rateTimer-> start ( RATE_TIMER_INTERVAL, false );
  }
  else
    rateTimer-> stop ( );
}

void VolumeControl::rateTimerDone ( )
{
  if ( upButton-> isDown ( ))
    volSlider-> setValue ( volSlider-> value ( ) - 2 );
  else // if ( downButton-> isDown ( ))
    volSlider-> setValue ( volSlider-> value ( ) + 2 );
}

void VolumeControl::show ( bool /*showMic*/ )
{
  readConfig ( );

  QPoint curPos = m_icon-> mapToGlobal ( QPoint ( 0, 0 ));
  
  int w = sizeHint ( ). width ( );
  int x = curPos.x ( ) - ( w / 2 );
  
  if (( x + w ) > QPEApplication::desktop ( )-> width ( ))
    x = QPEApplication::desktop ( )-> width ( ) - w;
    
  move ( x, curPos. y ( ) - sizeHint ( ). height ( ));
  QFrame::show ( );

}

void VolumeControl::readConfig ( bool force )
{
  Config cfg ( "qpe" );
  cfg. setGroup ( "Volume" );

  int old_vp = m_vol_percent;
  int old_mp = m_mic_percent;
  bool old_vm = m_vol_muted;
  bool old_mm = m_mic_muted;
  bool old_sk = m_snd_key;
  bool old_st = m_snd_touch;
  bool old_sa = m_snd_alarm;
  int old_ap = m_alarm_percent;

  m_vol_percent   = cfg. readNumEntry ( "VolumePercent", 50 );
  m_mic_percent   = cfg. readNumEntry ( "Mic", 50 );
  m_vol_muted     = cfg. readBoolEntry ( "Mute", 0 );
  m_mic_muted     = cfg. readBoolEntry ( "MicMute", 0 );
  m_snd_key       = cfg. readBoolEntry ( "KeySound", 0 );
  m_snd_touch     = cfg. readBoolEntry ( "TouchSound", 0 );
  m_snd_alarm     = cfg. readBoolEntry ( "AlarmSound", 1 );
  m_alarm_percent = cfg. readNumEntry ( "AlarmPercent", 65 );

  if ( force || ( m_vol_percent != old_vp )) 
    volSlider-> setValue ( 100 - m_vol_percent );
  if ( force || ( m_mic_percent != old_mp ))
    micSlider-> setValue ( 100 - m_mic_percent );
  if ( force || ( m_alarm_percent != old_ap )) 
    alarmSlider-> setValue ( 100 - m_alarm_percent );
    
  if ( force || ( m_vol_muted != old_vm ))
    volLed-> setOn ( !m_vol_muted );
  if ( force || ( m_mic_muted != old_mm ))
    micLed-> setOn ( !m_mic_muted );
  if ( force || ( m_snd_alarm != old_sa ))
    alarmLed-> setOn ( m_snd_alarm );
    
  if ( force || ( m_snd_key != old_sk ))
    keyBox-> setChecked ( m_snd_key );
  if ( force || ( m_snd_touch != old_st ))
    tapBox-> setChecked ( m_snd_touch );
  if ( force || ( m_snd_alarm != old_sa ))
    alarmBox-> setChecked ( m_snd_alarm );    
}


void VolumeControl::volumeChanged ( bool /*nowMuted*/ )
{
  int prevVol = m_vol_percent;
  bool prevMute = m_vol_muted;

  readConfig ( );

  // Handle case where muting it toggled
  if ( m_vol_muted != prevMute )
  	m_icon-> redraw ( true );
  else if ( prevVol != m_vol_percent ) // Avoid over repainting
  	m_icon-> redraw ( false );
}

void VolumeControl::micChanged ( bool nowMuted )
{
  if ( !nowMuted )
    readConfig ( );
  m_mic_muted = nowMuted;
}

void VolumeControl::screenTapToggled ( bool b )
{
  m_snd_touch = b;
  writeConfigEntry ( "TouchSound", m_snd_touch, UPD_Vol );
}

void VolumeControl::keyClickToggled ( bool b )
{
  m_snd_key = b;
  writeConfigEntry ( "KeySound", m_snd_key, UPD_Vol );
}

void VolumeControl::alarmSoundToggled ( bool b )
{
  m_snd_alarm = b;
  writeConfigEntry ( "AlarmSound", m_snd_alarm, UPD_Vol );
}

void VolumeControl::volMuteToggled ( bool b )
{
  m_vol_muted = !b;
  
  m_icon-> redraw ( true );
  
  writeConfigEntry ( "Mute", m_vol_muted, UPD_Vol );
}

void VolumeControl::micMuteToggled ( bool b )
{
  m_mic_muted = !b; 
  writeConfigEntry ( "MicMute", m_mic_muted, UPD_Mic );
}


void VolumeControl::volMoved ( int percent )
{
  m_vol_percent = 100 - percent;

  // clamp volume percent to be between 0 and 100
  m_vol_percent = ( m_vol_percent < 0 ) ? 0 : (( m_vol_percent > 100 ) ? 100 : m_vol_percent );
  // repaint just the little volume rectangle
  m_icon-> redraw ( false );
  
  writeConfigEntry ( "VolumePercent", m_vol_percent, UPD_Vol );
}

void VolumeControl::micMoved ( int percent )
{
  m_mic_percent = 100 - percent;

  // clamp volume percent to be between 0 and 100
  m_mic_percent = ( m_mic_percent < 0 ) ? 0 : (( m_mic_percent > 100 ) ? 100 : m_mic_percent );

  writeConfigEntry ( "Mic", m_mic_percent, UPD_Mic );
}

void VolumeControl::alarmMoved ( int percent )
{
  m_alarm_percent = 100 - percent;

  // clamp volume percent to be between 0 and 100
  m_alarm_percent = ( m_alarm_percent < 0 ) ? 0 : (( m_alarm_percent > 100 ) ? 100 : m_alarm_percent );

  writeConfigEntry ( "AlarmPercent", m_alarm_percent, UPD_None );
}


void VolumeControl::writeConfigEntry ( const char *entry, int val, eUpdate upd )
{
  Config cfg ( "qpe" );
  cfg. setGroup ( "Volume" );
  cfg. writeEntry ( entry, val );
//  cfg. write ( );
  
#if ( defined Q_WS_QWS || defined(_WS_QWS_) ) && !defined(QT_NO_COP)
  switch ( upd ) {
    case UPD_Vol: {
      QCopEnvelope ( "QPE/System", "volumeChange(bool)" ) << m_vol_muted;
      break;
    }
    case UPD_Mic: {
      QCopEnvelope ( "QPE/System", "micChange(bool)" ) << m_mic_muted;
      break;
    }
    case UPD_None: 
      break;
  }
#endif
}

//===========================================================================

VolumeApplet::VolumeApplet( QWidget *parent, const char *name )
    : QWidget( parent, name )
{
  setFixedHeight ( 18 );
  setFixedWidth ( 14 );

  m_pixmap = new QPixmap ( Resource::loadPixmap ( "volume" ));
  m_dialog = new VolumeControl ( this );

  connect ( qApp, SIGNAL( volumeChanged ( bool )), m_dialog, SLOT( volumeChanged( bool )));
  connect ( qApp, SIGNAL( micChanged ( bool )), m_dialog, SLOT ( micChanged( bool )));
}

VolumeApplet::~VolumeApplet()
{
  delete m_pixmap;
}


void VolumeApplet::mousePressEvent ( QMouseEvent * )
{
  if ( m_dialog-> isVisible ( ))
    m_dialog-> hide ( );
  else
    m_dialog-> show ( true );
}

void VolumeApplet::redraw ( bool all )
{
	if ( all )
		repaint ( true );
	else
		repaint ( 2, height ( ) - 3, width ( ) - 4, 2, false );
}


void VolumeApplet::paintEvent ( QPaintEvent * )
{
  QPainter p ( this );

  p. drawPixmap ( 0, 1, *m_pixmap );
  p. setPen ( darkGray );
  p. drawRect ( 1, height() - 4, width() - 2, 4 );

  int pixelsWide = m_dialog-> volPercent ( ) * ( width() - 4 ) / 100;
  p. fillRect ( 2, height() - 3, pixelsWide, 2, red );
  p. fillRect ( pixelsWide + 2, height() - 3, width() - 4 - pixelsWide, 2, lightGray );

  if ( m_dialog-> volMuted ( )) {
    p. setPen ( red );
    p. drawLine ( 1, 2, width() - 2, height() - 5 );
    p. drawLine ( 1, 3, width() - 2, height() - 4 );
    p. drawLine ( width() - 2, 2, 1, height() - 5 );
    p. drawLine ( width() - 2, 3, 1, height() - 4 );
  }
}


