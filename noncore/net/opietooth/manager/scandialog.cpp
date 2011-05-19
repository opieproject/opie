/* main.cpp
 *
 * ---------------------
 *
 * copyright   : (c) 2002 by Maximilian Reiﬂ
 * email       : max.reiss@gmx.de
 *
 */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "scandialog.h"

#include <qheader.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qprogressbar.h>
#include <qlist.h>

#include <opie2/obluetooth.h>
#include <opie2/odebug.h>
using namespace Opie::Core;
using namespace Opie::Bluez;

#include <remotedevice.h>

namespace OpieTooth {

/**
 */
ScanDialog::ScanDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{

    setCaption( tr( "Scan for devices" ) );

    Layout11 = new QVBoxLayout( this );
    Layout11->setSpacing( 6 );
    Layout11->setMargin( 0 );

    progress = new QProgressBar( this, "progbar");
    progress->setTotalSteps(20);

    StartStopButton = new QPushButton( this, "StartButton" );
    StartStopButton->setText( tr( "Start scan" ) );

    serviceView = new QListView( this, "serviceView" );

    //serviceView->addColumn( tr( "Add" ) );
    serviceView->addColumn( tr( "Add Device" ) );
    //serviceView->addColumn( tr( "Type" ) );

    Layout11->addWidget( serviceView );
    Layout11->addWidget( progress );
    Layout11->addWidget( StartStopButton );

    m_bluetooth = OBluetooth::instance();
    connect( m_bluetooth, SIGNAL( defaultInterfaceChanged( OBluetoothInterface* ) ),
                this, SLOT( defaultInterfaceChanged( OBluetoothInterface* ) ) ) ;
    m_btinterface = m_bluetooth->defaultInterface();
    if( m_btinterface )
        connectInterface();

    connect( StartStopButton, SIGNAL( clicked() ), this, SLOT( startSearch() ) );

    m_progressStat = 0;
    m_search = false;

    QTimer::singleShot( 0, this, SLOT( startSearch() ) );
}

ScanDialog::~ScanDialog()
{
}

// hack, make cleaner later
void ScanDialog::progressTimer()
{
    m_progressStat++;
    if ( m_progressStat++ < 20 && m_search ) {
        QTimer::singleShot( 2000, this, SLOT( progressTimer() ) );
        progress->setProgress( m_progressStat++ );
    }
}

void ScanDialog::accept()
{
    if( m_search )
        stopSearch();
    emitToManager();
    QDialog::accept();
}

void ScanDialog::reject()
{
    if( m_search )
        stopSearch();
    QDialog::reject();
}

void ScanDialog::startSearch()
{
    if ( m_search ) {
        stopSearch();
        return;
    }
    m_added.clear();
    m_search = true;
    progress->setProgress(0);
    m_progressStat = 0;

    // empty list before a new scan
    serviceView->clear();

    progressTimer();
    // when finished, it emmite foundDevices()
    // checken ob initialisiert , qcop ans applet.
    StartStopButton->setText( tr( "Stop scan" ) );

    m_btinterface->startDiscovery();
}

void ScanDialog::stopSearch()
{
    m_btinterface->stopDiscovery();
}

void ScanDialog::propertyChanged( const QString &prop )
{
    if( prop == "Discovering" ) {
        if( ! m_btinterface->discovering() )
            searchStopped();
    }
}

void ScanDialog::deviceDiscovered( OBluetoothDevice *dev )
{
    if( m_search ) {
        QString macAddress = dev->macAddress();
        if( !m_added.contains( macAddress ) ) {
            m_added.append( macAddress );
            QCheckListItem *deviceItem = new QCheckListItem( serviceView, dev->name(),  QCheckListItem::CheckBox );
            deviceItem->setText( 1, macAddress );
        }
    }
}

void ScanDialog::defaultInterfaceChanged( OBluetoothInterface *intf )
{
    if( m_search )
        searchStopped();
    m_btinterface = intf;
    if( m_btinterface )
        connectInterface();
    else
        reject();
}

void ScanDialog::searchStopped()
{
    m_search = false;
    StartStopButton->setText( tr( "Start scan" ) );
    progress->setProgress(0);
    m_progressStat = 0;
}

void ScanDialog::connectInterface()
{
    disconnect( this, SLOT( deviceDiscovered( OBluetoothDevice* ) ) );
    disconnect( this, SLOT( propertyChanged( const QString& ) ) );
    connect( m_btinterface, SIGNAL( deviceDiscovered( OBluetoothDevice* ) ),
            this, SLOT( deviceDiscovered( OBluetoothDevice* ) ) ) ;
    connect( m_btinterface, SIGNAL( propertyChanged( const QString& ) ),
            this, SLOT( propertyChanged( const QString& ) ) ) ;
}

/**
 * Iterates trough the items, and collects the checked items.
 * Then it emits it, so the manager can connect to the signal to fill the listing.
 */
void ScanDialog::emitToManager()
{

    if (!serviceView) {
        return;
    }

    QValueList<RemoteDevice> deviceList;

    QListViewItemIterator it( serviceView );
    for ( ; it.current(); ++it ) {
        if ( ( (QCheckListItem*)it.current() )->isOn() ) {
            RemoteDevice device(  it.current()->text(1), it.current()->text(0) );
            deviceList.append( device );
        }
    }
    emit selectedDevices( deviceList );
}

}
