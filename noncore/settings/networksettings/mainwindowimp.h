#ifndef MAINWINOWIMP_H
#define MAINWINOWIMP_H

#include "mainwindow.h"
#include <qmap.h>
#include <qstringlist.h>

class Module;
class Interface;
class QLibrary;
class KProcess;

class MainWindowImp : public MainWindow {
  Q_OBJECT

public:
  MainWindowImp(QWidget *parent=0, const char *name=0);
  ~MainWindowImp();
  
private slots:
  void addClicked();
  void removeClicked();
  void configureClicked();
  void informationClicked();
  
  void jobDone(KProcess *process);
  void getInterfaceList();
 
  void addProfile();
  void removeProfile();
  void changeProfile(const QString&);
  
  void updateInterface(Interface *i);

private:
  void loadModules(QString path);
  
  Module* loadPlugin(QString pluginFileName,
		     QString resolveString = "create_plugin");

  // For our local list of names
  QMap<QString, Interface*> interfaceNames;

  QMap<Module*, QLibrary*> libraries;
  QMap<Interface*, QListViewItem*> items;
  QMap<QListViewItem*, Interface*> interfaceItems;
  
  QMap<KProcess*, QString> threads;
  QStringList profiles;

};

#endif

// mainwindowimp.h

