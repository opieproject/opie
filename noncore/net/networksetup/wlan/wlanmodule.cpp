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

