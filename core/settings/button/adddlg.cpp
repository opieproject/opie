
#include "adddlg.h"
#include "buttonutils.h"

/* OPIE */
#include <opie2/odevice.h>
#include <opie2/odebug.h>

/* Qt */
#include <qlayout.h>
#include <qlabel.h>
#include <qapplication.h>


using namespace Opie::Core;


AddDlg::AddDlg( QWidget* parent, const char* name )
    : QWidget( parent, name, Qt::WType_Modal )
{
    m_last_key = 0;
    m_closing = false;

    QVBoxLayout *vb = new QVBoxLayout( this );
    m_instrLabel = new QLabel( this );
    m_instrLabel->setText( "<center>" + QObject::tr( "Hold button for 3s..." ) + "</center>" );
    vb->addWidget( m_instrLabel );
    setGeometry( qApp->desktop()->width() / 8, (qApp->desktop()->height() / 2) - 25,
                 3 * (qApp->desktop()->width() / 4), 50 );

    m_keyLabel = new QLabel( this );
    vb->addWidget( m_keyLabel );

    m_timer = new QTimer( this );
    connect ( m_timer, SIGNAL( timeout() ), this, SLOT( keyTimeout() ));
}

AddDlg::~AddDlg()
{
}

void AddDlg::keyPressEvent ( QKeyEvent *e )
{
    if( !e->isAutoRepeat() ) {
        m_keyLabel->setText( "<center>" + ODevice::keyToString( e->key() ) + "</center>" );
        m_timer->stop();
        m_last_key = e->key();
        m_timer->start ( 2000, true );
    }
}

void AddDlg::keyReleaseEvent ( QKeyEvent *e )
{
    if( !e->isAutoRepeat() ) {
        if( m_closing ) {
            close();
        }
        else {
            m_keyLabel->clear();
            m_timer->stop();
        }
    }
}

void AddDlg::keyTimeout()
{
    m_closing = true;
    m_keyLabel->setText( "<b>" + m_keyLabel->text() + "</b>" );
    m_instrLabel->setText( "<center>" + QObject::tr( "Now release the button..." ) + "</center>" );
}

void AddDlg::closeEvent( QCloseEvent *e )
{
    QWidget::closeEvent( e );
    if( m_closing )
        emit closed( m_last_key );
    else
        emit closed( 0 );
}
