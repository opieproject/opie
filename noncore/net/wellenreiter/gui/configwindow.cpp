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

/* LOCAL */
#include "configwindow.h"
#include "mainwindow.h"

/* QT */
#include <qapplication.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qfile.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qmap.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qspinbox.h>
#include <qtextstream.h>

/* OPIE */
#include <opie2/onetwork.h>

#ifdef QWS
#include <opie2/oapplication.h>
#include <opie2/oconfig.h>
#endif

WellenreiterConfigWindow* WellenreiterConfigWindow::_instance = 0;

WellenreiterConfigWindow::WellenreiterConfigWindow( QWidget * parent, const char * name, WFlags f )
           :WellenreiterConfigBase( parent, name, true, f )
{
    _devicetype[ "cisco" ] = DEVTYPE_CISCO;
    _devicetype[ "wlan-ng" ] = DEVTYPE_WLAN_NG;
    _devicetype[ "hostap" ] = DEVTYPE_HOSTAP;
    _devicetype[ "orinoco" ] = DEVTYPE_ORINOCO;
    _devicetype[ "<manual>" ] = DEVTYPE_MANUAL;
    _devicetype[ "<file>" ] = DEVTYPE_FILE;

    // gather possible interface names from ONetwork
    ONetwork* net = ONetwork::instance();
    ONetwork::InterfaceIterator it = net->iterator();
    while ( it.current() )
    {
        if ( it.current()->isWireless() )
            interfaceName->insertItem( it.current()->name() );
        ++it;
    }

    load();

    #ifdef Q_WS_X11 // We're on X11: adding an Ok-Button for the Dialog here
    QPushButton* okButton = new QPushButton( "ok", this );
    okButton->show();
    WellenreiterConfigBaseLayout->addWidget( okButton, 0, 3 ); //FIXME: rename this in configbase.ui
    connect( okButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
    #endif

    WellenreiterConfigWindow::_instance = this;

    connect( deviceType, SIGNAL( activated(int) ), this, SLOT( changedDeviceType(int) ) );
    connect( getCaptureFileName, SIGNAL( clicked() ), this, SLOT( getCaptureFileNameClicked() ) );

    // make the checkbox 'channelAll' control all other channels
    connect( channelAll, SIGNAL( stateChanged(int) ), this, SLOT( channelAllClicked(int) ) );

    connect( autodetect, SIGNAL( clicked() ), this, SLOT( performAutodetection() ) );
};


WellenreiterConfigWindow::~WellenreiterConfigWindow()
{
    save();
}


void WellenreiterConfigWindow::performAutodetection()
{
    //TODO: insert modal splash screen here
    //      and sleep a second, so that it looks
    //      like we're actually doing something fancy... ;-)

    qDebug( "WellenreiterConfigWindow::performAutodetection()" );

    // try to guess device type
    QFile m( "/proc/modules" );
    if ( m.open( IO_ReadOnly ) )
    {
        int devicetype(0);
        QString line;
        QTextStream modules( &m );
        while( !modules.atEnd() && !devicetype )
        {
            modules >> line;
            if ( line.contains( "cisco" ) ) devicetype = DEVTYPE_CISCO;
            else if ( line.contains( "hostap" ) ) devicetype = DEVTYPE_HOSTAP;
            else if ( line.contains( "prism" ) ) devicetype = DEVTYPE_WLAN_NG;
            else if ( line.contains( "orinoco" ) ) devicetype = DEVTYPE_ORINOCO;
        }
        if ( devicetype )
        {
            deviceType->setCurrentItem( devicetype );
            _guess = devicetype;
            qDebug( "Wellenreiter: guessed device type to be #%d", devicetype );
        }
    }
}


int WellenreiterConfigWindow::driverType() const
{
    QString name = deviceType->currentText();
    if ( _devicetype.contains( name ) )
    {
        return _devicetype[name];
    }
    else
    {
        return 0;
    }
};


int WellenreiterConfigWindow::hoppingInterval() const
{
    return hopInterval->cleanText().toInt();
}


bool WellenreiterConfigWindow::usePrismHeader() const
{
    return prismHeader->isChecked();
}


bool WellenreiterConfigWindow::isChannelChecked( int channel ) const
{
    switch ( channel )
    {
        case 1: return channel1->isOn();
        case 2: return channel2->isOn();
        case 3: return channel3->isOn();
        case 4: return channel4->isOn();
        case 5: return channel5->isOn();
        case 6: return channel6->isOn();
        case 7: return channel7->isOn();
        case 8: return channel8->isOn();
        case 9: return channel9->isOn();
        case 10: return channel10->isOn();
        case 11: return channel11->isOn();
        case 12: return channel12->isOn();
        case 13: return channel13->isOn();
        case 14: return channel14->isOn();
    }
}


void WellenreiterConfigWindow::changedDeviceType(int t)
{
    if ( t != DEVTYPE_FILE ) return;
    QString name = ( (WellenreiterMainWindow*) qApp->mainWidget() )->getFileName(false);
    if ( !name.isEmpty() && QFile::exists( name ) )
    {
        interfaceName->insertItem( name );
        interfaceName->setCurrentItem( interfaceName->count()-1 );
    }
    else
    {
        deviceType->setCurrentItem( _guess );
    }

}


void WellenreiterConfigWindow::getCaptureFileNameClicked()
{
    QString name = ( (WellenreiterMainWindow*) qApp->mainWidget() )->getFileName(true);
    qDebug( "name = %s", (const char*) name );
    if ( !name.isEmpty() )
    {
        captureFileName->setText( name );
    }
}


void WellenreiterConfigWindow::channelAllClicked(int state)
{
    bool b = state;
    channel1->setChecked( b );
    channel2->setChecked( b );
    channel3->setChecked( b );
    channel4->setChecked( b );
    channel5->setChecked( b );
    channel6->setChecked( b );
    channel7->setChecked( b );
    channel8->setChecked( b );
    channel9->setChecked( b );
    channel10->setChecked( b );
    channel11->setChecked( b );
    channel12->setChecked( b );
    channel13->setChecked( b );
    channel14->setChecked( b );
}


bool WellenreiterConfigWindow::useGPS() const
{
    return enableGPS->isChecked();
}


const QString WellenreiterConfigWindow::gpsHost() const
{
    return useGPS() ? gpsdHost->currentText() : QString::null;
}


int WellenreiterConfigWindow::gpsPort() const
{
    bool ok;
    return useGPS() ? gpsdPort->value() : -1;
}


void WellenreiterConfigWindow::load()
{
#ifdef Q_WS_X11
    #warning Persistent Configuration not yet implemented for standalone X11 build
    performAutodetection();
#else
    qDebug( "loading configuration settings..." );

    /* This is dumb monkey typing stuff... We _need_ to do this automatically! */

    OConfig* c = oApp->config();

    c->setGroup( "Interface" );

    QString interface = c->readEntry( "name", "<none>" );
    if ( interface != "<none>" )
    {
#if QT_VERSION < 300
        interfaceName->insertItem( interface, 0 );
        interfaceName->setCurrentItem( 0 );
#else
        interfaceName->setCurrentText( interface );
#endif

        QString device = c->readEntry( "type", "<select>" );
#if QT_VERSION < 300
        for ( int i = 0; i < deviceType->count(); ++i )
        {
            if ( deviceType->text( i ) == device )
            {
                deviceType->setCurrentItem( i );
                break;
            }
        }
#else
        deviceType->setCurrentText( device );
#endif
    }
    else
    {
        performAutodetection();
    }

    prismHeader->setChecked( c->readBoolEntry( "prism", false ) );
    hopChannels->setChecked( c->readBoolEntry( "hop", true ) );
    hopInterval->setValue( c->readNumEntry( "interval", 250 ) );
    adaptiveHopping->setChecked( c->readBoolEntry( "adaptive", true ) );

    c->setGroup( "Capture" );
    captureFileName->setText( c->readEntry( "filename", "/tmp/capture" ) );

    c->setGroup( "UI" );
    lookupVendor->setChecked( c->readBoolEntry( "lookupVendor", true ) );
    openTree->setChecked( c->readBoolEntry( "openTree", true ) );
    disablePM->setChecked( c->readBoolEntry( "disablePM", true ) );

    c->setGroup( "GPS" );
    enableGPS->setChecked( c->readBoolEntry( "use", false ) );
#if QT_VERSION < 300
    gpsdHost->insertItem( c->readEntry( "host", "localhost" ), 0 );
    gpsdHost->setCurrentItem( 0 );
#else
    gpsdHost->setCurrentText( c->readEntry( "host", "localhost" ) );
#endif
    gpsdPort->setValue( c->readNumEntry( "port", 2947 ) );
    startGPS->setChecked( c->readBoolEntry( "start", false ) );
    commandGPS->setText( c->readEntry( "command", "gpsd -p /dev/ttyS3 -s 57600" ) );

#endif
}


void WellenreiterConfigWindow::save()
{
#ifdef Q_WS_X11
    #warning Persistent Configuration not yet implemented for standalone X11 build
#else
    qDebug( "saving configuration settings..." );

    /* This is dumb monkey typing stuff... We _need_ to do this automatically! */

    OConfig* c = oApp->config();

    c->setGroup( "Interface" );
    c->writeEntry( "name", interfaceName->currentText() );
    c->writeEntry( "type", deviceType->currentText() );
    c->writeEntry( "prism", prismHeader->isChecked() );
    c->writeEntry( "hop", hopChannels->isChecked() );
    c->writeEntry( "interval", hopInterval->value() );
    c->writeEntry( "adaptive", adaptiveHopping->isChecked() );

    c->setGroup( "Capture" );
    c->writeEntry( "filename", captureFileName->text() );

    c->setGroup( "UI" );
    c->writeEntry( "lookupVendor", lookupVendor->isChecked() );
    c->writeEntry( "openTree", openTree->isChecked() );
    c->writeEntry( "disablePM", disablePM->isChecked() );

    c->setGroup( "GPS" );
    c->writeEntry( "use", enableGPS->isChecked() );
    c->writeEntry( "host", gpsdHost->currentText() );
    c->writeEntry( "port", gpsdPort->value() );
    c->writeEntry( "start", startGPS->isChecked() );
    c->writeEntry( "command", commandGPS->text() );

    c->write();

#endif
}
