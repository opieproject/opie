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

#include <qfile.h>
#include <qheader.h>
#include <qlayout.h>
#include <qlistview.h>
#include <qtimer.h>
#include <qwhatsthis.h>

#include "modulesinfo.h"

ModulesInfo::ModulesInfo( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    QVBoxLayout *layout = new QVBoxLayout( this, 5 );

    ModulesView = new QListView( this, "ModulesView" );
    int colnum = ModulesView->addColumn( tr( "Module" ) );
    colnum = ModulesView->addColumn( tr( "Size" ) );
    ModulesView->setColumnAlignment( colnum, Qt::AlignRight );
    colnum = ModulesView->addColumn( tr( "Use#" ) );
    ModulesView->setColumnAlignment( colnum, Qt::AlignRight );
    colnum = ModulesView->addColumn( tr( "Used By" ) );
    ModulesView->setAllColumnsShowFocus( TRUE );
    QPEApplication::setStylusOperation( ModulesView->viewport(), QPEApplication::RightOnHold );
    connect( ModulesView, SIGNAL( rightButtonPressed( QListViewItem *, const QPoint &, int ) ),
            this, SLOT( viewModules( QListViewItem * ) ) );
    layout->addWidget( ModulesView );
    QWhatsThis::add( ModulesView, tr( "This is a list of all the kernel modules currently loaded on this handheld device.\n\nClick and hold on a module to see additional information about the module, or to unload it." ) );

    QTimer *t = new QTimer( this );
    connect( t, SIGNAL( timeout() ), this, SLOT( updateData() ) );
    t->start( 5000 );

    updateData();
    
    ModulesDtl = new ModulesDetail( 0, 0, 0 );
    ModulesDtl->ModulesView->setTextFormat( PlainText );
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

void ModulesInfo::viewModules( QListViewItem *modules )
{
    QString modname = modules->text( 0 );   
    ModulesDtl->setCaption( QString( "Module: " ) + modname );
    ModulesDtl->modname = modname;
    QString command = QString( "/sbin/modinfo " ) + modules->text( 0 );
    
    FILE* modinfo = popen( command, "r" );
    
    if ( modinfo )
    {
        char line[200];
        ModulesDtl->ModulesView->setText( " Details:\n------------\n" );
        
        while( true )
        {        
            int success = fscanf( modinfo, "%[^\n]\n", line );
            if ( success == EOF )
                break;         
            ModulesDtl->ModulesView->append( line );
        }
        
        pclose( modinfo );
    }

    ModulesDtl->showMaximized();
}
