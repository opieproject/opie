/**********************************************************************
** Copyright (C) Holger 'zecke' Freyther <freyther@kde.org>
** Copyright (C) Lorn Potter <llornkcor@handhelds.org>
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Opie Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include <qpe/storage.h>

#include <qcopchannel_qws.h>

#include <stdio.h>

#if defined(_OS_LINUX_) || defined(Q_OS_LINUX)
#include <sys/vfs.h>
#include <mntent.h>
#endif

#ifdef Q_OS_MACX
#  include <sys/param.h>
#  include <sys/ucred.h>
#  include <sys/mount.h>
#  include <stdio.h>  // For strerror()
#  include <errno.h>
#endif /* Q_OS_MACX */


#include <qdir.h>
#include <qmap.h>

static bool isUSB( const QString &devpath )
{
    bool matched = false;
    if( devpath.startsWith( "/dev/" ) ) {
        QDir diskdir("/dev/disk/by-id/");
        diskdir.setFilter( QDir::System );
        diskdir.setNameFilter( "usb-*" );
        const QFileInfoList *list = diskdir.entryInfoList();
        QFileInfoListIterator it( *list );
        QFileInfo *fi;

        for( ; (fi=it.current()); ++it ) {
            QString absdevpath = QDir::cleanDirPath(diskdir.absFilePath(fi->readLink()));
            if( absdevpath == devpath ) {
                matched = true;
                break;
            }
        }
    }
    return matched;
}

static bool isCF(const QString& m)
{
    bool matched = false;
    if(m.startsWith("/dev/")) {
        QString blockdev = m.mid(5);

        QDir pcmciadir("/sys/bus/pcmcia/devices");
        if( pcmciadir.exists() ) {
            pcmciadir.setFilter( QDir::Dirs );

            QMap<QString,QString> pcmciadevs;
            for ( unsigned int i=0; i<pcmciadir.count(); i++ ) {
                QString devid = pcmciadir[i];
                if(devid[0] == '.') continue;
                QDir pcmciadevdir("/sys/bus/pcmcia/devices/" + devid);
                // Now we need to split out the socket number
                devid = QStringList::split('.', devid)[0];
                pcmciadevs.insert( devid, pcmciadevdir.canonicalPath() );
            }

            if(!pcmciadevs.isEmpty()) {
                QDir blockdir("/sys/block");
                blockdir.setFilter( QDir::Dirs );

                for ( unsigned int i=0; i<blockdir.count(); i++ ) {
                    if(blockdir[i][0] == '.') continue;
                    QString fpath = "/sys/block/" + blockdir[i];
                    if ( QFile::exists( fpath + "/" + blockdev ) || fpath == blockdev ) {
                        // Check if mount point is associated with a PCMCIA device
                        bool matched = false;
                        if(!pcmciadevs.isEmpty()) {
                            QDir devdir(fpath + "/device");
                            QString devlink = devdir.canonicalPath();

                            QMap<QString,QString>::Iterator pit;
                            for( pit = pcmciadevs.begin(); pit != pcmciadevs.end(); ++pit ) {
                                if ( devlink.startsWith(pit.data()) ) {
                                    matched = true;
                                    break;
                                }
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
    return matched;
}

/*! \class StorageInfo storage.h
  \brief The StorageInfo class describes the disks mounted on the file system.

  This class provides access to the mount information for the Linux
  filesystem. Each mount point is represented by the FileSystem class.
  To ensure this class has the most up to date size information, call
  the update() method. Note that this will automatically be signaled
  by the operating system when a disk has been mounted or unmounted.

  \ingroup qtopiaemb
*/

/*! Constructor that determines the current mount points of the filesystem.
  The standard \a parent parameters is passed on to QObject.
 */
StorageInfo::StorageInfo( QObject *parent )
        : QObject( parent )
{
    mFileSystems.setAutoDelete( TRUE );
    channel = new QCopChannel( "QPE/Card", this );
    connect( channel, SIGNAL(received(const QCString&,const QByteArray&)),
             this, SLOT(cardMessage(const QCString&,const QByteArray&)) );
    update();
}

/*! Returns the longest matching FileSystem that starts with the
   same prefix as \a filename as its mount point.
*/
const FileSystem *StorageInfo::fileSystemOf( const QString &filename )
{
    const FileSystem *fs = 0;
    unsigned int maxlen = 0;

    for (QListIterator<FileSystem> i(mFileSystems); i.current(); ++i)
    {
        const QString &path = (*i)->path();
        if ( (path.length() > maxlen) && filename.startsWith( path ) ) {
            fs = (*i);
            maxlen = path.length();
        }
    }
    return fs;
}


void StorageInfo::cardMessage( const QCString& msg, const QByteArray& )
{
    if ( msg == "mtabChanged()" )
        update();
}


/*! Updates the mount and free space available information for each mount
  point. This method is automatically called when a disk is mounted or
  unmounted.
*/
// cause of the lack of a d pointer we need
// to store informations in a config file :(
void StorageInfo::update()
{
    //qDebug("StorageInfo::updating");
#if defined(_OS_LINUX_) || defined(Q_OS_LINUX)
    struct mntent *me;
    FILE *mntfp = setmntent( "/etc/mtab", "r" );

    QStringList curdisks;
    QStringList curopts;
    QStringList curfs;
    bool rebuild = FALSE;
    int n=0;
    if ( mntfp )
    {
        while ( (me = getmntent( mntfp )) != 0 )
        {
            QString fs = me->mnt_fsname;
            if ( fs.startsWith( "/dev/hd" ) || fs.startsWith( "/dev/sd" )
                    || fs.startsWith( "/dev/mtd" ) || fs.startsWith( "/dev/mmc" )
                    || fs == "/dev/root" )
            {
                n++;
                curdisks.append(fs);
                curopts.append( me->mnt_opts );
                //qDebug("-->fs %s opts %s", fs.latin1(), me->mnt_opts );
                curfs.append( me->mnt_dir );
                bool found = FALSE;
                for (QListIterator<FileSystem> i(mFileSystems); i.current(); ++i)
                {
                    if ( (*i)->disk() == fs )
                    {
                        found = TRUE;
                        break;
                    }
                }
                if ( !found )
                    rebuild = TRUE;
            }
        }
        endmntent( mntfp );
    }
    if ( rebuild || n != (int)mFileSystems.count() )
    {
        mFileSystems.clear();
        QStringList::ConstIterator it=curdisks.begin();
        QStringList::ConstIterator fsit=curfs.begin();
        QStringList::ConstIterator optsIt=curopts.begin();
        for (; it!=curdisks.end(); ++it, ++fsit, ++optsIt)
        {
            QString opts = *optsIt;

            QString disk = *it;
            QString humanname;
            bool removable = FALSE;
            if ( isCF(disk) )
            {
                humanname = tr("CF Card");
                removable = TRUE;
            }
            else if ( disk.startsWith( "/dev/hd" ) )
            {
                humanname = tr("IDE Disk");
            }
            else if ( disk.startsWith( "/dev/mmc" ) )
            {
                humanname = tr("SD/MMC Card");
                removable = TRUE;
            }
            else if ( disk.startsWith( "/dev/sd" ) ) {
                if( isUSB( disk ) ) {
                    humanname = tr("USB Disk") + " " + disk;
                    removable = TRUE;
                }
                else
                    humanname = tr("SCSI Disk") + " " + disk;
            }
            else if ( disk.startsWith( "/dev/mtd" ) ) 
                humanname = tr("Internal Storage") + " " + disk;
            else if ( disk == "/dev/root" )
                humanname = tr("Internal Storage") + " " + disk;
            else
                humanname = tr("Unknown") + " " + disk;
            FileSystem *fs = new FileSystem( disk, *fsit, humanname, removable, opts );
            mFileSystems.append( fs );
        }
        emit disksChanged();
    }
    else
    {
        // just update them
        for (QListIterator<FileSystem> i(mFileSystems); i.current(); ++i)
            i.current()->update();
    }
#endif
}

bool deviceTab( const char *device)
{
    QString name = device;
    bool hasDevice=false;

#ifdef Q_OS_MACX
    // Darwin (MacOS X)
    struct statfs** mntbufp;
    int count = 0;
    if ( ( count = getmntinfo( mntbufp, MNT_WAIT ) ) == 0 )
    {
        qWarning("deviceTab: Error in getmntinfo(): %s",strerror( errno ) );
        hasDevice = false;
    }
    for( int i = 0; i < count; i++ )
    {
        QString deviceName = mntbufp[i]->f_mntfromname;
        qDebug(deviceName);
        if( deviceName.left( name.length() ) == name )
            hasDevice = true;
    }
#else
    // Linux
    struct mntent *me;
    FILE *mntfp = setmntent( "/etc/mtab", "r" );
    if ( mntfp )
    {
        while ( (me = getmntent( mntfp )) != 0 )
        {
            QString deviceName = me->mnt_fsname;
            //          qDebug(deviceName);
            if( deviceName.left(name.length()) == name)
            {
                hasDevice = true;
            }
        }
    }
    endmntent( mntfp );
#endif /* Q_OS_MACX */


    return hasDevice;
}

/*!
 * @fn static bool StorageInfo::hasCf()
 * @brief returns whether device has Cf mounted
 *
 */
bool StorageInfo::hasCf()
{
    return deviceTab("/dev/hd");
}

/*!
 * @fn static bool StorageInfo::hasSd()
 * @brief returns whether device has SD mounted
 *
 */
bool StorageInfo::hasSd()
{
    return deviceTab("/dev/mmc");
}

/*!
 * @fn static bool StorageInfo::hasMmc()
 * @brief returns whether device has mmc mounted
 * Note that we simply consider MMC devices to be SD now since they are
 * largely indistinguishable to userspace, thus this function always
 * returns false
 */
bool StorageInfo::hasMmc()
{
    return false;
}

/*! \fn const QList<FileSystem> &StorageInfo::fileSystems() const
  Returns a list of all available mounted file systems.

  \warning This may change in Qtopia 3.x to return only relevant Qtopia file systems (and ignore mount points such as /tmp)
*/

/*! \fn void StorageInfo::disksChanged()
  Gets emitted when a disk has been mounted or unmounted, such as when
  a CF c
*/
//---------------------------------------------------------------------------

FileSystem::FileSystem( const QString &disk, const QString &path, const QString &name, bool rem, const QString &o )
        : fsdisk( disk ), fspath( path ), humanname( name ), blkSize(512), totalBlks(0), availBlks(0), removable( rem ), opts( o )
{
    update();
}

void FileSystem::update()
{
#if defined(_OS_LINUX_) || defined(Q_OS_LINUX)
    struct statfs fs;
    if ( !statfs( fspath.latin1(), &fs ) )
    {
        blkSize = fs.f_bsize;
        totalBlks = fs.f_blocks;
        availBlks = fs.f_bavail;
    }
    else
    {
        blkSize = 0;
        totalBlks = 0;
        availBlks = 0;
    }
#endif
}

/*! \class FileSystem storage.h
  \brief The FileSystem class describes a single mount point.

  This class simply returns information about a mount point, including
  file system name, mount point, human readable name, size information
  and mount options information.
  \ingroup qtopiaemb

  \sa StorageInfo
*/

/*! \fn const QString &FileSystem::disk() const
  Returns the file system name, such as /dev/hda3
*/

/*! \fn const QString &FileSystem::path() const
  Returns the mount path, such as /home
*/

/*! \fn const QString &FileSystem::name() const
  Returns the translated, human readable name for the mount directory.
*/

/*! \fn const QString &FileSystem::options() const
  Returns the mount options
*/

/*! \fn long FileSystem::blockSize() const
  Returns the size of each block on the file system.
*/

/*! \fn long FileSystem::totalBlocks() const
  Returns the total number of blocks on the file system
*/

/*! \fn long FileSystem::availBlocks() const
  Returns the number of available blocks on the file system
 */

/*! \fn bool FileSystem::isRemovable() const
  Returns flag whether the file system can be removed, such as a CF card
  would be removable, but the internal memory wouldn't
*/

/*! \fn bool FileSystem::isWritable() const
  Returns flag whether the file system is mounted as writable or read-only.
  Returns FALSE if read-only, TRUE if read and write.
*/

/*! \fn QStringList StorageInfo::fileSystemNames() const
  Returns a list of filesystem names.
*/
