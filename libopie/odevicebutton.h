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

class OQCopMessageData;

namespace Opie
{

class OQCopMessage {
public:
	OQCopMessage ( );
	OQCopMessage ( const OQCopMessage &copy );
	OQCopMessage ( const QCString &m_channel, const QCString &message, const QByteArray &args = QByteArray ( ));

	OQCopMessage &operator = ( const OQCopMessage &assign );

	void setChannel ( const QCString &channel );
	void setMessage ( const QCString &message );
	void setData ( const QByteArray &ba );

	QCString channel ( ) const;
	QCString message ( ) const;
	QByteArray data ( ) const;

	bool send ( );

private:
	void init ( const QCString &m_channel, const QCString &message, const QByteArray &args );

	OQCopMessageData *d;
};


/**
 * This class represents a physical button on a Qtopia device.  A device may
 * have n "user programmable" buttons, which are number 1..n.  The location
 * and number of buttons will vary from device to device.  userText() and pixmap()
 * may be used to describe this button to the user in help documentation.
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
};

}

#endif
