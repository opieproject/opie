#ifndef WINDOW_H 
#define WINDOW_H

#include <qmainwindow.h>
#include "backuprestorebase.h"
#include <qmap.h>
#include <qlist.h>

class QListViewItem;

class BackupAndRestore : public BackupAndRestoreBase { 

Q_OBJECT
	
public:
  BackupAndRestore( QWidget* parent = 0, const char* name = 0);
  ~BackupAndRestore();
  
  
private slots:
  void backupPressed();
  void restore();
  void selectItem(QListViewItem *currentItem);
  void sourceDirChanged(int);
  void rescanFolder(QString directory);

private:
  void scanForApplicationSettings();
  int getBackupFiles(QString &backupFiles, QListViewItem *parent);
  QMap<QString, QString> backupLocations;
  QList<QListViewItem> getAllItems(QListViewItem *item, QList<QListViewItem> &list);  
  
  QListViewItem *systemSettings;
  QListViewItem *applicationSettings;
  QListViewItem *documents;

};

#endif

// backuprestore.h 

