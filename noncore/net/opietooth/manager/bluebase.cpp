/*
 * bluebase.cpp *
 * ---------------------
 *
 * copyright   : (c) 2002 by Maximilian Reiß
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

#include "bluebase.h"
#include "scandialog.h"

#include <qframe.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qtabwidget.h>
#include <qscrollview.h>
#include <qvbox.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qdir.h>

#include <qpe/resource.h>
#include <qpe/config.h>

#include <remotedevice.h>


namespace OpieTooth {


    BlueBase::BlueBase( QWidget* parent,  const char* name, WFlags fl )
        : BluetoothBase( parent, name, fl ) {


        QObject::connect( (QObject*)PushButton2,  SIGNAL( clicked() ), this, SLOT(startScan() ) );
        QObject::connect( (QObject*)configApplyButton, SIGNAL(clicked() ), this, SLOT(applyConfigChanges() ) );
        QObject::connect( (QObject*)ListView2, SIGNAL(  expanded( QListViewItem * item ) ),
                          this, SLOT( addServicesToDevice( QListViewItem * item ) ) );
        QObject::connect( (QObject*)ListView2, SIGNAL( clicked( QListViewItem * )),
                          this, SLOT( startServiceActionClicked( QListViewItem * item ) ) );

        //
        //      QObject::connect( (QObject*) Manager, SIGNAL (foundServices( const QString& device, Services::ValueList ), this () ) );

        //Load all icons needed


        QPixmap offPix = Resource::loadPixmap( "editdelete" );
        QPixmap onPix = Resource::loadPixmap( "installed" );

        QPalette pal = this->palette();
        QColor col = pal.color(QPalette::Active, QColorGroup::Background);
        pal.setColor(QPalette::Active, QColorGroup::Button, col);
        pal.setColor(QPalette::Inactive, QColorGroup::Button, col);
        pal.setColor(QPalette::Normal, QColorGroup::Button, col);
        pal.setColor(QPalette::Disabled, QColorGroup::Button, col);
        this->setPalette(pal);

        setCaption( tr( "Bluetooth Manager" ) );

        readConfig();
        initGui();


        //TESTING

        ListView2->setRootIsDecorated(true);

        QListViewItem *topLV = new QListViewItem( ListView2, "Harlekins Dongle" , "yes");
        topLV->setPixmap(0, offPix);
        (void) new QListViewItem( topLV, "on" );
        (void) new QListViewItem( topLV, "off"  );

        QListViewItem *topLV2 = new QListViewItem( ListView2, "Siemens S45" , "no" );
        topLV2->setPixmap(0, onPix);
        (void) new QListViewItem( topLV2, "on"  );
        (void) new QListViewItem( topLV2, "off" );

    }


    /**
     * Reads all options from the config file
     */
    void BlueBase::readConfig() {

        Config cfg("bluetoothmanager");
        cfg.setGroup("bluezsettings");


        deviceName = cfg.readEntry("name", "No name"); // name the device should identify with
        defaultPasskey = cfg.readEntryCrypt("passkey", ""); // <- hmm, look up how good the trolls did that, maybe too weak
        useEncryption = cfg.readNumEntry("useEncryption", 1);
        enableAuthentification = cfg.readNumEntry("enableAuthentification", 1);
        enablePagescan = cfg.readNumEntry("enablePagescan",1);
        enableInquiryscan = cfg.readNumEntry("enableInquiryscan", 1);

    }

    /**
     * Writes all options to the config file
     */
    void BlueBase::writeConfig() {


        Config cfg("bluetoothmanager");
        cfg.setGroup("bluezsettings");

        cfg.writeEntry("name", deviceName);
        cfg.writeEntryCrypt("passkey", defaultPasskey);
        cfg.writeEntry("useEncryption", useEncryption);
        cfg.writeEntry("enableAuthentification", enableAuthentification);
        cfg.writeEntry("enablePagescan",enablePagescan);
        cfg.writeEntry("enableInquiryscan", enableInquiryscan);
}


    /*
     * Read the list of allready known devices
     *
     */
    void BlueBase::readSavedDevices() {

        QList<RemoteDevice> *loadedDevices = new QList<RemoteDevice>;

        Config deviceList( QDir::homeDirPath() + "/Settings/bluetooth/devicelist.conf", Config::File );


        //   RemoteDevice *currentDevice = RemoteDevice( ,  );
        //loadedDevices->append( currentDevice );

        addSearchedDevices( *loadedDevices );
    }

    /*
     * Write the list of allready known devices
     *
     */
    void BlueBase::writeSavedDevices() {

        QListViewItemIterator it( ListView2 );

        // one top conf file with all decices (by mac adress)
        Config deviceList( QDir::homeDirPath() + "/Settings/bluetooth/devicelist.conf", Config::File );

        for ( ; it.current(); ++it ) {

            // MAC adress as group
            deviceList.setGroup( it.current()->text(1) );
            deviceList.writeEntry("inList",  1);

            // seperate config file for each device, to store more information in future.

            Config conf( QDir::homeDirPath() + "/Settings/bluetooth/" + (it.current()->text(1)) + ".conf", Config::File );
            conf.setGroup("Info");
            conf.writeEntry("name", it.current()->text(0) );
        }
    }


    /**
     * Set up the gui
     */
    void BlueBase::initGui() {

        StatusLabel->setText(getStatus()); // maybe move it to getStatus()

        cryptCheckBox->setChecked(useEncryption);
        authCheckBox->setChecked(enableAuthentification);
        pagescanCheckBox->setChecked(enablePagescan);
        inquiryscanCheckBox->setChecked(enableInquiryscan);
        deviceNameLine->setText(deviceName);
        passkeyLine->setText(defaultPasskey);
        // set info tab
        setInfo();
    }


    /**
     * Get the status informations and returns it
     * @return QString the status informations gathered
     */
    QString BlueBase::getStatus(){

        QString infoString = tr("<b>Device name : </b> Ipaq" );
        infoString += QString("<br><b>" + tr("MAC adress: ") +"</b> No idea");
        infoString += QString("<br><b>" + tr("Class") + "</b> PDA");

        return (infoString);

    }


    /**
     * Read the current values from the gui and invoke writeConfig()
     */
    void BlueBase::applyConfigChanges() {

        deviceName =  deviceNameLine->text();
        defaultPasskey = passkeyLine->text();
        useEncryption = cryptCheckBox->isChecked();
        enableAuthentification = authCheckBox->isChecked();
        enablePagescan = pagescanCheckBox->isChecked();
        enableInquiryscan = inquiryscanCheckBox->isChecked();

        writeConfig();

        QMessageBox*  box = new QMessageBox(this, "Test");
        box->setText(tr("Changes applied"));
        box->show();

  // falls nötig hcid killhupen - die funktionalität adden
}


    /*
     * Add fresh found devices from scan dialog to the listing
     *
     */
    void BlueBase::addSearchedDevices( QList<RemoteDevice> &newDevices ) {

        QListViewItem * deviceItem;

        QListIterator<RemoteDevice> it(newDevices);

        for( ; it.current(); ++it ) {


            RemoteDevice *dev = it.current();
            deviceItem = new QListViewItem( ListView2, dev->name() );

            if (deviceActive( dev ) ) {
                deviceItem->setPixmap(0, onPix);
            } else {
                deviceItem->setPixmap(0, offPix);
            }

            deviceItem->setText(1, dev->mac() );
        }
    }


    /*
     * Action that is toggled on entrys on click
     */
    void BlueBase::startServiceActionClicked( QListViewItem * item ) {


    }

    /*
     * Action that are toggled on hold (mostly QPopups i guess)
     */
    void BlueBase::startServiceActionHold( QListViewItem * item, const QPoint & point, int column) {


    }

    /*
     * Search and display avail. services for a device (on expand from device listing)
     *
     */
    void BlueBase::addServicesToDevice( QListViewItem * item ) {
        // row of mac adress
        RemoteDevice *device = new RemoteDevice(item->text(1),  item->text(0));
        //dann nen manager und darauf dann searchServises, das liefert nen signal, das wieder connected
        // werden  muss.

        // delete
    }

    /*
     * Find out if a device can  currently be reached
     */
    bool BlueBase::deviceActive( RemoteDevice *device ) {

        return true;
    }

    /**
     * Open the "scan for devices"  dialog
     */
    void BlueBase::startScan() {

        ScanDialog *scan = new ScanDialog( this, "", true);
        QObject::connect( scan, SIGNAL( selectedDevices(QList<RemoteDevice>&) ),
                         this, SLOT( addSearchedDevices(QList<RemoteDevice>& ) ));

        scan->showMaximized();
    }


    void BlueBase::setInfo() {
        StatusLabel->setText(getStatus());
    }

    /**
     * Decontructor
     */
    BlueBase::~BlueBase(){
        writeSavedDevices();
    }

}

