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

#include <manager.h>
#include <device.h>
#include <remotedevice.h>


namespace OpieTooth {

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
        StartButton->setText( tr( "Start" ) );

        StopButton = new QPushButton( buttonFrame, "StopButton" );
        StopButton->setText( tr( "Cancel" ) );

        QHBoxLayout *buttonLayout = new QHBoxLayout(buttonFrame);
        buttonLayout->addWidget(StartButton);
	buttonLayout->addWidget(StopButton);

        ListView1 = new QListView( privateLayoutWidget, "ListView1" );

        ListView1->addColumn( tr( "Add" ) );
        ListView1->addColumn( tr( "Device Name" ) );
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

        progressTimer();
        // when finished, it emmite foundDevices()
        // checken ob initialisiert , qcop ans applet.
        localDevice->searchDevices();

    }

    void ScanDialog::stopSearch() {

    }

    void ScanDialog::fillList(const QString& device, RemoteDevices::ValueList deviceList) {

        QListViewItem * deviceItem;

        RemoteDevices::ValueList::Iterator it;
        for( it = deviceList.begin(); it != deviceList.end(); ++it ) {

            deviceItem = new QListViewItem( ListView1, (*it).name() );
        }
    }

/*
 * Cleanup
 */
    ScanDialog::~ScanDialog() {
        delete localDevice;
    }

}
