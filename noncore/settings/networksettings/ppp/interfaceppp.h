#ifndef INTERFACEPPP_H
#define INTERFACEPPP_H

#include <sys/types.h>

#include "interface.h"

class PPPData;
class Modem;

class InterfacePPP : public Interface
{
    Q_OBJECT

public:
    InterfacePPP(QObject *parent=0, const char *name="PPP", bool status=false);

    PPPData* data()const;
    Modem* modem()const;
    QString pppDev()const; // returns the ppp device.... /dev/ppp0...
    pid_t pppPID()const;

signals:
  void begin_connect();
  void hangup_now();

public slots:
  virtual bool refresh();
  virtual void start();
  virtual void stop();
  void save();
  void setPPPDpid( pid_t ); // sets the pppd pid for modem...

 private:
  mutable Modem *_modemPtr;
  mutable PPPData *_dataPtr;
};


#endif
