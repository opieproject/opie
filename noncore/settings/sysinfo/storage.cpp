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

#include <qpe/storage.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qscrollview.h>
#include <qtimer.h>
#include <qwhatsthis.h>

#include "graph.h"
#include "storage.h"

#include <stdio.h>
 #if defined(_OS_LINUX_) || defined(Q_OS_LINUX)
#include <sys/vfs.h>
#include <mntent.h>
 #endif

FileSysInfo::FileSysInfo( QWidget *parent, const char *name )
    : QWidget( parent, name )
{
    QVBoxLayout *tmpvb = new QVBoxLayout( this );
    QScrollView *sv = new QScrollView( this );
    tmpvb->addWidget( sv, 0, 0 );
    sv->setResizePolicy( QScrollView::AutoOneFit );
    sv->setFrameStyle( QFrame::NoFrame );
    container = new QWidget( sv->viewport() );
    sv->addChild( container );
    vb = 0x0;

    storage = new StorageInfo( this );
    connect( storage, SIGNAL( disksChanged() ), this, SLOT( disksChanged() ) );
  
    lines.setAutoDelete(TRUE);

    rebuildDisks = TRUE;
    updateMounts();
    startTimer( 5000 );
}

void FileSysInfo::timerEvent(QTimerEvent*)
{
    updateMounts();
}

void FileSysInfo::updateMounts()
{
    storage->update();
    
    if ( rebuildDisks )
    {
        // Cannot auto delete QDict<MountInfo> disks because it seems to delete
        // the filesystem object as well causing a segfault
        MountInfo *mi; 
        for ( QDictIterator<MountInfo> delit(disks); delit.current(); ++delit )
        {
            mi = delit.current();
            mi->fs = 0x0;
            delete mi;
        }
        disks.clear();
        lines.clear();
        
        delete vb;
        vb = new QVBoxLayout( container/*, n > 3 ? 1 : 5*/ );

        bool frst=TRUE;
    
        FileSystem *fs;
        for ( QListIterator<FileSystem> it(storage->fileSystems()); it.current(); ++it )
        {
            fs = it.current();
        
            if ( !frst )
            {
                QFrame *f = new QFrame( container );
                vb->addWidget(f);
                f->setFrameStyle( QFrame::HLine | QFrame::Sunken );
                lines.append(f);
                f->show();
            }
            frst = FALSE;
           
            MountInfo *mi = new MountInfo( fs, container );
            vb->addWidget( mi );
            disks.insert( fs->path(), mi );
            mi->show();
            QString tempstr = fs->name().left( 2 );
            if ( tempstr == tr( "CF" ) )
                QWhatsThis::add( mi, tr( "This graph represents how much memory is currently used on this Compact Flash memory card." ) );
            else if ( tempstr == tr( "Ha" ) )
                QWhatsThis::add( mi, tr( "This graph represents how much storage is currently used on this hard drive." ) );
            else if ( tempstr == tr( "SD" ) )
                QWhatsThis::add( mi, tr( "This graph represents how much memory is currently used on this Secure Digital memory card." ) );
            else if ( tempstr == tr( "SC" ) )
                QWhatsThis::add( mi, tr( "This graph represents how much storage is currently used on this hard drive." ) );
            else if ( tempstr == tr( "In" ) )
                QWhatsThis::add( mi, tr( "This graph represents how much memory is currently used of the built-in memory (i.e. Flash memory) on this handheld device." ) );
            else if ( tempstr == tr( "RA" ) )
                QWhatsThis::add( mi, tr( "This graph represents how much memory is currently used of the temporary RAM disk." ) );
        }
        vb->addStretch();
    }
    else
    {
        for (QDictIterator<MountInfo> i(disks); i.current(); ++i)
            i.current()->updateData();
    }
    
    rebuildDisks = FALSE;
}

void FileSysInfo::disksChanged()
{
    rebuildDisks = TRUE;
}

MountInfo::MountInfo( FileSystem *filesys, QWidget *parent, const char *name )
    : QWidget( parent, name )
{
    QVBoxLayout *vb = new QVBoxLayout( this, 3 );

    totalSize = new QLabel( this );
    vb->addWidget( totalSize );

    fs = filesys;
    title = fs->name();
    
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

