#ifndef MODULE_H
#define MODULE_H

#include "module.h"

class QWidget;

class DefaultModule : Module{

public:
  DefaultModule(){};
  
  virtual bool isOwner(Interface *i);
  virtual QWidget *configure();
  virtual QWidget *information()};
  virtual QList<Interface> getInterfaces();
  virtual QMap<QString, QString> possibleNewInterfaces();
  virtual Interface *addNewInterface(QString name);
  virtual bool remove(Interface* i);

};

#endif

// module.h

