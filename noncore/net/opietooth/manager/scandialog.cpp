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

#include "manager_base.h"
#include "device.h"


namespace OpieTooth {

#include "../include/remotedevice.h"

/**
 */
    ScanDialog::ScanDialog( Manager_Base* manager, QWidget* parent,  const char* name, bool modal, WFlags fl )
        : QDialog( parent, name, modal, fl ), localDevice( manager ) {

        setCaption( tr( "Scan for devices" ) );

        Layout11 = new QVBoxLayout( this );
        Layout11->setSpacing( 6 );
        Layout11->setMargin( 0 );

        progress = new QProgressBar( this, "progbar");
        progress->setTotalSteps(20);

        StartStopButton = new QPushButton( this, "StartButton" );
        StartStopButton->setText( tr( "Start scan" ) );

        ListView1 = new QListView( this, "ListView1" );

        //ListView1->addColumn( tr( "Add" ) );
        ListView1->addColumn( tr( "Add Device" ) );
        //ListView1->addColumn( tr( "Type" ) );

        Layout11->addWidget( ListView1 );
        Layout11->addWidget( progress );
        Layout11->addWidget( StartStopButton );

        // localDevice = new Manager( "hci0" );

        connect( StartStopButton, SIGNAL( clicked() ), this, SLOT( startSearch() ) );
        connect( localDevice, SIGNAL( foundDevices( const QString& , RemoteDevice::ValueList ) ),
                 this, SLOT( fillList( const QString& , RemoteDevice::ValueList ) ) ) ;

        progressStat = 0;
        m_search = false;
    }

// hack, make cleaner later
    void ScanDialog::progressTimer() {

        progressStat++;
        if ( progressStat++ < 20 && m_search ) {
            QTimer::singleShot( 2000, this, SLOT( progressTimer() ) );
            progress->setProgress( progressStat++ );
        }
    }

    void ScanDialog::accept() {
        emitToManager();
        QDialog::accept();
    }


    void ScanDialog::startSearch() {
        if ( m_search ) {
            stopSearch();
            return;
        }
        m_search = true;
        progress->setProgress(0);
        progressStat = 0;

        // empty list before a new scan
        ListView1->clear();

        progressTimer();
        // when finished, it emmite foundDevices()
        // checken ob initialisiert , qcop ans applet.
        StartStopButton->setText( tr( "Stop scan" ) );

        localDevice->searchDevices();

    }

    void ScanDialog::stopSearch() {
        m_search = true;
    }

    void ScanDialog::fillList(const QString&, RemoteDevice::ValueList deviceList) {
        progress->setProgress(0);
        progressStat = 0;
        QCheckListItem * deviceItem;

        RemoteDevice::ValueList::Iterator it;
        for( it = deviceList.begin(); it != deviceList.end(); ++it ) {

            deviceItem = new QCheckListItem( ListView1, (*it).name(),  QCheckListItem::CheckBox );
            deviceItem->setText( 1, (*it).mac() );
        }
        m_search = false;
        StartStopButton->setText( tr( "Start scan" ) );
    }

/**
 * Iterates trough the items, and collects the checked items.
 * Then it emits it, so the manager can connect to the signal to fill the listing.
 */
    void ScanDialog::emitToManager() {

        if (!ListView1) {
            return;
        }

        QValueList<RemoteDevice> deviceList;

        QListViewItemIterator it( ListView1 );
        for ( ; it.current(); ++it ) {
            if ( ( (QCheckListItem*)it.current() )->isOn() ) {
                RemoteDevice device(  it.current()->text(1), it.current()->text(0) );
                deviceList.append( device );
            }
        }
        emit selectedDevices( deviceList );
    }

/**
 * Cleanup
 */
    ScanDialog::~ScanDialog() {
        qWarning("delete scan dialog");
        delete localDevice;
    }
}
