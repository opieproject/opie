#include "wlanmodule.h"
#include "wlanimp.h"
#include "infoimp.h"
#include "wextensions.h"
#include "interfaceinformationimp.h"

#include <qlabel.h>
#include <qprogressbar.h>
#include <qtabwidget.h>

/**
 * Constructor, find all of the possible interfaces
 */
WLANModule::WLANModule() : Module() {
}

/**
 * Delete any interfaces that we own.
 */
WLANModule::~WLANModule(){
  Interface *i;
  for ( i=list.first(); i != 0; i=list.next() )
    delete i;
}

/**
 * Change the current profile
 */
void WLANModule::setProfile(const QString &newProfile){
  profile = newProfile;
}

/**
 * get the icon name for this device.
 * @param Interface* can be used in determining the icon.
 * @return QString the icon name (minus .png, .gif etc)
 */
QString WLANModule::getPixmapName(Interface* ){
  return "wlan";
}

/**
 * Check to see if the interface i is owned by this module.
 * @param Interface* interface to check against
 * @return bool true if i is owned by this module, false otherwise.
 */
bool WLANModule::isOwner(Interface *i){
  WExtensions we(i->getInterfaceName());
  if(!we.doesHaveWirelessExtensions())
    return false;

  i->setHardwareName("802.11b");
  list.append(i);
  return true;
}

/**
 * Create, and return the WLANConfigure Module
 * @return QWidget* pointer to this modules configure.
 */
QWidget *WLANModule::configure(Interface *i){
  WLANImp *wlanconfig = new WLANImp(0, "WlanConfig", i, false,  Qt::WDestructiveClose);
  wlanconfig->setProfile(profile);
  return wlanconfig;
}

/**
 * Create, and return the Information Module
 * @return QWidget* pointer to this modules info.
 */
QWidget *WLANModule::information(Interface *i){
  WExtensions we(i->getInterfaceName());
  if(!we.doesHaveWirelessExtensions())
    return NULL;

  WlanInfoImp *info = new WlanInfoImp(0, i->getInterfaceName(), Qt::WDestructiveClose);
  InterfaceInformationImp *information = new InterfaceInformationImp(info->tabWidget, "InterfaceSetupImp", i);
  info->tabWidget->insertTab(information, "TCP/IP");
  return info;
}

/**
 * Get all active (up or down) interfaces
 * @return QList<Interface> A list of interfaces that exsist that havn't
 * been called by isOwner()
 */
QList<Interface> WLANModule::getInterfaces(){
  return list;
}

/**
 * Attempt to add a new interface as defined by name
 * @param name the name of the type of interface that should be created given
 *  by possibleNewInterfaces();
 * @return Interface* NULL if it was unable to be created.
 */
Interface *WLANModule::addNewInterface(const QString &){
  // We can't add a 802.11 interface, either the hardware will be there
  // or it wont.
  return NULL;
}

/**
 * Attempts to remove the interface, doesn't delete i
 * @return bool true if successfull, false otherwise.
 */
bool WLANModule::remove(Interface*){
  // Can't remove a hardware device, you can stop it though.
  return false;
}

void WLANModule::receive(const QCString &param, const QByteArray &arg)
{
    qDebug("WLANModule::receive "+param);
    QStringList params = QStringList::split(",",param);
    int count = params.count();
    qDebug("got %i params", count );
    if (count < 2){
        qDebug("Erorr less than 2 parameter");
        qDebug("RETURNING");
        return;
    }

    QDataStream stream(arg,IO_ReadOnly);
    QString interface;
    QString action;

    stream >> interface;
    stream >> action;
    qDebug("got interface %s and acion %s", interface.latin1(), action.latin1());

    if (count == 2){
        // those should call the interface
        if ( action.contains("start" ) ){
            qDebug("starting %s not yet implemented",interface.latin1());
        } else if ( action.contains("restart" ) ){
            qDebug("restarting %s not yet implemented",interface.latin1());
        } else if ( action.contains("stop" ) ){
            qDebug("stopping %s not yet implemented",interface.latin1());
        }
    }else if (count == 3){
        QString value;
        stream >> value;
        qDebug("setting %s of %s to %s", action.latin1(), interface.latin1(), value.latin1() );
    }
    // if (param.contains("QString,QString,QString")) {
//         QDataStream stream(arg,IO_ReadOnly);
//         QString arg1, arg2, arg3;
//         stream >> arg1 >> arg2 >> arg3 ;
//         qDebug("interface >%s< setting >%s< value >%s<",arg1.latin1(),arg2.latin1(),arg3.latin1());
//     }
}

