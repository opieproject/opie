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
#include <qpushbutton.h>
#include <qtextview.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qmessagebox.h>

ProcessDetail::ProcessDetail( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    pid = 0;

    QGridLayout *layout = new QGridLayout( this );
    layout->setSpacing( 6 );
    layout->setMargin( 11 );

    SignalCB = new QComboBox( FALSE, this, "SignalCB" );
    SignalCB->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)7, SignalCB->sizePolicy().hasHeightForWidth() ) );
    SignalCB->setMinimumSize( QSize( 50, 24 ) );
    SignalCB->setMaximumSize( QSize( 600, 24 ) );
    SignalCB->insertItem( " 1: SIGHUP" );
    SignalCB->insertItem( " 9: SIGKILL" );
    SignalCB->insertItem( "15: SIGTERM" );
    layout->addWidget( SignalCB, 1, 0 );

    ProcessView = new QTextView( this, "ProcessView" );
    ProcessView->setFrameShadow( QTextView::Plain );
    layout->addMultiCellWidget( ProcessView, 0, 0, 0, 1 );

    SendButton = new QPushButton( this, "SendButton" );
    SendButton->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, SendButton->sizePolicy().hasHeightForWidth() ) );
    SendButton->setMinimumSize( QSize( 50, 24 ) );
    SendButton->setMaximumSize( QSize( 50, 24 ) );
    SendButton->setText( tr( "Send" ) );
    connect( SendButton, SIGNAL( clicked() ), this, SLOT( slotSendClicked() ) );
    layout->addWidget( SendButton, 1, 1 );
}

ProcessDetail::~ProcessDetail()
{
}

void ProcessDetail::slotSendClicked()
{
    QString sigstr = SignalCB->currentText();
    sigstr.truncate(2);
    int sigid = sigstr.toUInt();

    if ( !QMessageBox::information( this, caption(),
                                   ( tr( "You really want to send\n" + SignalCB->currentText() + "\nto this process?") ),
                                   ( tr( "Yes" ) ), ( tr( "No" ) ), 0 ) )
    {
        if ( kill( pid, sigid ) == 0 )
        {
            accept();
        }
    }

}


