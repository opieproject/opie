/*
                             This file is part of the OPIE Project
                             Copyright (C) 2003 Maximilian Reiss <harlekin@handhelds.org>
                             Copyright (C) 2003 Greg Gilbert <ggilbert@treke.net>
               =.            Copyright (C) 2004 Michael Lauer <mickey@Vanille.de>
             .=l.
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <,   >  .   <=           redistribute it and/or  modify it under
:=1 )Y*s>-.--   :            the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|     MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>:      PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .     .:        details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=            this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include "rotate.h"

/* OPIE */
#include <opie/odevice.h>
#include <qpe/config.h>
#include <qpe/power.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/resource.h>
using namespace Opie;

/* QT */

#include <time.h>

RotateApplet::RotateApplet()
             :QObject( 0, "RotateApplet" ), m_flipped( false )
{

#if !defined(QT_NO_COP)
    QCopChannel *rotateChannel = new QCopChannel( "QPE/Rotation" , this );
    connect ( rotateChannel, SIGNAL( received( const QCString &, const QByteArray &) ),
              this, SLOT ( channelReceived( const QCString &, const QByteArray &) ) );
#endif

}

RotateApplet::~RotateApplet ( )
{}

/**
 * Qcop receive method.
 */
void RotateApplet::channelReceived( const QCString &msg, const QByteArray & data )
{
    qDebug( "RotateApplet::channelReceived( '%s' )", (const char*) msg );

    if ( ODevice::inst()->hasHingeSensor() )
    {
        struct timespec interval;
        struct timespec remain;
        interval.tv_sec = 0;
        interval.tv_nsec = 600000;
        ::nanosleep( &interval, &remain );
        OHingeStatus status = ODevice::inst()->readHingeSensor();
        qDebug( "RotateApplet::readHingeSensor = %d", (int) status );

        Config cfg( "apm" );
        cfg.setGroup( PowerStatusManager::readStatus().acStatus() == PowerStatus::Online ? "AC" : "Battery" );
        int action = cfg.readNumEntry( "CloseHingeAction", 0 );

        if ( status == CASE_CLOSED )
        {
            switch ( action )
            {
                case 1: /* DISPLAY OFF */ ODevice::inst()->setDisplayBrightness( 0 ); break;
                case 2: /* SUSPEND */ ODevice::inst()->suspend(); break;
                default: /* IGNORE */ break;
            }
        }
        else /* status != CASE_CLOSED */
        {
            switch ( action )
            {
                case 1: /* DISPLAY OFF */ ODevice::inst()->setDisplayBrightness( 127 ); break;
                case 2: /* SUSPEND */ /* How to wake up the device from kernel? */; break;
                default: /* IGNORE */ break;
            }
        }
        qDebug( "RotateApplet::switchAction %d performed.", cfg.readNumEntry( "CloseHingeAction", 0 ) );
    }

    QDataStream stream( data, IO_ReadOnly );
    if ( msg == "flip()" )
    {
        activated ( );
    }
    else if ( msg == "rotateDefault()")
    {
        rotateDefault();
    }
}

int RotateApplet::position() const
{
    return 3;
}

QString RotateApplet::name() const
{
    return tr( "Rotate shortcut" );
}

QString RotateApplet::text() const
{
    return tr( "Rotate" );
}

/*QString RotateApplet::tr( const char* s ) const
{
    return qApp->translate( "RotateApplet", s, 0 );
}

QString RotateApplet::tr( const char* s, const char* p ) const
{
    return qApp->translate( "RotateApplet", s, p );
}
*/

QIconSet RotateApplet::icon() const
{
    QPixmap pix;
    QImage img = Resource::loadImage( "Rotation" );
    if ( !img.isNull() )
        pix.convertFromImage( img.smoothScale( 14, 14 ) );
    return pix;
}

QPopupMenu* RotateApplet::popup(QWidget*) const
{
    return 0;
}

void RotateApplet::rotateDefault()
{
    int rot = ODevice::inst()->rotation();

    switch (rot)
    {
        case Rot0:   rot=0;   break;
        case Rot90:  rot=90;  break;
        case Rot180: rot=180; break;
        case Rot270: rot=270; break;
        default:     rot=0;   break;
    }

    Config cfg( "qpe" );
    cfg.setGroup( "Appearance" );

    bool rotateEnabled = cfg.readBoolEntry( "rotateEnabled",true );

    if ( !rotateEnabled) return;

    // hide inputs methods before rotation
    QCopEnvelope en( "QPE/TaskBar", "hideInputMethod()" );

    QCopEnvelope env( "QPE/System", "setCurrentRotation(int)" );
    env << rot;

    m_flipped = false;

}
void RotateApplet::activated()
{
    int defaultRotation = QPEApplication::defaultRotation();
    int newRotation = defaultRotation;

    Config cfg( "qpe" );
    cfg.setGroup( "Appearance" );

    int rotDirection = cfg.readNumEntry( "rotatedir" );
    ODirection rot = CW;

    if (rotDirection == -1)
    {
        rot = ODevice::inst()->direction();
    }
    else
    {
        rot = (ODirection) rotDirection;
    }

    // hide inputs methods before rotation
    QCopEnvelope en( "QPE/TaskBar", "hideInputMethod()" );

    if ( m_flipped )
    {
        // if flipped, flip back to the original state,
        // regardless of rotation direction
        newRotation = defaultRotation;
    }
    else
    {
        if ( rot == CCW )
        {
            newRotation = ( defaultRotation + 90 ) % 360;
        }
        else if ( rot == CW )
        {
            newRotation = ( defaultRotation + 270 ) % 360;
        }
        else if ( rot == Flip )
        {
            newRotation = ( defaultRotation + 180 ) % 360;
        }
    }

    QCopEnvelope env( "QPE/System", "setCurrentRotation(int)" );
    env << newRotation;

    m_flipped = !m_flipped;
}


QRESULT RotateApplet::queryInterface( const QUuid &uuid, QUnknownInterface **iface )
{
    *iface = 0;
    if ( uuid == IID_QUnknown )
        *iface = this;
    else if ( uuid == IID_MenuApplet )
        *iface = this;
    else
	return QS_FALSE;

    if ( *iface )
        (*iface)->addRef();
    return QS_OK;
}

Q_EXPORT_INTERFACE()
{
    Q_CREATE_INSTANCE( RotateApplet )
}
