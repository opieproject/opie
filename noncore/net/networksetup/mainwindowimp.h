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
  void getAllInterfaces();
	
  void addClicked();
  void removeClicked();
  void configureClicked();
  void informationClicked();
  
  void addProfile();
  void removeProfile();
  void changeProfile();
  
  void updateInterface(Interface *i);
  void newProfileChanged(const QString& newText);

private:
  void loadModules(const QString &path);
  
  Module* loadPlugin(const QString &pluginFileName,
		     const QString &resolveString = "create_plugin");

  // For our local list of names
  QMap<QString, Interface*> interfaceNames;

  QMap<Module*, QLibrary*> libraries;
  QMap<Interface*, QListViewItem*> items;
  QMap<QListViewItem*, Interface*> interfaceItems;
  
  QMap<KProcess*, QString> threads;
  QStringList profiles;

  bool advancedUserMode;
  QString scheme;
};

#endif

// mainwindowimp.h

