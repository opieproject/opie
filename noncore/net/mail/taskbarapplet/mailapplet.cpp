#include <qpainter.h>
#include <qtimer.h>

#include <qpe/qcopenvelope_qws.h>
#include <qpe/resource.h>
#include <qpe/config.h>
#include <qpe/applnk.h>
#include <opie2/odebug.h>
#include <opie2/odevice.h>
#include <qpe/sound.h>

#include <libmailwrapper/settings.h>

#include "mailapplet.h"

/* UNIX */
#include <signal.h>

using namespace Opie::Core;

MailApplet::MailApplet( QWidget *parent )
: QLabel( parent ) {

    setFixedHeight(AppLnk::smallIconSize());
    setMinimumWidth(AppLnk::smallIconSize());

    hide();

    m_newMails = 0;
    m_statusMail = 0l;

    /* for networking we must block SIGPIPE and Co. */
    struct sigaction blocking_action,temp_action;
    blocking_action.sa_handler = SIG_IGN;
    sigemptyset(&(blocking_action.sa_mask));
    blocking_action.sa_flags = 0;
    sigaction(SIGPIPE,&blocking_action,&temp_action);

    QTimer::singleShot( 5000, this, SLOT( startup() ) );
}


MailApplet::~MailApplet() {
    if ( m_statusMail )
        delete m_statusMail;
}

void MailApplet::paintEvent( QPaintEvent*ev )
{
    QPainter p( this );
    p.drawPixmap( 0, 0, Resource::loadPixmap( "mail/inbox" ) );
    QLabel::paintEvent(ev);
#if 0
    QFont f( "vera", AppLnk::smallIconSize() );
    QFontMetrics fm( f );
    p.setFont( f );
    p.setPen( Qt::blue );
    p.drawText( AppLnk::smallIconSize()/3, AppLnk::smallIconSize() - 2, QString::number( m_newMails ) );
#endif
    return;
}

void MailApplet::mouseReleaseEvent( QMouseEvent* e ) {
    slotClicked();
}

void MailApplet::slotClicked()
{
    {
        QCopEnvelope e( "QPE/System", "execute(QString)" );
        e << QString( "opiemail" );
    }

    ledOnOff(false);
    if (m_statusMail)
        m_statusMail->reset_status();
    hide();
}

void MailApplet::startup()
{
    Settings *settings = new Settings();
    QList<Account> ma = settings->getAccounts();
    m_statusMail = new StatusMail( ma );
    delete settings;

    //m_intervalMs = m_config->readNumEntry( "CheckEvery", 5 ) * 60000;
    m_intervalMs = 100;
    m_intervalTimer = new QTimer();
    m_intervalTimer->start( m_intervalMs );
    connect( m_intervalTimer, SIGNAL( timeout() ), this, SLOT( slotCheck() ) );
}

void MailApplet::ledOnOff(bool how)
{
    ODevice *device = ODevice::inst();
    if ( !device->ledList().isEmpty() ) {
        OLed led = ( device->ledList().contains( Led_Mail ) ) ? Led_Mail : device->ledList()[0];
        device->setLedState( led,  (how?(device->ledStateList( led ).contains( Led_BlinkSlow )?Led_BlinkSlow:Led_On):Led_Off) );
    }
}

void MailApplet::slotCheck() {
    // Check wether the check interval has been changed.
    odebug << "MailApplet::slotCheck()" << oendl;
    Config m_config( "mail" );
    m_config.setGroup( "Applet" );

    int newIntervalMs = m_config.readNumEntry( "CheckEvery", 5 ) * 60000;
    if ( newIntervalMs != m_intervalMs ) {
        m_intervalTimer->changeInterval( newIntervalMs );
        m_intervalMs = newIntervalMs;
    }
    if ( m_config.readBoolEntry( "Disabled", false ) ) {
        hide();
        ledOnOff(false);
        odebug << "MailApplet::slotCheck() - disabled" << oendl;
        return;
    }
    if (m_statusMail == 0) {
        odebug << "MailApplet::slotCheck() - no mailhandle" << oendl;
        return;
    }
    folderStat stat;
    m_statusMail->check_current_stat( stat );
    int newMailsOld = m_newMails;
    m_newMails = stat.message_unseen;
    odebug << QString( "test %1" ).arg( m_newMails ) << oendl;
    if ( m_newMails > 0) {
        if (isHidden())
            show();
        if (newMailsOld != m_newMails) {
            if ( m_config.readBoolEntry( "BlinkLed", true ) ) {
                ledOnOff(true);
            }
            if ( m_config.readBoolEntry( "PlaySound", false ) )
                Sound::soundAlarm();
            m_config.setGroup( "Status" );
            m_config.writeEntry( "newMails", m_newMails );
            {
                QCopEnvelope env( "QPE/Pim", "newMails(int)" );
                env <<  m_newMails;
            }
            setText(QString::number( m_newMails ));
        }
    } else {
        ODevice *device = ODevice::inst();
        if ( !isHidden() )
            hide();
        if ( newMailsOld != m_newMails ) {
            ledOnOff(false);
            m_config.setGroup( "Status" );
            m_config.writeEntry( "newMails", m_newMails );
            QCopEnvelope env( "QPE/Pim", "newMails(int)" );
            env <<  m_newMails;
        }
    }
}
