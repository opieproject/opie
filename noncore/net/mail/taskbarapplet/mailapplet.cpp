#include <qlayout.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qsound.h>
#include <qtimer.h>
#include <qdir.h>

#include <qpe/qcopenvelope_qws.h>
#include <qpe/resource.h>
#include <qpe/config.h>

#include <opie/odevice.h>

#include "mailapplet.h"

using namespace Opie;

MailApplet::MailApplet( QWidget *parent, const char *name, WFlags fl )
	: QButton( parent, name, fl ) {

	m_config = new Config( "mail" );
	m_config->setGroup( "Applet" );

	QVBoxLayout *layout = new QVBoxLayout( this );
	layout->addItem( new QSpacerItem( 0,0 ) );

	QLabel *pixmap = new QLabel( this );
	pixmap->setPixmap( Resource::loadPixmap( "opiemail/mailchecker" ) );
	layout->addWidget( pixmap );

	layout->addItem( new QSpacerItem( 0,0 ) );

	hide();

	connect( this, SIGNAL( clicked() ), SLOT( slotClicked() ) );

	if ( !m_config->readBoolEntry( "Disabled", false ) ) {
		m_intervalMs = m_config->readNumEntry( "CheckEvery", 5 ) * 60000;
		m_intervalTimer = new QTimer();
		m_intervalTimer->start( m_intervalMs );
		connect( m_intervalTimer, SIGNAL(timeout() ), SLOT( slotCheck() ) );

		QTimer::singleShot( 0, this, SLOT( slotCheck() ) );
	}
}

void MailApplet::drawButton(QPainter *) { }
void MailApplet::drawButtonText(QPainter *) { }

void MailApplet::slotClicked() {
  qDebug( " CLICKED" );
	QCopEnvelope e( "QPE/System", "execute(QString)" );
	e << QString( "opiemail" );

	ODevice *device = ODevice::inst();
	if ( !device-> ledList ( ). isEmpty ( ) ) {
		OLed led = ( device-> ledList ( ). contains ( Led_Mail )) ? Led_Mail : device-> ledList ( ) [0];

		device->setLedState( led, Led_Off );
	}
}

void MailApplet::slotCheck() {
	// Check wether the check interval has been changed.
	int newIntervalMs = m_config->readNumEntry( "CheckEvery", 5 ) * 60000;
	if ( newIntervalMs != m_intervalMs ) {
		m_intervalTimer->changeInterval( newIntervalMs );
		m_intervalMs = newIntervalMs;
	}

        // go trough accounts and check here
        // depending on result show or hide
        // also trigger qcop call and save status to config
        // get led to blink
}
