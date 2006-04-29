/*
 * bluebase.cpp *
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

#include "bluebase.h"
#include "scandialog.h"
#include "hciconfwrapper.h"
#include "devicehandler.h"
#include "btconnectionitem.h"
#include "rfcommassigndialogimpl.h"
#include "forwarder.h"
#include "servicesdialog.h"
#include <termios.h>
#include <string.h>
#include <errno.h>

/* OPIE */
#include <qpe/qpeapplication.h>
#include <qpe/resource.h>
#include <qpe/config.h>
#include <opie2/odebug.h>
#ifdef Q_WS_QWS
#include <qpe/qcopenvelope_qws.h>
#endif

using namespace Opie::Core;

/* QT */
#include <qframe.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qtabwidget.h>
#include <qscrollview.h>
#include <qvbox.h>
#include <qmessagebox.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qdir.h>
#include <qpopupmenu.h>
#include <qtimer.h>
#include <qlist.h>
#include <qfile.h>

/* STD */
#include <remotedevice.h>
#include <services.h>
#include <stdlib.h>

using namespace OpieTooth;
//Array of possible speeds of the serial port
struct SerSpeed {
    const char* str; //string value
    int val; //value itself
} speeds[] = { 
      { "150", B150 },   { "300", B300 },   { "600", B600 }, { "1200", B1200 }, 
    { "2400", B2400 }, { "4800", B4800 }, { "9600", B9600 }, 
    { "19200", B19200 }, { "38400", B38400 }, { "57600", B57600 }, 
    { "115200", B115200}
};

BlueBase::BlueBase( QWidget* parent,  const char* name, WFlags fl )
        : BluetoothBase( parent, name, fl )
{
    m_localDevice = new Manager( "hci0" );

    connect( PushButton2,  SIGNAL( clicked() ), this, SLOT(startScan() ) );
    connect( configApplyButton, SIGNAL(clicked() ), this, SLOT(applyConfigChanges() ) );

    connect( rfcommBindButton,  SIGNAL( clicked() ),  this,  SLOT( rfcommDialog() ) );

    connect( devicesView, SIGNAL( clicked(QListViewItem*)),
             this, SLOT( startServiceActionClicked(QListViewItem*) ) );
    connect( devicesView, SIGNAL( rightButtonClicked(QListViewItem*,const QPoint&,int) ),
             this,  SLOT(startServiceActionHold(QListViewItem*,const QPoint&,int) ) );
    connect( m_localDevice , SIGNAL( foundServices(const QString&,Services::ValueList) ),
             this, SLOT( addServicesToDevice(const QString&,Services::ValueList) ) );
    connect( m_localDevice, SIGNAL( available(const QString&,bool) ),
             this, SLOT( deviceActive(const QString&,bool) ) );
    connect( m_localDevice, SIGNAL( connections(ConnectionState::ValueList) ),
             this, SLOT( addConnectedDevices(ConnectionState::ValueList) ) );
    connect( m_localDevice, SIGNAL( signalStrength(const QString&,const QString&) ),
             this, SLOT( addSignalStrength(const QString&,const QString&) ) );
    connect(runButton, SIGNAL(clicked()), this, SLOT(doForward()));
    connect(encCheckBox, SIGNAL(toggled(bool)), this, SLOT(doEncrypt(bool)));
    connect(servicesEditButton, SIGNAL(clicked()), this, SLOT(editServices()));

    // let hold be rightButtonClicked()
    QPEApplication::setStylusOperation( devicesView->viewport(), QPEApplication::RightOnHold);
    QPEApplication::setStylusOperation( connectionsView->viewport(), QPEApplication::RightOnHold);

    //Load all icons needed
    m_offPix = Resource::loadPixmap( "opietooth/notconnected" );
    m_onPix = Resource::loadPixmap( "opietooth/connected" );
    m_findPix = Resource::loadPixmap( "opietooth/find" );

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

    devicesView->setRootIsDecorated(true);
    m_iconLoader = new BTIconLoader();
    writeToHciConfig();
    addConnectedDevices();
    readSavedDevices();
    addServicesToDevices();
    QTimer::singleShot( 3000, this, SLOT( addServicesToDevices() ) );
    forwarder = NULL;
}

/**
 * Reads all options from the config file
 */
void BlueBase::readConfig()
{

    Config cfg( "bluetoothmanager" );
    cfg.setGroup( "bluezsettings" );

    m_deviceName = cfg.readEntry( "name" , "No name" ); // name the device should identify with
    m_defaultPasskey = cfg.readEntryCrypt( "passkey" , "" ); // <- hmm, look up how good the trolls did that, maybe too weak
    m_useEncryption = cfg.readBoolEntry( "useEncryption" , TRUE );
    m_enableAuthentification = cfg.readBoolEntry( "enableAuthentification" , TRUE );
    m_enablePagescan = cfg.readBoolEntry( "enablePagescan" , TRUE );
    m_enableInquiryscan = cfg.readBoolEntry( "enableInquiryscan" , TRUE );
}

/**
 * Writes all options to the config file
 */
void BlueBase::writeConfig()
{

    Config cfg( "bluetoothmanager" );
    cfg.setGroup( "bluezsettings" );

    cfg.writeEntry( "name" , m_deviceName );
    cfg.writeEntryCrypt( "passkey" , m_defaultPasskey );
    cfg.writeEntry( "useEncryption" , m_useEncryption );
    cfg.writeEntry( "enableAuthentification" , m_enableAuthentification );
    cfg.writeEntry( "enablePagescan" , m_enablePagescan );
    cfg.writeEntry( "enableInquiryscan" , m_enableInquiryscan );

    writeToHciConfig();
}

/**
 * Modify the hcid.conf file to our needs
 */
void BlueBase::writeToHciConfig()
{
    QFile pinFile("/etc/bluetooth/pin"); // /etc/bluetooth/pin file
    owarn << "writeToHciConfig" << oendl;
    //Write /etc/bluetooth/hcid.conf file
    HciConfWrapper hciconf ( "/etc/bluetooth/hcid.conf" );
    hciconf.load();
    hciconf.setPinHelper( QPEApplication::qpeDir() + "bin/bluepin" );
    hciconf.setName( m_deviceName );
    hciconf.setEncrypt( m_useEncryption );
    hciconf.setAuth( m_enableAuthentification );
    hciconf.setPscan( m_enablePagescan );
    hciconf.setIscan( m_enableInquiryscan );
    hciconf.save();
    // Write /etc/bluetooth/pin (default PIN file)
    pinFile.open(IO_WriteOnly | IO_Truncate);
    pinFile.writeBlock(m_defaultPasskey, m_defaultPasskey.length());
    pinFile.writeBlock("\n", sizeof("\n"));
    pinFile.flush();
    pinFile.close();
}


/**
 * Read the list of already known devices
 */
void BlueBase::readSavedDevices()
{

    QValueList<RemoteDevice> loadedDevices;
    DeviceHandler handler;
    loadedDevices = handler.load();

    addSearchedDevices( loadedDevices );
}


/**
 * Write the list of already known devices
 */
void BlueBase::writeSavedDevices()
{
    QListViewItemIterator it( devicesView );
    BTListItem* item;
    BTDeviceItem* device;
    RemoteDevice::ValueList list;
    for ( ; it.current(); ++it )
    {
        item = (BTListItem*)it.current();
        if(item->typeId() != BTListItem::Device )
            continue;
        device = (BTDeviceItem*)item;

        list.append( device->remoteDevice() );
    }
    /*
     * if not empty save the List through DeviceHandler
     */
    if ( list.isEmpty() )
        return;
    DeviceHandler handler;
    handler.save( list );
}


/**
 * Set up the gui
 */
void BlueBase::initGui()
{
    StatusLabel->setText( status() ); // maybe move it to getStatus()
    cryptCheckBox->setChecked( m_useEncryption );
    authCheckBox->setChecked( m_enableAuthentification );
    pagescanCheckBox->setChecked( m_enablePagescan );
    inquiryscanCheckBox->setChecked( m_enableInquiryscan );
    deviceNameLine->setText( m_deviceName );
    passkeyLine->setText( m_defaultPasskey );
    // set info tab
    setInfo();
    serDevName->setText(tr("/dev/ircomm0"));
    for (unsigned int i = 0; i < (sizeof(speeds) / sizeof(speeds[0])); i++) {
        serSpeed->insertItem(speeds[i].str);
    }
    serSpeed->setCurrentItem((sizeof(speeds) / sizeof(speeds[0])) - 1);
    encCheckBox->setChecked(true);
}


/**
 * Get the status informations and returns it
 * @return QString the status informations gathered
 */
QString BlueBase::status()const
{
    QString infoString = tr( "<b>Device name : </b> Ipaq" );
    infoString += QString( "<br><b>" + tr( "MAC adress: " ) +"</b> No idea" );
    infoString += QString(  "<br><b>" + tr( "Class" ) + "</b> PDA" );

    return (infoString);
}


/**
 * Read the current values from the gui and invoke writeConfig()
 */
void BlueBase::applyConfigChanges()
{
    m_deviceName =  deviceNameLine->text();
    m_defaultPasskey = passkeyLine->text();
    m_useEncryption = cryptCheckBox->isChecked();
    m_enableAuthentification = authCheckBox->isChecked();
    m_enablePagescan = pagescanCheckBox->isChecked();
    m_enableInquiryscan = inquiryscanCheckBox->isChecked();

    writeConfig();

    QMessageBox::information( this, tr("Test") , tr("Changes were applied.") );
}

/**
 * Launch Rfcomm Bind dialog
 *
 */
void BlueBase::rfcommDialog()
{
    RfcommAssignDialog rfcommAssign ( this, "RfcommAssignDialog", true, WStyle_ContextHelp  );

    if ( QPEApplication::execDialog( &rfcommAssign ) == QDialog::Accepted )
    {
        rfcommAssign.saveConfig();
    }
}

/**
 * Add fresh found devices from scan dialog to the listing
 *
 */
void BlueBase::addSearchedDevices( const QValueList<RemoteDevice> &newDevices )
{
    BTDeviceItem * deviceItem;
    QValueList<RemoteDevice>::ConstIterator it;

    for( it = newDevices.begin(); it != newDevices.end() ; ++it )
    {

        if (find( (*it)  )) // is already inserted
            continue;

        deviceItem = new BTDeviceItem( devicesView , (*it) );
        deviceItem->setPixmap( 1, m_findPix );
        deviceItem->setExpandable ( true );

        // look if device is avail. atm, async
        deviceActive( (*it) );
    }
}


/**
 * Action that is toggled on entrys on click
 */
void BlueBase::startServiceActionClicked( QListViewItem */*item*/ )
{}


/**
 * Action that are toggled on hold (mostly QPopups i guess)
 */
void BlueBase::startServiceActionHold( QListViewItem * item, const QPoint & point, int /*column*/ )
{
    if (!item )
        return;

    QPopupMenu *menu = new QPopupMenu();

    if ( static_cast<BTListItem*>( item )->type() == "device")
    {
        QPopupMenu *groups = new QPopupMenu();

        menu->insertItem( static_cast<BTDeviceItem*>( item )->name(), 0 );
        menu->insertSeparator( 1 );
        menu->insertItem( tr( "&Rescan services" ), 2);
        // menu->insertItem( tr( "&Add to group" ), groups, 3);
        menu->insertItem( tr( "&Delete"),  4);
        int ret = menu->exec( point, 0);

        switch(ret)
        {
            case -1:
                break;
            case 2:
                addServicesToDevice( static_cast<BTDeviceItem*>( item ) );
                break;

            case 4:
                // deletes childs too
                delete item;
                break;
        }
        // delete groups;
    }

    /*
     * We got service sensitive PopupMenus in our factory
     * We will create one through the factory and will insert
     * our Separator + ShowInfo into the menu or create a new
     * one if the factory returns 0
     * PopupMenu deletion is kind of weird.
     * If escaped( -1 ) or any of our items were chosen we'll
     * delete the PopupMenu otherwise it's the responsibility of
     * the PopupMenu to delete itself
     *
     */
    else if ( ((BTListItem*)item)->type() == "service")
    {
        BTServiceItem* service = (BTServiceItem*)item;
        QMap<int, QString> list = service->services().classIdList();
        QMap<int, QString>::Iterator it = list.begin();
        QPopupMenu *popup =0l;
        if ( it != list.end() )
        {
            owarn << "Searching id " << it.key() << " " << it.data().latin1() << "" << oendl;
            popup = m_popHelper.find( it.key(),
                                      service->services(),
                                      (BTDeviceItem*)service->parent() );
        }
        else
        {
            owarn << "Empty" << oendl;
        }

        if ( popup == 0l )
        {
            owarn << "factory returned 0l" << oendl;
            popup = new QPopupMenu();
        }
        int test1 = popup->insertItem( tr("Test1:"),  2);

        int ret = popup->exec( point );
        owarn << "returned from exec() " << oendl;
        if ( ret == -1 )
        {
            ;
        }
        else if ( ret == test1 )
        {
            ;
        }
        delete popup;
    }
    delete menu;
}


void BlueBase::addServicesToDevices()
{
    odebug << "BlueBase::addServicesToDevices()" << oendl;
    BTDeviceItem* item = (BTDeviceItem*) devicesView->firstChild();
    while ( item )
    {
        addServicesToDevice( item );
        item = (BTDeviceItem*) static_cast<QListViewItem*>( item )->nextSibling();
    }
}

/**
 * Search and display avail. services for a device (on expand from device listing)
 * @param item the service item returned
 */
void BlueBase::addServicesToDevice( BTDeviceItem * item )
{
    odebug << "BlueBase::addServicesToDevice" << oendl;
    // row of mac adress text(3)
    RemoteDevice device = item->remoteDevice();
    m_deviceList.insert( item->mac() , item );
    // and some time later I get a signal foundServices( const QString& device, Services::ValueList ); back
    m_localDevice->searchServices( device );
}


/**
 * @param device the mac address of the remote device
 * @param servicesList the list with the service the device has.
 */
void BlueBase::addServicesToDevice( const QString& device, Services::ValueList servicesList )
{
    odebug << "BlueBase::fill services list" << oendl;

    QMap<QString,BTDeviceItem*>::Iterator it;
    BTDeviceItem* deviceItem = 0;

    // get the right devices which requested the search
    it = m_deviceList.find( device );
    if( it == m_deviceList.end() )
        return;
    deviceItem = it.data();

    // remove previous entries
    QList<QListViewItem> tempList;
    tempList.setAutoDelete( true );
    QListViewItem * child = deviceItem->firstChild();
    while( child )
    {
        tempList.append( child );
        child = child->nextSibling();
    }
    tempList.clear();

    QValueList<OpieTooth::Services>::Iterator it2;
    BTServiceItem* serviceItem;


    if (!servicesList.isEmpty() )
    {
        // add services
        QMap<int, QString> list;
        QMap<int, QString>::Iterator classIt;
        for( it2 = servicesList.begin(); it2 != servicesList.end(); ++it2 )
        {
            serviceItem = new BTServiceItem( deviceItem, (*it2)  );
            list = (*it2).classIdList();
            classIt = list.begin();
            int classId=0;
            if ( classIt != list.end() )
            {
                classId = classIt.key();
            }

            serviceItem->setPixmap( 0, m_iconLoader->serviceIcon( classId ) );
        }
    }
    else
    {
        Services s1;
        s1.setServiceName( tr("no services found") );
        serviceItem = new BTServiceItem( deviceItem, s1 );
    }
    // now remove them from the  list
    m_deviceList.remove( it );
}

void BlueBase::addSignalStrength()
{

    QListViewItemIterator it( connectionsView );
    for ( ; it.current(); ++it )
    {
        m_localDevice->signalStrength( ((BTConnectionItem*)it.current() )->connection().mac() );
    }

    QTimer::singleShot( 5000, this, SLOT( addSignalStrength() ) );
}

void BlueBase::addSignalStrength( const QString& mac, const QString& strength )
{

    QListViewItemIterator it( connectionsView );
    for ( ; it.current(); ++it )
    {
        if( ((BTConnectionItem*)it.current())->connection().mac() == mac )
        {
            ((BTConnectionItem*)it.current() )->setSignalStrength( strength );
        }
    }
}

/**
 * Add the existing connections (pairs) to the connections tab.
 * This one triggers the search
 */
void BlueBase::addConnectedDevices()
{
    m_localDevice->searchConnections();
    QTimer::singleShot( 5000, this, SLOT( addSignalStrength() ) );
}

/**
 * This adds the found connections to the connection tab.
 * @param connectionList the ValueList with all current connections
 */
void BlueBase::addConnectedDevices( ConnectionState::ValueList connectionList )
{

    QValueList<OpieTooth::ConnectionState>::Iterator it;
    BTConnectionItem * connectionItem;

    if ( !connectionList.isEmpty() )
    {

        for (it = connectionList.begin(); it != connectionList.end(); ++it)
        {

            QListViewItemIterator it2( connectionsView );
            bool found = false;
            for ( ; it2.current(); ++it2 )
            {
                if( ( (BTConnectionItem*)it2.current())->connection().mac() == (*it).mac() )
                {
                    found = true;
                }
            }

            if ( found == false )
            {
                connectionItem = new BTConnectionItem( connectionsView, (*it) );

                if( m_deviceList.find((*it).mac()).data() )
                {
                    connectionItem->setName( m_deviceList.find( (*it).mac()).data()->name() );
                }
            }

        }

        QListViewItemIterator it2( connectionsView );
        for ( ; it2.current(); ++it2 )
        {
            bool found = false;
            for (it = connectionList.begin(); it != connectionList.end(); ++it)
            {
                if( ( ((BTConnectionItem*)it2.current())->connection().mac() ) == (*it).mac() )
                {
                    found = true;
                }
            }

            if ( !found )
            {
                delete it2.current();
            }

        }


    }
    else
    {
        connectionsView->clear();
        ConnectionState con;
        con.setMac( tr("No connections found") );
        connectionItem = new BTConnectionItem( connectionsView , con );
    }

    //  recall connection search after some time
    QTimer::singleShot( 15000, this, SLOT( addConnectedDevices() ) );
}


/**
 * Find out if a device can  currently be reached
 * @param device
 */
void BlueBase::deviceActive( const RemoteDevice &device )
{
    // search by mac, async, gets a signal back
    // We should have a BTDeviceItem there or where does it get added to the map -zecke
    m_localDevice->isAvailable( device.mac() );
}


/**
 * The signal catcher. Set the avail. status on device.
 * @param device - the mac address
 * @param connected - if it is avail. or not
 */
void BlueBase::deviceActive( const QString& device, bool connected  )
{
    odebug << "deviceActive slot" << oendl;

    QMap<QString,BTDeviceItem*>::Iterator it;

    it = m_deviceList.find( device );
    if( it == m_deviceList.end() )
        return;

    BTDeviceItem* deviceItem = it.data();

    if ( connected )
    {
        deviceItem->setPixmap( 1, m_onPix );
    }
    else
    {
        deviceItem->setPixmap( 1, m_offPix );
    }
    m_deviceList.remove( it );
}


/**
 * Open the "scan for devices"  dialog
 */
void BlueBase::startScan()
{
    ScanDialog *scan = new ScanDialog( this, "ScanDialog",
                                       true, WDestructiveClose );
    QObject::connect( scan, SIGNAL( selectedDevices(const QValueList<RemoteDevice>&) ),
                      this, SLOT( addSearchedDevices(const QValueList<RemoteDevice>&) ) );

    QPEApplication::showDialog( scan );
}


/**
 * Set the informations about the local device in information Tab
 */
void BlueBase::setInfo()
{
    StatusLabel->setText( status() );
}


/**
 * Decontructor
 */
BlueBase::~BlueBase()
{
    writeSavedDevices();
    if (forwarder) {
#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
        QCopEnvelope("QPE/System", "setScreenSaverMode(int)" ) 
            << QPEApplication::Enable;
#endif
        delete forwarder;
        forwarder = NULL;
    }
    delete m_iconLoader;
}


/**
 * find searches the ListView for a BTDeviceItem containig
 * the same Device if found return true else false
 * @param dev RemoteDevice to find
 * @return returns true if found
 */
bool BlueBase::find( const RemoteDevice& rem )
{
    QListViewItemIterator it( devicesView );
    BTListItem* item;
    BTDeviceItem* device;
    for (; it.current(); ++it )
    {
        item = (BTListItem*) it.current();
        if ( item->typeId() != BTListItem::Device )
            continue;

        device = (BTDeviceItem*)item;
        if ( rem.equals( device->remoteDevice() ) )
            return true;
    }
    return false; // not found
}

/**
 * Start process of the cell phone forwarding
 */
void BlueBase::doForward()
{
    if (forwarder && forwarder->isRunning()) {
        runButton->setText("start gateway");
        forwarder->stop();
        delete forwarder;
        forwarder = NULL;
        return;
    }
    QString str = serDevName->text();
    forwarder = new SerialForwarder(str, speeds[serSpeed->currentItem()].val);
    connect(forwarder, SIGNAL(processExited(Opie::Core::OProcess*)), 
        this, SLOT(forwardExited(Opie::Core::OProcess*)));
    if (forwarder->start(OProcess::NotifyOnExit) < 0) {
        QMessageBox::critical(this, tr("Forwarder Error"), 
            tr("Forwarder start error:") + tr(strerror(errno)));
        return;
    }
    runButton->setText("stop gateway");
#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
    QCopEnvelope("QPE/System", "setScreenSaverMode(int)")
            << QPEApplication::DisableSuspend;
#endif
}

/**
 * React on the process end
 */
void BlueBase::forwardExit(Opie::Core::OProcess* proc)
{
#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
    QCopEnvelope("QPE/System", "setScreenSaverMode(int)" ) 
            << QPEApplication::Enable;
#endif
    if (proc->exitStatus() != 0)
        QMessageBox::critical(this, tr("Forwarder Error"), 
            tr("Forwarder start error"));
    delete proc;
    forwarder = NULL;
    runButton->setText("start gateway");
}

/**
 * Encrypt entered passkey
 * doit - do encryption of the key
 */
void BlueBase::doEncrypt(bool doit)
{
    passkeyLine->setEchoMode((doit)? QLineEdit::Password: QLineEdit::Normal);
}

/**
 * Start services edit dialog
 */
void BlueBase::editServices()
{
    QString conf = "/etc/default/bluetooth";
//// Use for debugging purposes    
////    QString conf = "/mnt/net/opie/bin/bluetooth";
    ServicesDialog svcEdit(conf, this, "ServicesDialog", true, 
        WStyle_ContextHelp);

    if (QPEApplication::execDialog(&svcEdit) == QDialog::Accepted)
    {
    }
}

//eof
