#ifndef PPP_MODULE_H
#define PPP_MODULE_H

#include "module.h"

class PPPModule : Module {

signals:
  void updateInterface(Interface *i);
	
public:
  PPPModule();
  ~PPPModule();

  virtual void setProfile(const QString &newProfile);
  virtual bool isOwner(Interface *);
  virtual QWidget *configure(Interface *i);
  virtual QWidget *information(Interface *i);
  virtual QList<Interface> getInterfaces();
  virtual void possibleNewInterfaces(QMap<QString, QString> &){};
  virtual Interface *addNewInterface(const QString &name);
  virtual bool remove(Interface* i);
  virtual QString getPixmapName(Interface* i);

private:
  QList<Interface> list;
  QString profile;

};

extern "C"
{
  void* create_plugin() {
    return new PPPModule();
  }
};

#endif

// pppmodule.h

