/**********************************************************************
** ModulesDetail
**
** Display module information
**
** Copyright (C) 2002, Michael Lauer
**                    mickey@tm.informatik.uni-frankfurt.de
**                    http://www.Vanille.de
**
** Based on ProcessDetail by Dan Williams <williamsdr@acm.org>
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

#include "modulesdetail.h"

#include <sys/types.h>
#include <stdio.h>

#include <qcombobox.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtextview.h>
#include <qwhatsthis.h>

ModulesDetail::ModulesDetail( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, WStyle_ContextHelp )
{
    modname = "";

    QGridLayout *layout = new QGridLayout( this );
    layout->setSpacing( 4 );
    layout->setMargin( 4 );

    CommandCB = new QComboBox( FALSE, this, "CommandCB" );
    CommandCB->insertItem( "modprobe -r" );
    CommandCB->insertItem( "rmmod" );
    // I can't think of other useful commands yet. Anyone?
    layout->addWidget( CommandCB, 1, 0 );
    QWhatsThis::add( CommandCB, tr( "Select a command here and then click the Send button to the right to send the command." ) );

    ModulesView = new QTextView( this, "ModulesView" );
    layout->addMultiCellWidget( ModulesView, 0, 0, 0, 1 );
    QWhatsThis::add( ModulesView, tr( "This area shows detailed information about this module." ) );

    SendButton = new QPushButton( this, "SendButton" );
    SendButton->setMinimumSize( QSize( 50, 24 ) );
    SendButton->setMaximumSize( QSize( 50, 24 ) );
    SendButton->setText( tr( "Send" ) );
    connect( SendButton, SIGNAL( clicked() ), this, SLOT( slotSendClicked() ) );
    layout->addWidget( SendButton, 1, 1 );
    QWhatsThis::add( SendButton, tr( "Click here to send the selected command to this module." ) );
}

ModulesDetail::~ModulesDetail()
{
}

void ModulesDetail::slotSendClicked()
{
    QString command = QString( "/sbin/" ) 
                    + CommandCB->currentText()
                    + QString( " " ) + modname;
    
    if ( QMessageBox::warning( this, caption(),
                              tr( "You really want to \n" + CommandCB->currentText() + "\nthis Module?"),
                              QMessageBox::Yes | QMessageBox::Default, QMessageBox::No | QMessageBox::Escape )
           == QMessageBox::Yes )
    {
        FILE* stream = popen( command, "r" );
        if ( stream )
            pclose( stream );
        {
            hide();
        }
    }

}


