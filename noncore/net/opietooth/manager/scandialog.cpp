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

#include <qframe.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qprogressbar.h>
#include <qlist.h>

#include <manager.h>
#include <device.h>



namespace OpieTooth {

#include <remotedevice.h>

/*
 */
    ScanDialog::ScanDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
        : QDialog( parent, name, modal, fl ) {

        if ( !name )
            setName( "ScanDialog" );
        resize( 240, 320 );
        setCaption( tr( "Scan for devices" ) );

        Frame7 = new QFrame( this, "Frame7" );
        Frame7->setGeometry( QRect( 0, 0, 240, 331 ) );
        Frame7->setFrameShape( QFrame::StyledPanel );
        Frame7->setFrameShadow( QFrame::Raised );

        QWidget* privateLayoutWidget = new QWidget( Frame7, "Layout11" );
	privateLayoutWidget->setGeometry( QRect( 10, 9, 221, 280 ) );
        Layout11 = new QVBoxLayout( privateLayoutWidget );
        Layout11->setSpacing( 6 );
        Layout11->setMargin( 0 );

        progress = new QProgressBar(privateLayoutWidget,  "progbar");
        progress->setTotalSteps(20);

        QFrame *buttonFrame = new QFrame(Frame7, "");

        StartButton = new QPushButton( buttonFrame, "StartButton" );
        StartButton->setText( tr( "Start scan" ) );

        StopButton = new QPushButton( buttonFrame, "StopButton" );
        StopButton->setText( tr( "Cancel scan" ) );

        QHBoxLayout *buttonLayout = new QHBoxLayout(buttonFrame);

        buttonLayout->addWidget(StartButton);
	buttonLayout->addWidget(StopButton);

        ListView1 = new QListView( privateLayoutWidget, "ListView1" );

        //ListView1->addColumn( tr( "Add" ) );
        ListView1->addColumn( tr( "Add Device" ) );
        //ListView1->addColumn( tr( "Type" ) );

        Layout11->addWidget( ListView1);
        Layout11->addWidget(progress);
        Layout11->addWidget( buttonFrame);

        localDevice = new Manager( "hci0" );

        connect( (QObject*)StartButton, SIGNAL( clicked() ), this, SLOT( startSearch() ) );
        connect( (QObject*)StopButton, SIGNAL( clicked() ),  this,  SLOT( stopSearch() ) );
        connect( (QObject*)localDevice, SIGNAL( foundDevices( const QString& , RemoteDevices::ValueList ) ),
                 this, SLOT(fillList(const QString& , RemoteDevices::ValueList ) ) ) ;
        progressStat = 0;
    }

// hack, make cleaner later
    void ScanDialog::progressTimer() {

        progressStat++;
            if (progressStat++ < 20) {
                QTimer::singleShot( 1000, this, SLOT(progressTimer() ) );
            }
            progress->setProgress(progressStat++);

    }

    void ScanDialog::startSearch() {
        progress->setProgress(0);
        progressStat = 0;

        QCheckListItem  *deviceItem2 = new QCheckListItem( ListView1, "Test1", QCheckListItem::CheckBox );
        deviceItem2->setText(1, "BLAH" );

        progressTimer();
        // when finished, it emmite foundDevices()
        // checken ob initialisiert , qcop ans applet.
        localDevice->searchDevices();

    }

    void ScanDialog::stopSearch() {

    }

    void ScanDialog::fillList(const QString& device, RemoteDevices::ValueList deviceList) {

        QCheckListItem * deviceItem;

        RemoteDevices::ValueList::Iterator it;
        for( it = deviceList.begin(); it != deviceList.end(); ++it ) {

            deviceItem = new QCheckListItem( ListView1, (*it).name() );
            deviceItem->setText(1, (*it).mac() );
        }
    }

/*
 * Iterates trough the items, and collects the checked items.
 * Then it emits it, so the manager can connect to the signal to fill the listing.
 */
    void ScanDialog::emitToManager() {

        if (!ListView1) {
            return;
        }

        QList<RemoteDevices> *deviceList = new QList<RemoteDevices>;

        QListViewItemIterator it( ListView1 );
        for ( ; it.current(); ++it ) {
            if ( ((QCheckListItem*)it.current())->isOn() ) {
                RemoteDevices* device = new RemoteDevices(  it.current()->text(1), it.current()->text(0));
                   deviceList->append( device );
            }
        }
        emit selectedDevices( *deviceList );
    }

/*
 * Cleanup
 */
    ScanDialog::~ScanDialog() {
        emitToManager();
        delete localDevice;
    }

}
