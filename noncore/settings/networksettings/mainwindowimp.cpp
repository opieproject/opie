#include "mainwindowimp.h"
#include "addconnectionimp.h"
#include "interfaceinformationimp.h"
#include "interfacesetupimp.h"
#include "interfaces.h"
#include "module.h"

#include <qpushbutton.h>
#include <qlistbox.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qheader.h>
#include <qlabel.h>

#include <qmainwindow.h>
#include <qmessagebox.h>

#include <qpe/config.h>
#include <qpe/qlibrary.h>
#include <qpe/resource.h>
#include <qpe/qpeapplication.h>

#include <qlist.h>
#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>

#include <net/if.h>
#include <sys/ioctl.h>

#define DEFAULT_SCHEME "/var/lib/pcmcia/scheme"

MainWindowImp::MainWindowImp(QWidget *parent, const char *name) : MainWindow(parent, name, true), advancedUserMode(false), scheme(DEFAULT_SCHEME){
  connect(addConnectionButton, SIGNAL(clicked()), this, SLOT(addClicked()));
  connect(removeConnectionButton, SIGNAL(clicked()), this, SLOT(removeClicked()));
  connect(informationConnectionButton, SIGNAL(clicked()), this, SLOT(informationClicked()));
  connect(configureConnectionButton, SIGNAL(clicked()), this, SLOT(configureClicked())); 
  
  connect(newProfileButton, SIGNAL(clicked()), this, SLOT(addProfile()));
  connect(removeProfileButton, SIGNAL(clicked()), this, SLOT(removeProfile()));
  connect(setCurrentProfileButton, SIGNAL(clicked()), this, SLOT(changeProfile()));

  connect(newProfile, SIGNAL(textChanged(const QString&)), this, SLOT(newProfileChanged(const QString&)));  
  // Load connections.
  loadModules(QPEApplication::qpeDir() + "/plugins/networksetup");
  getAllInterfaces();
  
  Interfaces i;
  QStringList list = i.getInterfaceList();
  QMap<QString, Interface*>::Iterator it;
  for ( QStringList::Iterator ni = list.begin(); ni != list.end(); ++ni ) {
    bool found = false;
    for( it = interfaceNames.begin(); it != interfaceNames.end(); ++it ){
      if(it.key() == (*ni))
        found = true;
    }
    if(!found){
      if(!(*ni).contains("_")){
        Interface *i = new Interface(this, *ni, false);
        i->setAttached(false);
        i->setHardwareName("Disconnected");
        interfaceNames.insert(i->getInterfaceName(), i);
        updateInterface(i);
        connect(i, SIGNAL(updateInterface(Interface *)), this, SLOT(updateInterface(Interface *)));
      }
    }
  }
  
  //getInterfaceList();
  connectionList->header()->hide();

  Config cfg("NetworkSetup");
  profiles = QStringList::split(" ", cfg.readEntry("Profiles", "All"));
  for ( QStringList::Iterator it = profiles.begin(); it != profiles.end(); ++it)
    profilesList->insertItem((*it));  
  currentProfileLabel->setText(cfg.readEntry("CurrentProfile", "All"));
  advancedUserMode = cfg.readBoolEntry("AdvancedUserMode", false);
  scheme = cfg.readEntry("SchemeFile", DEFAULT_SCHEME);

  QFile file(scheme);
  if ( file.open(IO_ReadOnly) ) {    // file opened successfully
    QTextStream stream( &file );        // use a text stream
    while ( !stream.eof() ) {        // until end of file...
      QString line = stream.readLine();       // line of text excluding '\n'
      if(line.contains("SCHEME")){
        line = line.mid(7, line.length());
        currentProfileLabel->setText(line);
        break;
      }
    }
    file.close();
  }
}

/**
 * Deconstructor. Save profiles.  Delete loaded libraries.
 */
MainWindowImp::~MainWindowImp(){
  // Save profiles.
  Config cfg("NetworkSetup");
  cfg.setGroup("General");
  cfg.writeEntry("Profiles", profiles.join(" "));
  
  // Delete all interfaces that don't have owners.
  QMap<Interface*, QListViewItem*>::Iterator iIt;
  for( iIt = items.begin(); iIt != items.end(); ++iIt ){
    if(iIt.key()->getModuleOwner() == NULL)
      delete iIt.key();
  }
  
  // Delete Modules and Libraries
  QMap<Module*, QLibrary*>::Iterator it;
  for( it = libraries.begin(); it != libraries.end(); ++it ){
    delete it.key();
    // I wonder why I can't delete the libraries
    // What fucking shit this is.
    //delete it.data();
  }
}

/**
 * Query the kernel for all of the interfaces.
 */ 
void MainWindowImp::getAllInterfaces(){
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(sockfd == -1)
    return;
  
  char buf[8*1024];
  struct ifconf ifc;
  ifc.ifc_len = sizeof(buf);
  ifc.ifc_req = (struct ifreq *) buf;
  int result=ioctl(sockfd, SIOCGIFCONF, &ifc);

  for (char* ptr = buf; ptr < buf + ifc.ifc_len; ){
    struct ifreq *ifr =(struct ifreq *) ptr;
    int len = sizeof(struct sockaddr);
#ifdef  HAVE_SOCKADDR_SA_LEN
    if (ifr->ifr_addr.sa_len > len)
      len = ifr->ifr_addr.sa_len;    /* length > 16 */
#endif
    ptr += sizeof(ifr->ifr_name) + len;  /* for next one in buffer */

    int flags;
    struct sockaddr_in *sinptr;
    Interface *i = NULL;
    switch (ifr->ifr_addr.sa_family){
      case AF_INET:
        sinptr = (struct sockaddr_in *) &ifr->ifr_addr;
        flags=0;

        struct ifreq ifcopy;
        ifcopy=*ifr;
        result=ioctl(sockfd,SIOCGIFFLAGS,&ifcopy);
        flags=ifcopy.ifr_flags;
        i = new Interface(this, ifr->ifr_name, false);
	i->setAttached(true);
        if ((flags & IFF_UP) == IFF_UP)
          i->setStatus(true);
        else
          i->setStatus(false);

        if ((flags & IFF_BROADCAST) == IFF_BROADCAST)
          i->setHardwareName("Ethernet");
        else if ((flags & IFF_POINTOPOINT) == IFF_POINTOPOINT)
          i->setHardwareName("Point to Point");
        else if ((flags & IFF_MULTICAST) == IFF_MULTICAST)
          i->setHardwareName("Multicast");
        else if ((flags & IFF_LOOPBACK) == IFF_LOOPBACK)
          i->setHardwareName("Loopback");
        else
          i->setHardwareName("Unknown");

        interfaceNames.insert(i->getInterfaceName(), i);
        updateInterface(i);
        connect(i, SIGNAL(updateInterface(Interface *)), this, SLOT(updateInterface(Interface *)));
	break;

      default:
	qDebug(ifr->ifr_name);
	qDebug(QString("%1").arg(ifr->ifr_addr.sa_family).latin1());
        break;
    }
  }
}

/**
 * Load all modules that are found in the path
 * @param path a directory that is scaned for any plugins that can be loaded
 *  and attempts to load them
 */ 
void MainWindowImp::loadModules(const QString &path){
  //qDebug(path.latin1());
  QDir d(path);
  if(!d.exists())
    return;

  // Don't want sym links
  d.setFilter( QDir::Files | QDir::NoSymLinks );
  const QFileInfoList *list = d.entryInfoList();
  QFileInfoListIterator it( *list );
  QFileInfo *fi;
  while ( (fi=it.current()) ) {
    if(fi->fileName().contains(".so")){
      loadPlugin(path + "/" + fi->fileName());
    }
    ++it;
  }
}

/**
 * Attempt to load a function and resolve a function.
 * @param pluginFileName - the name of the file in which to attempt to load
 * @param resolveString - function pointer to resolve 
 * @return pointer to the function with name resolveString or NULL
 */ 
Module* MainWindowImp::loadPlugin(const QString &pluginFileName, const QString &resolveString){
  //qDebug(QString("MainWindowImp::loadPlugin: %1").arg(pluginFileName).latin1());
  QLibrary *lib = new QLibrary(pluginFileName);
  void *functionPointer = lib->resolve(resolveString);
  if( !functionPointer ){
    qDebug(QString("MainWindowImp: File: %1 is not a plugin, but though was.").arg(pluginFileName).latin1());
    delete lib;
    return NULL;
  }
  
  // Try to get an object.
  Module *object = ((Module* (*)()) functionPointer)();
  if(object == NULL){
    qDebug("MainWindowImp: Couldn't create object, but did load library!");
    delete lib;
    return NULL;
  }

  // Store for deletion later
  libraries.insert(object, lib);
  return object;
}

/**
 * The Add button was clicked.  Bring up the add dialog and if OK is hit
 * load the plugin and append it to the list
 */ 
void MainWindowImp::addClicked(){
  QMap<Module*, QLibrary*>::Iterator it;
  QMap<QString, QString> list;
  QMap<QString, Module*> newInterfaceOwners;
  //list.insert("USB (PPP) / (ADD_TEST)", "A dialup connection over the USB port");
  //list.insert("IrDa (PPP) / (ADD_TEST)", "A dialup connection over the IdDa port");
  for( it = libraries.begin(); it != libraries.end(); ++it ){
    if(it.key()){
      (it.key())->possibleNewInterfaces(list);
    }
  }
  // See if the list has anything that we can add.
  if(list.count() == 0){
    QMessageBox::information(this, "Sorry", "Nothing to add.", QMessageBox::Ok);
    return;
  }
  AddConnectionImp addNewConnection(this, "AddConnectionImp", true);
  addNewConnection.addConnections(list);
  addNewConnection.showMaximized();
  if(QDialog::Accepted == addNewConnection.exec()){
    QListViewItem *item = addNewConnection.registeredServicesList->currentItem();
    if(!item)
      return;
    
    for( it = libraries.begin(); it != libraries.end(); ++it ){
      if(it.key()){
        Interface *i = (it.key())->addNewInterface(item->text(0));
        if(i){
          interfaceNames.insert(i->getInterfaceName(), i);
          updateInterface(i);
        }
      }
    }
  }
}

/**
 * Prompt the user to see if they really want to do this.
 * If they do then remove from the list and unload.
 */ 
void MainWindowImp::removeClicked(){
  QListViewItem *item = connectionList->currentItem();
  if(!item) {
    QMessageBox::information(this, "Sorry","Please select an interface First.", QMessageBox::Ok);
    return; 
  }
  
  Interface *i = interfaceItems[item];
  if(i->getModuleOwner() == NULL){
    QMessageBox::information(this, "Can't remove interface.", "Interface is built in.", QMessageBox::Ok);
  }
  else{
    if(!i->getModuleOwner()->remove(i))
      QMessageBox::information(this, "Error", "Unable to remove.", QMessageBox::Ok);
    else{
      QMessageBox::information(this, "Success", "Interface was removed.", QMessageBox::Ok);
      // TODO memory managment....
      // who deletes the interface?
    }
  }
}

/**
 * Pull up the configure about the currently selected interface.
 * Report an error if no interface is selected.
 * If the interface has a module owner then request its configure.
 */ 
void MainWindowImp::configureClicked(){
  QListViewItem *item = connectionList->currentItem();
  if(!item){
    QMessageBox::information(this, "Sorry","Please select an interface first.", QMessageBox::Ok);
    return;
  } 

  Interface *i = interfaceItems[item];
  if(i->getModuleOwner()){
    QWidget *moduleConfigure = i->getModuleOwner()->configure(i);
    if(moduleConfigure != NULL){
      moduleConfigure->showMaximized();
      moduleConfigure->show();
      return;
    }
  }
  
  InterfaceSetupImpDialog *configure = new InterfaceSetupImpDialog(0, "InterfaceSetupImp", i, true, Qt::WDestructiveClose);
  QString currentProfileText = currentProfileLabel->text();
  if(currentProfileText.upper() == "ALL");
    currentProfileText = "";
  configure->setProfile(currentProfileText);
  configure->showMaximized();
  configure->show();
}

/**
 * Pull up the information about the currently selected interface.
 * Report an error if no interface is selected.
 * If the interface has a module owner then request its configure.
 */ 
void MainWindowImp::informationClicked(){
  QListViewItem *item = connectionList->currentItem();
  if(!item){
    QMessageBox::information(this, "Sorry","Please select an interface First.", QMessageBox::Ok);
    return;
  } 
 
  Interface *i = interfaceItems[item];
  if(!i->isAttached()){
    QMessageBox::information(this, "Sorry","No information about\na disconnected interface.", QMessageBox::Ok);
    return;
  }

  if(i->getModuleOwner()){
    QWidget *moduleInformation = i->getModuleOwner()->information(i);
    if(moduleInformation != NULL){
      moduleInformation->showMaximized();
      moduleInformation->show();
      return;
    }
  }  
  InterfaceInformationImp *information = new InterfaceInformationImp(0, "InterfaceSetupImp", i, true);
  information->showMaximized();
  information->show();
}

/**
 * Update this interface.  If no QListViewItem exists create one.
 * @param Interface* pointer to the interface that needs to be updated.
 */ 
void MainWindowImp::updateInterface(Interface *i){
  if(!advancedUserMode){
    if(i->getInterfaceName() == "lo")
      return;
  }
  
  QListViewItem *item = NULL;
  
  // Find the interface, making it if needed.
  if(items.find(i) == items.end()){
    item = new QListViewItem(connectionList, "", "", "");
    // See if you can't find a module owner for this interface
    QMap<Module*, QLibrary*>::Iterator it;
    for( it = libraries.begin(); it != libraries.end(); ++it ){
      if(it.key()->isOwner(i))
        i->setModuleOwner(it.key());
    }
    items.insert(i, item);
    interfaceItems.insert(item, i);
  }
  else
    item = items[i];
 
  // Update the icons and information 
  item->setPixmap(0, (Resource::loadPixmap(i->getStatus() ? "up": "down")));
 
  QString typeName = "lan";
  if(i->getHardwareName().contains("Local Loopback"))
    typeName = "lo";
  if(i->getInterfaceName().contains("irda"))
    typeName = "irda";
  if(i->getInterfaceName().contains("wlan"))
    typeName = "wlan";
  if(i->getInterfaceName().contains("usb"))
    typeName = "usb";
  
  if(!i->isAttached())
    typeName = "connect_no";
  // Actually try to use the Module
  if(i->getModuleOwner() != NULL)
    typeName = i->getModuleOwner()->getPixmapName(i);
  
  item->setPixmap(1, (Resource::loadPixmap(typeName)));
  item->setText(2, i->getHardwareName());
  item->setText(3, QString("(%1)").arg(i->getInterfaceName()));
  item->setText(4, (i->getStatus()) ? i->getIp() : QString(""));
}

void MainWindowImp::newProfileChanged(const QString& newText){
  if(newText.length() > 0)
   newProfileButton->setEnabled(true);
  else
    newProfileButton->setEnabled(false);
}

/**
 * Adds a new profile to the list of profiles.
 * Don't add profiles that already exists.
 * Appends to the list and QStringList
 */ 
void MainWindowImp::addProfile(){
  QString newProfileName = newProfile->text();
  if(profiles.grep(newProfileName).count() > 0){
    QMessageBox::information(this, "Can't Add","Profile already exists.", QMessageBox::Ok);
    return;
  }
  profiles.append(newProfileName);
  profilesList->insertItem(newProfileName);
}

/**
 * Removes the currently selected profile in the combo.
 * Doesn't delete if there are less then 2 profiles.
 */
void MainWindowImp::removeProfile(){
  if(profilesList->count() <= 1){
    QMessageBox::information(this, "Can't remove.","At least one profile\nis needed.", QMessageBox::Ok);
    return;
  }
  QString profileToRemove = profilesList->currentText();
  if(profileToRemove == "All"){
    QMessageBox::information(this, "Can't remove.","Can't remove default.", QMessageBox::Ok);
    return;
  }
  // Can't remove the curent profile
  if(profileToRemove == currentProfileLabel->text()){
    QMessageBox::information(this, "Can't remove.",QString("%1 is the current profile.").arg(profileToRemove), QMessageBox::Ok);
    return;

  }
  
  if(QMessageBox::information(this, "Question",QString("Remove profile: %1").arg(profileToRemove), QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok){
    profiles = QStringList::split(" ", profiles.join(" ").replace(QRegExp(profileToRemove), ""));
    profilesList->clear();
    for ( QStringList::Iterator it = profiles.begin(); it != profiles.end(); ++it)
      profilesList->insertItem((*it));

    // Remove any interface settings and mappings.
    Interfaces interfaces;
    // Go through them one by one
    QMap<Interface*, QListViewItem*>::Iterator it;
    for( it = items.begin(); it != items.end(); ++it ){
      QString interfaceName = it.key()->getInterfaceName();
      qDebug(interfaceName.latin1());
      if(interfaces.setInterface(interfaceName + "_" + profileToRemove)){
       interfaces.removeInterface();
       if(interfaces.setMapping(interfaceName)){
         if(profilesList->count() == 1)
            interfaces.removeMapping();
          else{
            interfaces.removeMap("map", interfaceName + "_" + profileToRemove);
          }
        }
        interfaces.write();
        break;
      }
    }
  }
}

/**
 * A new profile has been selected, change.
 * @param newProfile the new profile.
 */ 
void MainWindowImp::changeProfile(){
  if(profilesList->currentItem() == -1){
    QMessageBox::information(this, "Can't Change.","Please select a profile.", QMessageBox::Ok);
    return;
  }
  QString newProfile = profilesList->text(profilesList->currentItem());
  if(newProfile != currentProfileLabel->text()){
    currentProfileLabel->setText(newProfile);
    QFile::remove(scheme);
    QFile file(scheme);
    if ( file.open(IO_ReadWrite) ) {
      QTextStream stream( &file );
      stream << QString("SCHEME=%1").arg(newProfile);
      file.close();
    }
    // restart all up devices?
    if(QMessageBox::information(this, "Question","Restart all running interfaces?", QMessageBox::Ok, QMessageBox::No) == QMessageBox::Ok){
      // Go through them one by one
      QMap<Interface*, QListViewItem*>::Iterator it;
      for( it = items.begin(); it != items.end(); ++it ){
        if(it.key()->getStatus() == true)
          it.key()->restart();
      }
    }
  }
  // TODO change the profile in the modules
}

// mainwindowimp.cpp

