#include "pppmodule.h"
#include "pppimp.h"
#include "interfaceinformationimp.h"

/**
 * Constructor, find all of the possible interfaces
 */ 
PPPModule::PPPModule() : Module() {
}

/**
 * Delete any interfaces that we own.
 */ 
PPPModule::~PPPModule(){
  Interface *i;
  for ( i=list.first(); i != 0; i=list.next() )
    delete i;
}

/**
 * Change the current profile
 */ 
void PPPModule::setProfile(const QString &newProfile){
  profile = newProfile;
}

/**
 * get the icon name for this device.
 * @param Interface* can be used in determining the icon.
 * @return QString the icon name (minus .png, .gif etc) 
 */ 
QString PPPModule::getPixmapName(Interface* ){
  return "ppp";
}

/**
 * Check to see if the interface i is owned by this module.
 * @param Interface* interface to check against
 * @return bool true if i is owned by this module, false otherwise.
 */ 
bool PPPModule::isOwner(Interface *i){
  if(!i->getInterfaceName().upper().contains("PPP"))
    return false;
  
  i->setHardwareName("PPP");
  list.append(i);
  return true;
}

/**
 * Create, and return the WLANConfigure Module
 * @return QWidget* pointer to this modules configure.
 */ 
QWidget *PPPModule::configure(Interface *i){
  return NULL;
  //PPPConfigureImp *pppconfig = new PPPConfigureImp(0, "PPPConfig", i, false,  Qt::WDestructiveClose);
  //pppconfig->setProfile(profile);
  //return wlanconfig;
}

/**
 * Create, and return the Information Module
 * @return QWidget* pointer to this modules info.
 */ 
QWidget *PPPModule::information(Interface *i){
  // We don't have any advanced pppd information widget yet :-D
  // TODO ^
  InterfaceInformationImp *information = new InterfaceInformationImp(0, "InterfaceSetupImp", i);
  return information;
}

/**
 * Get all active (up or down) interfaces
 * @return QList<Interface> A list of interfaces that exsist that havn't
 * been called by isOwner()
 */
QList<Interface> PPPModule::getInterfaces(){
  // List all of the files in the peer directory
  return list;
}

/**
 * Attempt to add a new interface as defined by name
 * @param name the name of the type of interface that should be created given
 *  by possibleNewInterfaces();
 * @return Interface* NULL if it was unable to be created.
 */ 
Interface *PPPModule::addNewInterface(const QString &newInterface){
  // If the 
  PPPConfigureImp imp(0, "PPPConfigImp");
  if(imp.exec() == QDialog::Accepted ){

  }
  return NULL; 
}

/**
 * Attempts to remove the interface, doesn't delete i
 * @return bool true if successfull, false otherwise.
 */ 
bool PPPModule::remove(Interface*){
  // Can't remove a hardware device, you can stop it though.
  return false; 
}

// pppmodule.cpp

