#include "tabmanager.h"
#include "app.h"
#include "wait.h"
#include "tabapplnk.h"

#include <qpe/applnk.h>
#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qlistview.h>
#include <qheader.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <stdlib.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/qpeapplication.h>
#include <qpe/resource.h>


#define HOME_APP_DIR QPEApplication::qpeDir()+"/apps"
#define HOME_APP_INSTALL_DIR "/usr/lib/ipkg/info"
#define NEW_FOLDER "EmptyTab"
#define NEW_APPLICATION "NewApp"
#define APPLICATION_EXTENSION ".desktop"
#define APPLICATION_EXTENSION_LENGTH 8

/**
 * Constructor.  Sets up signals.  Performs initial scan of applications
 * and tabs
 */
TabManager::TabManager( QWidget* parent, const char* name):TabManagerBase(parent, name), changed(false), application(NULL){
  rescanFolder(HOME_APP_DIR);

  // Connect the signals and slots
  connect(tabList, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(editItem(QListViewItem*)));
  (tabList->header())->hide();
  connect(tabList, SIGNAL(moveItem(QListViewItem *, QListViewItem *)), this, SLOT(moveApplication(QListViewItem *, QListViewItem *)));
}

/**
 * If anything in the tab's have been changed then update the system or alert
 * the user.
 */
TabManager::~TabManager(){
  if(changed){
    // Prompt.
    //int answer = QMessageBox::warning(this, "Message", "Should your desktop be","Yes", "Cancel", 0, 1 );
    //if (answer)
    //  return;
    QCopEnvelope e("QPE/System", "linkChanged(QString)");
    QString link; //we'll just send an empty string
    e << link;
  }
}

/**
 * Scans root directory for any tabs or applications.  Will recursivly go down,
 * but will not follow symlinks.
 * @param directory - the directory to look in.
 * @param parent - the parent to place any new tabs or apps into.  If parent is
 * NULL then the item is a tab and should be placed as a child of the window.
 */
void TabManager::rescanFolder(QString directory, QListViewItem* parent){
  //qDebug(QString("rescanFolder: ") + directory.latin1());
  
  QDir d;
  d.setPath(directory);
  // Show hidden files for .directories
  d.setFilter( QDir::Files | QDir::Hidden | QDir::Dirs);

  const QFileInfoList *list = d.entryInfoList();
  QFileInfoListIterator it( *list );      // create list iterator
  QFileInfo *fi;                          // pointer for traversing

  while ( (fi=it.current()) ) {           // for each file...
    // If it is a dir and not .. or . then add it as a tab and go down.
    if(fi->isDir()){
      if(fi->fileName() != ".." && fi->fileName() != ".") {
        QListViewItem* newItem;
	if(!parent)
	  newItem = new QListViewItem(tabList, fi->fileName());
	else
	  newItem = new QListViewItem(parent, fi->fileName());
	itemList.insert(newItem, directory + "/" + fi->fileName() + "/.directory" );
	rescanFolder(directory + "/" + fi->fileName(), newItem);
      }
    }
    else{
      // it is a file, if not a .directory add to parent.
      
      // Change parents name and icon to reflect icon.
      if(fi->fileName() == ".directory"){
	AppLnk app(directory + "/" + fi->fileName());
	if(parent){
          parent->setPixmap(0,app.pixmap());
          parent->setText(0, app.name());
        }
      }
      else{
	// Add any desktop files found.
        QListViewItem* newItem;
	if(directory != HOME_APP_DIR){
	  if(!parent)
            newItem = new QListViewItem(tabList, fi->fileName());
          else
            newItem = new QListViewItem(parent, fi->fileName());
          if(fi->fileName().right(APPLICATION_EXTENSION_LENGTH) == APPLICATION_EXTENSION){
            AppLnk app(directory + "/" + fi->fileName());
            newItem->setPixmap(0,app.pixmap());
            newItem->setText(0, app.name());
	    itemList.insert(newItem, directory + "/" + fi->fileName());
	  }
	}
      }
    }
    ++it;                               // goto next list element
  }
}

/**
 * Create a new blank Tab.
 * Create a physical folder with .directory file
 * Create a item on the list
 */
void TabManager::newFolder(){
  QDir r;
  r.mkdir(QString(HOME_APP_DIR) + "/" + NEW_FOLDER);
  system((QString("echo [Desktop Entry] | cat >> ") +  HOME_APP_DIR + "/" + NEW_FOLDER "/.directory").latin1());
  system((QString("echo Name=" NEW_FOLDER " | cat >> ") +  HOME_APP_DIR + "/" + NEW_FOLDER "/.directory").latin1());
 
  QString homeLocation = QString(HOME_APP_DIR) + "/" + NEW_FOLDER + "/.directory";
  QListViewItem *newItem = new QListViewItem(tabList, NEW_FOLDER);
  itemList.insert(newItem, homeLocation );
 
  // We have changed something.
  changed = true;
}

/**
 * Create a new blank application
 * Make sure a tab is selected
 * create physical file
 * fill file with default information (entry, name, type).
 */
void TabManager::newApplication(){
  QListViewItem *item = tabList->currentItem();
  if(!item || item->parent())
    return;
 
  QString parentDir = itemList[item].mid(0,itemList[item].length()-11); 
  QString homeLocation = parentDir + "/" NEW_APPLICATION APPLICATION_EXTENSION;
  system((QString("echo [Desktop Entry] | cat >> ") +  homeLocation).latin1());
  system((QString("echo Name=" NEW_APPLICATION " | cat >> ") +  homeLocation).latin1());
  int slash = parentDir.findRev('/', -1);
  QString folderName = parentDir.mid(slash+1, parentDir.length());
  
  system((QString("echo Type=") + folderName + " | cat >> " +  homeLocation).latin1());
  
  // Insert into the tree
  QListViewItem *newItem = new QListViewItem(item, NEW_APPLICATION);
  itemList.insert(newItem, homeLocation );
 
  // We have changed something.
  changed = true;
}

/**
 * Remove the item.
 * Check if we can
 * Prompt user
 * Delete physical file (Dir, remove .dir, then dir.  File, remove file)
 * Remove from installer if need too.
 */ 
void TabManager::removeItem(){
  // Make sure we can delete
  QListViewItem *item = tabList->currentItem();
  if(!item)
    return;
  if(item->childCount() > 0){
    QMessageBox::critical(this, tr("Message"), tr("Can't remove with applications\nstill in the group."), tr("Ok") );
    return;	  
  }
  
  // Prompt.
  int answer = QMessageBox::warning(this, tr("Message"), tr("Are you sure you want to delete?"), tr("Yes"), tr("Cancel"), 0, 1 );
  if (answer)
    return;
  
  bool removeSuccessfull = true;
  QString location = itemList[item];
  // Remove file (.directory in a Directory case)
  if(!QFile::remove(location))
      removeSuccessfull = false;
  
  // Remove directory
  if(item->parent() == NULL){
    // Remove .directory file string
    location = location.mid(0,location.length()-10);
    QDir dir;
    if(!dir.rmdir(location))
      removeSuccessfull = false;
    else
      removeSuccessfull = true;
  }
 
  // If removing failed. 
  if(!removeSuccessfull){
    qDebug((QString("removeItem: ") + location).latin1());
    QMessageBox::critical(this, tr("Message"), tr("Can't remove."), tr("Ok") );
    return;
  }
  
  // Remove from the installer so it wont fail.
  // Don't need to do this sense the current install uses rm -f so no error
  
  // Remove from the gui list.
  itemList.remove(item);
  if(item->parent())
    item->parent()->takeItem(item);
  delete item;
 
  // We have changed something.
  changed = true;
}

/**
 * Helper function.  Edits the current item.
 * calls editItem with the currently selected item.
 */
void TabManager::editCurrentItem(){
  editItem(tabList->currentItem());
}

/**
 * Edit the item that is passed in.
 * Show application dialog and if anything changed 
 * @param item the item to edit.
 */ 
void TabManager::editItem( QListViewItem * item){
  if(!item)
    return;
  
  TabAppLnk app(itemList[item]);
  if(!app.isValid()){
    qDebug(QString("editItem: Not a valid applnk file: ") + itemList[item].latin1());
    return;	 
  } 
  
  // Fill with all of the icons
  if(!application){
    Wait waitDialog(this, "Wait dialog");
    waitDialog.waitLabel->setText(tr("Gathering icons..."));
    waitDialog.show();
    qApp->processEvents();
    application = new AppEdit(this, "Application edit", true);
	  
    QDir d(QPEApplication::qpeDir() + "/pics/");
    d.setFilter( QDir::Files);

    const QFileInfoList *list = d.entryInfoList();
    QFileInfoListIterator it( *list );      // create list iterator
    QFileInfo *fi;                          // pointer for traversing

    while ( (fi=it.current()) ) {           // for each file...
      QString fileName = fi->fileName();
      if(fileName.right(4) == ".png"){
        fileName = fileName.mid(0,fileName.length()-4);
        QPixmap imageOfFile(Resource::loadPixmap(fileName));
        QImage foo = imageOfFile.convertToImage();
	foo = foo.smoothScale(16,16);
	imageOfFile.convertFromImage(foo);
        application->iconLineEdit->insertItem(imageOfFile,fileName); 
      }
      //qDebug(fi->fileName().latin1());
      ++it;
    }
    waitDialog.hide();
  }
  int pixmap = -1;
  QString pixmapText = app.pixmapString();
  QComboBox *f = application->iconLineEdit;
  for(int i = 0; i < application->iconLineEdit->count(); i++){
    if(f->text(i) == pixmapText){
      pixmap = i;
      break;
    }
  }
  if(pixmap != -1)
    application->iconLineEdit->setCurrentItem(pixmap);
  else if(pixmapText.isEmpty()){
    application->iconLineEdit->setCurrentItem(0);
  }
  else{
    QPixmap imageOfFile(Resource::loadPixmap(pixmapText));
    QImage foo = imageOfFile.convertToImage();
    foo = foo.smoothScale(16,16);
    imageOfFile.convertFromImage(foo);
    application->iconLineEdit->insertItem(imageOfFile,pixmapText,0); 
    application->iconLineEdit->setCurrentItem(0);
  }
  
  application->nameLineEdit->setText(app.name());
  application->execLineEdit->setText(app.exec());
  application->commentLineEdit->setText(app.comment());
 
  if(item->parent() == NULL){
    application->execLineEdit->setEnabled(false);
    application->TextLabel3->setEnabled(false);
    application->setCaption(tr("Tab"));
  }
  else{
    application->execLineEdit->setEnabled(true);
    application->TextLabel3->setEnabled(true);
    application->setCaption(tr("Application"));
  }
  
  // Only do somthing if they hit OK
  application->showMaximized();
  if(application->exec() == 0)
    return;

  // If nothing has changed exit (hmmm why did they hit ok?)
  if(app.name() == application->nameLineEdit->text() &&
     app.pixmapString() == application->iconLineEdit->currentText() &&
     app.comment() == application->commentLineEdit->text() &&
     app.exec() == application->execLineEdit->text())
    return;

  // Change the applnk file
  QString oldName = app.name();
  app.setName(application->nameLineEdit->text());
  app.setIcon(application->iconLineEdit->currentText());
  app.setComment(application->commentLineEdit->text());
  app.setExec(application->execLineEdit->text());
  if(!app.writeLink()){
    QMessageBox::critical(this, tr("Message"), "Can't save.", tr("Ok") );
    return;
  }
  
  // Update the gui icon and name
  item->setText(0,app.name());
  item->setPixmap(0,app.pixmap());
  
  // We have changed something.
  changed = true;
  
  // If we were dealing with a new folder or new application change
  // the file names.  Also change the item location in itemList
  if(oldName == NEW_FOLDER){
    QDir r;
    QString oldName = itemList[item];
    oldName = oldName.mid(0,oldName.length()-11);
    QString newName = oldName.mid(0,oldName.length()-9);
    newName = newName + "/" + app.name();
    r.rename(oldName, newName);
    itemList.remove(item);
    itemList.insert(item, newName + "/.directory" );
  }
  else if(oldName == NEW_APPLICATION){
    if(!item->parent())
      return;
    QString parentDir = itemList[item->parent()];
    QDir r;
    QString oldName = itemList[item];
    QString newName = oldName.mid(0, parentDir.length()-10);
    newName = newName + app.name() + APPLICATION_EXTENSION;
    r.rename(oldName, newName);
    itemList.remove(item);
    itemList.insert(item, newName);
  }
}
 
/**
 * Move an application from one directory to another.
 * Move in the gui, move in the applnk file, move in the installer.
 * @param item the application to move
 * @pearam newGroup the new parent of this application
 */ 
void TabManager::moveApplication(QListViewItem *item, QListViewItem *newGroup){
  // Can we even move it?
  if(!item || !item->parent() || newGroup->parent())
    return;	  
  if(item->parent() == newGroup)
    return;

  // Get the new folder, new file name, 
  QString newFolder = itemList[newGroup];
  newFolder = newFolder.mid(0,newFolder.length()-11);
  int slash = newFolder.findRev('/', -1);
  QString folderName = newFolder.mid(slash+1, newFolder.length());
  
  QString desktopFile = itemList[item];
  slash = desktopFile.findRev('/', -1);
  desktopFile = desktopFile.mid(slash, desktopFile.length());
  newFolder = newFolder + desktopFile;

  // Move file
  QDir r;
  if(!r.rename(itemList[item], newFolder)){
    QMessageBox::critical(this, tr("Message"), "Can't move application.", tr("Ok") );
    return;	  
  }
  //qDebug((QString("moveApplication: ") + itemList[item]).latin1());
  //qDebug((QString("moveApplication: ") + newFolder).latin1());
 
  // Move in the gui
  item->parent()->takeItem(item);
  newGroup->insertItem(item);
  newGroup->setOpen(true);
  
  // Move file in the installer
  QString installedAppFile;
  if(findInstalledApplication(desktopFile, installedAppFile))
    swapInstalledLocation(installedAppFile, desktopFile, newFolder);
  else
    qDebug("moveApplication: No installed app found for dekstop file");  
		  
  // Move application type  
  AppLnk app(newFolder);
  app.setType(folderName);
  app.writeLink();
 
  // Move in our internal list
  itemList.remove(item);
  itemList.insert(item, newFolder);
  
  // We have changed something.
  changed = true;
}

/**
 * File the installed application that has this desktop file.
 * Go through each file in HOME_APP_INSTALL_DIR and see if it contains desktop
 * file
 * @param desktopFile - the .desktop file to search for [foo.desktop]
 * @param installedAppFile - location of the app install list
 * @return true if successfull, false if file not found.
 */ 
bool TabManager::findInstalledApplication(QString desktopFile, QString &installedAppFile){
  
  QDir d;
  d.setPath(HOME_APP_INSTALL_DIR);
  d.setFilter( QDir::Files );

  const QFileInfoList *list = d.entryInfoList();
  QFileInfoListIterator it( *list );      // create list iterator
  QFileInfo *fi;                          // pointer for traversing

  while ( (fi=it.current()) ) { // for each file...
    QFile file(QString(HOME_APP_INSTALL_DIR) + "/" + fi->fileName());
    if ( file.open(IO_ReadOnly) ) { // file opened successfully
      QTextStream stream( &file ); // use a text stream
      QString line;
      while ( !stream.eof() ) { // until end of file...
        line = stream.readLine(); // line of text excluding '\n'
        if(line.contains(desktopFile)){
          installedAppFile = QString(HOME_APP_INSTALL_DIR) + "/" + fi->fileName();
          file.close();
          return true;
        }
      }
      file.close();
    }
    else
      qDebug((QString("findInstalledApplication: Can't open file") + HOME_APP_INSTALL_DIR + "/" + fi->fileName()).latin1());
    ++it;  // goto next list element
  }
  return false;
}

/**
 * Open a file and replace a file containing the old desktop file with the new.
 * @param installedAppFile application installed list
 * @param desktopFile old .desktop file 
 * @param newLocation new .desktop file
 */
void TabManager::swapInstalledLocation( QString installedAppFile, QString desktopFile, QString newLocation ){
  QFile file(installedAppFile);
  if ( !file.open(IO_ReadOnly) ){
    qDebug(QString("swapInstalledLocation: Can't edit file: %1").arg(installedAppFile).latin1());
    return;
  }
  
  QTextStream stream( &file );        // use a text stream
  QString allLines;
  while ( !stream.eof() ) {        // until end of file...
    QString line = stream.readLine();       // line of text excluding '\n'
    if(line.contains(desktopFile))
      allLines += newLocation;
    else
      allLines += line;
    allLines += '\n';
  }
  file.close();

  if ( !file.open(IO_ReadWrite) ){
    qDebug(QString("swapInstalledLocation: Can't edit file: %1").arg(installedAppFile).latin1());
    return;
  }
  QTextStream streamOut( &file );
  streamOut << allLines;
  file.close();
}

// tabmanager.cpp

