
#include <stdlib.h>

#include <qdir.h>
#include <qpe/config.h>

#include "devicehandler.h"

using namespace OpieTooth;

DeviceHandler::DeviceHandler() {

};
DeviceHandler::~DeviceHandler() {

}

RemoteDevice::ValueList DeviceHandler::load() {
    RemoteDevice::ValueList list;
QString path = QDir::homeDirPath() + "/Settings/bluetooth";
    QDir deviceListSave( path);

    // list of .conf files
    QStringList devicesFileList = deviceListSave.entryList();


    // cut .conf of to get the mac and also read the name entry in it.
    if (!devicesFileList.isEmpty() ) {
        QString name;
        QString mac;
        QStringList::Iterator it;
        for (it = devicesFileList.begin(); it != devicesFileList.end(); ++it ) {
            if ( (*it) == "." || (*it) == ".." )
                continue;

            qDebug((*it).latin1() );
            Config conf(path + "/"+(*it),  Config::File);
            conf.setGroup("Info");
            name = conf.readEntry("name", "Error");
            mac = conf.readEntry("mac", QString::null);
            qDebug("MAC: " + mac);
            qDebug("NAME: " + name);
            if (mac.isEmpty() )
                continue;
            RemoteDevice currentDevice( mac , name  );
            list.append( currentDevice );
        }
    }
    return list;
};
/*
 * This is some how rude but make sure all old devices
 * are getting deleted
 */
void DeviceHandler::save( const RemoteDevice::ValueList& list) {
    QCString rm;
    rm += "rm -rf ";
    rm += QDir::homeDirPath() + "/Settings/bluetooth";
    system ( rm.data() );

    if (list.isEmpty() ) // no need to create the dir
        return;

    /**
     * Create a new dir
     */
    rm = "mkdir ";
    rm += QDir::homeDirPath() + "/Settings/bluetooth";
    qWarning("out %s",  rm.data() );
    system( rm.data() );

    RemoteDevice::ValueList::ConstIterator it;
    // write the config

    for ( it = list.begin(); it != list.end(); ++it ) {
        qDebug( "/Settings/bluetooth/" + (*it).mac() + ".conf");

        Config conf( QDir::homeDirPath() +
                     "/Settings/bluetooth/" +
                     (*it).mac() + ".conf", Config::File );

        conf.setGroup( "Info" );
        conf.writeEntry( "name", (*it).name() );
        conf.writeEntry( "mac", (*it).mac() );
    }

}
