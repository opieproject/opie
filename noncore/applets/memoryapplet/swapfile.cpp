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

#include "swapfile.h"

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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <qcopchannel_qws.h>
#include <qpe/resource.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/vfs.h>
#include <mntent.h>
#include <unistd.h>
#include <sys/types.h>

Swapfile::Swapfile( QWidget *parent, const char *name, WFlags f )
    : QWidget( parent, name, f )
{
	// are we running as root?
	isRoot = geteuid() == 0;

    QVBoxLayout* vb = new QVBoxLayout(this, 5);

	QHButtonGroup* cfsdRBG = new QHButtonGroup(tr("Swapfile location"), this);
	cfsdRBG->setRadioButtonExclusive(true);
    vb->addWidget(cfsdRBG);
	
	ramRB = new QRadioButton(tr("RAM"), cfsdRBG);
	cfRB = new QRadioButton(tr("CF Card"), cfsdRBG);
	sdRB = new QRadioButton(tr("SD Card"), cfsdRBG);

	QHBox *hb1 = new QHBox(this);
	hb1->setSpacing(5);

	swapPath1 = new QLineEdit(hb1);
	swapPath1->setEnabled(false);

	QPushButton* swapOn = new QPushButton(tr(" On "), hb1);
	QPushButton* swapOff = new QPushButton(tr(" Off "), hb1);
	vb->addWidget(hb1);

	QVGroupBox* box1 = new QVGroupBox(tr("Manage Swapfile"), this);
    vb->addWidget(box1);

	QHBox *hb2 = new QHBox(box1);
	hb2->setSpacing(5);
	QPushButton* mkSwap = new QPushButton(tr("Generate"), hb2);
	QPushButton* rmSwap = new QPushButton(tr("Remove"), hb2);

	QHBox *hb3 = new QHBox(box1);
	hb3->setSpacing(5);
	swapSize = new QComboBox(hb3);
	swapSize->insertStringList(QStringList::split(",", tr("2 Mb,4 Mb,6 Mb,8 Mb")));

	mkswapProgress = new QProgressBar(3, hb3);
	mkswapProgress->setCenterIndicator(true);

	QHBox *hb4 = new QHBox(this);
	hb4->setSpacing(5);

	swapStatusIcon = new QLabel(hb4);
	swapStatus = new QLabel(tr(""), hb4);
	hb4->setStretchFactor(swapStatus, 99);
    vb->addWidget(hb4);

    connect(swapOn, SIGNAL(clicked()), this, SLOT(swapon()));
    connect(swapOff, SIGNAL(clicked()), this, SLOT(swapoff()));
    connect(cfRB, SIGNAL(clicked()), this, SLOT(cfsdchecked()));
    connect(sdRB, SIGNAL(clicked()), this, SLOT(cfsdchecked()));
    connect(ramRB, SIGNAL(clicked()), this, SLOT(cfsdchecked()));
    connect(mkSwap, SIGNAL(clicked()), this, SLOT(makeswapfile()));
    connect(rmSwap, SIGNAL(clicked()), this, SLOT(removeswapfile()));

	cfRB->setEnabled(FALSE);
    sdRB->setEnabled(FALSE);
    
    QCopChannel *pcmciaChannel = new QCopChannel("QPE/Card", this);
    connect(pcmciaChannel, SIGNAL(received(const QCString &, const QByteArray &)), this, SLOT(cardnotify(const QCString &, const QByteArray &)));
    QCopChannel *sdChannel = new QCopChannel("QPE/Card", this);
    connect(sdChannel, SIGNAL(received(const QCString &, const QByteArray &)), this, SLOT(cardnotify(const QCString &, const QByteArray &)));
    
    cardInPcmcia0 = FALSE;
    cardInPcmcia1 = FALSE;
    cardInSd = FALSE;

    Swapfile::status();
    Swapfile::getStatusPcmcia();
    Swapfile::getStatusSd();
}

int Swapfile::exec(const QString& arg)
{
	return system((!isRoot ? "sudo " : "") + arg);
}


Swapfile::~Swapfile()
{
}

void Swapfile::cardnotify(const QCString & msg, const QByteArray &)
{
    if (msg == "stabChanged()") 
	{
        getStatusPcmcia();
    } 
	else if (msg == "mtabChanged()") 
	{
        getStatusSd();
	}
}

void Swapfile::getStatusPcmcia()
{

    bool cardWas0 = cardInPcmcia0;	// remember last state
    bool cardWas1 = cardInPcmcia1;

    QString fileName;

    // one of these 3 files should exist
    if (QFile::exists("/var/run/stab")) {
	fileName = "/var/run/stab";
    } else if (QFile::exists("/var/state/pcmcia/stab")) {
	fileName = "/var/state/pcmcia/stab";
    } else {
	fileName = "/var/lib/pcmcia/stab";
    }

    QFile f(fileName);

    if (f.open(IO_ReadOnly)) {
	QStringList list;
	QTextStream stream(&f);
	QString streamIn;
	streamIn = stream.read();
	list = QStringList::split("\n", streamIn);
	for (QStringList::Iterator line = list.begin(); line != list.end();
	     line++) {
	    if ((*line).startsWith("Socket 0:")) {
		if ((*line).startsWith("Socket 0: empty") && cardInPcmcia0) {
		    cardInPcmcia0 = FALSE;
		} else if (!(*line).startsWith("Socket 0: empty")
			   && !cardInPcmcia0) {
		    cardInPcmcia0 = TRUE;
		}
	    } else if ((*line).startsWith("Socket 1:")) {
		if ((*line).startsWith("Socket 1: empty") && cardInPcmcia1) {
		    cardInPcmcia1 = FALSE;
		} else if (!(*line).startsWith("Socket 1: empty")
			   && !cardInPcmcia1) {
		    cardInPcmcia1 = TRUE;
		}
	    }
	}
	f.close();

	if (cardWas0 != cardInPcmcia0 || cardWas1 != cardInPcmcia1) {
	    QString text = QString::null;
	    QString what = QString::null;
	    if (cardWas0 != cardInPcmcia0) {
		if (cardInPcmcia0) {
		   cfRB->setEnabled(TRUE); 
		} else {
    		   cfRB->setChecked(FALSE);
		   cfRB->setEnabled(FALSE); 
		}
	    }

	    if (cardWas1 != cardInPcmcia1) {
		if (cardInPcmcia1) {
		   cfRB->setEnabled(TRUE); 
		} else {
    		   cfRB->setChecked(FALSE);
		   cfRB->setEnabled(FALSE); 
		}
	    }
	}
    } else {
	// no file found
	qDebug("no file found");
	cardInPcmcia0 = FALSE;
	cardInPcmcia1 = FALSE;
    }
    Swapfile::cfsdchecked();
}


void Swapfile::getStatusSd()
{

    bool cardWas = cardInSd;	// remember last state
    cardInSd = FALSE;

#if defined(_OS_LINUX_) || defined(Q_OS_LINUX)
    struct mntent *me;
    FILE *mntfp = setmntent("/etc/mtab", "r");

    if (mntfp) {
	while ((me = getmntent(mntfp)) != 0) {
	    QString fs = me->mnt_fsname;
	    if (fs.left(14) == "/dev/mmc/part1" || fs.left(7) == "/dev/sd"
		|| fs.left(9) == "/dev/mmcd") {
		cardInSd = TRUE;
		show();
	    }
	}
	endmntent(mntfp);
    }

    if (cardWas != cardInSd) {
	QString text = QString::null;
	QString what = QString::null;
	if (cardInSd) {
	   sdRB->setEnabled(TRUE); 
	} else {
    	   sdRB->setChecked(FALSE);
	   sdRB->setEnabled(FALSE); 
	}
    }
#else
#error "Not on Linux"
#endif
	Swapfile::cfsdchecked();
}

int rc=0;

void Swapfile::swapon()
{
	char swapcmd[128] ="swapon ";
	Swapfile::cfsdchecked();
	strcat(swapcmd,swapPath1->text());
	char *runcmd = swapcmd;
	rc = exec(QString("%1").arg(runcmd));
	if (rc != 0) {
	       setStatusMessage("Failed to attach swapfile.", true);
        }
	else {
/*	       QMessageBox::information(this, "Information", "Swapfile is active!"); */
	       setStatusMessage("Swapfile activated.");
	}
       Swapfile::status();
}


void Swapfile::setStatusMessage(const QString& text, bool error /* = false */)
{
	swapStatus->setText("<b>" + text + "</b>");
	swapStatusIcon->setPixmap(Resource::loadPixmap(error ? "close" : "done"));
}


void Swapfile::swapoff()
{
	char swapcmd[128] ="swapoff ";
	if (Swapfile::cfRB->isChecked() == TRUE) 
	Swapfile::cfsdchecked();
	strcat(swapcmd,swapPath1->text());
	char *runcmd = swapcmd;
	rc = exec(QString("%1").arg(runcmd));
	if (rc != 0) {
	       setStatusMessage(tr("Failed to detach swapfile."), true);
        }
	else {
/*	       QMessageBox::information(this, "Information", "Swapfile is inactive!"); */
	       setStatusMessage(tr("Swapfile deactivated."));
/*       	       Swapfile::swapPath->clear();*/
	}
       Swapfile::status();
}

void Swapfile::cfsdchecked()
{
/*       	Swapfile::swapPath->clear();*/
       	Swapfile::swapPath1->clear();
	if (Swapfile::ramRB->isChecked() == TRUE) 
	{
		Swapfile::swapPath1->insert("/home/swapfile");
	}
	if  (Swapfile::sdRB->isChecked() == TRUE)
	{
       		Swapfile::swapPath1->insert("/mnt/card/swapfile");
	}
	if  (Swapfile::cfRB->isChecked() == TRUE)
	{
        	Swapfile::swapPath1->insert("/mnt/cf/swapfile");
	}
/*	Swapfile::swapPath->insert(Swapfile::swapPath1->text());*/
}

void Swapfile::makeswapfile()
{	
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
	}		
	if (rc != 0) {
		   setStatusMessage(tr("Failed to create swapfile."), true);
        }

	mkswapProgress->setProgress(2);	
	rc=exec(QString("mkswap %1").arg(swapPath1->text()));
	if (rc != 0) {
		   setStatusMessage(tr("Failed to initialize swapfile."), true);
        }
	mkswapProgress->setProgress(3);	
	mkswapProgress->reset();	
	setStatusMessage(tr("Swapfile created."));
}	

void Swapfile::removeswapfile()
{
	exec(QString("swapoff %1").arg(swapPath1->text()));
	rc=exec(QString("rm -rf %1").arg(swapPath1->text()));
	if (rc != 0) {
		   setStatusMessage(tr("Failed to remove swapfile."), true);
        }
	Swapfile::status();
	Swapfile::cfsdchecked();
	setStatusMessage(tr("Swapfile removed."));
}

void Swapfile::status()
{
	FILE *fp;
	char buffer[128], swapfile[128], temp[128];
	int swapsize=2000, i=1;

	fp=fopen("/proc/swaps", "r");
	while  ( (fgets(buffer,128,fp)) != NULL ) {
	       sscanf(buffer, "%s %s %d %s %s\n", swapfile, temp, &swapsize, temp, temp);
   	}
	fclose(fp);
    
	ramRB->setChecked(FALSE);
        cfRB->setChecked(FALSE);
        sdRB->setChecked(FALSE);

        i=strcmp(swapfile, "/home/swapfile");
        if ( i == 0 ) {
           ramRB->setChecked(TRUE); 
/*	   QMessageBox::information(this, "Information", "Swapfile is active!"); */
	   setStatusMessage(tr("Swapfile activated."));
	}
	i=strcmp(swapfile, "/usr/mnt.rom/cf/swapfile");
        if ( i == 0 ) {
           cfRB->setChecked(TRUE); 
/*	   QMessageBox::information(this, "Information", "Swapfile is active!"); */
	   setStatusMessage(tr("Swapfile activated."));
	}
	i=strcmp(swapfile, "/mnt/cf/swapfile");
        if ( i == 0 ) {
           cfRB->setChecked(TRUE); 
/*	   QMessageBox::information(this, "Information", "Swapfile is active!"); */
	   setStatusMessage(tr("Swapfile activated."));
	}
	i=strcmp(swapfile, "/usr/mnt.rom/card/swapfile");
        if ( i == 0 ) {
           sdRB->setChecked(TRUE); 
/*	   QMessageBox::information(this, "Information", "Swapfile is active!"); */
	   setStatusMessage(tr("Swapfile activated."));
	}
	i=strcmp(swapfile, "/mnt/card/swapfile");
        if ( i == 0 ) {
           sdRB->setChecked(TRUE); 
/*	   QMessageBox::information(this, "Information", "Swapfile is active!"); */
	   setStatusMessage(tr("Swapfile activated."));
	}

	Swapfile::cfsdchecked();
	    
        	
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
	}		
	

}


