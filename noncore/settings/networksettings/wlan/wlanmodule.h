#ifndef WLAN_MODULE_H
#define WLAN_MODULE_H

#include "module.h"

class WLANModule : Module{

signals:
  void updateInterface(Interface *i);
	
public:
  WLANModule();
  ~WLANModule();

  void setProfile(const QString &newProfile);
  bool isOwner(Interface *);
  QWidget *configure(Interface *i);
  QWidget *information(Interface *i);
  QList<Interface> getInterfaces();
  void possibleNewInterfaces(QMap<QString, QString> &){};
  Interface *addNewInterface(const QString &name);
  bool remove(Interface* i);
  QString getPixmapName(Interface* i);

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

