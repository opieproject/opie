
#include "backuprestore.h"
//#include "output.h"

#include <qapplication.h>
#include <qmultilineedit.h>

#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qheader.h>
#include <qpe/resource.h>
#include <qpe/config.h>
#include <qmessagebox.h>
#include <qcombobox.h>
#include <qlist.h>
#include <stdlib.h>
#include <qregexp.h>

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#define HEADER_NAME 0
#define HEADER_BACKUP 1
#define BACKUP_LOCATION 2

#define EXTENSION ".bck"

BackupAndRestore::BackupAndRestore( QWidget* parent, const char* name)
        : BackupAndRestoreBase(parent, name){
  this->showMaximized();
  backupList->header()->hide();
  restoreList->header()->hide();
  connect(backupButton, SIGNAL(clicked()),
          this, SLOT(backupPressed()));
  connect(restoreButton, SIGNAL(clicked()),
          this, SLOT(restore()));
  connect(backupList, SIGNAL(clicked( QListViewItem * )),
          this, SLOT(selectItem(QListViewItem*))); 
  connect(restoreSource, SIGNAL(activated( int  )),
          this, SLOT(sourceDirChanged(int))); 

  systemSettings = new QListViewItem(backupList, "System Settings", "",
                                     "/etc");
  selectItem(systemSettings);
  applicationSettings = new QListViewItem(backupList, "Application Settings", "",
                                          QDir::homeDirPath() + "/Settings/");
  selectItem(applicationSettings);
  documents= new QListViewItem(backupList, "Documents", "",
                               QDir::homeDirPath() + "/Documents/");
  selectItem(documents);
  
  scanForApplicationSettings();
  
  Config config("BackupAndRestore");
  config.setGroup("General");
  int totalLocations = config.readNumEntry("totalLocations",0);
  
//todo make less static here and use Storage class to get infos
  if(totalLocations == 0){
    backupLocations.insert("Documents", "/root/Documents");
    backupLocations.insert("CF", "/mnt/cf");
    backupLocations.insert("SD", "/mnt/card");
  }
  else{
    for(int i = 0; i < totalLocations; i++){
      backupLocations.insert(config.readEntry(QString("backupLocationName_%1").arg(i)),
                             config.readEntry(QString("backupLocation_%1").arg(i)));
    }
  }
  QMap<QString, QString>::Iterator it;
  for( it = backupLocations.begin(); it != backupLocations.end(); ++it ){
    storeToLocation->insertItem(it.key());
    restoreSource->insertItem(it.key());
  }
 
  // Read the list of items to ignore. 
  QList<QString> dontBackupList;
  dontBackupList.setAutoDelete(true);
  config.setGroup("DontBackup");
  int total = config.readNumEntry("Total", 0);
  for(int i = 0; i < total; i++){
    dontBackupList.append(new QString(config.readEntry(QString("%1").arg(i), "")));
  }

  QList<QListViewItem> list;
  getAllItems(backupList->firstChild(), list);
  
  for(uint i = 0; i < list.count(); i++){
    QString text = list.at(i)->text(HEADER_NAME);
    for(uint i2 = 0;  i2 < dontBackupList.count(); i2++){
      if(*dontBackupList.at(i2) == text){
        selectItem(list.at(i));
        break;
      }
    }
  }
}

BackupAndRestore::~BackupAndRestore(){
  QList<QListViewItem> list;
  getAllItems(backupList->firstChild(), list);
  
  Config config("BackupAndRestore");
  config.setGroup("DontBackup");
  config.clearGroup();
  
  int count = 0;
  for(uint i = 0; i < list.count(); i++){
    if(list.at(i)->text(HEADER_BACKUP) == ""){
      config.writeEntry(QString("%1").arg(count), list.at(i)->text(HEADER_NAME));
      count++;
    }
  }
  config.writeEntry("Total", count);
}

QList<QListViewItem> BackupAndRestore::getAllItems(QListViewItem *item, QList<QListViewItem> &list){
  while(item){
    if(item->childCount() > 0)
      getAllItems(item->firstChild(), list);    
    list.append(item);
    item = item->nextSibling();
  }
  return list;
}

/**
 * Selects and unselects the item by setting the HEADER_BACKUP to B or !.
 * and changing the icon to match
 * @param currentItem the item to swich the selection choice.
 */
void BackupAndRestore::selectItem(QListViewItem *currentItem){
  if(!currentItem)
    return;

  if(currentItem->text(HEADER_BACKUP) == "B"){
    currentItem->setPixmap(HEADER_NAME, Resource::loadPixmap("backup/null"));
    currentItem->setText(HEADER_BACKUP, "");
  }
  else{
    currentItem->setPixmap(HEADER_NAME, Resource::loadPixmap("backup/check"));
    currentItem->setText(HEADER_BACKUP, "B");
  }
}

void BackupAndRestore::scanForApplicationSettings(){
  QDir d(applicationSettings->text(BACKUP_LOCATION));
  d.setFilter( QDir::Files | QDir::NoSymLinks );
  const QFileInfoList *list = d.entryInfoList();
  QFileInfoListIterator it( *list );
  QFileInfo *fi;
  while ( (fi=it.current()) ) {
    //qDebug((d.path()+fi->fileName()).latin1());
    QListViewItem *newItem = new QListViewItem(applicationSettings, fi->fileName());
    selectItem(newItem);
    ++it;
  }
}

/**
 * The "Backup" button has been pressed.  Get a list of all of the files that
 * should be backed up.  If there are no files, emit and error and exit.  
 * Determine the file name to store the backup in.  Backup the file(s) using
 * tar and gzip  --best.  Report failure or success
 */ 
void BackupAndRestore::backupPressed(){
  QString backupFiles;
  if(getBackupFiles(backupFiles, NULL) == 0){
    QMessageBox::critical(this, "Message",
                          "No items selected.",QString("Ok") );
    return;   
  }

  setCaption(tr("Backup and Restore... working..."));
  QString outputFile = backupLocations[storeToLocation->currentText()];

  QDateTime time = QDateTime::currentDateTime();
  QString dateString = time.date().toString().replace(QRegExp(" "), "");
  outputFile += "/" + dateString;
 
  QString t = outputFile;
  int c = 1;
  while(QFile::exists(outputFile + EXTENSION)){
    outputFile = t + QString("%1").arg(c);
    c++;
  }

  qDebug(QString("system(\"tar -c %1 | gzip > %2\")").arg(backupFiles).arg(outputFile).latin1());
  outputFile += EXTENSION;

   int r = system( QString("tar -c %1 | gzip > %2").arg(backupFiles).arg(outputFile).latin1() );



   if(r != 0){
    perror("Error: ");
    QString errorMsg="Error\n"+(QString)strerror(errno);

     QMessageBox::critical(this, "Message", "Backup Failed.\n"+errorMsg, QString("Ok") );
     return;   
   }
   else{
     QMessageBox::critical(this, "Message", "Backup Successfull.",QString("Ok") );
     
   }
   setCaption(tr("Backup and Restore"));
}

/***
 * Get a list of all of the files to backup.
 */ 
int BackupAndRestore::getBackupFiles(QString &backupFiles, QListViewItem *parent){
  QListViewItem * currentItem;
  QString currentHome;
  if(!parent)
    currentItem = backupList->firstChild();
  else{
    currentItem = parent->firstChild();
    currentHome = parent->text(BACKUP_LOCATION);
  }
  
  uint count = 0;
  while( currentItem != 0 ){
    if(currentItem->text(HEADER_BACKUP) == "B" ){
      if(currentItem->childCount() == 0 ){
        if(parent == NULL)
    backupFiles += currentItem->text(BACKUP_LOCATION);
  else
          backupFiles += currentHome + currentItem->text(HEADER_NAME);
  backupFiles += " ";
  count++;
      }
      else{
        count += getBackupFiles(backupFiles, currentItem);
      }
    }
    currentItem = currentItem->nextSibling();
  }
  return count;
}

void BackupAndRestore::sourceDirChanged(int selection){
  restoreList->clear();
  rescanFolder(backupLocations[restoreSource->text(selection)]);
}

/**
 * Scans directory for any backup files.  Will recursivly go down,
 * but will not follow symlinks.
 * @param directory - the directory to look in.
 */
void BackupAndRestore::rescanFolder(QString directory){
  //qDebug(QString("rescanFolder: ") + directory.latin1());
  QDir d(directory);
  if(!d.exists())
    return;
  
  d.setFilter( QDir::Files | QDir::Hidden | QDir::Dirs);
  const QFileInfoList *list = d.entryInfoList();
  QFileInfoListIterator it( *list );
  QFileInfo *file;
  while ( (file=it.current()) ) {  // for each file...
    // If it is a dir and not .. or . then add it as a tab and go down.
    if(file->isDir()){
      if(file->fileName() != ".." && file->fileName() != ".") {
  rescanFolder(directory + "/" + file->fileName());
      }
    }
    else{
      // If it is a backup file add to list.
      if(file->fileName().contains(EXTENSION))
        (void)new QListViewItem(restoreList, file->fileName());
    }
    ++it;
  }
}

/**
 * Restore a backup file.
 * Report errors or success
 */ 
void BackupAndRestore::restore(){
  QListViewItem *restoreItem = restoreList->currentItem();
  if(!restoreItem){
    QMessageBox::critical(this, "Message",
                          "Please select something to restore.",QString("Ok") );
    return;   
  }
  QString restoreFile = backupLocations[restoreSource->currentText()];
  
  restoreFile += "/" + restoreItem->text(0);
  
  int r = system(QString("tar -C / -zxf %1").arg(restoreFile).latin1());
  if(r != 0){
    QMessageBox::critical(this, "Message",
                          "Restore Failed.",QString("Ok") );
  }
  else{
    QMessageBox::critical(this, "Message",
                          "Restore Successfull.",QString("Ok") );
  }
}

// backuprestore.cpp

