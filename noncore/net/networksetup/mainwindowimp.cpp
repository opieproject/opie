#include "mainwindowimp.h"
#include "addserviceimp.h"
#include "interfaceinformationimp.h"
#include "interfacesetupimp.h"

#include <qpushbutton.h>
#include <qdir.h>
#include <qtabwidget.h>
#include <qmainwindow.h>
#include <qmessagebox.h>
#include <qlistbox.h>
#include <qpe/config.h>
#include <qpe/qlibrary.h>
#include <qpe/resource.h>
#include <qlist.h>

#include <qfile.h>
#include <qtextstream.h>
#include <qlistview.h>
#include <qheader.h>
// For library loading.
#include <dlfcn.h>

#define TEMP_ALL "/tmp/ifconfig-a"
#define TEMP_UP "/tmp/ifconfig"

#define NO_SELECT_ERROR_AND_RETURN { \
  QMessageBox::information(this, "Error","Please select an interface.", "Ok"); \
  return; \
}

MainWindowImp::MainWindowImp(QWidget *parent, const char *name) : MainWindow(parent, name, true) {
  connect(addServiceButton, SIGNAL(clicked()), this, SLOT(addClicked()));
  connect(removeServiceButton, SIGNAL(clicked()), this, SLOT(removeClicked()));
  connect(informationServiceButton, SIGNAL(clicked()), this, SLOT(informationClicked()));
  connect(configureServiceButton, SIGNAL(clicked()), this, SLOT(configureClicked())); 
  // Make sure we have a plugin directory to scan.
  QString DirStr = QDir::homeDirPath() + "/.networksetup/" ;
  QDir pluginDir( DirStr );
  pluginDir.mkdir( DirStr );
  pluginDir.mkdir( ( DirStr + "plugins/" ) );
  QString path = DirStr + "plugins";
  pluginDir.setPath(path);
  if(!pluginDir.exists()){
    qDebug(QString("MainWindowImp: ERROR: %1 %2").arg(__FILE__).arg(__LINE__).latin1());
    return;
  }

  // Load any saved services.
  loadModules(path);
  getInterfaceList();
  serviceList->header()->hide();
}

/**
 * Deconstructor.  Unload libraries and save.
 */
MainWindowImp::~MainWindowImp(){
}

void MainWindowImp::loadModules(QString path){
}

/**
 * Attempt to load a function and resolve a function.
 * @param pluginFileName - the name of the file in which to attempt to load
 * @param resolveString - function pointer to resolve 
 * @return pointer to the function with name resolveString or NULL
 */ 
Module* MainWindowImp::loadPlugin(QString pluginFileName, QString resolveString){
  //qDebug(pluginFileName.latin1());
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

  // Store for reference
  libraries.insert(object, lib);
  return object;
}

/*
QList<QString> MainWindowImp::retrieveUnloadedPluginList(){
  QString DirStr = QDir::homeDirPath() + "/.networksetup/" ;
  QString path = DirStr + "plugins";
  QDir d(path);
  d.setFilter( QDir::Files | QDir::Hidden );

  QMap<QObject*, QLibrary*>::Iterator libraryIt;
  QList<QString> rlist; 
  rlist.setAutoDelete(false);
	  
  const QFileInfoList *list = d.entryInfoList();
  QFileInfoListIterator it( *list );
  QFileInfo *fi;
  while ( (fi=it.current()) ) {
    if(fi->fileName().contains(".so")){
      for( libraryIt = libraries.begin(); libraryIt != libraries.end(); ++libraryIt )
        if((path + "/" + fi->fileName()) != (libraryIt.data())->library()){
          QString *s = new QString(path + "/" + fi->fileName());
          rlist.append(s);
	}
    }
    ++it;
  }
  return rlist;
}
*/

/**
 * The Add button was clicked.  Bring up the add dialog and if OK is hit
 * load the plugin and append it to the list
 */ 
void MainWindowImp::addClicked(){
  // Now that we have a list of all of the protocals, list them.
  {
    QMessageBox::information(this, "No Modules", "Nothing to add.", "Ok");
    return;
  }
  AddServiceImp service(this, "AddService", true);
  service.showMaximized();
  service.exec();
}

/**
 * Prompt the user to see if they really want to do this.
 * If they do then remove from the list and unload.
 */ 
void MainWindowImp::removeClicked(){
  QListViewItem *item = serviceList->currentItem();
  if(item == NULL) NO_SELECT_ERROR_AND_RETURN
  
  if(modules.find(interfaceItems[item]) == modules.end()){
    QMessageBox::information(this, "Can't remove interface.", "Interface is built in.", "Ok");
  }
  else{
    // Try to remove. 
  }

}

/**
 * See if there is a configuration for the selected protocal.
 * Prompt with errors.
 */ 
void MainWindowImp::configureClicked(){
  QListViewItem *item = serviceList->currentItem();
  if(item == NULL) NO_SELECT_ERROR_AND_RETURN
  
  if(modules.find(interfaceItems[item]) == modules.end()){
    InterfaceSetupImp *conf = new InterfaceSetupImp(0, "InterfaceConfiguration", interfaceItems[item]);
    conf->showMaximized();
    conf->show();
  }
  else{
    InterfaceSetupImp *conf = new InterfaceSetupImp(this, "InterfaceConfiguration");
    conf->show();
  }
}

/**
 * Pull up the information about the selected interface
 * Report an error
 */ 
void MainWindowImp::informationClicked(){
  QListViewItem *item = serviceList->currentItem();
  if(item == NULL)NO_SELECT_ERROR_AND_RETURN
	
  if(modules.find(interfaceItems[item]) == modules.end()){
    InterfaceInformationImp *i = new InterfaceInformationImp(0, "InterfaceInformationImp", interfaceItems[item]);
    i->showMaximized();
    i->show();
  }
  else{
    QTabWidget *t = new QTabWidget(this, "InterfaceInformationTAB");
    InterfaceInformationImp *i = new InterfaceInformationImp(t, "TCPIPInformation", interfaceItems[item], true);
    t->insertTab(i, "TCP/IP");
    t->show();
  }
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
        i->setHardwareName(line.mid(hardwareName+11, macAddress-(hardwareName+11)));
      // We have found an interface
      //qDebug(QString("MainWindowImp: Found Interface: %1").arg(line).latin1());
      interfaceNames.insert(i->getInterfaceName(), i);
      updateInterface(i);
    }
  }
  file.close();
  QFile::remove(fileName);
} 

void MainWindowImp::updateInterface(Interface *i){
  QListViewItem *item = NULL;
  
  // See if we already have it
  if(items.find(i) == items.end()){
    item = new QListViewItem(serviceList, "", "", "");
    // See if you can't find a module owner for this interface
    //EmployeeMap::Iterator it;
    //for( it = map.begin(); it != map.end(); ++it )
    //        printf( "%s, %s earns %d\n", it.key().latin1(), it.data().name().latin1(), it.data().salary() );

    items.insert(i, item);
    interfaceItems.insert(item, i);
  }
  else
    item = items[i];
  
  QString statusImage = "down";
  if(i->getStatus())
    statusImage = "up";
  QPixmap status = (Resource::loadPixmap(statusImage));
  item->setPixmap(0, status);
 
  QString typeName = "lan";
  if(i->getHardwareName().contains("Local Loopback"))
    typeName = "lo";
  QPixmap type = (Resource::loadPixmap(typeName));
  item->setPixmap(1, type);

  item->setText(2, i->getHardwareName());
  
}

void MainWindowImp::addProfile(){

}

void MainWindowImp::removeProfile(){

}

// mainwindowimp.cpp

