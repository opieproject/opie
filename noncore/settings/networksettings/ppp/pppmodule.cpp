
#include "pppconfig.h"
#include "pppmodule.h"
#include "pppdata.h"
#include "interfaceinformationppp.h"
#include "interfaceppp.h"


/**
 * Constructor, find all of the possible interfaces
 */
PPPModule::PPPModule() : Module()
{
    QMap<QString,QString> ifaces = PPPData::getConfiguredInterfaces();
    QMap<QString,QString>::Iterator it;
    InterfacePPP *iface;
    qDebug("getting interfaces");
    for( it = ifaces.begin(); it != ifaces.end(); ++it ){
        qDebug("ifaces %s", it.key().latin1());
        iface = new InterfacePPP( 0, it.key() );
        iface->setHardwareName( it.data() );
        list.append( (Interface*)iface );
    }
}

/**
 * Delete any interfaces that we own.
 */
PPPModule::~PPPModule(){
    QMap<QString,QString> ifaces;
    Interface *i;
    for ( i=list.first(); i != 0; i=list.next() ){
        ifaces.insert( i->getInterfaceName(), i->getHardwareName() );
        delete i;
    }
    PPPData::setConfiguredInterfaces( ifaces );
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
    return list.find( i ) != -1;
}

/**
 * Create, and return the WLANConfigure Module
 * @return QWidget* pointer to this modules configure.
 */
QWidget *PPPModule::configure(Interface *i){
    qDebug("return ModemWidget");
    PPPConfigWidget *pppconfig = new PPPConfigWidget( (InterfacePPP*)i,
                                                      0, "PPPConfig", false,
                                                      Qt::WDestructiveClose );
    return pppconfig;
}

/**
 * Create, and return the Information Module
 * @return QWidget* pointer to this modules info.
 */
QWidget *PPPModule::information(Interface *i){
  // We don't have any advanced pppd information widget yet :-D
  // TODO ^
    qDebug("return PPPModule::information");
//  InterfaceInformationImp *information = new InterfaceInformationImp( 0, "InterfaceSetupImp", i);
   InterfaceInformationPPP *information = new InterfaceInformationPPP( 0, "InterfaceInformationPPP", i );
  return information;
}

/**
 * Get all active (up or down) interfaces
 * @return QList<Interface> A list of interfaces that exsist that havn't
 * been called by isOwner()
 */
QList<Interface> PPPModule::getInterfaces(){
  // List all of the files in the peer directory
    qDebug("PPPModule::getInterfaces");
  return list;
}

/**
 * Attempt to add a new interface as defined by name
 * @param name the name of the type of interface that should be created given
 *  by possibleNewInterfaces();
 * @return Interface* NULL if it was unable to be created.
 */
Interface *PPPModule::addNewInterface(const QString &newInterface){

  qDebug("try to add iface %s",newInterface.latin1());

  InterfacePPP *ifaceppp;
  Interface *iface;
  ifaceppp = new InterfacePPP();
  PPPConfigWidget imp(ifaceppp, 0, "PPPConfigImp", true);
  imp.showMaximized();
  if(imp.exec() == QDialog::Accepted ){
      iface->setModuleOwner( this );
      iface = ifaceppp;
      list.append( iface );
      return iface;
  }else {
      delete iface;
      iface = NULL;
  }
  return iface;
}

/**
 * Attempts to remove the interface, doesn't delete i
 * @return bool true if successfull, false otherwise.
 */
bool PPPModule::remove(Interface*){
  // Can't remove a hardware device, you can stop it though.
  return false;
}

void PPPModule::possibleNewInterfaces(QMap<QString, QString> &newIfaces)
{
    newIfaces.insert(QObject::tr("PPP") ,
                     QObject::tr("generic ppp device"));
}


