#include "wlanmodule.h"
#include "wlanimp.h"
#include "info.h"
#include "wextensions.h"

#include <qlabel.h>
#include <qprogressbar.h>

/**
 * Constructor, find all of the possible interfaces
 */ 
WLANModule::WLANModule() : Module() {
}

/**
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
 * Create, set tabWiget and return the WLANConfigure Module
 * @param tabWidget a pointer to the tab widget that this configure has.
 * @return QWidget* pointer to the tab widget in this modules configure.
 */ 
QWidget *WLANModule::configure(Interface *, QTabWidget **tabWidget){
  WLANImp *wlanconfig = new WLANImp(0, "WlanConfig");
  (*tabWidget) = wlanconfig->tabWidget;
  return wlanconfig;
}

/**
 * Create, set tabWiget and return the Information Module
 * @param tabWidget a pointer to the tab widget that this information has.
 * @return QWidget* pointer to the tab widget in this modules info.
 */ 
QWidget *WLANModule::information(Interface *i, QTabWidget **tabWidget){
  WExtensions we(i->getInterfaceName());
  if(!we.doesHaveWirelessExtensions())
    return NULL;
 
  WlanInfo *info = new WlanInfo(0, "wireless info", Qt::WDestructiveClose);
  (*tabWidget) = info->tabWidget;
  
  info->essidLabel->setText(we.essid());
  info->apLabel->setText(we.ap());
  info->stationLabel->setText(we.station());
  info->modeLabel->setText(we.mode());
  info->freqLabel->setText(QString("%1 GHz").arg(we.frequency()));
  int signal = 0;
  int noise = 0;
  int quality = 0;
  we.stats(signal, noise, quality);
  info->signalProgressBar->setProgress(signal);
  info->noiseProgressBar->setProgress(noise);
  info->qualityProgressBar->setProgress(quality);
  info->rateLabel->setText(QString("%1 Mb/s").arg(we.rate()));
  //WlanInfo info (0, "wireless info", true);
  //info.show();
  //return NULL;
  
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

