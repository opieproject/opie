#include "mainwindowimp.h"
#include "addconnectionimp.h"
#include "interfaceinformationimp.h"
#include "interfacesetupimp.h"
#include "module.h"

#include "kprocess.h"

#include <qpushbutton.h>
#include <qtabwidget.h>
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

#include <qlist.h>
#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>

#define TEMP_ALL "/tmp/ifconfig-a"
#define TEMP_UP "/tmp/ifconfig"

MainWindowImp::MainWindowImp(QWidget *parent, const char *name) : MainWindow(parent, name, true), advancedUserMode(false){
  connect(addConnectionButton, SIGNAL(clicked()), this, SLOT(addClicked()));
  connect(removeConnectionButton, SIGNAL(clicked()), this, SLOT(removeClicked()));
  connect(informationConnectionButton, SIGNAL(clicked()), this, SLOT(informationClicked()));
  connect(configureConnectionButton, SIGNAL(clicked()), this, SLOT(configureClicked())); 
  
  connect(newProfileButton, SIGNAL(clicked()), this, SLOT(addProfile()));
  connect(removeProfileButton, SIGNAL(clicked()), this, SLOT(removeProfile()));
  connect(setCurrentProfileButton, SIGNAL(clicked()), this, SLOT(changeProfile()));

  connect(newProfile, SIGNAL(textChanged(const QString&)), this, SLOT(newProfileChanged(const QString&)));  
  // Load connections.
  loadModules(QDir::homeDirPath() + "/.networksetup/plugins");
  getInterfaceList();
  connectionList->header()->hide();


  Config cfg("NetworkSetup");
  profiles = QStringList::split(" ", cfg.readEntry("Profiles", "All"));
  for ( QStringList::Iterator it = profiles.begin(); it != profiles.end(); ++it)
    profilesList->insertItem((*it));  
  advancedUserMode = cfg.readBoolEntry("AdvancedUserMode", false);
}

/**
 * Deconstructor. Save profiles.  Delete loaded libraries.
 */
MainWindowImp::~MainWindowImp(){
  // Save profiles.
  if(profiles.count() > 1){
    Config cfg("NetworkSetup");
    cfg.setGroup("General");
    cfg.writeEntry("Profiles", profiles.join(" "));
  }
  // Delete Modules and Libraries
  QMap<Module*, QLibrary*>::Iterator it;
  for( it = libraries.begin(); it != libraries.end(); ++it ){
    delete it.key();
    delete it.data();
  }
}

/**
 * Load all modules that are found in the path
 * @param path a directory that is scaned for any plugins that can be loaded
 *  and attempts to load them
 */ 
void MainWindowImp::loadModules(QString path){
  qDebug(path.latin1());
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
Module* MainWindowImp::loadPlugin(QString pluginFileName, QString resolveString){
  qDebug(QString("MainWindowImp::loadPlugin: %1").arg(pluginFileName).latin1());
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
  list.insert("USB (PPP) / (ADD_TEST)", "A dialup connection over the USB port");
  list.insert("IrDa (PPP) / (ADD_TEST)", "A dialup connection over the IdDa port");
  for( it = libraries.begin(); it != libraries.end(); ++it ){
    if(it.key()){
      (it.key())->possibleNewInterfaces(list);
    }
  }
  // See if the list has anything that we can add.
  if(list.count() == 0){
    QMessageBox::information(this, "Sorry", "Nothing to add.", "Ok");
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
    QMessageBox::information(this, "Error","Please select an interface.", "Ok");
    return; 
  }
  
  Interface *i = interfaceItems[item];
  if(i->getModuleOwner() == NULL){
    QMessageBox::information(this, "Can't remove interface.", "Interface is built in.", "Ok");
  }
  else{
    if(!i->getModuleOwner()->remove(i))
      QMessageBox::information(this, "Error", "Unable to remove.", "Ok");
    else{
      QMessageBox::information(this, "Success", "Interface was removed.", "Ok");
      // TODO memory managment....
      // who deletes the interface?
    }
  }
}

/**
 * Pull up the configure about the currently selected interface.
 * Report an error if no interface is selected.
 * If the interface has a module owner then request its configure with a empty
 * tab.  If tab is !NULL then append the interfaces setup widget to it.
 */ 
void MainWindowImp::configureClicked(){
  QListViewItem *item = connectionList->currentItem();
  if(!item){
    QMessageBox::information(this, "Error","Please select an interface.", QMessageBox::Ok);
    return;
  } 
 
  Interface *i = interfaceItems[item];
  if(i->getModuleOwner()){
    QTabWidget *tabWidget = NULL;
    QWidget *moduleConfigure = i->getModuleOwner()->configure(&tabWidget);
    if(moduleConfigure != NULL){
      if(tabWidget != NULL){
	InterfaceSetupImp *configure = new InterfaceSetupImp(tabWidget, "InterfaceSetupImp", i, true);
        tabWidget->insertTab(configure, "TCP/IP");
      }
      moduleConfigure->showMaximized();
      moduleConfigure->show();
      return;
    }
  }
  
  InterfaceSetupImp *configure = new InterfaceSetupImp(0, "InterfaceSetupImp", i, true);
  configure->showMaximized();
  configure->show();
}

/**
 * Pull up the information about the currently selected interface.
 * Report an error if no interface is selected.
 * If the interface has a module owner then request its configure with a empty
 * tab.  If tab is !NULL then append the interfaces setup widget to it.
 */ 
void MainWindowImp::informationClicked(){
  QListViewItem *item = connectionList->currentItem();
  if(!item){
    QMessageBox::information(this, "Error","Please select an interface.", QMessageBox::Ok);
    return;
  } 
 
  Interface *i = interfaceItems[item];
  if(i->getModuleOwner()){
    QTabWidget *tabWidget = NULL;
    QWidget *moduleInformation = i->getModuleOwner()->information(&tabWidget);
    if(moduleInformation != NULL){
      if(tabWidget != NULL){
	InterfaceInformationImp *information = new InterfaceInformationImp(tabWidget, "InterfaceSetupImp", i, true);
        tabWidget->insertTab(information, "TCP/IP");
      }
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
 * Aquire the list of active interfaces from ifconfig
 * Call ifconfig and ifconfig -a
 */ 
void MainWindowImp::getInterfaceList(){
  KShellProcess *processAll = new KShellProcess();
  *processAll << "/sbin/ifconfig" << "-a" << " > " TEMP_ALL;
  connect(processAll, SIGNAL(processExited(KProcess *)),
          this, SLOT(jobDone(KProcess *)));
  threads.insert(processAll, TEMP_ALL);
  processAll->start(KShellProcess::NotifyOnExit);

  KShellProcess *process = new KShellProcess();
  *process << "/sbin/ifconfig" << " > " TEMP_UP;
  connect(process, SIGNAL(processExited(KProcess *)),
          this, SLOT(jobDone(KProcess *)));
  threads.insert(process, TEMP_UP);
  process->start(KShellProcess::NotifyOnExit);
}

void MainWindowImp::jobDone(KProcess *process){
  QString fileName = threads[process];
  threads.remove(process);
  delete process;

  QFile file(fileName);
  if (!file.open(IO_ReadOnly)){
    qDebug(QString("MainWindowImp: Can't open file: %1").arg(fileName).latin1());
    return;
  }
  
  QTextStream stream( &file );
  QString line;
  while ( !stream.eof() ) {
    line = stream.readLine();
    int space = line.find(" ");
    if(space > 1){
      // We have found an interface
      QString interfaceName = line.mid(0, space);
      if(!advancedUserMode){
        if(interfaceName == "lo")
		break;
      }
      Interface *i;
      // See if we already have it
      if(interfaceNames.find(interfaceName) == interfaceNames.end()){
        if(fileName == TEMP_ALL)
          i = new Interface(interfaceName, false);
        else
          i = new Interface(interfaceName, true);
      }
      else{
        i = interfaceNames[interfaceName];
        if(fileName != TEMP_ALL)
          i->setStatus(true);
      }
      
      i->setAttached(true);
      i->setInterfaceName(interfaceName);

      QString hardName = "Ethernet";
      int hardwareName = line.find("Link encap:");
      int macAddress = line.find("HWaddr");
      if(macAddress == -1)
        macAddress = line.length();
      if(hardwareName != -1)
        i->setHardwareName(line.mid(hardwareName+11, macAddress-(hardwareName+11)) + QString(" (%1)").arg(i->getInterfaceName()));
      // We have found an interface
      //qDebug(QString("MainWindowImp: Found Interface: %1").arg(line).latin1());
      interfaceNames.insert(i->getInterfaceName(), i);
      updateInterface(i);
    }
  }
  file.close();
  QFile::remove(fileName);
} 

/**
 * Update this interface.  If no QListViewItem exists create one.
 * @param Interface* pointer to the interface that needs to be updated.
 */ 
void MainWindowImp::updateInterface(Interface *i){
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
  // Actually try to use the Module
  if(i->getModuleOwner() != NULL)
    typeName = i->getModuleOwner()->getPixmapName(i);
  
  item->setPixmap(1, (Resource::loadPixmap(typeName)));
  item->setText(2, i->getHardwareName());
  item->setText(3, (i->getStatus()) ? i->getIp() : QString(""));
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
    QMessageBox::information(this, "Can't Add","Profile already exists.", "Ok");
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
    QMessageBox::information(this, "Can't remove anything.","Need One Profile.", "Ok");
    return;
  }
  QString profileToRemove = profilesList->currentText();
  if(QMessageBox::information(this, "Question",QString("Remove profile: %1").arg(profileToRemove), QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok){
    profiles = QStringList::split(" ", profiles.join(" ").replace(QRegExp(profileToRemove), ""));
    profilesList->clear();
    for ( QStringList::Iterator it = profiles.begin(); it != profiles.end(); ++it)
      profilesList->insertItem((*it));
  }

}

/**
 * A new profile has been selected, change.
 * @param newProfile the new profile.
 */ 
void MainWindowImp::changeProfile(){
  currentProfileLabel->setText(profilesList->text(profilesList->currentItem()));
}

// mainwindowimp.cpp

