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

#include "scanlist.h"

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

    infoIconSet = new QIconSet( Resource::loadPixmap( "wellenreiter/InfoIcon" ) );
    settingsIconSet = new QIconSet( Resource::loadPixmap( "wellenreiter/SettingsIcon" ) );
    #ifdef QWS
    searchIconSet = new QIconSet( Resource::loadPixmap( "wellenreiter/SearchIcon" ) );
    cancelIconSet = new QIconSet( Resource::loadPixmap( "wellenreiter/CancelIcon" ) );
    #else
    startStopIconSet = new QIconSet();
    startStopIconSet->setPixmap( Resource::loadPixmap( "wellenreiter/SearchIcon" ), QIconSet::Automatic, QIconSet::Normal, QIconSet::Off );
    startStopIconSet->setPixmap( Resource::loadPixmap( "wellenreiter/CancelIcon" ), QIconSet::Automatic, QIconSet::Normal, QIconSet::On );
    #endif

    // setup tool buttons

    startStopButton = new QToolButton( 0 );
    startStopButton->setAutoRaise( true );
    #ifdef QWS
    startStopButton->setOnIconSet( *cancelIconSet );
    startStopButton->setOffIconSet( *searchIconSet );
    #else
    startStopButton->setIconSet( *startStopIconSet );
    #endif
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
    file->insertItem( "&Load..." );
    file->insertItem( "&Save..." );

    QPopupMenu* view = new QPopupMenu( mb );
    view->insertItem( "&Configure..." );

    QPopupMenu* sniffer = new QPopupMenu( mb );
    sniffer->insertItem( "&Configure..." );
    sniffer->insertSeparator();

    QPopupMenu* demo = new QPopupMenu( mb );
    demo->insertItem( "&Add something", this, SLOT( demoAddStations() ) );

    int id;
    id = mb->insertItem( "&File", file );
    mb->setItemEnabled( id, false );
    id = mb->insertItem( "&View", view );
    mb->setItemEnabled( id, false );
    id = mb->insertItem( "&Sniffer", sniffer );
    mb->setItemEnabled( id, false );
    id = mb->insertItem( "&Demo", demo );
    mb->setItemEnabled( id, true );

    #ifdef QWS
    mb->insertItem( startStopButton );
    mb->insertItem( c );
    mb->insertItem( d );
    #else // Qt3 changed the insertion order. It's now totally random :(
    mb->insertItem( d );
    mb->insertItem( c );
    mb->insertItem( startStopButton );
    #endif

    // setup status bar (for now only on X11)

    #ifndef QWS
    statusBar()->message( "Ready." );
    #endif

};

void WellenreiterMainWindow::showConfigure()
{
    qDebug( "show configure..." );
    cw->setCaption( tr( "Configure" ) );
    #ifdef QWS
    cw->showMaximized();
    #endif
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

    delete infoIconSet;
    delete settingsIconSet;
    #ifdef QWS
    delete searchIconSet;
    delete cancelIconSet;
    #else
    delete startStopIconSet;
    #endif

};

void WellenreiterMainWindow::demoAddStations()
{
    mw->netView()->addNewItem( "managed", "Vanille", "04:00:20:EF:A6:43", true, 6, 80 );
    mw->netView()->addNewItem( "managed", "Vanille", "04:00:20:EF:A6:23", true, 11, 10 );
    mw->netView()->addNewItem( "adhoc", "ELAN", "40:03:43:E7:16:22", false, 3, 10 );
    mw->netView()->addNewItem( "adhoc", "ELAN", "40:03:53:E7:56:62", false, 3, 15 );
    mw->netView()->addNewItem( "adhoc", "ELAN", "40:03:63:E7:56:E2", false, 3, 20 );
}

