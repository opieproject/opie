#ifndef INTERFACEPPP_H 
#define INTERFACEPPP_H

#include "interface.h"

class PPPData;
class Modem;

class InterfacePPP : public Interface
{
    Q_OBJECT

public:
    InterfacePPP(QObject *parent=0, const char *name="PPP", bool status=false);

    PPPData* data();
    Modem* modem();

signals:
  void begin_connect();

public slots:
  virtual bool refresh();
  virtual void start();
  virtual void stop();

 private:
  Modem *_modemPtr;
  PPPData *_dataPtr;
};


#endif
