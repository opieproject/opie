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

/* LOCAL */
#include "configwindow.h"
#include "mainwindow.h"

/* OPIE */
#include <opie2/onetwork.h>
#ifdef QWS
#include <opie2/oapplication.h>
#include <opie2/oconfig.h>
#include <opie2/odevice.h>
#include <opie2/odebug.h>
using namespace Opie::Core;
using namespace Opie::Net;
#endif

/* QT */
#include <qapplication.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qfile.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qmap.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qtabwidget.h>
#include <qtoolbutton.h>
#include <qtextstream.h>

/* STD */
#include <assert.h>

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
    connect( newNetworkAction, SIGNAL( activated(int) ), this, SLOT( changedNetworkAction(int) ) );
    connect( newClientAction, SIGNAL( activated(int) ), this, SLOT( changedClientAction(int) ) );
    connect( newStationAction, SIGNAL( activated(int) ), this, SLOT( changedStationAction(int) ) );
    connect( getCaptureFileName, SIGNAL( clicked() ), this, SLOT( getCaptureFileNameClicked() ) );

    // make the checkbox 'channelAll' control all other channels
    connect( channelAll, SIGNAL( stateChanged(int) ), this, SLOT( channelAllClicked(int) ) );

    connect( autodetect, SIGNAL( clicked() ), this, SLOT( performAutodetection() ) );

    // hide tab4 (parse) until Wellenreiter 1.2
    tab->removePage( tab_4 );
};


void WellenreiterConfigWindow::accept()
{
    save();
    QDialog::accept();
}


WellenreiterConfigWindow::~WellenreiterConfigWindow()
{
}


void WellenreiterConfigWindow::performAutodetection()
{
    //TODO: insert modal splash screen here
    //      and sleep a second, so that it looks
    //      like we're actually doing something fancy... ;-)

    odebug << "WellenreiterConfigWindow::performAutodetection()" << oendl;

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
            odebug << "Wellenreiter: guessed device type to be #" << devicetype << "" << oendl;
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


void WellenreiterConfigWindow::synchronizeActionsAndScripts()
{
    if ( newNetworkAction->currentItem() == 4 ) newNetworkScript->show(); else newNetworkScript->hide();
    if ( newClientAction->currentItem() == 4 ) newClientScript->show(); else newClientScript->hide();
    if ( newStationAction->currentItem() == 4 ) newStationScript->show(); else newStationScript->hide();

    //newNetworkScript->setEnabled( newNetworkAction->currentItem() == 4 );
    //newClientScript->setEnabled( newClientAction->currentItem() == 4 );
    //newStationScript->setEnabled( newStationAction->currentItem() == 4 );
}


void WellenreiterConfigWindow::changedNetworkAction(int t)
{
    synchronizeActionsAndScripts();
}


void WellenreiterConfigWindow::changedClientAction(int t)
{
    synchronizeActionsAndScripts();
}


void WellenreiterConfigWindow::changedStationAction(int t)
{
    synchronizeActionsAndScripts();
}


void WellenreiterConfigWindow::getCaptureFileNameClicked()
{
    QString name = ( (WellenreiterMainWindow*) qApp->mainWidget() )->getFileName(true);
    odebug << "name = " << name << "" << oendl;
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


void WellenreiterConfigWindow::performAction( const QString& type,
                                              const QString& essid,
                                              const QString& mac,
                                              bool wep,
                                              int channel,
                                              int signal
                                              /* , const GpsLocation& loc */ )
{
    int action;
    QString script;

    if ( type == "network" )
    {
        action = newNetworkAction->currentItem();
        script = newNetworkScript->text();
    }
    else if ( type == "managed" || type == "adhoc" )
    {
        action = newClientAction->currentItem();
        script = newClientScript->text();
    }
    else if ( type == "station" )
    {
        action = newStationAction->currentItem();
        script = newStationScript->text();
    }
    else
    {
        owarn << "WellenreiterConfigWindow::performAction(): unknown type '" << type << "'" << oendl;
        return;
    }

    odebug << "for event '" << (const char*) type << "' I'm going to perform action " << action << " (script='" << script << "')" << oendl;

    switch( action )
    {
        case 0: /* Ignore */ return;
        case 1: /* Play Alarm */ ODevice::inst()->playAlarmSound(); return;
        case 2: /* Play Click */ ODevice::inst()->playTouchSound(); return;
        case 3: /* Blink LED */ break; //FIXME: Implement this
        case 4: /* Run Script */
        {
            /**
                *
                * Script Substitution Information:
                *
                * $SSID = SSID
                * $MAC = MAC
                * $WEP = Wep
                * $CHAN = Channel
                *
                **/
            script = script.replace( QRegExp( "$SSID" ), essid );
            script = script.replace( QRegExp( "$MAC" ), mac );
            script = script.replace( QRegExp( "$WEP" ), wep ? QString( "true" ) : QString( "false" ) );
            script = script.replace( QRegExp( "$CHAN" ), QString::number( channel ) );

            odebug << "going to call script '" << script << "'" << oendl;
            ::system( script );
            odebug << "script returned." << oendl;
            return;
        }
        default: assert( false );
    }
}


void WellenreiterConfigWindow::load()
{
#ifdef Q_WS_X11
    #warning Persistent Configuration not yet implemented for standalone X11 build
    performAutodetection();
#else
    odebug << "loading configuration settings..." << oendl;

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
    writeCaptureFile->setChecked( c->readBoolEntry( "writeCaptureFile", true ) );
    captureFileName->setEnabled( writeCaptureFile->isChecked() );
    getCaptureFileName->setEnabled( writeCaptureFile->isChecked() );
    captureFileName->setText( c->readEntry( "filename", "/tmp/capture" ) );
    hexViewBufferUnlimited->setChecked( c->readBoolEntry( "hexViewBufferUnlimited", true ) );
    hexViewBufferLimited->setChecked( !c->readBoolEntry( "hexViewBufferUnlimited", true ) );
    hexViewBufferSize->setValue( c->readNumEntry( "hexViewBufferSize", 2000 ) );

    c->setGroup( "UI" );
    lookupVendor->setChecked( c->readBoolEntry( "lookupVendor", true ) );
    openTree->setChecked( c->readBoolEntry( "openTree", true ) );
    disablePM->setChecked( c->readBoolEntry( "disablePM", true ) );
    newNetworkAction->setCurrentItem( c->readNumEntry( "newNetworkAction", 1 ) ); // Default: Play Alarm
    newNetworkScript->setText( c->readEntry( "newNetworkScript", "" ) );
    newClientAction->setCurrentItem( c->readNumEntry( "newClientAction", 2 ) ); // Default: Play Click
    newClientScript->setText( c->readEntry( "newClientScript", "" ) );
    newStationAction->setCurrentItem( c->readNumEntry( "newStationAction", 2 ) ); // Default: Play Click
    newStationScript->setText( c->readEntry( "newStationScript", "" ) );
    synchronizeActionsAndScripts(); // needed for showing/hiding the script QLineEdit on demand

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
    odebug << "saving configuration settings..." << oendl;

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
    c->writeEntry( "writeCaptureFile", writeCaptureFile->isChecked() );
    c->writeEntry( "filename", captureFileName->text() );
    c->writeEntry( "hexViewBufferUnlimited", hexViewBufferUnlimited->isChecked() );
    c->writeEntry( "hexViewBufferSize", hexViewBufferSize->value() );

    c->setGroup( "UI" );
    c->writeEntry( "lookupVendor", lookupVendor->isChecked() );
    c->writeEntry( "openTree", openTree->isChecked() );
    c->writeEntry( "disablePM", disablePM->isChecked() );
    c->writeEntry( "newNetworkAction", newNetworkAction->currentItem() );
    c->writeEntry( "newNetworkScript", newNetworkScript->text() );
    c->writeEntry( "newClientAction", newClientAction->currentItem() );
    c->writeEntry( "newClientScript", newClientScript->text() );
    c->writeEntry( "newStationAction", newStationAction->currentItem() );
    c->writeEntry( "newStationScript", newStationScript->text() );

    c->setGroup( "GPS" );
    c->writeEntry( "use", enableGPS->isChecked() );
    c->writeEntry( "host", gpsdHost->currentText() );
    c->writeEntry( "port", gpsdPort->value() );
    c->writeEntry( "start", startGPS->isChecked() );
    c->writeEntry( "command", commandGPS->text() );

    c->write();

#endif
}


int WellenreiterConfigWindow::hexViewBuffer() const
{
    return hexViewBufferUnlimited->isChecked() ? -1 : hexViewBufferSize->value();
}
