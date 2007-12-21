/*
                             This file is part of the Opie Project
             =.              (C) Copyright 2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
            .=l.             (C) Copyright 2007 Paul Eggleton <bluelightning@bluelightning.org>
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "cardapplet.h"
#include "configdialog.h"
#include "promptactiondialog.h"

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/odevice.h>
#include <opie2/oconfig.h>
#include <opie2/oprocess.h>
#include <opie2/opcmciasystem.h>
#include <opie2/oresource.h>
#include <opie2/otaskbarapplet.h>
#include <qpe/applnk.h>
#include <qpe/global.h>
#include <qpe/resource.h>
using namespace Opie::Core;
using namespace Opie::Ui;

/* QT */
#include <qcombobox.h>
#include <qcopchannel_qws.h>
#include <qpainter.h>
#include <qfile.h>
#include <qdir.h>
#include <qtextstream.h>
#include <qmessagebox.h>
#include <qsound.h>
#include <qtimer.h>

/* STD */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#if defined(_OS_LINUX_) || defined(Q_OS_LINUX)
#include <sys/vfs.h>
#include <mntent.h>
#endif

enum { CMD_UNMOUNT, CMD_FUSER, CMD_PSINFO };

CardApplet::CardApplet( QWidget * parent ) : QWidget( parent ), popupMenu( 0 )
{
    updateMounts( false );

    QCopChannel * pcmciaChannel = new QCopChannel( "QPE/Card", this );
    connect( pcmciaChannel,
             SIGNAL( received(const QCString&,const QByteArray&) ), this,
             SLOT( cardMessage(const QCString&,const QByteArray&) ) );

    setFocusPolicy( NoFocus );
    setFixedWidth ( AppLnk::smallIconSize() );
    setFixedHeight ( AppLnk::smallIconSize() );
    pm = Opie::Core::OResource::loadPixmap( "pcmcia", Opie::Core::OResource::SmallIcon );
    configuring = false;

    QCopChannel *channel = new QCopChannel( "QPE/System", this );
    connect( channel, SIGNAL(received(const QCString&,const QByteArray&)),
                this, SLOT(handleSystemChannel(const QCString&,const QByteArray&)) );
}


CardApplet::~CardApplet()
{
}

void CardApplet::handleSystemChannel( const QCString&msg, const QByteArray& )
{
    if ( msg == "returnFromSuspend()" )
    {
        if ( !OPcmciaSystem::instance()->cardCount() ) return;
        OPcmciaSystem* sys = OPcmciaSystem::instance();
        OPcmciaSystem::CardIterator it = sys->iterator();

        while ( it.current() )
        {
            if ( !it.current()->isEmpty() )
            {
                executeAction( it.current(), "resume" );
            }
        ++it;
        }
    }
}

void CardApplet::popUp( QString message, QString icon )
{
    if ( !popupMenu)
    {
        popupMenu = new QPopupMenu( this );
        popupMenu->setFocusPolicy( QWidget::NoFocus );
    }
    popupMenu->clear();

    if ( icon.isEmpty() ) {
        popupMenu->insertItem( message, 0 );
    } else {
        popupMenu->insertItem( QIconSet( Opie::Core::OResource::loadPixmap( icon, Opie::Core::OResource::SmallIcon ) ),
                               message, 0 );
    }

    QPoint p = mapToGlobal( QPoint( 0, 0 ) );
    QSize s = popupMenu->sizeHint();
    popupMenu->popup( QPoint( p.x() + ( width() / 2 ) - ( s.width() / 2 ),
                              p.y() - s.height() ), 0 );

    QTimer::singleShot( 2500, this, SLOT( popupTimeout() ) );
}


void CardApplet::popupTimeout()
{
    popupMenu->hide();
}

enum { EJECT, INSERT, SUSPEND, RESUME, RESET, CONFIGURE, ACTIVATE };
static const char* actionText[] = { "eject", "insert", "suspend", "resum", "resett", "configur", "activat" };

void CardApplet::mousePressEvent( QMouseEvent* )
{
    QPopupMenu* menu = new QPopupMenu( this );
    QStringList cmd;

    int i = 0;

    OPcmciaSystem* sys = OPcmciaSystem::instance();
    sys->synchronize();
    if ( sys->count() ) {
        OPcmciaSystem::CardIterator it = sys->iterator();
        while ( it.current() )
        {
            QPopupMenu* submenu = new QPopupMenu( menu );
            submenu->insertItem( "&Eject",     EJECT+i*100 );
            submenu->insertItem( "&Insert",    INSERT+i*100 );
            submenu->insertItem( "&Suspend",   SUSPEND+i*100 );
            submenu->insertItem( "&Resume",    RESUME+i*100 );
            submenu->insertItem( "Rese&t",     RESET+i*100 );
            submenu->insertItem( "&Configure", CONFIGURE+i*100 );

            bool isSuspended = it.current()->isSuspended();
            bool isEmpty = it.current()->isEmpty();

            submenu->setItemEnabled( EJECT+i*100, !isEmpty );
            submenu->setItemEnabled( INSERT+i*100, isEmpty );
            submenu->setItemEnabled( SUSPEND+i*100, !isEmpty && !isSuspended );
            submenu->setItemEnabled( RESUME+i*100, !isEmpty && isSuspended );
            submenu->setItemEnabled( RESET+i*100, !isEmpty && !isSuspended );
            submenu->setItemEnabled( CONFIGURE+i*100, !isEmpty && !configuring );

            connect( submenu, SIGNAL(activated(int)), this, SLOT(userCardAction(int)) );
            menu->insertItem( tr( "%1: %2" ).arg( i++ ).arg( it.current()->identity() ), submenu, 1 );
            ++it;
        }
    }

    if ( m_mounts.count() ) {
        if ( sys->count() )
            menu->insertSeparator(i++);

        QPixmap mpm = Opie::Core::OResource::loadPixmap( "cardmon/ide", Opie::Core::OResource::SmallIcon );
        for( QStringList::Iterator it = m_mounts.begin(); it != m_mounts.end(); ++it ) {
            QPopupMenu* submenu = new QPopupMenu( menu );
            submenu->insertItem( tr("Safely remove"), EJECT+((i+1)*1000) );
            connect( submenu, SIGNAL(activated(int)), this, SLOT(userCardAction(int)) );
            menu->insertItem( mpm, tr( "%1" ).arg( (*it) ), submenu, i++ );
        }
    }
    else if ( sys->count() == 0 )
        menu->insertItem( tr( "No cards" ), 1 );

    QPoint p = mapToGlobal( QPoint( 0, 0 ) );
    QSize s = menu->sizeHint();
    int opt = menu->exec( QPoint( p.x() + ( width() / 2 ) - ( s.width() / 2 ), p.y() - s.height() ), 0 );
    qDebug( "pcmcia: menu result = %d", opt );
    delete menu;
}


void CardApplet::cardMessage( const QCString & msg, const QByteArray & )
{
    odebug << "CardApplet::cardMessage( '" << msg << "' )" << oendl;
    if ( msg == "stabChanged()" ) {
        updatePcmcia();
    }
    else if ( msg == "mtabChanged()" ) {
        // Another device was mounted/unmounted
        updateMounts( true );
    }
}

void CardApplet::updatePcmcia()
{
    /* check if a previously unknown card has been inserted */
    OPcmciaSystem::instance()->synchronize();

    if ( !OPcmciaSystem::instance()->cardCount() ) return;

    OConfig cfg( "PCMCIA" );
    cfg.setGroup( "Global" );
    int nCards = cfg.readNumEntry( "nCards", 0 );

    OPcmciaSystem* sys = OPcmciaSystem::instance();
    OPcmciaSystem::CardIterator it = sys->iterator();

    bool newCard = true;
    OPcmciaSocket* theCard = 0;

    while ( it.current() && newCard )
    {
        if ( it.current()->isEmpty() )
        {
            odebug << "pcmcia: skipping empty card in socket " << it.current()->number() << oendl;
            ++it;
            continue;
        }
        else
        {
            theCard = it.current();
            QString cardName = theCard->productIdentity();
            for ( int i = 0; i < nCards; ++i )
            {
                QString cardSection = QString( "Card_%1" ).arg( i );
                cfg.setGroup( cardSection );
                QString name = cfg.readEntry( "name" );
                odebug << "pcmcia: comparing card '" << cardName << "' with known card '" << name << "'" << oendl;
                if ( cardName == name )
                {
                    newCard = false;
                    odebug << "pcmcia: we have seen this card before" << oendl;
                    executeAction( theCard, "insert" );
                    break;
                }
            }
            if ( !newCard ) ++it; else break;
        }
    }

    if ( !theCard ) {
        owarn << "pcmcia: Finished working through cards in PCMCIA system but I do not have a valid card handle" << oendl;
        return;
    }

    if ( newCard )
    {
        odebug << "pcmcia: unconfigured card detected" << oendl;
        QString newCardName = theCard->productIdentity();
        int result = QMessageBox::information( qApp->desktop(),
                                        tr( "PCMCIA/CF Subsystem" ),
                                        tr( "<qt>You have inserted the card<br/><b>%1</b><br/>This card is not yet configured. Do you want to configure it now?</qt>" ).arg( newCardName ),
                                        tr( "Yes" ), tr( "No" ), 0, 0, 1 );
        odebug << "pcmcia: result = " << result << oendl;
        if ( result == 0 )
        {
            configure( theCard );
        }
        else
        {
            odebug << "pcmcia: user doesn't want to configure " << newCardName << " now." << oendl;
        }
    }
    else // it's an already configured card
    {
        odebug << "pcmcia: doing nothing... why do we come here?" << oendl;
    }
}

void CardApplet::updateMounts( bool showPopup )
{
    QStringList blockdevs;
    
    struct mntent *me;
    FILE *mntfp = setmntent( "/etc/mtab", "r" );

    if ( mntfp ) {
        QDir d("/sys/block");
        d.setFilter( QDir::Dirs );
        const QFileInfoList *list = d.entryInfoList();
        QFileInfo *fi;

        // Iterate over mount points
        while ( ( me = getmntent( mntfp ) ) != 0 ) {
            QString fs = QFile::decodeName( me->mnt_fsname );
            // Find which mounted devices are ones we are interested in
            // FIXME skip previously known devices here?
            if(fs.startsWith("/dev/")) {
                QString blockdev = fs.mid(5);

                QFileInfoListIterator it( *list );
                while ( (fi=it.current()) ) {
                    QString fpath = fi->filePath();
                    if ( QFile::exists( fpath + "/" + blockdev ) || fpath == blockdev ) {
                        // Attempt to determine if the device is removable
                        QFile f(fpath + "/removable");
                        if ( f.open(IO_ReadOnly) ) {
                            if(f.getch() == '1') {
                                odebug << "CARDAPPLET: " << fs << oendl;
                                blockdevs << fs;
                            }
                            else
                                odebug << fs << " is not removable" << oendl;
                        }
                        else
                            odebug << "could not open " << fpath + "/removable" << oendl;
                        break;
                    }
                    ++it;
                }
            }
        }
        endmntent( mntfp );
    }

    if ( showPopup ) {
        for( QStringList::Iterator it = blockdevs.begin(); it != blockdevs.end(); ++it )
            if(!m_mounts.contains(*it))
                mountChanged( (*it), true );

        for( QStringList::Iterator it = m_mounts.begin(); it != m_mounts.end(); ++it )
            if(!blockdevs.contains(*it))
                mountChanged( (*it), false );
    }
    m_mounts = blockdevs;
}


void CardApplet::mountChanged( const QString &device, bool mounted )
{
    QString text = QString::null;
    QString what = QString::null;
    if ( mounted ) {
        text += QObject::tr("Inserted %1").arg(device); // FIXME description
        what = "on";
    } else {
        text += QObject::tr("Removed %1").arg(device);
        what = "off";
    }
    #ifndef QT_NO_SOUND
    QSound::play( Resource::findSound( "cardmon/card" + what ) );
    #endif
    popUp( text, "cardmon/ide" );    // FIXME add SD pic
}

void CardApplet::paintEvent( QPaintEvent * )
{
    QPainter p( this );
    p.drawPixmap( 0, 0, pm );
}

int CardApplet::position()
{
    return 7;
}

void CardApplet::execCommand( const QStringList &strList )
{
    delete m_process;
    m_process = 0;

    if ( m_process == 0 ) {
        m_process = new OProcess();


        for ( QStringList::ConstIterator it = strList.begin(); it != strList.end(); ++it ) {
            *m_process << *it;
        }

        connect(m_process, SIGNAL(processExited(Opie::Core::OProcess*)),
                this, SLOT( slotExited(Opie::Core::OProcess*)));
        connect(m_process,  SIGNAL(receivedStdout(Opie::Core::OProcess*, char*,  int ) ),
                this,  SLOT(slotStdout(Opie::Core::OProcess*, char*, int) ) );
        connect(m_process,  SIGNAL(receivedStderr(Opie::Core::OProcess*, char*,  int ) ),
                this,  SLOT(slotStderr(Opie::Core::OProcess*, char*, int) ) );

        m_commandStdout = "";
        m_commandStderr = "";

        if(!m_process->start(OProcess::NotifyOnExit, OProcess::AllOutput ) ) {
            delete m_process;
            m_process = 0;
        }
    }
}

void CardApplet::slotStdout( OProcess* proc, char* buf, int len )
{
    m_commandStdout += QString::fromLocal8Bit(buf, len) + '\n';
}

void CardApplet::slotStderr( OProcess* proc, char* buf, int len )
{
    m_commandStderr += QString::fromLocal8Bit(buf, len) + '\n';
}

void CardApplet::slotExited( OProcess*  )
{
    if( m_process->normalExit() ) {
        int ret = m_process->exitStatus();
        if( ret != 0 ) {
            if ( m_commandOrig == CMD_UNMOUNT ) {
                QMessageBox::warning(0,
                    tr("Remove failed"),
                    tr("<p>Unmounting the selected device failed:"
                       "<p>%1").arg( m_commandStderr ) );
            }
            else if ( m_commandOrig == CMD_FUSER ) {
                // fuser check was OK
                unmount(m_mountpt, false);
            }
            else if ( m_commandOrig == CMD_PSINFO ) {
                // Getting info on used processes failed, can't do much more than show a generic message
                QMessageBox::warning(0,
                    tr("Card in use"),
                    tr("<p>The device you are trying to remove is in use. Please check that no applications are using this device and then try again." ));
            }
        }
        else if ( m_commandOrig == CMD_FUSER ) {
            // fuser check returned a list of processes
            QString procs = m_commandStdout;
            getProcessInfo(procs);
        }
        else if ( m_commandOrig == CMD_PSINFO ) {
            QMessageBox::warning(0,
                tr("Card in use"),
                tr("<p>The device you are trying to remove is in use by the following applications:</p><pre>%1</pre><p>Please close these applications and then try again.").arg( m_commandStdout.stripWhiteSpace() ) );
        }
    }
}

void CardApplet::userCardAction( int action )
{
    if(action < 1000) {
        // PCMCIA card action
        int socket = action / 100;
        int what = action % 100;
        bool success = false;

        odebug << "cardapplet: user action on pcmcia socket " << socket << " requested. action = " << what << oendl;

        switch ( what )
        {
            case CONFIGURE:
            {
                QString insertAction; QString resumeAction; QString driver; QString conf;
                configure( OPcmciaSystem::instance()->socket( socket ) );
                return;
            }
            case EJECT:    success = OPcmciaSystem::instance()->socket( socket )->eject();
                        break;
            case INSERT:   success = OPcmciaSystem::instance()->socket( socket )->insert();
                        break;
            case SUSPEND:  success = OPcmciaSystem::instance()->socket( socket )->suspend();
                        break;
            case RESUME:   success = OPcmciaSystem::instance()->socket( socket )->resume();
                        break;
            case RESET:    success = OPcmciaSystem::instance()->socket( socket )->reset();
                        break;
            case ACTIVATE: success = true;
                        break;
            default:       odebug << "pcmcia: not yet implemented" << oendl;
        }

        if ( success )
        {
            odebug << tr( "Successfully %1ed card in socket #%2" ).arg( actionText[action] ).arg( socket ) << oendl;
            popUp( tr( "Successfully %1ed card in socket #%2" ).arg( actionText[action] ).arg( socket ) );
        }
        else
        {
            odebug << tr( "Error while %1ing card in socket #%2" ).arg( actionText[action] ).arg( socket ) << oendl;
            popUp( tr( "Error while %1ing card in socket #%2" ).arg( actionText[action] ).arg( socket ) );
        }
    }
    else {
        // Other mount point action
        int mountpoint = (action / 1000) - 1;
        int what = action % 1000;

        odebug << "cardapplet: user action on mount point " << mountpoint << " requested. action = " << what << oendl;

        if( what == EJECT ) {
            if( mountpoint < m_mounts.count() )
                unmount( m_mounts[mountpoint], true);
            else
                odebug << "mount point out of range!" << oendl;
        }
        else
            odebug << "unrecognised mount point action!" << oendl;
    }
}

void CardApplet::configure( OPcmciaSocket* card )
{
    configuring = true;
    ConfigDialog dialog( card, qApp->desktop() );
    int result = QPEApplication::execDialog( &dialog, false );
    configuring = false;
    odebug << "pcmcia: configresult = " << result << oendl;
    if ( result )
    {
        dialog.writeConfiguration( card );
    }
}

void CardApplet::executeAction( Opie::Core::OPcmciaSocket* card, const QString& type )
{
    odebug << "pcmcia: performing " << type << " action ..." << oendl;
    QString theAction = ConfigDialog::preferredAction( card, type );
    int intAction = card->number() * 100;

    if ( theAction == "prompt for" )
    {
        PromptActionDialog dialog( qApp->desktop(), "promptfor", true );
        dialog.setCaption( QString( "Choose action for card #%1" ).arg( card->number() ) );
        int result = QPEApplication::execDialog( &dialog, true );
        odebug << "pcmcia: configresult = " << result << oendl;
        if ( result )
        {
            theAction = dialog.cbAction->currentText();
        }
        else
        {
            odebug << "pcmcia: prompted to do nothing" << oendl;
            return;
        }
    }
    if ( theAction == "activate" ) intAction += ACTIVATE;
    else if ( theAction == "eject" ) intAction += EJECT;
    else if ( theAction == "suspend" ) intAction += SUSPEND;
    else
    {
        owarn << "pcmcia: action '" << theAction << "' not known. Huh?" << oendl;
        return;
    }
    userCardAction( intAction );
}

void CardApplet::unmount( const QString &device, bool fusercheck )
{
    odebug << "Unmounting " << device << oendl;
    QStringList cmd;

    if(fusercheck) {
        cmd << "fuser" << "-m" << device;
        m_mountpt = device;
        m_commandOrig = CMD_FUSER;
    }
    else {
        cmd << "umount" << device;
        m_commandOrig = CMD_UNMOUNT;
    }
    execCommand( cmd );
}

void CardApplet::getProcessInfo( const QString &processes )
{
    QStringList cmd;
    QStringList procs = QStringList::split(QRegExp("[ \n]"), processes);
    cmd << "ps" << "-p" << procs.join(",") << "-o" << "comm,pid";
    m_commandOrig = CMD_PSINFO;
    execCommand( cmd );
}

EXPORT_OPIE_APPLET_v1( CardApplet )

