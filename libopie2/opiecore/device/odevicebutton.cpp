/*
                             This file is part of the Opie Project
                             Copyright (C) The Opie Team <opie-devel@lists.sourceforge.net>
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

#include <qpixmap.h>
#include <qstring.h>

#include <qpe/qcopenvelope_qws.h>
#include <opie2/odevicebutton.h>


namespace Opie {
namespace Core {
namespace Internal {

class OQCopMessageData
{
  public:
    QCString m_channel;
    QCString m_message;
    QByteArray m_data;
};

}

using namespace Opie::Core;
using Opie::Core::Internal::OQCopMessageData;

OQCopMessage::OQCopMessage()
        : d ( 0 )
        , m_data( 0 )
{
    init ( QCString(), QCString(), QByteArray());
}

OQCopMessage::OQCopMessage ( const OQCopMessage &copy )
        : d ( 0 )
        , m_data( 0 )
{
    init ( copy. channel(), copy. message(), copy. data());
}

OQCopMessage::~OQCopMessage()
{
    delete d;
}

OQCopMessage &OQCopMessage::operator = ( const OQCopMessage &assign )
{
    init ( assign. channel(), assign. message(), assign. data());
    return *this;
}

OQCopMessage::OQCopMessage ( const QCString &ch, const QCString &m, const QByteArray &arg )
        : d ( 0 )
        , m_data( 0 )
{
    init ( ch, m, arg );
}

void OQCopMessage::init ( const QCString &ch, const QCString &m, const QByteArray &arg )
{
    if ( !d )
        d = new OQCopMessageData();
    d->m_channel = ch;
    d->m_message = m;
    d->m_data = arg;
}

bool OQCopMessage::send()
{
    if ( d->m_channel. isEmpty() || d->m_message. isEmpty() )
        return false;

    QCopEnvelope e ( d->m_channel, d->m_message );

    if ( d->m_data. size())
        e. writeRawBytes ( d->m_data. data(), d->m_data. size());

    return true;
}

QCString OQCopMessage::channel() const
{
    return d->m_channel;
}

QCString OQCopMessage::message() const
{
    return d->m_message;
}

QByteArray OQCopMessage::data() const
{
    return d->m_data;
}

bool OQCopMessage::isNull() const
{
    return d->m_message.isNull() || d->m_channel.isNull();
}
void OQCopMessage::setChannel ( const QCString &ch )
{
    d->m_channel = ch;
}

void OQCopMessage::setMessage ( const QCString &m )
{
    d->m_message = m;
}

void OQCopMessage::setData ( const QByteArray &data )
{
    d->m_data = data;
}

/*! \class Opie::Core::ODeviceButton
    \brief The Opie::Core::ODeviceButton class represents a physical user mappable button on a Qtopia device.

    This class represents a physical button on a Qtopia device.    A
    device may have "user programmable" buttons.
    The location and number of buttons will vary from device to
    device.    userText() and pixmap() may be used to describe this button
    to the user in help documentation.

    \ingroup qtopiaemb
    \internal
*/

ODeviceButton::ODeviceButton()
    : m_Keycode( 0 )
    , m_UserText()
    , m_Pixmap()
    , d( 0 )
{}

ODeviceButton::~ODeviceButton()
{}

/*!
Returns the button's keycode.
*/
ushort ODeviceButton::keycode() const
{
    return m_Keycode;
}


/*!
This function returns a human readable, translated description of the button.
*/
QString ODeviceButton::userText() const
{
    return m_UserText;
}

/*!
This function returns the pixmap for this button.    If there isn't one
it will return an empty (null) pixmap.
*/
QPixmap ODeviceButton::pixmap() const
{
    return m_Pixmap;
}

/*!
This function returns the factory preset (default) action for when this button
is pressed.    The return value is a legal QCop message.
*/
OQCopMessage ODeviceButton::factoryPresetPressedAction() const
{
    return m_FactoryPresetPressedAction;
}

/*!
This function returns the user assigned action for when this button is pressed.
If no action is assigned, factoryPresetAction() is returned.
*/
OQCopMessage ODeviceButton::pressedAction() const
{
    if (m_PressedAction.channel().isEmpty())
        return factoryPresetPressedAction();
    return m_PressedAction;
}

/*!
This function returns the factory preset (default) action for when this button
is pressed and held.    The return value is a legal QCop message.
*/
OQCopMessage ODeviceButton::factoryPresetHeldAction() const
{
    return m_FactoryPresetHeldAction;
}

/*!
This function returns the user assigned action for when this button is pressed
and held. If no action is assigned, factoryPresetAction() is returned.
*/
OQCopMessage ODeviceButton::heldAction() const
{
    if (m_HeldAction.channel().isEmpty())
        return factoryPresetHeldAction();
    return m_HeldAction;
}

void ODeviceButton::setKeycode(ushort keycode)
{
    m_Keycode = keycode;
}

void ODeviceButton::setUserText(const QString& text)
{
    m_UserText = text;
}

void ODeviceButton::setPixmap(const QPixmap& picture)
{
    m_Pixmap = picture;
}

void ODeviceButton::setFactoryPresetPressedAction(const OQCopMessage& action)
{
    m_FactoryPresetPressedAction = action;
}


void ODeviceButton::setPressedAction(const OQCopMessage& action)
{
    m_PressedAction = action;
}

void ODeviceButton::setFactoryPresetHeldAction(const OQCopMessage& action)
{
    m_FactoryPresetHeldAction = action;
}

void ODeviceButton::setHeldAction(const OQCopMessage& action)
{
    m_HeldAction = action;
}

/////////////////////////////////////////////////////////////////

ODeviceButtonCombo::ODeviceButtonCombo()
    : m_keycode1( 0 )
    , m_keycode2( 0 )
    , m_keycode3( 0 )
    , m_down1( false )
    , m_down2( false )
    , m_down3( false )
    , m_lockedOnly( false )
    , m_hold( false )
    , d( 0 )
{
}

ODeviceButtonCombo::~ODeviceButtonCombo()
{
}

QString ODeviceButtonCombo::keycodeDesc() const
{
    return m_keycodeDesc;
}

QString ODeviceButtonCombo::actionDesc() const
{
    return m_actionDesc;
}

OQCopMessage ODeviceButtonCombo::action() const
{
    return m_action;
}

bool ODeviceButtonCombo::keyCodeEvent( ushort keycode, bool down, bool locked )
{
    bool res = false;
    if( m_down1 && m_down2 && m_down3 )
        res = true;

    if( keycode == m_keycode1 )
        m_down1 = down;
    else if( keycode == m_keycode2 )
        m_down2 = down;
    else if( keycode == m_keycode3 )
        m_down3 = down;
    else
        return false;

    if( !locked && m_lockedOnly ) {
        return false;
    }
    
    if( m_down1 && m_down2 && m_down3 ) {
        if( !m_hold )
            m_action.send();
        return true;
    }
    else
        return res;
}

void ODeviceButtonCombo::setKeycodes( ushort keycode1, ushort keycode2, ushort keycode3 )
{
    m_keycode1 = keycode1;
    m_keycode2 = keycode2;
    m_keycode3 = keycode3;

    if( m_keycode2 == Qt::Key_unknown ) {
        m_down2 = true;
        m_keycode2 = 0;
    }
    if( m_keycode3 == Qt::Key_unknown ) {
        m_down3 = true;
        m_keycode3 = 0;
    }
}

void ODeviceButtonCombo::setAction( const OQCopMessage& qcopMessage )
{
    m_action = qcopMessage;
}

void ODeviceButtonCombo::setActionDesc( const QString& desc )
{
    m_actionDesc = desc;
}

void ODeviceButtonCombo::setKeycodeDesc( const QString& desc )
{
    m_keycodeDesc = desc;
}

void ODeviceButtonCombo::setLockedOnly( bool lockedOnly )
{
    m_lockedOnly = lockedOnly;
}

void ODeviceButtonCombo::setHold( bool hold )
{
    m_hold = hold;
}

bool ODeviceButtonCombo::hold() const
{
    return m_hold;
}

bool ODeviceButtonCombo::checkActivate()
{
    if( m_down1 && m_down2 && m_down3 ) {
        m_action.send();
        return true;
    }
    else
        return false;
}

}
}
