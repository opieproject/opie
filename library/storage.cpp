/**********************************************************************
** Copyright (C) Holger 'zecke' Freyther <freyther@kde.org>
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
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
#ifdef QT_QWS_CUSTOM
#include <qpe/custom.h>
#endif

#include <qfile.h>
#include <qtimer.h>
#include <qcopchannel_qws.h>

#include <stdio.h>

#if defined(_OS_LINUX_) || defined(Q_OS_LINUX)
#include <sys/vfs.h>
#include <mntent.h>
#endif

#include <qstringlist.h>

static bool isCF(const QString& m)
{
    FILE* f = fopen("/var/run/stab", "r");
    if (!f) f = fopen("/var/state/pcmcia/stab", "r");
    if (!f) f = fopen("/var/lib/pcmcia/stab", "r");
    if ( f ) {
        char line[1024];
        char devtype[80];
        char devname[80];
        while ( fgets( line, 1024, f ) ) {
              // 0       ide     ide-cs  0       hda     3       0
            if ( sscanf(line,"%*d %s %*s %*s %s", devtype, devname )==2 )
              {
                  if ( QString(devtype) == "ide" && m.find(devname)>0 ) {
                      fclose(f);
                      return TRUE;
                  }
              }
        }
        fclose(f);
    }
    return FALSE;
}

StorageInfo::StorageInfo( QObject *parent )
    : QObject( parent )
{
    mFileSystems.setAutoDelete( TRUE );
    channel = new QCopChannel( "QPE/Card", this );
    connect( channel, SIGNAL(received(const QCString &, const QByteArray &)),
       this, SLOT(cardMessage( const QCString &, const QByteArray &)) );
    update();
}

const FileSystem *StorageInfo::fileSystemOf( const QString &filename )
{
    for (QListIterator<FileSystem> i(mFileSystems); i.current(); ++i) {
  if ( filename.startsWith( (*i)->path() ) )
       return (*i);
    }
    return 0;
}


void StorageInfo::cardMessage( const QCString& msg, const QByteArray& )
{
    if ( msg == "mtabChanged()" )
  update();
}
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
    if ( mntfp ) {
  while ( (me = getmntent( mntfp )) != 0 ) {
      QString fs = me->mnt_fsname;
      if ( fs.left(7)=="/dev/hd" || fs.left(7)=="/dev/sd"
        || fs.left(8)=="/dev/mtd" || fs.left(9) == "/dev/mmcd"
        || fs.left(5)=="tmpfs" )
      {
    n++;
    curdisks.append(fs);
    curopts.append( me->mnt_opts );
    //qDebug("-->fs %s opts %s", fs.latin1(), me->mnt_opts );
    curfs.append( me->mnt_dir );
    bool found = FALSE;
    for (QListIterator<FileSystem> i(mFileSystems); i.current(); ++i) {
        if ( (*i)->disk() == fs ) {
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
    if ( rebuild || n != (int)mFileSystems.count() ) {
  mFileSystems.clear();
  QStringList::ConstIterator it=curdisks.begin();
  QStringList::ConstIterator fsit=curfs.begin();
  QStringList::ConstIterator optsIt=curopts.begin();
  for (; it!=curdisks.end(); ++it, ++fsit, ++optsIt) {
      QString opts = *optsIt;
  
      QString disk = *it; 
      QString humanname;
      bool removable = FALSE;
      if ( isCF(disk) ) {
    humanname = tr("CF Card");
    removable = TRUE;
      } else if ( disk == "/dev/hda1" ) {
    humanname = tr("Hard Disk");
      } else if ( disk.left(9) == "/dev/mmcd" ) {
    humanname = tr("SD Card");
    removable = TRUE;
      } else if ( disk.left(7) == "/dev/hd" )
    humanname = tr("Hard Disk") + " " + disk;
      else if ( disk.left(7) == "/dev/sd" )
    humanname = tr("SCSI Hard Disk") + " " + disk;
      else if ( disk.left(14) == "/dev/mtdblock6" ) //openzaurus ramfs
    humanname = tr("Internal Memory");
      else if ( disk == "/dev/mtdblock1" || humanname == "/dev/mtdblock/1" )
    humanname = tr("Internal Storage");
      else if ( disk.left(14) == "/dev/mtdblock/" )
    humanname = tr("Internal Storage") + " " + disk;
      else if ( disk.left(13) == "/dev/mtdblock" )
    humanname = tr("Internal Storage") + " " + disk;
      else if ( disk.left(5) == "tmpfs" ) //ipaqs /mnt/ramfs
    humanname = tr("Internal Memory");
      FileSystem *fs = new FileSystem( disk, *fsit, humanname, removable, opts );
      mFileSystems.append( fs );
  }
  emit disksChanged();
    } else {
  // just update them
  for (QListIterator<FileSystem> i(mFileSystems); i.current(); ++i)
      i.current()->update();
    }
#endif
}

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
    if ( !statfs( fspath.latin1(), &fs ) ) {
  blkSize = fs.f_bsize;
  totalBlks = fs.f_blocks;
  availBlks = fs.f_bavail;
    } else {
  blkSize = 0;
  totalBlks = 0;
  availBlks = 0;
    }
#endif
}

