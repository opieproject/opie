/*
 * bluebase.cpp *
 * ---------------------
 *
 * copyright   : (c) 2002 by Maximilian Rei�
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
#include <qpopupmenu.h>

#include <qpe/resource.h>
#include <qpe/config.h>

#include <remotedevice.h>


namespace OpieTooth {


    BlueBase::BlueBase( QWidget* parent,  const char* name, WFlags fl )
        : BluetoothBase( parent, name, fl ) {

        localDevice = new Manager( "hci0" );

        connect( PushButton2,  SIGNAL( clicked() ), this, SLOT(startScan() ) );
        connect( configApplyButton, SIGNAL(clicked() ), this, SLOT(applyConfigChanges() ) );
        // not good since lib is async
        //       connect( ListView2, SIGNAL( expanded ( QListViewItem* ) ),
        //                  this, SLOT( addServicesToDevice( QListViewItem * ) ) );
        connect( ListView2, SIGNAL( clicked( QListViewItem* )),
                          this, SLOT( startServiceActionClicked( QListViewItem* ) ) );
        connect( ListView2, SIGNAL( rightButtonClicked( QListViewItem *, const QPoint &, int ) ),
                 this,  SLOT(startServiceActionHold( QListViewItem *, const QPoint &, int) ) );
        connect( localDevice , SIGNAL( foundServices( const QString& , Services::ValueList ) ),
                this, SLOT( addServicesToDevice( const QString& , Services::ValueList ) ) );


        //Load all icons needed

        offPix = Resource::loadPixmap( "editdelete" );
        onPix = Resource::loadPixmap( "installed" );

        QPalette pal = this->palette();
        QColor col = pal.color( QPalette::Active, QColorGroup::Background );
        pal.setColor( QPalette::Active, QColorGroup::Button, col );
        pal.setColor( QPalette::Inactive, QColorGroup::Button, col );
        pal.setColor( QPalette::Normal, QColorGroup::Button, col );
        pal.setColor( QPalette::Disabled, QColorGroup::Button, col );
        this->setPalette( pal );

        setCaption( tr( "Bluetooth Manager" ) );

        readConfig();
        initGui();

        //TESTING
        ListView2->setRootIsDecorated(true);

        QListViewItem *topLV2 = new QListViewItem( ListView2, "Siemens S45" , "no" );
        topLV2->setPixmap( 1, onPix );
        topLV2->setText(4,  "device" );
        (void) new QListViewItem( topLV2, "Serial"  );
        (void) new QListViewItem( topLV2, "BlueNiC" );
    }


    /**
     * Reads all options from the config file
     */
    void BlueBase::readConfig() {

        Config cfg( "bluetoothmanager" );
        cfg.setGroup( "bluezsettings" );


        deviceName = cfg.readEntry( "name" , "No name" ); // name the device should identify with
        defaultPasskey = cfg.readEntryCrypt( "passkey" , "" ); // <- hmm, look up how good the trolls did that, maybe too weak
        useEncryption = cfg.readNumEntry( "useEncryption" , 1 );
        enableAuthentification = cfg.readNumEntry( "enableAuthentification" , 1 );
        enablePagescan = cfg.readNumEntry( "enablePagescan" , 1 );
        enableInquiryscan = cfg.readNumEntry( "enableInquiryscan" , 1 );

    }

    /**
     * Writes all options to the config file
     */
    void BlueBase::writeConfig() {


        Config cfg( "bluetoothmanager" );
        cfg.setGroup( "bluezsettings" );

        cfg.writeEntry( "name" , deviceName );
        cfg.writeEntryCrypt( "passkey" , defaultPasskey );
        cfg.writeEntry( "useEncryption" , useEncryption );
        cfg.writeEntry( "enableAuthentification" , enableAuthentification );
        cfg.writeEntry( "enablePagescan" , enablePagescan );
        cfg.writeEntry( "enableInquiryscan" , enableInquiryscan );
}


    /**
     * Read the list of allready known devices
     *
     */
    void BlueBase::readSavedDevices() {

        QList<RemoteDevice> *loadedDevices = new QList<RemoteDevice>;

        QDir deviceListSave( QDir::homeDirPath() + "/Settings/bluetooth/");
        // list of .conf files
        QStringList devicesFileList = deviceListSave.entryList();

        // cut .conf of to get the mac and also read the name entry in it.

        for ( QStringList::Iterator it = devicesFileList.begin(); it != devicesFileList.end(); ++it ) {

            QString name;
            QString mac;
            qDebug((*it).latin1() );
            Config conf((*it));
            conf.setGroup("Info");
            name = conf.readEntry("name", "Error");
            qDebug("MAC: " + mac);
            qDebug("NAME: " + name);
            RemoteDevice currentDevice = RemoteDevice( mac , name  );
            loadedDevices->append( &currentDevice );
        }
        addSearchedDevices( *loadedDevices );
    }

    /**
     * Write the list of allready known devices
     *
     */
    void BlueBase::writeSavedDevices() {

        QListViewItemIterator it( ListView2 );

        for ( ; it.current(); ++it ) {

            // seperate config file for each device, to store more information in future.

            Config conf( QDir::homeDirPath() + "/Settings/bluetooth/" + (it.current()->text(3)) + ".conf", Config::File );
            conf.setGroup( "Info" );
            conf.writeEntry( "name", it.current()->text(0) );
        }
    }


    /**
     * Set up the gui
     */
    void BlueBase::initGui() {

        StatusLabel->setText( getStatus() ); // maybe move it to getStatus()

        cryptCheckBox->setChecked( useEncryption );
        authCheckBox->setChecked( enableAuthentification );
        pagescanCheckBox->setChecked( enablePagescan );
        inquiryscanCheckBox->setChecked( enableInquiryscan );
        deviceNameLine->setText( deviceName );
        passkeyLine->setText( defaultPasskey );
        // set info tab
        setInfo();
    }


    /**
     * Get the status informations and returns it
     * @return QString the status informations gathered
     */
    QString BlueBase::getStatus(){

        QString infoString = tr( "<b>Device name : </b> Ipaq" );
        infoString += QString( "<br><b>" + tr( "MAC adress: " ) +"</b> No idea" );
        infoString += QString( "<br><b>" + tr( "Class" ) + "</b> PDA" );

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

        QMessageBox*  box = new QMessageBox( this, "Test" );
        box->setText( tr( "Changes applied" ) );
        box->show();
  // falls n�tig hcid killhupen - die funktionalit�t adden
    }


    /**
     * Add fresh found devices from scan dialog to the listing
     *
     */
    void BlueBase::addSearchedDevices( QList<RemoteDevice> &newDevices ) {

        QListViewItem * deviceItem;

        QListIterator<RemoteDevice> it( newDevices );

        for( ; it.current() ; ++it ) {


            RemoteDevice *dev = it.current();
            deviceItem = new QListViewItem( ListView2 , dev->name() );
            deviceItem->setExpandable ( true );

            if ( deviceActive( dev ) ) {
                deviceItem->setPixmap( 1 , onPix );
            } else {
                deviceItem->setPixmap( 1, offPix );
            }

            deviceItem->setText( 3, dev->mac() );
            // what kind of entry is it.
            deviceItem->setText( 4, "device");

            // ggf auch hier?
            addServicesToDevice( deviceItem );
        }
    }


    /**
     * Action that is toggled on entrys on click
     */
    void BlueBase::startServiceActionClicked( QListViewItem *item ) {


    }

    /**
     * Action that are toggled on hold (mostly QPopups i guess)
     */
    void BlueBase::startServiceActionHold( QListViewItem * item, const QPoint & point, int column ) {

          QPopupMenu *menu = new QPopupMenu();
          QPopupMenu *groups = new QPopupMenu();
          int ret=0;

          //QSize s = menu->sizeHint ( );

          if ( item->text(4) == "device") {
              menu->insertItem( tr("rescan sevices:"),  0);
              menu->insertItem( tr("to group"), groups ,  1);
              menu->insertItem( tr("delete"),  2);
          } else if ( item->text(4) == "service") {
              menu->insertItem( tr("Test1:"),  0);
              menu->insertItem( tr("connect"), 1);
              menu->insertItem( tr("delete"),  2);
          }

          ret = menu->exec( point  , 0);

// noch differenzieren
          switch(ret) {
          case 0:
              break;
          case 1:
              break;
          case 2:
              // delete childs too
              delete item;
              break;
          }

          delete menu;
          delete groups;
    }

    /**
     * Search and display avail. services for a device (on expand from device listing)
     *
     */
    void BlueBase::addServicesToDevice( QListViewItem * item ) {

        qDebug("addServicesToDevice");
        // row of mac adress text(3)
        RemoteDevice *device = new RemoteDevice( item->text(3),  item->text(0) );

        deviceList.insert( item->text(3) ,  item );

        // and some time later I get a signal foundServices( const QString& device, Services::ValueList ); back
        localDevice->searchServices( *device );

        // delete
    }


    /**
     * Overloaded. This one it the one that is connected to the foundServices signal
     * @param device the mac address of the remote device
     * @param servicesList the list with the service the device has.
     */
    void BlueBase::addServicesToDevice( const QString& device, Services::ValueList servicesList ) {

        qDebug("fill services list");

        QMap<QString,QListViewItem*>::Iterator it;

        QListViewItem* deviceItem;

        // get the right devices which requested the search
        for( it = deviceList.begin(); it != deviceList.end(); ++it ) {
            if ( it.key() == device ) {
                deviceItem = it.data();
            }
        }


        // empty entries
        //  QListViewItem * myChild = deviceItem->firstChild();
        //QList<QListViewItem*> tmpList;
        //while( myChild ) {
        //    tmpList.append(myChild);
        //    myChild = myChild->nextSibling();
        // }


        QValueList<OpieTooth::Services>::Iterator it2;

        QListViewItem * serviceItem;

        if (!servicesList.isEmpty() ) {
            // add services
            for( it2 = servicesList.begin(); it2 != servicesList.end(); ++it2 ) {
                serviceItem = new QListViewItem( deviceItem  ,  (*it2).serviceName()   );
                serviceItem->setText(4, "service");
            }
        } else {
            serviceItem = new QListViewItem( deviceItem  , tr("no services found")  );
            serviceItem->setText(4, "service");
        }
    }


    /**
     * Add the existing connections (pairs) to the connections tab.
     *
     */
    void BlueBase::addConnectedDevices() {


        //mac address

    }

    /**
     * Find out if a device can  currently be reached
     */
    bool BlueBase::deviceActive( RemoteDevice *device ) {

        // search by mac
       localDevice->isAvailable( device->mac() );

        return true;
    }


    /**
     * Open the "scan for devices"  dialog
     */
    void BlueBase::startScan() {

        ScanDialog *scan = new ScanDialog( this, "", true);
        QObject::connect( scan, SIGNAL( selectedDevices( QList<RemoteDevice>& ) ),
                         this, SLOT( addSearchedDevices( QList<RemoteDevice>& ) ) );

        scan->showMaximized();
    }


    /**
     * Set the informations about the local device in information Tab
     */
    void BlueBase::setInfo() {
        StatusLabel->setText( getStatus() );
    }

    /**
     * Decontructor
     */
    BlueBase::~BlueBase() {
        writeSavedDevices();
    }
}

