/**********************************************************************
** ModulesInfo
**
** Display Modules information
**
** Copyright (C) 2002, Michael Lauer
**                    mickey@tm.informatik.uni-frankfurt.de
**                    http://www.Vanille.de
**
** Based on ProcessInfo by Dan Williams <williamsdr@acm.org>
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

#include <qcombobox.h>
#include <qfile.h>
#include <qheader.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qtimer.h>
#include <qwhatsthis.h>

#include "modulesinfo.h"

ModulesInfo::ModulesInfo( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    QGridLayout *layout = new QGridLayout( this );
    layout->setSpacing( 4 );
    layout->setMargin( 4 );

    ModulesView = new QListView( this );
    int colnum = ModulesView->addColumn( tr( "Module" ) );
    colnum = ModulesView->addColumn( tr( "Size" ) );
    ModulesView->setColumnAlignment( colnum, Qt::AlignRight );
    colnum = ModulesView->addColumn( tr( "Use#" ) );
    ModulesView->setColumnAlignment( colnum, Qt::AlignRight );
    colnum = ModulesView->addColumn( tr( "Used By" ) );
    ModulesView->setAllColumnsShowFocus( TRUE );
    layout->addMultiCellWidget( ModulesView, 0, 0, 0, 1 );
    QWhatsThis::add( ModulesView, tr( "This is a list of all the kernel modules currently loaded on this handheld device.\n\nClick and hold on a module to see additional information about the module, or to unload it." ) );

    CommandCB = new QComboBox( FALSE, this );
    CommandCB->insertItem( "modprobe -r" );
    CommandCB->insertItem( "rmmod" );
    // I can't think of other useful commands yet. Anyone?
    layout->addWidget( CommandCB, 1, 0 );
    QWhatsThis::add( CommandCB, tr( "Select a command here and then click the Send button to the right to send the command to module selected above." ) );

    QPushButton *btn = new QPushButton( this );
    btn->setMinimumSize( QSize( 50, 24 ) );
    btn->setMaximumSize( QSize( 50, 24 ) );
    btn->setText( tr( "Send" ) );
    connect( btn, SIGNAL( clicked() ), this, SLOT( slotSendClicked() ) );
    layout->addWidget( btn, 1, 1 );
    QWhatsThis::add( btn, tr( "Click here to send the selected command to the module selected above." ) );

    QTimer *t = new QTimer( this );
    connect( t, SIGNAL( timeout() ), this, SLOT( updateData() ) );
    t->start( 5000 );

    updateData();
}

ModulesInfo::~ModulesInfo()
{
}

void ModulesInfo::updateData()
{
    char modname[64];
    char usage[200];
    int modsize, usecount;

    ModulesView->clear();

    FILE *procfile = fopen( ( QString ) ( "/proc/modules"), "r");

    if ( procfile )
    {
        while ( true ) {
            int success = fscanf( procfile, "%s%d%d%[^\n]", modname, &modsize, &usecount, usage );

            if ( success == EOF )
                break;

            QString qmodname = QString( modname );
            QString qmodsize = QString::number( modsize ).rightJustify( 6, ' ' );
            QString qusecount = QString::number( usecount ).rightJustify( 2, ' ' );
            QString qusage = QString( usage );

            ( void ) new QListViewItem( ModulesView, qmodname, qmodsize, qusecount, qusage );
        }

        fclose( procfile );
    }
}

void ModulesInfo::slotSendClicked()
{
	QString capstr = tr( "You really want to execute\n" );
	capstr.append( CommandCB->currentText() );
	capstr.append( "\nfor this module?" );

    if ( QMessageBox::warning( this, caption(), capstr,
         QMessageBox::Yes | QMessageBox::Default, QMessageBox::No | QMessageBox::Escape ) == QMessageBox::Yes )
    {
		QString command = "/sbin/";
		command.append( CommandCB->currentText() );
		command.append( " " );
		command.append( ModulesView->currentItem()->text( 0 ) );

        FILE* stream = popen( command, "r" );
        if ( stream )
            pclose( stream );
        //{
          //  hide();
        //}
    }

}
