/**********************************************************************
** Copyright (C) 2002 Michael 'Mickey' Lauer.  All rights reserved.
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
**********************************************************************/

#include "configwindow.h"
#include "mainwindow.h"
#include "wellenreiter.h"

#include <qcombobox.h>
#include <qiconset.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qstatusbar.h>
#include <qtoolbutton.h>

#ifdef QWS
#include <qpe/resource.h>
#else
#include "resource.h"
#endif

WellenreiterMainWindow::WellenreiterMainWindow( QWidget * parent, const char * name, WFlags f )
           :QMainWindow( parent, name, f )
{
    cw = new WellenreiterConfigWindow( this );
    mw = new Wellenreiter( this );
    mw->setConfigWindow( cw );
    setCentralWidget( mw );

    // setup icon sets

    searchIconSet = new QIconSet( Resource::loadPixmap( "wellenreiter/SearchIcon" ) );
    infoIconSet = new QIconSet( Resource::loadPixmap( "wellenreiter/InfoIcon" ) );
    settingsIconSet = new QIconSet( Resource::loadPixmap( "wellenreiter/SettingsIcon" ) );
    cancelIconSet = new QIconSet( Resource::loadPixmap( "wellenreiter/CancelIcon" ) );

    // setup tool buttons

    startStopButton = new QToolButton( 0 );
    startStopButton->setAutoRaise( true );
    startStopButton->setOnIconSet( *cancelIconSet );
    startStopButton->setOffIconSet( *searchIconSet );
    startStopButton->setToggleButton( true );
    connect( startStopButton, SIGNAL( clicked() ), mw, SLOT( startStopClicked() ) );
    startStopButton->setEnabled( false );

    QToolButton* c = new QToolButton( 0 );
    c->setAutoRaise( true );
    c->setIconSet( *infoIconSet );
    c->setEnabled( false );

    QToolButton* d = new QToolButton( 0 );
    d->setAutoRaise( true );
    d->setIconSet( *settingsIconSet );
    connect( d, SIGNAL( clicked() ), this, SLOT( showConfigure() ) );

    // setup menu bar

    QMenuBar* mb = menuBar();

    QPopupMenu* file = new QPopupMenu( mb );
    file->insertItem( "&Load" );
    file->insertItem( "&Save" );

    QPopupMenu* view = new QPopupMenu( mb );
    view->insertItem( "&Configure" );

    QPopupMenu* sniffer = new QPopupMenu( mb );
    sniffer->insertItem( "&Configure" );
    sniffer->insertSeparator();

    int id;

    id = mb->insertItem( "&File", file );
    mb->setItemEnabled( id, false );
    id = mb->insertItem( "&View", view );
    mb->setItemEnabled( id, false );
    id = mb->insertItem( "&Sniffer", sniffer );
    mb->setItemEnabled( id, false );

    mb->insertItem( startStopButton );
    mb->insertItem( c );
    mb->insertItem( d );

    // setup status bar (for now only on X11)

    #ifndef QWS
    statusBar()->message( "Ready." );
    #endif

};

void WellenreiterMainWindow::showConfigure()
{
    qDebug( "show configure..." );
    cw->setCaption( tr( "Configure" ) );
    cw->showMaximized();
    int result = cw->exec();

    if ( result )
    {
        // check configuration from config window

        const QString& interface = cw->interfaceName->currentText();
        const int cardtype = cw->daemonDeviceType();
        const int interval = cw->daemonHopInterval();

        if ( ( interface != "<select>" ) && ( cardtype != 0 ) )
            startStopButton->setEnabled( true );
            //TODO ...
        else
            startStopButton->setEnabled( false );
            //TODO ...
    }
}

WellenreiterMainWindow::~WellenreiterMainWindow()
{

    delete searchIconSet;
    delete infoIconSet;
    delete settingsIconSet;
    delete cancelIconSet;


};



