/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
** Copyright (C) 2002-2005 the Opie Team <opie-devel@handhelds.org>
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

#include "swapfile.h"

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/oresource.h>
using namespace Opie::Core;

#include <qpe/storage.h>

/* QT */
#include <qfile.h>
#include <qtextstream.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qhbuttongroup.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qprogressbar.h>
#include <qcombobox.h>
#include <qvgroupbox.h>
#include <qhbox.h>
#include <qmessagebox.h>
#include <qcopchannel_qws.h>

/* STD */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/vfs.h>
#include <mntent.h>
#include <unistd.h>
#include <sys/types.h>

Swapfile::Swapfile( QWidget *parent, const char *name, WFlags f )
    : QWidget( parent, name, f )
{
    if ( !QFile::exists( "/proc/swaps" ) ) {
        new QLabel( tr( "Swap disabled in kernel" ), this );
        return;
    }
    // are we running as root?
    isRoot = geteuid() == 0;

    QVBoxLayout* vb = new QVBoxLayout(this, 5);

    QHButtonGroup* cfsdRBG = new QHButtonGroup(tr("Swapfile location"), this);
    cfsdRBG->setRadioButtonExclusive(true);
    vb->addWidget(cfsdRBG);

    swapLocation = new QComboBox( cfsdRBG );

    QHBox *hb1 = new QHBox(this);
    hb1->setSpacing(5);

    swapPath1 = new QLineEdit(hb1);
    swapPath1->setEnabled(false);

    swapOn = new QPushButton(tr(" On "), hb1);
    swapOff = new QPushButton(tr(" Off "), hb1);
    vb->addWidget(hb1);

    QVGroupBox* box1 = new QVGroupBox(tr("Manage Swapfile"), this);
    vb->addWidget(box1);

    QHBox *hb2 = new QHBox(box1);
    hb2->setSpacing(5);
    mkSwap = new QPushButton(tr("Generate"), hb2);
    rmSwap = new QPushButton(tr("Remove"), hb2);

    QHBox *hb3 = new QHBox(box1);
    hb3->setSpacing(5);
    swapSize = new QComboBox(hb3);
    swapSize->insertStringList(QStringList::split(",", tr("2 Mb,4 Mb,6 Mb,8 Mb,16 Mb,32 Mb,64 Mb")));

    mkswapProgress = new QProgressBar(3, hb3);
    mkswapProgress->setCenterIndicator(true);

    QHBox *hb4 = new QHBox(this);
    hb4->setSpacing(5);

    swapStatusIcon = new QLabel(hb4);
    swapStatus = new QLabel("", hb4);
    hb4->setStretchFactor(swapStatus, 99);
    vb->addWidget(hb4);

    connect(swapOn, SIGNAL(clicked()), this, SLOT(swapon()));
    connect(swapOff, SIGNAL(clicked()), this, SLOT(swapoff()));
    connect(swapLocation, SIGNAL(activated(int)), this, SLOT(locationSelected(int)));
    connect(mkSwap, SIGNAL(clicked()), this, SLOT(makeswapfile()));
    connect(rmSwap, SIGNAL(clicked()), this, SLOT(removeswapfile()));

    QCopChannel *sdChannel = new QCopChannel("QPE/Card", this);
    connect(sdChannel, SIGNAL(received(const QCString&,const QByteArray&)), this, SLOT(cardnotify(const QCString&,const QByteArray&)));

    Swapfile::getMounts();
}

int Swapfile::exec(const QString& arg)
{
    return system((!isRoot ? "sudo " : "") + arg);
}


void Swapfile::cardnotify(const QCString & msg, const QByteArray &)
{
    if (msg == "mtabChanged()")
        getMounts();
}


void Swapfile::getMounts()
{

#if defined(_OS_LINUX_) || defined(Q_OS_LINUX)
    struct mntent *me;
    FILE *mntfp = setmntent("/etc/mtab", "r");

    swapLocation->clear();
    swapLocation->insertItem( tr("(None)") );
    StorageInfo storage;

    if (mntfp) {
        while ((me = getmntent(mntfp)) != 0) {
            QString mntdir = QFile::decodeName( me->mnt_dir );
            if( ! (strcmp(me->mnt_type, "tmpfs") == 0
                || strcmp(me->mnt_type, "ramfs") == 0
                || mntdir.startsWith("/dev" )
                || mntdir.startsWith("/sys" )
                || mntdir.startsWith("/proc" )
                )) {
                const FileSystem *fsi = storage.fileSystemOf(mntdir);
                if(fsi)
                    swapLocation->insertItem( mntdir );
            }
        }
        endmntent(mntfp);
    }

#else
#error "Not on Linux"
#endif
    status();
}

int rc=0;

void Swapfile::swapon()
{
    if( QFile::exists( swapPath1->text() ) ) {
        if( checkSwapFileActive( swapPath1->text() ) ) {
            // Don't make this an error, nothing's really wrong...
            setStatusMessage(tr("Swapfile already active."));
            return;
        }
        
        char swapcmd[128] ="swapon ";
        strncat(swapcmd,swapPath1->text(), 120);
        char *runcmd = swapcmd;
        rc = exec(QString("%1").arg(runcmd));
        if (rc != 0)
            setStatusMessage("Failed to attach swapfile.", true);
        else
            setStatusMessage("Swapfile activated.");
        status();
    }
    else 
        setStatusMessage("Swapfile not found", true);
}


void Swapfile::setStatusMessage(const QString& text, bool error /* = false */)
{
    swapStatus->setText("<b>" + text + "</b>");
    swapStatusIcon->setPixmap(Opie::Core::OResource::loadPixmap(error ? "close" : "done", Opie::Core::OResource::SmallIcon));
}


void Swapfile::swapoff()
{
    if( !checkSwapFileActive( swapPath1->text() ) ) {
        setStatusMessage(tr("Swapfile not active."), true);
        return;
    }
    
    char swapcmd[128] ="swapoff ";
    strncat(swapcmd,swapPath1->text(), 120);
    char *runcmd = swapcmd;
    rc = exec(QString("%1").arg(runcmd));
    if (rc != 0)
        setStatusMessage(tr("Failed to detach swapfile."), true);
    else
        setStatusMessage(tr("Swapfile deactivated."));
    Swapfile::status();
}

void Swapfile::locationSelected(int)
{
    if( swapLocation->currentItem() > 0 ) {
        QString path = swapLocation->currentText();
        if( path.right(1) != "/" )
            path += "/";
        path += "swapfile";
        swapPath1->setText( path );
        swapOn->setEnabled( true );
        swapOff->setEnabled( true );
        rmSwap->setEnabled( true );
        mkSwap->setEnabled( true );
    }
    else {
        swapPath1->clear();
        swapOn->setEnabled( false );
        swapOff->setEnabled( false );
        rmSwap->setEnabled( false );
        mkSwap->setEnabled( false );
    }
}

bool Swapfile::checkSwapFileActive( const QString &path )
{
    if( QFile::exists( path ) ) {
        FILE *fp;
        char buffer[128], swapfile[128], temp[128];
        bool found = false;
        fp=fopen("/proc/swaps", "r");
        while  ( (fgets(buffer,128,fp)) != NULL ) {
            sscanf(buffer, "%s %s %s %s %s\n", swapfile, temp, temp, temp, temp);
            if( strcmp(swapfile, path) == 0 ) {
                found = true;
                break;
            }
        }
        fclose(fp);
        return found;
    }
    else
        return false;
}

void Swapfile::makeswapfile()
{
    if( checkSwapFileActive( swapPath1->text() ) ) {
        setStatusMessage(tr("Swap file currently active!"), true);
        return;
    }

    int i = swapSize->currentItem();

    mkswapProgress->setProgress(1);
    switch ( i ) {
        case 0: rc=exec(QString("dd if=/dev/zero of=%1 bs=1k count=2048").arg(swapPath1->text()));
        break;
        case 1: rc=exec(QString("dd if=/dev/zero of=%1 bs=1k count=4096").arg(swapPath1->text()));
        break;
        case 2: rc=exec(QString("dd if=/dev/zero of=%1 bs=1k count=6144").arg(swapPath1->text()));
        break;
        case 3: rc=exec(QString("dd if=/dev/zero of=%1 bs=1k count=8192").arg(swapPath1->text()));
        break;
        case 4: rc=exec(QString("dd if=/dev/zero of=%1 bs=1k count=16384").arg(swapPath1->text()));
        break;
        case 5: rc=exec(QString("dd if=/dev/zero of=%1 bs=1k count=32768").arg(swapPath1->text()));
        break;
        case 6: rc=exec(QString("dd if=/dev/zero of=%1 bs=1k count=65536").arg(swapPath1->text()));
        break;
    }
    if (rc != 0)
        setStatusMessage(tr("Failed to create swapfile."), true);

    mkswapProgress->setProgress(2);
    rc = exec(QString("mkswap %1").arg(swapPath1->text()));
    if (rc != 0)
        setStatusMessage(tr("Failed to initialize swapfile."), true);

    mkswapProgress->setProgress(3);
    mkswapProgress->reset();
    setStatusMessage(tr("Swapfile created."));
}

void Swapfile::removeswapfile()
{
    exec(QString("swapoff %1").arg(swapPath1->text()));
    rc = exec(QString("rm -f %1").arg(swapPath1->text()));
    if (rc != 0)
        setStatusMessage(tr("Failed to remove swapfile."), true);

    Swapfile::status();
    setStatusMessage(tr("Swapfile removed."));
}

void Swapfile::status()
{
    FILE *fp;
    char buffer[128], swapfile[128], swaptype[128], temp[128];
    int swapsize=2000;

    bool found = false;
    fp=fopen("/proc/swaps", "r");
    while  ( (fgets(buffer,128,fp)) != NULL ) {
        sscanf(buffer, "%s %s %d %s %s\n", swapfile, swaptype, &swapsize, temp, temp);
        if( strcmp(swaptype, "file") == 0 ) {
            found = true;
            break;
        }
    }

    fclose(fp);

    if( found ) {
        QString swapfilestr(swapfile);
        QString swaptypestr(swaptype);
        QString foundfs;
        for( int i=1; i<swapLocation->count(); i++ ) {
            QString fs = swapLocation->text(i);
            if( swapfilestr.startsWith( fs ) && fs.length() >= foundfs.length() ) {
                swapLocation->setCurrentItem(i);
                foundfs = fs;
            }
        }
    }

    locationSelected(-1);

    swapsize /=1000;

    switch ( swapsize ) {
        case 2: swapSize->setCurrentItem(0);
        break;
        case 4: swapSize->setCurrentItem(1);
        break;
        case 6: swapSize->setCurrentItem(2);
        break;
        case 8: swapSize->setCurrentItem(3);
        break;
        case 16: swapSize->setCurrentItem(4);
        break;
        case 32: swapSize->setCurrentItem(5);
        break;
        case 64: swapSize->setCurrentItem(6);
        break;
    }
}
