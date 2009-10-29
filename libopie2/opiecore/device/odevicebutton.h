/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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
#ifndef DEVICE_BUTTON_H
#define DEVICE_BUTTON_H

#include <qpixmap.h>
#include <qstring.h>


namespace Opie{
namespace Core{
namespace Internal {
class OQCopMessageData;
}

class OQCopMessage
{
public:
    OQCopMessage ( );
    OQCopMessage ( const OQCopMessage &copy );
    OQCopMessage ( const QCString &m_channel, const QCString &message, const QByteArray &args = QByteArray ( ));
    ~OQCopMessage ( );

    OQCopMessage &operator = ( const OQCopMessage &assign );

    void setChannel ( const QCString &channel );
    void setMessage ( const QCString &message );
    void setData ( const QByteArray &ba );

    QCString channel ( ) const;
    QCString message ( ) const;
    QByteArray data ( ) const;

    bool isNull()const;

    bool send ( );

private:
    void init ( const QCString &m_channel, const QCString &message, const QByteArray &args );

    Internal::OQCopMessageData *d;
    class Private;
    Private* m_data;
};


/**
 * This class represents a physical button on a Qtopia device.  A device may
 * have n "user programmable" buttons, which are number 1..n.  The location
 * and number of buttons will vary from device to device.  userText() and pixmap()
 * may be used to describe this button to the user in help documentation.
 *
 * @version 1.0
 * @author Trolltech
 * @short A representation of buttons
 */

class ODeviceButton
{
  public:
    ODeviceButton();
    virtual ~ODeviceButton();

    ushort keycode ( ) const;
    QString userText ( ) const;
    QPixmap pixmap ( ) const;
    OQCopMessage factoryPresetPressedAction ( ) const;
    OQCopMessage pressedAction ( ) const;
    OQCopMessage factoryPresetHeldAction ( ) const;
    OQCopMessage heldAction ( ) const;

    void setKeycode ( ushort keycode );
    void setUserText ( const QString& text );
    void setPixmap ( const QPixmap& picture );
    void setFactoryPresetPressedAction ( const OQCopMessage& qcopMessage );
    void setPressedAction ( const OQCopMessage& qcopMessage );
    void setFactoryPresetHeldAction ( const OQCopMessage& qcopMessage );
    void setHeldAction ( const OQCopMessage& qcopMessage );

  private:
    ushort m_Keycode;
    QString m_UserText;
    QPixmap m_Pixmap;
    OQCopMessage m_FactoryPresetPressedAction;
    OQCopMessage m_PressedAction;
    OQCopMessage m_FactoryPresetHeldAction;
    OQCopMessage m_HeldAction;
    class Private;
    Private *d;
};

/**
 * This class represents a combination of physical buttons on a device.
 * This is intended to be used for rarely used hard-coded actions such as
 * launching screen calibration.
 *
 * @version 1.0
 * @author Paul Eggleton
 * @short A representation of special multi-button combinations
 */
class ODeviceButtonCombo
{
  public:
    ODeviceButtonCombo();
    virtual ~ODeviceButtonCombo();

    QString keycodeDesc() const;
    QString actionDesc() const;
    OQCopMessage action() const;
    bool hold() const;
    bool keyCodeEvent( ushort keycode, bool down, bool locked );

    void setKeycodes( ushort keycode1, ushort keycode2, ushort keycode3 );
    void setAction( const OQCopMessage& qcopMessage );
    void setActionDesc( const QString& desc );
    void setKeycodeDesc( const QString& desc );
    void setLockedOnly( bool lockedOnly );
    void setHold( bool hold );
  public:
    bool checkActivate();
    
  private:
    ushort m_keycode1;
    ushort m_keycode2;
    ushort m_keycode3;
    bool m_down1;
    bool m_down2;
    bool m_down3;
    OQCopMessage m_action;
    QString m_actionDesc;
    QString m_keycodeDesc;
    bool m_lockedOnly;
    bool m_hold;
    class Private;
    Private *d;
};


}
}

#endif
