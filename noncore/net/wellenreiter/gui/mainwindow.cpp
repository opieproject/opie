/**********************************************************************
** Copyright (C) 2002-2004 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Wellenreiter II.
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
#include "gps.h"
#include "logwindow.h"
#include "packetview.h"
#include "mainwindow.h"
#include "wellenreiter.h"
#include "scanlist.h"

/* OPIE */
#ifdef QWS
#include <qpe/resource.h>
#include <opie2/odebug.h>
#include <opie2/ofiledialog.h>
#else
#include "resource.h"
#include <qapplication.h>
#include <qfiledialog.h>
#endif
using namespace Opie::Core;
using namespace Opie::Net;
using namespace Opie::Ui;

/* QT */
#include <qcombobox.h>
#include <qdatastream.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qiconset.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qstatusbar.h>
#include <qspinbox.h>
#include <qtextstream.h>
#include <qtoolbutton.h>
#include <qwhatsthis.h>

/* STD */
#include <unistd.h>

WellenreiterMainWindow::WellenreiterMainWindow( QWidget * parent, const char * name, WFlags f )
           :QMainWindow( parent, name, f )
{
    cw = new WellenreiterConfigWindow( this );
    mw = new Wellenreiter( this );
    mw->setConfigWindow( cw );
    setCentralWidget( mw );

    // setup application icon

    setIcon( Resource::loadPixmap( "wellenreiter/appicon-trans" ) );
    #ifndef QWS
    setIconText( "Wellenreiter/X11" );
    #endif

    // setup tool buttons

    startButton = new QToolButton( 0 );
    QWhatsThis::add( startButton, tr( "Click here to start scanning." ) );
    #ifdef QWS
    startButton->setAutoRaise( true );
    #endif
    startButton->setIconSet( Resource::loadIconSet( "wellenreiter/SearchIcon" ) );
    startButton->setEnabled( false );
    connect( startButton, SIGNAL( clicked() ), mw, SLOT( startClicked() ) );

    stopButton = new QToolButton( 0 );
    QWhatsThis::add( stopButton, tr( "Click here to stop scanning." ) );
    #ifdef QWS
    stopButton->setAutoRaise( true );
    #endif
    stopButton->setIconSet( Resource::loadIconSet( "wellenreiter/CancelIcon" ) );
    stopButton->setEnabled( false );
    connect( stopButton, SIGNAL( clicked() ), mw, SLOT( stopClicked() ) );

    QToolButton* d = new QToolButton( 0 );
    QWhatsThis::add( d, tr( "Click here to open the configure dialog." ) ),
    #ifdef QWS
    d->setAutoRaise( true );
    #endif
    d->setIconSet( Resource::loadIconSet( "wellenreiter/SettingsIcon" ) );
    connect( d, SIGNAL( clicked() ), this, SLOT( showConfigure() ) );

    uploadButton = new QToolButton( 0 );
    QWhatsThis::add( uploadButton, tr( "Click here to upload a capture session." ) );
    #ifdef QWS
    uploadButton->setAutoRaise( true );
    #endif
    uploadButton->setIconSet( Resource::loadIconSet( "up" ) );
    uploadButton->setEnabled( false );
    //uploadButton->setEnabled( true ); // DEBUGGING
    connect( uploadButton, SIGNAL( clicked() ), this, SLOT( uploadSession() ) );

    // setup menu bar

    int id;

    QMenuBar* mb = menuBar();

    QPopupMenu* fileSave = new QPopupMenu( mb );
    fileSave->insertItem( tr( "&Session..." ), this, SLOT( fileSaveSession() ) );
    fileSave->insertItem( tr( "&Text Log..." ), this, SLOT( fileSaveLog() ) );
    fileSave->insertItem( tr( "&Hex Log..." ), this, SLOT( fileSaveHex() ) );

    QPopupMenu* fileLoad = new QPopupMenu( mb );
    fileLoad->insertItem( tr( "&Session..." ), this, SLOT( fileLoadSession() ) );
    //fileLoad->insertItem( "&Log", this, SLOT( fileLoadLog() ) );

    QPopupMenu* file = new QPopupMenu( mb );
    file->insertItem( tr( "&New" ), this, SLOT( fileNew() ) );
    id = file->insertItem( tr( "&Load" ), fileLoad );
    file->insertItem( tr( "&Save" ), fileSave );
    file->insertSeparator();
    uploadID = file->insertItem( tr( "&Upload Session" ), this, SLOT( uploadSession() ) );
    file->insertSeparator();
    file->insertItem( tr( "&Exit" ), qApp, SLOT( quit() ) );

    QPopupMenu* sniffer = new QPopupMenu( mb );
    sniffer->insertItem( tr( "&Configure..." ), this, SLOT( showConfigure() ) );
    sniffer->insertSeparator();
    startID = sniffer->insertItem( tr( "&Start" ), mw, SLOT( startClicked() ) );
    sniffer->setItemEnabled( startID, false );
    stopID = sniffer->insertItem( tr( "Sto&p" ), mw, SLOT( stopClicked() ) );
    sniffer->setItemEnabled( stopID, false );

    QPopupMenu* view = new QPopupMenu( mb );
    view->insertItem( tr( "&Expand All" ), this, SLOT( viewExpandAll() ) );
    view->insertItem( tr( "&Collapse All" ), this, SLOT( viewCollapseAll() ) );

    QPopupMenu* demo = new QPopupMenu( mb );
    demo->insertItem( tr( "&Add something" ), this, SLOT( demoAddStations() ) );
    //demo->insertItem( tr( "&Read from GPSd" ), this, SLOT( demoReadFromGps() ) );

    id = mb->insertItem( tr( "&File" ), file );

    id = mb->insertItem( tr( "&View" ), view );
    //mb->setItemEnabled( id, false );

    id = mb->insertItem( tr( "&Sniffer" ), sniffer );

    id = mb->insertItem( tr( "&Demo" ), demo );
    mb->setItemEnabled( id, true );
    mb->setItemEnabled( uploadID, false );

    #ifdef QWS
    mb->insertItem( startButton );
    mb->insertItem( stopButton );
    mb->insertItem( uploadButton );
    mb->insertItem( d );
    #else // Qt3 changed the insertion order. It's now totally random :(
    mb->insertItem( d );
    mb->insertItem( uploadButton );
    mb->insertItem( stopButton );
    mb->insertItem( startButton );
    #endif

    updateToolButtonState();

    // setup status bar (for now only on X11)

    #ifndef QWS
    statusBar()->message( tr( "Ready." ) );
    #endif

    connect( mw, SIGNAL( startedSniffing() ), this, SLOT( changedSniffingState() ) );
    connect( mw, SIGNAL( stoppedSniffing() ), this, SLOT( changedSniffingState() ) );
};



void WellenreiterMainWindow::showConfigure()
{
    odebug << "show configure..." << oendl;
    cw->setCaption( tr( "Configure" ) );
    int result = QPEApplication::execDialog( cw );

    if ( result ) updateToolButtonState();
}


void WellenreiterMainWindow::updateToolButtonState()
{
    const QString& interface = cw->interfaceName->currentText();
    const int cardtype = cw->driverType();

    if ( ( interface != "<select>" ) && ( cardtype != 0 ) )
    {
        startButton->setEnabled( true );
        menuBar()->setItemEnabled( startID, true );
    }
    else
    {
        startButton->setEnabled( false );
        menuBar()->setItemEnabled( startID, false );
    }
}


void WellenreiterMainWindow::changedSniffingState()
{
    startButton->setEnabled( !mw->sniffing );
    menuBar()->setItemEnabled( startID, !mw->sniffing );
    stopButton->setEnabled( mw->sniffing );
    menuBar()->setItemEnabled( stopID, mw->sniffing );

    if ( !mw->sniffing && QFile::exists( mw->dumpname ) )
    {
        menuBar()->setItemEnabled( uploadID, true );
        uploadButton->setEnabled( true );
    }
}


WellenreiterMainWindow::~WellenreiterMainWindow()
{
    odebug << "Wellenreiter: bye." << oendl;
};


void WellenreiterMainWindow::demoAddStations()
{
    //mw = 0; // test SIGSEGV handling

    mw->netView()->addNewItem( "managed", "Vanille", OMacAddress::fromString("00:00:20:EF:A6:43"), true, 6, 80, GpsLocation( 39.8794, -94.0936) );
    mw->netView()->addNewItem( "managed", "Vanille", OMacAddress::fromString("00:30:6D:EF:A6:23"), true, 11, 10, GpsLocation( 0.0, 0.0 ) );
    mw->netView()->addNewItem( "adhoc", "ELAN", OMacAddress::fromString("00:03:F8:E7:16:22"), false, 3, 10, GpsLocation( 5.5, 2.3 ) );
    mw->netView()->addNewItem( "adhoc", "ELAN", OMacAddress::fromString("00:04:01:E7:56:62"), false, 3, 15, GpsLocation( 2.3, 5.5 ) );
    mw->netView()->addNewItem( "adhoc", "ELAN", OMacAddress::fromString("00:05:8E:E7:56:E2"), false, 3, 20, GpsLocation( -10.0, -20.5 ) );
}


void WellenreiterMainWindow::demoReadFromGps()
{
    WellenreiterConfigWindow* configwindow = WellenreiterConfigWindow::instance();
    GPS* gps = new GPS( this );
    odebug << "Wellenreiter::demoReadFromGps(): url=gps://" << configwindow->gpsdHost->currentText() << ":" << configwindow->gpsdPort->value() << "/" << oendl;
    gps->open( configwindow->gpsdHost->currentText(), configwindow->gpsdPort->value() );
    GpsLocation loc = gps->position();
    QMessageBox::information( this, "Wellenreiter/Opie", tr( "GPS said:\n%1" ).arg( loc.dmsPosition() ) );
    delete gps;
}


QString WellenreiterMainWindow::getFileName( bool save )
{
    QMap<QString, QStringList> map;
    map.insert( tr("All"), QStringList() );
    QStringList text;
    text << "text/*";
    map.insert( tr("Text"), text );
    text << "*";
    map.insert( tr("All"), text );

    QString str;
    if ( save )
    {
        #ifdef QWS
        str = OFileDialog::getSaveFileName( 2, "/", QString::null, map );
        #else
        str = QFileDialog::getSaveFileName();
        #endif
        if ( str.isEmpty() /*|| QFileInfo(str).isDir()*/ )
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
            odebug << "Saved log to file '" << fname << "'" << oendl;
        }
        else
        {
            odebug << "Problem saving log to file '" << fname << "'" << oendl;
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
            odebug << "Saved session to file '" << fname << "'" << oendl;
        }
        else
        {
            odebug << "Problem saving session to file '" << fname << "'" << oendl;
        }
    }
}

void WellenreiterMainWindow::fileSaveHex()
{
    #warning DOES NOT WORK AT THE MOMENT
    /*
    QString fname = getFileName( true );
    if ( !fname.isEmpty() )
    {
        QFile f( fname );
        if ( f.open(IO_WriteOnly) )
        {
            QTextStream t( &f );
            t << mw->hexWindow()->getLog();
            f.close();
            odebug << "Saved hex log to file '" << fname << "'" << oendl;
        }
        else
        {
            odebug << "Problem saving hex log to file '" << fname << "'" << oendl;
        }
    }
    */
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
            odebug << "Loaded session from file '" << fname << "'" << oendl;
        }
        else
        {
            odebug << "Problem loading session from file '" << fname << "'" << oendl;
        }
    }
}


void WellenreiterMainWindow::fileNew()
{
    mw->netView()->clear();
    mw->logWindow()->clear();
    mw->hexWindow()->clear();
}


void WellenreiterMainWindow::closeEvent( QCloseEvent* e )
{
    if ( mw->isDaemonRunning() )
    {
        QMessageBox::warning( this, "Wellenreiter/Opie",
                              tr( "Sniffing in progress!\nPlease stop sniffing before closing." ) );
        e->ignore();
    }
    else
    {
        QMainWindow::closeEvent( e );
    }
}

static const char* CAP_hostname = "www.vanille.de";

#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

void WellenreiterMainWindow::uploadSession()
{
    QLineEdit* from;
    QLineEdit* location;
    QLineEdit* comments;
    QPushButton* accept;
    QPushButton* reject;

    QDialog* d = new QDialog( 0, "session upload", true );
    d->setCaption( tr( "Upload Session" ) );
    QGridLayout* g = new QGridLayout( d, 4, 2, 3 );
    g->addWidget( new QLabel( tr( "From: " ), d ), 0, 0 );
    g->addWidget( from = new QLineEdit( d ), 0, 1 );
    g->addWidget( new QLabel( tr( "Location: " ), d ), 1, 0 );
    g->addWidget( location = new QLineEdit( d ), 1, 1 );
    g->addWidget( new QLabel( tr( "Comments: " ), d ), 2, 0 );
    g->addWidget( comments = new QLineEdit( d ), 2, 1 );
    g->addWidget( accept = new QPushButton( tr( "&Ok" ), d ), 3, 0 );
    g->addWidget( reject = new QPushButton( tr( "&Cancel" ), d ), 3, 1 );
    accept->setDefault( true );
    accept->setAutoDefault( true );
    from->setText( "WL II User" );
    location->setText( "WL II Location" );
    comments->setText( "No Comments." );
    connect( accept, SIGNAL( clicked() ), d, SLOT( accept() ) );
    connect( reject, SIGNAL( clicked() ), d, SLOT( reject() ) );
    int result = d->exec();

    if ( !result )
    {
        odebug << "Session upload cancelled :(" << oendl;
        return;
    }

    odebug << "Starting upload..." << oendl;

    struct sockaddr_in raddr;
    struct hostent *rhost_info;
    int sock = -1;
    bool ok = false;

    rhost_info = (struct hostent *) ::gethostbyname( CAP_hostname );
    if ( rhost_info )
    {


        if ( !QFile::exists( mw->captureFileName() ) )
        {
            QMessageBox::warning( 0, tr( "Error" ), tr( "<p>Logfile '%1' doesn't exist</p>").arg( mw->captureFileName() ) );
            return;
        }

        QFile f( mw->captureFileName() );
        if ( !f.open( IO_ReadOnly ) )
        {
            QMessageBox::warning( 0, tr( "Error" ), tr( "<p>Can't open Logfile '%1'</p>").arg( mw->captureFileName() ) );
            return;
        }

        int content_length = f.size();

        ::memset( &raddr, 0, sizeof (struct sockaddr_in) );
        ::memcpy( &raddr. sin_addr, rhost_info-> h_addr, rhost_info-> h_length );
        raddr.sin_family = rhost_info-> h_addrtype;
        raddr.sin_port = htons ( 80 );

        sock = ::socket( AF_INET, SOCK_STREAM, 0 );

        if ( sock >= 0 )
        {
            if ( ::connect ( sock, (struct sockaddr *) & raddr, sizeof (struct sockaddr)) >= 0 )
            {
                QString header;
                QString content;
                QString preambel;

                header = ""
                "POST /projects/capturedump.spy HTTP/1.1\r\n"
                "Host: www.vanille.de\r\n"
                "User-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.5) Gecko/20031010 Galeon/1.3.10\r\n"
                "Accept: text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,image/jpeg,image/gif;q=0.2,*/*;q=0.1\r\n"
                "Accept-Language: en\r\n"
                "Accept-Encoding: gzip, deflate, compress;q=0.9\r\n"
                "Accept-Charset: us-ascii,utf-8;q=0.7,*;q=0.7\r\n"
                "Keep-Alive: 300\r\n"
                "Connection: keep-alive\r\n"
                "Referer: http://www.vanille.de/projects/capturedump.spy\r\n"
                "Content-Type: multipart/form-data; boundary=---------------------------97267758015830030481215568065\r\n"
                "Content-Length: %1\r\n"
                "\r\n";

                content = ""
                "-----------------------------97267758015830030481215568065\r\n"
                "Content-Disposition: form-data; name=\"Name\"\r\n"
                "\r\n"
                "%1\r\n"
                "-----------------------------97267758015830030481215568065\r\n"
                "Content-Disposition: form-data; name=\"Location\"\r\n"
                "\r\n"
                "%2\r\n"
                "-----------------------------97267758015830030481215568065\r\n"
                "Content-Disposition: form-data; name=\"Comments\"\r\n"
                "\r\n"
                "%3\r\n"
                "-----------------------------97267758015830030481215568065\r\n"
                "Content-Disposition: form-data; name=\"upfile\"; filename=\"%4\"\r\n"
                "Content-Type: application/octet-stream\r\n"
                "\r\n";

                preambel = ""
                "\r\n-----------------------------97267758015830030481215568065--\r\n";

                content = content.arg( from->text().isEmpty() ? QString( "Anonymous Wellenreiter II User" ) : from->text() );
                content = content.arg( location->text().isEmpty() ? QString( "Anonymous Wellenreiter II Location" ) : location->text() );
                content = content.arg( comments->text().isEmpty() ? QString( "Anonymous Wellenreiter II Comments" ) : comments->text() );
                content = content.arg( mw->captureFileName() );

                header = header.arg( QString::number( content.length() + f.size() + preambel.length() ) );

                // write header

                const char* ascii = header.latin1();
                uint ascii_len = ::strlen( ascii );
                ::write ( sock, ascii, ascii_len );

                // write fixed content

                ascii = content.latin1();
                ascii_len = ::strlen( ascii );
                ::write ( sock, ascii, ascii_len );

                // write variable content

                char ch;
                while ( !f.atEnd() )
                {
                    f.readBlock( &ch, 1 );
                    ::write ( sock, &ch, 1 );
                }

                // write preambel

                ascii = preambel.latin1();
                ascii_len = ::strlen( ascii );
                ::write ( sock, ascii, ascii_len );

                // done!

                ok = true;
            }
        }
        ::close ( sock );
    }
    if ( ok )
    QMessageBox::information( 0, tr( "Success" ),
                              QString ( "<p>%1</p>" ).arg( tr( "Capture Dump was uploaded to %1" ) ).arg( CAP_hostname ) );
    else
    QMessageBox::warning( 0, tr( "Error" ),
                          QString ( "<p>%1</p>" ).arg ( tr( "Connection to %1 failed" ) ).arg( CAP_hostname ) );
}


void WellenreiterMainWindow::viewExpandAll()
{
    mw->netView()->expand();
}

void WellenreiterMainWindow::viewCollapseAll()
{
    mw->netView()->collapse();
}
