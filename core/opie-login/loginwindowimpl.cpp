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

	QHBoxLayout *lay = new QHBoxLayout ( m_taskbar, 4, 4 );
	m_input = new InputMethods ( m_taskbar );
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
}

LoginWindowImpl::~LoginWindowImpl ( )
{
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
