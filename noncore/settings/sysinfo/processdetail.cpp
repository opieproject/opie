/**********************************************************************
** ProcessDetail
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

#include "processdetail.h"

#include <sys/types.h>
#include <signal.h>

#include <qcombobox.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtextview.h>
#include <qwhatsthis.h>

ProcessDetail::ProcessDetail( QWidget* parent,  const char* name, WFlags )
    : QWidget( parent, name, WStyle_ContextHelp )
{
    pid = 0;

    QGridLayout *layout = new QGridLayout( this );
    layout->setSpacing( 4 );
    layout->setMargin( 4 );

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

    ProcessView = new QTextView( this, "ProcessView" );
    layout->addMultiCellWidget( ProcessView, 0, 0, 0, 1 );
    QWhatsThis::add( ProcessView, tr( "This area shows detailed information about this process." ) );

    SendButton = new QPushButton( this, "SendButton" );
    SendButton->setMinimumSize( QSize( 50, 24 ) );
    SendButton->setMaximumSize( QSize( 50, 24 ) );
    SendButton->setText( tr( "Send" ) );
    connect( SendButton, SIGNAL( clicked() ), this, SLOT( slotSendClicked() ) );
    layout->addWidget( SendButton, 1, 1 );
    QWhatsThis::add( SendButton, tr( "Click here to send the selected signal to this process." ) );
}

ProcessDetail::~ProcessDetail()
{
}

void ProcessDetail::slotSendClicked()
{
    QString sigstr = SignalCB->currentText();
    sigstr.truncate(2);
    int sigid = sigstr.toUInt();

    if ( QMessageBox::warning( this, caption(),
                              tr( "You really want to send\n" + SignalCB->currentText() + "\nto this process?"),
                              QMessageBox::Yes | QMessageBox::Default, QMessageBox::No | QMessageBox::Escape )
           == QMessageBox::Yes )
    {
        if ( kill( pid, sigid ) == 0 )
        {
            hide();
        }
    }

}


