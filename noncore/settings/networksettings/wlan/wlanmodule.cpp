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
 * get the icon name for this device.
 * @param Interface* can be used in determining the icon.
 * @return QString the icon name (minus .png, .gif etc) 
 */ 
QString WLANModule::getPixmapName(Interface* ){
  return "wlan";
}

/**
 * Check to see if the interface i is owned by this module.
 * @return bool true if i is owned by this module, false otherwise.
 */ 
bool WLANModule::isOwner(Interface *i){
  if(i->getInterfaceName() == "eth0")
    return true;
  return false;
}

/**
 * Create, set tabWiget and return the WLANConfigure Module
 * @param tabWidget a pointer to the tab widget that this configure has.
 * @return QWidget* pointer to the tab widget in this modules configure.
 */ 
QWidget *WLANModule::configure(QTabWidget **tabWidget){
  Config cfg("wireless");
  WLANImp *wlanconfig = new WLANImp(cfg);
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
 *
 */ 
QList<Interface> WLANModule::getInterfaces(){
  return list
}

/**
 * Return a list of possible new interfaces 
 */ 
QMap<QString, QString> WLANModule::possibleNewInterfaces(){
  //return list;       
}

/**
 * Attempt to add a new interface as defined by name
 * @param name the name of the type of interface that should be created given
 *  by possibleNewInterfaces();
 * @return Interface* NULL if it was unable to be created.
 */ 
Interface *WLANModule::addNewInterface(QString name){
  return NULL; 
}

/**
 * Attempts to remove the interface, doesn't delete i
 * @return bool true if successfull, false otherwise.
 */ 
bool WLANModule::remove(Interface* i){
  return false; 
}

// wlanmodule.cpp

