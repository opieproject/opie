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
#include <qpushbutton.h>
#include <qtextview.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qmessagebox.h>

ModulesDetail::ModulesDetail( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
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

    ModulesView = new QTextView( this, "ModulesView" );
    layout->addMultiCellWidget( ModulesView, 0, 0, 0, 1 );

    SendButton = new QPushButton( this, "SendButton" );
    SendButton->setMinimumSize( QSize( 50, 24 ) );
    SendButton->setMaximumSize( QSize( 50, 24 ) );
    SendButton->setText( tr( "Send" ) );
    connect( SendButton, SIGNAL( clicked() ), this, SLOT( slotSendClicked() ) );
    layout->addWidget( SendButton, 1, 1 );
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


