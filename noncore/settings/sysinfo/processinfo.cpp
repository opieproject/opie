/**********************************************************************
** ProcessInfo
**
** Display process information
**
** Copyright (C) 2002, Dan Williams
**                    williamsdr@acm.org
**                    http://draknor.net
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

#include <qpe/qpeapplication.h>

#include <qdir.h>
#include <qfile.h>
#include <qheader.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qmessagebox.h>
#include <qtimer.h>
#include <qwhatsthis.h>

#include <sys/types.h>
#include <signal.h>

#include "processinfo.h"
#include "detail.h"

ProcessInfo::ProcessInfo( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    QGridLayout *layout = new QGridLayout( this );
    layout->setSpacing( 4 );
    layout->setMargin( 4 );


    ProcessView = new QListView( this, "ProcessView" );
    int colnum = ProcessView->addColumn( tr( "PID" ) );
    ProcessView->setColumnAlignment( colnum, Qt::AlignRight );
    colnum = ProcessView->addColumn( tr( "Command" ),96 );
    colnum = ProcessView->addColumn( tr( "Status" ) );
    colnum = ProcessView->addColumn( tr( "Time" ) );
    ProcessView->setColumnAlignment( colnum, Qt::AlignRight );
    ProcessView->setAllColumnsShowFocus( TRUE );
    QPEApplication::setStylusOperation( ProcessView->viewport(), QPEApplication::RightOnHold );
    connect( ProcessView, SIGNAL( rightButtonPressed( QListViewItem *, const QPoint &, int ) ),
            this, SLOT( viewProcess( QListViewItem * ) ) );
    layout->addMultiCellWidget( ProcessView, 0, 0, 0, 1 );
    QWhatsThis::add( ProcessView, tr( "This is a list of all the processes on this handheld device.\n\nClick and hold on a process to see additional information about the process, or to send a signal to it." ) );
    
	SignalCB = new QComboBox( FALSE, this, "SignalCB" );
    SignalCB->insertItem( " 1: SIGHUP" );
    SignalCB->insertItem( " 2: SIGINT" );
    SignalCB->insertItem( " 3: SIGQUIT" );
    SignalCB->insertItem( " 5: SIGTRAP" );
    SignalCB->insertItem( " 6: SIGABRT" );
    SignalCB->insertItem( " 9: SIGKILL" );
    SignalCB->insertItem( "14: SIGALRM" );
    SignalCB->insertItem( "15: SIGTERM" );
    SignalCB->insertItem( "18: SIGCONT" );
    SignalCB->insertItem( "19: SIGSTOP" );
    layout->addWidget( SignalCB, 1, 0 );
    QWhatsThis::add( SignalCB, tr( "Select a signal here and then click the Send button to the right to send to this process." ) );

    SendButton = new QPushButton( this, "SendButton" );
    SendButton->setMinimumSize( QSize( 50, 24 ) );
    SendButton->setMaximumSize( QSize( 50, 24 ) );
    SendButton->setText( tr( "Send" ) );
    connect( SendButton, SIGNAL( clicked() ), this, SLOT( slotSendClicked() ) );
    layout->addWidget( SendButton, 1, 1 );
    QWhatsThis::add( SendButton, tr( "Click here to send the selected signal to this process." ) );

    QTimer *t = new QTimer( this );
    connect( t, SIGNAL( timeout() ), this, SLOT( updateData() ) );
    t->start( 5000 );

    updateData();

    ProcessDtl = new Detail();
    QWhatsThis::add( ProcessDtl->detailView, tr( "This area shows detailed information about this process." ) );
}

ProcessInfo::~ProcessInfo()
{
}

void ProcessInfo::updateData()
{
    int pid, ppid, pgrp, session, tty, tpgid, utime, stime, cutime, cstime, counter, priority, starttime,
       signal, blocked, sigignore, sigcatch;
    uint flags, minflt, cminflt, majflt, cmajflt, timeout, itrealvalue, vsize, rss, rlim, startcode,
         endcode, startstack, kstkesp, kstkeip, wchan;
    char state;
    char comm[64];

    ProcessView->clear();

    QDir *procdir = new QDir("/proc", 0, QDir::Name, QDir::Dirs);
    QFileInfoList *proclist = new QFileInfoList(*(procdir->entryInfoList()));
    if ( proclist )
    {
        QFileInfoListIterator it(*proclist);
        QFileInfo *f;
        while ( ( f = it.current() ) != 0 )
        {
            ++it;
            QString processnum = f->fileName();
            if ( processnum >= "1" && processnum <= "99999" )
            {
                FILE *procfile = fopen( ( QString ) ( "/proc/" + processnum + "/stat"), "r");

                if ( procfile )
                {
                    fscanf( procfile,
                           "%d %s %c %d %d %d %d %d %u %u %u %u %u %d %d %d %d %d %d %u %u %d %u %u %u %u %u %u %u %u %d %d %d %d %u",
                           &pid, comm, &state, &ppid, &pgrp, &session,&tty, &tpgid, &flags, &minflt, &cminflt,
                           &majflt, &cmajflt, &utime, &stime, &cutime, &cstime, &counter, &priority, &timeout,
                           &itrealvalue, &starttime, &vsize, &rss, &rlim, &startcode, &endcode, &startstack,
                           &kstkesp, &kstkeip, &signal, &blocked, &sigignore, &sigcatch, &wchan );
                    processnum = processnum.rightJustify( 5, ' ' );
                    QString processcmd = QString( comm ).replace( QRegExp( "[()]" ), "" );
                    QString processstatus = QChar(state);
                    QString processtime = QString::number( ( utime + stime ) / 100 );
                    processtime = processtime.rightJustify( 9, ' ' );
                    fclose( procfile );

                    ( void ) new QListViewItem( ProcessView, processnum, processcmd, processstatus, processtime );
                }
            }
        }
    }

    delete proclist;
    delete procdir;
}

void ProcessInfo::slotSendClicked()
{
	QListViewItem *currprocess = ProcessView->currentItem();
	if ( !currprocess )
	{
		return;
	}
	
	QString capstr = tr( "You really want to send\n" );
	capstr.append( SignalCB->currentText() );
	capstr.append( "\nto this process?" );

	
    if ( QMessageBox::warning( this, currprocess->text( 1 ), capstr,
         QMessageBox::Yes | QMessageBox::Default, QMessageBox::No | QMessageBox::Escape ) == QMessageBox::Yes )
	{
		QString sigstr = SignalCB->currentText();
		sigstr.truncate(2);
		int sigid = sigstr.toUInt();
        if ( kill( currprocess->text( 0 ).stripWhiteSpace().toUInt(), sigid ) == 0 )
        {
            hide();
        }
    }

}

void ProcessInfo::viewProcess( QListViewItem *process )
{
    QString pid= process->text( 0 ).stripWhiteSpace();
    QString command = process->text( 1 );
    ProcessDtl->setCaption( pid + " - " + command );
    FILE *statfile = fopen( ( QString ) ( "/proc/" + pid + "/status"), "r");
    if ( statfile )
    {
        char line[81];
        fgets( line, 81, statfile );
        ProcessDtl->detailView->setText( line );
        while ( fgets( line, 81, statfile ) )
        {
            ProcessDtl->detailView->append( line );
        }
        fclose( statfile );
    }
    ProcessDtl->showMaximized();
}
