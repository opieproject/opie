/*
                             This file is part of the Opie Project
             =.              (C) 2002-2005 The Opie Team <opie-devel@lists.sourceforge.net>
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
#include <qdir.h>
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

bool suspended;

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

    if ( model == "Palm Tungsten T" ) {
        d->m_modelstr  = "Palm Tungsten|T";
        d->m_model  = Model_Palm_TT;
    }
    else if ( model == "Palm Tungsten T2" ) {
        d->m_modelstr  = "Palm Tungsten|T2";
        d->m_model  = Model_Palm_TT2;
    }
    else if ( model == "Palm Tungsten T3" ) {
        d->m_modelstr  = "Palm Tungsten|T3";
        d->m_model  = Model_Palm_TT3;
    }
    else if ( model == "Palm Tungsten T5" ) {
        d->m_modelstr  = "Palm Tungsten|T5";
        d->m_model  = Model_Palm_TT5;
    }
    else if ( model == "Palm Tungsten C" ) {
        d->m_modelstr  = "Palm Tungsten|C";
        d->m_model  = Model_Palm_TC;
    }
    else if ( model == "Palm Tungsten E" ) {
        d->m_modelstr  = "Palm Tungsten|E";
        d->m_model  = Model_Palm_TE;
    }
    else if ( model == "Palm Tungsten E2" ) {
        d->m_modelstr  = "Palm Tungsten|E2";
        d->m_model  = Model_Palm_TE2;
    }
    else if ( model == "Palm LifeDrive" ) {
        d->m_modelstr = "Palm LifeDrive";
        d->m_model = Model_Palm_LD;
    }
    else if ( model == "Palm TX" ) {
        d->m_modelstr = "Palm TX";
        d->m_model = Model_Palm_TX;
    }
    else if ( model == "Palm Zire 71" ) {
        d->m_modelstr  = "Palm Zire 71";
        d->m_model  = Model_Palm_Z71;
    }
    else if ( model == "Palm Zire 72" ) {
        d->m_modelstr = "Palm Zire 72";
        d->m_model = Model_Palm_Z72;
    }
    else if ( model == "Palm Treo 600" ) {
        d->m_modelstr  = "Palm Treo 600";
        d->m_model  = Model_Palm_T600;
    }
    else if ( model == "Palm Treo 650" ) {
        d->m_modelstr  = "Palm Treo 650";
        d->m_model  = Model_Palm_T650;
    }
    else if ( model == "Palm Treo 680" ) {
        d->m_modelstr  = "Palm Treo 680";
        d->m_model  = Model_Palm_T680;
    }
    else if ( model == "Palm Treo 700w" ) {
        d->m_modelstr  = "Palm Treo 700w/700wx";
        d->m_model  = Model_Palm_T700W;
    }
    else if ( model == "Palm Treo 700p" ) {
        d->m_modelstr  = "Palm Treo 700p";
        d->m_model  = Model_Palm_T700P;
    }
    else if ( model == "Palm Treo 750" ) {
        d->m_modelstr  = "Palm Treo 750";
        d->m_model  = Model_Palm_T750;
    }
    else if ( model == "Palm Treo 755p" ) {
        d->m_modelstr	=  "Palm Treo 755";
        d->m_model	=  Model_Palm_T755P;
    }
    else if ( model == "Palm Foleo" ) {
        d->m_modelstr	=  "Palm Foleo";
        d->m_model	=  Model_Palm_FOLEO;
    }
    else
        d->m_model = Model_Unknown;

    switch ( d->m_model )
    {
        case Model_Palm_TT3:
        case Model_Palm_TT5:
        case Model_Palm_TC:
        case Model_Palm_LD:
        case Model_Palm_TX:
        case Model_Palm_TE2:
        case Model_Palm_Z72:
        case Model_Palm_T650:
        case Model_Palm_T680:
        case Model_Palm_T700W:
        case Model_Palm_T700P:
        case Model_Palm_T750:
        case Model_Palm_T755P:
            d->m_rotation  = Rot0;
            d->m_direction = CCW;
            d->m_qteDriver = "Transformed";
            break;
        default:
            // do nothing (having this here avoids a warning)
            break;
    }
}


void Palm::initButtons()
{
    if ( d->m_buttons )
        return ;

    if ( isQWS( ) ) {
        addPreHandler(this);
    }

    ODevice::initButtons();
}


bool Palm::filter ( int /*unicode*/, int keycode, int modifiers, bool isPress, bool autoRepeat )
{
    int newkeycode = keycode;

    switch ( keycode ) {
        case Key_Left :
        case Key_Right:
        case Key_Up   :
        case Key_Down :
            newkeycode = Key_Left + ( keycode - Key_Left + qt_screen->transformOrientation() ) % 4;
        default:
            break;
    }

    if ( newkeycode != keycode ) {
        if ( newkeycode != Key_unknown ) {
            QWSServer::sendKeyEvent ( -1, newkeycode, modifiers, isPress, autoRepeat );
        }
        return true;
    }

    return false;
}


bool Palm::suspend()
{
    // some Palms do not implement their own power management at the moment.

    bool res = false;

    if ( !isQWS( ) ) // only qwsserver is allowed to suspend
        return false;

    if (d->m_model == Model_Palm_LD) {
        suspended = !suspended;
        if (!suspended)
            return 0;
    }

    switch ( d->m_model ) {
        case Model_Palm_TT:
        case Model_Palm_TT2:
        case Model_Palm_TT3:
        case Model_Palm_TT5:
        case Model_Palm_TE:
        case Model_Palm_TE2:
        case Model_Palm_TC:
        case Model_Palm_LD:
        case Model_Palm_TX:
        case Model_Palm_Z71:
        case Model_Palm_Z72:
        case Model_Palm_T600:
        case Model_Palm_T650:
        case Model_Palm_T680:
        case Model_Palm_T700W:
        case Model_Palm_T700P:
        case Model_Palm_T750:
        case Model_Palm_T755P:
            {
                res = apmSuspend( 0 );
            }
            break;
        default:
            break;
    }

    return res;
}
