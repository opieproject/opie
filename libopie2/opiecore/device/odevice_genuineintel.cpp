/*
                             This file is part of the Opie Project
                             Copyright (C) 2002,2003,2004 The Opie Team <opie-devel@handhelds.org>
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

#include "odevice_genuineintel.h"

/* QT */
#include <qapplication.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qwindowsystem_qws.h>

/* OPIE */
#include <qpe/config.h>
#include <qpe/resource.h>
#include <qpe/sound.h>
#include <qpe/qcopenvelope_qws.h>
#include <opie2/okeyfilter.h>

#ifndef QT_NO_SOUND
#include <linux/soundcard.h>
#endif

using namespace Opie::Core;
using namespace Opie::Core::Internal;

struct gi_button genuineintel_buttons [] = {
    { 
    Qt::Key_F1, QT_TRANSLATE_NOOP("Button", "Calendar Button"),
    "devicebuttons/ipaq_calendar",
    "datebook", "nextView()",
    "today", "raise()" },
    { 
    Qt::Key_F2, QT_TRANSLATE_NOOP("Button", "Contacts Button"),
    "devicebuttons/ipaq_contact",
    "addressbook", "raise()",
    "addressbook", "beamBusinessCard()" },
    { 
    Qt::Key_F3, QT_TRANSLATE_NOOP("Button", "Menu Button"),
    "devicebuttons/ipaq_menu",
    "QPE/TaskBar", "toggleMenu()",
    "QPE/TaskBar", "toggleStartMenu()" },
    { 
    Qt::Key_F4, QT_TRANSLATE_NOOP("Button", "Mail Button"),
    "devicebuttons/ipaq_mail",
    "opiemail", "raise()",
    "opiemail", "newMail()" },
    { 
    Qt::Key_F5, QT_TRANSLATE_NOOP("Button", "Home Button"),
    "devicebuttons/ipaq_home",
    "QPE/Launcher", "home()",
    "buttonsettings", "raise()" },
    { 
    Qt::Key_F6, QT_TRANSLATE_NOOP("Button", "Record Button"),
    "devicebuttons/ipaq_record",
    "QPE/VMemo", "toggleRecord()",
    "sound", "raise()" },
};

void GenuineIntel::init(const QString& model)
{
    d->m_vendorstr = "Intel";
    d->m_vendor = Vendor_GenuineIntel;

    QStringList SL = QStringList::split( " ", model );

    d->m_model = Model_GenuineIntel;
    d->m_rotation = Rot0;
}

void GenuineIntel::initButtons()
{
    if ( d->m_buttons )
        return;

    if ( isQWS( ) ) {
        addPreHandler(this);
    }

    d->m_buttons = new QValueList <ODeviceButton>;

    for ( uint i = 0; i < ( sizeof( genuineintel_buttons ) / sizeof( gi_button )); i++ ) {
        gi_button *ib = genuineintel_buttons + i;
        ODeviceButton b;

        b. setKeycode ( ib->code );
        b. setUserText ( QObject::tr ( "Button", ib->utext ));
        b. setPixmap ( Resource::loadPixmap ( ib->pix ));
        b. setFactoryPresetPressedAction ( OQCopMessage ( makeChannel ( ib->fpressedservice ), ib->fpressedaction ));
        b. setFactoryPresetHeldAction ( OQCopMessage ( makeChannel ( ib->fheldservice ), ib->fheldaction ));

        d->m_buttons->append ( b );
    }
    reloadButtonMapping();
}

QValueList <OLed> GenuineIntel::ledList() const
{
    QValueList <OLed> vl;
    return vl; //none
}

QValueList <OLedState> GenuineIntel::ledStateList ( OLed ) const
{
    QValueList <OLedState> vl;
    return vl; //  none
}

OLedState GenuineIntel::ledState ( OLed ) const
{
    return Led_Off;
}

bool GenuineIntel::setLedState ( OLed , OLedState )
{
    return false;
}


bool GenuineIntel::filter ( int /*unicode*/, int , int , bool , bool )
{
    return false;
}

void GenuineIntel::playAlarmSound()
{
#ifndef QT_NO_SOUND
    static Sound snd ( "alarm" );
    if(!snd.isFinished())
	return;

    changeMixerForAlarm(0, "/dev/sound/mixer", &snd );
    snd. play();
#endif
}


bool GenuineIntel::setSoftSuspend ( bool )
{
    return false;
}


bool GenuineIntel::setDisplayBrightness ( int )
{
    return false;
}

int GenuineIntel::displayBrightnessResolution() const
{
    return 1; // perhaps to avoid division by zero
}


bool GenuineIntel::hasLightSensor() const
{
    return false;
}

int GenuineIntel::readLightSensor()
{
    return 0;
}

int GenuineIntel::lightSensorResolution() const
{
    return 1; // see above
}
