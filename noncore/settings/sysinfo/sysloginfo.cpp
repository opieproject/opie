/**********************************************************************
** SyslogInfo
**
** Display Syslog information
**
** Copyright (C) 2004, Michael Lauer
**                    mickey@tm.informatik.uni-frankfurt.de
**                    http://www.Vanille.de
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#include "sysloginfo.h"
#include "detail.h"

/* OPIE */
#include <opie2/olistview.h>
#include <opie2/oconfig.h>
using namespace Opie::Core;
using namespace Opie::Ui;

/* QT */
#include <qcombobox.h>
#include <qfile.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qsocketnotifier.h>
#include <qtextbrowser.h>
#include <qtimer.h>
#include <qwhatsthis.h>
#include <qtextview.h>

/* STD */
#include <sys/klog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define SYSLOG_READ 2
#define SYSLOG_READ_ALL 3
#define SYSLOG_READ_ALL_CLEAR 4
#define SYSLOG_UNREAD 9

#undef APPEND

const unsigned int bufsize = 16384;
char buf[bufsize];

SyslogInfo::SyslogInfo( QWidget* parent,  const char* name, WFlags fl )
        : QWidget( parent, name, fl )
{
    QGridLayout *layout = new QGridLayout( this );
    layout->setSpacing( 4 );
    layout->setMargin( 4 );

    syslogview = new QTextView( this );
    syslogview->setTextFormat( PlainText );
    OConfig cfg( "qpe" );
    cfg.setGroup( "Appearance" );
    syslogview->setFont( QFont( "Fixed", cfg.readNumEntry( "FontSize", 10 ) ) );
    layout->addWidget( syslogview, 0, 0 );
    syslogview->setText( "..." );

    memset( buf, 0, bufsize );
    ::klogctl( SYSLOG_READ_ALL, buf, bufsize );
    syslogview->setText( buf );

#ifdef APPEND
    fd = ::open( "/proc/kmsg", O_RDONLY|O_SYNC );
    if ( fd == -1 )
    {
        syslogview->setText( "Couldn't open /proc/kmsg: " + QString( strerror( errno ) ) );
        return;
    }
    QSocketNotifier *sn = new QSocketNotifier( fd, QSocketNotifier::Read, this );
    QObject::connect( sn, SIGNAL(activated(int)), this, SLOT(updateData()) );
#else
    QPushButton* pb = new QPushButton( "&Refresh", this );
    layout->addWidget( pb, 1, 0 );
    QObject::connect( pb, SIGNAL(clicked()), this, SLOT(updateData()) );
#endif
}

SyslogInfo::~SyslogInfo()
{
    if ( fd != -1 ) ::close( fd );
}

void SyslogInfo::updateData()
{
    qDebug( "SyslogInfo: updateData" );
#ifdef APPEND
    memset( buf, 0, bufsize );
    int num = ::read( fd, buf, bufsize );
    if ( num ) // -1 = error (permission denied)
    {
        syslogview->append( "\n" );
        syslogview->append( buf );
        qDebug( "SyslogInfo: adding '%s'", buf );
    }
#else
    memset( buf, 0, bufsize );
    ::klogctl( SYSLOG_READ_ALL, buf, bufsize );
    syslogview->setText( buf );
    syslogview->ensureVisible( 0, syslogview->contentsHeight() );
#endif
}
