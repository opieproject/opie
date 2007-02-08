/*
                             This file is part of the Opie Project

                             Copyright (C)2002-2005 The Opie Team <opie-devel@handhelds.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
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

#include "odevice_palm.h"

/* QT */
#include <qapplication.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qwindowsystem_qws.h>
#include <qgfx_qws.h>

/* OPIE */
#include <qpe/config.h>
#include <qpe/sound.h>
#include <qpe/qcopenvelope_qws.h>

#include <opie2/okeyfilter.h>
#include <opie2/oresource.h>

/* STD */
#include <fcntl.h>
#include <math.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <unistd.h>
#ifndef QT_NO_SOUND
#include <linux/soundcard.h>
#endif

using namespace Opie::Core;
using namespace Opie::Core::Internal;

struct palm_button palm_buttons [] = {
    { Model_Palm_TX | Model_Palm_LD | Model_Palm_Z72,
      Qt::Key_F9, QT_TRANSLATE_NOOP( "Button", "Home Button" ),
        "devicebuttons/palm_home",
        "QPE/Launcher", "home()",
        "buttonsettings", "raise()" },
    { Model_Palm_TX | Model_Palm_LD | Model_Palm_Z72,
      Qt::Key_F10, QT_TRANSLATE_NOOP( "Button", "Calendar Button" ),
        "devicebuttons/palm_calendar",
        "datebook", "nextView()",
        "today", "raise()" },
    { Model_Palm_TX | Model_Palm_LD | Model_Palm_Z72,
      Qt::Key_F11, QT_TRANSLATE_NOOP( "Button", "Todo Button" ),
        "devicebuttons/palm_todo",
        "todolist", "raise()",
        "todolist", "create()" },
    { Model_Palm_TX | Model_Palm_LD | Model_Palm_Z72,
      Qt::Key_F12, QT_TRANSLATE_NOOP( "Button", "Mail Button" ),
        "devicebuttons/palm_mail",
        "opiemail", "raise()",
        "opiemail", "newmail()" },
    { Model_Palm_LD,
      Qt::Key_F7, QT_TRANSLATE_NOOP( "Button", "Voice Memo Button" ),
        "devicebuttons/palm_voice_memo",
	"QPE/TaskBar", "toggleMenu()",
        "QPE/TaskBar", "toggleStartMenu()" },
    { Model_Palm_LD,
      Qt::Key_F8, QT_TRANSLATE_NOOP( "Button", "Rotate Button" ),
        "devicebuttons/palm_rotate",
        "QPE/Rotation", "flip()",0,0},
};

void Palm::init(const QString& cpu_info)
{
    d->m_vendorstr = "Palm";
    d->m_vendor = Vendor_Palm;

    QString model = "unknown";

    int loc = cpu_info.find( ":" );
    if ( loc != -1 )
        model = cpu_info.mid( loc+2 ).simplifyWhiteSpace();
    else
        model = cpu_info;

    if ( model == "Palm LifeDrive" ) {
      d->m_modelstr = "Palm LifeDrive";
      d->m_model = Model_Palm_LD;
    }
    else if ( model == "Palm TX" ) {
      d->m_modelstr = "Palm TX";
      d->m_model = Model_Palm_TX;
    }
    else if ( model == "Palm Zire 72" ) {
      d->m_modelstr = "Palm Zire 72";
      d->m_model = Model_Palm_Z72;
    }
    else
      d->m_model = Model_Unknown;

    switch ( d->m_model )
    {
      case Model_Palm_LD:
      case Model_Palm_TX:
      case Model_Palm_Z72:
	m_backlightdev = "/sys/class/backlight/pxapwm-bl/";
	d->m_rotation  = Rot0;
	d->m_direction = CCW;
	d->m_qteDriver = "Transformed";
	break;
      default:
            m_backlightdev = "";
    }

}


void Palm::initButtons()
{

    if ( d->m_buttons )
        return ;

    if ( isQWS( ) ) {
	addPreHandler(this);
    }

    d->m_buttons = new QValueList <ODeviceButton>;

    for ( uint i = 0; i < ( sizeof( palm_buttons ) / sizeof( palm_button ) ); i++ )
    {
	palm_button *ib = palm_buttons + i;
        ODeviceButton b;

	if (( ib->model & d->m_model ) == d->m_model ) {
	    b. setKeycode ( ib->code );
	    b. setUserText ( QObject::tr ( "Button", ib->utext ));
	    b. setPixmap ( OResource::loadPixmap ( ib->pix ));
	    b. setFactoryPresetPressedAction ( OQCopMessage ( makeChannel ( ib->fpressedservice ), ib->fpressedaction ));
	    b. setFactoryPresetHeldAction ( OQCopMessage ( makeChannel ( ib->fheldservice ), ib->fheldaction ));

	    d->m_buttons->append ( b );
	}
    }
    reloadButtonMapping();
}


bool Palm::filter ( int /*unicode*/, int keycode, int modifiers, bool isPress, bool autoRepeat )
{
    int newkeycode = keycode;

    if (qt_screen->transformOrientation() != Rot0){

        switch ( keycode ) {
	    case Key_Left :
            case Key_Right:
            case Key_Up   :
            case Key_Down :
		    newkeycode = Key_Left + ( keycode - Key_Left + (int) qt_screen->transformOrientation() ) % 4;
	    default:
		break;
        }
    
        if (newkeycode!=keycode) {
            if ( newkeycode != Key_unknown ) {
                QWSServer::sendKeyEvent ( -1, newkeycode, modifiers, isPress, autoRepeat );
            }
            return true;
        }

    }
  
    return false;
}


bool Palm::suspend()
{
    // some Palms do not implement their own power management at the moment.

    bool res = false;

    if ( !isQWS( ) ) // only qwsserver is allowed to suspend
        return false;

    switch ( d->m_model ) {
        case Model_Palm_LD:
        case Model_Palm_Z72:
        {
            QCopChannel::send( "QPE/System", "aboutToSuspend()" );

            ::sync(); // flush fs caches
            res = ( ::system ( "apm --suspend" ) == 0 );

            QCopChannel::send( "QPE/System", "returnFromSuspend()" );
        }
        break;
	default:
	break;
    }

    return res;
}


int Palm::displayBrightnessResolution() const
{
    int res = 1;

    switch ( d->m_model )
    {
      case Model_Palm_LD:
      case Model_Palm_TX:
      case Model_Palm_Z72:
        int fd = ::open( m_backlightdev + "max_brightness", O_RDONLY|O_NONBLOCK );
        if ( fd )
        {
          char buf[100];
          if ( ::read( fd, &buf[0], sizeof buf ) ) ::sscanf( &buf[0], "%d", &res );
          ::close( fd );
        }
	break;

      default:
	res = 1;
    }
 
    return res;
}


bool Palm::setDisplayBrightness( int bright )
{
    bool res = false;

    if ( bright > 255 ) bright = 255;
    if ( bright < 0 ) bright = 0;

    int numberOfSteps = displayBrightnessResolution();
    int val = ( bright == 1 ) ? 1 : ( bright * numberOfSteps ) / 255;

    switch ( d->m_model )
    {

      case Model_Palm_LD:
      case Model_Palm_TX:
      case Model_Palm_Z72:
        int fd = ::open( m_backlightdev + "brightness", O_WRONLY|O_NONBLOCK );
        if ( fd )
        {
            char buf[100];
            int len = ::snprintf( &buf[0], sizeof buf, "%d", val );
            res = ( ::write( fd, &buf[0], len ) == 0 );
            ::close( fd );
        }
        break;
      
      default: res = false;
    }
    return res;
}
