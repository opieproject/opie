/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2003 Maximilian Reiss <harlekin@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <,   >  .   <=          redistribute it and/or  modify it under
:=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/


#include <qpe/resource.h>
#include <qpe/qcopenvelope_qws.h>

#include <qpe/qpeapplication.h>
#include <qpe/config.h>
#include <qiconset.h>
#include <qpopupmenu.h>

#include "rotate.h"


RotateApplet::RotateApplet ( )
	: QObject ( 0, "RotateApplet" ), ref ( 0 )
{
    m_flipped = false;
}

RotateApplet::~RotateApplet ( )
{
}

int RotateApplet::position ( ) const
{
    return 2;
}

QString RotateApplet::name ( ) const
{
	return tr( "Rotate shortcut" );
}

QString RotateApplet::text ( ) const
{
	return tr( "Rotate" );
}

QString RotateApplet::tr( const char* s ) const
{
    return qApp->translate( "RotateApplet", s, 0 );
}

QString RotateApplet::tr( const char* s, const char* p ) const
{
    return qApp->translate( "RotateApplet", s, p );
}

QIconSet RotateApplet::icon ( ) const
{
	QPixmap pix;
	QImage img = Resource::loadImage ( "Rotation" );

	if ( !img. isNull ( ))
		pix. convertFromImage ( img. smoothScale ( 14, 14 ));
	return pix;
}

QPopupMenu *RotateApplet::popup ( QWidget * ) const
{
	return 0;
}

void RotateApplet::activated ( )
{
    int currentRotation = QPEApplication::defaultRotation();

    int newRotation;

    Config cfg( "qpe" );
    cfg.setGroup( "Appearance" );

    // 0 -> 90° clockwise, 1 -> 90° counterclockwise
    bool rotDirection = cfg.readBoolEntry( "rotatedir" );

    // hide inputs methods before rotation
    QCopEnvelope en( "QPE/TaskBar", "hideInputMethod()" );

    if ( m_flipped )  {
        if ( rotDirection  )  {
            newRotation = currentRotation - 90;
            if (newRotation <=0) newRotation = 270;
	//ipaqs like the 36xx have the display rotated
	// to 270 as default, and -90 is invalid => handle this here
        } else {
            newRotation = currentRotation + 90;
            if(newRotation >= 360) newRotation = 0;	//ipaqs like the 36xx have the display
            //rotated to 270 as default, so 360 does nothing => handle this here
        }
    } else {
        if ( rotDirection )  {
            newRotation = currentRotation + 90;
             if(newRotation >= 360) newRotation = 0;
        } else {
            newRotation = currentRotation - 90;
            if (newRotation <=0) newRotation = 270;
         }
    }
    QCopEnvelope env( "QPE/System", "setCurrentRotation(int)" );
    env << newRotation;
    QCopEnvelope env2( "QPE/System", "setDefaultRotation(int)" );
    env2 << newRotation;

    m_flipped = !m_flipped;
}


QRESULT RotateApplet::queryInterface ( const QUuid &uuid, QUnknownInterface **iface )
{
	*iface = 0;
	if ( uuid == IID_QUnknown )
		*iface = this;
	else if ( uuid == IID_MenuApplet )
		*iface = this;

	if ( *iface )
		(*iface)-> addRef ( );
	return QS_OK;
}

Q_EXPORT_INTERFACE( )
{
	Q_CREATE_INSTANCE( RotateApplet )
}


