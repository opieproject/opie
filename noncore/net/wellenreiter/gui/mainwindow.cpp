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
#include "logwindow.h"
#include "mainwindow.h"
#include "wellenreiter.h"

#include "scanlist.h"

#include <qcombobox.h>
#include <qdatastream.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qiconset.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qstatusbar.h>
#include <qtextstream.h>
#include <qtoolbutton.h>

#ifdef QWS
#include <qpe/resource.h>
#include <opie/ofiledialog.h>
#else
#include "resource.h"
#include <qfiledialog.h>
#endif

WellenreiterMainWindow::WellenreiterMainWindow( QWidget * parent, const char * name, WFlags f )
           :QMainWindow( parent, name, f )
{
    cw = new WellenreiterConfigWindow( this );
    mw = new Wellenreiter( this );
    mw->setConfigWindow( cw );
    setCentralWidget( mw );

    // setup application icon

    #ifndef QWS
    setIcon( Resource::loadPixmap( "wellenreiter/appicon-trans" ) );
    setIconText( "Wellenreiter/X11" );
    #endif

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
    #ifdef QWS
    startStopButton->setAutoRaise( true );
    #endif
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
    #ifdef QWS
    c->setAutoRaise( true );
    #endif
    c->setIconSet( *infoIconSet );
    c->setEnabled( false );

    QToolButton* d = new QToolButton( 0 );
    #ifdef QWS
    d->setAutoRaise( true );
    #endif
    d->setIconSet( *settingsIconSet );
    connect( d, SIGNAL( clicked() ), this, SLOT( showConfigure() ) );

    // setup menu bar

    int id;

    QMenuBar* mb = menuBar();

    QPopupMenu* fileSave = new QPopupMenu( mb );
    fileSave->insertItem( "&Session...", this, SLOT( fileSaveSession() ) );
    fileSave->insertItem( "&Log...", this, SLOT( fileSaveLog() ) );

    QPopupMenu* fileLoad = new QPopupMenu( mb );
    fileLoad->insertItem( "&Session...", this, SLOT( fileLoadSession() ) );
    //fileLoad->insertItem( "&Log", this, SLOT( fileLoadLog() ) );

    QPopupMenu* file = new QPopupMenu( mb );
    id = file->insertItem( "&Load", fileLoad );
    file->insertItem( "&Save", fileSave );

    QPopupMenu* view = new QPopupMenu( mb );
    view->insertItem( "&Configure..." );

    QPopupMenu* sniffer = new QPopupMenu( mb );
    sniffer->insertItem( "&Configure..." );
    sniffer->insertSeparator();

    QPopupMenu* demo = new QPopupMenu( mb );
    demo->insertItem( "&Add something", this, SLOT( demoAddStations() ) );

    id = mb->insertItem( "&File", file );
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
    mw->netView()->addNewItem( "managed", "Vanille", "00:00:20:EF:A6:43", true, 6, 80 );
    mw->netView()->addNewItem( "managed", "Vanille", "00:30:6D:EF:A6:23", true, 11, 10 );
    mw->netView()->addNewItem( "adhoc", "ELAN", "00:A0:F8:E7:16:22", false, 3, 10 );
    mw->netView()->addNewItem( "adhoc", "ELAN", "00:AA:01:E7:56:62", false, 3, 15 );
    mw->netView()->addNewItem( "adhoc", "ELAN", "00:B0:8E:E7:56:E2", false, 3, 20 );
}

QString WellenreiterMainWindow::getFileName( bool save )
{
    QMap<QString, QStringList> map;
    map.insert( tr("All"), QStringList() );
    QStringList text;
    text << "text/*";
    map.insert(tr("Text"), text );
    text << "*";
    map.insert(tr("All"), text );

    QString str;
    if ( save )
    {
        #ifdef QWS
        str = OFileDialog::getSaveFileName( 2, "/", QString::null, map );
        #else
        str = QFileDialog::getSaveFileName();
        #endif
        if ( str.isEmpty() || QFileInfo(str).isDir() )
            return "";
    }
    else
    {
        #ifdef QWS
        str = OFileDialog::getOpenFileName( 2, "/", QString::null, map );
        #else
        str = QFileDialog::getOpenFileName();
        #endif
        if ( str.isEmpty() || !QFile(str).exists() || QFileInfo(str).isDir() )
            return "";
    }
    return str;
}

void WellenreiterMainWindow::fileSaveLog()
{
    QString fname = getFileName( true );
    if ( !fname.isEmpty() )
    {
        QFile f( fname );
        if ( f.open(IO_WriteOnly) )
        {
            QTextStream t( &f );
            t << mw->logWindow()->getLog();
            f.close();
            qDebug( "Saved log to file '%s'", (const char*) fname );
        }
        else
        {
            qDebug( "Problem saving log to file '%s'", (const char*) fname );
        }
    }
}

void WellenreiterMainWindow::fileSaveSession()
{
    QString fname = getFileName( true );
    if ( !fname.isEmpty() )
    {

        QFile f( fname );
        if ( f.open(IO_WriteOnly) )
        {
            QDataStream t( &f );
            t << *mw->netView();
            f.close();
            qDebug( "Saved session to file '%s'", (const char*) fname );
        }
        else
        {
            qDebug( "Problem saving session to file '%s'", (const char*) fname );
        }
    }
}

void WellenreiterMainWindow::fileLoadSession()
{
    QString fname = getFileName( false );
    if ( !fname.isEmpty() )
    {
        QFile f( fname );
        if ( f.open(IO_ReadOnly) )
        {
            QDataStream t( &f );
            t >> *mw->netView();
            f.close();
            qDebug( "Loaded session from file '%s'", (const char*) fname );
        }
        else
        {
            qDebug( "Problem loading session from file '%s'", (const char*) fname );
        }
    }
}

void WellenreiterMainWindow::closeEvent( QCloseEvent* e )
{
    if ( mw->isDaemonRunning() )
    {
        QMessageBox::warning( this, "Wellenreiter/Opie", "Sniffing in progress!\nPlease stop sniffing before closing." );
        e->ignore();
    }
    else
    {
        QMainWindow::closeEvent( e );
    }
}
