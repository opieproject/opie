/**********************************************************************
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
// additions copyright 2002 by L.J. Potter
#include <qlabel.h>
#include <qlayout.h>
#include <qtimer.h>
#include <qlayout.h>
#include "graph.h"
#include "storage.h"

#include <stdio.h>
 #if defined(_OS_LINUX_) || defined(Q_OS_LINUX)
#include <sys/vfs.h>
#include <mntent.h>
 #endif

StorageInfo::StorageInfo( QWidget *parent, const char *name )
    : QWidget( parent, name )
{
    vb = 0;
    disks.setAutoDelete(TRUE);
    lines.setAutoDelete(TRUE);
    updateMounts();
    startTimer( 5000 );
}

void StorageInfo::timerEvent(QTimerEvent*)
{
    updateMounts();
}

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
            if ( sscanf(line,"%*d %s %*s %*s %s", devtype, devname )==2 ) {
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

void StorageInfo::updateMounts()
{
#if defined(_OS_LINUX_) || defined(Q_OS_LINUX)
    struct mntent *me;
    FILE *mntfp = setmntent( "/etc/mtab", "r" );
    QStringList curdisks;
    QStringList curfs;
    QStringList mountList;
    QStringList fsT;
    bool rebuild = FALSE;
    int n=0;
    if ( mntfp ) {
        while ( (me = getmntent( mntfp )) != 0 ) {
            QString fs = me->mnt_fsname;
            qDebug(fs+" "+(QString)me->mnt_type);
            if ( fs.left(7)=="/dev/hd" || fs.left(7)=="/dev/sd"
                 || fs.left(8)=="/dev/mtd" || fs.left(9) == "/dev/mmcd"
                 || fs.left(9) == "/dev/root" || fs.left(5) == "/ramfs") {
                n++;
                curdisks.append(fs);
                QString d = me->mnt_dir;
                curfs.append(d);
                QString mount =  me->mnt_dir;
                mountList.append(mount);
                QString t = me->mnt_type;
                fsT.append(t);
                if ( !disks.find(d) )
                    rebuild = TRUE;
            }
        }
        endmntent( mntfp );
    }
    if ( rebuild || n != (int)disks.count() ) {
        disks.clear();
        lines.clear();
        delete vb;
        vb = new QVBoxLayout( this, n > 3 ? 1 : 5 );
        bool frst=TRUE;
        QStringList::ConstIterator it=curdisks.begin();
        QStringList::ConstIterator fsit=curfs.begin();
        QStringList::ConstIterator fsmount=mountList.begin();
        QStringList::ConstIterator fsTit=fsT.begin();

    for (; it!=curdisks.end(); ++it, ++fsit) {
            if ( !frst ) {
                QFrame *f = new QFrame( this );
                vb->addWidget(f);
                f->setFrameStyle( QFrame::HLine | QFrame::Sunken );
                lines.append(f);
                f->show();
            } frst=FALSE;
            QString humanname=*it;
//            qDebug(humanname);
            if ( isCF(humanname) )
                humanname = tr("CF Card: "+*fsmount+" "+*fsTit+" ");
            else if ( humanname == "/dev/hda1" )
                humanname = tr("Hard Disk "+*fsmount+" "+*fsTit+" ");
            else if ( humanname.left(9) == "/dev/mmcd" )
                humanname = tr("SD Card "+*fsmount+" "+*fsTit+" ");
            else if ( humanname.left(7) == "/dev/hd" )
                humanname = tr("Hard Disk") + " " + humanname.mid(7)+" "+*fsmount+" "+*fsTit+" ";
            else if ( humanname.left(7) == "/dev/sd" )
                humanname = tr("SCSI Hard Disk") + " " + humanname.mid(7)+" "+*fsmount+" "+*fsTit+" ";
            else if ( humanname == "/dev/mtdblock1" || humanname == "/dev/mtdblock/1" )
                humanname = tr("Int. Storage "+*fsmount+" "+*fsTit+"\n");
            else if ( humanname.left(14) == "/dev/mtdblock/" )
                humanname = tr("Int. Storage") + " " + humanname.mid(14)+" "+*fsmount+" "+*fsTit+" ";
            else if ( humanname.left(13) == "/dev/mtdblock" )
                humanname = tr("Int. Storage") + " " + humanname.mid(13)+" "+*fsmount+" "+*fsTit+" ";
             else if ( humanname.left(9) == "/dev/root" )
                 humanname = tr("Int. Storage "+*fsmount+" "+*fsTit+" ");
              // etc.
            MountInfo* mi = new MountInfo( *fsit, humanname, this );
            vb->addWidget(mi);
            disks.insert(*fsit,mi);
            mi->show();
            fsmount++;fsTit++;
        }
        vb->addStretch();
    } else {
          // just update them
        for (QDictIterator<MountInfo> i(disks); i.current(); ++i)
            i.current()->updateData();
    }
#endif
}


MountInfo::MountInfo( const QString &path, const QString &ttl, QWidget *parent, const char *name )
    : QWidget( parent, name ), title(ttl)
{
    qDebug("new path is "+path);
    fs = new FileSystem( path );
    QVBoxLayout *vb = new QVBoxLayout( this, 3 );

    totalSize = new QLabel( this );
    vb->addWidget( totalSize );

    data = new GraphData();
    graph = new BarGraph( this );
    graph->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    vb->addWidget( graph, 1 );
    graph->setData( data );

    legend = new GraphLegend( this );
    legend->setOrientation(Horizontal);
    vb->addWidget( legend );
    legend->setData( data );

    updateData();
}

MountInfo::~MountInfo()
{
    delete data;
    delete fs;
}

void MountInfo::updateData()
{
    fs->update();

    long mult = fs->blockSize() / 1024;
    long div = 1024 / fs->blockSize();
    if ( !mult ) mult = 1;
    if ( !div ) div = 1;
    long total = fs->totalBlocks() * mult / div;
    long avail = fs->availBlocks() * mult / div;
    long used = total - avail;
    totalSize->setText( title + tr(" : %1 kB").arg( total ) );
    data->clear();
    data->addItem( tr("Used (%1 kB)").arg(used), used );
    data->addItem( tr("Available (%1 kB)").arg(avail), avail );
    graph->repaint( FALSE );
    legend->update();
    graph->show();
    legend->show();
}

//---------------------------------------------------------------------------

FileSystem::FileSystem( const QString &p )
    : fspath( p ), blkSize(512), totalBlks(0), availBlks(0)
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

