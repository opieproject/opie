/*
 * bluebase.cpp *
 * ---------------------
 *
 * begin       : Sun 10 17:20:00 CEST 2002
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

#include <qpe/resource.h>
#include <qpe/config.h>


namespace OpieTooth {


    #include <remotedevice.h>

    BlueBase::BlueBase( QWidget* parent,  const char* name, WFlags fl )
        : BluetoothBase( parent, name, fl ) {


        QObject::connect( (QObject*) PushButton2,  SIGNAL( clicked() ), this, SLOT(startScan()));
        QObject::connect((QObject*)configApplyButton, SIGNAL(clicked() ), this, SLOT(applyConfigChanges()));




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
}


/**
 * Get the status informations and returns it
 * @return QString the status informations gathered
 */
    QString BlueBase::getStatus(){

        return ("manger.h need also a status method");

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


    void BlueBase::addSearchedDevices( QList<RemoteDevice> &newDevices ) {

        QListViewItem * deviceItem;

        QListIterator<RemoteDevice> it(newDevices);

        for( ; it.current(); ++it ) {


            RemoteDevice *dev = it.current();
            deviceItem = new QListViewItem( ListView4, dev->name() );
            deviceItem->setText(1, dev->mac() );
        }
    }


/**
 * Open the "scan for devices"  dialog
 */
    void BlueBase::startScan() {
        ScanDialog *scan = new ScanDialog( this, "", true);
        QObject::connect((QObject*)scan, SIGNAL( selectedDevices(QList<RemoteDevice>&) ),
                         this, SLOT( addSearchedDevices(QList<RemoteDevice>& ) ));

        scan->showMaximized();
    }

/**
 * Decontructor
 */
    BlueBase::~BlueBase(){
    }

}

