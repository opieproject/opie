/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 Robert Griebl <sandman@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include <qapplication.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qframe.h>
#include <qlineedit.h>
#include <qtimer.h>
#include <qcombobox.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>
#include <qimage.h>
#if QT_VERSION < 300
#include <qgfx_qws.h>
#endif
#include <qwindowsystem_qws.h>

#include <qpe/resource.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/config.h>

#include <opie/odevice.h>

#include <stdio.h>
#include <stdlib.h>

#include "loginwindowimpl.h"
#include "loginapplication.h"
#include "inputmethods.h"

using namespace Opie;


LoginWindowImpl::LoginWindowImpl ( ) : LoginWindow ( 0, "LOGIN-WINDOW", WStyle_Customize | WStyle_NoBorder | WDestructiveClose )
{
	QPopupMenu *pop = new QPopupMenu ( this );
	pop-> insertItem ( tr( "Restart" ), this, SLOT( restart ( )));
	pop-> insertItem ( tr( "Quit" ), this, SLOT( quit ( )));
	m_menu-> setPopup ( pop );

	QCopChannel *channel = new QCopChannel ( "QPE/TaskBar", this );
	connect ( channel, SIGNAL( received ( const QCString &, const QByteArray & )), this, SLOT( receive ( const QCString &, const QByteArray & )));	         

	QHBoxLayout *lay = new QHBoxLayout ( m_taskbar, 4, 4 );
	m_input = new InputMethods ( m_taskbar );
 	connect ( m_input, SIGNAL( inputToggled ( bool )), this, SLOT( calcMaxWindowRect ( )));
	lay-> addWidget ( m_input );
	lay-> addStretch ( 10 );

	setActiveWindow ( );
	m_password-> setFocus ( );

	m_user-> insertStringList ( lApp-> allUsers ( ));

	QTimer::singleShot ( 0, this, SLOT( showIM ( )));

	QString opiedir = ::getenv ( "OPIEDIR" );
	QPixmap bgpix ( opiedir + "/pics/launcher/opie-background.jpg" );

	if ( !bgpix. isNull ( ))
		setBackgroundPixmap ( bgpix );

	m_caption-> setText ( m_caption-> text ( ) + tr( "<center>%1 %2</center>" ). arg ( ODevice::inst ( )-> systemString ( )). arg ( ODevice::inst ( )-> systemVersionString ( )));

	Config cfg ( "opie-login" );
	cfg. setGroup ( "General" );
	QString last = cfg. readEntry ( "LastLogin" );

	if ( !last. isEmpty ( ))
		m_user-> setEditText ( last );
	
	calcMaxWindowRect ( );
}

LoginWindowImpl::~LoginWindowImpl ( )
{
}


void LoginWindowImpl::receive ( const QCString &msg, const QByteArray &data )
{
	QDataStream stream ( data, IO_ReadOnly );
	
	if ( msg == "hideInputMethod()" ) 
		m_input-> hideInputMethod ( );
	else if ( msg == "showInputMethod()" )
		m_input-> showInputMethod ( );
	else if ( msg == "reloadInputMethods()" )
		m_input-> loadInputMethods ( );
}

void LoginWindowImpl::calcMaxWindowRect ( )
{
#ifdef Q_WS_QWS
	QRect wr;
	int displayWidth = qApp-> desktop ( )-> width ( );
	QRect ir = m_input-> inputRect ( );
	if ( ir.isValid() )
		wr.setCoords( 0, 0, displayWidth-1, ir.top()-1 );
	else
		wr.setCoords( 0, 0, displayWidth-1, m_taskbar->y()-1 );
	
#if QT_VERSION < 300
	wr = qt_screen-> mapToDevice ( wr, QSize ( qt_screen-> width ( ), qt_screen-> height ( )));
#endif

	QWSServer::setMaxWindowRect( wr );
#endif
}
                                        

void LoginWindowImpl::keyPressEvent ( QKeyEvent *e )
{
	switch ( e-> key ( )) {
		case HardKey_Suspend:   suspend ( );
		                        break;
		case HardKey_Backlight: backlight ( );
		                        break;
		default:                e-> ignore ( );
		                        break;
	}
	LoginWindow::keyPressEvent ( e );
}


void LoginWindowImpl::toggleEchoMode ( bool t )
{
	m_password-> setEchoMode ( t ? QLineEdit::Normal : QLineEdit::Password );
}

void LoginWindowImpl::showIM ( )
{
	m_input-> showInputMethod ( );
}

void LoginWindowImpl::restart ( )
{
	qApp-> quit ( );
}

void LoginWindowImpl::quit ( )
{
	lApp-> quitToConsole ( );
}

void LoginWindowImpl::suspend ( )
{
	ODevice::inst ( )-> suspend ( );

	QCopEnvelope e("QPE/System", "setBacklight(int)");
	e << -3; // Force on
}

void LoginWindowImpl::backlight ( )
{
	QCopEnvelope e("QPE/System", "setBacklight(int)");
	e << -2; // toggle
}

class WaitLogo : public QLabel {
public:
	WaitLogo ( ) : QLabel ( 0, "wait hack!", WStyle_Customize | WStyle_NoBorder | WStyle_Tool )
	{
		QImage img = Resource::loadImage ( "launcher/new_wait" );
		QPixmap pix;
		pix. convertFromImage ( img );
		setPixmap ( pix );
		setAlignment ( AlignCenter );
		move ( 0, 0 );
		resize ( qApp-> desktop ( )-> width ( ), qApp-> desktop ( )-> height ( ));

		m_visible = false;
		show ( );
	}

	virtual void showEvent ( QShowEvent *e )
	{
		QLabel::showEvent ( e );
		m_visible = true;
	}

	virtual void paintEvent ( QPaintEvent *e )
	{
		QLabel::paintEvent ( e );
		if ( m_visible )
			qApp-> quit ( );
	}

private:
	bool m_visible;
};

void LoginWindowImpl::login ( )
{
	const char *user = ::strdup ( m_user-> currentText ( ). local8Bit ( ));
	const char *pass = ::strdup ( m_password-> text ( ). local8Bit ( ));

	if ( !user || !user [0] )
		return;
	if ( !pass )
		pass = "";

	if ( lApp-> checkPassword ( user, pass )) {
		Config cfg ( "opie-login" );
		cfg. setGroup ( "General" );
		cfg. writeEntry ( "LastLogin", user );
		cfg. write ( );

		lApp-> setLoginAs ( user );

		// Draw a big wait icon, the image can be altered in later revisions
		m_input-> hideInputMethod ( );
		new WaitLogo ( );
		// WaitLogo::showEvent() calls qApp-> quit()
	}
	else {
		QMessageBox::warning ( this, tr( "Wrong password" ), tr( "The given password is incorrect." ));
		m_password-> clear ( );
	}
}
