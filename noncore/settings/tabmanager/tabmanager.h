#ifndef TABMANAGER_H
#define TABMANAGER_H

#include "tabmanagerbase.h"
#include <qmap.h>
#include <qevent.h> 

class QListViewItem;
class AppEdit;

class TabManager : public TabManagerBase { 
  Q_OBJECT

public:
  TabManager( QWidget* parent = 0, const char* name = 0);
  ~TabManager();
  bool changed;
  
public slots:
  void newFolder();
  void newApplication();
  void removeItem();
  void editCurrentItem();

private slots:
  void moveApplication(QListViewItem *, QListViewItem *);
  void editItem(QListViewItem * item);

protected:
  void rescanFolder(QString directory,  QListViewItem* parent=NULL);
  QMap<QListViewItem *, QString> itemList;
  bool findInstalledApplication(QString desktopFile, QString &installedAppFile);     
  void swapInstalledLocation( QString installedAppFile, QString desktopFile, QString newLocation );

private:
  AppEdit *application;
  
};

#endif

// tabmanager.h

