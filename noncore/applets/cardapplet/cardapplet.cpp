/*
 � � � � � � � �             This file is part of the Opie Project
             =.              (C) Copyright 2005 Michael 'Mickey' Lauer <mickey@Vanille.de>
            .=l.             (C) Copyright 2007 Paul Eggleton <bluelightning@bluelightning.org>
� � � � � �.>+-=
�_;:, � � .> � �:=|.         This program is free software; you can
.> <`_, � > �. � <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.-- � :           the terms of the GNU Library General Public
.="- .-=="i, � � .._         License as published by the Free Software
�- . � .-<_> � � .<>         Foundation; either version 2 of the License,
� � �._= =} � � � :          or (at your option) any later version.
� � .%`+i> � � � _;_.
� � .i_,=:_. � � �-<s.       This program is distributed in the hope that
� � �+ �. �-:. � � � =       it will be useful,  but WITHOUT ANY WARRANTY;
� � : .. � �.:, � � . . .    without even the implied warranty of
� � =_ � � � �+ � � =;=|`    MERCHANTABILITY or FITNESS FOR A
� _.=:. � � � : � �:=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.= � � � = � � � ;      Library General Public License for more
++= � -. � � .` � � .:       details.
�: � � = �...= . :.=-
�-. � .:....=;==+<;          You should have received a copy of the GNU
� -_. . . � )=. �=           Library General Public License along with
� � -- � � � �:-=`           this library; see the file COPYING.LIB.
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
#include <qpe/storage.h>
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
#include <sys/stat.h>
#endif

enum { CMD_UNMOUNT, CMD_FUSER, CMD_PSINFO };
enum { EM_NONE, EM_UNMOUNT, EM_EJECT };

CardApplet::CardApplet( QWidget * parent ) : QWidget( parent ), popupMenu( 0 )
{
    findRootBlock();
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
    m_ejectMode = EM_NONE;
    m_process = 0;

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
            submenu->insertItem( tr("&Eject"),     EJECT+i*100 );
            submenu->insertItem( tr("&Insert"),    INSERT+i*100 );
            submenu->insertItem( tr("&Suspend"),   SUSPEND+i*100 );
            submenu->insertItem( tr("&Resume"),    RESUME+i*100 );
            submenu->insertItem( tr("Rese&t"),     RESET+i*100 );
            submenu->insertItem( tr("&Configure"), CONFIGURE+i*100 );

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

        QMap<QString,QString>::Iterator it;
        int idx = 1;
        for( it = m_mounts.begin(); it != m_mounts.end(); ++it ) {
            QPopupMenu* submenu = new QPopupMenu( menu );
            submenu->insertItem( tr("&Eject"), EJECT+((idx++)*1000) );
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
        updateMounts( m_ejectMode == EM_NONE );
    }
}

void CardApplet::updatePcmcia()
{
    static bool excllock = false;

    if(excllock)
        return;
    
    excllock = true;
    
    OPcmciaSystem::instance()->synchronize();

    OPcmciaSystem* sys = OPcmciaSystem::instance();
    OPcmciaSystem::CardIterator it = sys->iterator();

    // This is protection against receiving duplicate events - for some
    // reason with the udev script on h2200 you get about 5 notifications
    // for each card insert.
    bool changed = false;
    while ( it.current() )
    {
        QString cardname = "$$$NOCARD$";
        int socketnum = it.current()->number();
        if ( ! it.current()->isEmpty() )
            cardname = it.current()->productIdentity();
        if ( m_cardnames[socketnum] != cardname ) {
            changed = true;
            m_cardnames[socketnum] = cardname;
        }
        ++it;
    }

    /* check if a previously unknown card has been inserted */
    if ( sys->cardCount() != 0 && changed ) {
        OConfig cfg( "PCMCIA" );
        cfg.setGroup( "Global" );
        int nCards = cfg.readNumEntry( "nCards", 0 );

        bool newCard = true;
        OPcmciaSocket* theCard = 0;

        it = sys->iterator();
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
        }
        else {
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
        }
    }

    excllock = false;
}

void CardApplet::updateMounts( bool showPopup )
{
    QMap<QString,QString> blockdevs;
    StorageInfo storage;

    struct mntent *me;
    FILE *mntfp = setmntent( "/etc/mtab", "r" );

    m_cardmounts.clear();
    if ( mntfp ) {
        QDir blockdir("/sys/block");
        blockdir.setFilter( QDir::Dirs );

        QDir pcmciadir("/sys/bus/pcmcia/devices");
        pcmciadir.setFilter( QDir::Dirs );

        QMap<QString,QString> pcmciadevs;
        for ( unsigned int i=0; i<pcmciadir.count(); i++ ) {
            QString devid = pcmciadir[i];
            if(devid[0] == '.') continue;
            QDir pcmciadevdir("/sys/bus/pcmcia/devices/" + devid);
            // Now we need to split out the socket number
            devid = QStringList::split('.', devid)[0];
            QString devnode = pcmciadevdir.canonicalPath();
            pcmciadevs.insert( devid, devnode );
            odebug << "pcmcia " << devid << " " << devnode << oendl;
            // Check if this is associated with the root device
            if(m_rootdevnode != "" && m_rootdevnode.startsWith(devnode)) {
                odebug << "CARD: root card is " << devid.toInt() << oendl;
                m_cardmounts.insert( "/", devid.toInt() );
            }
        }

        // Iterate over mount points
        while ( ( me = getmntent( mntfp ) ) != 0 ) {
            QString fs = QFile::decodeName( me->mnt_fsname );
            QString mntdir = QFile::decodeName( me->mnt_dir );
            // Find which mounted devices are ones we are interested in
            if(fs.startsWith("/dev/")) {
                QString blockdev = fs.mid(5);

                for ( unsigned int i=0; i<blockdir.count(); i++ ) {
                    if(blockdir[i][0] == '.') continue;
                    QString fpath = "/sys/block/" + blockdir[i];
                    if ( QFile::exists( fpath + "/" + blockdev ) || fpath == blockdev ) {
                        // Check if mount point is associated with a PCMCIA device
                        bool matched = false;
                        if(!pcmciadevs.isEmpty()) {
                            QDir devdir(fpath + "/device");
                            QString devlink = devdir.canonicalPath();
                            odebug << "devnode is " << devlink << oendl;

                            QMap<QString,QString>::Iterator pit;
                            for( pit = pcmciadevs.begin(); pit != pcmciadevs.end(); ++pit ) {
                                if ( devlink.startsWith(pit.data()) ) {
                                    m_cardmounts.insert( mntdir, pit.key().toInt() );
                                    matched = true;
                                    odebug << "MATCHED " << fs << " to " << pit.key() << oendl;
                                    break;
                                }
                            }
                        }

                        if (!matched) {
                            // Attempt to determine if the device is removable
                            // Actually, I think this first test is useless because the removable flag
                            // seems to pretty much always be 0 :-/
                            QFile f(fpath + "/removable");
                            bool removable = false;
                            if ( f.open(IO_ReadOnly) ) {
                                if(f.getch() == '1')
                                    removable = true;
                            }
                            
                            if(!removable && mntdir != "/") {
                                const FileSystem *fsi = storage.fileSystemOf(mntdir);
                                if(fsi)
                                    removable = fsi->isRemovable();
                            }

                            if(removable)
                                blockdevs.insert(fs, mntdir);
                            else
                                odebug << fs << " is not removable" << oendl;
                        }

                        break;
                    }
                }
            }
        }
        endmntent( mntfp );
    }

    if ( showPopup ) {
        QMap<QString,QString>::Iterator it;
        for( it = blockdevs.begin(); it != blockdevs.end(); ++it )
            if(!m_mounts.contains(it.key()))
                mountChanged( it.data(), true );

        for( it = m_mounts.begin(); it != m_mounts.end(); ++it )
            if(!blockdevs.contains(it.key()))
                mountChanged( it.data(), false );
    }
    m_mounts = blockdevs;
}

void CardApplet::findRootBlock()
{
    // Find the /sys/block entry that corresponds to the root block device
    m_rootdevnode = "";
    struct stat st_root;
    if(stat("/", &st_root) == 0) {
        m_rootdev = st_root.st_dev;
        unsigned int rootmaj = m_rootdev >> 8;
        unsigned int rootmin = m_rootdev & 0xff;

        QDir blockdir("/sys/block");
        blockdir.setFilter( QDir::Dirs );
        QString rootblock = "";
        for ( unsigned int i=0; i<blockdir.count(); i++ ) {
            QString blockname = blockdir[i];
            if(blockname[0] == '.') continue;
            QDir blocksubdir("/sys/block/" + blockname);
            blocksubdir.setFilter( QDir::Dirs );
            for ( unsigned int j=0; j<blocksubdir.count(); j++ ) {
                QString blocksubname = blocksubdir[j];
                if(blocksubname[0] == '.') continue;
                if(blocksubname.startsWith(blockname)) {
                    QFile f("/sys/block/" + blockname + "/" + blocksubname + "/dev");
                    if ( f.open(IO_ReadOnly) ) {
                        QString devaddr = "";
                        if(f.readLine(devaddr, 16) > 0 ) {
                            QStringList devparts = QStringList::split(':', devaddr);
                            if(devparts.count() == 2) {
                                if (devparts[0].toUInt() == rootmaj && devparts[1].toUInt() == rootmin) {
                                    rootblock = "/sys/block/" + blockname;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            if(rootblock != "") {
                QDir rootdevdir(rootblock + "/device");
                m_rootdevnode = rootdevdir.canonicalPath();
                break;
            }
        }
    }
    
    odebug << "CARD: root devnode = " << m_rootdevnode << oendl;
}

void CardApplet::mountChanged( const QString &device, bool mounted )
{
    QString text = QString::null;
    QString what = QString::null;
    if ( mounted ) {
        text += QObject::tr("%1 inserted").arg(device); // FIXME description
        what = "on";
    } else {
        text += QObject::tr("%1 removed").arg(device);
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
    if(m_process) {
        delete m_process;
        m_process = 0;
    }

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
        odebug << "process returned, code " << ret << oendl;

        if( ret != 0 ) {
            if ( m_commandOrig == CMD_UNMOUNT ) {
                m_ejectMode = EM_NONE;
                QMessageBox::warning(0,
                    tr("Remove failed"),
                    tr("<p>Unmounting the selected device failed:"
                       "<p>%1").arg( m_commandStderr ) );
            }
            else if ( m_commandOrig == CMD_FUSER ) {
                // fuser found no processes using the device
                // Note that if fuser failed and is returning non-zero here for some other reason
                // it doesn't really matter since the unmount will fail anyway
                
                // Move checked mount point to umount list
                m_umount += m_check[0];
                m_check.remove(m_check.begin());
                if(m_check.isEmpty()) {
                    // All checked, time to move onto unmounting
                    umountNextMount();
                }
                else
                    checkNextMount();
            }
            else if ( m_commandOrig == CMD_PSINFO ) {
                // Getting info on used processes failed, can't do much more than show a generic message
                QMessageBox::warning(0,
                    tr("Card in use"),
                    tr("<p>The device you are trying to remove is in use. Please check that no applications are using this device and then try again." ));
            }
        }
        else if ( m_commandOrig == CMD_FUSER ) {
            // fuser check returned a list of processes, get some info about them to show to the user
            m_ejectMode = EM_NONE;
            QString procs = m_commandStdout;
            getProcessInfo(procs);
        }
        else if ( m_commandOrig == CMD_PSINFO ) {
            QMessageBox::warning(0,
                tr("Card in use"),
                tr("<p>The device you are trying to remove is in use by the following applications:</p><pre>%1</pre><p>Please close these applications and then try again.").arg( m_commandStdout.stripWhiteSpace() ) );
        }
        else if ( m_commandOrig == CMD_UNMOUNT ) {
            // Unmount succeeded
            m_umount.remove(m_umount.begin());
            if(m_umount.isEmpty()) {
                if(m_ejectMode == EM_UNMOUNT && m_ejectSocket > -1 ) {
                    m_ejectMode = EM_EJECT;
                    userCardAction( (m_ejectSocket * 100) + EJECT );
                }
                else
                    m_ejectMode = EM_NONE;
            }
            else
                umountNextMount();
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
        QString sound = "";

        odebug << "cardapplet: user action on pcmcia socket " << socket << " requested. action = " << what << oendl;

        switch ( what )
        {
            case CONFIGURE:
            {
                QString insertAction; QString resumeAction; QString driver; QString conf;
                configure( OPcmciaSystem::instance()->socket( socket ) );
                return;
            }
            case EJECT:
                sound = "cardmon/cardoff";
                if(m_ejectMode == EM_EJECT) {
                    success = OPcmciaSystem::instance()->socket( socket )->eject();
                }
                else {
                    // Set up list of mount points to check
                    m_ejectMode = EM_UNMOUNT;
                    m_ejectSocket = socket;
                    QMap<QString,int>::Iterator it;
                    m_check.clear();
                    m_umount.clear();
                    for( it = m_cardmounts.begin(); it != m_cardmounts.end(); ++it ) {
                        if(it.data() == socket) {
                            if(it.key() == "/") {
                                QMessageBox::warning(0,
                                    tr("Card in use"),
                                    tr("<p>The system is running from the selected device - it cannot be removed.</p>" ));
                                return;
                            }
                            m_check += it.key();
                        }
                    }
                    if(m_check.isEmpty()) {
                        // This card has no associated mount points
                        success = OPcmciaSystem::instance()->socket( socket )->eject();
                    }
                    else {
                        checkNextMount();
                        return;
                    }
                }
                m_ejectMode = EM_NONE;
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
                        sound = "cardmon/cardon";
                        break;
            default:       odebug << "pcmcia: not yet implemented" << oendl;
        }

        if ( success )
        {
            #ifndef QT_NO_SOUND
            if( sound != "" )
                QSound::play( Resource::findSound( sound ) );
            #endif

            odebug << tr( "Successfully %1ed card in socket #%2" ).arg( actionText[what] ).arg( socket ) << oendl;
            popUp( tr( "Successfully %1ed card in socket #%2" ).arg( actionText[what] ).arg( socket ) );
        }
        else
        {
            odebug << tr( "Error while %1ing card in socket #%2" ).arg( actionText[what] ).arg( socket ) << oendl;
            popUp( tr( "Error while %1ing card in socket #%2" ).arg( actionText[what] ).arg( socket ) );
        }
    }
    else {
        // Other mount point action
        int mountpoint = (action / 1000) - 1;
        int what = action % 1000;

        odebug << "cardapplet: user action on mount point " << mountpoint << " requested. action = " << what << oendl;

        if( what == EJECT ) {
            if( mountpoint < m_mounts.count() ) {
                QMap<QString,QString>::Iterator it;
                int i = 0;
                for( it = m_mounts.begin(); it != m_mounts.end(); ++it ) {
                    if(i == mountpoint) {
                        m_ejectSocket = -1;
                        m_check.clear();
                        m_umount.clear();
                        m_check += it.data();
                        checkNextMount();
                        break;
                    }
                    i++;
                }
            }
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

void CardApplet::getProcessInfo( const QString &processes )
{
    QStringList cmd;
    QStringList procs = QStringList::split(QRegExp("[ \n]"), processes);
    cmd << "ps" << "-p" << procs.join(",") << "-o" << "comm,pid";
    m_commandOrig = CMD_PSINFO;
    execCommand( cmd );
}

bool CardApplet::isRoot ( const QString &mountpt )
{
    struct stat st_mountpt;
   
    odebug << "CARD: Checking if " << mountpt << " is the root" << oendl;
    if(stat(mountpt, &st_mountpt) == 0 && (m_rootdev != 0) )
        return ( st_mountpt.st_dev == m_rootdev );
    else
        return false;
}

void CardApplet::checkNextMount()
{
    if( isRoot(m_check[0]) ) {
        QMessageBox::warning(0,
            tr("Card in use"),
            tr("<p>The system is running from the selected device - it cannot be removed.</p>" ));
        return;
    }

    QStringList cmd;
    odebug << "Using fuser to check usage of " << m_check[0] << oendl;
    cmd << "fuser" << "-m" << m_check[0];
    m_commandOrig = CMD_FUSER;
    execCommand( cmd );
}

void CardApplet::umountNextMount()
{
    QStringList cmd;

    odebug << "Unmounting " << m_umount[0] << oendl;
    cmd << "umount" << m_umount[0];
    m_commandOrig = CMD_UNMOUNT;
    execCommand( cmd );
}

EXPORT_OPIE_APPLET_v1( CardApplet )

