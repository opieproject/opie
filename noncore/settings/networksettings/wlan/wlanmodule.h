#ifndef WLAN_MODULE_H
#define WLAN_MODULE_H

#include "module.h"

class WLANModule : Module{

signals:
  void updateInterface(Interface *i);
	
public:
  WLANModule();
  
  virtual void setProfile(QString newProfile);
  virtual bool isOwner(Interface *);
  virtual QWidget *configure(QTabWidget **tabWidget);
  virtual QWidget *information(QTabWidget **tabWidget);
  virtual QList<Interface> getInterfaces();
  virtual void possibleNewInterfaces(QMap<QString, QString> &list){};
  virtual Interface *addNewInterface(QString name);
  virtual bool remove(Interface* i);
  virtual QString getPixmapName(Interface* i);

private:
  QList<Interface> list;
  QString profile;

};

extern "C"
{
  void* create_plugin() {
    return new WLANModule();
  }
};

#endif

// wlanmodule.h

