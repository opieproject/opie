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

#include "mainwindow.h"
#include "wellenreiter.h"

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
    mw = new Wellenreiter( this );
    setCentralWidget( mw );

    // setup icon sets

    searchIconSet = new QIconSet( Resource::loadPixmap( "wellenreiter/SearchIcon" ) );
    infoIconSet = new QIconSet( Resource::loadPixmap( "wellenreiter/InfoIcon" ) );
    settingsIconSet = new QIconSet( Resource::loadPixmap( "wellenreiter/SettingsIcon" ) );
    cancelIconSet = new QIconSet( Resource::loadPixmap( "wellenreiter/CancelIcon" ) );

    // setup tool buttons

    QToolButton* b = new QToolButton( 0 );
    b->setAutoRaise( true );
    b->setOnIconSet( *cancelIconSet );
    b->setOffIconSet( *searchIconSet );
    b->setToggleButton( true );

    QToolButton* c = new QToolButton( 0 );
    c->setAutoRaise( true );
    c->setIconSet( *infoIconSet );
    c->setEnabled( false );

    QToolButton* d = new QToolButton( 0 );
    d->setAutoRaise( true );
    d->setIconSet( *settingsIconSet );

    // setup menu bar

    QMenuBar* mb = menuBar();

    QPopupMenu* p = new QPopupMenu( mb );
    p->insertItem( "&Load" );
    p->insertItem( "&Save" );

    mb->insertItem( "&File", p );
    mb->setItemEnabled( mb->insertItem( b ), false );
    mb->setItemEnabled( mb->insertItem( c ), false );
    mb->insertItem( d );

    // setup status bar

    // statusBar()->message( "Ready." );

};

WellenreiterMainWindow::~WellenreiterMainWindow()
{

    delete searchIconSet;
    delete infoIconSet;
    delete settingsIconSet;
    delete cancelIconSet;


};



