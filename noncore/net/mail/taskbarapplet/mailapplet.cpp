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
#include <qlist.h>

#include <libmailwrapper/settings.h>

#include "mailapplet.h"

using namespace Opie;

MailApplet::MailApplet( QWidget *parent, const char *name, WFlags fl )
	: QButton( parent, name, fl ) {

	m_config = new Config( "mail" );
	m_config->setGroup( "Applet" );

	QVBoxLayout *layout = new QVBoxLayout( this );
	layout->addItem( new QSpacerItem( 0,0 ) );

	QLabel *pixmap = new QLabel( this );
	pixmap->setPixmap( Resource::loadPixmap( "mail/mailchecker" ) );
	layout->addWidget( pixmap );

	layout->addItem( new QSpacerItem( 0,0 ) );

	hide();

        m_statusMail = 0;

  	connect( this, SIGNAL( clicked() ), SLOT( slotClicked() ) );

	if ( !m_config->readBoolEntry( "Disabled", false ) ) {
		m_intervalMs = m_config->readNumEntry( "CheckEvery", 5 ) * 60000;
		m_intervalTimer = new QTimer();
		m_intervalTimer->start( m_intervalMs );
		connect( m_intervalTimer, SIGNAL(timeout() ), SLOT( slotCheck() ) );

                // delay 5 sec until the whole mail backend gets started .-)
		QTimer::singleShot( 5000, this, SLOT( startup() ) );
	}
}

MailApplet::~MailApplet() {
       if (m_statusMail) delete m_statusMail;
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

   //        m_statusMails->reset_status();
}

void MailApplet::startup() {
      Settings *settings = new Settings();
      QList<Account> ma = settings->getAccounts();
      StatusMail m_statusMail = StatusMail( ma );
      delete settings;
}

void MailApplet::slotCheck() {
	// Check wether the check interval has been changed.
	int newIntervalMs = m_config->readNumEntry( "CheckEvery", 5 ) * 60000;
	if ( newIntervalMs != m_intervalMs ) {
		m_intervalTimer->changeInterval( newIntervalMs );
		m_intervalMs = newIntervalMs;
	}


        folderStat stat;
        m_statusMail->check_current_stat( stat );

        qDebug( QString( "test %1" ).arg( stat.message_unseen ) );
       	if ( stat.message_unseen > 0 ) {
		ODevice *device = ODevice::inst();
		if ( isHidden() ) show();
		if ( m_config->readBoolEntry( "BlinkLed", true ) ) {
			if ( !device-> ledList ( ).isEmpty( ) ) {
				OLed led = ( device->ledList( ).contains( Led_Mail ) ) ? Led_Mail : device->ledList( ) [0];
					device->setLedState( led, device->ledStateList( led ).contains( Led_BlinkSlow ) ? Led_BlinkSlow : Led_On );
			}
		}
		if ( m_config->readBoolEntry( "PlaySound", false ) )
			device->alarmSound();

        qDebug( QString( "test %1" ).arg( stat.message_unseen ) );

        Config cfg( "mail" );
        cfg.setGroup( "Status" );
        cfg.writeEntry( "NewMails", ( int )stat.message_unseen  );
    QCopEnvelope env( "QPE/Pim", "newMails(int)" );
    env << stat.message_unseen;

	} else {
		ODevice *device = ODevice::inst();
		if ( !isHidden() ) hide();
		if ( !device-> ledList( ).isEmpty( ) ) {
			OLed led = ( device->ledList( ).contains( Led_Mail ) ) ? Led_Mail : device->ledList( ) [0];
				device->setLedState( led, Led_Off );
		}
	}

        // go trough accounts and check here
        // also trigger qcop call and save status to config
}
