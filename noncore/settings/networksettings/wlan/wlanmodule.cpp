#include "wlanmodule.h"
#include <qpe/config.h>
#include "wlanimp.h"

/**
 * Constructor, find all of the possible interfaces
 */ 
WLANModule::WLANModule() : Module() {
  // get output from iwconfig
}

/**
 * Change the current profile
 */ 
void WLANModule::setProfile(QString newProfile){
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
  if(i->getInterfaceName() == "eth0" || i->getInterfaceName() == "wlan0"){
    i->setHardwareName(QString("802.11b (%1)").arg(i->getInterfaceName()));
    return true;
  }
  return false;
}

/**
 * Create, set tabWiget and return the WLANConfigure Module
 * @param tabWidget a pointer to the tab widget that this configure has.
 * @return QWidget* pointer to the tab widget in this modules configure.
 */ 
QWidget *WLANModule::configure(QTabWidget **tabWidget){
  Config *cfg = new Config("wireless");
  WLANImp *wlanconfig = new WLANImp(*cfg);
  (*tabWidget) = wlanconfig->tabWidget;
  return wlanconfig;
}

/**
 * Create, set tabWiget and return the Information Module
 * @param tabWidget a pointer to the tab widget that this information has.
 * @return QWidget* pointer to the tab widget in this modules info.
 */ 
QWidget *WLANModule::information(QTabWidget **tabWidget){
  return NULL;
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
Interface *WLANModule::addNewInterface(QString ){
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

// wlanmodule.cpp

